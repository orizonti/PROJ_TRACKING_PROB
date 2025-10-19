#include "widget_processing_image_generic.h"
#include "register_settings.h"
#include "ui_widget_processing_image_generic.h"
#include <qnamespace.h>
#include <QThread>
#include "register_settings.h"
#include "widget_adjustable.h"
#include "debug_output_filter.h"

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
   auto Image = QImage(ImageSize.first,ImageSize.second,QImage::Format_ARGB32); DisplayImagesMini.append(Image);
        Image = QImage(ImageSize.first,ImageSize.second,QImage::Format_ARGB32); DisplayImagesMini.append(Image);

        DisplayImage = QImage(ImageSize.first,ImageSize.second,QImage::Format_ARGB32); 

   QObject::connect(ui->labelImageDisplay,SIGNAL(SignalPressedPos(int,int)),this, SLOT(SlotPosPressed(int,int)));
   auto Widget  = new WidgetMiniLabelsGroup;
        Widget->layout()->setSpacing(6);
        Widget->layout()->setContentsMargins(2,2,2,2);

        Widget->setStyleSheet(
        " QWidget { background-color: rgba(46, 65, 83,100);"
        " border: 2px solid #455364;"
        " padding: 0px;"
        " color: #E0E1E3;"
        " selection-background-color: #346792;"
        " selection-color: #E0E1E3; }"
        );

   QObject::connect(Widget, SIGNAL(SignalChannelChanged(int)),this, SLOT(SlotSetActiveChannel(int)));
   LinkedWidget = Widget;
   this->AddMiniLabel(); 
   this->AddMiniLabel(); 
   //LinkedWidget->hide();

   connect(&timerDisplay, &QTimer::timeout, this, static_cast<void (WidgetProcessingImage::*)()>(&WidgetProcessingImage::SlotDisplayImage));
   connect(&timerDisplayMiniLabels, &QTimer::timeout, this, &WidgetProcessingImage::SlotDisplayMiniLabels);
}

void WidgetProcessingImage::AddMiniLabel()
{
   auto Widget = reinterpret_cast<WidgetMiniLabelsGroup*>(LinkedWidget);
   auto Label = new LabelImage; Label->setFixedSize(100,100); Label->setStyleSheet(ui->labelImageDisplay->styleSheet()); Widget->AddLabel(Label);
}

void WidgetProcessingImage::moveEvent(QMoveEvent* event)
{
  if(LinkedWidget != 0) LinkedWidget->move(LinkedWidget->pos() + event->pos() - event->oldPos());
  QWidget::moveEvent(event);
}

void WidgetProcessingImage::SlotPosPressed(int x, int y)
{
 emit SignalPosPressed(QPair<float,float>(x,y));
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
FLAG_PAINT_TRAJECTORY = OnOff; if(FLAG_PAINT_TRAJECTORY) Trajectory.clear();
}

void WidgetProcessingImage::SlotDisplayMiniLabels()
{
   for(int n = 0; n < ImageSources.size(); n++)
   {
   ImageSources[n]->getImageToDisplay(DisplayImagesMini[n]);
   reinterpret_cast<WidgetMiniLabelsGroup*>(LinkedWidget)->SlotDisplayImage(DisplayImagesMini[n], n);
   }

}

