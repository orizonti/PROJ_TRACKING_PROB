#ifndef PIDCLASS_H
#define PIDCLASS_H

#include "interface_pass_coord.h"
#include "pid_param_struct.h"
#include <chrono>
#include "state_block_enum.h"
#include <QTime>
#include "module_period_measure.h"


class PIDClass : public PassCoordClass<float>
{
public:

   PIDClass() { this->StateBlock = StateBlockAtWork; }
  ~PIDClass() { }

	StateEnumBlock StateBlock = StateEnumBlock::StateBlockDisabled;

	void setInput(const QPair<float,float>& Coord) override;

	QPair<float,float> getLastInput() { return this->CoordInput;};

	void Reset();

	void setGainList(std::vector<float> Gains);
	void setGainList(float gain1, float gain2, float gain3, float gain4);

	void setFrameRate(double Rate);

	float PeriodThreshold = 10;
	float PeriodInput = 10;

  int LimitIntegrator = 30000;
  int LimitVelocity   = 10;

  std::vector<float> GainList{1,0,0,0,0,0};
private:
	QPair<float,float> SummInput {0,0};
	QPair<float,float> SummInputLast {0,0};

	QPair<float,float> VelocityInput {0,0};
	QPair<float,float> VelocityInputLast {0,0};

	QPair<float,float> LastCoord {0,0};
	QPair<float,float> CoordInput{0,0};
   MeasurePeriodNode MeasurePeriod;

	void processInput(QPair<float,float> CoordError);
};
#endif //PIDCLASS_H
