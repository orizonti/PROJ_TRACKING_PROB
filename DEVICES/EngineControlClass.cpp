#include "debug_output_filter.h"
#include "scanator_control_class.h"
using namespace std;
#define TAG "[ ENGINE CTRL ]" 
int ScanatorControlClass::EngineCount = 0;

OutputFilter PrintFilter(20);
//template<> PassValueClass<double>& PassValueClass<double>::operator>>(double& Receiver) { Receiver = this->GetValue(); return *this;};


ScanatorControlClass::~ScanatorControlClass() { qDebug() << TAG << "DELETE"; }

ScanatorControlClass::ScanatorControlClass(QObject* parrent) : QObject(parrent)
{
	ScanatorControlClass::EngineCount++; EngineNumber = EngineCount; 
  StateBlock = StateBlockAtWork;

  EngineRemoteInterface = new UDPEngineInterface(2323);

  EngineRemoteInterface->RemoteHost = QString("192.168.0.178");
  EngineRemoteInterface->RemotePort = 2323;
  qDebug() << "CREATE ENGINE CONTROL: " << EngineRemoteInterface->RemoteHost;

  SlotMoveToPos(this->NullRelativePos);
}

bool ScanatorControlClass::isEngineLimit() { return false; }



bool ScanatorControlClass::isEngineValid()
{
  return StateBlock != StateBlockFault &&
         StateBlock != StateBlockBroken && 
         StateBlock != StateBlockNoConnect;
}

bool ScanatorControlClass::isEngineFault() { return !isEngineValid(); }


//EngineData ScanatorControlClass::GetEnginesState()
//{
//	return Data;
//}


void ScanatorControlClass::SetCoordAbs(QPair<double, double> CoordSeconds)
{
   decltype(CoordSeconds) CoordRadian;
   CoordSeconds >> SecondsToRadian >> CoordRadian;
   //this->EngineInterface->SetCoordAbs(CoordRadian);
}

void ScanatorControlClass::SetInput(const QPair<double,double>& Coord) 
{ 
  //qDebug() << "ENGINE SET COORD: " << Coord.first << Coord.second;
  //this->SlotMoveToPosRelative(Coord, NullRelativePos);
  this->SlotMoveToPos(Coord);
}
const QPair<double, double>& ScanatorControlClass::GetOutput() 
{ 
  return ScanatorPos;
}

