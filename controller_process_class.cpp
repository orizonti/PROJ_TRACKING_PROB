#include "controller_process_class.h"
#include "debug_output_filter.h"
#include "interface_image_source.h"
#include "interface_pass_coord.h"
#include "message_command_structures.h"
#include "message_header_generic.h"
#include "register_settings.h"
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
shared_ptr<ModuleImageProcessing>    ProcessControllerClass::ModuleImageProc1  = nullptr;
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

    shared_ptr<RotationDataCollector> ProcessControllerClass::ModuleDataCollector = nullptr;

    shared_ptr<TypeDeviceRotary> ProcessControllerClass::DeviceRotary      = nullptr;
    shared_ptr<TypeRotaryMirrorRemote> ProcessControllerClass::DeviceRotaryMirror  = nullptr;

    shared_ptr<VideoStreamRTSP>  ProcessControllerClass::ModuleVideoOutput = nullptr;

std::shared_ptr<UDPConnectionEngine> ProcessControllerClass::ConnectionControlUDP = nullptr;
std::shared_ptr<UDPConnectionEngine> ProcessControllerClass::ConnectionRotaryUDP = nullptr;
std::shared_ptr<UDPConnectionEngine> ProcessControllerClass::ConnectionProcessorUDP = nullptr;

std::shared_ptr<UARTConnectionEngine> ProcessControllerClass::ConnectionUART = nullptr;
std::shared_ptr<EmptyConnectionEngine> ProcessControllerClass::ConnectionEmpty = nullptr;

std::shared_ptr<CANConnectionEngine    > ProcessControllerClass::ConnectionCAN      = nullptr;
std::shared_ptr<CANOpenConnectionEngine> ProcessControllerClass::ConnectionCAN_OPEN = nullptr;

ProcessControllerClass* ProcessControllerClass::GetInstance(QObject* parent)
{
    if(ProcessControllerInstance == nullptr) ProcessControllerInstance = new ProcessControllerClass(parent);
                                      return ProcessControllerInstance;
}

std::vector<std::string> cameras_test_links
{
  "rtspsrc location=rtsp://192.168.1.59:8554/test latency=10 drop-on-latency=true is-live=true buffer-mode=auto ! queue ! rtph264depay ! h264parse ! openh264dec ! videoconvert ! appsink drop=1 sync=0 max-buffers=1 async=1",
   "rtspsrc location=rtsp://192.168.1.108:554/stream3 latency=10 drop-on-latency=true is-live=true buffer-mode=auto! queue ! rtpjpegdepay ! jpegparse ! jpegdec ! videoconvert ! appsink name=sink_node drop=1 sync=0 max-buffers=1 async=1",
  "udpsrc port=5000 ! application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96 ! rtph264depay ! h264parse ! openh264dec ! videoconvert n-threads=3 ! video/x-raw,format=RGB ! appsink name=sink_node drop=1 async=false sync=true max-buffers=1",
"rtspsrc location=rtsp://192.168.1.185:554/user=admin_password=_channel=1_stream=0.sdp latency=20 buffer-mode=auto ! queue ! rtph264depay ! h264parse ! openh264dec ! videoconvert ! videoscale ! video/x-raw,format=BGR,width=720,height=540 ! appsink sync=false max-buffers=1 async=true",
    "compositor name=comp sink_0::xpos=0 sink_0::ypos=0 sink_0::width=720 sink_0::height=540 sink_1::xpos=0 sink_1::ypos=0 sink_1::width=720 sink_1::height=540 ! videoconvert ! appsink \
filesrc location=/home/orangepi/VIDEO/Sky5.mp4 ! decodebin ! videoconvert ! video/x-raw,format=BGRA ! alpha alpha=0.6 ! comp.sink_0 \videotestsrc pattern=ball ! video/x-raw,format=BGRA,width=720,height=540 ! videoconvert ! alpha alpha=0.34 ! comp.sink_1",
"videotestsrc pattern=ball ! video/x-raw,format=BGR,width=720,height=540 ! videoconvert ! appsink",
};

