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


	ModulePID.setGainList(0.2,0,0,1);
	//==================================================================

  auto path = QString("/home/%1/DATA/TrackingProject/MEASURES/MeasureData.txt").arg(SettingsRegister::GetString("USER"));
  Rotation.LoadRotationFromFile(path);
                                            

  QObject::connect(this, SIGNAL(signalStateIdle())  , this, SLOT(slotStopProcessing  ()), Qt::QueuedConnection); 
  QObject::connect(this, SIGNAL(signalStateActive()), this, SLOT(slotStartProcessing()) , Qt::QueuedConnection); 
  QObject::connect(this, SIGNAL(signalReset())      , this, SLOT(slotReset())           , Qt::QueuedConnection);

  if(TypeAimingSchemeUsed == AIMING_SCHEME_1) qDebug() << TAG_NAME.c_str() << "[AIMING SCHEME 1]";
  if(TypeAimingSchemeUsed == AIMING_SCHEME_2) qDebug() << TAG_NAME.c_str() << "[AIMING SCHEME 2]";
  if(TypeAimingSchemeUsed == AIMING_SCHEME_3) qDebug() << TAG_NAME.c_str() << "[AIMING SCHEME 3]";
  
  //QObject::connect(&timerProcessAiming, SIGNAL(timeout()), this, SLOT(slotProcessLoop2()));

  if(TypeAimingSchemeUsed == AIMING_SCHEME_1) 
  QObject::connect(&timerProcessAiming, SIGNAL(timeout()), this, SLOT(slotProcessLoop1()));

  if(TypeAimingSchemeUsed == AIMING_SCHEME_2) 
  QObject::connect(&timerProcessAiming, SIGNAL(timeout()), this, SLOT(slotProcessLoop2()));

  if(TypeAimingSchemeUsed == AIMING_SCHEME_3) 
  QObject::connect(&timerProcessAiming, SIGNAL(timeout()), this, SLOT(slotProcessLoop3()));



  qDebug() << TAG_NAME.c_str() << "[PERIOD INPUT  ]" << periodProcessInput; 
  qDebug() << TAG_NAME.c_str() << "[PERIOD PROCESS]" << periodProcess; 
  qDebug() << TAG_NAME.c_str() << "[AIMING SCHEME ]" << TypeAimingSchemeUsed; 
}


bool AimingClass::isAimingFault()
{
  return StateBlock == StateBlockDisabled || StateBlock == StateBlockFault || StateBlock == StateBlockBroken;
}

float AimingClass::GetAbsError() { return std::hypot(CoordAimingError.first, CoordAimingError.second); }



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
                              << "ERROR "   << CoordAimingError.first << CoordAimingError.second 
                              << "OUTPUT"   << VectorOutput.first     << VectorOutput.second
                              << "PERIOD: " << FrameMeasureInput.getMilliseconds()
                              << "GAIN: "   << GainList[3];
                              //<< "PROLONG"  << VectorOutputProlong.first   << VectorOutputProlong.second
}

void AimingClass::BlockOutput(bool channelx, bool channely)
{
  if(channelx) VectorOutput.first = 0;
  if(channely) VectorOutput.second = 0;
};
   

void AimingClass::setInput(const QPair<float,float>& Coord) 
{ 

  if(this->StateBlock != StateBlockAtWork) return; 
  //FrameMeasureInput++;
  //qDebug() << OutputFilter::Filter(50) << TAG_NAME.c_str() << "[ INPUT PERIOD ]" << FrameMeasureInput.getMilliseconds();

  mutexInput.lock(); CoordsInput.push(Coord); mutexInput.unlock(); 
}

void AimingClass::slotProcessLoop1()
{
   //LOOP FOR FAST CONTOUR
   if (this->StateBlock != StateBlockAtWork) return; 

                                                          GainList[4] = 1; 
   if(StatisticCoord<float>::Norm(CoordAimingError) < 40) GainList[4] = 1.3;
   if(StatisticCoord<float>::Norm(CoordAimingError) < 10) GainList[4] = 1.4;
   if(StatisticCoord<float>::Norm(CoordAimingError) < 5 ) GainList[4] = 1.5;
   GainList[0] = GainList[3]*GainList[4];

   mutexInput.lock();         isInputAvailable=false;
   if(!CoordsInput.empty()) { isInputAvailable=true;  CoordInput = CoordsInput.front(); CoordsInput.pop(); }
   mutexInput.unlock();


   if(isInputAvailable)
   {
     CoordInput >> Substract;
       CoordAim >> Substract >> CoordAimingError >> Gain(GainList[0]) >> IntegratorInput(30000) 
                                                                      >> AxisInversion(2) 
                                                                      >> VectorOutput; 
     PrintpassCoords(CoordInput); 
     BlockOutput(true,true); 
     PassCoordClass::passCoord();
   }

   //if(false) { Reset(); true >> NodeSignalFault; } 
}

