#include "controller_process_class.h"
#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include "debug_output_filter.h"
#include "interface_image_source.h"
#include "interface_pass_coord.h"
#include "message_header_generic.h"
#include "state_block_enum.h"
#include "thread_operation_nodes.h"

#undef signals
#include "stream_video_rtsp.h"
#define signals Q_SIGNALS
using namespace std;

ProcessControllerClass* ProcessControllerClass::ProcessControllerInstance = nullptr;
shared_ptr<ImageTrackerCentroid>    ProcessControllerClass::ModuleImageProc;
shared_ptr<ImageTrackerCentroid>    ProcessControllerClass::ModuleImageProc2;
shared_ptr<ImageTrackerCentroidGPU> ProcessControllerClass::ModuleImageProc3;

//shared_ptr<CameraInterfaceAravis> ProcessControllerClass::DeviceCamera;
shared_ptr<CameraInterfaceHIK>     ProcessControllerClass::DeviceCamera;

shared_ptr<DeviceTypeLaserPower>   ProcessControllerClass::DeviceLaserPower;
shared_ptr<DeviceTypeLaserPointer> ProcessControllerClass::DeviceLaserPointer;

shared_ptr<AimImageImitatorClass>  ProcessControllerClass::ModuleImitatorImage;

shared_ptr<AimingClass>          ProcessControllerClass::ModuleAiming1;
shared_ptr<AimingClass>          ProcessControllerClass::ModuleAiming2;

shared_ptr<DeviceRotaryInterface> ProcessControllerClass::DeviceRotary;

shared_ptr<VideoStreamRTSP>      ProcessControllerClass::ModuleVideoOutput;

std::shared_ptr<UDPEngineInterface> ProcessControllerClass::ConnectionUDP;
std::shared_ptr<CANEngineInterface> ProcessControllerClass::ConnectionCAN;


ProcessControllerClass* ProcessControllerClass::GetInstance(QObject* parent)
{
    if(ProcessControllerInstance == nullptr) ProcessControllerInstance = new ProcessControllerClass(parent);
                                      return ProcessControllerInstance;
}


using MessageType = MessageGeneric<void*, MESSAGE_HEADER_GENERIC>;

ProcessControllerClass::ProcessControllerClass(QObject* parrent): QObject(parrent)
{
qDebug() << Qt::endl;
qDebug() << TAG_NAME.c_str() << "[ CREATE PROCESS CONTROLLER ]";

ModuleImageProc     = make_shared<ImageTrackerCentroid>();
ModuleImageProc2    = make_shared<ImageTrackerCentroid>();
ModuleImageProc3    = make_shared<ImageTrackerCentroidGPU>();


ConnectionUDP        = make_shared<UDPEngineInterface>("192.168.0.36",2323,"192.168.0.59",2525);
ConnectionCAN        = make_shared<CANEngineInterface>("can0");

DeviceRotary        = make_shared<DeviceRotaryControl<CANEngineInterface,CommandSetPosRotary,CommandSetPosRotary> >(ConnectionCAN);
DeviceLaserPointer  = make_shared<DeviceTypeLaserPointer>(ConnectionCAN);
DeviceLaserPower    = make_shared<DeviceTypeLaserPower  >(ConnectionCAN);

ModuleImitatorImage = make_shared<AimImageImitatorClass>();
ModuleImitatorImage = make_shared<AimImageImitatorClass>();
ModuleAiming1       = make_shared<AimingClass>();
ModuleAiming2       = make_shared<AimingClass>();

DeviceCamera        = make_shared<CameraInterfaceHIK>();
ModuleVideoOutput   = make_shared<VideoStreamRTSP>();

ConnectionUDP->Dispatcher->AppendCallback<CommandSetPosScanator>( [this](MessageType& message)
                  {
                     auto command = UDPEngineInterface::DispatcherType::ExtractData<CommandSetPosScanator>(&message);
                     this->DeviceRotary->moveToPos(command->toPair());
                  }
                  );

ConnectionUDP->Dispatcher->AppendCallback<CommandSetPosRotary>( [this](MessageType& message)
                  {
                     auto command = UDPEngineInterface::DispatcherType::ExtractData<CommandSetPosRotary>(&message);
                     this->DeviceRotary->moveToPos(command->toPair());
                  }
                  );

ConnectionUDP->Dispatcher->AppendCallback<CommandDeviceLaserPower>( [this](MessageType& message)
                  {
                     auto command = UDPEngineInterface::DispatcherType::ExtractData<CommandDeviceLaserPower>(&message);
                     DeviceLaserPower->transmitCommand(*command);
                     qDebug() << "COMMAND LASER: " << Qt::hex << command->Command;
                  }
                  );


ConnectionUDP->Dispatcher->AppendCallback<CommandDeviceLaserPointer>( [this](MessageType& message)
                  {
                     auto command = UDPEngineInterface::DispatcherType::ExtractData<CommandDeviceLaserPointer>(&message);
                     DeviceLaserPointer->transmitCommand(*command);
                     qDebug() << "COMMAND LASER: " << Qt::hex << command->Command;
                  }
                  );

 //DeviceRotary->moveSinus(true);

 //DeviceCamera->moveToThread(&ThreadProcess);
 //ThreadCamera.start();
 //ThreadCamera.setPriority(QThread::TimeCriticalPriority);

 //DeviceCamera->moveToThread(&ThreadProcess);
 ModuleImageProc->moveToThread(&ThreadProcess);
 ModuleImageProc->timerProcessImage.moveToThread(&ThreadProcess);
 QObject::connect(&ThreadProcess, SIGNAL(started()), &ModuleImageProc->timerProcessImage, SLOT(start()));
 ThreadProcess.start();
 ThreadProcess.setPriority(QThread::HighPriority);

 //ModuleImageProc2->moveToThread(&ThreadProcess2);
 //ModuleImageProc2->timerProcessImage.moveToThread(&ThreadProcess2);
 //QObject::connect(&ThreadProcess2, SIGNAL(started()), &ModuleImageProc2->timerProcessImage, SLOT(start()));
 //ThreadProcess2.start();
 //ThreadProcess2.setPriority(QThread::HighPriority);

// slotSetProcessCamera(true);
// slotSetProcessImitation(true);
// slotSetProcessAiming(true);

 //QObject::connect(this, SIGNAL(SignalProcessEnd()), DeviceCamera.get()   , SLOT(SlotDeinitCamera())  , Qt::QueuedConnection);
 QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);

