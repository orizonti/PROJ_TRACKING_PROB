#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "CVImageProcessing.h"
#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include "AIM_IMAGE_IMITATION/AimImageImitatorClass.h"
#include <QPainter>
#include <QLine>
#include "AdjustableWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DisplayWidget; }
QT_END_NAMESPACE


class DisplayWidget : public AdjustableWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget* parent = 0);

    AimImageImitatorClass* ImitationModule = 0;

void LinkToImitator(AimImageImitatorClass* Imitator);
void LinkToImageSource(ImageSourceInterface* ImageSource);
void LinkToProcessModule(CVImageProcessing* Processor);
int Counter = 0;
int ImageCounter = 0;
QRect ObjectPosRect;
QRect ObjectFindRect;
QLineF ObjectVelLine;
QLineF ArrowLeft;
QLineF ArrowRight;
QImage DisplayImage;

private:

signals:

public  slots:
void SlotDisplayImage(QImage Image);
void SlotDisplayObjectPos(cv::Point ObjectPos, std::pair<float,float> ObjectVel, cv::Rect ROIRect);
void on_butMoveRight_pressed();
void on_butMoveLeft_pressed();
void on_butMoveUp_pressed();
void on_butMoveDown_pressed();

void on_butMoveRight_released();
void on_butMoveLeft_released();
void on_butMoveUp_released();
void on_butMoveDown_released();

void on_butStartAimMoving_clicked();
void on_butStopAimMoving_clicked();

private:
Ui::DisplayWidget *ui;
};

#endif // MYWIDGET_H
