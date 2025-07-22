#include "interface_camera_hik.h"
#include "interface_image_source.h"
#include <QDebug>
#include "debug_output_filter.h"
OutputFilter DebugFilter{200};
OutputFilter DebugFilter2{200};

CameraInterfaceHIK* CameraInterfaceHIK::CameraInterface;

std::vector<uint8_t*> CameraInterfaceHIK::CameraImageStorage::Buffers;
std::vector<uint8_t*>::iterator CameraInterfaceHIK::CameraImageStorage::BufferToWrite;
QImage CameraInterfaceHIK::CameraImageStorage::ImageToDisplay;
std::pair<int,int> CameraInterfaceHIK::CameraImageStorage::SizeImage;
long int CameraInterfaceHIK::CameraImageStorage::FrameNumber;

bool CameraInterfaceHIK::PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        qDebug() << "[ HIK CAMERAS NOT FOUND ]";
        return false;
    }

    if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        qDebug() << QString("[ DEVICE MODEL NAME: %1 ]").arg((char*)pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        qDebug() << QString("[ USER_DEFINED_NAME: %1 ]").arg((char*)pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    return true;
}

void __stdcall CameraInterfaceHIK::ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX* FrameInfo, void* pUser)
{
  CameraInterfaceHIK::CameraImageStorage::PutNewFrameToStorage(pData, FrameInfo->nFrameLenEx,FrameInfo->nExtendWidth,FrameInfo->nExtendHeight);
                           CameraInterface->FrameMeasureInput++;
  emit CameraInterfaceHIK::CameraInterface->SignalNewImage();
}

std::pair<float,float> CameraInterfaceHIK::GetFramePeriod() { return std::pair<float,float>(FrameMeasureInput.FramePeriod, 
                                                                                    FrameMeasureProcess.FramePeriod);};


std::shared_ptr<ImageSourceInterface> CameraInterfaceHIK::GetImageSourceChannel()
{
  auto ImageSourceChannel = ImageChanneledStore.back();
                            ImageChanneledStore.push_back(std::make_shared<CameraImageStorage>(this));
      CurrentStoreChannel = ImageChanneledStore.begin();

return ImageSourceChannel;
}

void CameraInterfaceHIK::EnumerateCameras()
{
    int nRet = MV_OK;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE, &stDeviceList);
                                if (MV_OK != nRet) { qDebug() << "MV_CC_ENUMDEVICES FAIL!" << nRet; return; }

    if (stDeviceList.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            qDebug() << "[DEVICE]:" << i;
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i]; if (NULL == pDeviceInfo) return;  
            PrintDeviceInfo(pDeviceInfo);            
        }  
        return;
    } 
    
    qDebug() << "[ FIND NO DEVICES ]"; 

}

void CameraInterfaceHIK::CameraInit()
{
    int nRet = MV_OK;
        nRet = MV_CC_Initialize(); if (MV_OK != nRet) { qDebug() << "INITIALIZE SDK FAIL" << nRet; return; }

        EnumerateCameras();
        // Select device and create handle
        unsigned int nIndex = 0;
        nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
                                             if (MV_OK != nRet) { qDebug() << "MV_CC_CREATE HANDLE FAIL " << nRet; return; }
        nRet = MV_CC_OpenDevice(handle);     if (MV_OK != nRet) { qDebug() << "MV_CC_OPEND EVICE FAIL "   << nRet; return; }
        
        // Set trigger mode as off
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0); if (MV_OK != nRet) { qDebug() << "MV_CC_SETTRIGGER MODE FAIL" << nRet; return; }

        nRet = MV_CC_RegisterImageCallBackEx(handle, ImageCallBackEx, handle);
                                             if (MV_OK != nRet) { qDebug() << "MV_CC_REGISTER_IMAGECALLBACKEX FAIL" << nRet; return; }

}

void CameraInterfaceHIK::StartStream()
{
    qDebug() << "[ START CAMERA ]";
    int nRet = MV_OK;
        nRet = MV_CC_StartGrabbing(handle);  if (MV_OK != nRet) { qDebug() << "MV_CC_STARTG_RABBING FAIL" << nRet; return; }
}

void CameraInterfaceHIK::StopStream()
{
    qDebug() << "[ STOP CAMERA ]";
    int nRet = MV_OK;
        nRet = MV_CC_StopGrabbing(handle);  if (MV_OK != nRet) { qDebug() << "MV_CC_STOP_GRABBING FAIL" << nRet; return; }

}

