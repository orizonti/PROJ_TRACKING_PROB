#ifndef SCANATOR_CONTROL_CLASS_H
#define SCANATOR_CONTROL_CLASS_H

//#include "RotateOperationContainer.h"
#include "interface_pass_coord.h"
#include "thread_operation_nodes.h"
#include "rotate_vector_class.h"
#include "state_block_enum.h"
#include "transform_coord_class.h"

#include <QSettings>
#include <QTimer>
#include "engine_udp_interface.h"
#include "engine_tcp_interface.h"
#include "message_command_structures.h"
#include "message_struct_generic_ext.h"
#include "message_header_generic_ext.h"
#include "device_generic_interface.h"
#include "debug_output_filter.h"
#include "sinus_generator_class.h"

class DynamicModule: public QObject
{
  Q_OBJECT
  public:
  explicit DynamicModule(DeviceRotaryInterface* DeviceRotary) 
  { 
    Device = DeviceRotary; QObject::connect(&timerMove, &QTimer::timeout, this, &DynamicModule::slotMove); 
  }

  virtual void enableMove(bool OnOff) { if(OnOff && timerMove.isActive()) return; if(OnOff) timerMove.start(1); else timerMove.stop(); }
  public slots:
  virtual void slotMove() { Device->moveOnStep(Device->getVelocity()); }
  protected:
  QTimer timerMove;
  DeviceRotaryInterface* Device = nullptr;
};

class SinusMoveModule : public DynamicModule , public PassCoordClass<float>
{
  Q_OBJECT
  public:
  explicit SinusMoveModule(DeviceRotaryInterface* DeviceRotary) : DynamicModule(DeviceRotary) 
  { 
    SinusGenerator | *this;
  }
  SinusGeneratorClass SinusGenerator;
  QPair<float,float> Position{0,0};

  void enableMove(bool OnOff) { if(OnOff && SinusGenerator.isActive()) return; if(OnOff) SinusGenerator.slotStartGenerate(true); 
                                                                                    else SinusGenerator.slotStartGenerate(false); }

	void setInput(const QPair<float,float>& Coord) { Position = Coord; slotMove(); } 
	const QPair<float,float>& getOutput() { return Position;};
  public slots:
  void slotMove() { Device->moveToPos(Position); }
};


template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
class DeviceRotaryControl : public DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>, public DeviceRotaryInterface
{
public:
  using DEVICE_BASE_TYPE = DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>; 
  explicit DeviceRotaryControl(std::shared_ptr<T_CONNECTION> Connection, QString Name = "[ DEVICE ]");

	QString TAG_NAME{"[ ROTARY ]"};

	~DeviceRotaryControl() { qDebug() << TAG_NAME << "DELETE"; }

	QPair<int, int> LimitDown{0,0};
	QPair<int, int> LimitUp  {50000, 50000};
	QPair<int, int> Range{LimitUp - LimitDown};
	QPair<int, int> RangeOffset {0,0};

	QPair<int, int> PositionNull {0,0};
	QPair<int, int> Position{0,0};
	QPair<int, int> PositionTarget{0,0};
	QPair<int, int> Distance{0,0};
	QPair<int, int> PositionRotated{0,0};
	QPair<int, int> Velocity {0,0};

  //===============================================================================================
	void moveToPos(const QPair<int, int>& Pos) override;
	void moveToPosRelative(const QPair<int, int>& Pos) override;
	void moveOnStep(const QPair<int, int>& Pos) override;
	void moveWithVelocity(const QPair<int, int>& VelocityVector) override { Velocity = VelocityVector; ModuleMoveVelocity.enableMove(true); }
	void      setVelocity(const QPair<int, int>& VelocityVector) override { Velocity = VelocityVector;};
	void stopMove()  override { ModuleMoveVelocity.enableMove(false); ModuleMoveSinus.enableMove(false); }

	void moveSinus(bool OnOff) { ModuleMoveSinus.enableMove(OnOff); } ;

	          bool checkRangeOffset(QPair<int,int> Pos) override;
	QPair<int,int> getLimits(int axis) override;
	QPair<int,int> getRange()  override { return Range; }
	const QPair<int,int>& getPos()      { return Position;} ;
	const QPair<int,int>& getVelocity() { return Velocity;};

  bool isAtLimit();
	void setToNull();
  //===============================================================================================

	void setParam(uint8_t ID, uint16_t Value) override;
	void setParams(uint16_t Value1, uint16_t Value2) override;
	void setParams(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t Value4) override; 

  void putMessage(T_MESSAGE Message) override {}; 
  //===============================================================================================
  void loadSettings();


std::shared_ptr<PortAdapter<DeviceRotaryInterface>> PortMoveRelative = nullptr;

private:
	RotateVectorClass<int>   RotAxis;
	DynamicModule   ModuleMoveVelocity{this};
	SinusMoveModule ModuleMoveSinus{this};
};

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
DeviceRotaryControl<T_CONNECTION, T_COMMAND, T_MESSAGE>::DeviceRotaryControl(std::shared_ptr<T_CONNECTION> Connection, QString Name): 
DeviceGenericInterface<T_CONNECTION,T_COMMAND, T_MESSAGE>(Connection, Name)
{
	setToNull();

  PortMoveRelative = std::make_shared<PortAdapter<DeviceRotaryInterface>>();
  PortMoveRelative->linkAdapter(this, &DeviceRotaryInterface::moveToPosRelative, 
                                      &DeviceRotaryInterface::getPos);
  PortMoveRelative | *this; // LINK INTEGRATOR TO moveToPos, EMULATE MOVE WITH VELOCTIY
}


