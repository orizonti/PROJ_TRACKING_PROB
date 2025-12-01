#ifndef IMITATOR_WIDGET_H
#define IMITATOR_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
#include <QPainter>
#include <QLine>
#include "widget_adjustable.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetImageImitator; }
QT_END_NAMESPACE


class WidgetImageImitator : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetImageImitator(QWidget* parent = 0);

    std::shared_ptr<AimImageImitatorClass> ImitationModule;

void linkToModule(std::shared_ptr<AimImageImitatorClass> Imitator);


public  slots:
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
Ui::WidgetImageImitator *ui;
};

#endif // IMITATOR_WIDGET_H