void CameraInterfaceHIK::DeinitCamera()
{
int nRet = MV_OK;
    nRet = MV_CC_StopGrabbing(handle);   if (MV_OK != nRet) { qDebug() << "MV_CC_STOP_GRABBING FAIL" << nRet;  return; }
    nRet = MV_CC_CloseDevice(handle);    if (MV_OK != nRet) { qDebug() << "MV_CC_CLOSE_DEVICE FAIL" << nRet;   return; }
    nRet = MV_CC_DestroyHandle(handle);  if (MV_OK != nRet) { qDebug() << "MV_CC_DESTROY_HANDLE FAIL" << nRet; return; } 
    handle = NULL;

MV_CC_Finalize();
}

CameraInterfaceHIK::CameraInterfaceHIK()
{
qDebug() << "[ HIK CAMERA INTERFACE CREATE FORMAT 720 540 ]";
CameraInterface = this;
CameraInit();
ImageChanneledStore.push_back(std::make_shared<CameraImageStorage>(this));
CurrentStoreChannel = ImageChanneledStore.begin();

}

CameraInterfaceHIK::~CameraInterfaceHIK()
{
qDebug() << "[ HIK CAMERA INTERFACE DEINIT]"; DeinitCamera();
}

void CameraInterfaceHIK::CameraSetOffset(int XOffset, int YOffset)
{
int Result = MV_CC_SetIntValueEx(handle, "OffsetX", XOffset);    
    Result = MV_CC_SetIntValueEx(handle, "OffsetY", YOffset);    

qDebug() << "[ SET OFFSET ]" << XOffset << YOffset; if (MV_OK != Result) qDebug() << "[ FAILED ]";  
}


void CameraInterfaceHIK::CameraSetHeight(int Height)
{
int Result = MV_CC_SetIntValueEx(handle, "Height", Height);    
qDebug() << "[ SET HEIGHT ]" << Height; if (MV_OK != Result) qDebug() << "[ FAILED ]";  
}

void CameraInterfaceHIK::CameraSetWidth(int Width)
{
int Result = MV_CC_SetIntValueEx(handle, "Width", Width);    
qDebug() << "[ SET WIDTH ]" << Width; if (MV_OK != Result) qDebug() << "[ FAILED ]";  
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
qDebug() << "[ SET EXPOSURE ]" << Exposure; if (MV_OK != Result) qDebug() << "[ FAILED ]";   
}

void CameraInterfaceHIK::CameraSetTriggerMode(int Mode)
{
unsigned int nTriggerMode = 0;
int Result = MV_CC_SetEnumValue(handle, "TriggerMode", nTriggerMode);

if (MV_OK == Result) qDebug() << "[ SET TRIGGERMODE OK! ]"; else qDebug() << "[ SET TRIGGERMODE FAILED! ]", Result;
}

void CameraInterfaceHIK::CameraSetHorisontalReverse(bool OnOff)
{
    
int Result = MV_CC_SetBoolValue(handle, "ReverseX", OnOff);

if (MV_OK == Result) qDebug() << "[ SET REVERSEX OK! ]"; else qDebug() << "[ SET REVERSEX FAILED! ]", Result;
}

void CameraInterfaceHIK::CameraSetCameraID(QString str)
{

int Result = MV_CC_SetStringValue(handle, "DeviceUserID", str.toStdString().c_str());

if (MV_OK == Result) qDebug() << "[ SET DEVICEUSERID OK! ]"; else qDebug() << "[ SET DEVICEUSERID FAILED! ]", Result;
}


void CameraInterfaceHIK::CameraGetTriggerMode()
{

MVCC_ENUMVALUE stTriggerMode = {0};
int Result = MV_CC_GetEnumValue(handle, "TriggerMode", &stTriggerMode);

if (MV_OK != Result) { qDebug() << "[ GET TRIGGERMODE FAILED! ]" << Result; return; } 

qDebug() << "[ TRIGGERMODE CURRENT ]" << stTriggerMode.nCurValue << "[ SUPPORTED TRIGGER_MODE ]" << stTriggerMode.nSupportedNum;
for (unsigned int i = 0; i < stTriggerMode.nSupportedNum; ++i) qDebug() << "[ SUPPORTED TRIGGERMODE ]" << i << stTriggerMode.nSupportValue[i];
}

//===================================================
void CameraInterfaceHIK::CameraGetID()
{
MVCC_STRINGVALUE stStringValue = {0};
int Result = MV_CC_GetStringValue(handle, "DeviceUserID", &stStringValue);
if (MV_OK == Result) qDebug() << "[ GET DEVICE_USER_ID  ]", stStringValue.chCurValue; else qDebug() << "[ GET DEVICE_USER_ID FAILED ]", Result;
}

