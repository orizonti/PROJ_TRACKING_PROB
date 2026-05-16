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
#include "register_settings.h"


class CameraInterfaceUniversal :public QObject, 
                                public SourceImageInterface, 
                                public SourceImageDisplayInterface, 
                                public CameraControlInterface,
                                public DeviceGenericHandleControl
 
{
  Q_OBJECT

  public:
  explicit CameraInterfaceUniversal(std::string strVideoSource, uint32_t Number = 0, QString NAME = "[ CAMERA ]");
           ~CameraInterfaceUniversal();

  public:
  std::pair<int,int> SIZE_ROI = SettingsRegister::GetPair("PROCESS_ROI_SIZE");
  std::pair<int,int> OFFSET_ROI = SettingsRegister::GetPair("PROCESS_ROI_OFFSET");
  std::pair<int,int> SIZE_CAMERA = SettingsRegister::GetPair("CAMERA_RESOLUTION");

  std::string    TAG_NAME{"[ CAMERA_RTSP ]"};
  std::string CAMERA_INFO{"[ CAMERA NO DATA ]"};

  cv::Mat inputImage          {SIZE_CAMERA.first,SIZE_CAMERA.second,CV_8UC3};
  cv::Mat inputImageResized   {SIZE_ROI.first,SIZE_ROI.second,CV_8UC3};
  cv::Mat inputImageProcessed {SIZE_ROI.first,SIZE_ROI.second,CV_8UC1};

  cv::Rect rectCrop{OFFSET_ROI.first,OFFSET_ROI.second,SIZE_ROI.first,SIZE_ROI.second};
  QString getName() override { return QString::fromStdString(TAG_NAME); };

  void moveToThread(QThread* thread);


  std::shared_ptr<SourceImageInterface> getImageSourceChannel() override;

       int getAvailableFrames() override;

       std::pair<float,float> getTickPeriod() override { return std::pair<float,float>(0,0); };

      void getImageToDisplay(QImage& ImageDst) override ;

  cv::Mat& getImageToProcess()                 override ;
      void getImageToProcess(cv::Mat& ImageDst)override ;

    const std::vector<QPair<int,int>>& getPoints() override { return CameraPoints;};  
              const std::vector<QRect>& getRects() override { return CameraRects;};  
                      const std::string& getInfo() override { return CAMERA_INFO;};  

                 std::pair<int,int> getSizeImage() override;
  //=========================================

  void CameraSetRegion  (int XOffset, int YOfffset, int width, int height ) override;
  void CameraSetSize    (int Width  , int Height)  override;
  void CameraSetOffset  (int XOffset, int YOffset) override;
  void CameraSetHeight  (int Height) override;
  void CameraSetWidth   (int Width ) override;
  void CameraSetZoom    (int Zoom  ) override;

  void CameraSetExposure(float Exposure) override;
  void CameraSetGain    (float Gain)     override;
  void CameraStartStream(bool OnOff )    override { if(OnOff) emit signalStart(); else emit signalStop(); }

  void SetStateActive() { emit signalStart(); };
  void SetStateIdle()   { emit signalStop(); }; 
  void SetReset()       { emit signalReset(); }; 

	void setParam (uint16_t CommandID, float    CommandParam) override {};

  MeasurePeriodNode FrameMeasureInput;
  MeasurePeriodNode FrameMeasureProcess;

  //=============================================
  std::pair<int,int> ImagePos {20 ,20 }; 
  std::pair<int,int> SizeImage{1440/2,1440/2}; 

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
  void slotReset() {};

  signals:
  void signalStart(); 
  void signalStop();  
  void signalReset();  

  private:

  void enumerateCameras();
  void initCamera();
  void deinitCamera();
};


#endif 


