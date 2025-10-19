#include "widget_output_text_display.h"
#include <QTime>

#define TAG "[ REMOTE DISP ]" 
WidgetOutputTextDisplay::WidgetOutputTextDisplay(QWidget *parent)
	: WidgetAdjustable(parent)
{
	ui.setupUi(this);
  QObject::connect(&timerAutoClear,SIGNAL(timeout()),this,SLOT(SlotAutoClear()));
}

WidgetOutputTextDisplay::~WidgetOutputTextDisplay()
{
	qDebug() << "DELETE OTPUT_TEXT_DISPLAY_CLASS";
}

void WidgetOutputTextDisplay::SlotPrintString(QString Command)
{
  if(Command.isEmpty()) return;
  if(Command.contains("NO")) return;

  OutputString = Command.replace("\r\n","");
  if(FLAG_DISPLAY_TIME) OutputString = QTime::currentTime().toString() + ": " + OutputString;

  ui.listMessages->addItem(OutputString);
  if(ui.listMessages->count() > 10) ui.listMessages->clear();
}

void WidgetOutputTextDisplay::SlotAutoClear() { ui.listMessages->clear(); }

void WidgetOutputTextDisplay::setAutoClear(int msec)
{
  if(msec == 0) { timerAutoClear.stop(); return; }
  timerAutoClear.start(msec);
}

void WidgetOutputTextDisplay::setDisplayTime(bool OnOff) { FLAG_DISPLAY_TIME = OnOff; }