//===================================================
bool CameraInterfaceHIK::isReverseHorizontal()
{
bool bGetBoolValue = false;
int Result = MV_CC_GetBoolValue(handle, "ReverseX", &bGetBoolValue);

    if(!Result) { qDebug() << "[ GET REVERSEX FAILED! ]", Result; return false; } 

if (0 != bGetBoolValue) return true; else return false;

}

//===================================================
void CameraInterfaceHIK::CameraGetImageSize()
{

MVCC_INTVALUE height = {0}; MVCC_INTVALUE width = {0};

int Result = MV_CC_GetIntValue(handle, "Height", &height);
    Result = MV_CC_GetIntValue(handle, "Width", &width);

if (MV_OK == Result) qDebug() << "[ HEIGHT] [ CURRENT ]"<< height.nCurValue << "[ MAX ]" << height.nMax << "[ MIN ]"<< height.nMin << "[ INCREMENT ]"<< height.nInc;
if (MV_OK == Result) qDebug() << "[ WIDTH] [ CURRENT ]"<< width.nCurValue << "[ MAX ]" << width.nMax << "[ MIN ]"<< width.nMin << "[ INCREMENT ]"<< width.nInc;

qDebug() << "[ GET SIZE FAILED ]", Result;
}

//===================================================
void CameraInterfaceHIK::CameraGetExposure()
{
MVCC_FLOATVALUE stExposureTime = {0};
int Result = MV_CC_GetFloatValue(handle, "ExposureTime", &stExposureTime);
if (MV_OK == Result) { qDebug() << "[ EXPOSURE]  [CURRENT ]"<< stExposureTime.fCurValue <<  "[ MAX ]"<< stExposureTime.fMax << "[ MIN ]"<< stExposureTime.fMin; }
else                   qDebug() << "[ GET EXPOSURE FAILED ]"<< Result;
}
//==========================================================================================================

void CameraInterfaceHIK::CameraImageStorage::InitStorage()
{
if(Buffers.empty())
{

for (int i = 0; i < 20; i++) Buffers.push_back(new uint8_t[720*540]); //USER STORAGE BUFFERS TO PROCESSS
             BufferToWrite = Buffers.begin();
}
             BufferToRead = Buffers.begin();
             qDebug() << "INIT STORAGE FRAME AVAILABLE: " << GetAvailableFrames();
}

void CameraInterfaceHIK::CameraImageStorage::DeinitStorage() { if(Buffers.size()) for(auto Buffer: Buffers) delete Buffer; }


void CameraInterfaceHIK::CameraImageStorage::PutNewFrameToStorage(uint8_t* Data, int Size, int Width, int Height)
{
  std::copy_n(Data,Size,*BufferToWrite);
  BufferToWrite++; if(BufferToWrite == Buffers.end()) BufferToWrite = Buffers.begin();

  SizeImage.first = Width; SizeImage.second = Height;
  ImageToDisplay = QImage(Data,Width,Height,QImage::Format_Grayscale8);
  FrameNumber++;
}

void CameraInterfaceHIK::CameraImageStorage::SkipFrames()
{
	qDebug() << "[ SKIP FRAMES ]" << BufferToWrite - BufferToRead;
    FrameNumber = 0; FrameProcessed = 0;
    BufferToRead = BufferToWrite;
}

QImage& CameraInterfaceHIK::CameraImageStorage::GetImageToDisplay() { return ImageToDisplay; }
void CameraInterfaceHIK::CameraImageStorage::GetImageToDisplay(QImage& ImageDst){ ImageDst = ImageToDisplay.copy();};

cv::Mat& CameraInterfaceHIK::CameraImageStorage::GetImageToProcess()                  
{
    //FrameProcessed++; qDebug() << DebugFilter2 << "[ FRAME PASSED ]" << FrameNumber << "[ PROCESSED ]" << FrameProcessed;
    SwitchToNextFrame(); return ImageToProcess; 
}

void   CameraInterfaceHIK::CameraImageStorage::GetImageToProcess(cv::Mat& ImageDst) 
{

    SwitchToNextFrame(); ImageDst = ImageToProcess.clone(); 
    FrameProcessed++; qDebug() << DebugFilter2 << "[ FRAME PASSED ]" << FrameNumber << "[ PROCESSED ]" << FrameProcessed;
};

bool CameraInterfaceHIK::CameraImageStorage::SwitchToNextFrame() 
{
   if(BufferToRead == BufferToWrite) return false;
   
   ImageToProcess = cv::Mat(SizeImage.second,SizeImage.first,CV_8UC1,*BufferToRead); 
   BufferToRead++; if(BufferToRead == Buffers.end()) BufferToRead = Buffers.begin(); 
   
   return true;
}