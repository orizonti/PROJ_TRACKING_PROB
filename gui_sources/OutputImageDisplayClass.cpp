#include "OutputImageDisplayClass.h"
#include <QPainter>

#define TAG "[ REMOTE DISP ]" 
OutputImageDisplayClass::OutputImageDisplayClass(QWidget *parent)
	: AdjustableWidget(parent)
{
	ui.setupUi(this);
  QObject::connect(&timerAutoClear,SIGNAL(timeout()),this,SLOT(SlotAutoClear()));

}

OutputImageDisplayClass::~OutputImageDisplayClass()
{
	qDebug() << "DELETE OTPUT_IMAGE_DISPLAY_CLASS";
}

void OutputImageDisplayClass::SlotPrintString(QString Command)
{

}

void OutputImageDisplayClass::SlotAutoClear() {}

void OutputImageDisplayClass::setAutoClear(int msec)
{
  if(msec == 0) { timerAutoClear.stop(); return; }
  timerAutoClear.start(msec);
}

void OutputImageDisplayClass::setDisplayTime(bool OnOff) { FLAG_DISPLAY_TIME = OnOff; }


void OutputImageDisplayClass::DisplayImage(QImage Image)
{
  ImageToDisplay = Image;
	ImageToDisplay = ImageToDisplay.scaled(1024-256, 256-64);
	ui.LabelImage->setPixmap(QPixmap::fromImage(Image)); 
}

void OutputImageDisplayClass::DisplayImage(uchar* Image, int width, int height)
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


void OutputImageDisplayClass::AppendColor(QColor color) 
{ 
  ColorList.append(color); 
  PenList.append(QPen(color)); PenList.last().setWidth(2);
}
void OutputImageDisplayClass::ResetColors() { ColorList.clear(); PenList.clear();}
void OutputImageDisplayClass::SetOutputSize(int width, int height) 
{
  ui.LabelImage->setFixedSize(width,height); 
  ImageWidth = width; 
  ImageHeight = height;
};

void OutputImageDisplayClass::DisplayString(QString str) 
{ 
  DisplayStringList.append(str); 
  if(DisplayStringList.size() > ColorList.size()) AppendColor(Qt::black);
}
void OutputImageDisplayClass::DisplayRect(QRect rect) 
{ 
  DisplayRectList.append(rect); 
  if(DisplayRectList.size() > ColorList.size()) AppendColor(Qt::black);
}
void OutputImageDisplayClass::DisplayLine(QLine line)
{
  DisplayLineList.append(line);
  if(DisplayLineList.size() > ColorList.size()) AppendColor(Qt::black);
}