ProcessControllerClass::ProcessControllerClass(QObject* parrent): QObject(parrent)
{

qDebug() << "======================================================" << Qt::endl;
qDebug() << TAG_NAME.c_str() << "[ CREATE PROCESS CONTROLLER ]";


ModuleImageProc1   = make_shared<ImageTrackerTemplate>();
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

ConnectionEmpty  = make_shared<EmptyConnectionEngine>();
//ConnectionUART   = make_shared<UARTConnectionEngine>("/dev/ttyS3", 115200);


#if(USE_ROTARY_TYPE == ROTARY_TYPE_SCANATOR)
ConnectionCAN          = make_shared<CANConnectionEngine>("can0");
#endif

#if(USE_ROTARY_TYPE == ROTARY_TYPE_PLATFORM_2)
ConnectionCAN_OPEN     = make_shared<CANOpenConnectionEngine>("can0");
#endif

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
//DeviceRotary->setLimits();
DeviceRotary->moveToPos(QPair<float,float>(0,0));
QTimer::singleShot(100,[this]() { DeviceRotary->moveToPos(QPair<float,float>(0,0)); });
QTimer::singleShot(200,[this]() { DeviceRotary->moveToPos(QPair<float,float>(0,0)); });
}
#endif

#if(USE_ROTARY_TYPE == ROTARY_TYPE_PLATFORM)
{
DeviceRotary = std::make_shared<TypeRotaryPlatform>(ConnectionRotaryUDP, CONTROL_PARAM::POS, "[PLATFORM]");
DeviceRotary->setLimits({0,360},{-20,80});
DeviceRotary->setNull(QPair<float,float>(66,5.3));

}
#endif


auto ROTARY_NULL_X = SettingsRegister::GetValue("ROTARY_NULL_X");
auto ROTARY_NULL_Y = SettingsRegister::GetValue("ROTARY_NULL_Y");
qDebug() << TAG_NAME.c_str() << "[ROTARY NULL]" << ROTARY_NULL_X << ROTARY_NULL_Y;

#if(USE_ROTARY_TYPE == ROTARY_TYPE_PLATFORM_2)
{
DeviceRotary = std::make_shared<TypeRotaryPlatform2>(ConnectionCAN_OPEN, CONTROL_PARAM::POS, "[PLATFORM]");
DeviceRotary->setLimits({0,220 },{-20,20});
DeviceRotary->setNull  ({-180,0});
DeviceRotary->setGainDevice(5000);
DeviceRotary->moveToPos({ROTARY_NULL_X,ROTARY_NULL_Y});

//DeviceRotary->ModuleMoveSinus.setFreq(0.2);
//DeviceRotary->ModuleMoveSinus.setAmplitude(20*5000);
//DeviceRotary->ModuleMoveSinus.enableMove(true);
}
#endif


DeviceLaserIllum  = make_shared<TypeDeviceLaserIllum>(ConnectionControlUDP);
DeviceLaserPower  = make_shared<TypeDeviceLaserPower>(ConnectionControlUDP);

ModuleAiming1       = make_shared<AimingClass>();
ModuleAiming2       = make_shared<AimingClass>();
ModuleAimingMonitor1 = make_shared<TypeAimingMonitoring>(ConnectionControlUDP);
ModuleAimingMonitor2 = make_shared<TypeAimingMonitoring>(ConnectionControlUDP);
ModuleAimingMonitor3 = make_shared<TypeAimingMonitoring>(ConnectionControlUDP);

ModuleAimingMonitor1->linkToModule(ModuleImageProc1);

//DeviceCamera  = make_shared<TypeCamera>("[HIK CAMERA]", 0);
//if(TypeCameraUsed == CAMERA_TYPE_HIK)  DeviceCamera  = make_shared<TypeCamera>("[HIK CAMERA]", 0);
//if(TypeCameraUsed == CAMERA_TYPE_RTSP) DeviceCamera  = make_shared<TypeCamera>(SettingsRegister::GetString("CAMERA_LINK").toStdString());

DeviceCamera  = make_shared<TypeCamera>(cameras_test_links.back());

ModuleVideoOutput   = make_shared<VideoStreamRTSP>();


