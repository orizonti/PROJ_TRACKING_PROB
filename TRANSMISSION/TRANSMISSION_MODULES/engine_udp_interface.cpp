#include "engine_udp_interface.h"
#include <qhostaddress.h>

UDPEngineInterface::UDPEngineInterface(QString remote_address,int remote_port, QObject *parent)
    : QObject{parent}
{
    ControlSocket = new QUdpSocket(this);
    RemotePort = remote_port;
    RemoteHost = remote_address;
    InitBuffer();
}

void UDPEngineInterface::InitBuffer()
{
     RingBuffer = new std::remove_reference<decltype((*RingBuffer))>::type;
     RingBuffer->RegisterMessage(ControlMessage1());
     RingBuffer->RegisterMessage(ControlMessage2());
     RingBuffer->RegisterMessage(ControlMessage3());
     RingBuffer->RegisterMessage(ControlMessage4());
     RingBuffer->RegisterMessage(ControlMessage5());
}

void UDPEngineInterface::BindTo(QHostAddress::SpecialAddress address, int Port)
{
    LocalPort = Port;
    ControlSocket->bind(address, LocalPort);
    ControlSocket->open(QIODevice::ReadWrite);           qDebug() << "SOCKET: " << ControlSocket->isOpen();
                                                         qDebug() << ControlSocket->peerAddress().toString();
    ControlSocket->flush();
    QObject::connect(ControlSocket,SIGNAL(readyRead()),this,SLOT(SlotReadData()));
}

void UDPEngineInterface::BindTo(QString address, int Port)
{
    LocalPort = Port;
    ControlSocket->bind(QHostAddress(address), LocalPort);
    ControlSocket->open(QIODevice::ReadWrite);           qDebug() << "SOCKET: " << ControlSocket->isOpen();
                                                         qDebug() << ControlSocket->peerAddress().toString();
    ControlSocket->flush();
    QObject::connect(ControlSocket,SIGNAL(readyRead()),this,SLOT(SlotReadData()));
}

UDPEngineInterface::~UDPEngineInterface()
{
 qDebug() << "DELETE UDP INTERFACE";
 delete ControlSocket;
}

void UDPEngineInterface::SlotReadData()
{

   if(ControlSocket->bytesAvailable() < RingBuffer->MIN_MESSAGE_SIZE) return;

   int bytes_available = ControlSocket->bytesAvailable();

   //int MESSAGE_SIZE_NUMBER = bytes_available/RingBuffer->MIN_MESSAGE_SIZE;
   //qDebug() << "SERVER READ DATA FRAMES: " << MESSAGE_SIZE_NUMBER << "BYTES: " << bytes_available;
   //RingBuffer->AppendData((uint8_t*)engineSocket->read(RingBuffer->MIN_MESSAGE_SIZE*MESSAGE_SIZE_NUMBER).data(), 
   //                                                    RingBuffer->MIN_MESSAGE_SIZE*MESSAGE_SIZE_NUMBER);

   //RingBuffer->AppendData((uint8_t*)engineSocket->read(24).data(), 24);
   RingBuffer->AppendData((uint8_t*)ControlSocket->readAll().data(), bytes_available);
   //RingBuffer->AppendData(engineSocket->readAll());


   if(RingBuffer->isMessageAvailable()) 
   {
   //emit SignalMessageAvailable();
   }

   if(ControlSocket->bytesAvailable() > 24) SlotReadData();
}


void UDPEngineInterface::SlotSendCommand(QByteArray ArrayCommand)
{
 //qDebug() << "COMMAND: " << ArrayCommand.toHex();
 //qDebug() << "SET TO : " << RemoteHost << RemotePort;
 ControlSocket->writeDatagram(ArrayCommand,QHostAddress(RemoteHost),RemotePort); //ControlSocket->waitForBytesWritten(2);
}

void UDPEngineInterface::SlotCheckConnection()
{
}

void UDPEngineInterface::SendendMessage()
{
  //HEADERStruct header; header.HEADER1 = 0xC2;
  //QByteArray array((const char*)&header,4);
  //ControlSocket->writeDatagram(array,QHostAddress(RemoteHost),RemotePort); ControlSocket->waitForBytesWritten(200);
}


