#include "tcp_server_common.h"
#include "message_struct_generic.h"
#include "engine_ring_buffer_generic.h"
#include <type_traits>


//===================================================
void TCPConnectionEngine::SlotConnectedToHost()
{
    qDebug() << "CONNECTED TO HOST: " << HOST << PORT; emit DeviceConnected();
}


void TCPConnectionEngine::SlotReadData()
{
   int bytes_available = engineSocket->bytesAvailable();
    if(bytes_available < RingBuffer->MIN_MESSAGE_SIZE) return;

      RingBuffer->AppendData((uint8_t*)engineSocket->readAll().data(), bytes_available);
                                    if(engineSocket->bytesAvailable() > 24) SlotReadData();

   if(RingBuffer->isMessageAvailable()) emit SignalMessageAvailable();
}


void TCPConnectionEngine::SlotSendMessage(QByteArray Command)
{
   //qDebug() << "SenD COMMAND: " << Command.toHex();
   if(!engineSocket->isOpen()) return; engineSocket->write(Command);
}

TCPConnectionEngine::~TCPConnectionEngine() 
{ 
    if(RingBuffer != 0) delete RingBuffer;
    if(Server != 0) delete Server;
}

TCPConnectionEngine::TCPConnectionEngine(QObject *parent)
    : QObject(parent)
{

     RingBuffer = new std::remove_reference<decltype((*RingBuffer))>::type;

     RingBuffer->RegisterMessage(ControlMessage1());
     RingBuffer->RegisterMessage(ControlMessage2());
     RingBuffer->RegisterMessage(ControlMessage3());
     RingBuffer->RegisterMessage(ControlMessage4());
     RingBuffer->RegisterMessage(ControlMessage5());
}

bool TCPConnectionEngine::IsConnected()
{
    if(engineSocket == 0)      return false;
    if(engineSocket->isOpen()) return true;

    return true;
}

void TCPConnectionEngine::TryToConnectConstantly(QString address, int Port)
{
   HOST = address; PORT = Port;
   QObject::connect(&TimerAutoconnection,&QTimer::timeout,this,&TCPConnectionEngine::SlotConnectionAttempt);
   TimerAutoconnection.start(2000);
}

void TCPConnectionEngine::SlotConnectionAttempt()
{
  //qDebug() << "TRY TO CONNECT: " << HOST << PORT;
 if(engineSocket == 0)
 {
    engineSocket = new QTcpSocket(this);
    connect(engineSocket, SIGNAL(connected()), this, SLOT(SlotConnectedToHost()),Qt::QueuedConnection);
 }

 if(engineSocket->isOpen()) return;

 QHostAddress host = QHostAddress(HOST);
 engineSocket->connectToHost(host, PORT, QIODevice::ReadWrite);

 bool connected = engineSocket->waitForConnected(5);

 if(connected)
 {
   qDebug() << "CONNECTED TO HOST " << HOST << PORT;
   connect(engineSocket, SIGNAL(readyRead()), this, SLOT(SlotReadData()),Qt::QueuedConnection);
   TimerAutoconnection.stop();
   QObject::disconnect(&TimerAutoconnection,&QTimer::timeout,this,&TCPConnectionEngine::SlotConnectionAttempt);
   return;
 }

 engineSocket->close();
}

void TCPConnectionEngine::WaitConnectionOn(QString address, int Port)
{
  qDebug() << "WAIT DEVICE AT ADDRESS : " << address << " PORT: " << Port;
  delete engineSocket;
  HOST = address; PORT = Port;
  Server = new QTcpServer;
  Server->listen(QHostAddress(address),PORT);
  connect(Server, SIGNAL(newConnection()), this, SLOT(SlotAcceptConnection()),Qt::QueuedConnection);
}

void TCPConnectionEngine::WaitConnectionOn(QHostAddress::SpecialAddress address, int Port)
{
  qDebug() << "WAIT DEVICE AT ADDRESS : " << address << " PORT: " << Port;
  PORT = Port;

  if(engineSocket != 0) delete engineSocket; //MUST BE NEW SOCKET IF CONNECTION ALREADY HAD BEen ESTABLISHED

  Server = new QTcpServer; Server->listen(address,PORT);
  connect(Server, SIGNAL(newConnection()), this, SLOT(SlotAcceptConnection()),Qt::QueuedConnection);
}

