#include "controller_process_class.h"
#include "debug_output_filter.h"
#include "interface_image_source.h"
#include "interface_pass_coord.h"
#include "message_header_generic.h"
#include "state_block_enum.h"
#include "thread_operation_nodes.h"
#include "tracker_template.h"
#include "finder_moving_centroid.h"


#undef signals
#include "stream_video_rtsp.h"
#define signals Q_SIGNALS
using namespace std;


std::shared_ptr<MessageStorageInterface> ProcessControllerClass::RingBuffer  = nullptr;
std::shared_ptr<DispatcherType>          ProcessControllerClass::Dispatcher  = nullptr;

std::shared_ptr<MessageStorageInterface> ProcessControllerClass::RingBuffer2  = nullptr;
std::shared_ptr<DispatcherType>          ProcessControllerClass::Dispatcher2  = nullptr;

std::shared_ptr<MessageStorageInterface> ProcessControllerClass::RingBuffer3 = nullptr;
std::shared_ptr<DispatcherType2>         ProcessControllerClass::Dispatcher3 = nullptr;


ProcessControllerClass* ProcessControllerClass::ProcessControllerInstance = nullptr;
shared_ptr<ModuleImageProcessing>    ProcessControllerClass::ModuleImageProc  = nullptr;
shared_ptr<ModuleImageProcessing>    ProcessControllerClass::ModuleImageProc2 = nullptr;
shared_ptr<ModuleImageProcessing>    ProcessControllerClass::ModuleImageProc3 = nullptr;


std::shared_ptr<TypeCamera>       ProcessControllerClass::DeviceCamera = nullptr;

shared_ptr<TypeDeviceLaserPower> ProcessControllerClass::DeviceLaserPower = nullptr;
shared_ptr<TypeDeviceLaserIllum> ProcessControllerClass::DeviceLaserIllum = nullptr;

    shared_ptr<AimingClass>          ProcessControllerClass::ModuleAiming1 = nullptr;
    shared_ptr<AimingClass>          ProcessControllerClass::ModuleAiming2 = nullptr;

    shared_ptr<TypeAimingMonitoring> ProcessControllerClass::ModuleAimingMonitor1 = nullptr;
    shared_ptr<TypeAimingMonitoring> ProcessControllerClass::ModuleAimingMonitor2 = nullptr;
    shared_ptr<TypeAimingMonitoring> ProcessControllerClass::ModuleAimingMonitor3 = nullptr;

    shared_ptr<TypeDeviceRotary> ProcessControllerClass::DeviceRotary      = nullptr;
    shared_ptr<VideoStreamRTSP>  ProcessControllerClass::ModuleVideoOutput = nullptr;

std::shared_ptr<UDPConnectionEngine> ProcessControllerClass::ConnectionControlUDP = nullptr;
std::shared_ptr<UDPConnectionEngine> ProcessControllerClass::ConnectionRotaryUDP = nullptr;
std::shared_ptr<UDPConnectionEngine> ProcessControllerClass::ConnectionProcessorUDP = nullptr;

std::shared_ptr<CANConnectionEngine> ProcessControllerClass::ConnectionCAN = nullptr;

ProcessControllerClass* ProcessControllerClass::GetInstance(QObject* parent)
{
    if(ProcessControllerInstance == nullptr) ProcessControllerInstance = new ProcessControllerClass(parent);
                                      return ProcessControllerInstance;
}

std::vector<std::string> cameras_links 
{
  "rtspsrc location=rtsp://192.168.1.59:8554/test latency=10 drop-on-latency=true is-live=true buffer-mode=auto ! queue ! rtph264depay ! h264parse ! openh264dec ! videoconvert ! appsink drop=1 sync=0 max-buffers=1 async=1",
   "rtspsrc location=rtsp://192.168.1.108:554/stream3 latency=10 drop-on-latency=true is-live=true buffer-mode=auto! queue ! rtpjpegdepay ! jpegparse ! jpegdec ! videoconvert ! appsink name=sink_node drop=1 sync=0 max-buffers=1 async=1",
  "udpsrc port=5000 ! application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96 ! rtph264depay ! h264parse ! openh264dec ! videoconvert n-threads=3 ! video/x-raw,format=RGB ! appsink name=sink_node drop=1 async=false sync=true max-buffers=1"
};