void AimingClass::slotProcessLoop2()
{
   //LOOP FOR SLOW CONTOUR
   qDebug() << OutputFilter::Filter(100) << "[ AIMING PROCESS ]" << "[GAINS]"  
    << ModulePID.GainList[0]  
    << ModulePID.GainList[2]  
    << ModulePID.GainList[3];

   mutexInput.lock();         isInputAvailable=false;
   if(!CoordsInput.empty()) { isInputAvailable=true;  CoordInput = CoordsInput.front(); CoordsInput.pop(); }
   mutexInput.unlock();
   

   if(isInputAvailable)
   {
   CoordInput >> Substract;
     CoordAim >> Substract >> CoordAimingError >> ModulePID >> IntegratorInput(60,20) 
                                                            >> AxisInversion(2) 
                                                            >> FlipCoord >> VectorOutput; 
    
    PrintpassCoords(CoordInput); 
    BlockOutput(false,false);
    PassCoordClass::passCoord();
   }

    CoordAimingError >> Gain(0.1) >> ModulePID >> IntegratorInput(60,20) >> AxisInversion(2) >> FlipCoord >> VectorOutput; 
    PassCoordClass::passCoord();
}


void AimingClass::slotProcessLoop3()
{
   if (this->StateBlock != StateBlockAtWork) return; 

   CoordOutputCorrection = ZeroCoord;
   decltype(CoordInput) RotatedCoord;

   CoordInput >> Substract;
     CoordAim >> Substract >> CoordAimingError >> Rotation >>  Gain(GainList[0]) >> VectorOutput;
                              CoordAimingError >> Rotation >>  Gain(GainList[1]) >> IntegratorInput >> CoordOutputCorrection;

                             VectorOutput.second *= GainList[2];
                             VectorOutput = VectorOutput + CoordOutputCorrection; 
}

void AimingClass::slotProcessLoop4() 
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
                        CoordInput >> Substract >> CoordAimingError >> 
                                                    IntegratorInput >> 
                                                        Gain(10000) >> 
                                                   AxisInversion(0) >> VectorOutput ; //BlockOutput(false,true);

}


void AimingClass::slotProcessDirect1()
{
   if (this->StateBlock != StateBlockAtWork) return; 

     CoordInput >> Substract;                               
       CoordAim >> Substract >> CoordAimingError >> Gain(100) >>AxisInversion(2) >> VectorOutput; 
    //PrintpassCoords(CoordInput); 
}






void AimingClass::setState(int state)
{
  qDebug() << TAG_NAME.c_str() << "[SET STATE]" << state;
  if(state == 0) SetStateIdle();
  if(state == 1) SetStateActive();
}

void AimingClass::SetStateIdle  () { emit signalStateIdle(); }
void AimingClass::SetReset()       { emit signalReset(); }
void AimingClass::SetStateActive() { emit signalStateActive(); }

void AimingClass::slotStartProcessing() 
{ 
  qDebug() << TAG_NAME.c_str() << "[AIMING START]";
  StateBlock = StateBlockAtWork; PassCoordClass::PassBlocked = false;  
  timerProcessAiming.start(periodProcess);
};

void AimingClass::slotStopProcessing() 
{ 
  qDebug() << TAG_NAME.c_str() << "[AIMING STOP]";
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
  PassCoordClass<float>::passCoord(); //RESET ROTARY DEVICE

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

void AimingClass::setGain(float Gain, int Number) 
{ 
  if(GainList.size() < Number) return;
     qDebug() << TAG_NAME.c_str() << "[SET GAIN]" << Gain << Number;
     GainList[Number] = Gain; ModulePID.setGainList(GainList);
}
void AimingClass::setGainList(float Gain1, float Gain2, float Gain3, float Gain4)
{
  GainList[0] = Gain1;
  GainList[1] = Gain2;
  GainList[2] = Gain3;
  GainList[3] = Gain4; ModulePID.setGainList(GainList);
}

void AimingClass::setCommand(CommandDevice<0> Command)
{
      auto ID  = Command.Command; 
      auto Param = Command.CommandParam.ParamFloat;
         
        switch(Command.Command)
        {
          case 0: setGain(Param,0); break;
          case 1: setGain(Param,1); break;
          case 2: setGain(Param,2); break;
          case 3: setGain(Param,3); break;
          case 4: setState(Param); break;
          case 5: SetReset(); break;
          default: qDebug() << TAG_NAME.c_str() << "[ SET PARAM ]" << Command.CommandParam.ParamFloat;
        }
}

//std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();
//std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint2 = std::chrono::high_resolution_clock::now();
//auto StepPeriod = std::chrono::duration<double>((TimePoint2 - TimePoint)).count();
//
//
//
//
  

