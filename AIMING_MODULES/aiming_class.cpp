#include "aiming_class.h"
#include "debug_output_filter.h"
#include "interface_pass_coord.h"
#include "rotate_vector_class.h"
#include "state_block_enum.h"
#include <QTimer>
#include <QFile>
OutputFilter FilterOutput50{20};
OutputFilter FilterOutput2{100};
OutputFilter FilterOutput3{2};
OutputFilter FilterOutput4{200};

#define TAG "[   AIMING   ]" 

int AimingClass::ModuleCounter = 0;

AimingClass::~AimingClass() 
{ 
  qDebug() << TAG << "DELETE";
}


const QPair<double, double>& AimingClass::GetOutput() { return this->VectorOutput; }

AimingClass::AimingClass()
{
  ModuleCounter++; NumberChannel = ModuleCounter;
	//======================================================================
  PortSetAiming = std::make_shared<PortAdapter<AimingClass>>();
  PortMoveAiming = std::make_shared<PortAdapter<AimingClass>>();
  PortCalibration = std::make_shared<PortAdapter<AimingClass>>();
  PortCorrection = std::make_shared<PortAdapter<AimingClass>>();
  PortCorrectionOutput = std::make_shared<PortAdapter<AimingClass>>();

  PortSignalSetAiming.LinkToPort(PortSetAiming.get());

  PortSetAiming->LinkAdapter(this,&AimingClass::SetAimingPosition,&AimingClass::GetAimPosition);
  PortMoveAiming->LinkAdapter(this,&AimingClass::MoveAimingCorrection,&AimingClass::GetAimPosition);
  PortCalibration->LinkAdapter(this,&AimingClass::SetAimingCorrection,&AimingClass::GetBeamPosition);
  PortCorrection->LinkAdapter(this,&AimingClass::SetAimingCorrection,&AimingClass::GetAimingError);
  PortCorrectionOutput->LinkAdapter(this,&AimingClass::SetOutputCorrection,&AimingClass::GetAimingError);

	this->AimingSlowParam.Common = 1;
	this->AimingSlowParam.RateParam = 500;
	this->AimingSlowParam.IntParam  = 2.8;
	this->AimingSlowParam.DiffParam = 1.8;

	this->AimingWorkSlowParam.Common = 1;
	this->AimingWorkSlowParam.RateParam = 5.1;
	this->AimingWorkSlowParam.IntParam  = 0.0002;
	this->AimingWorkSlowParam.DiffParam = 0.0002;

	this->AimingFastParam.Common = 1;           // for move by velocity one integrator plant
	this->AimingFastParam.RateParam = 16.45;
	this->AimingFastParam.IntParam  = 46.60;
	this->AimingFastParam.DiffParam = 0.0164;
	ModulePID.SetPIDParam(AimingSlowParam);
	//==================================================================

  Rotation.LoadRotationFromFile("/home/broms/DATA/TrackingProject/MEASURES/MeasureData.txt");


                                            

	this->PixToRadian.TransformCoord = [this](QPair<double,double> CoordError)
							{
								PixToRadian.Output.first = (CoordError.first * PixToRadian.Scale) * M_PI / (60.0 * 60.0 * 180.0);
								PixToRadian.Output.second = (CoordError.second * PixToRadian.Scale) * M_PI / (60.0 * 60.0 * 180.0);
							};

	this->RadianToPix.TransformCoord = [this](QPair<double,double> CoordError)
							{
								RadianToPix.Output.first = (CoordError.first*(60.0*60.0*180.0)/M_PI)/PixToRadian.Scale;
								RadianToPix.Output.second = (CoordError.second*(60.0*60.0*180.0)/M_PI)/PixToRadian.Scale;
							};

	//Filter.enableFiltering(false);
  SetBlockEnabled(false);
}


bool AimingClass::isAimingFault()
{
  return StateBlock == StateBlockDisabled || StateBlock == StateBlockFault || StateBlock == StateBlockBroken;
}

double AimingClass::GetAbsError() { return std::hypot(CoordAimingError.first, CoordAimingError.second); }