ProcessControllerClass::ProcessControllerClass(QObject* parrent): QObject(parrent)
{

qDebug() << "======================================================" << Qt::endl;
qDebug() << TAG_NAME.c_str() << "[ CREATE PROCESS CONTROLLER ]";


ModuleImageProc    = make_shared<ImageTrackerTemplate>();
ModuleImageProc2   = make_shared<ImageTrackerCentroid>();
ModuleImageProc3   = make_shared<FinderObjectMoving>();

//================================================================================================
qDebug() << "======================================================" << Qt::endl;
qDebug() << TAG_NAME.c_str() << "[ CONNECTIONS ]" ;

auto SocketTerminal1 = SettingsRegister::GetString("TERMINAL_REMOTE").split(":");
auto SocketTerminal2 = SettingsRegister::GetString("TERMINAL_LOCAL").split(":");

auto SocketPlatform1 = SettingsRegister::GetString("PLATFORM_REMOTE").split(":");
auto SocketPlatform2 = SettingsRegister::GetString("PLATFORM_LOCAL").split(":");

auto SocketControl1 = SettingsRegister::GetString("PROCESSOR_REMOTE").split(":");
auto SocketControl2 = SettingsRegister::GetString("PROCESSOR_LOCAL").split(":");

QString ip_control1 = SocketControl1[0]; int port_control1 = SocketControl1[1].toInt();
QString ip_control2 = SocketControl2[0]; int port_control2 = SocketControl1[1].toInt();

QString ip_terminal1 = SocketTerminal1[0]; int port_terminal1 = SocketTerminal1[1].toInt();
QString ip_terminal2 = SocketTerminal2[0]; int port_terminal2 = SocketTerminal2[1].toInt();

QString ip_platform1 = SocketPlatform1[0]; int port_platform1 = SocketPlatform1[1].toInt();
QString ip_platform2 = SocketPlatform2[0]; int port_platform2 = SocketPlatform2[1].toInt();

ConnectionControlUDP   = make_shared<UDPConnectionEngine>(ip_terminal1,port_terminal1,ip_terminal2,port_terminal2);
ConnectionProcessorUDP = make_shared<UDPConnectionEngine>(ip_control1 ,port_control1 ,ip_control2 ,port_control2);

if(TypeRotaryUsed == ROTARY_TYPE_PLATFORM)
ConnectionRotaryUDP    = make_shared<UDPConnectionEngine>(ip_platform1,port_platform1,ip_platform2,port_platform2);
                                                                              qDebug() << Qt::endl;

ConnectionCAN          = make_shared<CANConnectionEngine>("can0");

                  RingBuffer = make_shared<BufferType    >();
                  Dispatcher = make_shared<DispatcherType>();

                  RingBuffer2 = make_shared<BufferType    >();
                  Dispatcher2 = make_shared<DispatcherType>();

                  RingBuffer3 = make_shared<BufferType2    >();
                  Dispatcher3 = make_shared<DispatcherType2>();

 *ConnectionControlUDP   | RingBuffer  | Dispatcher;
 *ConnectionProcessorUDP | RingBuffer2 | Dispatcher2;

  initMessageDispatchers();

// if(TypeRotaryUsed == ROTARY_TYPE_PLATFORM) *ConnectionRotaryUDP | RingBuffer3 | Dispatcher3;
qDebug() << "======================================================";
 //================================================================================================

#if(USE_ROTARY_TYPE == ROTARY_TYPE_SCANATOR)
{
DeviceRotary = std::make_shared<TypeRotaryScanator>(ConnectionCAN, CONTROL_PARAM::POS, "[SCANATOR]");
DeviceRotary->setLimits(CONTROL_PARAM::POS, 30000,30000);
}
#endif

#if(USE_ROTARY_TYPE == ROTARY_TYPE_PLATFORM)
{
DeviceRotary = std::make_shared<TypeRotaryPlatform>(ConnectionRotaryUDP, CONTROL_PARAM::POS, "[PLATFORM]");
DeviceRotary->setLimits(CONTROL_PARAM::POS, 360,20);
DeviceRotary->setNull(QPair<float,float>(37.50,-6.3));
}
#endif


DeviceLaserIllum  = make_shared<TypeDeviceLaserIllum>(ConnectionControlUDP);
DeviceLaserPower  = make_shared<TypeDeviceLaserPower>(ConnectionControlUDP);

ModuleAiming1       = make_shared<AimingClass>();
ModuleAiming2       = make_shared<AimingClass>();
ModuleAimingMonitor1 = make_shared<TypeAimingMonitoring>(ConnectionControlUDP);
ModuleAimingMonitor2 = make_shared<TypeAimingMonitoring>(ConnectionControlUDP);
ModuleAimingMonitor3 = make_shared<TypeAimingMonitoring>(ConnectionControlUDP);

ModuleAimingMonitor1->linkToModule(ModuleImageProc2);

if(TypeCameraUsed == CAMERA_TYPE_HIK)  DeviceCamera  = make_shared<TypeCamera>("[HIK CAMERA]", 0);
if(TypeCameraUsed == CAMERA_TYPE_RTSP) DeviceCamera  = make_shared<TypeCamera>(SettingsRegister::GetString("CAMERA_LINK").toStdString());

ModuleVideoOutput   = make_shared<VideoStreamRTSP>();




 //DeviceRotary->moveSinus(true);



 //QObject::connect(this, SIGNAL(SignalProcessEnd()), DeviceCamera.get()   , SLOT(SlotDeinitCamera())  , Qt::QueuedConnection);
 QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc.get() , SLOT(SlotStopProcessing()), Qt::QueuedConnection);
 QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc2.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);
 QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc3.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);

