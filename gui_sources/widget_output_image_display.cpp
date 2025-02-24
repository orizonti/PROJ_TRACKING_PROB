#include "widget_output_image_display.h"
#include <QPainter>

#define TAG "[ REMOTE DISP ]" 
WidgetOutputImageDisplay::WidgetOutputImageDisplay(QWidget *parent)
	: WidgetAdjustable(parent)
{
	ui.setupUi(this);
  QObject::connect(&timerAutoClear,SIGNAL(timeout()),this,SLOT(SlotAutoClear()));

}

WidgetOutputImageDisplay::~WidgetOutputImageDisplay()
{
	qDebug() << "DELETE OTPUT_IMAGE_DISPLAY_CLASS";
}

void WidgetOutputImageDisplay::SlotPrintString(QString Command)
{

}

void WidgetOutputImageDisplay::SlotAutoClear() {}

void WidgetOutputImageDisplay::setAutoClear(int msec)
{
  if(msec == 0) { timerAutoClear.stop(); return; }
  timerAutoClear.start(msec);
}

void WidgetOutputImageDisplay::setDisplayTime(bool OnOff) { FLAG_DISPLAY_TIME = OnOff; }


void WidgetOutputImageDisplay::DisplayImage(QImage Image)
{
  ImageToDisplay = Image;
	ImageToDisplay = ImageToDisplay.scaled(1024-256, 256-64);
	ui.LabelImage->setPixmap(QPixmap::fromImage(Image)); 
}

void WidgetOutputImageDisplay::DisplayImage(uchar* Image, int width, int height)
{
	ImageToDisplay = QImage((const uchar*)Image, width, height, width, QImage::Format_Grayscale8);
	ImageToDisplay = ImageToDisplay.scaled(512, 128);

  QPainter paint;
  paint.begin(&ImageToDisplay);

  //for(int n = 0; n < DisplayLineList.size(); n++)
  //{
  //paint.setPen(PenList[n]);
  //paint.drawLine(DisplayLineList[n]);
  //}

  for(int n = 0; n < DisplayRectList.size(); n++)
  {
  paint.setPen(PenList[n]);
  paint.drawRect(DisplayRectList[n]);
  }
  
  paint.end();


	ui.LabelImage->setPixmap(QPixmap::fromImage(ImageToDisplay)); 
}


void WidgetOutputImageDisplay::AppendColor(QColor color) 
{ 
  ColorList.append(color); 
  PenList.append(QPen(color)); PenList.last().setWidth(2);
}
void WidgetOutputImageDisplay::ResetColors() { ColorList.clear(); PenList.clear();}
void WidgetOutputImageDisplay::SetOutputSize(int width, int height) 
{
  ui.LabelImage->setFixedSize(width,height); 
  ImageWidth = width; 
  ImageHeight = height;
};

void WidgetOutputImageDisplay::DisplayString(QString str) 
{ 
  DisplayStringList.append(str); 
  if(DisplayStringList.size() > ColorList.size()) AppendColor(Qt::black);
}
void WidgetOutputImageDisplay::DisplayRect(QRect rect) 
{ 
  DisplayRectList.append(rect); 
  if(DisplayRectList.size() > ColorList.size()) AppendColor(Qt::black);
}
void WidgetOutputImageDisplay::DisplayLine(QLine line)
{
  DisplayLineList.append(line);
  if(DisplayLineList.size() > ColorList.size()) AppendColor(Qt::black);
}


