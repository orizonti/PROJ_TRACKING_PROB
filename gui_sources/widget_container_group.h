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
    int NumberChannels = 0;

public  slots:
    void SlotSetChannel(int Channel);
signals:
    void SignalChannelChanged(int Channel);

private:
Ui::WidgetContainerGroup *ui;
};

#endif // CONTAINTER_WIDGET_H
