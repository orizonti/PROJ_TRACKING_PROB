#ifndef ENGINE_UDP_INTERFACE_H
#define ENGINE_UDP_INTERFACE_H

#include <QDebug>

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTextStream>
#include <QAbstractSocket>
#include <QProcess>
#include <QFile>

#include <connection_interface.h>
#include <QTimer>

#include "engine_ring_buffer_generic.h"
#include "message_command_structures.h"

//class ConnectionInterface : public QObject
//{
//  //Q_OBJECT
//  public:
//  ConnectionInterface(QObject* parent = nullptr) : QObject(parent) {}; 
//  virtual bool isMessageAvailable() = 0;
//  virtual bool isConnected() = 0;
//  virtual void connectTo(QString IPDevice, int Port) = 0;
//  virtual void  listenTo(QString IPDevice, int Port) = 0;
//  virtual void  listenTo(QHostAddress::SpecialAddress IPDevice, int Port) = 0;
//  virtual void tryConnectConstantly(QString address, int Port) = 0;
//
//  public slots:
//  virtual void slotSendMessage(const QByteArray& ArrayCommand, uint8_t Param = 0) = 0;
//  virtual void slotSendMessage(const char* DataCommand, int size, uint8_t Param = 0) = 0 ;
//
//
//};

class UDPEngineInterface : public ConnectionInterface
{
    Q_OBJECT
public:
    explicit UDPEngineInterface(QString IPDevice, int Port,QString IPListen, int PortLocal, QObject *parent = nullptr);
             UDPEngineInterface(QString IPDevice, int Port,QHostAddress::SpecialAddress IPListen, int PortLocal, QObject *parent = nullptr);
    ~UDPEngineInterface();

    bool isMessageAvailable() override;
    bool isConnected() override;
    void connectTo(QString IPDevice, int Port) override;
    void  listenTo(QString IPHost, int Port) override;
    void  listenTo(QHostAddress::SpecialAddress IPHost, int Port) override;
    void tryConnectConstantly(QString Address, int Port) override;
    int DataCounter = 0;

public slots:
  void slotSendMessage(const QByteArray& ArrayCommand, uint8_t Param = 0) override;
  void slotSendMessage(const char* DataCommand, int size, uint8_t Param = 0) override;

  void slotCheckConnection() override;
  void slotCloseConnection() override;

private:
   QUdpSocket* Socket = nullptr;
   QTimer timerConnectAttempt;

private slots:
   void slotReadData();

public: 
    QString IPRemote;
        int PortRemote = 7575;
        int PortLocal  = 7575;
       bool Connected  = false;

RingBufferGeneric<MESSAGE_HEADER_GENERIC,sizeof(MessageDevice<0>), 4,IteratorMode::Continous>* RingBuffer = nullptr;

signals:
void SignalDeviceConnected();
void signalMessageAvailable();

};



#endif // UDPEngineInterface_H
