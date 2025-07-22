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
	double StepPeriod = 0;
  void Reset() { CoordIntegral = QPair<double,double>(0,0); StepPeriod = 0;}

	void SetInput(const QPair<T,T>& Coord) override
	{
	auto TimePoint = std::chrono::high_resolution_clock::now();
	    StepPeriod = std::chrono::duration<double>((TimePoint - LastTimePoint)).count(); 

	LastTimePoint = TimePoint; if(StepPeriod*1000 > 20) return;

    PassCoordClass<double>::OutputCoord = PassCoordClass<double>::OutputCoord + Coord*StepPeriod;
	}

};

template<typename T>
class IntegratorClass : public PassCoordClass<T>
{
	public:
	void SetInput(const QPair<double,double>& Coord) override 
  { PassCoordClass<T>::OutputCoord = PassCoordClass<double>::OutputCoord + Coord; }
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

	void SetInput(const QPair<double,double>& Coord) override 
  { 
    *CurrentPos = Coord; CurrentPos++; if(CurrentPos == End) { CurrentPos = Register.begin(); FlagRegisterFilled = true; }
  }
};


class SignalPortAdapter : public QObject
{
Q_OBJECT
public:
    SignalPortAdapter(){};
    SignalPortAdapter(PassCoordClass<double>* Port){ LinkToPort(Port);};
    PassCoordClass<double>* SetCoordPort = 0;
    void LinkToPort(PassCoordClass<double>* Port) { SetCoordPort = Port;};

public slots: 
void SlotSetCoord(QPair<double,double> Coord) { if(SetCoordPort != 0) Coord >> *SetCoordPort; }

};


class AimingParamOptimizator: public PassCoordClass<double>
{
  public:
  AimingParamOptimizator();
  AimingParamOptimizator(std::vector<PIDParamStruct> ParamTable);
  void operator=(const AimingParamOptimizator& Optimizator);

  int BestPIDParamNumber = 0;
  std::vector<PIDParamStruct> PIDParamTable;
  StatisticGroup<double> PIDParamGroupStat = StatisticGroup(10,200);
  StatisticNode<double> CurrentStatistic = StatisticNode<double>(200);
  PIDParamStruct BestPIDParam;

  int LimitDispersionCounter = 0;

	bool isAimingFaultStatistic();
  bool FLAG_CONTROL_PARAM_REGIM = false;
  bool FLAG_REGISTRATION_REGIM = true;

  void SetInput(const QPair<double,double>& Coord);
  const QPair<double,double>& GetOutput() { return CurrentStatistic.NodeCoord.GetDispersionCoord(); }
  void AppendPIDParam(PIDParamStruct PIDParam);
  void Reset();
};



class AimingClass : public PassCoordClass<double>
  {
  public:
      
  AimingClass();
  ~AimingClass();
  
  std::string TAG_NAME{"[ AIMING ]"};
  
  int NumberChannel = 0;
  static int ModuleCounter;
  TypeEnumBlock  TypeBlock   = TypeEnumBlock::AimingBlock;
  StateEnumBlock StateBlock  = StateEnumBlock::StateBlockDisabled;
  TypeEnumAiming AimingState = AimingDirect;
  
  std::vector<double> GainList{0.10,
                               1.0,
                               1.4,
                               70};

  double GetAbsError();
  void   Reset();
  
  RotateVectorClass Rotation;

  CoordInversionAxisNode<double> AxisInversion{0};
  StatisticValue<double> StatValue{100};

  SubstractNode<double> Substract;
  SumNode<double> Sum;

  CoordPassFilter<double> PassFilter{30};
  
  const QPair<double, double>& GetOutput();
  const QPair<double, double>& GetAimPosition();  //AIM POSITION IS SET MANUAL OR FROM CAMERA 
  const QPair<double, double>& GetBeamPosition(); //BEAM POSITION IS SET FROM CAMERA 
  const QPair<double, double>& GetAimingError(); 
  
  void SetBlockEnabled(bool OnOff); 
  
  void SetInput           (const QPair<double,double>& Coord);

  //========================================================
  void SetAimingPosition  (const QPair<double, double>& Coord);
  void SetAimingCorrection(const QPair<double, double>& Coord);
  void MoveAimingCorrection (const QPair<double, double>& Velocity);
  void SetOutputCorrection(const QPair<double, double>& Coord);

  void SetPIDParamFromTable(int Number);
  void SetPIDParam(PIDParamStruct param);
  void SetAimingRegim(TypeEnumAiming Aiming);
  void SetGain(int Number, double Gain);
  //========================================================
  
  void LoadPIDParam(QString SettingsFile);
  void LoadSettings();
  
  bool isAimingFault();
  void PrintPassCoords(QPair<double,double> Coord);

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
  
  std::shared_ptr<PortAdapter<AimingClass>> PortSetAiming = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortMoveAiming = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCalibration = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCorrection = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCorrectionOutput = nullptr;
  
  SignalPortAdapter PortSignalSetAiming;
  
  PIDParamStruct AimingSlowParam;
  PIDParamStruct AimingWorkSlowParam;
  PIDParamStruct AimingFastParam;
  
  PIDClass ModulePID;
  
  AimingParamOptimizator AimingOptimizator;
  StatisticNode<double> AimingStatistic;
  StatisticNode<double> FaultStatistic;
  
  std::vector<PIDParamStruct> PIDParamTable;
  int PIDParamNumber = 0;
  
  //DESIERED COORDS THAT GET FROM CAMERA OR SET MANUAL
  QPair<double, double> CoordNullPosition {0.0,0.0}; 
  QPair<double, double> CoordAim          {200.0,200.0}; 
  QPair<double, double> CoordSpot         {200.0,200.0}; 
  QPair<double, double> CenterPoint       {200.0,200.0}; 
  QPair<double, double> CoordAimCorrection{0.0,0.0}; 
  QPair<double, double> CoordOutputCorrection{0.0,0.0}; 
  QPair<double, double> CoordCorrectCalib{0.0,0.0}; 
  QPair<double, double> CoordBeamPos      {0.0,0.0}; 
  QPair<double, double> ZeroCoord      {0.0,0.0}; 
  
  //PROCESSING COORDS DURING AIMING
  QPair<double, double> CoordAimingError    {0.0,0.0};
  QPair<double, double> CoordAimingErrorDisp{0.0,0.0};
  QPair<double, double> VectorVelocityOutput{0.0,0.0};
  QPair<double, double> VectorAccelOutput   {0.0,0.0};
  QPair<double, double> VectorOutput        {0.0,0.0};
  //==========================================
  
  TransformCoordClass PixToRadian;
  TransformCoordClass RadianToPix;
  TransformCoordClass Gain{10};
  TransformCoordClass Saturation{1,0,5000};

  TimeIntegratorClass<double> Integrator;
  TimeIntegratorClass<double> IntegratorInput;
  TimeIntegratorClass<double> IntegratorInputSignal;
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
  void SignalCoord1(QPair<double,double> Coord);
  void SignalCoord2(QPair<double,double> Coord);
  void SignalCoord3(QPair<double,double> Coord);

};

#endif //AIMINGCLASS_H