void AimingClass::SetGain(int Number, double Gain)
{
  GainList[Number] = Gain;
}


void AimingClass::SetPIDParamFromTable(int Number)
{
  if(Number > PIDParamTable.size() - 1) return;
  if(PIDParamNumber == Number) return;
  SetPIDParam(PIDParamTable[Number]); PIDParamNumber = Number;
}
void AimingClass::SetPIDParam(PIDParamStruct param)
{
auto CurrentParam = param;

ModulePID.SetPIDParam(CurrentParam);

}

void AimingClass::SetAimingRegim(TypeEnumAiming Aiming)
{
  AimingState = Aiming;
}

const QPair<double, double>& AimingClass::GetAimPosition()  { return CoordAim; }
const QPair<double, double>& AimingClass::GetBeamPosition() { return CoordAimingError; }
const QPair<double, double>& AimingClass::GetAimingError()  
{ 
  CoordSpot >> Substract;
   CoordAim >> Substract >> CoordAimingError;
                     return CoordAimingError; 
}

void AimingClass::SetAimingPosition(const QPair<double, double>& Coord)       { CoordAim = Coord; qDebug() << "SET AIM POS: " << Coord;}
void AimingClass::SetAimingCorrection(const QPair<double, double>& Coord)     { CoordAimCorrection = Coord; qDebug() << "SET CORR: " << Coord;}
void AimingClass::SetOutputCorrection(const QPair<double, double>& Coord)     { CoordOutputCorrection = Coord; };

void AimingClass::MoveAimingCorrection(const QPair<double, double>& Velocity) { Velocity >> Integrator >> CoordAimCorrection; }


void AimingClass::PrintPassCoords(QPair<double,double> Coord)
{
    qDebug() << FilterOutput2 << "INPUT" << Coord.first << Coord.second 
                              << "AIM" << CoordAim.first << CoordAim.second 
                              << "ERROR" << CoordAimingError.first << CoordAimingError.second
                              << "OUTPUT" << VectorOutput.first << VectorOutput.second
                              << "TYPE: " << (int)TypeEnumAiming::AimingLoop;
}

void AimingClass::BlockOutput(bool channelx, bool channely)
{
  if(channelx) VectorOutput.first = 0;
  if(channely) VectorOutput.second = 0;
};

void AimingClass::ProcessLoop1()
{
  //MUST WORK ALWAYS, SIMPLE INTEGRATOR
   CoordSpot >> Substract;
    CoordAim >> Substract >> CoordAimingError >> Rotation >> IntegratorInput >> Gain(GainList[3]) >> VectorOutput; //BlockOutput(true,false);
}

void AimingClass::ProcessLoop2()
{
   CoordOutputCorrection = ZeroCoord;
   decltype(CoordSpot) RotatedCoord;

   CoordSpot >> Substract;
    CoordAim >> Substract >> CoordAimingError >> Rotation >>  Gain(GainList[0]) >> VectorOutput;
                             CoordAimingError >> Rotation >> IntegratorInput >> Gain(GainList[1]) >> CoordOutputCorrection;

                             VectorOutput.second *= GainList[2];
                             VectorOutput = VectorOutput + CoordOutputCorrection; 

}
void AimingClass::ProcessLoop3() 
{
   CoordSpot >> Substract;
    CoordAim >> Substract >> CoordAimingError >> PassFilter >> ModulePID >> IntegratorInput >> Saturation >> VectorOutput;
}

void AimingClass::ProcessLoop4() 
{
}

void AimingClass::ProcessLoopForCalibration()
{
             CoordAim >> Substract;
   CoordAimCorrection >> Substract >> Substract;
                         CoordSpot >> Substract >> CoordAimingError >> IntegratorInput >> Gain(10000) >> AxisInversion(0) >> VectorOutput ; //BlockOutput(false,true);

}


void AimingClass::ProcessDirect1()
{
                CoordAim >> Substract;                               
      CoordAimCorrection >> Substract >> Rotation >> AxisInversion(2) >> VectorOutput; 
}
void AimingClass::ProcessDirect2()
{
}
void AimingClass::ProcessDirect3()
{

}
void AimingClass::ProcessDirect4()
{

}