qDebug() << TAG_NAME.c_str() << "[ INIT END ]";
qDebug() << "======================================================" << Qt::endl;

}


ProcessControllerClass::~ProcessControllerClass()
{
  qDebug() << TAG_NAME.c_str() << "DELETE PROCESS CONTROLLER" << QThread::currentThread();
  emit SignalProcessEnd(); QThread::sleep(2);

  ThreadProcess.quit(); ThreadProcess.deleteLater();
   ThreadCamera.quit();  ThreadCamera.deleteLater();

                           QThread::sleep(2);

  qDebug() << TAG_NAME.c_str() << "DELETE PROCESS CONTROLLER END";
  qDebug() << Qt::endl;
}


void ProcessControllerClass::StopAllProcess() { }
void ProcessControllerClass::DeleteModulesLinks() { }
void ProcessControllerClass::slotSetProcessAiming2(bool OnOff) { }
void ProcessControllerClass::slotSetProcessCamera (bool OnOff) { DeviceCamera | ModuleImageProc;  }
void ProcessControllerClass::slotStartProcessRTSP (bool OnOff) 
{ 
  //ModuleVideoOutput->linkToSource(DeviceCamera.get()); 
}


void ProcessControllerClass::slotSetProcessAiming(bool OnOff)
{
 if(!OnOff) return; DeleteModulesLinks(); StopAllProcess(); 

   qDebug() << Qt::endl << "======================================================" << Qt::endl;
   qDebug() << TAG_NAME.c_str() << "[ PROCESS AIMING ]";


    ModuleImageProc->moveToThread(&ThreadProcess);
   ModuleImageProc2->moveToThread(&ThreadProcess2);
   ModuleImageProc3->moveToThread(&ThreadProcess3);
      ModuleAiming1->moveToThread(&ThreadProcessAiming);


   ModuleAiming1->setAimingRegim(AimingLoop);
   ModuleAiming2->setAimingRegim(AimingLoop);
   ModuleAiming1->SetAimingPosition(std::pair<float,float>(0.5,0.5));

   
                                         auto RotaryControlPort = DeviceRotary->ControlRotaryPosRelative;
   if(TypeRotaryUsed == ROTARY_TYPE_SCANATOR) RotaryControlPort = DeviceRotary->ControlRotaryPos;


   DeviceCamera | ModuleImageProc3 | ModuleImageProc;
                  ModuleImageProc3 | ModuleImageProc2;

                                     ModuleImageProc2 | ModuleImageProc;
   DeviceCamera | ModuleImageProc  | ModuleImageProc2; 
   DeviceCamera | ModuleImageProc2 | ModuleAiming1 | RotaryControlPort;                     

   //ModuleAiming1->NodeSignalFault |  ModuleImageProc->NodeSignalFault;
   //ModuleAiming1->NodeSignalFault | ModuleImageProc2->NodeSignalFault;

   ModuleImageProc2->SetHighFrequencyProcessing();

    ModuleImageProc->SetLowFrequencyProcessing();
   ModuleImageProc3->SetLowFrequencyProcessing();

    ModuleImageProc2->SetSlaveMode();


   ModuleAimingMonitor1->startWork(true);
   
    ThreadProcess.start();
    ThreadProcess.setPriority(QThread::HighPriority);

   ThreadProcess2.start();
   ThreadProcess2.setPriority(QThread::HighPriority);

   ThreadProcess3.start();
   ThreadProcess3.setPriority(QThread::HighPriority);

   ThreadProcessAiming.start();
   ThreadProcessAiming.setPriority(QThread::HighPriority);

     ModuleImageProc->SetStateActive();
        DeviceCamera->CameraStartStream(true);
    //ModuleImageProc2->SetStateActive();
    //ModuleImageProc3->SetStateActive();
    //ModuleAiming1->SetStateActive();
    //
    ModuleImageProc->printInfo();
   ModuleImageProc2->printInfo();
   ModuleImageProc3->printInfo();
               this->printInfo();
    
   qDebug() << Qt::endl << "======================================================" << Qt::endl;
}

