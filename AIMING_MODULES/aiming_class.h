#ifndef AIMINGCLASS_H
#define AIMINGCLASS_H
#include "interface_pass_coord.h"

#include "pid_param_struct.h"
#include "rotate_vector_class.h"
#include "type_block_enum.h"
#include "state_block_enum.h"
#include "pid_class.h"
#include <QSettings>
#include "register_settings.h"
#include "engine_statistics.h"
#include "thread_operation_nodes.h"
#include "transform_coord_class.h"
#include <QTimer>

template<typename T>
class TimeIntegratorClass : public PassCoordClass<T>
{
	public:
	QPair<T,T> CoordIntegral;

    std::chrono::time_point<std::chrono::high_resolution_clock> LastTimePoint;
	float StepPeriod = 0;
  void Reset() { CoordIntegral = QPair<float,float>(0,0); StepPeriod = 0;}

	void setInput(const QPair<T,T>& Coord) override
	{
	auto TimePoint = std::chrono::high_resolution_clock::now();
	    StepPeriod = std::chrono::duration<float>((TimePoint - LastTimePoint)).count(); 

	LastTimePoint = TimePoint; if(StepPeriod*1000 > 20) return;

    PassCoordClass<float>::OutputCoord = PassCoordClass<float>::OutputCoord + Coord*StepPeriod;
	}

};

template<typename T>
class IntegratorClass : public PassCoordClass<T>
{
	public:
	void setInput(const QPair<float,float>& Coord) override 
  { PassCoordClass<T>::OutputCoord = PassCoordClass<float>::OutputCoord + Coord; }
};

template<typename T>
class SignalRegister : public PassCoordClass<T>
{
	public:
  SignalRegister(int Size)
  {
    Register.resize(Size);
    CurrentPos = Register.begin();
    End = Register.end();
  }
  std::vector<QPair<T,T>> Register;
  std::vector<QPair<T,T>>::iterator CurrentPos;
  std::vector<QPair<T,T>>::iterator End;
  bool FlagRegisterFilled = false;

	void setInput(const QPair<float,float>& Coord) override 
  { 
    *CurrentPos = Coord; CurrentPos++; if(CurrentPos == End) { CurrentPos = Register.begin(); FlagRegisterFilled = true; }
  }
};


class SignalPortAdapter : public QObject
{
Q_OBJECT
public:
    SignalPortAdapter(){};
    SignalPortAdapter(PassCoordClass<float>* Port){ linkToPort(Port);};
    PassCoordClass<float>* SetCoordPort = 0;
    void linkToPort(PassCoordClass<float>* Port) { SetCoordPort = Port;};

public slots: 
void slotSetCoord(QPair<float,float> Coord) 
{ 
  if(SetCoordPort != 0) Coord >> *SetCoordPort; 
}

};


class AimingParamOptimizator: public PassCoordClass<float>
{
  public:
  AimingParamOptimizator();
  AimingParamOptimizator(std::vector<PIDParamStruct> ParamTable);
  void operator=(const AimingParamOptimizator& Optimizator);

  int BestPIDParamNumber = 0;
  std::vector<PIDParamStruct> PIDParamTable;
  StatisticGroup<float> PIDParamGroupStat = StatisticGroup(10,200);
  StatisticNode<float> CurrentStatistic = StatisticNode<float>(200);
  PIDParamStruct BestPIDParam;

  int LimitDispersionCounter = 0;

	bool isAimingFaultStatistic();
  bool FLAG_CONTROL_PARAM_REGIM = false;
  bool FLAG_REGISTRATION_REGIM = true;

  void setInput(const QPair<float,float>& Coord);
  const QPair<float,float>& getOutput() { return CurrentStatistic.NodeCoord.GetDispersionCoord(); }
  void AppendPIDParam(PIDParamStruct PIDParam);
  void Reset();
};



