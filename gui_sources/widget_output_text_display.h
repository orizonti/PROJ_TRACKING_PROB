#ifndef WidgetOutputTextDisplay_H
#define WidgetOutputTextDisplay_H

#include <QWidget>
#include "ui_widget_output_text_display.h"
#include "widget_adjustable.h"


class WidgetOutputTextDisplay : public WidgetAdjustable
{
	Q_OBJECT

public:
	WidgetOutputTextDisplay(QWidget *parent = Q_NULLPTR);
	~WidgetOutputTextDisplay();
	void SetLabel(QString Label) { ui.LabelBlockName->setText(Label);}
	void SetOutputSize(int width, int height) { ui.listMessages->setFixedSize(width,height); 
	                                                       this->setFixedSize(width+16,height+60);
                                                         }

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
#endif //WidgetOutputTextDisplay_H
