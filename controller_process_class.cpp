#include "controller_process_class.h"
#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include "debug_output_filter.h"
#include "interface_pass_coord.h"

using namespace std;

ProcessControllerClass* ProcessControllerClass::ProcessControllerInstance = nullptr;
shared_ptr<ImageTrackerCentroid>          ProcessControllerClass::ModuleImageProc;
shared_ptr<ImageTrackerCentroid>          ProcessControllerClass::ModuleImageProc2;
shared_ptr<AimingClass>                ProcessControllerClass::ModuleAimingLoop;

shared_ptr<AimImageImitatorClass>      ProcessControllerClass::ModuleImitatorImage;
shared_ptr<SinusGeneratorClass>        ProcessControllerClass::ModuleSinusGenerator;

shared_ptr<CameraInterfaceClassAravis> ProcessControllerClass::DeviceCamera;
shared_ptr<ScanatorControlClass>       ProcessControllerClass::DeviceScanator;
shared_ptr<LaserControlClass>          ProcessControllerClass::DeviceLaser;
//shared_ptr<MotorInterface>             ProcessControllerClass::DeviceImitatorMotor;

shared_ptr<RotationFindProcessClass>  ProcessControllerClass::ProcessFindRotation;

ProcessControllerClass* ProcessControllerClass::GetInstance()
{
   if(ProcessControllerInstance == nullptr) ProcessControllerInstance = new ProcessControllerClass;
   return ProcessControllerInstance;
}


ProcessControllerClass::ProcessControllerClass(QObject* parrent): QObject(parrent)
{

ModuleImitatorImage = make_shared<AimImageImitatorClass>();
ModuleAimingLoop    = make_shared<AimingClass>();
ModuleImageProc     = make_shared<ImageTrackerCentroid>();
ModuleImageProc2    = make_shared<ImageTrackerCentroid>();

DeviceCamera        = make_shared<CameraInterfaceClassAravis>();
DeviceScanator      = make_shared<ScanatorControlClass>();
//DeviceLaser         = make_shared<LaserControlClass>();
//DeviceImitatorMotor = make_shared<MotorInterface>();

ModuleSinusGenerator = make_shared<SinusGeneratorClass>();
ModuleSinusGenerator->SetTimerPeriod(5);

ProcessFindRotation  = make_shared<RotationFindProcessClass>();

   //==============================================================================

   ModuleImageProc->moveToThread(&processThread);
   processThread.start();
   processThread.setPriority(QThread::HighPriority);
   
   //==============================================================================

   ModuleSinusGenerator->LinkToDevice(*DeviceScanator);
   ModuleSinusGenerator->MoveGeneratorToThread(&utiliteThread);
   utiliteThread.start();
   utiliteThread.setPriority(QThread::NormalPriority);
   //=================================================

   //SlotSetProcessImitation(true);
   SlotSetProcessAiming(true);

}

void ProcessControllerClass::DisplayState()
{
   if(ProcessState == ProcessStateList::ProcessAiming) qDebug() << TAG_NAME << "[ SET PROCESS AIMING ]";
   if(ProcessState == ProcessStateList::ProcessImitation) qDebug() << TAG_NAME << "[ SET PROCESS IMITATION ]";
   if(ProcessState == ProcessStateList::ProcessTestSignal) qDebug() << TAG_NAME << "[ SET PROCESS TEST SIGNAL ]";
}

void ProcessControllerClass::SlotSetProcessAiming(bool OnOff)
{
   if(!OnOff) return;

   //DeviceCamera | ModuleImageProc | ModuleAimingLoop | DeviceScanator;  ProcessState = ProcessStateList::ProcessAiming; DisplayState();
  

   DeviceCamera | ModuleImageProc  | ModuleAimingLoop | DeviceScanator; ProcessState = ProcessStateList::ProcessAiming; DisplayState();
   //DeviceCamera | ModuleImageProc2 ; 
   //DeviceCamera | ModuleImageProc2 | DeviceScanator; 

}

void ProcessControllerClass::SlotSetProcessImitation(bool OnOff)
{
   if(!OnOff) return;

                                           //ModuleAimingLoop->SetBlockEnabled(false);
   //ModuleImitatorImage | ModuleImageProc | ModuleAimingLoop | DeviceScanator; ProcessState = ProcessStateList::ProcessImitation; DisplayState();
   ModuleImitatorImage | ModuleImageProc; 
   ModuleImitatorImage | ModuleImageProc2; 
}

void ProcessControllerClass::SlotSetProcessTestSignal(bool OnOff)
{
  if(!OnOff) return;

  ModuleSinusGenerator | DeviceScanator; ProcessState = ProcessStateList::ProcessTestSignal; DisplayState();
}


void ProcessControllerClass::SlotStartProcessRotFind(bool OnOff)
{
   if(!OnOff) return;

   qDebug() << TAG_NAME << "[ SET PROCESS FIND ROTATION:  ]" << DeviceScanator->TAG_NAME << ModuleImageProc->TAG_NAME;
                                          
   ProcessFindRotation->SetRotatedModules(DeviceScanator, ModuleImageProc);
   ProcessFindRotation->SlotStartRotationFind(true); ProcessState = ProcessStateList::ProcessCalibration;

}

void ProcessControllerClass::SlotStartProcessRotFindRevert(bool OnOff)
{
   if(!OnOff) return;

   qDebug() << TAG_NAME << "[ SET PROCESS FIND ROTATION:  ]" << ModuleAimingLoop->TAG_NAME << DeviceScanator->TAG_NAME;
   ProcessFindRotation->SetRotatedModules(ModuleAimingLoop, DeviceScanator);
   ProcessFindRotation->SlotStartRotationFind(true); ProcessState = ProcessStateList::ProcessCalibration;
}