void ProcessControllerClass::printInfo()
{
  if(TypeCameraUsed == CAMERA_TYPE_HIK)      qDebug() << TAG_NAME.c_str() << "[ USES CAMERA TYPE HIK ]";
  if(TypeCameraUsed == CAMERA_TYPE_RTSP)     qDebug() << TAG_NAME.c_str() << "[ USES CAMERA TYPE RTSP]";
  if(TypeRotaryUsed == ROTARY_TYPE_PLATFORM) qDebug() << TAG_NAME.c_str() << "[ USES ROTARY TYPE PLATFORM]";
  if(TypeRotaryUsed == ROTARY_TYPE_SCANATOR) qDebug() << TAG_NAME.c_str() << "[ USES ROTARY TYPE SCANATOR]";
}


void ProcessControllerClass::initMessageDispatchers()
{
qDebug() << "===============================================";
Dispatcher->AppendCallback<CommandSetPosScanator>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandSetPosScanator>(&message);
                     this->DeviceRotary->moveToPos(data->Command); 
                     //qDebug() << OutputFilter::Filter(10) << "SET POS SCANATOR: " << data->Command.first << data->Command.second;
                  }
                  );

Dispatcher->AppendCallback<CommandSetPosRotary>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandSetPosRotary>(&message);
                     this->DeviceRotary->moveToPos(data->Command); 
                     //qDebug() << OutputFilter::Filter(10) << "SET POS ROTARY: " << data->Command.first << data->Command.second;
                  }
                  );

Dispatcher->AppendCallback<CommandDeviceLaserPower>( [this](MessageType& message)
                  {
                     DeviceLaserPower->transmitMessage(reinterpret_cast<const char*>(&message), 
                                                            message.GetSize(), 
                                                            message.HEADER.MESSAGE_IDENT);

                     auto data = DispatcherType::ExtractData<CommandDeviceLaserPower>(&message);
                     qDebug() << "COMMAND LASER POWER: " << message.HEADER.MESSAGE_IDENT << Qt::hex << data->Command;
                  }
                  );


Dispatcher->AppendCallback<CommandDeviceLaserPointer>( [this](MessageType& message)
                  {
                     DeviceLaserIllum->transmitMessage(reinterpret_cast<const char*>(&message), 
                                                            message.GetSize(), 
                                                            message.HEADER.MESSAGE_IDENT);

                     auto data = DispatcherType::ExtractData<CommandDeviceLaserPointer>(&message);
                     qDebug() << "COMMAND LASER ILLUM: " << message.HEADER.MESSAGE_IDENT << Qt::hex << data->Command;
                  }
                  );


Dispatcher->AppendCallback<CommandAiming1>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandAiming2>(&message);
                     qDebug() << "[ COMMAND SECELT AIM ]"  << data->Command.first << data->Command.second;
                     ModuleImageProc->SlotSelectObject(data->Command);;
                  }
                  );

Dispatcher->AppendCallback<CommandAiming2>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandAiming1>(&message);
                     qDebug() << "[ COMMAND AIMING SET AIM ]" << data->Command.first << data->Command.second;
                     if(data->CommandType == 0) data->Command >> *ModuleAiming1->PortSetAiming;
                     if(data->CommandType == 0) data->Command >> *ModuleAiming1->PortCorrection;
                  }
                  );

qDebug() << "===============================================";
}
