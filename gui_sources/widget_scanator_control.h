#ifndef REMOTE_DEVICE_WIDGET_h
#define REMOTE_DEVICE_WIDGET_h

#include <QWidget>
#include <QDebug>
#include "widget_adjustable.h"
#include "scanator_control_class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetScanatorControl; }
QT_END_NAMESPACE


class WidgetScanatorControl : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetScanatorControl(QWidget* parent = 0);
    WidgetScanatorControl(QString Modulename, QWidget* parent = 0);

    void LinkToDevice(std::shared_ptr<ScanatorControlClass> EngineModule);

    std::shared_ptr<ScanatorControlClass> ScanatorDevice;

    QTimer timerUpdateState;

    QString numPosX;
    QString numPosY;
    QString numPosXReal;
    QString numPosYReal;
    QString numVelX;
    QString numVelY;

signals:

public  slots:
  void SlotDisplayScanatorState();

private:
Ui::WidgetScanatorControl *ui;
};

#endif // REMOTE_DEVICE_WIDGET_H
