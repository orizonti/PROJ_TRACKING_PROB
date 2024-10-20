#include "OutputTextDisplayClass.h"
#include <QTime>

#define TAG "[ REMOTE DISP ]" 
OutputTextDisplayClass::OutputTextDisplayClass(QWidget *parent)
	: AdjustableWidget(parent)
{
	ui.setupUi(this);
  QObject::connect(&timerAutoClear,SIGNAL(timeout()),this,SLOT(SlotAutoClear()));
}

OutputTextDisplayClass::~OutputTextDisplayClass()
{
	qDebug() << "DELETE OTPUT_TEXT_DISPLAY_CLASS";
}

void OutputTextDisplayClass::SlotPrintString(QString Command)
{
  if(Command.isEmpty()) return;
  if(Command.contains("NO")) return;

  OutputString = Command.replace("\r\n","");
  if(FLAG_DISPLAY_TIME) OutputString = QTime::currentTime().toString() + ": " + OutputString;

  ui.listMessages->addItem(OutputString);
  if(ui.listMessages->count() > 10) ui.listMessages->clear();
}

void OutputTextDisplayClass::SlotAutoClear() { ui.listMessages->clear(); }

void OutputTextDisplayClass::setAutoClear(int msec)
{
  if(msec == 0) { timerAutoClear.stop(); return; }
  timerAutoClear.start(msec);
}

void OutputTextDisplayClass::setDisplayTime(bool OnOff) { FLAG_DISPLAY_TIME = OnOff; }

