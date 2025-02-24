#include "widget_processing_image_generic.h"
#include "register_settings.h"
#include "ui_widget_processing_image_generic.h"
#include <qnamespace.h>
#include <QThread>
#include "register_settings.h"
#include "widget_adjustable.h"

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
   auto Widget  = new WidgetMiniLabelsGroup;
        Widget->setStyleSheet(this->styleSheet());
        Widget->layout()->setSpacing(1);
        Widget->layout()->setContentsMargins(1,1,1,1);

   QObject::connect(Widget, SIGNAL(SignalGroupSelected(int)),this, SLOT(SlotSetActiveChannel(int)));
   LinkedWidget = Widget;
   this->AddMiniLabel();
   this->AddMiniLabel();
   //LinkedWidget->hide();

}

void WidgetProcessingImage::AddMiniLabel()
{
   auto Widget = reinterpret_cast<WidgetMiniLabelsGroup*>(LinkedWidget);
   auto Label = new LabelImage; Label->setFixedSize(100,100); Label->setStyleSheet(ui->labelImageDisplay->styleSheet()); Widget->AddLabel(Label);
}

void WidgetProcessingImage::moveEvent(QMoveEvent* event)
{
  qDebug() << "WIDGET PROCESSING MOVE: " << event->pos() - event->oldPos();
  if(LinkedWidget != 0) LinkedWidget->move(LinkedWidget->pos() + event->pos() - event->oldPos());
  QWidget::moveEvent(event);
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
   if(!ImageSourceActive){ qDebug() << TAG_NAME << "SOURCE NOT LINKED";  return; };

   //CopyImageToDisplayImage(ImageSourceActive->GetImageToDisplay());
   ImageSourceActive->GetImageToDisplay(DisplayImage);
   //DisplayImage = ImageSourceActive->GetImageToDisplay().copy();

   const auto& ImagePoints = ImageSourceActive->GetPoints(); 
   const auto& ImageRects = ImageSourceActive->GetRects(); 
   auto& InfoString = ImageSourceActive->GetInfo();

   auto pen_it = pens.begin();
   QPainter paint;
            paint.begin(&DisplayImage);

            for(auto& rect: ImageRects){paint.setPen(*pen_it); 
                                        paint.drawRect(rect); pen_it++;} pen_it = pens.begin();
                                                                         paint.setPen(pen1);
                                        paint.drawPoint(ImagePoints[0].first, 
                                                        ImagePoints[0].second);
                                        paint.drawPoint(ui->labelImageDisplay->X_Pressed, 
                                                        ui->labelImageDisplay->Y_Pressed);

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
   ImageSourceActive = Source;
   ImageSources.push_back(Source);
   if(ImageSourceActive) QObject::disconnect(ImageSourceActive.get(),SIGNAL(SignalNewImage()), this,SLOT(SlotDisplayImage()));

   QObject::connect(ImageSourceActive.get(),SIGNAL(SignalNewImage()), this,SLOT(SlotDisplayImage()),Qt::QueuedConnection);
}

void WidgetProcessingImage::SetName(QString name) {ui->labelName->setText(name);}; 

void WidgetProcessingImage::SlotDisplayString(QString InfoString)
{
   ui->labelDataDisplay->setText(InfoString);
}

void WidgetProcessingImage::SlotSetActiveChannel(int Channel)
{
 qDebug() << "WIDGET PROCESSING SET ACTIVE CHANNEL : " << Channel << " CHANNELS: " << ImageSources.size();

                           if(Channel >= ImageSources.size()) return; 
 QObject::disconnect(ImageSourceActive.get(),SIGNAL(SignalNewImage()), this,SLOT(SlotDisplayImage()));
                     ImageSourceActive = ImageSources[Channel]; 
    QObject::connect(ImageSourceActive.get(),SIGNAL(SignalNewImage()), this,SLOT(SlotDisplayImage()),Qt::QueuedConnection);
}

//=============================================================
void WidgetMiniLabelsGroup::ChannelSelected(int Number)
{
   qDebug() << "[ GROUP MINI LABELS SELEDTED ] " << Number;
   emit SignalGroupSelected(Number-1);
}

void WidgetMiniLabelsGroup::AddLabel(LabelImage* label) 
{ 
   this->layout()->addWidget(label);

   int Number = ++NumberChannel;
   connect(label, &LabelImage::SignalPressedPos, [Number,this](){ChannelSelected(Number);});
}