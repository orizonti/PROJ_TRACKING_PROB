#include "engine_udp_interface.h"
#include "connection_interface.h"
#include <qhostaddress.h>



UDPEngineInterface::UDPEngineInterface(QString IPDevice, int Port, QString IPListen, int PortLocal, QObject *parent)
    : ConnectionInterface{parent}
{
    connectTo(IPDevice, Port); 
    listenTo(IPListen, PortLocal);

    RingBuffer = new std::remove_reference<decltype((*RingBuffer))>::type;
    Dispatcher = new DispatcherType;

    QObject::connect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));

    qDebug() << "[ UDP CONNECTION ]   SEND TO:" << IPDevice << Port ;
    qDebug() << "[ UDP CONNECTION ] LISTEN TO:" << IPListen << PortLocal;
}

UDPEngineInterface::UDPEngineInterface(QString IPDevice, int Port,QHostAddress::SpecialAddress IPListen, int PortLocal, QObject *parent)
 : ConnectionInterface(parent)
{
    connectTo(IPDevice, Port); 
    listenTo(IPListen, PortLocal);

    RingBuffer = new std::remove_reference<decltype((*RingBuffer))>::type;
    QObject::connect(Socket,SIGNAL(readyRead()),this,SLOT(slotReadData()));

    qDebug() << "[ UDP CONNECTION ]   SEND TO:" << IPDevice << Port ;
    qDebug() << "[ UDP CONNECTION ] LISTEN TO:" << IPListen << PortLocal;
}

void UDPEngineInterface::connectTo(QString IPDevice, int Port) 
{
    if(isConnected()) slotCloseConnection();

    if(Socket == nullptr) { Socket = new QUdpSocket(this); 
                            Socket->open(QIODevice::ReadWrite); }

    PortRemote = Port; IPRemote = IPDevice;
};

void  UDPEngineInterface::listenTo(QString IPHost, int Port)
{
    if(Socket == nullptr) { Socket = new QUdpSocket(this); 
                            Socket->open(QIODevice::ReadWrite); }
       Socket->bind(QHostAddress(IPHost), Port); 
       Socket->flush();
}
void  UDPEngineInterface::listenTo(QHostAddress::SpecialAddress IPHost, int Port)
{
    if(Socket == nullptr) { Socket = new QUdpSocket(this); 
                            Socket->open(QIODevice::ReadWrite); }
       //Socket->bind(IPHost, Port); 
       Socket->bind(QHostAddress::Any, 1313); 
       Socket->flush();
}

UDPEngineInterface::~UDPEngineInterface()
{
 qDebug() << "DELETE UDP INTERFACE";
 delete Socket;
}

void UDPEngineInterface::slotReadData()
{
   if(Socket->bytesAvailable() < RingBuffer->MIN_MESSAGE_SIZE) return;

      auto Datagram = Socket->receiveDatagram();
      DataCounter += Datagram.data().size();             //qDebug() << "READ DATA : " << DataCounter;

      RingBuffer->AppendData((uint8_t*)Datagram.data().data(), Datagram.data().size());

   if(Socket->bytesAvailable() > 24) slotReadData(); else *RingBuffer | *Dispatcher;

   //if(RingBuffer->isMessageAvailable()) emit signalMessageAvailable();
}


void UDPEngineInterface::slotSendMessage(const QByteArray& ArrayCommand, uint16_t Param)
{
 Socket->writeDatagram(ArrayCommand,QHostAddress(IPRemote),PortRemote); 
 //Socket->waitForBytesWritten(5);
}

void UDPEngineInterface::slotSendMessage(const char* DataCommand, int size, uint16_t Param)
{
    Socket->writeDatagram(DataCommand,size,QHostAddress(IPRemote), PortRemote);
}



void UDPEngineInterface::slotCloseConnection()
{
    Connected = false; Socket->close();
}


bool UDPEngineInterface::isConnected() { return Connected;}

void UDPEngineInterface::tryConnectConstantly(QString Address, int Port)
{
   Connected = false; 

   RingBuffer->Reset(); connectTo(Address, Port); slotCheckConnection();

   QTimer::singleShot(2000, this, SLOT(slotCheckConnection()));
}

void UDPEngineInterface::slotCheckConnection()
{
  if(RingBuffer->isMessageAvailable()) 
  {
  qDebug() << "CONNECTED TO : " << IPRemote << ":" << PortRemote; 
  Connected = true; 
  }

  qDebug() << "TRY TO CONNECT TO : " << IPRemote << ":" << PortRemote; 
  QTimer::singleShot(2000, this, SLOT(SlotCheckRequest()));
}


bool UDPEngineInterface::isMessageAvailable() { return RingBuffer->isMessageAvailable(); }