//DeviceRotaryMirror  = std::make_shared<TypeRotaryMirrorRemote>(ConnectionEmpty, CONTROL_PARAM::POS, "[SCANATOR]");
//ModuleDataCollector = std::make_shared<RotationDataCollector>(ModuleAiming1->PortCalibration,DeviceRotaryMirror);


//QObject::connect(this, SIGNAL(SignalProcessEnd()), DeviceCamera.get()   , SLOT(SlotDeinitCamera())  , Qt::QueuedConnection);
QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc1.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);
QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc2.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);
QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc3.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);

qDebug() << TAG_NAME.c_str() << "[ INIT END ]";
qDebug() << "======================================================" << Qt::endl;

}


ProcessControllerClass::~ProcessControllerClass()
{
  qDebug() << TAG_NAME.c_str() << "DELETE PROCESS CONTROLLER" << QThread::currentThread();
  emit SignalProcessEnd(); QThread::sleep(2);

  ThreadProcess1.quit(); ThreadProcess1.deleteLater();
  ThreadProcess2.quit(); ThreadProcess2.deleteLater();
  ThreadProcess3.quit(); ThreadProcess3.deleteLater();
    ThreadCamera.quit();   ThreadCamera.deleteLater();
  ThreadProcessAiming.quit();  ThreadProcessAiming.deleteLater();

                           QThread::sleep(2);

  qDebug() << TAG_NAME.c_str() << "DELETE PROCESS CONTROLLER END";
  qDebug() << Qt::endl;
}


void ProcessControllerClass::StopAllProcess() { }
void ProcessControllerClass::DeleteModulesLinks() { }
void ProcessControllerClass::slotSetProcessAiming2(bool OnOff) { }
void ProcessControllerClass::slotSetProcessCamera (bool OnOff) 
{ 
 DeviceCamera->moveToThread(&ThreadCamera);
                             ThreadCamera.start(QThread::NormalPriority);       
 DeviceCamera->CameraStartStream(true);

}
void ProcessControllerClass::slotStartProcessRTSP (bool OnOff) 
{ 
  DeviceCamera | ModuleVideoOutput;

  QTimer::singleShot(2000,[this]() 
  {
  MessageGenericExt<CommandCheckConnection, MESSAGE_HEADER_GENERIC> CheckConnection;
  ConnectionControlUDP->slotSendMessage(CheckConnection.toByteArray()); });
}

void ProcessControllerClass::slotStartProcessCalibration(bool OnOff)
{
  auto DeviceBase    = ModuleAiming1->PortCalibration;
  auto DeviceRotated = DeviceRotary;

  //auto DeviceBase    = DeviceRotary;
  //auto DeviceRotated = ModuleImageProc2;

if(ModuleDataCollector)
   ModuleDataCollector = std::make_shared<RotationDataCollector>(DeviceBase, DeviceRotated, ConnectionControlUDP);

   ModuleDataCollector->SlotStartProcess(true);

}

void ProcessControllerClass::setScheme1(std::shared_ptr<PassCoordClass<float>> DeviceControl)
{
   qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]"  << "[PROC3 | PROC2; PROC2 | PROC1; PROC1 | PROC2 | CONTROL]" ;
   DeviceCamera | ModuleImageProc1; 
   DeviceCamera | ModuleImageProc2; 
   DeviceCamera | ModuleImageProc3 | ModuleImageProc1 | ModuleImageProc2 | DeviceControl;
                                     ModuleImageProc2 | ModuleImageProc1;

                  ModuleImageProc2->SetSlaveMode(ModuleImageProcessing::ModesModule::SlaveActive);

   ModulesActive.push_back(ModuleImageProc1);
   ModulesActive.push_back(ModuleImageProc2);
   ModulesActive.push_back(ModuleImageProc3);

   //QTimer::singleShot(1000, [this]() { ModuleImageProc3->SetStateActive();}) ;
}

