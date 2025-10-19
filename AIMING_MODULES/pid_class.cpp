#include "pid_class.h"

#define TAG "[ PID CONTROL ]" 

PIDClass::PIDClass()
{
	this->CoordAimingError = QPair<float,float>(-10000, -10000);
	this->StateBlock = StateBlockAtWork;
	this->ErrorsSumm = QPair<float,float>(0, 0);

	TimeFromLastCommand = std::chrono::high_resolution_clock::now();
	PIDControlOutput = QPair<float,float>(0, 0);
}
PIDClass::~PIDClass() { }


void PIDClass::ResetPID()
{
	this->ErrorsSumm = QPair<float,float>(0, 0);
    this->PIDControlOutput = QPair<float,float>(0, 0);
    this->CoordAimingError = QPair<float,float>(0, 0);
}

QPair<float,float> PIDClass::CalcVelocityToengine(QPair<float,float> CoordError)
{

	if(CoordError.first == CoordAimingError.first && CoordError.second == CoordAimingError.second) return  this->PIDControlOutput;

	std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();
	double StepPeriod = std::chrono::duration<double>((TimePoint - TimeFromLastCommand)).count();
    TimeFromLastCommand = TimePoint;

	//if (StepPeriod > StepPeriodThreshold) return  this->PIDControlOutput;

	double DerivateErrorXAxis = 0; double DerivateErrorYAxis = 0;

	this->ErrorsSumm.first  = ErrorsSumm.first  + CoordError.first*StepPeriod;
	this->ErrorsSumm.second = ErrorsSumm.second + CoordError.second*StepPeriod;

	if (CoordAimingError.first != -10000 && CoordAimingError.second != -10000)
	{
		DerivateErrorXAxis = (CoordError.first - this->CoordAimingError.first) / StepPeriod;
		DerivateErrorYAxis = (CoordError.second - this->CoordAimingError.second) / StepPeriod;
	}
	this->PIDControlOutput.second = CoordError.first*PIDParam.RateParam + ErrorsSumm.first*PIDParam.IntParam + DerivateErrorXAxis*PIDParam.DiffParam;
	this->PIDControlOutput.first  = CoordError.second*PIDParam.RateParam + ErrorsSumm.second*PIDParam.IntParam + DerivateErrorYAxis*PIDParam.DiffParam;
	
	//--------------------------------------------------------->
	this->CoordAimingError = CoordError;

	return PIDControlOutput;

}

void PIDClass::setInput(const QPair<float,float>& Coord)
{
	auto CoordNew = Coord;
	CoordNew.second = Coord.first; CoordNew.first = Coord.second; 
	this->CalcVelocityToengine(CoordNew);
}

const QPair<float,float>& PIDClass::getOutput()                 { return PIDControlOutput; }
                 void PIDClass::SetPIDParam(PIDParamStruct Param) { this->PIDParam = Param; this->ResetPID(); }
                 void PIDClass::SetFrameRate(double Rate)         { StepPeriodThreshold = 2.0/Rate; }


