#include "engine_can_interface.h"

#include "message_command_id.h"
#include "debug_output_filter.h"

CANEngineInterface::CANEngineInterface(QString Device, QObject *parent) : ConnectionInterface(parent)
{

  QString errorString;
  device = QCanBus::instance()->createDevice( QStringLiteral("socketcan"), Device, &errorString);

  if (!device)                  { qDebug() << "[ CAN DEVICE NOT OPEN ] " << errorString; return; }
       device->connectDevice(); { qDebug() << "[ " << Device << "DEVICE OPENED ] "; }

  QObject::connect(device,&QCanBusDevice::framesReceived, this, &CANEngineInterface::slotReadData);


      engineSendTest = new CANTestMessage ; engineSendTest->linkTo(this);
  engineDelayMeasure = new CANDelayMeasure; engineDelayMeasure->linkTo(this);

  qDebug() << "==========================";
}

//void CANEngineInterface::listenTo(QString IDDeviceStr, int IDDevice)
//{
//  DispatcherType<CommandDevice<0>>::appendCall([](CommandDevice<0> Command){}, TypeRegister<CommandDevice<0>>::GetTypeID());
//}

void CANEngineInterface::printAvailableDevices()
{

  QString errorString;
  const QList<QCanBusDeviceInfo> devices = QCanBus::instance()->availableDevices( QStringLiteral("socketcan"), &errorString);
  if (!errorString.isEmpty()) qDebug() << errorString; 

  for(auto dev: devices) qDebug() << "[ AVAILABLE DEVICE ] " << dev.name();
}

CANEngineInterface::~CANEngineInterface() 
{ 
}


void CANEngineInterface::slotReadData()
{
    while (device->framesAvailable()) 
    {
        const QCanBusFrame frame = device->readFrame();

                                                   QString data;
        if (frame.frameType() == QCanBusFrame::ErrorFrame) data = device->interpretErrorFrame(frame);

        qDebug() << "[ GET FRAME ]" << frame.frameId() << frame.payload().toHex();

        switch(frame.frameId())
        {
          case TypeRegister<MessageDeviceController>::TYPE_ID: 
               CommandDispatcherGeneric<TypeRegister<MessageDeviceController>::TYPE_ID>::dispatchCommand(frame.payload());
            return; 
          case TypeRegister<MessageDeviceLaserPower>::TYPE_ID: 
               CommandDispatcherGeneric<TypeRegister<MessageDeviceLaserPower>::TYPE_ID>::dispatchCommand(frame.payload());
            return; 

          case TypeRegister<MessageDeviceLaserPointer>::TYPE_ID: 
               CommandDispatcherGeneric<TypeRegister<MessageDeviceLaserPointer>::TYPE_ID>::dispatchCommand(frame.payload());
            return; 

          case TypeRegister<MessageDeviceFocusator>::TYPE_ID: 
               CommandDispatcherGeneric<TypeRegister<MessageDeviceFocusator>::TYPE_ID>::dispatchCommand(frame.payload());
            return; 
          break;
        }

        emit signalMessageAvailable();
    }
}


void CANEngineInterface::slotSendMessage(const QByteArray& message, uint16_t IDDevice)
{
    qDebug() << OutputFilter::Filter(100) << "[ CAN ] SEND: " << QString(message.toHex()); 

    QCanBusFrame frame = QCanBusFrame(IDDevice, message);
                 frame.setFrameType(QCanBusFrame::DataFrame);

    device->writeFrame(frame);
}

void CANEngineInterface::slotSendMessage(const char* DataCommand, int size, uint16_t IDDevice)
{
  QByteArray message(DataCommand,size);

  QCanBusFrame frame = QCanBusFrame(IDDevice, message);
               frame.setFrameType(QCanBusFrame::DataFrame);

  device->writeFrame(frame);
}




bool CANEngineInterface::isConnected()
{
    return true;
}

void CANEngineInterface::slotCheckConnection()
{
}

bool CANEngineInterface::isMessageAvailable() { return false;}
