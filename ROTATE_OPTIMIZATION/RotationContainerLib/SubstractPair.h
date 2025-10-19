#ifndef SUBSTRACTPAIR_H
#define SUBSTRACTPAIR_H

#include "interface_pass_coord.h"


template <class Type>

class Substract : public PassCoordClass<float>
{
public:
	QPair<Type, Type> CoordFirst;
	QPair<Type, Type> CoordSecond;
	QPair<Type, Type> CoordOutput;

	int InputCount = 0;

	 const QPair<float,float>& getOutput()
	 {
		 return CoordOutput;
	 }
	 void setInput(const QPair<float,float>& Coord)
	 {
		 
			if (InputCount == 0)
			{
				CoordFirst = Coord;
				InputCount++;
			}
			else
			{
				CoordSecond = Coord;
				InputCount = 0;
			CoordOutput.first = CoordFirst.first - CoordSecond.first;
			CoordOutput.second = CoordFirst.second - CoordSecond.second;
			}
	 }

};



template <class Type>
class SummVec : public PassCoordClass<float>
{
public:
	QPair<Type, Type> CoordOutput;

	 const QPair<float,float>& getOutput()
	 {
		 QPair<Type, Type> Return = CoordOutput;
		CoordOutput.first = 0;
		CoordOutput.second = 0;
		return Return;
	 }
	 void setInput(const QPair<float,float>& Coord)
	 {
		 
		CoordOutput.first += Coord.first;
		CoordOutput.second += Coord.second;
	 }

	friend SummVec<Type>& operator<<(SummVec<Type>& SubObj,QPair<Type, Type> Coord)
    {

		SubObj.setInput(Coord);
			return SubObj;

    }
	friend SummVec<Type>& operator<<( SummVec<Type>& SubObj,PassCoordClass<float>& Sender)
    {

		SubObj.setInput(Sender.getOutput());
			return SubObj;

    }
};
#endif //SUBSTRACTPAIR_H
