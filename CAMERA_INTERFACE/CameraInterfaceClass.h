#pragma once
#include <qnamespace.h>
#include <iostream>
#include <typeinfo>
#include <QObject>
#include <queue>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include "AIM_IMAGE_IMITATION/AimImageImitatorClass.h"


#undef signals
extern "C" 
{ 
    #include <arv.h> 
	#include <stdlib.h>
}
#define signals Q_SIGNALS

class AravisCameraInterfaceClass;
struct ArvStreamCallbackData
{
	ArvStream *stream;
	int counter;
	gboolean done;
    AravisCameraInterfaceClass* Receiver;

};

class AravisCameraInterfaceClass: public ImageSourceInterface
{
    Q_OBJECT
    public:
    AravisCameraInterfaceClass(QObject* parent = 0);
    ~AravisCameraInterfaceClass();
	GError *error = NULL;

    int StartCameraStream();
    int StopCameraStream();
    int InitCamera();
    void PutNewFrameToStorage(ArvBuffer* buffer);
    QTimer timerDisplayTestImage;

    cv::Mat ImageToProcess;
    QImage ImageToDisplay;

    QImage GetNewImageDisplay();
    cv::Mat& GetNewImageProcess();

	ArvCamera *camera;
    std::vector<uint8_t*> Buffers;
    std::vector<uint8_t*>::iterator CurrentBuffer;

    size_t payload;

	ArvStreamCallbackData callback_data;

    signals:
    void SignalNewImage(const cv::Mat&);
    void SignalNewImage(QImage);
    public slots:
    void SlotDisplayProcessImage();
};
