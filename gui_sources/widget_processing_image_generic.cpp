#include "widget_processing_image_generic.h"
#include "register_settings.h"
#include "ui_widget_processing_image_generic.h"
#include <qnamespace.h>
#include <QThread>
#include "register_settings.h"

WidgetProcessingImage::WidgetProcessingImage(QString ModuleName, QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetProcessingImage)
{
   ui->setupUi(this);
   ObjectPosRect.setRect(0, 0, 96, 96);
   SetName(ModuleName);

   strFreq = QString::number(0.0,'f',1).leftJustified(4,'0'); 
   strPeriodProcess = QString::number(0.0,'f',1).leftJustified(4,'0'); 
   strDisplayData = QString("ЧАСТОТА: %1  ОБРАБОТКА: %2").arg(strFreq).arg(strPeriodProcess);

   SlotDisplayString(strDisplayData);

                    auto ImageSize = SettingsRegister::GetPair("CAMERA_IMAGE_SIZE");
   DisplayImage = QImage(ImageSize.first,ImageSize.second,QImage::Format_ARGB32);
   QObject::connect(ui->labelImageDisplay,SIGNAL(SignalPressedPos(int,int)),this, SLOT(SlotPosPressed(int,int)));
}


void WidgetProcessingImage::SlotPosPressed(int x, int y)
{
 qDebug() << "WIDGET IMAGE POS: " << x << y; 
 emit SignalPosPressed(QPair<double,double>(x,y));
}


void WidgetProcessingImage::CopyImageToDisplayImage(const QImage& Image)
{
   if(Image.width() != DisplayImage.width() ||
      Image.height() != DisplayImage.height()) { DisplayImage = Image.copy().convertedTo(QImage::Format_RGB32); return; }

   for(int line = 0; line < DisplayImage.height()-1; ++line)
   {
       QRgb* pixelsLine = (QRgb *)DisplayImage.scanLine(line);
       const uint8_t* pixelsLineSource = Image.scanLine(line);

       for(int pixel = 0; pixel < DisplayImage.width()-1; ++pixel)
       {
         pixelsLine++; pixelsLineSource++;
         *pixelsLine = qRgb(*pixelsLineSource, *pixelsLineSource, *pixelsLineSource);
       }
   }
}
void WidgetProcessingImage::SlotStartPaintTrajectory(bool OnOff)
{
qDebug() << "DISPLAY PAINT TRAJECTORY: " << OnOff;
FLAG_PAINT_TRAJECTORY = OnOff; if(FLAG_PAINT_TRAJECTORY) Trajectory.clear();
}

void WidgetProcessingImage::SlotDisplayImage()
{
   //qDebug() << "DISPLAY: " << QThread::currentThread();
   if(!ImageSource){ qDebug() << TAG_NAME << "SOURCE NOT LINKED";  return; };

   //CopyImageToDisplayImage(ImageSource->GetImageToDisplay());
   ImageSource->GetImageToDisplay(DisplayImage);
   //DisplayImage = ImageSource->GetImageToDisplay().copy();

   const auto& ImagePoints = ImageSource->GetPoints(); 
   const auto& ImageRects = ImageSource->GetRects(); 
   auto& InfoString = ImageSource->GetInfo();

   auto pen_it = pens.begin();
   QPainter paint;
            paint.begin(&DisplayImage);

            for(auto& rect: ImageRects){paint.setPen(*pen_it); 
                                        paint.drawRect(rect); pen_it++;} pen_it = pens.begin();
                                                                         paint.setPen(pen3);
                                        paint.drawPoint(ImagePoints[0].first, 
                                                        ImagePoints[0].second);

                                                                         paint.setPen(pen6);
                                        paint.drawEllipse(ui->labelImageDisplay->X_Pressed-2, 
                                                          ui->labelImageDisplay->Y_Pressed-2,4,4);

            paint.end();

   ui->labelImageDisplay->setPixmap(QPixmap::fromImage(DisplayImage));

}

void WidgetProcessingImage::SlotDisplayImage(const QImage& Image)
{
   CopyImageToDisplayImage(Image);
   QPixmap pix = QPixmap::fromImage(DisplayImage);
   ui->labelImageDisplay->setPixmap(pix);
}


void WidgetProcessingImage::LinkToModule(std::shared_ptr<ImageSourceInterface> Source)
{
   ImageSource = Source;
   QObject::connect(ImageSource.get(),SIGNAL(SignalNewImage()), this,SLOT(SlotDisplayImage()),Qt::QueuedConnection);
   //QObject::connect(ImageSource.get(),SIGNAL(SignalNewImage(const QImage&)), this,SLOT(SlotDisplayImage(const QImage&)),Qt::QueuedConnection);
}


void WidgetProcessingImage::SetName(QString name) {ui->labelName->setText(name);}; 

void WidgetProcessingImage::SlotDisplayString(QString InfoString)
{
   ui->labelDataDisplay->setText(InfoString);
}