void WidgetProcessingImage::SlotDisplayImage()
{
   if(!ImageSourceActive){ qDebug() << TAG_NAME << "SOURCE NOT LINKED";  return; };

   ImageSourceActive->getImageToDisplay(DisplayImage); if(DisplayImage.isNull()) return;

      Thinning(30)++;
   if(Thinning.isOpen()) 
   {
   auto FramePeriods = ImageSources[0]->getFramePeriod();
   strDisplayData = QString("ПЕРИОД: %1  ОБРАБОТКА: %2").arg(1/FramePeriods.first,5,'f',2).arg(FramePeriods.second,5,'f',2);
   this->SlotDisplayString(strDisplayData);
   }

   if(!timerDisplayMiniLabels.isActive()) { if(ImageSources.size() > 1) timerDisplayMiniLabels.start(100); }

   const auto& CoordsImage = ImageSourceActive->getPoints(); 
   const auto& RectsImage = ImageSourceActive->getRects(); 
   auto& InfoString = ImageSourceActive->getInfo();

   auto pen_it = pens.begin();
   QFont font("Ubunt Sans");
   font.setWeight(QFont::Thin);
   font.setPixelSize(8);
   QPainter paint;
            paint.begin(&DisplayImage);
            paint.setFont(font);

            for(auto& rect: RectsImage){paint.setPen(*pen_it); 
                                        paint.drawRect(rect); pen_it++;} pen_it = pens.begin();
                                                                         paint.setPen(pen1);
                                        paint.drawText(20,20,QString("%1 %2").arg(CoordsImage[0].first - 61).arg(CoordsImage[0].second - 62));
                                        paint.drawPoint(CoordsImage[0].first, 
                                                        CoordsImage[0].second);
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
   
   reinterpret_cast<WidgetMiniLabelsGroup*>(LinkedWidget)->SlotDisplayImage(Image, NumberActiveChannel);
}


void WidgetProcessingImage::LinkToModule(std::shared_ptr<ImageSourceInterface> Source)
{
   NumberActiveChannel++;
   ImageSourceActive = Source;
   ImageSources.push_back(Source);

   timerDisplay.start(30);
   //if(ImageSourceActive) QObject::disconnect(ImageSourceActive.get(),SIGNAL(signalNewImage()), this,SLOT(SlotDisplayImage()));
   //QObject::connect(ImageSourceActive.get(),SIGNAL(signalNewImage()), this,SLOT(SlotDisplayImage()),Qt::QueuedConnection);

}

void WidgetProcessingImage::SetName(QString name) {ui->labelName->setText(name);}; 

void WidgetProcessingImage::SlotDisplayString(QString InfoString)
{
   ui->labelDataDisplay->setText(InfoString);
}

void WidgetProcessingImage::SlotSetActiveChannel(int Channel)
{

                           if(Channel >= ImageSources.size() ||
                              Channel == NumberActiveChannel   ) return; 

 QObject::disconnect(ImageSourceActive.get(),SIGNAL(signalNewImage()), this,SLOT(SlotDisplayImage()));
                     ImageSourceActive = ImageSources[Channel]; 
    QObject::connect(ImageSourceActive.get(),SIGNAL(signalNewImage()), this,SLOT(SlotDisplayImage()),Qt::QueuedConnection);
    NumberActiveChannel = Channel;
    emit SignalChannelChanged(Channel);

    reinterpret_cast<WidgetMiniLabelsGroup*>(LinkedWidget)->SlotSetActiveChannel(Channel);

}

//=============================================================

void WidgetMiniLabelsGroup::SlotSetActiveChannel(int Number)
{
   if(Number == NumberChannel) return;
   Labels[NumberChannel]->setStyleSheet("border-width: 3px; border-color: rgb(100, 90, 90); border-radius: 2px; background-color: rgb(39, 39, 36)");
   NumberChannel = Number;
   Labels[NumberChannel]->setStyleSheet("border-width: 3px; border-color: rgb(120, 61, 27); border-radius: 2px; background-color: rgb(39, 39, 36)");
   emit SignalChannelChanged(Number); 
}

void WidgetMiniLabelsGroup::SlotDisplayImage(const QImage& Image, int Channel)
{
   if(Image.isNull()) return; Labels[Channel]->setPixmap(QPixmap::fromImage(Image.scaled(100,100)));
}


void WidgetMiniLabelsGroup::AddLabel(LabelImage* label) 
{ 

   this->layout()->addWidget(label);

   int Number = ++NumberChannel; 
   connect(label, &LabelImage::SignalPressedPos, [Number,this](){ SlotSetActiveChannel(Number);});

   if(!Labels.empty()) Labels.last()->setStyleSheet("border-width: 3px; border-color: rgb(100, 90, 90); border-radius: 2px; background-color: rgb(39, 39, 36)");
                               label->setStyleSheet("border-width: 3px; border-color: rgb(120, 61, 27); border-radius: 2px; background-color: rgb(39, 39, 36)");

   Labels.append(label);
}
