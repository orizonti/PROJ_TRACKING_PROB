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

	LastTimePoint = TimePoint; if(StepPeriod*1000 > 100) return;

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

  PolynomApproximation<3> trackApproximation1{100,10};
  PolynomApproximation<3> trackApproximation2{100,10};

  NodeCoordPassValue<float> PickValue; NodeCoordJoinValue<float> JoinValue;
  NodeCoordSplitToTime<float> SplitToTime1; 
  NodeCoordSplitToTime<float> SplitToTime2; 

  
  std::vector<float> GainList{0.2,
                               1.0,
                               1.0,
                               1000};


  MeasurePeriodNode FrameMeasureInput;
  
       RotateVectorClass<float> Rotation;
  NodeCoordAxisInversion<float> AxisInversion{0};
          StatisticValue<float> StatValue{100};

  NodeCoordDifference<float> Substract;
         NodeCoordSum<float> Sum;
        NodeCoordGain<float> Gain{10};
  NodeCoordSaturation<float> Saturation{30000};
  NodeCoordPassFilter<float> PassFilter{30};
  
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

  void moveToThread(QThread* thread);
  //========================================================
  void SetAimingPosition  (const QPair<float, float>& Coord);
  void SetRangeCoords  (const QPair<float, float>& Range);

  void SetAimingCorrection(const QPair<float, float>& Coord);
  void MoveAimingCorrection (const QPair<float, float>& Velocity);
  void SetOutputCorrection(const QPair<float, float>& Coord);

  void setPIDParam(PIDParamStruct param);
  void setAimingRegim(TypeEnumAiming Aiming);
  void SetGain(int Number, float Gain);
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
  
  PIDParamStruct AimingSlowParam;
  PIDParamStruct AimingWorkSlowParam;
  PIDParamStruct AimingFastParam;
  std::vector<PIDParamStruct> PIDParamTable;
  
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

