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
#include "interface_image_source.h"



class AimImageImitatorClass; 

class AimControlInterface : public QObject
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
    int AimSpeed = 1;

    private slots:
    void SlotMoveAim() override; 

};

class AimImageImitatorClass : public ImageSourceInterface
{
    Q_OBJECT
public:
    explicit AimImageImitatorClass(QObject* parent = 0);
    ~AimImageImitatorClass();

	std::string TAG_NAME{"[ IMAGE_IMIT ]"};
	    QString INFO_STRING{"[ NO DATA ]"};

    cv::Mat ImageProc;
    cv::Mat ImageTestObject;
    cv::Mat BlotchObject;
    cv::Mat OutputImage;
    cv::Mat ImageToProcess;
    QImage  ImageToDisplay;
    cv::Rect AIM_RECT = cv::Rect(30,30,96,96);

    static std::pair<int,int> SizeImage; // MAY BE CHANGED IN INIT
    static std::pair<int,int> SizeAimBoundry; // MAY BE CHANGED IN INIT

    ManualAimControl ManualControl;
    DynamicAimControl DynamicControl;

QTime ThinningTimePeriod;

QImage& GetImageToDisplay();
cv::Mat& GetImageToProcess();

void GetImageToDisplay(QImage& ImageDst) { ImageDst = ImageToDisplay.copy();};
//void GetImageToDisplay(QImage& ImageDst) { GetImageToDisplayColor(ImageDst);};
void GetImageToProcess(cv::Mat& ImageDst) { ImageDst = ImageToProcess.clone();};
void GetImageToDisplayColor(QImage& ImageDst);

const std::vector<QPair<int,int>>& GetPoints();  
const std::vector<QRect>&          GetRects();  
const QString&                     GetInfo();  
//==================================================
std::vector<QPair<int,int>> CoordsImage;
std::vector<QRect>          RectsImage;

void GenerateAimImage();
void CheckCorrectROI(cv::Rect& ROI);
bool IsROIValid(cv::Rect& ROI);

public  slots:

void SlotMoveAimPos(int XStep, int YStep);
void SlotSetAimPos(int PosX, int PosY);

};

#endif // CV_AIM_IMIT_H
