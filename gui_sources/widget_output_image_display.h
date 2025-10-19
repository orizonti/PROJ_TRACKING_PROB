#ifndef widget_output_image_display_H
#define widget_output_image_display_H

#include <QWidget>
#include "ui_widget_output_image_display.h"
#include "widget_adjustable.h"
#include <QImage>


class WidgetOutputImageDisplay : public WidgetAdjustable
{
	Q_OBJECT

public:
	WidgetOutputImageDisplay(QWidget *parent = Q_NULLPTR);
	~WidgetOutputImageDisplay();
  void SetOutputSize(int width, int height);
  int ImageWidth = 712; 
  int ImageHeight = 192;

  bool FLAG_DISPLAY_TIME = false;
  QTimer timerAutoClear;
  void setAutoClear(int msec);
  void setDisplayTime(bool OnOff);

  QStringList    DisplayStringList;
  QVector<QRect> DisplayRectList;
  QVector<QLine> DisplayLineList;

  QVector<QColor> ColorList;
  QVector<QPen> PenList;
  QImage ImageToDisplay;
public slots:
    void SlotPrintString(QString Command);
    void SlotAutoClear();
    void DisplayImage(QImage Image);
    void DisplayImage(uchar* Image, int width, int height);
    void AppendColor(QColor color);
    void ResetColors();

    void DisplayString(QString str);
    void DisplayRect(QRect rect);
    void DisplayLine(QLine line);
private:
	Ui::WidgetOutputImageDisplay ui;
};
#endif //WidgetOutputImageDisplay
