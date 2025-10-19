#include "engine_can_interface.h"


CANConnectionEngine::CANConnectionEngine(QObject *parent) : ConnectionInterface(parent)
{

  QString errorString;
  device = QCanBus::instance()->createDevice( QStringLiteral("socketcan"), QStringLiteral("can0"), &errorString);

  if (!device)                  { qDebug() << "[ CAN DEVICE NOT OPEN ] " << errorString; return; }
       device->connectDevice(); { qDebug() << "[ CAN0 DEVICE OPENED ] "; }

  QObject::connect(device,&QCanBusDevice::framesReceived, this, &CANConnectionEngine::slotReadData);

  //qDebug() << "SET BITRATE: 500000";
  //device->setConfigurationParameter( QCanBusDevice::BitRateKey, 500000);

  engineSendTest = new CANTestMessage; engineSendTest->linkTo(this);
  engineDelayMeasure = new CANDelayMeasure; engineDelayMeasure->linkTo(this);

  qDebug() << "==========================";
}

//void CANConnectionEngine::listenTo(QString IDDeviceStr, int IDDevice)
//{
//  DispatcherType<CommandDevice<0>>::appendCall([](CommandDevice<0> Command){}, TypeRegister<CommandDevice<0>>::GetTypeID());
//}

void CANConnectionEngine::printAvailableDevices()
{

  QString errorString;
  const QList<QCanBusDeviceInfo> devices = QCanBus::instance()->availableDevices( QStringLiteral("socketcan"), &errorString);
  if (!errorString.isEmpty()) qDebug() << errorString; 

  for(auto dev: devices) qDebug() << "[ AVAILABLE DEVICE ] " << dev.name();
}

CANConnectionEngine::~CANConnectionEngine() 
{ 
}


void CANConnectionEngine::slotReadData()
{
    while (device->framesAvailable()) 
    {
        const QCanBusFrame frame = device->readFrame();

                                                   QString data;
        if (frame.frameType() == QCanBusFrame::ErrorFrame) data = device->interpretErrorFrame(frame);

            //data = QString::fromLatin1(frame.payload().toHex(' ').toUpper());
        qDebug() << "[ GET FRAME ]" << frame.frameId() << frame.payload().toHex();
        switch(frame.frameId())
        {
          case CommandDispatcherGeneric<10>::ID: 
               CommandDispatcherGeneric<10>::dispatchCommand(frame.payload());
            return; 
          case CommandDispatcherGeneric<20>::ID: 
               CommandDispatcherGeneric<20>::dispatchCommand(frame.payload());
            return; 
          break;
        }

        emit signalMessageAvailable();
    }
}


void CANConnectionEngine::slotSendMessage(const QByteArray& message, uint8_t IDDevice)
{
    qDebug() << "[ CAN ] SEND: " << message.toHex(); 

    QCanBusFrame frame = QCanBusFrame(IDDevice, message);
                 frame.setFrameType(QCanBusFrame::DataFrame);

    device->writeFrame(frame);
}

void CANConnectionEngine::slotSendMessage(const char* DataCommand, int size, uint8_t IDDevice)
{
  QByteArray message(DataCommand,size);

  QCanBusFrame frame = QCanBusFrame(IDDevice, message);
               frame.setFrameType(QCanBusFrame::DataFrame);

  device->writeFrame(frame);
}




bool CANConnectionEngine::isConnected()
{
    return true;
}



void CANConnectionEngine::slotCheckConnection()
{

}


bool CANConnectionEngine::isMessageAvailable() { return false;}