void TCPConnectionEngine::SlotAcceptConnection()
{
  engineSocket = Server->nextPendingConnection();
  connect(engineSocket, SIGNAL(readyRead()), this, SLOT(SlotReadData()),Qt::QueuedConnection);

  qDebug() << "ACCEPT CONNECTION DEVICE: " << engineSocket->peerAddress();
}

void TCPConnectionEngine::ConnectToDevice(QHostAddress::SpecialAddress address, int Port)
{
 if(engineSocket == 0)
 {
    engineSocket = new QTcpSocket(this);
    connect(engineSocket, SIGNAL(connected()), this, SLOT(SlotConnectedToHost()),Qt::QueuedConnection);
 }
 PORT = Port;
 qDebug() << "CONNECT TO DEVICE : " << HOST << PORT;

 connect(engineSocket, SIGNAL(readyRead()), this, SLOT(SlotReadData()),Qt::QueuedConnection);

 engineSocket->connectToHost(address, Port, QIODevice::ReadWrite);
}

void TCPConnectionEngine::ConnectToDevice(QString address, int Port)
{
 if(engineSocket == 0)
 {
    engineSocket = new QTcpSocket(this);
    connect(engineSocket, SIGNAL(connected()), this, SLOT(SlotConnectedToHost()),Qt::QueuedConnection);
 }

 HOST = address; PORT = Port;
 qDebug() << "CONNECT TO DEVICE : " << HOST << PORT;

 connect(engineSocket, SIGNAL(readyRead()), this, SLOT(SlotReadData()),Qt::QueuedConnection);

 QHostAddress host = QHostAddress(address);
 engineSocket->connectToHost(host, Port, QIODevice::ReadWrite);
}

void TCPConnectionEngine::SlotCloseConnection()
{
   SendendCommand();

   engineSocket->flush();
   engineSocket->disconnectFromHost();
   engineSocket->close();
}

void TCPConnectionEngine::SendendCommand()
{
    MessageStructGeneric<MessageConnectionControl,MESSAGE_HEADER> close_command;
    close_command.HEADER.DATA_SIZE = 1;
    close_command.HEADER.DATA_SIZE2 = 1;

    engineSocket->write(close_command.toByteArray()); engineSocket->waitForBytesWritten(200);

}

void TCPConnectionEngine::ResetSocket()
{
 qDebug() << "=============================";
 qDebug() << "        [RESET SOCKET]       ";
 //qDebug()<<"[ SOCKET BYTES: " << engineSocket->CountBytesInStore() << " BUFFER: " << LastDataPointer.pos() << " ]";
 engineSocket->flush();
 qDebug() << "=============================";
}


//void TCPEngineInterface::SlotCheckSocket()
//{
//  qDebug() << "SOCKET STATE: " << ControlSocket->state() << " VALID: " << ControlSocket->isValid();
//  QByteArray array; array.resize(2);
//  array[0] = 0xF1;
//  array[1] = 0xC2;
//
//  if(ControlSocket->isOpen()) ControlSocket->write(array);
//}
//
//void TCPEngineInterface::SlotReadData()
//{
//    HEADERStruct header;
//    LaserStateStruct LaserState;
//
//    if(ControlSocket->bytesAvailable() > 0) InputDataBuffer.append(ControlSocket->readAll());
//    qDebug() << "READ DATA: " << InputDataBuffer.toHex();
//
//    QDataStream in_stream(InputDataBuffer); in_stream.setByteOrder(QDataStream::Littleendian);
//    in_stream >> header; if(InputDataBuffer.size() < header.DataSize) return;
//
//
//    if(header.HEADER2 == 0xC3) qDebug() << "REQUEST FROM DEVICE";
//
//    if(header.HEADER2 == 0xC7) in_stream >> LaserState; emit SignalStateChanged(LaserState);
//    qDebug() << "LASER STATE CHANGED MANUAL : " << LaserState.duty << LaserState.enable << LaserState.ManualControl;
//
//    InputDataBuffer.remove(0,header.DataSize); if(InputDataBuffer.size() >=8) SlotReadData();
//}
//
