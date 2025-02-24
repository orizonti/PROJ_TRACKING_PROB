#ifndef REMOTE_DEVICE_WIDGET_h
#define REMOTE_DEVICE_WIDGET_h

#include <QWidget>
#include <QDebug>
#include "widget_adjustable.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetTransmissionControl; }
QT_END_NAMESPACE


class WidgetTransmissionControl : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetTransmissionControl(QWidget* parent = 0);
    WidgetTransmissionControl(QString Modulename, QWidget* parent = 0);

signals:

public  slots:

private:
Ui::WidgetTransmissionControl *ui;
};

#endif // REMOTE_DEVICE_WIDGET_H
