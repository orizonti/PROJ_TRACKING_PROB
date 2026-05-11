#include "interface_camera_hik.h"
#include "interface_image_source.h"
#include <QDebug>
#include "debug_output_filter.h"
#include "register_settings.h"

CameraInterfaceHIK* CameraInterfaceHIK::CameraInterface;
cv::Mat CameraInterfaceHIK::inputImage;
std::mutex CameraInterfaceHIK::mutexGetImage;


bool CameraInterfaceHIK::PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        qDebug() << TAG_NAME.c_str() << "[ HIK CAMERAS NOT FOUND ]";
        return false;
    }

    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        qDebug() << TAG_NAME.c_str() << QString("[ DEVICE MODEL NAME: %1 ]").arg((char*)pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        qDebug() << TAG_NAME.c_str() << QString("[ IP: %1 ]").arg(pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp);
    }
    return true;
}

int CameraInterfaceHIK::getAvailableFrames() { return 0; };
//int CameraInterfaceHIK::getAvailableFrames() { return CurrentStoreChannel->getAvailableFrames(); };

void __stdcall CameraInterfaceHIK::ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX* FrameInfo, void* pUser)
{
  inputImage = cv::Mat(FrameInfo->nExtendHeight,FrameInfo->nExtendWidth,CV_8UC1, pData);
  CameraTypeStorage::putNewFrameToStorage(inputImage);
                           CameraInterface->FrameMeasureInput++;
}

std::pair<float,float> CameraInterfaceHIK::getTickPeriod() { return std::pair<float,float>(FrameMeasureInput.TickPeriod, 
                                                                                    FrameMeasureProcess.TickPeriod);};


std::shared_ptr<SourceImageInterface> CameraInterfaceHIK::getImageSourceChannel()
{
  auto ImageSourceChannel = ImageChanneledStore.back();
                            ImageChanneledStore.push_back(std::make_shared<CameraTypeStorage>(this,SizeImage));
      CurrentStoreChannel = ImageChanneledStore.begin();

return ImageSourceChannel;
}

void CameraInterfaceHIK::EnumerateCameras()
{
    int nRet = MV_OK;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE, &stDeviceList);
                                if (MV_OK != nRet) { qDebug() << TAG_NAME.c_str() << "MV_CC_ENUMDEVICES FAIL!" << nRet; return; }

    if(stDeviceList.nDeviceNum == 0) { qDebug() << TAG_NAME.c_str() << "[ NO DEVICE FOUND ]";  return; }

    CountDevice = stDeviceList.nDeviceNum-1;
    qDebug() << "==============================";
    qDebug() << "[ FOUND CAMERAS]";
    for(int n =0; n < stDeviceList.nDeviceNum; n++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[n]; if (NULL == pDeviceInfo) return;  
        PrintDeviceInfo(pDeviceInfo);            
    }

}

void CameraInterfaceHIK::CameraInit()
{
    int nRet = MV_OK;
        nRet = MV_CC_Initialize(); if (MV_OK != nRet) { qDebug() << TAG_NAME.c_str() << "INITIALIZE SDK FAIL" << nRet; return; }

        EnumerateCameras();
        if(DeviceNum > CountDevice){ qDebug() << TAG_NAME.c_str() << "DEVICE INDEX EXCEED DEVICE COUNT" << DeviceNum; return; }
        DeviceNum = 0;

        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[DeviceNum]);
                                             if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_CREATE HANDLE FAIL " << nRet; return; }
        nRet = MV_CC_OpenDevice(handle);     if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_OPEND EVICE FAIL "   << nRet; return; }
        
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0); if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_SETTRIGGER MODE FAIL" << nRet; return; }

        nRet = MV_CC_RegisterImageCallBackEx(handle, ImageCallBackEx, handle);
                                             if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_REGISTER_IMAGECALLBACKEX FAIL" << nRet; return; }

}

void CameraInterfaceHIK::StartStream()
{
    qDebug()<< TAG_NAME.c_str()  << "[ START STREAM ]";
    int nRet = MV_OK;
        nRet = MV_CC_StartGrabbing(handle);  if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_STARTG_RABBING FAIL" << nRet; return; }
}

void CameraInterfaceHIK::StopStream()
{
    qDebug() << "[HIK STOP CAMERA ]";
    int nRet = MV_OK;
        nRet = MV_CC_StopGrabbing(handle);  if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_STOP_GRABBING FAIL" << nRet; return; }

}

void CameraInterfaceHIK::DeinitCamera()
{
int nRet = MV_OK;
    nRet = MV_CC_StopGrabbing(handle);   if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_STOP_GRABBING FAIL" << nRet;  return; }
    nRet = MV_CC_CloseDevice(handle);    if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_CLOSE_DEVICE FAIL" << nRet;   return; }
    nRet = MV_CC_DestroyHandle(handle);  if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_DESTROY_HANDLE FAIL" << nRet; return; } 
    handle = NULL;

MV_CC_Finalize();
}

