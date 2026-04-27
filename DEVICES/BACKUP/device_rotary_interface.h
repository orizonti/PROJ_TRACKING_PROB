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

#include "device_generic_interface.h"
#include "debug_output_filter.h"
#include "transform_coord_class.h"
#include "sinus_generator_class.h"
#include <QThread>

class DeviceRotaryInterface : public PassCoordClass<float>
{
	public:

	virtual void moveToPosRelative(const QPair<float, float>& Pos) = 0;
	virtual void moveOnStep       (const QPair<float, float>& Pos) = 0;
	virtual void moveToPos        (const QPair<float, float>& Pos) = 0;
	virtual void moveWithVelocity (const QPair<float, float>& Velocity) = 0;
	virtual void moveWithVelocityManual(const QPair<float, float>& Velocity) = 0;
	virtual void stopMove() = 0;

	const QPair<float, float>& getOutput() { PassCoordClass<float>::OutputCoord = getPos(); return PassCoordClass<float>::OutputCoord;};
	void setInput(const QPair<float, float>& Coord) { moveToPos(Coord); };

	virtual const QPair<float,float>& getPos() = 0;
	virtual const QPair<float,float>& getPosDevice() = 0;
	virtual const QPair<float,float>& getVelocity() = 0;
	virtual const QPair<float,float>& getVelocityDevice() = 0;

	virtual QPair<float,float> getLimits()  = 0;
};

class DynamicModule: public QObject
{
  Q_OBJECT
  public:
  explicit DynamicModule(DeviceRotaryInterface* DeviceRotary) 
  { 
    Device = DeviceRotary; QObject::connect(&timerMove, &QTimer::timeout, this, &DynamicModule::slotMove); 
    reset();
  };

  void reset()
  {
    VelocityLimit.first  = Device->getLimits().first/600;
    VelocityLimit.second = Device->getLimits().second/600;
  }

  int PeriodStep  = 2;

  QPair<float,float> VelocityLimit;
  QPair<float,float> Velocity;
 
  void moveWithVelocity(QPair<float,float> Scale) 
  { 
    Velocity = VelocityLimit*Scale; if(timerMove.isActive()) return; timerMove.start(PeriodStep); 
  }
  void stopMove() { timerMove.stop(); }

  public slots:
  void slotMove() { Device->moveOnStep(Velocity); }
  private:
  QTimer timerMove;
  protected:
  DeviceRotaryInterface* Device = nullptr;
};

class SinusMoveModule : public DynamicModule , public PassCoordClass<float>
{
  Q_OBJECT
  public:
  explicit SinusMoveModule(DeviceRotaryInterface* DeviceRotary) : DynamicModule(DeviceRotary)
  {
    QObject::connect(this, SIGNAL(signalStartMove(bool)), &SinusGenerator, SLOT(slotStartGenerate(bool)));

    SinusGenerator | *this;
    SinusGenerator.slotSetOffset(0);
    SinusGenerator.moveToAnotherThread();
  }

  QThread thread;
  SinusGeneratorClass SinusGenerator;
   QPair<float,float> Position{0,0};

  void setAmplitude(float Value) { SinusGenerator.slotSetAmplitude(Value); };
  void setFreq(float Value)      { SinusGenerator.slotSetFrequency(Value); };

  void enableMove(bool OnOff) { if(OnOff && SinusGenerator.isActive()) return; emit signalStartMove(OnOff); }

        void setInput(const QPair<float,float>& Coord) { Position = Coord; slotMove(); }
        const QPair<float,float>& getOutput() { return Position;};

  public slots:
  void slotMove() { Device->moveToPos(Position); }
  signals:
  void signalStartMove(bool);
};

template <class T>
class DeviceRotaryControlAdapter : public PassCoordClass<float>, public DeviceGenericHandleControl
{
public:
    DeviceRotaryControlAdapter(){};
    DeviceRotaryControlAdapter(T* Device,std::function<void(T&,QPair<float,float>)> SetFunction, std::function<QPair<float,float>(T&)> GetFunction)
    {linkAdapter(Device, SetFunction, GetFunction);};
    void linkAdapter(T* Device,std::function<void(T&,QPair<float,float>)> SetFunction, std::function<QPair<float,float>(T&)> GetFunction)
    { InputFunction = SetFunction; OutputFunction = GetFunction; DeviceLink = Device; };

    T* DeviceLink =  nullptr;
    std::pair<float,float> PosRotary{0,0};

    std::function<void(T&,QPair<float,float>)>  InputFunction = nullptr;
         std::function<QPair<float,float>(T&)> OutputFunction = nullptr;

    void setInput(const QPair<float,float>& coord) { if(InputFunction != nullptr) InputFunction(*DeviceLink,coord);};
    const QPair<float,float>& getOutput() { if(OutputFunction != nullptr) PassCoordClass<float>::OutputCoord = OutputFunction(*DeviceLink); 
                                                                   return PassCoordClass<float>::OutputCoord;};

