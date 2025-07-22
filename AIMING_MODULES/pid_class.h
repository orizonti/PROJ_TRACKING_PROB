#ifndef PIDCLASS_H
#define PIDCLASS_H

#include "interface_pass_coord.h"
#include "pid_param_struct.h"
#include <chrono>
#include "state_block_enum.h"
#include <QTime>


class PIDClass : public PassCoordClass<double>
{
public:
	PIDClass();
	~PIDClass();

	StateEnumBlock StateBlock = StateEnumBlock::StateBlockDisabled;
	
	QPair<double, double> CalcVelocityToengine(QPair<double, double> CoordError);
	QPair<double, double> CalcStep(QPair<double, double> CoordError);
	const QPair<double, double>& GetOutput();
	QPair<double, double> GetLastCoordError() { return this->CoordAimingError;};
	void ResetPID();
	void SetInput(const QPair<double,double>& Coord);
	void SetPIDParam(PIDParamStruct Param);
	void SetFrameRate(double Rate);
	double StepPeriodThreshold = 0.01;

	std::chrono::time_point<std::chrono::high_resolution_clock> TimeFromLastCommand;
	PIDParamStruct PIDParam;
	QPair<double, double> ErrorsSumm;
private:
    //double MaxAccelAxis1 = 20*4.84/1000000;
    //double MaxAccelAxis2 = 20*4.84/1000000;
	QPair<double, double> PIDControlOutput;
	QPair<double, double> CoordAimingError;
    //QTime timeAiming;
};
#endif //PIDCLASS_H
