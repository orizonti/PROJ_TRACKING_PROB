#pragma once
#include <QWidget>
#include <QDebug>
#include <QString>
#include <QDoubleSpinBox>
#include <QThread>
#include <QTimer>
#include <thread>
#include <math.h>
#include <QFile>
#include <QTextStream>
#include "LaserCommandStructures.h"
#include "widget_adjustable.h"


QT_BEGIN_NAMESPACE
namespace Ui { class WindowLaserControlClass; }
QT_END_NAMESPACE

class WindowLaserControlClass;
class TCPControlInterface;

class LaserControlClass : public QObject
{
    Q_OBJECT
    public: 
    LaserControlClass();
    ~LaserControlClass();
    WindowLaserControlClass* WindowControl = 0;
    void linkToWindow(WindowLaserControlClass* Window);

	std::string TAG_NAME{"[ LASER ]"};
    ControlCommandLaser ControlData;
    qint32 IP_PORT = 2325;
    QString IP_DEVICE = "172.16.16.16";

    bool ConnectedToDevice = false;

    public slots:
    void slotSetPower(int Power);
    void slotStartWork(bool enable);
    void SlotLaserStateChanged(LaserStateStruct state);

    void SlotConnectDevice();
    void SlotDisconnect(bool);
    void SlotDeviceConnected();
    void SlotDeviceDisconnected();
    void LoadINI();

    private:
    TCPControlInterface* DeviceInterface;
};

class WindowLaserControlClass : public WidgetAdjustable
{
    Q_OBJECT

public:
    WindowLaserControlClass(QWidget* parent = nullptr);
    ~WindowLaserControlClass();
    void linkToModule(LaserControlClass* LaserControl);
private slots:
private:
    Ui::WindowLaserControlClass *ui;
};