                      //HANDLE CONTROL INTERFACE
	                    void setEnable(bool OnOff, uint16_t Number = 0) { DeviceLink->setEnable(OnOff,Number); };
	                    void setPair(std::pair<float,float> Coord) { PosRotary = Coord; setInput(PosRotary); };
	  std::pair<float,float> getPair() { return getOutput(); };
    void setParam (uint16_t CommandID, uint32_t CommandParam) {};
    void setParam (uint16_t CommandID, float    CommandParam) 
    {
      if(CommandID == 1) 
      {
        if(CommandParam > 180) CommandParam = CommandParam - 360;
            PosRotary.second = CommandParam; 
      }
      else PosRotary.first  = CommandParam;   setInput(PosRotary);
    };
};

enum class CONTROL_PARAM { NONE = 0, POS = 1, VEL = 2, ACCEL = 3};
template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
class DeviceRotaryControl : public DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>, 
                            public DeviceRotaryInterface
{
public:
  using DEVICE_INTERFACE = DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>; 
  using DEVICE_TYPE = DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>; 
  DeviceRotaryControl(std::shared_ptr<T_CONNECTION> Connection, CONTROL_PARAM ControlType, QString Name = "[ DEVICE ]");

  NodeCoordGain<float>   Gain{1,1};
  NodeCoordOffset<float> Offset{1,1};
  RotateVectorClass<float> Rotation;

	QString TAG_NAME{"[ ROTARY ]"};

	~DeviceRotaryControl() { qDebug() << TAG_NAME << "DELETE"; }
  //================================================

  std::shared_ptr<DeviceRotaryControlAdapter<DEVICE_TYPE>> ControlRotaryPos;
  std::shared_ptr<DeviceRotaryControlAdapter<DEVICE_TYPE>> ControlRotaryVel;

  StateRotaryControl ControlEngineTarget;
  StateRotaryControl ControlEngineState;

	std::array<QPair<float, float>,3> Limits
  {
    //QPair<float,float>{180 ,180 },
    //QPair<float,float>{100 ,100 },
    //QPair<float,float>{100 ,100 }

    QPair<float,float>{30000 ,30000 },
    QPair<float,float>{100 ,100 },
    QPair<float,float>{100 ,100 }
  };

	QPair<float, float> PositionNullDevice   {72,0};

	QPair<float, float> PositionTarget {0,0};
	QPair<float, float> PositionTargetDevice {0,0};
	QPair<float, float> PositionRelativeAnchor {0,0};

  //================================================
	QPair<float, float> VelocityTarget {0,0};
  //================================================
	QPair<float, float> PositionDevice{0,0};
	QPair<float, float> VelocityDevice{0,0};

	void moveToPos        (const QPair<float, float>& Pos) override;
	void moveToPosRelative(const QPair<float, float>& Pos) override;
	void moveOnStep       (const QPair<float, float>& Pos) override;
	void moveWithVelocity (const QPair<float, float>& VelocityVector) override; 
	void moveWithVelocityManual(const QPair<float, float>& Vel) override 
  { 
    qDebug() << TAG_NAME << "MOVE WITH VEL MANUAL: " << Vel.first << Vel.second;
    ModuleMoveVelocity.moveWithVelocity(Vel);
  }; 
	//void stopMove()  override { ModuleMoveVelocity.stopMove(); }

	void stopMove()  override 
  { 
    //moveWithVelocity(QPair<float,float>(0,0)); 
    ModuleMoveVelocity.stopMove(); 
  }
  //===============================================================================================
  //DEVICE_GENERIC_HANDLE_CONTROL
	                  void setPair(std::pair<float,float> Coord) override { moveWithVelocity(Coord);};
	std::pair<float,float> getPair()       override { return getPosDevice(); };
	void setEnable(bool OnOff, uint16_t Number = 0) { if(!OnOff) stopMove(); };
    void setParam (uint16_t CommandID, uint32_t CommandParam) {};
    void setParam (uint16_t CommandID, float    CommandParam) 
    {
      if(CommandID == 0) PositionTarget.first  = CommandParam;
      if(CommandID == 1) PositionTarget.second = CommandParam; checkPositionOffset();

                    PositionTargetDevice = PositionTarget + PositionNullDevice; 
  this->sendCommand(PositionTargetDevice);                  PositionRelativeAnchor = PositionTarget;
    };
  //===============================================================================================

	const QPair<float,float>& getPos()            { return PositionTarget;} //       POS SET TO DEVICE
	const QPair<float,float>& getPosDevice()      { return PositionDevice;} //ACTUAL POS FROM DEVICE
	const QPair<float,float>& getVelocity()       { return VelocityTarget;};
	const QPair<float,float>& getVelocityDevice() { return VelocityDevice;};

	const QPair<float, float>& getOutput() { PassCoordClass<float>::OutputCoord = getPos(); return PassCoordClass<float>::OutputCoord;};
	void setInput(const QPair<float, float>& Coord) { moveToPos(Coord); };

  bool isAtLimit();
  void checkPositionOffset();
  void checkPositionOffset(QPair<float,float>& Position);
  void checkVelocityOffset();

	QPair<float,float> getLimits() { return Limits[0]; };
  //===============================================================================================

	void setMode  (CONTROL_PARAM Mode); 
	void setPositonMode() { setMode(CONTROL_PARAM::POS); };
	void setVelocityMode(){ setMode(CONTROL_PARAM::VEL); };

  template<CONTROL_PARAM PARAM_TYPE> 
	void setLimit  (float Limit) { ControlEngineTarget.Engine1.Limits[(int)PARAM_TYPE] = Limit; 
                                 ControlEngineTarget.Engine2.Limits[(int)PARAM_TYPE] = Limit; 
                                 Limits[0].first = Limit;
                                 Limits[0].second = Limit;

                                 this->Limits[(int)PARAM_TYPE] = QPair<int,int>(Limit, Limit);
                                 //DEVICE_INTERFACE::Command.setData(ControlEngineTarget);  
                                 ModuleMoveVelocity.reset();
                                }; 

  template<CONTROL_PARAM PARAM_TYPE> 
  void setLimits (float Limit1, float Limit2) { ControlEngineTarget.Engine1.Limits[(int)PARAM_TYPE-1]= Limit1; 
                                                ControlEngineTarget.Engine2.Limits[(int)PARAM_TYPE-1]= Limit2; 

                                                this->Limits[(int)PARAM_TYPE-1] = QPair<int,int>(Limit1, Limit2);
                                                //DEVICE_INTERFACE::Command.setData(ControlEngineTarget);   
                                                ModuleMoveVelocity.reset();
                                              };

  void putMessage(T_MESSAGE Message) override 
  {
      ControlEngineTarget << Message;
     PositionDevice.first = ControlEngineTarget.Engine1.Position;
    PositionDevice.second = ControlEngineTarget.Engine2.Position;

     VelocityDevice.first = ControlEngineTarget.Engine1.Velocity;
    VelocityDevice.second = ControlEngineTarget.Engine2.Velocity;
  }; 
  //===============================================================================================


  void setNull(QPair<float,float> PosNull) { setMode(CONTROL_PARAM::POS); PositionNullDevice = PosNull; setToNull();  };
	void setToNull();
  void loadSettings();
  std::shared_ptr<PortAdapter<DeviceRotaryInterface>> PortMoveRelative = nullptr;
  std::shared_ptr<PortAdapter<DeviceRotaryInterface>> PortMoveVelocity = nullptr;
  std::shared_ptr<PortAdapter<DeviceRotaryInterface>> PortMovePosition = nullptr;

  std::shared_ptr<PortAdapter<DeviceRotaryInterface>> PortMoveActive = nullptr;

	DynamicModule ModuleMoveVelocity{this};
	SinusMoveModule ModuleMoveSinus{this};
private:
	RotateVectorClass<int>   RotAxis;
};

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
DeviceRotaryControl<T_CONNECTION, T_COMMAND, T_MESSAGE>::DeviceRotaryControl(std::shared_ptr<T_CONNECTION> Connection, CONTROL_PARAM ControlType, QString Name): 
DeviceGenericInterface<T_CONNECTION,T_COMMAND, T_MESSAGE>(Connection, Name)
{
  setMode(ControlType);
  qDebug() << TAG_NAME << "COMMAND TYPE: " << typeid(T_COMMAND).name();

  ControlRotaryPos = std::make_shared<DeviceRotaryControlAdapter<DEVICE_TYPE>>();
  ControlRotaryVel = std::make_shared<DeviceRotaryControlAdapter<DEVICE_TYPE>>();

  ControlRotaryPos->linkAdapter(this, &DEVICE_TYPE::moveToPos, &DEVICE_TYPE::getPos);

  if(ControlType == CONTROL_PARAM::VEL)
  ControlRotaryVel->linkAdapter(this, &DEVICE_TYPE::moveWithVelocity, &DEVICE_TYPE::getPos);
  else
  ControlRotaryVel->linkAdapter(this, &DEVICE_TYPE::moveWithVelocityManual, &DEVICE_TYPE::getPos);

  //setLimit<CONTROL_PARAM::POS  >(180); // POSITION
  //setLimit<CONTROL_PARAM::VEL  >(100); // VELOCITY
  //setLimit<CONTROL_PARAM::ACCEL>(100); // ACCELERATION

  PortMoveRelative = std::make_shared<PortAdapter<DeviceRotaryInterface>>();
  PortMovePosition = std::make_shared<PortAdapter<DeviceRotaryInterface>>();
  PortMoveVelocity = std::make_shared<PortAdapter<DeviceRotaryInterface>>();

  PortMoveRelative->linkAdapter(this, &DeviceRotaryInterface::moveToPosRelative, 
                                      &DeviceRotaryInterface::getPos);
  PortMovePosition->linkAdapter(this, &DeviceRotaryInterface::moveToPos, 
                                      &DeviceRotaryInterface::getPos);
  PortMoveVelocity->linkAdapter(this, &DeviceRotaryInterface::moveWithVelocity, 
                                      &DeviceRotaryInterface::getPos);
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::checkPositionOffset(QPair<float,float>& Position)
{
    if(abs(Position.first)  > Limits[0].first)  
           Position.first   = Limits[0].first *Position.first /abs(Position.first); 

    if(abs(Position.second) > Limits[0].second) 
           Position.second  = Limits[0].second*Position.second/abs(Position.second); 
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::checkPositionOffset()
{
    if(abs(PositionTarget.first)  > Limits[0].first)  
           PositionTarget.first   = Limits[0].first *PositionTarget.first /abs(PositionTarget.first); 

    if(abs(PositionTarget.second) > Limits[0].second) 
           PositionTarget.second  = Limits[0].second*PositionTarget.second/abs(PositionTarget.second); 
}


template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::checkVelocityOffset()
{
    if(abs(VelocityTarget.first)  > Limits[0].first)  
           VelocityTarget.first   = Limits[0].first *VelocityTarget.first /abs(VelocityTarget.first); 
    if(abs(VelocityTarget.second) > Limits[0].second) 
           VelocityTarget.second  = Limits[0].second*VelocityTarget.second/abs(VelocityTarget.second); 
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
bool DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::isAtLimit() 
{ 
  if(abs(PositionTarget.first)  > Limits[0].first || 
     abs(PositionTarget.second) > Limits[0].second  ) return true; 
                                                    return false; 
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::setToNull() 
{ 
                    PositionTarget = QPair<float,float>(0,0);
                    PositionTargetDevice = PositionTarget + PositionNullDevice; 
  this->sendCommand(PositionTargetDevice);                  PositionRelativeAnchor = PositionTarget;

}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveWithVelocity(const QPair<float, float>& Velocity)
{
                                   VelocityTarget = Velocity;
                 this->sendCommand(VelocityTarget);
  qDebug() << "MOVE WITH VELOCITY: " << VelocityTarget.first << VelocityTarget.second;
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveOnStep(const QPair<float, float>& Step)
{
                          PositionTarget = PositionTarget + Step; checkPositionOffset();
                    PositionTargetDevice = PositionTarget + PositionNullDevice; 
  this->sendCommand(PositionTargetDevice);                  PositionRelativeAnchor = PositionTarget;
  qDebug() << OutputFilter::Filter(5) << "MOVE STEP TO POS: " << PositionTargetDevice.first << PositionTargetDevice.second << "STEP: " << Step.first;
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveToPos(const QPair<float, float>& Pos)
{
                    PositionTarget = Pos; checkPositionOffset();
                    PositionTargetDevice = PositionTarget + PositionNullDevice; 
  this->sendCommand(PositionTargetDevice);                  PositionRelativeAnchor = PositionTarget;
  //qDebug() << OutputFilter::Filter(100) << TAG_NAME << "MOVE TO POS: " << PositionTargetDevice.first << PositionTargetDevice.second;
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::moveToPosRelative(const QPair<float, float>& PosRelative) 
{
                    PositionTarget = PositionRelativeAnchor + PosRelative; checkPositionOffset();
                    PositionTargetDevice = PositionTarget + PositionNullDevice;
  this->sendCommand(PositionTargetDevice);
}

//================================================================================================================

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::loadSettings()
{
  //auto RotateParamCamEng = SettingsRegister::GetString(QString("ROTATE_CAM_ENG"));
  //auto RotateParamEngCam = SettingsRegister::GetString(QString("ROTATE_ENG_CAM"));
  //this->RotEngineToCamera.LoadRotationFromFile(RotateParamEngCam);
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceRotaryControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::setMode  (CONTROL_PARAM Mode) 
{                                   ControlEngineTarget.Engine1.Mode = (int)Mode; 
                                    ControlEngineTarget.Engine2.Mode = (int)Mode; 
  DEVICE_INTERFACE::Command.setData(ControlEngineTarget); 
  if(Mode == CONTROL_PARAM::VEL) PortMoveActive = PortMoveVelocity;
  if(Mode == CONTROL_PARAM::POS) PortMoveActive = PortMovePosition; };




#endif 
