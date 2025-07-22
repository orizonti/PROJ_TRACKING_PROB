#ifndef TRANSFORMCOORDCLASS_H
#define TRANSFORMCOORDCLASS_H

#include "interface_pass_coord.h"
#include <QPair>
#include <functional>


class  TransformCoordClass : public PassCoordClass<double>
{
public:
    TransformCoordClass() {};
    TransformCoordClass(double ScaleParam, double OffsetParam = 0, double Saturation = 20000)
	{
		SetParam(ScaleParam, OffsetParam, Saturation);
	}
	std::function<void(QPair<double,double>)> TransformCoord;

    const QPair<double, double>& GetOutput() { return Output; }
    void SetInput(const QPair<double,double>& Coord) { TransformCoord(Coord); }

	QPair<double, double> Output;

	double Scale = 1;
	double Offset = 0;
	double Saturation = 20000;
	TransformCoordClass& operator()(double ScaleParam) {Scale = ScaleParam; return *this;}
	void SetParam(double ScaleParam, double OffsetParam, double Saturation)
	{
	Scale = ScaleParam;
	Offset = OffsetParam;
	TransformCoord = [this](QPair<double,double> CoordError)
	{
		Output.first = CoordError.first * Scale  + Offset;  
		Output.second = CoordError.second * Scale + Offset; 

		if(std::abs(Output.first) > this->Saturation) Output.first = this->Saturation*Output.first/std::abs(Output.first);
		if(std::abs(Output.second) > this->Saturation) Output.second = this->Saturation*Output.second/std::abs(Output.second);

	};
	}
};




#endif //TRANSFORMCOORDCLASS_H
