#include "debug_output_filter.h"
#include "engine_tcp_interface.h"
#include "scanator_control_class.h"
using namespace std;
#define TAG "[ ENGINE CTRL ]" 

OutputFilter Filter(100);

ScanatorControlClass::~ScanatorControlClass() { qDebug() << TAG << "DELETE"; }

ScanatorControlClass::ScanatorControlClass(QObject* parrent) : QObject(parrent)
{
  StateBlock = StateBlockAtWork;
  PortMoveRelative = std::make_shared<PortAdapter<ScanatorControlClass>>();
  PortMoveRelative->LinkAdapter(this, &ScanatorControlClass::SlotMoveToPosRelative, &ScanatorControlClass::GetRelativePos);

  EnginePort = new UDPEngineInterface("192.168.0.178",2328); 
  EnginePort->BindTo(QHostAddress::Any,1313);

  QObject::connect(DebugPort, SIGNAL(SignalMessageAvailable()), this, SLOT(SlotReceiveTestSignal()));

  AngleToDAC = TransformCoordClass(SettingsRegister::GetValue("GAIN_ANGLE_DAC"),
                                   SettingsRegister::GetValue("DAC_OFFSET"));
  qDebug() << "GAIN ANGLE DAC: " << SettingsRegister::GetValue("GAIN_ANGLE_DAC");
  qDebug() << "DAC OFFSET    : " << SettingsRegister::GetValue("DAC_OFFSET");

  //SetToNullReal(); 

  PortMoveVelocity | *this; // LINK INTEGRATOR TO SlotMoveToPos, EMULATE MOVE WITH VELOCTIY

}

bool ScanatorControlClass::isAtLimit() { return false; }
bool ScanatorControlClass::isEnabled() { return StateBlock == StateBlockAtWork; }

const QPair<double, double>& ScanatorControlClass::GetOutput() { return ScanatorPos; }

void ScanatorControlClass::SetInput(const QPair<double,double>& Coord) 
{ 
  this->SlotMoveToPos(Coord); 
}

void ScanatorControlClass::SetBlockEnabled(bool OnOff)
{
  if( OnOff) StateBlock = StateBlockAtWork;
  if(!OnOff) StateBlock = StateBlockDisabled;
}

void ScanatorControlClass::SlotMoveVector(const QPair<double,double>& Pos, 
                                          const QPair<double,double>& Vel, 
                                          const QPair<double,double>& Accel, 
                                          const QPair<double,double>& RelPos)
  {

  ScanatorPos   = Pos; 
  ScanatorPos   = Vel; 
  ScanatorVel   = Accel; 
  ScanatorError = RelPos;

  CommandChannel1->Position     = ScanatorPos.first    * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel2->Position     = ScanatorPos.second   * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel1->Velocity     = ScanatorVel.first    * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel2->Velocity     = ScanatorVel.second   * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel1->Acceleration = ScanatorAccel.first  * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel2->Acceleration = ScanatorAccel.second * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel1->PositionRel  = ScanatorError.first  * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel2->PositionRel  = ScanatorError.second * AngleToDAC.Scale + AngleToDAC.Offset;

  EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SetToNullReal() 
{ 
  CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
  CommandChannel1->Position = NullRelativePos.first*0;
  CommandChannel2->Position = NullRelativePos.second*0;
  ScanatorPos = NullAbsolutePos; 
  if(EnginePort != 0) EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SetToNullWork()  
{
    CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
    CommandChannel1->Position = NullRelativePos.first*0;
    CommandChannel2->Position = NullRelativePos.second*0;
    ScanatorPos = NullRelativePos; 
    if(EnginePort != 0) EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 

}

void ScanatorControlClass::SlotMoveToPosRelative(const QPair<double, double>& Pos)
{
    if(StateBlock != StateBlockAtWork) return;
    
    ScanatorPosCorrection = Pos;

    CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
    CommandChannel1->Position = (Pos.first + ScanatorPos.first)  * AngleToDAC.Scale + AngleToDAC.Offset;
    CommandChannel2->Position = (Pos.second + ScanatorPos.second) * AngleToDAC.Scale + AngleToDAC.Offset;

    qDebug() << TAG_NAME << "[SET POS RELATIVE]" << CommandChannel1->Position << CommandChannel2->Position;

    if(EnginePort != 0)
       EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SlotMoveToPos(const QPair<double, double>& Pos)
{
    if(StateBlock != StateBlockAtWork) return;
  
    //ScanatorPos = Pos + NullAbsolutePos; 
    ScanatorPos = Pos ; 
    CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
    CommandChannel1->Position = ScanatorPos.first  ;
    CommandChannel2->Position = -ScanatorPos.second ;

    //qDebug() << TAG_NAME << "[SET POS]" << ScanatorPos.first << ScanatorPos.second;
//    qDebug() << TAG_NAME << " SET POS: " << CommandScanator.DATA.StateChannel1.Position 
//                                         << CommandScanator.DATA.StateChannel2.Position;
    if(EnginePort != 0)
       EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 

}

void ScanatorControlClass::SlotMoveOnStep(const QPair<double, double>& StepVector)
{
  if(StateBlock != StateBlockAtWork) return;

  CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
  ScanatorPos = ScanatorPos + StepVector; 

  //CommandChannel1->Position = ScanatorPos.first  * AngleToDAC.Scale + AngleToDAC.Offset;
  //CommandChannel2->Position = ScanatorPos.second * AngleToDAC.Scale + AngleToDAC.Offset;

  CommandChannel1->Position = ScanatorPos.first ;
  CommandChannel2->Position = -ScanatorPos.second;

  //qDebug() << TAG_NAME << " SET POS: " << CommandChannel1->Position 
  //                                     << CommandChannel2->Position;

  if(EnginePort != 0)
     EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SlotMoveWithVelocity(const QPair<double, double>& VelVector)
{
  if(StateBlock != StateBlockAtWork) return;

  ScanatorVel = VelVector; 
  CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
  CommandChannel1->Velocity = ScanatorVel.first  * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel2->Velocity = ScanatorVel.second * AngleToDAC.Scale + AngleToDAC.Offset;
  EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::SlotMoveWithAcceleration(const QPair<double, double>& AccelVector)
{
  if(StateBlock != StateBlockAtWork) return;

  ScanatorAccel = AccelVector; 
  CommandChannel1->ClearStruct(); CommandChannel2->ClearStruct();
  CommandChannel1->Acceleration = ScanatorAccel.first  * AngleToDAC.Scale + AngleToDAC.Offset;
  CommandChannel2->Acceleration = ScanatorAccel.second * AngleToDAC.Scale + AngleToDAC.Offset;
  EnginePort->SlotSendCommand(CommandScanator.toByteArray()); 
}

void ScanatorControlClass::LoadSettings()
{
  auto RotateParamCamEng = SettingsRegister::GetString(QString("ROTATE_CAM_ENG"));
  auto RotateParamEngCam = SettingsRegister::GetString(QString("ROTATE_ENG_CAM"));

  this->RotEngineToCamera.LoadRotationFromFile(RotateParamEngCam);
}
