#include "aiming_class.h"
#include "debug_output_filter.h"
#include "interface_pass_coord.h"
#include "rotate_vector_class.h"
#include "state_block_enum.h"
#include <QTimer>
#include <QFile>
#include "engine_statistics.h"
#include <QThread>

OutputFilter FilterOutput50{20};
OutputFilter FilterOutput2{100};
OutputFilter FilterOutput3{2};
OutputFilter FilterOutput4{200};

int AimingClass::ModuleCounter = 0;

AimingClass::~AimingClass() 
{ 
  qDebug() << TAG_NAME.c_str() << "DELETE";
}


const QPair<float,float>& AimingClass::getOutput() { return this->VectorOutput; }

AimingClass::AimingClass()
{
  ModuleCounter++; NumberChannel = ModuleCounter;

  TAG_NAME = QString("[%1_%2]").arg("AIMING",14).arg(NumberChannel).toStdString();
	//======================================================================
  PortSetAiming   = std::make_shared<PortAdapter<AimingClass>>();
  PortMoveAiming  = std::make_shared<PortAdapter<AimingClass>>();
  PortCalibration = std::make_shared<PortAdapter<AimingClass>>();
  PortCorrection  = std::make_shared<PortAdapter<AimingClass>>();
  PortCorrectionOutput = std::make_shared<PortAdapter<AimingClass>>();

  PortSignalSetAiming.linkToPort(PortSetAiming.get());

         PortSetAiming->linkAdapter(this,&AimingClass::SetAimingPosition,   &AimingClass::GetAimPosition);
        PortMoveAiming->linkAdapter(this,&AimingClass::MoveAimingCorrection,&AimingClass::GetAimPosition);
       PortCalibration->linkAdapter(this,&AimingClass::SetAimingCorrection, &AimingClass::GetBeamPosition);
        PortCorrection->linkAdapter(this,&AimingClass::SetAimingCorrection, &AimingClass::GetAimingError);
  PortCorrectionOutput->linkAdapter(this,&AimingClass::SetOutputCorrection, &AimingClass::GetAimingError);

	this->AimingSlowParam.Common = 1;
	this->AimingSlowParam.RateParam = 5.1;
	this->AimingSlowParam.IntParam  = 0.0002;
	this->AimingSlowParam.DiffParam = 0.0002;

	this->AimingFastParam.Common = 1;           // for move by velocity one integrator plant
	this->AimingFastParam.RateParam = 16.45;
	this->AimingFastParam.IntParam  = 46.60;
	this->AimingFastParam.DiffParam = 0.0164;

	ModulePID.setPIDParam(AimingSlowParam);
	//==================================================================

  auto path = QString("/home/%1/DATA/TrackingProject/MEASURES/MeasureData.txt").arg(SettingsRegister::GetString("USER"));
  Rotation.LoadRotationFromFile(path);
                                            

  QObject::connect(this, SIGNAL(signalStateIdle())  , this, SLOT(slotStopProcessing  ()), Qt::QueuedConnection); 
  QObject::connect(this, SIGNAL(signalStateActive()), this, SLOT(slotStartProcessing()) , Qt::QueuedConnection); 
  QObject::connect(this, SIGNAL(signalReset())      , this, SLOT(slotReset())      , Qt::QueuedConnection);

  QObject::connect(&timerProcessAiming, SIGNAL(timeout()), this, SLOT(slotProcessLoop1()));
}


bool AimingClass::isAimingFault()
{
  return StateBlock == StateBlockDisabled || StateBlock == StateBlockFault || StateBlock == StateBlockBroken;
}

float AimingClass::GetAbsError() { return std::hypot(CoordAimingError.first, CoordAimingError.second); }

void AimingClass::SetGain(int Number, float Gain)
{
  GainList[Number] = Gain;
}


void AimingClass::setPIDParam(PIDParamStruct param)
{
auto CurrentParam = param; ModulePID.setPIDParam(CurrentParam);
}

void AimingClass::setAimingRegim(TypeEnumAiming Aiming)
{
  AimingState = Aiming;
}

