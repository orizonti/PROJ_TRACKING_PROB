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
#include <QTimer>
#include "module_period_measure.h"
#include "interface_node_signal_adapter.h"
#include "nodes_track_approximation.h"
#include <queue>
#include "SETTINGS_PATH.h"
#include PATH_SETTINGS_DEFINES

template<typename T>
class TimeIntegratorClass : public PassCoordClass<T>
{
	public:
  PassCoordClass<T>& operator()(float Value) {Saturation.setLimit(Value); return *this; };
  PassCoordClass<T>& operator()(float Value, float Value2) {Saturation.setLimit(Value, Value2); return *this; };
	QPair<T,T> CoordTemp{0,0};
  NodeCoordSaturation<float> Saturation{30000};
  

    std::chrono::time_point<std::chrono::high_resolution_clock> LastTimePoint;
	         float StepPeriod = 0;
  void Reset() { StepPeriod = 0;}

	void setInput(const QPair<T,T>& Coord) override
	{
	auto TimePoint = std::chrono::high_resolution_clock::now();
	    StepPeriod = std::chrono::duration<float>((TimePoint - LastTimePoint)).count(); 

	LastTimePoint = TimePoint; if(StepPeriod*1000 > 50) return;

    //PassCoordClass<float>::OutputCoord = PassCoordClass<float>::OutputCoord + Coord*StepPeriod;
    CoordTemp = PassCoordClass<float>::OutputCoord + Coord*StepPeriod;
    CoordTemp >> Saturation >> PassCoordClass<float>::OutputCoord;
	}

};

template<typename T>
class IntegratorClass : public PassCoordClass<T>
{
	public:
	void setInput(const QPair<float,float>& Coord) override 
  { PassCoordClass<T>::OutputCoord = PassCoordClass<float>::OutputCoord + Coord; }
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




class AimingClass : public QObject, public PassCoordClass<float>, public DeviceGenericHandleControl
  {
  Q_OBJECT
  public:
      
  AimingClass();
  ~AimingClass();
  
             std::string TAG_NAME = QString("[%1] ").arg("AIMING",15).toStdString();
  std::pair<int,int> RANGE_COORDS = SettingsRegister::GetPair("CAMERA_SIZE_ACTIVE");


         int NumberChannel = 0;
  static int ModuleCounter;
  QTimer timerProcessAiming;

  TypeEnumBlock  TypeBlock   = TypeEnumBlock::AimingBlock;
  StateEnumBlock StateBlock  = StateEnumBlock::StateBlockDisabled;
  TypeEnumAiming AimingState = AimingDirect;
  static constexpr int TypeAimingSchemeUsed = USE_AIMING_SCHEME;

  float sizeWindowApprox = 20;
  float sizeRollbackApprox = 10;
  float sizeFutureStep = sizeRollbackApprox/sizeWindowApprox;
  PolynomApproximation<3> trackApproximation1{(int)sizeWindowApprox,(int)sizeRollbackApprox};
  PolynomApproximation<3> trackApproximation2{(int)sizeWindowApprox,(int)sizeRollbackApprox};

         int periodProcessInput = 20;
         int periodProcess = 2;
         //int periodProcess = periodProcessInput/sizeRollbackApprox;

  NodeCoordPassValue<float> PickValue; NodeCoordJoinValue<float> JoinValue;
  NodeCoordSplitToTime<float> SplitToTime1; 
  NodeCoordSplitToTime<float> SplitToTime2; 

  
  std::vector<float> GainList {0.1,
                               0.0,
                               0.0,
                               1.0,
                               1.0};

  void setGain(float Gain, int Number); 
  void setGainList(float Gain1, float Gain2, float Gain3, float Gain4);
  void setCommand(CommandDevice<0> Command);
  

  MeasurePeriodNode FrameMeasureInput;
  
       RotateVectorClass<float> Rotation;
  NodeCoordAxisInversion<float> AxisInversion{0};
          StatisticValue<float> StatValue{100};

  NodeCoordDifference<float> Substract;
         NodeCoordSum<float> Sum;
        NodeCoordGain<float> Gain{10};
  NodeCoordSaturation<float> Saturation{30000};
  NodeCoordPassFilter<float> PassFilter{30};
        NodeCoordFlip<float> FlipCoord;
  
  float GetAbsError();
  const QPair<float, float>& getOutput() override;
  const QPair<float, float>& GetAimPosition();  //AIM POSITION IS SET MANUAL OR FROM CAMERA 
  const QPair<float, float>& GetBeamPosition(); //BEAM POSITION IS SET FROM CAMERA 
  const QPair<float, float>& GetAimingError(); 
  
  std::mutex mutexInput;
  bool isInputAvailable=false;

  void setInput (const QPair<float,float>& Coord) override;
  public slots:

  void slotProcessLoop1();
  void slotProcessLoop2();
  void slotProcessLoop3();
  void slotProcessLoop4();

  void slotProcessLoopForCalibration();
  void slotProcessDirect1();


  void slotStartProcessing();
  void slotStopProcessing();
  void slotReset();

  signals:
  void signalStateActive();
  void signalStateIdle();
  void signalReset();

  public:
  void SetStateActive(); 
  void SetStateIdle  (); 
  void SetReset();
  void setState(int state);

  void moveToThread(QThread* thread);
  //========================================================
  void SetAimingPosition  (const QPair<float, float>& Coord);
  void SetRangeCoords  (const QPair<float, float>& Range);

  void SetAimingCorrection(const QPair<float, float>& Coord);
  void MoveAimingCorrection (const QPair<float, float>& Velocity);
  void SetOutputCorrection(const QPair<float, float>& Coord);

  void setAimingRegim(TypeEnumAiming Aiming);
  //========================================================
  
  
  bool isAimingFault();
  void PrintpassCoords(QPair<float,float> Coord);

  void BlockOutput(bool channelx, bool channely);

  //======================================================
  //DeviceGenericHandleControl
	void setParam (uint16_t CommandID, float    CommandParam);
 	void setLevel ( uint32_t Level) {};
	void setEnable(bool OnOff, uint16_t Number = 0);

  NodeSignalAdapter NodeSignalEnable{this,0};
  NodeSignalAdapter NodeSignalFault {this,1};
  //======================================================
  
  BlockCounterClass PassCounter = BlockCounterClass(200,true);
  
  std::shared_ptr<PortAdapter<AimingClass>> PortSetAiming  = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortMoveAiming = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCalibration= nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCorrection = nullptr;
  std::shared_ptr<PortAdapter<AimingClass>> PortCorrectionOutput = nullptr;

  
  SignalPortAdapter PortSignalSetAiming;
  
  PIDClass ModulePID;
  
  StatisticNode<float> AimingStatistic;
  StatisticNode<float> FaultStatistic;
  
  
  //DESIERED COORDS THAT GET FROM CAMERA OR SET MANUAL
  QPair<float, float> CoordNullPosition {0.0,0.0}; 
  QPair<float, float> CoordAim          {0.0,0.0}; 
  QPair<float, float> CoordInput        {0.0,0.0}; 
  QPair<float, float> CoordProc         {0.0,0.0}; 

  QPair<float, float> CenterPoint       {0.0,0.0}; 

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
  QPair<float, float> VectorOutputProlong {0.0,0.0};
  //==========================================
  std::queue<QPair<float,float>> CoordsInput;  

  TimeIntegratorClass<float> Integrator;
  TimeIntegratorClass<float> IntegratorInput;
};


#endif //AIMINGCLASS_H

