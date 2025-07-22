#ifndef TCPCAMERASERVER_H
#define TCPCAMERASERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
//#include "message_struct_generic.h"
//#include "RingBufferengine.h"
#include <QImage>
#include <QElapsedTimer>

#include <QMutex>
#include <QThread>
#include <QByteArray>
#include <QBuffer>
#include <QTimer>
#include "engine_ring_buffer_generic.h"
#include "message_struct_generic.h"
#include "message_command_structures.h"
#include "message_dispatcher_generic.h"


void DispatchMessage1(ControlMessage1* message); 
void DispatchMessage2(ControlMessage2* message);
void DispatchMessage3(ControlMessage3* message);
void DispatchMessage4(ControlMessage4* message);
void DispatchMessage5(ControlMessage5* message);
void DispatchMessage6(ControlMessage6* message);
extern MessageListDispatcher List;

class TCPConnectionEngine : public QObject
{
    Q_OBJECT
public:
    explicit TCPConnectionEngine(QObject *parent = nullptr);
             TCPConnectionEngine(QString address, int Port, QObject *parent = nullptr);
    ~TCPConnectionEngine();

protected:
QTcpSocket* engineSocket = 0;
QTcpServer* Server = 0;
QTimer TimerAutoconnection;
    int PORT = 2325;
QString HOST;

int MESSAGE_WAIT_COUNT = 0;
public:
virtual bool isMessageAvailable() {return RingBuffer->isMessageAvailable();};
//virtual QByteArray TakeMessageData() { return RingBuffer->TakeMessageData();};

void ResetSocket();
bool IsConnected();

//RingBufferMessageGeneric<MESSAGE_HEADER,50,100>* RingBuffer = 0;

//RingBufferMessageGenericFixed<MESSAGE_HEADER,sizeof(ControlMessage5) + sizeof(MESSAGE_HEADER),
//                                                           100,IteratorMode::Chunked>* RingBuffer = 0;

RingBufferGeneric<MESSAGE_HEADER,50, 1000,IteratorMode::Continous>* RingBuffer = 0;
//RingBufferMessageGenericFixed<MESSAGE_HEADER,50, 100,IteratorMode::Continous>* RingBuffer = 0;
//RingBufferengine<MESSAGE_HEADER,50,100>* RingBuffer = 0;
MessageDispatcherGenericClass<MessageListDispatcher> MessageDispatcher;

public slots:
virtual void SlotReadData();

public slots:
void ConnectToDevice(QString address, int Port);
void ConnectToDevice(QHostAddress::SpecialAddress address, int Port);

void WaitConnectionOn(QString address, int Port);
void WaitConnectionOn(QHostAddress::SpecialAddress address, int Port);
void TryToConnectConstantly(QString address, int Port);


void SlotSendMessage(QByteArray Command);
void SlotCloseConnection();

void SlotConnectedToHost();
void SlotAcceptConnection();
void SlotConnectionAttempt();
void SendendCommand();

signals:
void DeviceConnected();
void SignalMessageAvailable();
};


#endif // TCPCAMERASERVER_H
