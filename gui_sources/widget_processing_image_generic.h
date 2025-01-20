#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "cv_image_processing.h"
#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
#include <QPainter>
#include <QLine>
#include "widget_adjustable.h"
#include "labelimage.h"
#include <QBrush>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetProcessingImage; }
QT_END_NAMESPACE


class WidgetProcessingImage : public WidgetAdjustable
{
    Q_OBJECT
public:
    WidgetProcessingImage(QString ModuleName = "ДИСПЛЕЙ", QWidget* parent = 0);
    QString TAG_NAME{"[ IMG_CTRL ]"};

void LinkToModule(std::shared_ptr<ImageSourceInterface> ImageSource);

int Counter = 0;
int ImageCounter = 0;
QRect  ObjectPosRect;
QRect  ObjectFindRect;
QLineF ObjectVelLine;
QLineF ArrowLeft;
QLineF ArrowRight;
QImage DisplayImage;
std::shared_ptr<ImageSourceInterface> ImageSource;

QPolygonF Trajectory;

void SetName(QString name); 
void CopyImageToDisplayImage(const QImage& Image);
bool FLAG_PAINT_TRAJECTORY = false;

QPen pen1{QBrush(Qt::green) ,2}; 
QPen pen2{QBrush(Qt::red)   ,2, Qt::DashLine}; 
QPen pen3{QBrush(Qt::yellow),2}; 
QPen pen4{QBrush(Qt::black) ,2}; 
QPen pen5{QBrush(Qt::white) ,2}; 
QPen pen6{QBrush(Qt::red) ,1}; 
std::vector<QPen> pens{pen1, pen2, pen4, pen4, pen4, pen4};

QString strFreq; 
QString strPeriodProcess; 
QString strDisplayData; 

signals:
void SignalPosPressed(QPair<double,double>);

public  slots:
void SlotDisplayImage();
void SlotDisplayImage(const QImage& Image);
void SlotDisplayString(QString InfoString);
void SlotStartPaintTrajectory(bool OnOff);
void SlotPosPressed(int x, int y);

private:
Ui::WidgetProcessingImage *ui;
};

#endif // MYWIDGET_H
