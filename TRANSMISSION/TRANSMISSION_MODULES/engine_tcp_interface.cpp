#include "engine_tcp_interface.h"
#include "engine_type_register.h"
#include "message_command_structures.h"
#include "message_struct_generic_ext.h"
#include "engine_ring_buffer_generic.h"
#include <type_traits>
#include <message_dispatcher_generic.h>

using MessageVoid = MessageGeneric<void*, MESSAGE_HEADER_GENERIC>;
TCPConnectionEngine::TCPConnectionEngine(QObject *parent)
    : ConnectionInterface(parent)
{
     RingBuffer = new BufferType;
     Dispatcher = new MessageDispatcher<MESSAGE_HEADER_GENERIC, BufferType>;
     *RingBuffer | *Dispatcher;
}

TCPConnectionEngine::~TCPConnectionEngine() 
{ 
    qDebug() << "DELETE SERVER ENGINE";
    if(RingBuffer != 0) delete RingBuffer;
    if(Server != 0) delete Server;
}

//================================================================
void TCPConnectionEngine::connectTo(QString address, int Port)
{
 IPRemote = address; PortRemote = Port; qDebug() << "CONNECT TO DEVICE : " << IPRemote << PortRemote;

         if(Socket == 0) Socket = new QTcpSocket(this);
    connect(Socket, SIGNAL(connected()), this, SLOT(SlotConnectedToHost()),Qt::QueuedConnection);
    connect(Socket, SIGNAL(readyRead()), this, SLOT(SlotReadData()),Qt::QueuedConnection);

 QHostAddress host = QHostAddress(address);
 Socket->connectToHost(host, Port, QIODevice::ReadWrite);
}



void TCPConnectionEngine::slotReadData()
{
   if(Socket->bytesAvailable() < RingBuffer->MIN_MESSAGE_SIZE) return;

   int bytes_available = Socket->bytesAvailable();

      RingBuffer->AppendData((uint8_t*)Socket->readAll().data(), bytes_available);
   if(RingBuffer->isMessageAvailable()) { emit signalMessageAvailable(); }

   if(Socket->bytesAvailable() > 24) slotReadData();
}


void TCPConnectionEngine::slotSendMessage(const QByteArray& Command, uint8_t Param)
{
   qDebug() << OutputFilter::Filter(1000) << "SEND COMMAND: " << QString(Command.toHex());

   if(!Socket) return;
   if(!Socket->isOpen()) return; Socket->write(Command);
}

void TCPConnectionEngine::slotSendMessage(const char* DataCommand, int size, uint8_t Param)
{
   if(!Socket) return;
   if(!Socket->isOpen()) return; Socket->write(DataCommand,size);
}



bool TCPConnectionEngine::isConnected()
{
    if(Socket == 0)      return false;
    if(Socket->isOpen()) return true;

    return true;
}

void TCPConnectionEngine::tryConnectConstantly(QString Address, int Port)
{
   IPRemote = Address; PortRemote = Port;
   QObject::connect(&TimerAutoconnection,&QTimer::timeout,this,&TCPConnectionEngine::slotConnectionAttempt);
   TimerAutoconnection.start(2000);
}

void TCPConnectionEngine::slotConnectionAttempt()
{
  //qDebug() << "TRY TO CONNECT: " << IPRemote << PortRemote;
 if(Socket == 0)
 {
    Socket = new QTcpSocket(this);
    connect(Socket, SIGNAL(connected()), this, SLOT(SlotConnectedToHost()),Qt::QueuedConnection);
 }

 if(Socket->isOpen()) return;

 QHostAddress host = QHostAddress(IPRemote);
 Socket->connectToHost(host, PortRemote, QIODevice::ReadWrite);

 bool connected = Socket->waitForConnected(5);

 if(connected)
 {
   qDebug() << "CONNECTED TO HOST " << IPRemote << PortRemote;
   connect(Socket, SIGNAL(readyRead()), this, SLOT(SlotReadData()),Qt::QueuedConnection);
   TimerAutoconnection.stop();
   QObject::disconnect(&TimerAutoconnection,&QTimer::timeout,this,&TCPConnectionEngine::slotConnectionAttempt);
   return;
 }

 Socket->close();
}

void TCPConnectionEngine::listenTo(QString address, int Port)
{

  IPRemote = address; PortRemote = Port; if(Socket != 0) delete Socket; 

  Server = new QTcpServer;
  Server->listen(QHostAddress(address),PortRemote);
  qDebug() << "WAIT DEVICE AT ADDRESS : " << address << " PORT: " << Port << Server->isListening();
  connect(Server, SIGNAL(newConnection()), this, SLOT(SlotAcceptConnection()),Qt::QueuedConnection);
}

void TCPConnectionEngine::listenTo(QHostAddress::SpecialAddress address, int Port)
{
  qDebug() << "WAIT DEVICE AT ADDRESS : " << address << " PORT: " << Port;

  PortRemote = Port; if(Socket != 0) delete Socket; 

  Server = new QTcpServer; Server->listen(address,PortRemote);
  connect(Server, SIGNAL(newConnection()), this, SLOT(SlotAcceptConnection()),Qt::QueuedConnection);
}

void TCPConnectionEngine::slotCloseConnection()
{
   slotCloseConnection();
   Socket->flush();
   Socket->disconnectFromHost();
   Socket->close();
}

void TCPConnectionEngine::slotCheckConnection() { }

void TCPConnectionEngine::slotAcceptConnection()
{
  Socket = Server->nextPendingConnection();
  connect(Socket, SIGNAL(readyRead()), this, SLOT(slotReadData()),Qt::QueuedConnection);
  disconnect(Server, SIGNAL(newConnection()), this, SLOT(slotAcceptConnection()));

  qDebug() << "ACCEPT CONNECTION DEVICE: " << Socket->peerAddress();
}

void TCPConnectionEngine::slotConnectedToHost()
{
    emit signalDeviceConnected(); qDebug() << "CONNECTED TO HOST: " << IPRemote << PortRemote; 
}

bool TCPConnectionEngine::isMessageAvailable() { return RingBuffer->isMessageAvailable(); }