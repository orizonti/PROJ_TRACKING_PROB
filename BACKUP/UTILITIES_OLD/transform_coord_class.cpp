#include "transform_coord_class.h"  


const QPair<float,float>& TransformCoordClass::getOutput() { return TransformedCoord; }
void TransformCoordClass::setInput(const QPair<float,float>& Coord) { TransformCoord(Coord); }

TransformCoordClass::TransformCoordClass()
{
	Scale = M_PI / (60.0 * 60.0 * 180.0);
	Offset = 0;
	TransformCoord = [this](QPair<float,float> CoordError)
	{
		TransformedCoord.first = CoordError.first * Scale + Offset;
		TransformedCoord.second = CoordError.second * Scale + Offset;
	};
	//TRANSFORM SECS TO RADIAN
}
TransformCoordClass::TransformCoordClass(double ScaleParam, double OffsetParam)
{
	Scale = ScaleParam;
	Offset = OffsetParam;
	TransformCoord = [this](QPair<float,float> CoordError)
	{
		TransformedCoord.first = CoordError.first * Scale  + Offset;
		TransformedCoord.second = CoordError.second * Scale + Offset;
	};
}

TransformCoordClass::TransformCoordClass(const TransformCoordClass& CopyTransform)
{
	Scale = CopyTransform.Scale;
	TransformCoord = CopyTransform.TransformCoord;
}

