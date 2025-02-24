#ifndef LABELIMAGE_H
#define LABELIMAGE_H
#include "widget_adjustable.h"


class LabelImage : public AdjustableLabel
{
	Q_OBJECT

public:
	LabelImage(QWidget *parent = 0);
	~LabelImage();
	int X_Pressed = 0;
	int Y_Pressed = 0;
	std::pair<int,int> SizeImage;

protected:
	void mousePressEvent(QMouseEvent * ev);
	void mouseMoveEvent(QMouseEvent * ev);


signals:
	void SignalPressedPos(int,int);
	void SignalMovePos(int,int);
	
};

#endif // LABELIMAGE_H