void AimingClass::SetInput(const QPair<double,double>& Coord)
{

  CoordSpot = Coord;
  if (this->StateBlock != StateBlockAtWork) return; 

  if(AimingState == TypeEnumAiming::AimingLoop)
  {

      ProcessLoop1();
      //ProcessLoop2();
      //ProcessLoop3();
      //ProcessLoop4();
      //ProcessLoopForCalibration();

  }
 
  if(AimingState == TypeEnumAiming::AimingDirect)
  {
      ProcessDirect1();
      //ProcessDirect2();
      //ProcessDirect3();
      //ProcessDirect4();
  }

  //PrintPassCoords(Coord);
  //PassCoordClass<double>::PassCoord();

  //if(AimingState == AimingTuning) CoordAimingError >> AimingOptimizator; SetPIDParamFromTable(AimingOptimizator.BestPIDParamNumber); 
   
}

void AimingClass::Reset()
{

  qDebug() << "[ AIMING RESET ]";
  this->ModulePID.ResetPID();
  this->VectorOutput = QPair<double,double>(0,0);
  AimingStatistic.Reset();
  FaultStatistic.Reset();

  Integrator.Reset();
  IntegratorInput.Reset();
  IntegratorInputSignal.Reset();

}

void AimingClass::SetBlockEnabled(bool OnOff) 
{ 
                                              PassCoordClass::PassBlocked = true;
  if( OnOff) { StateBlock = StateBlockAtWork; PassCoordClass::PassBlocked = false; } 
  if(!OnOff)   StateBlock = StateBlockDisabled; 
  qDebug() << TAG_NAME << "BLOCK ENABLED: " << OnOff << "Number: " << NumberChannel;
};




void AimingClass::LoadPIDParam(QString SettingsFile)
{
    QFile file(SettingsFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;


    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line.contains("==")) continue;

        QStringList list_preference = line.split(" ");

        if(list_preference.size() < 3) continue;

        double Common = 1;
        double rate_param = list_preference.at(0).toDouble();
        double int_param = list_preference.at(1).toDouble();
        double diff_param = list_preference.at(2).toDouble();

        this->PIDParamTable.push_back(PIDParamStruct(rate_param,diff_param,int_param));

    }

    AimingOptimizator = AimingParamOptimizator(PIDParamTable);

    file.close();
}

void AimingParamOptimizator::operator=(const AimingParamOptimizator& Optimizator)
{
  BestPIDParamNumber = Optimizator.BestPIDParamNumber;
  PIDParamTable = Optimizator.PIDParamTable;
  PIDParamGroupStat = Optimizator.PIDParamGroupStat;
  CurrentStatistic = Optimizator.CurrentStatistic;
  BestPIDParam = Optimizator.BestPIDParam;

  int LimitDispersionCounter = Optimizator.LimitDispersionCounter;
  bool FLAG_CONTROL_PARAM_REGIM = Optimizator.FLAG_CONTROL_PARAM_REGIM;
  bool FLAG_REGISTRATION_REGIM = Optimizator.FLAG_REGISTRATION_REGIM;
}

void AimingClass::LoadSettings()
{
  auto PIDParamFile = SettingsRegister::GetString("PID_PARAM");
  auto BeamPosFile = SettingsRegister::GetString("AIMING_BEAM_POS");

  QSettings BeamPosSettings(BeamPosFile, QSettings::IniFormat);
  BeamPosSettings.beginGroup("BEAMS");
  int BEAM_X_POS = BeamPosSettings.value(QString("X%1").arg(NumberChannel)).toInt();
  int BEAM_Y_POS = BeamPosSettings.value(QString("Y%1").arg(NumberChannel)).toInt();
  int XPointerPos = BeamPosSettings.value("XPointer").toInt();
  int YPointerPos = BeamPosSettings.value("XPointer").toInt();

  BeamPosSettings.endGroup();

  auto PointerCenteredCoord = QPair<double,double>(XPointerPos,YPointerPos);
  
  this->CoordNullPosition = QPair<int, int>(BEAM_X_POS,BEAM_Y_POS);
  this->CoordAim = this->CoordNullPosition;

  this->LoadPIDParam(PIDParamFile);
}

