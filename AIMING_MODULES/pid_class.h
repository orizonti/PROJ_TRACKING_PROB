#ifndef PIDCLASS_H
#define PIDCLASS_H

#include "interface_pass_coord.h"
#include "pid_param_struct.h"
#include <chrono>
#include "state_block_enum.h"
#include <QTime>


class PIDClass : public PassCoordClass<float>
{
public:
	PIDClass();
	~PIDClass();

	StateEnumBlock StateBlock = StateEnumBlock::StateBlockDisabled;
	
	QPair<float,float> CalcVelocityToengine(QPair<float,float> CoordError);
	QPair<float,float> CalcStep(QPair<float,float> CoordError);
	const QPair<float,float>& getOutput();
	QPair<float,float> GetLastCoordError() { return this->CoordAimingError;};
	void ResetPID();
	void setInput(const QPair<float,float>& Coord);
	void SetPIDParam(PIDParamStruct Param);
	void SetFrameRate(double Rate);
	double StepPeriodThreshold = 0.01;

	std::chrono::time_point<std::chrono::high_resolution_clock> TimeFromLastCommand;
	PIDParamStruct PIDParam;
	QPair<float,float> ErrorsSumm;
private:
    //double MaxAccelAxis1 = 20*4.84/1000000;
    //double MaxAccelAxis2 = 20*4.84/1000000;
	QPair<float,float> PIDControlOutput;
	QPair<float,float> CoordAimingError;
    //QTime timeAiming;
};
#endif //PIDCLASS_H
