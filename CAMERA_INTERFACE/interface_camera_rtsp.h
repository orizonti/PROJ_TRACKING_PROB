#ifndef WIDGET_CAMERA_INTERFACE_H
#define WIDGET_CAMERA_INTERFACE_H
#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "interface_image_source.h"
#include <QDebug>
#include <QTimer>
#include <QMutex>
#include "camera_image_storage.h"


class CameraInterface
{
  public:
  virtual void CameraSetSize(int Width, int Height) = 0;
  virtual void CameraSetOffset(int XOffset, int YOffset) = 0;
  virtual void CameraSetHeight(int Height) = 0;
  virtual void CameraSetWidth(int Width) = 0;
  virtual void CameraSetExposure(float Exposure) = 0;
};


class CameraInterfaceUniversal :public QObject, public SourceImageInterface, public SourceImageDisplayInterface, public CameraInterface
{
  Q_OBJECT

  public:
  explicit CameraInterfaceUniversal(std::string strVideoSource, QString NAME = "[ CAMERA ]");
           ~CameraInterfaceUniversal();
  public:
  QString    TAG_NAME{"[ CAMERA_RTSP ]"};
  QString CAMERA_INFO{"[ CAMERA NO DATA ]"};
  cv::Mat inputImage{640,512,CV_8UC1};
  cv::Mat inputImageSmall{640,512,CV_8UC1};
  cv::Mat inputImageGray{640,512,CV_8UC1};

  std::shared_ptr<SourceImageInterface> getImageSourceChannel() override;

       int getAvailableFrames() override;

      QImage& getImageToDisplay() override ;
      void getImageToDisplay(QImage& ImageDst) override ;

  cv::Mat& getImageToProcess()                 override ;
      void getImageToProcess(cv::Mat& ImageDst)override ;

    const std::vector<QPair<int,int>>& getPoints() override { return CameraPoints;};  
              const std::vector<QRect>& getRects() override { return CameraRects;};  
                          const QString& getInfo() override { return CAMERA_INFO;};  

                 std::pair<int,int> getSizeImage() override;

    void moveToThread(QThread* thread);
  //=========================================
  //

  void CameraSetSize(int Width, int Height);
  void CameraSetOffset(int XOffset, int YOffset);
  void CameraSetHeight(int Height);
  void CameraSetWidth(int Width);
  void CameraSetExposure(float Exposure);

  void SetZoom(int Zoom) { };
  void StartCameraStream(bool OnOff ) { if(OnOff) slotStartStream(); else slotStopStream(); }

  FramePeriodMeasure FrameMeasureInput;
  FramePeriodMeasure FrameMeasureProcess;

  //=============================================
  std::pair<int,int> ImagePos {20 ,20 }; 
  //std::pair<int,int> SizeImage{2560/2,1440/2}; 
  std::pair<int,int> SizeImage{640,512}; 

  std::vector<QPair<int,int>> CameraPoints{2};
  std::vector<QRect>          CameraRects {2};

  cv::VideoCapture capture;
  QTimer timerGetFrame{this};

  QMutex mutexStorage;
  std::vector<std::shared_ptr<CameraImageStorage<CameraInterfaceUniversal>>> ImageStore;
  //=============================================

  public slots:
  void slotGetFrame();
  void slotStartStream();
  void slotStopStream();
  void slotEndWork();

  private:

  void enumerateCameras();
  void initCamera();
  void deinitCamera();
};


#endif "WIDGET_CAMERA_INTERFACE_H"


