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
#include "module_period_measure.h"
#include "camera_control_interface.h"
#include "device_generic_interface.h"


class CameraInterfaceUniversal :public QObject, 
                                public SourceImageInterface, 
                                public SourceImageDisplayInterface, 
                                public CameraControlInterface,
                                public DeviceGenericHandleControl
 
{
  Q_OBJECT

  public:
  explicit CameraInterfaceUniversal(std::string strVideoSource, QString NAME = "[ CAMERA ]");
           ~CameraInterfaceUniversal();

  public:

  QString    TAG_NAME{"[ CAMERA_RTSP ]"};
  QString CAMERA_INFO{"[ CAMERA NO DATA ]"};
  cv::Mat inputImage{2560,1440,CV_8UC3};
  cv::Mat inputImageSmall{2560/2,1440/2,CV_8UC3};
  cv::Mat inputImageGray{2560/2,1440/2,CV_8UC1};
  QString getName() override { return TAG_NAME; };

  std::shared_ptr<SourceImageInterface> getImageSourceChannel() override;

       int getAvailableFrames() override;

       std::pair<float,float> getTickPeriod() override { return std::pair<float,float>(0,0); };

      void getImageToDisplay(QImage& ImageDst) override ;

  cv::Mat& getImageToProcess()                 override ;
      void getImageToProcess(cv::Mat& ImageDst)override ;

    const std::vector<QPair<int,int>>& getPoints() override { return CameraPoints;};  
              const std::vector<QRect>& getRects() override { return CameraRects;};  
                          const QString& getInfo() override { return CAMERA_INFO;};  

                 std::pair<int,int> getSizeImage() override;

    void moveToThread(QThread* thread);
  //=========================================

  void CameraSetRegion  (int XOffset, int YOfffset, int width, int height ) override;
  void CameraSetSize    (int Width  , int Height)  override;
  void CameraSetOffset  (int XOffset, int YOffset) override;
  void CameraSetHeight  (int Height) override;
  void CameraSetWidth   (int Width ) override;
  void CameraSetZoom    (int Zoom  ) override;

  void CameraSetExposure(float Exposure) override;
  void CameraSetGain    (float Gain)     override;
  void CameraStartStream(bool OnOff )    override { if(OnOff) slotStartStream(); else slotStopStream(); }

	void setParam (uint16_t CommandID, uint32_t CommandParam) override {};
	void setParam (uint16_t CommandID, float    CommandParam) override {};


  MeasurePeriodNode FrameMeasureInput;
  MeasurePeriodNode FrameMeasureProcess;

  //=============================================
  std::pair<int,int> ImagePos {20 ,20 }; 
  std::pair<int,int> SizeImage{2560/4,1440/4}; 

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


#endif 


