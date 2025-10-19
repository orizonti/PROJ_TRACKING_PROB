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
    std::string TAG_NAME = QString("[ %1 ] ").arg("CAMERA STOR").toStdString();
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

    int getAvailableFrames() override {return BufferToWrite - BufferToRead;}
    bool isFrameAvailable() override { return BufferToWrite != BufferToRead;};

    CameraInterfaceHIK* Camera = nullptr;

    bool switchToNextFrame() override;
    void skipFrames() override;

    cv::Mat ImageToProcess;

  cv::Mat& getImageToProcess() override ;
   QImage& getImageToDisplay() override ;

      void getImageToProcess(cv::Mat& ImageDst)override ;
      void getImageToDisplay(QImage& ImageDst) override ;
    std::pair<int,int> getImageSize() override { Camera->getImageSize();};

    const std::vector<QPair<int,int>>& getPoints() override {return Camera->getPoints(); };  
    const std::vector<QRect>& getRects() override  {return Camera->getRects();};  
    const QString& getInfo() override {return Camera->getInfo();};  

  };


  public:
   CameraInterfaceHIK();
  ~CameraInterfaceHIK();

  public:
  QString CAMERA_INFO{"[ CAMERA NO DATA ]"};
  std::string TAG_NAME = QString("[ %1 ] ").arg("CAMERA").toStdString();

  std::shared_ptr<ImageSourceInterface> getImageSourceChannel() override;

  //======================================================
  public slots:
  void StartCameraStream(bool OnOff) override { if(OnOff) StartStream(); else StopStream(); } ;
  void SetCameraRegion(int x, int y, int width, int height ) override {CameraSetOffset(x,y); CameraSetSize(width,height);};
  void SetCameraExposure(int Exposure) override { CameraSetExposure(Exposure);};
  void SetZoom(int Number) override;
  //======================================================
  public:
  void StartStream();
  void StopStream();
  void CameraSetSize(int Width, int Height);
  void CameraSetHeight(int Height);
  void CameraSetWidth(int Width);
  void CameraSetOffset(int XOffset, int YOffset);
  void CameraSetExposure(float Exposure);

   QImage& getImageToDisplay() override {return (*CurrentStoreChannel)->getImageToDisplay();};
  cv::Mat& getImageToProcess() override {return (*CurrentStoreChannel)->getImageToProcess(); SwitchOutputChannel();};

      void getImageToDisplay(QImage& ImageDst) override { (*CurrentStoreChannel)->getImageToDisplay(ImageDst);};
      void getImageToProcess(cv::Mat& ImageDst)override { (*CurrentStoreChannel)->getImageToProcess(ImageDst); SwitchOutputChannel();};
      void SwitchOutputChannel() { CurrentStoreChannel++; if(CurrentStoreChannel == ImageChanneledStore.end()) CurrentStoreChannel = ImageChanneledStore.begin();}

      bool isFrameAvailable() override { return (*CurrentStoreChannel)->isFrameAvailable();};

    const std::vector<QPair<int,int>>& getPoints() override { return CameraPoints;};  
    const std::vector<QRect>& getRects() override { return CameraRects;};  
    const            QString& getInfo() override { return CAMERA_INFO;};  
           std::pair<float,float> getFramePeriod() override ;

    std::pair<int,int> getImageSize() override { return SizeImage;};
  //=========================================
          void CameraSetTriggerMode(int Mode);
          void CameraSetHorisontalReverse(bool OnOff);
          void CameraSetCameraID(QString str);
          void CameraGetTriggerMode();
          void CameraGetID();
          void CameragetImageSize();
          void CameraGetExposure();
          bool isReverseHorizontal();
  //=========================================

  signals:
  void signalNewImage(const cv::Mat&);
  void signalNewImage(const QImage&);
  void signalNewImage();

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


