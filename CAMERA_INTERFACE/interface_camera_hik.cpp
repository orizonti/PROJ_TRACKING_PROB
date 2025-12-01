#include "interface_camera_hik.h"
#include "interface_image_source.h"
#include <QDebug>
#include "debug_output_filter.h"
#include "register_settings.h"

CameraInterfaceHIK* CameraInterfaceHIK::CameraInterface;
cv::Mat CameraInterfaceHIK::inputImage;
QMutex CameraInterfaceHIK::mutexGetImage;


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
        qDebug() << TAG_NAME.c_str() << QString("[ USER_DEFINED_NAME: %1 ]").arg((char*)pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    return true;
}

void __stdcall CameraInterfaceHIK::ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX* FrameInfo, void* pUser)
{
  inputImage = cv::Mat(FrameInfo->nExtendWidth,FrameInfo->nExtendHeight,CV_8UC1, pData);
  CameraTypeStorage::putNewFrameToStorage(inputImage);
  mutexGetImage.lock();
  CameraTypeStorage::ImageToDisplay = QImage(pData,FrameInfo->nExtendWidth,FrameInfo->nExtendHeight,QImage::Format_Grayscale8);
  mutexGetImage.unlock();
                           CameraInterface->FrameMeasureInput++;
}

std::pair<float,float> CameraInterfaceHIK::getFramePeriod() { return std::pair<float,float>(FrameMeasureInput.FramePeriod, 
                                                                                    FrameMeasureProcess.FramePeriod);};


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

    if (stDeviceList.nDeviceNum > 0)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[0]; if (NULL == pDeviceInfo) return;  
        PrintDeviceInfo(pDeviceInfo);            
        return;
    } 
    
    qDebug() << TAG_NAME.c_str() << "[ FIND NO DEVICES ]"; 

}

void CameraInterfaceHIK::CameraInit()
{
    int nRet = MV_OK;
        nRet = MV_CC_Initialize(); if (MV_OK != nRet) { qDebug() << TAG_NAME.c_str() << "INITIALIZE SDK FAIL" << nRet; return; }

        EnumerateCameras();
        // Select device and create handle
        unsigned int nIndex = 0;
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
                                             if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_CREATE HANDLE FAIL " << nRet; return; }
        nRet = MV_CC_OpenDevice(handle);     if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_OPEND EVICE FAIL "   << nRet; return; }
        
        // Set trigger mode as off
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0); if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_SETTRIGGER MODE FAIL" << nRet; return; }

        nRet = MV_CC_RegisterImageCallBackEx(handle, ImageCallBackEx, handle);
                                             if (MV_OK != nRet) { qDebug()<< TAG_NAME.c_str()  << "MV_CC_REGISTER_IMAGECALLBACKEX FAIL" << nRet; return; }

}

void CameraInterfaceHIK::StartStream()
{
    qDebug()<< TAG_NAME.c_str()  << "[HIK START CAMERA ]";
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

CameraInterfaceHIK::CameraInterfaceHIK(std::string name)
{
qDebug()<< TAG_NAME.c_str()  << "[ HIK CAMERA INTERFACE CREATE FORMAT 720 540 ]";
CameraInterface = this;
CameraInit();
ImageChanneledStore.push_back(std::make_shared<CameraTypeStorage>(this,SizeImage));
CurrentStoreChannel = ImageChanneledStore.begin();

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
qDebug()<< TAG_NAME.c_str()  << "[ SET HEIGHT ]" << Height; if (MV_OK != Result) qDebug() << "[ FAILED ]";  
}

void CameraInterfaceHIK::CameraSetWidth(int Width)
{
int Result = MV_CC_SetIntValueEx(handle, "Width", Width);    
qDebug()<< TAG_NAME.c_str()  << "[ SET WIDTH ]" << Width; if (MV_OK != Result) qDebug() << "[ FAILED ]";  
}

void CameraInterfaceHIK::CameraSetSize(int Width, int Height) 
{ 
    SizeImage.first = Width;
    SizeImage.second = Height;
    //ImageToDisplay = QImage{Width,Height, QImage::Format_Grayscale8};
    CameraSetWidth(Width); CameraSetHeight(Height); 
}

void CameraInterfaceHIK::CameraSetExposure(float Exposure)
{
int Result = MV_CC_SetFloatValue(handle, "ExposureTime", Exposure);
qDebug()<< TAG_NAME.c_str()  << "[ SET EXPOSURE ]" << Exposure; if (MV_OK != Result) qDebug() << "[ FAILED ]";   
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

void CameraInterfaceHIK::SetZoom(int Number)
{
                int size = 120; 
    if(Number == 2) size = 200; 
    if(Number == 3) size = 240; 
    if(Number == 4) size = 360; 
    if(Number == 5) size = 400; 

    int X_ROI = 720/2 - size/2; X_ROI = X_ROI/4; X_ROI = X_ROI*4; 
    int Y_ROI = 540/2 - size/2; Y_ROI = Y_ROI/4; Y_ROI = Y_ROI*4; 

    int RIGHT  = X_ROI + size;
    int BOTTOM = Y_ROI + size;

    SettingsRegister::ResetSettings("CAMERA_IMAGE_POS",  std::pair<float,float>(X_ROI, Y_ROI));
    SettingsRegister::ResetSettings("CAMERA_IMAGE_SIZE", size);
    SettingsRegister::ResetSettings("CAMERA_IMAGE_SIZE", std::pair<float,float>(size,size));
    SettingsRegister::PrintSetting("CAMERA_IMAGE_SIZE");
    SettingsRegister::PrintSetting("CAMERA_IMAGE_POS");

    this->SetCameraRegion(X_ROI,Y_ROI,size,size);
}