CameraInterfaceHIK::CameraInterfaceHIK(std::string name, uint32_t Device): DeviceNum(Device)
{

     SizeCamera = SettingsRegister::GetPair("CAMERA_RESOLUTION"); 
     SizeImage = SettingsRegister::GetPair("CAMERA_SIZE_ACTIVE"); 

     SizeImage.first  /= 4; SizeImage.first  *= 4;
     SizeImage.second /= 4; SizeImage.second *= 4;
     SettingsRegister::ResetSettings("CAMERA_SIZE_ACTIVE",SizeImage);


CameraInterface = this;
CameraInit();
ImageChanneledStore.push_back(std::make_shared<CameraTypeStorage>(this,SizeImage));
CurrentStoreChannel = ImageChanneledStore.begin();

auto X_ROI = (SizeCamera.first-SizeImage.first )/2; X_ROI = X_ROI/4; PosImage.first  = X_ROI*4; 
auto Y_ROI = (SizeCamera.first-SizeImage.second)/2; Y_ROI = Y_ROI/4; PosImage.second = Y_ROI*4; 

                         qDebug() << "==============================";
                         qDebug() << TAG_NAME.c_str() << "[ SIZE IMAGE  ]" << SizeImage.first << SizeImage.second;
                         qDebug() << TAG_NAME.c_str() << "[ SIZE CAMERA ]" << SizeCamera.first << SizeCamera.second;

    CameraSetRegion(PosImage.first,PosImage.second,SizeImage.first,SizeImage.second);
  CameraSetExposure(SettingsRegister::GetValue("CAMERA_EXPOSURE"));
      CameraSetGain(SettingsRegister::GetValue("CAMERA_GAIN"));

                         qDebug() << "==============================";
}

CameraInterfaceHIK::~CameraInterfaceHIK()
{
qDebug()<< TAG_NAME.c_str()  << "[ HIK CAMERA INTERFACE DEINIT]"; DeinitCamera();
}

void CameraInterfaceHIK::CameraSetOffset(int XOffset, int YOffset)
{
int Result = MV_CC_SetIntValueEx(handle, "OffsetX", XOffset);    
    Result = MV_CC_SetIntValueEx(handle, "OffsetY", YOffset);    

qDebug()<< TAG_NAME.c_str()  << "[ SET OFFSET ]" << XOffset << YOffset; if (MV_OK != Result) qDebug() << "[ FAILED ]";  
}


void CameraInterfaceHIK::CameraSetHeight(int Height)
{
int Result = MV_CC_SetIntValueEx(handle, "Height", Height);    
if (MV_OK != Result) qDebug() <<TAG_NAME.c_str() << "[ FAILED SET HEIGH]";  
}

void CameraInterfaceHIK::CameraSetWidth(int Width)
{
int Result = MV_CC_SetIntValueEx(handle, "Width", Width);    
if (MV_OK != Result) qDebug() <<TAG_NAME.c_str() << "[ FAILED SET WIDTH]";  
}

void CameraInterfaceHIK::CameraSetSize(int Width, int Height) 
{ 
    SizeImage.first = Width;
    SizeImage.second = Height;
    CameraSetWidth(Width);  
    CameraSetHeight(Height);  
}

void CameraInterfaceHIK::CameraSetExposure(float Exposure)
{
int Result = MV_CC_SetFloatValue(handle, "ExposureTime", Exposure);
qDebug()<< TAG_NAME.c_str()  << "[ SET EXPOSURE ]" << Exposure; if (MV_OK != Result) qDebug() << "[ FAILED ]";   
}
void CameraInterfaceHIK::CameraSetGain(float Gain)
{
int Result = MV_CC_SetFloatValue(handle, "Gain", Gain);
qDebug()<< TAG_NAME.c_str()  << "[ SET GAIN ]" << Gain; if (MV_OK != Result) qDebug() << "[ FAILED ]";   
}

void CameraInterfaceHIK::CameraSetTriggerMode(int Mode)
{
unsigned int nTriggerMode = 0;
int Result = MV_CC_SetEnumValue(handle, "TriggerMode", nTriggerMode);

if (MV_OK == Result) qDebug()<< TAG_NAME.c_str()  << "[ SET TRIGGERMODE OK! ]"; else qDebug() << "[ SET TRIGGERMODE FAILED! ]", Result;
}

void CameraInterfaceHIK::CameraSetHorisontalReverse(bool OnOff)
{
    
int Result = MV_CC_SetBoolValue(handle, "ReverseX", OnOff);

if (MV_OK == Result) qDebug()<< TAG_NAME.c_str()  << "[ SET REVERSEX OK! ]"; else qDebug() << "[ SET REVERSEX FAILED! ]", Result;
}

void CameraInterfaceHIK::CameraSetCameraID(QString str)
{

int Result = MV_CC_SetStringValue(handle, "DeviceUserID", str.toStdString().c_str());

if (MV_OK == Result) qDebug()<< TAG_NAME.c_str()  << "[ SET DEVICEUSERID OK! ]"; else qDebug() << "[ SET DEVICEUSERID FAILED! ]", Result;
}


