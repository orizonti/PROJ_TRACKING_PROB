#ifndef WIDGET_CAMERA_INTERFACE_HIK_H
#define WIDGET_CAMERA_INTERFACE_HIK_H
#include <QObject>
#include <QImage>
#include "MvCameraControl.h"
#include <opencv2/opencv.hpp>
#include "interface_image_source.h"
#include <QDebug>
#include "camera_control_interface.h"

class CameraInterfaceHIK : public ImageSourceInterface, public CameraControlInterface

{
  Q_OBJECT
  public:
  class CameraImageStorage: public ImageSourceInterface
  {
    public:
    explicit CameraImageStorage(CameraInterfaceHIK* CameraDevice) : Camera(CameraDevice) {InitStorage(); };
            ~CameraImageStorage() {DeinitStorage(); };
    void InitStorage();
    void DeinitStorage();
    static std::vector<uint8_t*> Buffers;
    static std::vector<uint8_t*>::iterator BufferToWrite;
    static QImage ImageToDisplay;
    static std::pair<int,int> SizeImage; 

    static void PutNewFrameToStorage(uint8_t* Data, int Size, int Width, int Height);
    static long int FrameNumber;
           long int FrameProcessed = 0;

    std::vector<uint8_t*>::iterator BufferToRead;

    int GetAvailableFrames() override {return BufferToWrite - BufferToRead;}
    bool isFrameAvailable() override { return BufferToWrite != BufferToRead;};

    CameraInterfaceHIK* Camera = nullptr;

    bool SwitchToNextFrame() override;
    void SkipFrames() override;

    cv::Mat ImageToProcess;

  cv::Mat& GetImageToProcess() override ;
   QImage& GetImageToDisplay() override ;

      void GetImageToProcess(cv::Mat& ImageDst)override ;
      void GetImageToDisplay(QImage& ImageDst) override ;
    std::pair<int,int> GetImageSize() override { Camera->GetImageSize();};

    const std::vector<QPair<int,int>>& GetPoints() override {return Camera->GetPoints(); };  
    const std::vector<QRect>& GetRects() override  {return Camera->GetRects();};  
    const QString& GetInfo() override {return Camera->GetInfo();};  

  };


  public:
   CameraInterfaceHIK();
  ~CameraInterfaceHIK();

  public:
  QString    TAG_NAME{"[ CAMERA ]"};
  QString CAMERA_INFO{"[ CAMERA NO DATA ]"};


  std::shared_ptr<ImageSourceInterface> GetImageSourceChannel() override;

  //======================================================
  void StartCameraStream() override { StartStream();} ;
  void StopCameraStream()  override { StopStream();};
  void SetCameraRegion(int x, int y, int width, int height ) override {CameraSetOffset(x,y); CameraSetSize(width,height);};
  void SetCameraExposure(int Exposure) override { CameraSetExposure(Exposure);};
  //======================================================
  void StartStream();
  void StopStream();
  void CameraSetSize(int Width, int Height);
  void CameraSetHeight(int Height);
  void CameraSetWidth(int Width);
  void CameraSetOffset(int XOffset, int YOffset);
  void CameraSetExposure(float Exposure);

   QImage& GetImageToDisplay() override {return (*CurrentStoreChannel)->GetImageToDisplay();};
  cv::Mat& GetImageToProcess() override {return (*CurrentStoreChannel)->GetImageToProcess(); SwitchOutputChannel(); qDebug() << "GET";};

      void GetImageToDisplay(QImage& ImageDst) override { (*CurrentStoreChannel)->GetImageToDisplay(ImageDst);};
      void GetImageToProcess(cv::Mat& ImageDst)override { (*CurrentStoreChannel)->GetImageToProcess(ImageDst); SwitchOutputChannel();};
      void SwitchOutputChannel() { CurrentStoreChannel++; if(CurrentStoreChannel == ImageChanneledStore.end()) CurrentStoreChannel = ImageChanneledStore.begin();}

      bool isFrameAvailable() override { return (*CurrentStoreChannel)->isFrameAvailable();};

    const std::vector<QPair<int,int>>& GetPoints() override { return CameraPoints;};  
    const std::vector<QRect>& GetRects() override { return CameraRects;};  
    const            QString& GetInfo() override { return CAMERA_INFO;};  
           std::pair<float,float> GetFramePeriod() override ;

    std::pair<int,int> GetImageSize() override { return SizeImage;};
  //=========================================
          void CameraSetTriggerMode(int Mode);
          void CameraSetHorisontalReverse(bool OnOff);
          void CameraSetCameraID(QString str);
          void CameraGetTriggerMode();
          void CameraGetID();
          void CameraGetImageSize();
          void CameraGetExposure();
          bool isReverseHorizontal();
  //=========================================

  signals:
  void SignalNewImage(const cv::Mat&);
  void SignalNewImage(const QImage&);
  void SignalNewImage();

  private:
  MV_CC_DEVICE_INFO_LIST stDeviceList; 
  void* handle = NULL;

  std::pair<int,int> ImagePos{20,20}; 
  std::pair<int,int> SizeImage{720,540}; 
  std::vector<QPair<int,int>> CameraPoints{2};
  std::vector<QRect>          CameraRects{2};

  static void __stdcall ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX* FrameInfo, void* pUser);
  static CameraInterfaceHIK* CameraInterface;

           std::vector<std::shared_ptr<CameraImageStorage>> ImageChanneledStore;
  typename std::vector<std::shared_ptr<CameraImageStorage>>::iterator CurrentStoreChannel;

  //=====================================================

  bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);
  void EnumerateCameras();
  void CameraInit();
  void DeinitCamera();
};

#endif "WIDGET_CAMERA_INTERFACE_HIK_H"