const QPair<float,float>& AimingClass::GetAimPosition()  { return CoordAim; }
const QPair<float,float>& AimingClass::GetBeamPosition() { return CoordAimingError; }
const QPair<float,float>& AimingClass::GetAimingError()  
{ 
  CoordInput >> Substract;
   CoordAim >> Substract >> CoordAimingError;
                     return CoordAimingError; 
}

void AimingClass::SetRangeCoords(const QPair<float, float>& Range)
{
  RANGE_COORDS = Range;
}

void AimingClass::SetAimingPosition(const QPair<float,float>& Coord)       
{ 
  CoordAim = QPair<float,float>(RANGE_COORDS.first*Coord.first, RANGE_COORDS.second*Coord.second); 
  qDebug() << TAG_NAME.c_str() << "[ COORD AIM ]" << CoordAim.first << CoordAim.second;
}

void AimingClass::SetAimingCorrection(const QPair<float,float>& Coord)     { CoordAimCorrection    = Coord; }
void AimingClass::SetOutputCorrection(const QPair<float,float>& Coord)     { CoordOutputCorrection = Coord; };

void AimingClass::MoveAimingCorrection(const QPair<float,float>& Velocity) { Velocity >> Integrator >> CoordAimCorrection; }


void AimingClass::PrintpassCoords(QPair<float,float> Coord)
{
//    qDebug() << OutputFilter::Filter(100) << TAG_NAME.c_str() << "INPUT" << Coord.first << Coord.second 
//                              << "AIM" << CoordAim.first << CoordAim.second 
//                              << "ERROR" << CoordAimingError.first << CoordAimingError.second
//                              << "OUTPUT" << VectorOutput.first << VectorOutput.second
//                              << "TYPE: " << (int)TypeEnumAiming::AimingLoop;

    FrameMeasureInput++;
    qDebug() << OutputFilter::Filter(200) << TAG_NAME.c_str() 
                              << "OUTPUT" << VectorOutput.first          << VectorOutput.second
                              << "INPUT " << CoordInput.first << CoordInput.second 
                              << "ERROR " << CoordAimingError.first << CoordAimingError.second 
                              << "GAIN: " << GainList[3]
                              << "PERIOD: " << FrameMeasureInput.getMilliseconds();
}

void AimingClass::BlockOutput(bool channelx, bool channely)
{
  if(channelx) VectorOutput.first = 0;
  if(channely) VectorOutput.second = 0;
};

    //if(Axis == 0) { AxisXDirection = -1; AxisYDirection =  1;}
    //if(Axis == 1) { AxisXDirection = 1;  AxisYDirection = -1;}
    //if(Axis == 2) { AxisXDirection = -1; AxisYDirection = -1;}
    //
    //
   

void AimingClass::setInput(const QPair<float,float>& Coord) 
{ 
  //qDebug() << TAG_NAME.c_str() << "INPUT" << Coord.first << Coord.second;

  CoordsInput.push(Coord); IS_INPUT_AVAILABLE = true;
}

void AimingClass::slotProcessLoop1()
{
  //MUST WORK ALWAYS, SIMPLE INTEGRATOR
   if (this->StateBlock != StateBlockAtWork) return; 
                                                         GainList[3] = 800; 
   if(StatisticCoord<float>::Norm(CoordAimingError) < 5) GainList[3] = 1000;
   if(StatisticCoord<float>::Norm(CoordAimingError) < 3) GainList[3] = 2500;

   GainList[3] = 0.01;


   if(IS_INPUT_AVAILABLE)
   {
   CoordInput = CoordsInput.front(); CoordsInput.pop(); if(CoordsInput.empty()) IS_INPUT_AVAILABLE = false; 

   CoordInput >> Substract;
     CoordAim >> Substract >> CoordAimingError >> Gain(GainList[3]) >> IntegratorInput 
                                                                    >> AxisInversion(2) 
                                                                    >> Saturation(30000) 
                                                                    >> VectorOutput; 
    VectorOutput >> SplitToTime1(0) >> trackApproximation1; 
                    SplitToTime1(1) >> trackApproximation2; 
   }
   //     trackApproximation1.StepsForecasting = 1;
   //     trackApproximation2.StepsForecasting = 1;

   //     trackApproximation1 >> PickValue(1) >> JoinValue;
   //     trackApproximation2 >> PickValue(1) >> JoinValue >> VectorOutputProlong;
   //  if(trackApproximation1.isLoaded()) VectorOutput = VectorOutputProlong;


   PrintpassCoords(CoordInput); 
   BlockOutput(false,false);
   PassCoordClass::passCoord();

   //if(false) { Reset(); true >> NodeSignalFault; } 
}