template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
bool DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::isAtLimit() { return checkRangeOffset(Position); }


template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::setToNull() 
{ 
                                   Position = PositionNull; 
  DEVICE_BASE_TYPE::Command.DATA = Position;
  DEVICE_BASE_TYPE::ConnectionDevice->slotSendMessage(DEVICE_BASE_TYPE::Command.toByteArray()); 
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
bool DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::checkRangeOffset(QPair<int, int> Pos)
{
     RangeOffset = abs_pair(Pos - LimitDown) + abs_pair(LimitUp - Pos);
  if(RangeOffset.second > Range.second || RangeOffset.first  > Range.first ) return true;
  return false;
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveOnStep(const QPair<int, int>& Step)
{
                         PositionTarget = Position + Step;
  RangeOffset = abs_pair(PositionTarget - LimitDown) + abs_pair(LimitUp - PositionTarget);
     
  if(RangeOffset.first  > Range.first ) PositionTarget.first  = Position.first; 
  if(RangeOffset.second > Range.second) PositionTarget.second = Position.second;

                                   Position = PositionTarget;
  DEVICE_BASE_TYPE::Command.DATA = Position;
  DEVICE_BASE_TYPE::ConnectionDevice->slotSendMessage((const char*)(&DEVICE_BASE_TYPE::Command.DATA),
                                                              sizeof(DEVICE_BASE_TYPE::Command.DATA),2); 

}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveToPos(const QPair<int, int>& Pos)
{
                         PositionTarget = Pos;
  RangeOffset = abs_pair(PositionTarget - LimitDown) + abs_pair(LimitUp - PositionTarget);
     
  if(RangeOffset.first >Range.first ) PositionTarget.first =Pos.first <0 ?LimitDown.first  :LimitUp.first; 
  if(RangeOffset.second>Range.second) PositionTarget.second=Pos.second<0 ?LimitDown.second :LimitUp.second;


                                   Position = PositionTarget;
  DEVICE_BASE_TYPE::Command.DATA = Position;

   //qDebug() << OutputFilter::Filter(4) << "[ GET COMMAND SET POS SCANATOR ]" << Position.first 
   //                                                                            << Position.second;
  DEVICE_BASE_TYPE::ConnectionDevice->slotSendMessage((const char*)(&DEVICE_BASE_TYPE::Command.DATA),
                                                              sizeof(DEVICE_BASE_TYPE::Command.DATA),2); 
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveToPosRelative(const QPair<int, int>& PosRelative) 
{
                                     auto PositionAbs = Position + PosRelative;
                         PositionTarget = PositionAbs;
  RangeOffset = abs_pair(PositionTarget - LimitDown) + abs_pair(LimitUp - PositionTarget);
     
  if(RangeOffset.first >Range.first ) PositionTarget.first = PositionAbs.first <0 ?LimitDown.first  :LimitUp.first; 
  if(RangeOffset.second>Range.second) PositionTarget.second= PositionAbs.second<0 ?LimitDown.second :LimitUp.second;

  DEVICE_BASE_TYPE::Command.DATA = PositionTarget;
  DEVICE_BASE_TYPE::ConnectionDevice->slotSendMessage((const char*)(&DEVICE_BASE_TYPE::Command.DATA),
                                                              sizeof(DEVICE_BASE_TYPE::Command.DATA),2); 

}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::loadSettings()
{
  //auto RotateParamCamEng = SettingsRegister::GetString(QString("ROTATE_CAM_ENG"));
  //auto RotateParamEngCam = SettingsRegister::GetString(QString("ROTATE_ENG_CAM"));
  //this->RotEngineToCamera.LoadRotationFromFile(RotateParamEngCam);
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
QPair<int,int> DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::getLimits(int axis)
{
   QPair<int,int> Limits(LimitDown.first, LimitUp.first);
    if(axis == 1) Limits = QPair<int,int>(LimitDown.second, LimitUp.second);
    return Limits;
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION, T_COMMAND, T_MESSAGE>::setParam(uint8_t ID, uint16_t Value) 
{
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION, T_COMMAND, T_MESSAGE>::setParams(uint16_t Value1, uint16_t Value2) 
{
    DEVICE_BASE_TYPE::Command.DATA.Param1 = Value1;
    DEVICE_BASE_TYPE::Command.DATA.Param2 = Value2;
    DEVICE_BASE_TYPE::sendCommand();
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION, T_COMMAND, T_MESSAGE>::setParams(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t Value4) 
{
    DEVICE_BASE_TYPE::Command.DATA.Param1 = Value1;
    DEVICE_BASE_TYPE::Command.DATA.Param2 = Value2;
    DEVICE_BASE_TYPE::sendCommand();
}

#endif 
