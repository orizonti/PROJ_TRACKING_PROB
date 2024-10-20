#include "DisplayWidget.h"
#include "ui_DisplayWidget.h"
DisplayWidget::DisplayWidget(QWidget* parent) : AdjustableWidget(parent), ui(new Ui::DisplayWidget)
{
    ui->setupUi(this);
   ObjectPosRect.setRect(0, 0, 96, 96);
}

void DisplayWidget::SlotDisplayImage(QImage Image)
{
   DisplayImage = Image.copy();
   //Counter++; if(Counter == 30) {ImageCounter += Counter; qDebug() << "DISPLAY IMAGE :" << ImageCounter; Counter=0;}
   QPen pen1; pen1.setWidth(3); pen1.setColor(Qt::green);
   QPen pen2; pen2.setWidth(2); pen2.setColor(Qt::red); pen2.setStyle(Qt::DashLine);

   QPainter paint;
            paint.begin(&DisplayImage);
            paint.setPen(pen1);
            //paint.drawRect(ObjectPosRect);
            paint.drawText(20,50,QString("X: %1 Y: %2").arg(ObjectPosRect.left()).arg(ObjectPosRect.right()));
            paint.drawLine(ObjectVelLine);
            //paint.drawLine(ArrowRight);
            //paint.drawLine(ArrowLeft);
            paint.setPen(pen2);
            paint.drawRect(ObjectFindRect);
            paint.end();


   int ROI_SIZE = 320;
   //DisplayImage = Image.copy(512 - ROI_SIZE, 512 - ROI_SIZE, ROI_SIZE*2, ROI_SIZE*2);
   QPixmap pix = QPixmap::fromImage(DisplayImage);
   ui->labelImageDisplay->setPixmap(pix);
}

void DisplayWidget::SlotDisplayObjectPos(cv::Point ObjectPos, std::pair<float,float> ObjectVel, cv::Rect ROIRect)
{
   ObjectPosRect.setRect(ObjectPos.x, ObjectPos.y, 60, 60);
   ObjectVelLine.setLine(ROIRect.x + ROIRect.width,  
                         ROIRect.y, 
                         ROIRect.x + ROIRect.width + ObjectVel.first*20.0, 
                         ROIRect.y + ObjectVel.second*20.0);
   ArrowRight.setLine(ObjectVelLine.x2(),ObjectVelLine.y2(),ObjectVelLine.x2()-20,ObjectVelLine.y2());
   ArrowLeft.setLine(ObjectVelLine.x2(),ObjectVelLine.y2(),ObjectVelLine.x2(),ObjectVelLine.y2()+20);

   ObjectFindRect.setRect(ROIRect.x, ROIRect.y, ROIRect.width, ROIRect.height);
}

void DisplayWidget::LinkToImageSource(ImageSourceInterface* ImageSource)
{
   QObject::connect(ImageSource,SIGNAL(SignalNewImage(QImage)),this,SLOT(SlotDisplayImage(QImage)),Qt::QueuedConnection);
}
void DisplayWidget::LinkToImitator(AimImageImitatorClass* Imitator)
{
   qDebug() << "LINK TO IMITATOR MODULE : ";
   ImitationModule = Imitator; 
   Imitator->GenerateAimImage();
   LinkToImageSource(Imitator);
   //Imitator->StartGenerate();
}

void DisplayWidget::LinkToProcessModule(CVImageProcessing* Processor)
{
   qDebug() << "LINK DISPLAY to IMAGE PROCESSOR";
   QObject::connect(Processor,SIGNAL(SignalObjectDetected(cv::Point,std::pair<float,float>,cv::Rect)),this,SLOT(SlotDisplayObjectPos(cv::Point,std::pair<float,float>,cv::Rect)));
}

void DisplayWidget::on_butMoveRight_pressed(){ ImitationModule->ManualControl.SlotStartMoveAim(-1,0,2); }
void DisplayWidget::on_butMoveLeft_pressed() { ImitationModule->ManualControl.SlotStartMoveAim( 1,0,2); }
void DisplayWidget::on_butMoveUp_pressed()   { ImitationModule->ManualControl.SlotStartMoveAim(-1,1,2); }
void DisplayWidget::on_butMoveDown_pressed() { ImitationModule->ManualControl.SlotStartMoveAim( 1,1,2); }

void DisplayWidget::on_butMoveRight_released(){ ImitationModule->ManualControl.StopMove(); }
void DisplayWidget::on_butMoveLeft_released() { ImitationModule->ManualControl.StopMove(); }
void DisplayWidget::on_butMoveUp_released()   { ImitationModule->ManualControl.StopMove(); }
void DisplayWidget::on_butMoveDown_released() { ImitationModule->ManualControl.StopMove(); }

void DisplayWidget::on_butStartAimMoving_clicked() { ImitationModule->DynamicControl.StartMove(); }
void DisplayWidget::on_butStopAimMoving_clicked() { ImitationModule->DynamicControl.StopMove(); }