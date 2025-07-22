#include "debug_output_filter.h"
#include "rotation_find_executor.h"

using namespace std;
#define TAG "[ ROT_FIND ]" 

RotationFindProcessClass::RotationFindProcessClass(QObject* parrent)
{
  qDebug() << "[ CREATE ROTATION EXECUTOR EMPTY]";
}

RotationFindProcessClass::RotationFindProcessClass(std::shared_ptr<PassCoordClass<double>> Base, std::shared_ptr<PassCoordClass<double>> Rot,QObject* parrent) : QObject(parrent)
{
  BaseObject = Base;
	RotationObject = Rot;
}

void RotationFindProcessClass::SetRotatedModules(std::shared_ptr<PassCoordClass<double>> Base, std::shared_ptr<PassCoordClass<double>> Rot)
{
  BaseObject = Base;
	RotationObject = Rot;
}

void RotationFindProcessClass::SetRotatedModules(std::shared_ptr<PassCoordClass<double>> Base, std::shared_ptr<PassCoordClass<double>> Rot,std::shared_ptr<PassCoordClass<double>> Middle)
{
  BaseObject = Base;
	RotationObject = Rot;
  MiddleObject = Middle;
}

void RotationFindProcessClass::StopProcess()
{
    qDebug() << TAG_NAME << "[ STOP FINE ROTATION PROCEDURE ]";
    timerStepper.stop();
    QObject::disconnect(&timerStepper, SIGNAL(timeout()), this, SLOT(SlotMakeStep()));
}
void RotationFindProcessClass::StartProcess()
{
    qDebug() << "START ROTATION FIND PROCESS";
    QObject::connect(&timerStepper, SIGNAL(timeout()), this, SLOT(SlotMakeStep()));
    timerStepper.start(10);
}

void RotationFindProcessClass::SlotStartRotationFind(bool StartStop)
{
    if(!BaseObject || !RotationObject) { qDebug() << TAG << "ROTATION OBJECTS IS NOT SET CANT START CALIBRATION"; return;};
    if(StartStop) StartProcess(); else StopProcess();
}

void RotationFindProcessClass::SlotMakeStep()
{
	auto CoordBase = BaseObject->GetOutput();     
	auto CoordRot  = RotationObject->GetOutput();

	auto AimCoord = RotationContainer.MeasureFilter.GetWaitInputCoord();
       AimCoord >> *BaseObject;

  CoordBase = CoordBase + AimCoord;

  CoordBase >> RotationContainer.MeasureFilter >> RotationContainer;
  CoordRot  >> RotationContainer.MeasureFilter >> RotationContainer;

  if(!RotationContainer.IsDataFull()) return;

  StopProcess(); QPair<double,double>(0,0) >> *BaseObject;

  QTimer::singleShot(2000,[this]()
  {
  qDebug() << "===============================";
  qDebug() << "[ PROCESS ROTATION DATA ]";
  qDebug() << "===============================";
  RotationContainer.FindArbitraryRotationToVectors(RotationContainer.input_to_optimize_rotation, 
                                                   RotationContainer.output_to_optimize_rotation);
  qDebug() << "===============================";
  });

}


