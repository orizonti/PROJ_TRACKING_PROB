#ifndef TCP_ENGINE_INTERFACE_H
#define TCP_ENGINE_INTERFACE_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QImage>
#include <QElapsedTimer>

#include <QMutex>
#include <QThread>
#include <QByteArray>
#include <QBuffer>
#include <QTimer>
#include "engine_ring_buffer_generic.h"
#include "connection_interface.h"
#include "message_dispatcher_generic.h"
#include "debug_output_filter.h"


class TCPConnectionEngine : public ConnectionInterface
{
    Q_OBJECT
public:
  explicit TCPConnectionEngine(QObject *parent = nullptr);
          ~TCPConnectionEngine();
  using BufferType     = RingBufferGeneric<MESSAGE_HEADER_GENERIC,50, 1000,IteratorMode::Continous>; 
  using DispatcherType = MessageDispatcher<MESSAGE_HEADER_GENERIC, BufferType>; 
  using HeaderType     = MESSAGE_HEADER_GENERIC;

public:
  bool isMessageAvailable() override;
  bool isConnected() override;
  void connectTo(QString IPDevice, int Port) override;
  void  listenTo(QString IPDevice, int Port) override;
  void  listenTo(QHostAddress::SpecialAddress IPDevice, int Port) override;
  void tryConnectConstantly(QString Address, int Port) override;

public slots:
  void slotSendMessage(const QByteArray& Command, uint16_t Param = 0) override;
  void slotSendMessage(const char* DataCommand, int size, uint16_t Param = 0) override;

  void slotCheckConnection() override;
  void slotCloseConnection() override;

private slots:
  void slotReadData();
  void slotConnectedToHost();
  void slotConnectionAttempt();
  void slotAcceptConnection();

private:
  QTcpSocket* Socket = 0;
  QTcpServer* Server = 0;
  QTimer      TimerAutoconnection;
//=================================================================
public:
    QString IPRemote = "127.0.0.1";
        int PortRemote = 2325;
    QString IPLocal = "127.0.0.1";
        int PortLocal = 2325;
    BufferType* RingBuffer = nullptr;
    DispatcherType* Dispatcher = nullptr;

  signals:
  void signalDeviceConnected();
  void signalMessageAvailable();
};


#endif // TCP_ENGINE_INTERFACE_H
