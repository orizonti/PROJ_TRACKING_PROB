#include "AimImageImitatorClass.h"

AimImageImitatorClass::AimImageImitatorClass(QObject* parent) : ImageSourceInterface(parent)
{


//QString AimImagePath = "D:/DATA/UFO.png"; qDebug() << "LOAD IMAGE: " <<  AimImagePath;
//QString BlotchImagePath = "D:/DATA/blotch.png"; qDebug() << "LOAD IMAGE: " <<  BlotchImagePath;

QString AimImagePath = "/home/broms/DATA/UFO.png"; qDebug() << "LOAD IMAGE: " <<  AimImagePath;
QString BlotchImagePath = "/home/broms/DATA/blotch.png"; qDebug() << "LOAD IMAGE: " <<  BlotchImagePath;

ImageTestObject = cv::imread(AimImagePath.toStdString(),cv::IMREAD_GRAYSCALE );
BlotchObject = cv::imread(BlotchImagePath.toStdString(),cv::IMREAD_GRAYSCALE );
imshow("test_image", ImageTestObject);

FrameImage = cv::Mat(600,600,CV_8UC1); FrameImage = cv::Scalar(0,0,0);

AIM_RECT = cv::Rect(300,300,ImageTestObject.cols,ImageTestObject.rows);

auto BLOTCH_RECT = AIM_RECT; BLOTCH_RECT.x += 30; 
BlotchObject.copyTo(FrameImage(BLOTCH_RECT)); 

ImageTestObject.copyTo(FrameImage(AIM_RECT));

DynamicControl.LinkToImageImitator(this);
ManualControl.LinkToImageImitator(this);

ThinningTimePeriod = QTime::currentTime();
}

void AimImageImitatorClass::GenerateAimImage()
{

    if(!IsROIValid(AIM_RECT)) qDebug() << "INVALID ROI: " << AIM_RECT.x << AIM_RECT.y << AIM_RECT.width << AIM_RECT.height;
    if(!IsROIValid(AIM_RECT)) CheckCorrectROI(AIM_RECT);

    auto BLOTCH_RECT = AIM_RECT; BLOTCH_RECT.x += 30;
    FrameImage = cv::Scalar(0);
    BlotchObject.copyTo(FrameImage(BLOTCH_RECT)); 
    ImageTestObject.copyTo(FrameImage(AIM_RECT)); // DRAW NEW AIM IMAGE

    LastImageToDisplay = QImage(FrameImage.data,
                                FrameImage.cols,
                                FrameImage.rows,
               static_cast<int>(FrameImage.step),
                         QImage::Format_Grayscale8 );

    emit SignalNewImage(LastImageToDisplay);

    if(ThinningTimePeriod.msecsTo(QTime::currentTime()) >= 100)
    {
    FrameImageToProcess = FrameImage.clone();
    ThinningTimePeriod = QTime::currentTime();
    emit SignalNewImage(FrameImageToProcess);
    }
}

void AimImageImitatorClass::SlotMoveAimPos(int XStep, int YStep)
{
    FrameImage(AIM_RECT) = cv::Scalar(0,0,0); // CLEAR OLD AIM IMAGE

    AIM_RECT.x += XStep; AIM_RECT.y += YStep;
    if(AIM_RECT.x < 0)   AIM_RECT.x = 0;   if(AIM_RECT.y < 0)   AIM_RECT.y = 0;
    if(AIM_RECT.x > 600-AIM_RECT.width) AIM_RECT.x = 600-AIM_RECT.width; if(AIM_RECT.y > 600-AIM_RECT.height) AIM_RECT.y =600-AIM_RECT.height;

    GenerateAimImage();
}

void AimImageImitatorClass::SlotSetAimPos(int PosX, int PosY)
{
    FrameImage(AIM_RECT) = cv::Scalar(0,0,0); // CLEAR OLD AIM IMAGE
    AIM_RECT.x = PosX; AIM_RECT.y = PosY;
    if(AIM_RECT.x < 0)   AIM_RECT.x = 0;   if(AIM_RECT.y < 0)   AIM_RECT.y = 0;
    if(AIM_RECT.x > 600-AIM_RECT.width) AIM_RECT.x = 600-AIM_RECT.width; if(AIM_RECT.y > 600-AIM_RECT.height) AIM_RECT.y =600-AIM_RECT.height;

    GenerateAimImage();
}

QImage AimImageImitatorClass::GetNewImageDisplay()
{
    return LastImageToDisplay;
}

cv::Mat& AimImageImitatorClass::GetNewImageProcess()
{
    return FrameImageToProcess;
}


AimImageImitatorClass::~AimImageImitatorClass()
{
 qDebug() << "STOP AND DEINIT CAMEA";
}


void AimControlInterface::StartMove() { timerMoveAim.start(30); }
void AimControlInterface::StopMove() { timerMoveAim.stop(); }
void AimControlInterface::LinkToImageImitator(AimImageImitatorClass* ImitatorObjectPtr)
{
connect(&timerMoveAim, SIGNAL(timeout()), this ,SLOT(SlotMoveAim()));
ImageImitator = ImitatorObjectPtr;
}

void ManualAimControl::SlotMoveAim()
{
   if(Axis == 0) ImageImitator->SlotMoveAimPos(Step*Direction, 0); else ImageImitator->SlotMoveAimPos(0, Step*Direction);
}

void DynamicAimControl::SlotMoveAim()
{
    XNumberPos+=1; YNumberPos+=2; 
    if(XNumberPos == 360*AimSpeed) XNumberPos = 0; 
    if(YNumberPos == 360*AimSpeed) YNumberPos = 0; 

    XCoordPos = 300 + 200*sin(M_PI/180*XNumberPos/AimSpeed); YCoordPos = 300 + 200*sin(M_PI/180*YNumberPos/AimSpeed);
    ImageImitator->SlotSetAimPos(XCoordPos, YCoordPos);

}

void ManualAimControl::SlotStartMoveAim(int Dir, int MoveAxis, int MoveStep)
{
   Direction = Dir;
   Axis = MoveAxis;
   Step = MoveStep;
   this->StartMove();
}

bool AimImageImitatorClass::IsROIValid(cv::Rect& ROI)
{
const int& ImageWidth = FrameImage.cols;
const int& ImageHeight = FrameImage.rows;
//qDebug() << "CHECK ROI: " << ROI.x << ROI.y << ROI.width << ROI.height;

if((ROI.x + ROI.width + 1) > ImageWidth) return false;
if((ROI.y + ROI.height + 1) > ImageHeight) return false;
if (ROI.x < 0) return false;
if (ROI.y < 0) return false;

return true;
}

void AimImageImitatorClass::CheckCorrectROI(cv::Rect& ROI)
{
const int& ImageWidth = FrameImage.cols;
const int& ImageHeight = FrameImage.rows;

const int OFFSET_X = ROI.x + ROI.width - ImageWidth + 1;
const int OFFSET_Y = ROI.y + ROI.height - ImageHeight + 1;

if (OFFSET_X > 0) ROI.x = ImageWidth - ROI.width - 1;
if (OFFSET_Y > 0) ROI.y = ImageHeight - ROI.height - 1;

if (ROI.x < 0) ROI.x = 0;
if (ROI.y < 0) ROI.y = 0;

}