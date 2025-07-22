#include "controller_process_class.h"
#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include "debug_output_filter.h"
#include "interface_image_source.h"
#include "interface_pass_coord.h"
#include "rotation_find_executor.h"
#include "state_block_enum.h"
#include "thread_operation_nodes.h"

using namespace std;

ProcessControllerClass* ProcessControllerClass::ProcessControllerInstance = nullptr;
shared_ptr<ImageTrackerCentroid>          ProcessControllerClass::ModuleImageProc;
shared_ptr<ImageTrackerCentroid>          ProcessControllerClass::ModuleImageProc2;
shared_ptr<ImageTrackerCentroidGPU>          ProcessControllerClass::ModuleImageProc3;

//shared_ptr<CameraInterfaceAravis> ProcessControllerClass::DeviceCamera;
shared_ptr<CameraInterfaceHIK> ProcessControllerClass::DeviceCamera;

shared_ptr<AimImageImitatorClass>      ProcessControllerClass::ModuleImitatorImage;


shared_ptr<AimingClass>                ProcessControllerClass::ModuleAiming1;
shared_ptr<AimingClass>                ProcessControllerClass::ModuleAiming2;

shared_ptr<ScanatorControlClass>       ProcessControllerClass::DeviceScanator;

shared_ptr<RotationFindProcessClass>  ProcessControllerClass::ProcessFindRotation;


ProcessControllerClass* ProcessControllerClass::GetInstance(QObject* parent)
{
    if(ProcessControllerInstance == nullptr) ProcessControllerInstance = new ProcessControllerClass(parent);
                                      return ProcessControllerInstance;
}

ProcessControllerClass::ProcessControllerClass(QObject* parrent): QObject(parrent)
{
qDebug() << "[ CREATE PROCESS CONTROLLER ]";

ModuleImageProc     = make_shared<ImageTrackerCentroid>();
ModuleImageProc2    = make_shared<ImageTrackerCentroid>();
ModuleImageProc3    = make_shared<ImageTrackerCentroidGPU>();

//DeviceCamera        = make_shared<CameraInterfaceAravis>();
DeviceCamera        = make_shared<CameraInterfaceHIK>();
DeviceScanator      = make_shared<ScanatorControlClass>();

ModuleImitatorImage = make_shared<AimImageImitatorClass>();
ModuleImitatorImage = make_shared<AimImageImitatorClass>();
ModuleAiming1    = make_shared<AimingClass>();
ModuleAiming2    = make_shared<AimingClass>();

ProcessFindRotation  = make_shared<RotationFindProcessClass>();

//DeviceCamera        = make_shared<CameraInterfaceHIK>();
 //DeviceCamera->moveToThread(&ThreadCamera);
 //ThreadCamera.start();
 //ThreadCamera.setPriority(QThread::HighPriority);

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

// SlotSetProcessCamera(true);
// SlotSetProcessImitation(true);
   SlotSetProcessAiming(true);

 QObject::connect(this, SIGNAL(SignalProcessEnd()), DeviceCamera.get(), SLOT(SlotDeinitCamera()), Qt::QueuedConnection);
 QObject::connect(this, SIGNAL(SignalProcessEnd()), ModuleImageProc.get(), SLOT(SlotStopProcessing()), Qt::QueuedConnection);

}

ProcessControllerClass::~ProcessControllerClass()
{
  qDebug() << "DELETE PROCESS CONTROLLER" << QThread::currentThread();
  emit SignalProcessEnd(); QThread::sleep(2);

  ThreadProcess.quit(); ThreadProcess.deleteLater();
   ThreadCamera.quit(); ThreadCamera.deleteLater();

                           QThread::sleep(2);

  qDebug() << "DELETE PROCESS CONTROLLER END";
}


void ProcessControllerClass::StopAllProcess()
{
   if(ProcessFindRotation->isEnabled()) ProcessFindRotation->StopProcess();
}

void ProcessControllerClass::DeleteModulesLinks()
{
   //ModuleImageProc->CutLink();
   //ModuleImageProc2->CutLink();
   //ModuleAiming1->CutLink();
}

void ProcessControllerClass::SlotSetProcessAiming(bool OnOff)
{
   if(!OnOff) return; DeleteModulesLinks(); StopAllProcess();

   ModuleAiming1->SetAimingRegim(AimingLoop);
   ModuleAiming2->SetAimingRegim(AimingLoop);

   ModuleAiming1->SetAimingCorrection(QPair<double,double>(63,71)); //SET CENTER WHEN AIMING DIRECT

   ModuleImageProc->SetImageParam(2*2*255,50*50*255,6);

   DeviceCamera | ModuleImageProc  | ModuleAiming1 | DeviceScanator;                     
   //ModuleImitatorImage | ModuleImageProc;

}

void ProcessControllerClass::SlotSetProcessAiming2(bool OnOff)
{
   if(!OnOff) return; DeleteModulesLinks(); StopAllProcess();

   ModuleAiming1->SetAimingRegim(AimingDirect); ModuleAiming1->SetAimingCorrection(QPair<double,double>(-13,-2));
   ModuleAiming2->SetAimingRegim(AimingLoop);

   ModuleImageProc2->SetSlaveMode(ModuleImageProc.get());
   ModuleImageProc2->SetImageParam(2*2*255,20*20*255,6);
   ModuleImageProc2->SetThreshold(120);

     DeviceCamera | ModuleImageProc  | ModuleAiming2->PortSetAiming;
     DeviceCamera | ModuleImageProc2 | ModuleAiming2 | DeviceScanator;

     DeviceCamera | ModuleImageProc | ModuleAiming1 | DeviceScanator;

}

void ProcessControllerClass::SlotSetProcessCamera(bool OnOff)
{
   DeviceCamera | ModuleImageProc;  
}

void ProcessControllerClass::SlotSetProcessImitation(bool OnOff)
{
   ModuleImitatorImage | ModuleImageProc; 
   ProcessState = ProcessStateList::ProcessAiming; 
}