qDebug() << TAG_NAME.c_str() << "[ CREATE PROCESS CONTROLLER END]";
qDebug() << Qt::endl;

}

ProcessControllerClass::~ProcessControllerClass()
{
  qDebug() << TAG_NAME.c_str() << "DELETE PROCESS CONTROLLER" << QThread::currentThread();
  emit SignalProcessEnd(); QThread::sleep(2);

  ThreadProcess.quit(); ThreadProcess.deleteLater();
   ThreadCamera.quit(); ThreadCamera.deleteLater();

                           QThread::sleep(2);

  qDebug() << TAG_NAME.c_str() << "DELETE PROCESS CONTROLLER END";
  qDebug() << Qt::endl;
}


void ProcessControllerClass::StopAllProcess()
{
}

void ProcessControllerClass::DeleteModulesLinks()
{
   //ModuleImageProc->CutLink();
   //ModuleImageProc2->CutLink();
   //ModuleAiming1->CutLink();
}

void ProcessControllerClass::slotSetProcessAiming(bool OnOff)
{
   if(!OnOff) return; DeleteModulesLinks(); StopAllProcess();

   ModuleAiming1->SetAimingRegim(AimingLoop);
   ModuleAiming2->SetAimingRegim(AimingLoop);

   ModuleAiming1->SetAimingCorrection(QPair<float,float>(63,71)); //SET CENTER WHEN AIMING DIRECT

   ModuleImageProc->SetImageParam(2*2*255,50*50*255,6);

   DeviceCamera | ModuleImageProc  | ModuleAiming1 | DeviceRotary;                     

     ModuleAiming1->SetModuleEnabled(true);
   ModuleImageProc->SetModuleEnabled(false);

}

void ProcessControllerClass::slotSetProcessAiming2(bool OnOff)
{
   if(!OnOff) return; DeleteModulesLinks(); StopAllProcess();

   ModuleAiming1->SetAimingRegim(AimingDirect); ModuleAiming1->SetAimingCorrection(QPair<float,float>(-13,-2));
   ModuleAiming2->SetAimingRegim(AimingLoop);

   ModuleImageProc2->SetSlaveMode(ModuleImageProc.get());
   ModuleImageProc2->SetImageParam(2*2*255,20*20*255,6);
   ModuleImageProc2->SetThreshold(120);

     DeviceCamera | ModuleImageProc  | ModuleAiming2->PortSetAiming;
     DeviceCamera | ModuleImageProc2 | ModuleAiming2 | DeviceRotary;

     DeviceCamera | ModuleImageProc | ModuleAiming1 | DeviceRotary;

}

void ProcessControllerClass::slotSetProcessCamera(bool OnOff)
{
   DeviceCamera | ModuleImageProc;  
}

void ProcessControllerClass::slotStartProcessRTSP(bool OnOff)
{
   ModuleVideoOutput->linkToSource(DeviceCamera.get());
}

void ProcessControllerClass::slotSetProcessImitation(bool OnOff)
{
   ModuleImitatorImage | ModuleImageProc; 
   ProcessState = ProcessStateList::ProcessAiming; 
}