bool AimingParamOptimizator::isAimingFaultStatistic()
{
   auto Error =  CurrentStatistic.NodeCoord.GetAvarageDeviation(); 
   if(Error > 3) { LimitDispersionCounter++; CurrentStatistic.Reset();} 

   if(LimitDispersionCounter >= 10) { LimitDispersionCounter = 0; return true; }

   return false;
}

void AimingParamOptimizator::SetInput(const QPair<double,double>& Coord)
{

  Coord >> CurrentStatistic;
  if(FLAG_CONTROL_PARAM_REGIM && isAimingFaultStatistic()) 
  {
      BestPIDParamNumber--; if(BestPIDParamNumber < 0) BestPIDParamNumber = 0;
      BestPIDParam = PIDParamTable[BestPIDParamNumber];
  };

  if(!FLAG_REGISTRATION_REGIM) return;

                  Coord >> PIDParamGroupStat; 
   BestPIDParamNumber = PIDParamGroupStat.BestStatNumber;
            BestPIDParam = PIDParamTable[BestPIDParamNumber];

  if(PIDParamGroupStat.IsStatisticsLoaded()) 
  {
                           PIDParamGroupStat.PerformAvailableData();
   FLAG_REGISTRATION_REGIM = false;
   FLAG_CONTROL_PARAM_REGIM = true;
   qDebug() << "FOUND BEST PID PARAM NUMBER: " << BestPIDParamNumber << BestPIDParam.RateParam;
  }
}

void AimingParamOptimizator::AppendPIDParam(PIDParamStruct PIDParam)
{
   PIDParamTable.push_back(PIDParam);
}


AimingParamOptimizator::AimingParamOptimizator(std::vector<PIDParamStruct> ParamTable)
{
 //  this->PIDParamTable = ParamTable;
 //  if(PIDParamTable.size() != 0) BestPIDParam = PIDParamTable[BestPIDParamNumber];
 //  PIDParamGroupStat = StatisticGroup(PIDParamTable.size(),1500);

 //   double dispersion_threshold = 1.6;
 //   PIDParamGroupStat.FindBestStatisticCoord = [dispersion_threshold](std::map<int,Statistic>::iterator BeginStatistic,
 //                                                                     std::map<int,Statistic>::iterator endStatistic) -> int
 //   {
 //       int min_pos = 1;
 //       auto current_stat = BeginStatistic;
 //       std::vector<float> dispersions;

 //       while(current_stat != endStatistic) { dispersions.push_back(current_stat->second.AmplitudeCoordDeviation); current_stat++;};
 //       std::reverse(dispersions.begin(), dispersions.end());

 //       auto compare_to_threshold = [dispersion_threshold](double i){ return i < dispersion_threshold; };
 //       auto min_element = std::find_if(dispersions.begin(), dispersions.end(),compare_to_threshold);
 //            min_pos = std::distance(dispersions.begin(),min_element);
 //            min_pos = dispersions.size() - min_pos;

 //            //qDebug() << "FOUND BEST PID WITH ERROR AMPLITUDE  - " << *min_element  
 //            //                                                      << " NUMBER - " << min_pos  
 //            //                                                      << " SIZE - " << dispersions.size();

 //       return min_pos-1;
 //   };

}


AimingParamOptimizator::AimingParamOptimizator()
{
}
void AimingParamOptimizator::Reset()
{
  BestPIDParam = PIDParamTable[0];
  PIDParamGroupStat.Reset();
  BestPIDParamNumber = 0;

  FLAG_CONTROL_PARAM_REGIM = false;
  FLAG_REGISTRATION_REGIM = true;
}

//std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();
//std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint2 = std::chrono::high_resolution_clock::now();
//auto StepPeriod = std::chrono::duration<double>((TimePoint2 - TimePoint)).count();
