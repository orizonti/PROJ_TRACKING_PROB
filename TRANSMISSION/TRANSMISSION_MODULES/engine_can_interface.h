#ifndef CAN_ENGINE_INTERFACE_H
#define CAN_ENGINE_INTERFACE_H

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
#include <QCanBus>
#include <chrono>
#include <message_command_structures.h>
#include <engine_type_register.h>
#include "connection_interface.h"
#include "message_dispatcher_generic.h"

class CANTestMessage;
class CANDelayMeasure;

class CANEngineInterface : public ConnectionInterface
{
    Q_OBJECT
public:
  explicit CANEngineInterface(QString Device, QObject *parent = nullptr);
          ~CANEngineInterface();

public:

  bool isMessageAvailable();
  bool isConnected();

  QCanBusDevice *device = nullptr;
  void printAvailableDevices();

  CANTestMessage * engineSendTest = nullptr;
  CANDelayMeasure* engineDelayMeasure = nullptr;

  void connectTo(QString IDDeviceStr, int IDDevice = 0) {};
  void  listenTo(QString IDDeviceStr, int IDDevice = 0) {};
  void  listenTo(QHostAddress::SpecialAddress IPDevice, int Port) {};
  void tryConnectConstantly(QString address, int Port) {};
  //void linkTo(std::shared_ptr<DispatcherType> Dispatcher) { DispatcherLinked = Dispatcher; }  

public slots:
  void slotSendMessage(const QByteArray& ArrayCommand, uint16_t IDDevice = 0);
  void slotSendMessage(const char* DataCommand, int size, uint16_t IDDevice = 0);

  void slotCheckConnection();
  void slotCloseConnection(){};

private slots:
  void slotReadData();

private:
  //std::shared_ptr<DispatcherType> DispatcherLinked = nullptr;

signals:
  void signalMessageAvailable();
};


class CANTestMessage: public QObject
{
  Q_OBJECT
  public:
  CANTestMessage(QObject* parent = nullptr) : QObject(parent) 
  { connect(&timerSendTest,&QTimer::timeout, this, &CANTestMessage::slotSendTest); };

  void linkTo(CANEngineInterface* device) { deviceCAN = device; };
  CANEngineInterface* deviceCAN = nullptr;

  QTimer timerSendTest;
  CommandSetPosScanator Command;
  QByteArray message{(const char*)&Command, sizeof(Command)};

public slots:
  virtual void slotSendTest(){ deviceCAN->slotSendMessage(message, TypeRegister<CommandSetPosScanator>::GetTypeID()); }
  void slotStartSendTest(bool OnOff) {if(deviceCAN == nullptr) return; if(OnOff) timerSendTest.start(1000); else timerSendTest.stop(); }

};

class CANDelayMeasure: public CANTestMessage
{
  Q_OBJECT
  public:
  CANDelayMeasure(QObject* parent = nullptr) : CANTestMessage(parent) {};

  std::chrono::time_point<std::chrono::high_resolution_clock> timeStart;
  std::chrono::time_point<std::chrono::high_resolution_clock> timeEnd;
  std::chrono::microseconds Delay;

  void linkTo(CANEngineInterface* device) 
  { 
    connect(device,&CANEngineInterface::signalMessageAvailable, this, &CANDelayMeasure::slotMessageReceived); 
    deviceCAN = device; 
  };
  void slotSendTest() override 
  { 

    qDebug() << "[ CAN DELAY START ] ";
    timeStart = std::chrono::high_resolution_clock::now();
    CANTestMessage::slotSendTest();
  }

public slots:
  void slotMessageReceived()
  {
    if(!timerSendTest.isActive()) return;

    timeEnd = std::chrono::high_resolution_clock::now();
      Delay = std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart);
      qDebug() << "[ CAN DELAY ] " << Delay.count();
  }
};

#endif // CAN_ENGINE_INTERFACE_H
