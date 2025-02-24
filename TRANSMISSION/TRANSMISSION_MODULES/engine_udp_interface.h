#ifndef UDPEngineInterface_H
#define UDPEngineInterface_H

#include <QObject>
#include <QDebug>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTextStream>
#include <QAbstractSocket>
#include <QProcess>
#include <QFile>

#include "engine_ring_buffer_generic.h"
#include "message_struct_generic.h"

class UDPEngineInterface : public QObject
{
    Q_OBJECT
public:
    explicit UDPEngineInterface(QString remote_address,int remote_port, QObject *parent = nullptr);
    ~UDPEngineInterface();

    void SetDevice(QString address, int Port) {RemotePort = Port; RemoteHost = address;};
    void BindTo(QString address, int Port);
    void BindTo(QHostAddress::SpecialAddress address, int Port);
    void InitBuffer();

    bool isConnected() {return Connected;};

    QUdpSocket* ControlSocket;

    int LocalPort = 7575;
    int RemotePort = 7575;
    QString RemoteHost;

    RingBufferGeneric<MESSAGE_HEADER,50, 1000,IteratorMode::Continous>* RingBuffer = 0;

    bool Connected = false;
public slots:
   void SlotSendCommand(QByteArray ArrayCommand);
   void SlotCheckConnection();
signals:
   void SignalDeviceConnected();
public:
private slots:
    void SlotReadData();
    void SendendMessage();
};
#endif // UDPEngineInterface_H
