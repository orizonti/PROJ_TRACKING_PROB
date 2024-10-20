#include "labelimage.h"
#include "qevent.h"
#include "qdebug.h"

LabelImage::LabelImage(QWidget *parent)
	: AdjustableLabel(parent)
{
	
	this->setObjectName("ImageLabelDisplay");
}

LabelImage::~LabelImage()
{
}

void LabelImage::mouseMoveEvent(QMouseEvent * ev)
{
	
	double x = ev->x();
	double y = ev->y();
	qInfo()<<"[LABEL   ]" << "POS MOVE - " << x << y;

	emit SignalMovePos(x,y);
	
}

void LabelImage::mousePressEvent(QMouseEvent * ev)
{
	
	X_Pressed = ev->x()*2;
	Y_Pressed = ev->y()*2;
	qInfo() <<"[LABEL    ]"<< "POS PRESS - " << X_Pressed << Y_Pressed;

	emit SignalPressedPos(X_Pressed,Y_Pressed);
	
}


//void ImageProcessingDisplayWindow::DisplayAimingCoord(QVector<QPair<double, double> > Coords, QVector<QPair<double, double> > CoordsROI)
//{
//	QImage LabelImage = QImage(768,192,QImage::Format_RGB888);
//	LabelImage.fill(QColor(69, 75, 70));
//	QPen pen;
//	pen.setColor(Qt::red);
//	pen.setWidth(2);
//	QPainter paint;
//	auto RelativeCoord = Coords;
//	for(int n = 0; n < 3; n++)
//    {
//	   RelativeCoord[n] = RelativeCoord[n] - RelativeCoord[3];
//    }
//
//	int n = 1;
//	paint.begin(&LabelImage);
//	paint.setPen(pen);
//
//	QColor red = QColor(Qt::cyan);
//	QColor blue = QColor(Qt::yellow);
//	QColor green = QColor(Qt::green);
//	QColor red2 = QColor(Qt::red);
//	   
//	QVector<QColor> Colors; 
//
//	Colors.append(red);
//	Colors.append(blue);
//	Colors.append(green);
//	Colors.append(red2);
//	QVector<QColor>::Iterator ItColor = Colors.begin();
//
//	QString CoordText;
//	for (QPair<double, double> Coord : RelativeCoord)
//	{
//	paint.drawEllipse(Coord.first / 4, Coord.second / 4, 5, 5);
//	CoordText = QString("x - %1 y - %2").arg(Coord.first,0,'g',4).arg(Coord.second,0,'g',4);
//	paint.drawText(5,25*n+40, CoordText);
//
//	pen.setColor(*ItColor);
//	ItColor++;
//	paint.setPen(pen);
//	n++;
//	}
//
//	for (QPair<double, double> CoordROI : CoordsROI)
//	{
//	paint.drawRect(CoordROI.first / 4, CoordROI.second / 4, 256/4, 256/4);
//	}
//	paint.end();
//
//	ui.LabelStrobPosition->setPixmap(QPixmap::fromImage(LabelImage));
//}
