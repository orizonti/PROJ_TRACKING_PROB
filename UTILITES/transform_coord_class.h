#ifndef TRANSFORMCOORDCLASS_H
#define TRANSFORMCOORDCLASS_H

#include "state_block_enum.h"
#include "interface_pass_coord.h"
#include <qpair.h>
#include <functional>


class  TransformCoordClass : public PassTwoCoordClass
{
public:
    TransformCoordClass();
    TransformCoordClass(double ScaleParam, double OffsetParam = 0);
    TransformCoordClass(const TransformCoordClass& CopyTransform);
	std::function<void(QPair<double,double>)> TransformCoord;

	const QPair<double, double>& GetOutput();
	                        void SetInput(const QPair<double,double>& Coord);
	QPair<double, double> TransformedCoord;

	double Scale = 1;
	double Offset = 1;
};
#endif //TRANSFORMCOORDCLASS_H