void CameraInterfaceHIK::CameraGetTriggerMode()
{

MVCC_ENUMVALUE stTriggerMode = {0};
int Result = MV_CC_GetEnumValue(handle, "TriggerMode", &stTriggerMode);

if (MV_OK != Result) { qDebug()<< TAG_NAME.c_str()  << "[ GET TRIGGERMODE FAILED! ]" << Result; return; } 

qDebug()<< TAG_NAME.c_str()  << "[ TRIGGERMODE CURRENT ]" << stTriggerMode.nCurValue << "[ SUPPORTED TRIGGER_MODE ]" << stTriggerMode.nSupportedNum;
for (unsigned int i = 0; i < stTriggerMode.nSupportedNum; ++i) qDebug()<< TAG_NAME.c_str()  << "[ SUPPORTED TRIGGERMODE ]" << i << stTriggerMode.nSupportValue[i];
}

//===================================================
void CameraInterfaceHIK::CameraGetID()
{
MVCC_STRINGVALUE stStringValue = {0};
int Result = MV_CC_GetStringValue(handle, "DeviceUserID", &stStringValue);
if (MV_OK == Result) qDebug()<< TAG_NAME.c_str()  << "[ GET DEVICE_USER_ID  ]", stStringValue.chCurValue; 
                else qDebug()<< TAG_NAME.c_str()  << "[ GET DEVICE_USER_ID FAILED ]", Result;
}

//===================================================
bool CameraInterfaceHIK::isReverseHorizontal()
{
bool bGetBoolValue = false;
int Result = MV_CC_GetBoolValue(handle, "ReverseX", &bGetBoolValue);

    if(!Result) { qDebug() << TAG_NAME.c_str() << "[ GET REVERSEX FAILED! ]", Result; return false; } 

if (0 != bGetBoolValue) return true; else return false;

}

//===================================================
void CameraInterfaceHIK::CameragetSizeImage()
{

MVCC_INTVALUE height = {0}; MVCC_INTVALUE width = {0};

int Result = MV_CC_GetIntValue(handle, "Height", &height);
    Result = MV_CC_GetIntValue(handle, "Width", &width);

if (MV_OK == Result) qDebug()<< TAG_NAME.c_str()  << "[ HEIGHT] [ CURRENT ]"<< height.nCurValue << "[ MAX ]" << height.nMax << "[ MIN ]"<< height.nMin << "[ INCREMENT ]"<< height.nInc;
if (MV_OK == Result) qDebug()<< TAG_NAME.c_str()  << "[ WIDTH] [ CURRENT ]"<< width.nCurValue << "[ MAX ]" << width.nMax << "[ MIN ]"<< width.nMin << "[ INCREMENT ]"<< width.nInc;

qDebug()<< TAG_NAME.c_str()  << "[ GET SIZE FAILED ]", Result;
}

//===================================================
void CameraInterfaceHIK::CameraGetExposure()
{
MVCC_FLOATVALUE stExposureTime = {0};
int Result = MV_CC_GetFloatValue(handle, "ExposureTime", &stExposureTime);
if (MV_OK == Result) { qDebug()<< TAG_NAME.c_str()  << "[ EXPOSURE]  [CURRENT ]"<< stExposureTime.fCurValue <<  "[ MAX ]"<< stExposureTime.fMax << "[ MIN ]"<< stExposureTime.fMin; }
else                   qDebug()<< TAG_NAME.c_str()  << "[ GET EXPOSURE FAILED ]"<< Result;
}
//==========================================================================================================

void CameraInterfaceHIK::CameraSetZoom(int Number)
{
                    SizeImage = SettingsRegister::GetPair("CAMERA_SIZE_1"); 
    if(Number == 2) SizeImage = SettingsRegister::GetPair("CAMERA_SIZE_2"); 
    if(Number == 3) SizeImage = SettingsRegister::GetPair("CAMERA_SIZE_3"); 
    if(Number == 4) SizeImage = SettingsRegister::GetPair("CAMERA_SIZE_4"); 
    if(Number == 5) SizeImage = SettingsRegister::GetPair("CAMERA_SIZE_5"); 

    SizeImage.first  /= 4; SizeImage.first  *= 4;
    SizeImage.second /= 4; SizeImage.second *= 4;

    auto X_ROI = (SizeCamera.first-SizeImage.first )/2; X_ROI = X_ROI/4;  PosImage.first  = X_ROI*4; 
    auto Y_ROI = (SizeCamera.first-SizeImage.second)/2 ; Y_ROI = Y_ROI/4; PosImage.second = Y_ROI*4; 

    this->CameraSetRegion(PosImage.first,PosImage.second,SizeImage.first,SizeImage.second);
}

void CameraInterfaceHIK::CameraSetRegion(int x, int y, int width, int height ) 
{
  SettingsRegister::ResetSettings("CAMERA_SIZE_ACTIVE",std::make_pair(width,height));
  CameraSetOffset(x,y); 
  CameraSetSize(width,height);
};
