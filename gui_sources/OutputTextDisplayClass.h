#ifndef OUTPUTTEXTDISPLAYCLASS_H
#define OUTPUTTEXTDISPLAYCLASS_H

#include <QWidget>
#include "ui_OutputTextDisplay.h"
#include "AdjustableWidget.h"


class OutputTextDisplayClass : public AdjustableWidget
{
	Q_OBJECT

public:
	OutputTextDisplayClass(QWidget *parent = Q_NULLPTR);
	~OutputTextDisplayClass();
	void SetLabel(QString Label) { ui.LabelBlockName->setText(Label);}
	void SetOutputSize(int width, int height) { ui.listMessages->setFixedSize(width,height); 
	                                                       this->setFixedSize(width+10,height+10);}

  QString OutputString;
  bool FLAG_DISPLAY_TIME = false;
  QTimer timerAutoClear;
  void setAutoClear(int msec);
  void setDisplayTime(bool OnOff);
public slots:
    void SlotPrintString(QString Command);
    void SlotAutoClear();

private:
	Ui::OutputTextDisplay ui;
};
#endif //OUTPUTTEXTDISPLAYCLASS_H
