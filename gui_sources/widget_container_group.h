#ifndef CONTAINER_WIDGET_H
#define CONTAINER_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QLine>
#include "CAMERA_INTERFACE/camera_interface_class.h"
#include "widget_adjustable.h"
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetContainerGroup; }
QT_END_NAMESPACE


class WidgetContainerGroup : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetContainerGroup(QWidget* parent = 0);

    void AddWidget(WidgetAdjustable& Widget);
    int NumberChannel = -1;

public  slots:
    void SlotSetActiveChannel(int Number);
signals:
    void SignalChannelChanged(int);



private:
QVector<QPushButton*> buttons;
Ui::WidgetContainerGroup *ui;
};

#endif // CONTAINTER_WIDGET_H