void ProcessControllerClass::setScheme2(std::shared_ptr<PassCoordClass<float>> DeviceControl)
{
   qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]"  << "[PROC1 | PROC2 | CONTROL]" ;

   DeviceCamera | ModuleImageProc1 | ModuleImageProc2; 
   DeviceCamera | ModuleImageProc2 | ModuleAiming1 | DeviceControl;                     

                  ModuleImageProc2->SetSlaveMode(ModuleImageProcessing::ModesModule::SlaveActive);

   ModulesActive.push_back(ModuleImageProc1);
   ModulesActive.push_back(ModuleImageProc2);
    
   QTimer::singleShot(1000, [this]() { ModuleImageProc1->SetStateActive();}) ;
}

void ProcessControllerClass::setScheme3(std::shared_ptr<PassCoordClass<float>> DeviceControl)
{
   qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]"  << "[PROC3 | PROC2 | CONTROL]" ;
   DeviceCamera | ModuleImageProc2; 
   DeviceCamera | ModuleImageProc3 | ModuleImageProc2 | DeviceControl;
   QTimer::singleShot(1000, [this]() { ModuleImageProc3->SetStateActive();}) ;

   ModulesActive.push_back(ModuleImageProc2);
   ModulesActive.push_back(ModuleImageProc3);
}


void ProcessControllerClass::setScheme4(std::shared_ptr<PassCoordClass<float>> DeviceControl)
{
   qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]"  << "[PROC1 | CONTROL]" ;

   DeviceCamera | ModuleImageProc1 | ModuleAiming1 | DeviceControl;                     
   ModulesActive.push_back(ModuleImageProc1);
    
   QTimer::singleShot(1000, [this]() { ModuleImageProc1->SetStateActive();}) ;
}

void ProcessControllerClass::setScheme5(std::shared_ptr<PassCoordClass<float>> DeviceControl)
{
   qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]"  << "[PROC2 | CONTROL]" ;
   DeviceCamera | ModuleImageProc2 | ModuleAiming2 | DeviceControl;                     
   ModulesActive.push_back(ModuleImageProc2);
    
   QTimer::singleShot(1000, [this]() { ModuleImageProc2->SetStateActive();}) ;
}

void ProcessControllerClass::setScheme6(std::shared_ptr<PassCoordClass<float>> DeviceControl)
{
   qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]"  << "[PROC3 | CONTROL]" ;
   DeviceCamera | ModuleImageProc3;                     
   ModulesActive.push_back(ModuleImageProc3);
    
   QTimer::singleShot(1000, [this]() { ModuleImageProc3->SetStateActive();}) ;
}



void ProcessControllerClass::slotSetProcessAiming(bool OnOff)
{
  qDebug() << Qt::endl << "======================================================" << Qt::endl;

  ModuleImageProc1->moveToThread(&ThreadProcess1);
  ModuleImageProc2->moveToThread(&ThreadProcess2);
  ModuleImageProc3->moveToThread(&ThreadProcess3);
     ModuleAiming1->moveToThread(&ThreadProcessAiming);
      DeviceCamera->moveToThread(&ThreadCamera);
  qDebug() << Qt::endl << "======================================================" << Qt::endl;

  ModuleImageProc1->SetPeriodProcess(12);
  ModuleImageProc2->SetPeriodProcess(2);
  ModuleImageProc3->SetPeriodProcess(22);

  ModuleAiming1->setAimingRegim(AimingLoop);
  ModuleAiming2->setAimingRegim(AimingLoop);
  ModuleAiming1->SetAimingPosition(std::pair<float,float>(0.5,0.5));
   
                                        auto RotaryControlPort = DeviceRotary->ControlRotaryPosRelative;
  if(TypeRotaryUsed == ROTARY_TYPE_SCANATOR) RotaryControlPort = DeviceRotary->ControlRotaryPos;

  //=====================================================
//     TypeProcessingUsed = SettingsRegister::GetValue("PROCESSING_SCHEME"); 
//  if(TypeProcessingUsed == PROCESSING_SCHEME_1) setScheme1(RotaryControlPort);
//  if(TypeProcessingUsed == PROCESSING_SCHEME_2) setScheme2(RotaryControlPort);
//  if(TypeProcessingUsed == PROCESSING_SCHEME_3) setScheme3(RotaryControlPort);
//  if(TypeProcessingUsed == PROCESSING_SCHEME_4) setScheme4(RotaryControlPort);
//  if(TypeProcessingUsed == PROCESSING_SCHEME_5) setScheme5(RotaryControlPort);
  qDebug() << TAG_NAME.c_str() << "[PROCESSING SCHEME ]" << TypeProcessingUsed;
  setScheme6(RotaryControlPort);

  //=====================================================

             slotStartProcessRTSP(true);
  ModuleAimingMonitor1->startWork(true);

  QTimer::singleShot(100, [this]() { ThreadProcess1.start(QThread::NormalPriority);       }) ;
  QTimer::singleShot(200, [this]() { ThreadProcess2.start(QThread::HighestPriority);       }) ;
  QTimer::singleShot(300, [this]() { ThreadProcess3.start(QThread::LowPriority);     }) ;
  QTimer::singleShot(400, [this]() { ThreadCamera.start(QThread::LowPriority);       }) ;
  QTimer::singleShot(500, [this]() { ThreadProcessAiming.start(QThread::NormalPriority);}) ;
   
  DeviceCamera->CameraStartStream(true);
   
  ModuleImageProc1->printInfo();
  ModuleImageProc2->printInfo();
  ModuleImageProc3->printInfo();
              this->printInfo();

  qDebug() << Qt::endl << "======================================================" << Qt::endl;
}