class AimingClass : public PassCoordClass<float>
  {
  public:
      
  AimingClass();
  ~AimingClass();
  
  std::string TAG_NAME = QString("[ %1 ] ").arg("AIMING").toStdString();
  
  int NumberChannel = 0;
  static int ModuleCounter;
  TypeEnumBlock  TypeBlock   = TypeEnumBlock::AimingBlock;
  StateEnumBlock StateBlock  = StateEnumBlock::StateBlockDisabled;
  TypeEnumAiming AimingState = AimingDirect;
  
  std::vector<float> GainList{0.2,
                               1.0,
                               1.0,
                               80000};

  float GetAbsError();
  void   Reset();
  
  RotateVectorClass<float> Rotation;

  CoordInversionAxisNode<float> AxisInversion{0};
  StatisticValue<float> StatValue{100};

  SubstractNode<float> Substract;
  SumNode<float> Sum;

  CoordPassFilter<float> PassFilter{30};
  
  const QPair<float, float>& getOutput();
  const QPair<float, float>& GetAimPosition();  //AIM POSITION IS SET MANUAL OR FROM CAMERA 
  const QPair<float, float>& GetBeamPosition(); //BEAM POSITION IS SET FROM CAMERA 
  const QPair<float, float>& GetAimingError(); 
  
  void SetModuleEnabled(bool OnOff); 
  
  void setInput           (const QPair<float,float>& Coord);

  //========================================================
  void SetAimingPosition  (const QPair<float, float>& Coord);
  void SetAimingCorrection(const QPair<float, float>& Coord);
  void MoveAimingCorrection (const QPair<float, float>& Velocity);
  void SetOutputCorrection(const QPair<float, float>& Coord);

  void SetPIDParamFromTable(int Number);
  void SetPIDParam(PIDParamStruct param);
  void SetAimingRegim(TypeEnumAiming Aiming);
  void SetGain(int Number, float Gain);
  //========================================================
  
  void LoadPIDParam(QString SettingsFile);
  void LoadSettings();
  
  bool isAimingFault();
  void PrintpassCoords(QPair<float,float> Coord);

  void ProcessLoop1();
  void ProcessLoop2();
  void ProcessLoop3();
  void ProcessLoop4();
  void ProcessLoopForCalibration();

  void ProcessDirect1();
  void ProcessDirect2();
  void ProcessDirect3();
  void ProcessDirect4();
  void BlockOutput(bool channelx, bool channely);
  
  
  BlockCounterClass PassCounter = BlockCounterClass(200,true);
  
  std::shared_ptr<PortAdapter<AimingClass>> PortSetAiming  = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortMoveAiming = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCalibration= nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCorrection = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCorrectionOutput = nullptr;
  
  SignalPortAdapter PortSignalSetAiming;
  
  PIDParamStruct AimingSlowParam;
  PIDParamStruct AimingWorkSlowParam;
  PIDParamStruct AimingFastParam;
  
  PIDClass ModulePID;
  
  AimingParamOptimizator AimingOptimizator;
  StatisticNode<float> AimingStatistic;
  StatisticNode<float> FaultStatistic;
  
  std::vector<PIDParamStruct> PIDParamTable;
  int PIDParamNumber = 0;
  
  //DESIERED COORDS THAT GET FROM CAMERA OR SET MANUAL
  QPair<float, float> CoordNullPosition {0.0,0.0}; 
  QPair<float, float> CoordAim          {60.0,60.0}; 
  QPair<float, float> CoordSpot         {60.0,60.0}; 
  QPair<float, float> CenterPoint       {60.0,60.0}; 
  QPair<float, float> CoordAimCorrection{0.0,0.0}; 
  QPair<float, float> CoordOutputCorrection{0.0,0.0}; 
  QPair<float, float> CoordCorrectCalib{0.0,0.0}; 
  QPair<float, float> CoordBeamPos      {0.0,0.0}; 
  QPair<float, float> ZeroCoord      {0.0,0.0}; 
  
  //PROCESSING COORDS DURING AIMING
  QPair<float, float> CoordAimingError    {0.0,0.0};
  QPair<float, float> CoordAimingErrorDisp{0.0,0.0};
  QPair<float, float> VectorVelocityOutput{0.0,0.0};
  QPair<float, float> VectorAccelOutput   {0.0,0.0};
  QPair<float, float> VectorOutput        {0.0,0.0};
  //==========================================
  
  TransformCoordClass PixToRadian;
  TransformCoordClass RadianToPix;
  TransformCoordClass Gain{10};
  TransformCoordClass Saturation{1,0,30000};

  TimeIntegratorClass<float> Integrator;
  TimeIntegratorClass<float> IntegratorInput;
  TimeIntegratorClass<float> IntegratorInputSignal;
};

class AimingWatcherClass: public QObject
{
  Q_OBJECT
  public:
  AimingWatcherClass(AimingClass* AimingModule);
  AimingClass* ModuleWathing = 0;
  QTimer timerWatchModule;
  void StartWatching(bool OnOff)
  {
    if( OnOff) timerWatchModule.start(20);
    if(!OnOff) timerWatchModule.stop();
  }
  public slots:
  void SlotWatchModule()
  {
    emit SignalCoord1(ModuleWathing->GetAimingError());
  }
  signals:
  void SignalCoord1(QPair<float,float> Coord);
  void SignalCoord2(QPair<float,float> Coord);
  void SignalCoord3(QPair<float,float> Coord);

};

#endif //AIMINGCLASS_H