void ScanatorControlClass::SetToNullReal()
{
	qDebug() << TAG << "[ SET TO NULL ABS ]";

  ScanatorPos = NullAbsolutePos;

  CommandScanator.DATA.StateChannel1.Position = ScanatorPos.first;
  CommandScanator.DATA.StateChannel2.Position = ScanatorPos.second;
  EngineRemoteInterface->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SetToNullWork()
{
	qDebug() << TAG << "[ SET TO NULL REL ]";

  ScanatorPos = NullRelativePos;

  CommandScanator.DATA.StateChannel1.Position = ScanatorPos.first;
  CommandScanator.DATA.StateChannel2.Position = ScanatorPos.second;
  EngineRemoteInterface->SlotSendCommand(CommandScanator.toByteArray()); 
}


void ScanatorControlClass::SlotMoveToPosRelative(QPair<double, double> Pos, QPair<double,double> PosRel)
{
   SlotMoveOnStep(Pos + PosRel);
}

void ScanatorControlClass::SlotMoveToPos(QPair<double, double> Pos)
{
    Counter++;
    CommandScanator.DATA.StateChannel1.Position     = Pos.first;
    CommandScanator.DATA.StateChannel1.Velocity     = 0;
    CommandScanator.DATA.StateChannel1.Acceleration = 0;
    CommandScanator.DATA.StateChannel1.PositionRel  = 0;

    CommandScanator.DATA.StateChannel2.Position     = Pos.second;
    CommandScanator.DATA.StateChannel2.Velocity     = 0;
    CommandScanator.DATA.StateChannel2.Acceleration = 0;
    CommandScanator.DATA.StateChannel2.PositionRel  = 0;

    ScanatorPos = Pos;
    //if(Counter % 100 == 0) qDebug() << "MESS: " << CommandScanator.toByteArray();
    //EngineRemoteInterface->SlotSendCommand(CommandScanator.toByteArray()); 
    qDebug() << "MOTOR SET POS: " << ScanatorPos.first << ScanatorPos.second;
}

void ScanatorControlClass::SlotMoveOnStep(QPair<double, double> StepVector)
{
	std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();
	TimeFromLastCommand = TimePoint;

  ScanatorPos += StepVector;

  Counter++; if(Counter % 100 == 0) qDebug() << "ENGINE POS: " << ScanatorPos.first << ScanatorPos.second;

  CommandScanator.DATA.StateChannel1.Position = ScanatorPos.first;
  CommandScanator.DATA.StateChannel2.Position = ScanatorPos.second;
  EngineRemoteInterface->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SlotMoveWithVelocity(QPair<double, double> VelVector)
{

	auto TimePoint = std::chrono::high_resolution_clock::now();
	StepPeriod = std::chrono::duration<double>((TimePoint - TimeFromLastCommand)).count(); 

	TimeFromLastCommand = TimePoint; if(StepPeriod*1000 > 20) return;

  //EngineInterface->GetOutputControlSeconds() >> RotEngineToCamera >> SpotCoordFromEngine;

	QPair<double, double> StepVector(VelVector.first*StepPeriod, VelVector.second*StepPeriod);

  //CheckSpotRunaway(SpotCoordFromEngine, StepVector); //IsSpotRunaway();
  //if(!MovingBoundryCamera.isContainsPoint(SpotCoordFromEngine) && IsSpotRunaway()) 
  //{
  //qDebug() << PrintFilter3 << TAG << EngineNumber << "AT LIMIT";
  //return; 
  //}

	//if(StateBlock == StateBlockAtWork) StepVector >> RotCameraToEngine >> *EngineInterface; 
}


void ScanatorControlClass::SavePosition() 
{ 
  //if(!DetectorMissmatchCoord.isEngineOperable() || !DetectorMissmatchSpeed.isEngineOperable() || !isEngineLimit()) return;
  //     EngineInterface->ControlCoord >> CheckPointsStat;
  //     if(CheckPointsStat.IsCoordLoaded()) StaticSavePosition[1] = CheckPointsStat.GetAvarageCoord();
}




void ScanatorControlClass::RestoreFromFault()
{
}

void ScanatorControlClass::SlotMoveWithAcceleration(QPair<double, double> AccelVector)
{
	std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();

	StepPeriod = std::chrono::duration<double>((TimePoint - TimeFromLastCommand)).count();
	TimeFromLastCommand = TimePoint;

	VelVector.first += AccelVector.first*StepPeriod;
	VelVector.second += AccelVector.second*StepPeriod;
	//qDebug() << TAG << "CONVERT ACCELERATION - " <<AccelVector.first << VelVector.first <<"STEP_PERIOD - " << StepPeriod;

	QPair<double, double> StepVector(VelVector.first*StepPeriod, VelVector.second*StepPeriod); // CALCULATION PROLONGATION STEP TO MOVE ENGINE
	//StepVector >> RotCameraToEngine >> *EngineInterface;
}


void operator>>(AimingClass& Sender, ScanatorControlClass& Reciever)
{

 if (  Sender.StateBlock != StateBlockAtWork) return;
 if (Reciever.StateBlock == StateBlockAtWork) Reciever.SlotMoveWithVelocity(Sender.GetOutput());
}

void ScanatorControlClass::LoadSettings()
{
  //auto RotateParamEngCam = SettingsRegister::GetString(QString("ROTATE_ENG_CAM"));
  //auto NullPosFile       = SettingsRegister::GetString(QString("NULL_ENGINES_POS"));
  //this->RotEngineToCamera.LoadRotationFromFile(RotateParamEngCam);
}