//ModuleAiming1->SetStateActive();
//ModuleAiming1->NodeSignalFault |  ModuleImageProc->NodeSignalFault;
//ModuleAiming1->NodeSignalFault | ModuleImageProc2->NodeSignalFault;
   
void ProcessControllerClass::printInfo()
{
  if(TypeCameraUsed == CAMERA_TYPE_HIK)      qDebug() << TAG_NAME.c_str() << "[ USES CAMERA TYPE HIK ]";
  if(TypeCameraUsed == CAMERA_TYPE_RTSP)     qDebug() << TAG_NAME.c_str() << "[ USES CAMERA TYPE RTSP]";
  if(TypeRotaryUsed == ROTARY_TYPE_PLATFORM) qDebug() << TAG_NAME.c_str() << "[ USES ROTARY TYPE PLATFORM]";
  if(TypeRotaryUsed == ROTARY_TYPE_SCANATOR) qDebug() << TAG_NAME.c_str() << "[ USES ROTARY TYPE SCANATOR]";

  qDebug() << TAG_NAME.c_str() << "[ PROCESSING SCHEME]" << TypeProcessingUsed; 
  qDebug() << TAG_NAME.c_str() << "[ AIMING SCHEME]" << ModuleAiming1->TypeAimingSchemeUsed; 
}


void ProcessControllerClass::initMessageDispatchers()
{
qDebug() << "===============================================";
Dispatcher->AppendCallback<CommandSetPosScanator>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandSetPosScanator>(&message);
                     this->DeviceRotary->moveToPos(data->Command); 
                     qDebug() << "SET POS SCANATOR: " << data->Command.first << data->Command.second;
                  }
                  );

Dispatcher->AppendCallback<CommandSetPosRotary>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandSetPosRotary>(&message);
                     this->DeviceRotary->moveToPos(data->Command); 
                     //qDebug() << OutputFilter::Filter(10) << "SET POS ROTARY: " << data->Command.first << data->Command.second;
                     qDebug() << "SET POS ROTARY: " << data->Command.first << data->Command.second;
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
                     ModuleImageProc1->SlotSelectObject(data->Command);;
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

Dispatcher->AppendCallback<CommandDevice<0>>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandDevice<0>>(&message);
                     //qDebug() << "[ COMMAND AIMING CONTROL ]" << data->Command << data->CommandParam.ParamFloat;
                     ModuleAiming1->setCommand(*data);
                  }
                  );

Dispatcher->AppendCallback<CommandDevice<1>>( [this](MessageType& message)
                  {
                     auto data = DispatcherType::ExtractData<CommandDevice<0>>(&message);
                     qDebug() << "[ COMMAND DEVICE 1 ]" << data->Command << data->CommandParam.ParamFloat;
                     ModuleImageProc2->SetThreshold(data->CommandParam.ParamFloat);
                  }
                  );

qDebug() << "===============================================";
}
