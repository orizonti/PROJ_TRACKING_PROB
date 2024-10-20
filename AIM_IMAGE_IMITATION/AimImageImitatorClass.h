#ifndef CV_AIM_IMIT_H
#define CV_AIM_IMIT_H

//#include <chrono>
#include <QObject>
#include <QDebug>

#include <QTimer>
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>
#include <string>
#include <QTime>


class ImageSourceInterface: public QObject
{
    Q_OBJECT
    public:
    ImageSourceInterface(QObject* parent = 0): QObject{parent} {};
    virtual QImage GetNewImageDisplay() = 0;
    virtual cv::Mat& GetNewImageProcess() = 0;
    
    signals:
    void SignalNewImage(const cv::Mat&);
    void SignalNewImage(QImage);

};

class AimImageImitatorClass; class AimControlInterface : public QObject
{
    Q_OBJECT
    public:
    QTimer timerMoveAim;
    AimImageImitatorClass* ImageImitator = 0;

    void LinkToImageImitator(AimImageImitatorClass* ImitatorObjectPtr);
    void StopMove();
    void StartMove();

    private slots:
    virtual void SlotMoveAim() = 0;
};

class ManualAimControl : public AimControlInterface
{
    Q_OBJECT
    public:
    int Step = 0; 
    int Direction = 0;
    int Axis = 0;

    public slots:
    void SlotStartMoveAim(int Dir, int MoveAxis, int MoveStep);
    private slots:
    void SlotMoveAim() override; 
};

class DynamicAimControl : public AimControlInterface
{
    Q_OBJECT
    public:
    float XNumberPos = 0;
    float YNumberPos = 0;
    float XCoordPos = 0;
    float YCoordPos = 0;
    int AimSpeed = 2;

    private slots:
    void SlotMoveAim() override; 

};


class AimImageImitatorClass : public ImageSourceInterface
{
    Q_OBJECT
public:
    explicit AimImageImitatorClass(QObject* parent = 0);
    ~AimImageImitatorClass();
    QString BLOCK_NAME = "CV_AIM_IMITATOR";

    cv::Mat ImageProc;
    cv::Mat ImageTestObject;
    cv::Mat BlotchObject;
    cv::Mat FrameImage;
    cv::Mat FrameImageToProcess;
    QImage  LastImageToDisplay;
    cv::Rect AIM_RECT = cv::Rect(30,30,96,96);

    ManualAimControl ManualControl;
    DynamicAimControl DynamicControl;

QTime ThinningTimePeriod;

QImage GetNewImageDisplay();
cv::Mat& GetNewImageProcess();

void GenerateAimImage();
void CheckCorrectROI(cv::Rect& ROI);
bool IsROIValid(cv::Rect& ROI);

signals:
void SignalNewImage(const cv::Mat&);
void SignalNewImage(QImage);

public  slots:

void SlotMoveAimPos(int XStep, int YStep);
void SlotSetAimPos(int PosX, int PosY);

};

#endif // CV_AIM_IMIT_H
