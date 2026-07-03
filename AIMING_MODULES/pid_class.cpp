#include "pid_class.h"

#define TAG "[PID_CONTROL]" 


void PIDClass::Reset()
{
	  this->SummInput = QPair<float,float>(0, 0);
    this->OutputCoord = QPair<float,float>(0, 0);
    this->CoordInput = QPair<float,float>(0, 0);
}

void PIDClass::setGainList(std::vector<float> Gains)
{

      auto GainCommon = 1; if(Gains.size() >= 4) GainCommon = Gains[3];
      auto GainSet = Gains.begin();
  for(auto& Gain: GainList)
  {
   Gain = (*GainSet)*GainCommon; GainSet++; if(GainSet == Gains.end()) return; 
  }

  qDebug() << "[PID SET]" << GainList[0] << GainList[1] << GainList[2];
}

void PIDClass::setGainList(float gain1, float gain2, float gain3, float gain4)
{
  GainList[0] = gain1*gain4; 
  GainList[1] = gain2*gain4;
  GainList[2] = gain3*gain4; qDebug() << "[PID SET]" << GainList[0] << GainList[1] << GainList[2];
}


void PIDClass::setInput(const QPair<float,float>& Coord)
{

                    MeasurePeriod++;
      PeriodInput = MeasurePeriod.getSeconds();
	if (PeriodInput > PeriodThreshold || PeriodInput == 0) return;

	SummInput.first  = SummInput.first  + Coord.first *PeriodInput;
	SummInput.second = SummInput.second + Coord.second*PeriodInput;

  if(abs(SummInput.first) > LimitIntegrator) SummInput.first  = SummInputLast.first;
  if(abs(SummInput.first) > LimitIntegrator) SummInput.second = SummInputLast.second;

	VelocityInput.first  = (Coord.first  - LastCoord.first)  / PeriodInput;
	VelocityInput.second = (Coord.second - LastCoord.second) / PeriodInput;

  if(abs(VelocityInput.first ) > LimitVelocity) VelocityInput.first  = VelocityInputLast.first;
  if(abs(VelocityInput.second) > LimitVelocity) VelocityInput.second = VelocityInputLast.second;

	this->OutputCoord.second = Coord.first *GainList[0] + SummInput.first *GainList[1] + VelocityInput.first *GainList[2];
	this->OutputCoord.first  = Coord.second*GainList[0] + SummInput.second*GainList[1] + VelocityInput.second*GainList[2];
	
	//--------------------------------------------------------->
	this->LastCoord = Coord;
}