void AimingClass::slotProcessLoop2()
{
   if (this->StateBlock != StateBlockAtWork) return; 

   CoordOutputCorrection = ZeroCoord;
   decltype(CoordInput) RotatedCoord;

   CoordInput >> Substract;
     CoordAim >> Substract >> CoordAimingError >> Rotation >>  Gain(GainList[0]) >> VectorOutput;
                              CoordAimingError >> Rotation >> IntegratorInput >> Gain(GainList[1]) >> CoordOutputCorrection;

                             VectorOutput.second *= GainList[2];
                             VectorOutput = VectorOutput + CoordOutputCorrection; 
}
void AimingClass::slotProcessLoop3() 
{
   if (this->StateBlock != StateBlockAtWork) return; 

   CoordInput >> Substract;
     CoordAim >> Substract >> CoordAimingError >> PassFilter >> ModulePID >> IntegratorInput >> Saturation >> VectorOutput;
}


void AimingClass::slotProcessLoopForCalibration()
{
   if (this->StateBlock != StateBlockAtWork) return; 

             CoordAim >> Substract;
   CoordAimCorrection >> Substract >> Substract;
                         CoordInput >> Substract >> CoordAimingError >> IntegratorInput >> Gain(10000) >> AxisInversion(0) >> VectorOutput ; //BlockOutput(false,true);

}


void AimingClass::slotProcessDirect1()
{
   if (this->StateBlock != StateBlockAtWork) return; 

     CoordInput >> Substract;                               
       CoordAim >> Substract >> CoordAimingError >> Gain(100) >>AxisInversion(2) >> VectorOutput; 
    //PrintpassCoords(CoordInput); 
}




void AimingClass::SetStateActive() { emit signalStateActive(); }
void AimingClass::SetStateIdle  () { emit signalStateIdle(); }
void AimingClass::SetReset()       { emit signalReset(); }


void AimingClass::slotStartProcessing() 
{ 
  StateBlock = StateBlockAtWork; PassCoordClass::PassBlocked = false;  
  timerProcessAiming.start(2);
};

void AimingClass::slotStopProcessing() 
{ 
  StateBlock = StateBlockDisabled; PassCoordClass::PassBlocked = true;  
  timerProcessAiming.stop();
};

void AimingClass::slotReset()
{
  qDebug() << TAG_NAME.c_str() << "[ AIMING RESET ]";

  VectorOutput = QPair<float,float>(0,0);

  AimingStatistic.reset();

        ModulePID.Reset();
       Integrator.Reset();
  IntegratorInput.Reset();

}


void AimingClass::setParam(uint16_t CommandID, float    CommandParam)
{
        if(CommandID > GainList.size()) return;   
  GainList[CommandID] = CommandParam;
}

void AimingClass::setEnable(bool OnOff, uint16_t Number)
{
  switch(Number)
  {
    case 0: if(OnOff) SetStateActive(); else SetStateIdle(); break;
    case 1:           SetReset(); break;
  }
}

void AimingClass::moveToThread(QThread* thread)
{
  qDebug() << TAG_NAME.c_str() << "[ MOVE TO THREAD ]" << thread;

           QObject::moveToThread(thread);
 timerProcessAiming.moveToThread(thread);
    NodeSignalFault.moveToThread(thread);
   NodeSignalEnable.moveToThread(thread);
}


//std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();
//std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint2 = std::chrono::high_resolution_clock::now();
//auto StepPeriod = std::chrono::duration<double>((TimePoint2 - TimePoint)).count();
//
//
//
//
  

