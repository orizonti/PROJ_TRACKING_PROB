#ifndef WIDGET_CAMERA_INTERFACE_HIK_H
#define WIDGET_CAMERA_INTERFACE_HIK_H
#include <QObject>
#include <QImage>
#include "MvCameraControl.h"
#include <opencv2/opencv.hpp>
#include "interface_image_source.h"
#include <QDebug>
#include "camera_control_interface.h"
#include "interface_image_source.h"
#include "camera_image_storage.h"
#include "module_period_measure.h"
#include "device_generic_interface.h"

class CameraInterfaceHIK;
using CameraTypeStorage = CameraImageStorage<CameraInterfaceHIK>;
class CameraInterfaceHIK :public QObject, 
                          public SourceImageInterface, 
                          public SourceImageDisplayInterface, 
                          public CameraControlInterface,
                          public DeviceGenericHandleControl
{
  Q_OBJECT
  public:

  public:
   CameraInterfaceHIK(std::string name, uint32_t Device = 0);
  ~CameraInterfaceHIK();

  public:
  QString CAMERA_INFO{"[ CAMERA NO DATA ]"};
  std::string TAG_NAME{"[ CAMERA ]"};

  MeasurePeriodNode FrameMeasureInput;
  MeasurePeriodNode FrameMeasureProcess;

  QString getName() override { return "[CAMERA HIK]"; };
  std::shared_ptr<SourceImageInterface> getImageSourceChannel() override;
  static cv::Mat inputImage;
  static  QMutex mutexGetImage;

  //======================================================
  //======================================================
  public:
  void StartStream();
  void StopStream();

  void CameraSetRegion(int x, int y, int width, int height ) override {CameraSetOffset(x,y); CameraSetSize(width,height);};
  void CameraSetSize    (int Width, int Height)    override;
  void CameraSetOffset  (int XOffset, int YOffset) override;
  void CameraSetHeight  (int Height)               override;
  void CameraSetWidth   (int Width)                override;

  void CameraSetZoom    (int Number)     override;
  void CameraSetExposure(float Exposure) override;
  void CameraSetGain    (float Gain)     override;

  void CameraStartStream(bool OnOff)     override { if(OnOff) StartStream(); else StopStream(); } ;
  

  cv::Mat& getImageToProcess() override {return (*CurrentStoreChannel)->getImageToProcess(); SwitchOutputChannel();};

      void getImageToDisplay(QImage& ImageDst) override { mutexGetImage.lock(); ImageDst = CameraTypeStorage::ImageToDisplay.copy(); mutexGetImage.unlock(); };
      void getImageToProcess(cv::Mat& ImageDst)override { (*CurrentStoreChannel)->getImageToProcess(ImageDst); SwitchOutputChannel();};
      void SwitchOutputChannel() { CurrentStoreChannel++; if(CurrentStoreChannel == ImageChanneledStore.end()) CurrentStoreChannel = ImageChanneledStore.begin();}

      bool isFrameAvailable() override { return (*CurrentStoreChannel)->isFrameAvailable();};
      bool empty()            override { return (*CurrentStoreChannel)->empty();};

      int getAvailableFrames() override;

    const std::vector<QPair<int,int>>& getPoints() override { return CameraPoints;};  
    const std::vector<QRect>& getRects() override { return CameraRects;};  
    const            QString& getInfo() override { return CAMERA_INFO;};  
           std::pair<float,float> getTickPeriod() override ;

    std::pair<int,int> getSizeImage() override { return SizeImage;};
  //=========================================
          void CameraSetTriggerMode(int Mode);
          void CameraSetHorisontalReverse(bool OnOff);
          void CameraSetCameraID(QString str);
          void CameraGetTriggerMode();
          void CameraGetID();
          void CameragetSizeImage();
          void CameraGetExposure();
          bool isReverseHorizontal();
  //=========================================

	void setParam (uint16_t CommandID, float    CommandParam) override {};

  signals:
  void signalNewImage(const cv::Mat&);
  void signalNewImage(const QImage&);
  void signalNewImage();

  private:
  MV_CC_DEVICE_INFO_LIST stDeviceList; 
  void* handle = NULL;
  uint32_t CountDevice = 0;
  uint32_t DeviceNum = 0;

  std::pair<int,int> ImagePos{20,20}; 
  std::pair<int,int> SizeImage{720,540}; 
  std::vector<QPair<int,int>> CameraPoints{2};
  std::vector<QRect>          CameraRects{2};

  static void __stdcall ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX* FrameInfo, void* pUser);
  static CameraInterfaceHIK* CameraInterface;

           std::vector<std::shared_ptr<CameraTypeStorage>> ImageChanneledStore;
  typename std::vector<std::shared_ptr<CameraTypeStorage>>::iterator CurrentStoreChannel;

  //=====================================================

  bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);
  void EnumerateCameras();
  void CameraInit();
  void DeinitCamera();
};

#endif 


