#ifndef LABELIMAGE_H
#define LABELIMAGE_H
#include "widget_adjustable.h"


class LabelImage : public AdjustableLabel
{
	Q_OBJECT

public:
	LabelImage(QWidget *parent = 0);
	~LabelImage();
  QPair<float,float> PosPressed{0.5,0.5};
  QPair<float,float> PosPressedAbs{0,0};

  protected:
	void mousePressEvent(QMouseEvent * ev);
	void mouseMoveEvent(QMouseEvent * ev);

signals:
	void signalPosPressed(QPair<float,float>);
	void signalPosPressed2(QPair<float,float>);
};

#endif // LABELIMAGE_H
