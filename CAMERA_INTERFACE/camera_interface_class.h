#pragma once
#include <qnamespace.h>
#include <iostream>
#include <typeinfo>
#include <QObject>
#include <queue>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include "interface_image_source.h"
#include <QTimer>
#include "camera_control_interface.h"
#include <QThread>


#undef signals
extern "C" 
{ 
    #include <arv.h> 
	#include <stdlib.h>
}
#define signals Q_SIGNALS

class CameraInterfaceAravis;
struct ArvStreamCallbackData
{
	ArvStream *stream = NULL;
	int counter;
	gboolean done;
  CameraInterfaceAravis* Receiver = NULL;
};



class CameraInterfaceAravis: public ImageSourceInterface, public CameraControlInterface
{
    Q_OBJECT
    public:
    CameraInterfaceAravis(QObject* parent = 0);
    ~CameraInterfaceAravis();
	GError *error = NULL;
    QString TAG_NAME{"[ ARAVIS ]"};
    QString CAMERA_INFO{"[ CAMERA NO DATA ]"};
    bool FLAG_CAMERA_CONNECTED = false;
    bool FLAG_CAMERA_WORK = false;

    std::pair<int,int> SizeImage{400,400}; // MAY BE CHANGED IN INIT
    std::pair<int,int> ImagePos{20,20}; // MAY BE CHANGED IN INIT

    int ThinningCounter = 0;
    int FrequencyDevider = 4;
    int BaseFrequency = 200;
    int CurrentReceiver = 0;

    int  GetAvailableFrames() override { return NumberFrameToProcess; };
    void GetCurrentCameraRegion();

    void SetCameraRegion(std::pair<int,int> Pos, std::pair<int,int> Size) override;
    void SetCameraRegion(int x, int y, int width, int height ) override;
    void SetCameraExposure(int Exposure) override {}; 
    void  StartCameraStream() override;
    void  StopCameraStream()  override;

    bool SwitchToNextFrame() override;
    void SkipFrames() override;
    void SetFrequency(int Frequency) ;

    int  InitCamera();
    void DeinitCamera();
    void PutNewFrameToStorage(ArvBuffer* buffer);
    QTimer timerDisplayTestImage;

    cv::Mat ImageToProcess;
    QImage ImageToDisplay;
    int NumberFrameToProcess = 0;
    int Counter = 0;

     QImage& GetImageToDisplay() override ;
    cv::Mat& GetImageToProcess() override ;

    void GetImageToDisplay(QImage& ImageDst) override { ImageDst = ImageToDisplay.copy();};
    void GetImageToProcess(cv::Mat& ImageDst)override ;


    std::vector<QPair<int,int>>& GetPoints() override;  
    std::vector<QRect>& GetRects() override;  
    QString& GetInfo() override;  

    std::vector<QPair<int,int>> CameraPoints{2};
    std::vector<QRect>          CameraRects{2};

	  ArvCamera *camera = nullptr;
    std::vector<uint8_t*> Buffers;
    std::vector<ArvBuffer*> BuffersArv;

    std::vector<uint8_t*>::iterator BufferToWrite;
    std::vector<uint8_t*>::iterator BufferToRead;

    size_t payload;

	ArvStreamCallbackData callback_data;

    std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    std::chrono::duration<double> Duration;

    public slots:
    void SlotDisplayProcessImage();
    void SlotSetFrequencyDevider(int param) { FrequencyDevider = param; };
    void SlotSetHighFrequency();

    void SlotDeinitCamera(); 
};
