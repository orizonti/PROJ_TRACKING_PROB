#ifndef STAT_ENGINE_H
#define STAT_ENGINE_H

#include "interface_pass_coord.h"
#include "interface_pass_value.h"
#include <math.h>
#include <functional>
#include <QQueue>
#include <QDebug>
#include "debug_output_filter.h"
static OutputFilter EngFilter{20};
static OutputFilter EngFilter2{40};


template<typename T>
class StatisticValue  : public PassValueClass<T>
{
public:
	StatisticValue() { SetSize(10);};
	StatisticValue(int WindowSize) {SetSize(WindowSize);};

	void SetSize(int SizeStat){StatSample.resize(SizeStat); Reset(); this->Size = SizeStat;};

	std::vector<T> StatSample;

	decltype(StatSample.begin()) CurrentElement = StatSample.begin();
	decltype(StatSample.begin()) EndPos = StatSample.end();

	T ValueMax = 0;
	T ValueMin = 0;
	T ValueAvarage = 0;
	T ValueDispersion = 0;

	int Size = 10;
	bool StatLoaded = false;

	void CalcDispersion();

	void Reset(){ValueMax = 0; ValueMin = 0; ValueAvarage = 0; ValueDispersion = 0; 
				 std::fill(StatSample.begin(), StatSample.end(),0); 
				 CurrentElement = StatSample.begin(); EndPos = StatSample.end(); StatLoaded = false;};

	bool isLoaded() { return StatLoaded; };
	bool isValueRising();

	void SetValue(T Input) override;
	const T& GetValue() override { return ValueAvarage;}

	const T& GetDispersionValue() { return ValueDispersion;};
	const T& GetAvarageValue() {    return ValueAvarage;};

};

template<typename T>
class StatisticCoord  : public PassCoordClass<T>
{
public:
	StatisticCoord() {SetSize(10); };
	StatisticCoord(int WindowSize) { SetSize(WindowSize); };

    void SetSize(int WindowSize) { Size = WindowSize; StatSample.resize(Size); Reset();}
	std::vector<QPair<T,T>> StatSample;
	decltype(StatSample.begin()) CurrentElement = StatSample.begin();
	decltype(StatSample.begin()) EndPos = StatSample.end();

	QPair<T,T> CoordMax{0,0};
	QPair<T,T> CoordMin{0,0};
	QPair<T,T> CoordAvarage{0,0};
	QPair<T,T> CoordDispersion{0,0};
	
	T NormMin = 0;
	T NormMax = 0;
	T NormAvarage = 0;
	T NormDispersion = 0;

	T DeviationMax = 0;
	T DeviationAvarage = 0;

	int Size = 10;
	bool StatLoaded = false;

	void CalcDispersion();

	void Reset(){QPair<T,T> Null(0,0); CoordMax = Null; CoordMin = Null; CoordAvarage = Null; CoordDispersion = Null;
	                                   NormMin = 0; NormMax = 0; NormAvarage = 0; 
									   std::fill(StatSample.begin(), StatSample.end(),Null); 
									   CurrentElement = StatSample.begin(); EndPos = StatSample.end(); StatLoaded = false;
									};

	bool isLoaded() { return StatLoaded;};
	bool isValueRising();

	void setInput(const QPair<T,T>& Input);
	const QPair<T,T>& getOutput() { return CoordAvarage;};

	static T Norm(QPair<T,T> Coord) { return std::sqrt(std::pow(Coord.first,2) + std::pow(Coord.second,2)); }

	const QPair<T, T>& GetAvarageCoord() { return CoordAvarage;};
	const QPair<T, T>& GetMinCoord() { return CoordMin;};
	const QPair<T, T>& GetMaxCoord() { return CoordMax;};

	const QPair<T, T>& GetDispersionCoord() { return CoordDispersion;};
	               T   GetDispersionNorm() { return Norm(CoordDispersion);};

		T GetMaxDeviation() { return DeviationMax; };
		T GetAvarageDeviation() { return DeviationAvarage; }


};

template<typename T = float>
class StatisticNode : public PassValueClass<T>, public PassCoordClass<T>
{
	public:
	StatisticNode() { };
	StatisticNode(int size) {SetSize(size);};
	void SetSize(int size) {NodeValue.SetSize(size); NodeCoord.SetSize(size);  }

	StatisticValue<T> NodeValue{10};
	StatisticCoord<T> NodeCoord{10};

	 const QPair<T, T>& getOutput() override { return NodeCoord.getOutput();};
	 void setInput(const QPair<T, T>& Coord) override  { Coord >> NodeCoord;};

	 const T& GetValue() override  { return NodeValue.GetValue();};
	 void SetValue(T InputValue) override  { InputValue >> NodeValue; };

	 const T& GetAvarageValue() { return NodeValue.GetAvarageValue();}

	 bool IsValueLoaded() { return NodeValue.isLoaded();}
	 bool IsCoordLoaded() { return NodeCoord.isLoaded();}
	 T GetDispersionNorm() { return NodeCoord.GetDispersionNorm();}
	 T GetDispersionValue() { return NodeValue.GetDispersionValue();}
	 void Reset() { NodeValue.Reset(); NodeCoord.Reset();}
};
//================================================================================

template<typename T>
void StatisticValue<T>::SetValue(T Input) 
{
	if (ValueMin > Input) ValueMin = Input;
	if (ValueMax < Input) ValueMax = Input;
	if (ValueMin == 0)    ValueMin = Input;

	ValueAvarage += Input / Size;
	ValueAvarage -= *CurrentElement / Size; 
	                *CurrentElement = Input; 
					 CurrentElement++; 

	if(CurrentElement == EndPos) {CurrentElement = StatSample.begin(); CalcDispersion(); StatLoaded= true;}


};

template<typename T>
void StatisticValue<T>::CalcDispersion()
{
	ValueDispersion = 0;

	for(auto Value: StatSample)
	ValueDispersion += std::pow(Value - ValueAvarage, 2) / StatSample.size();
}

//================================================================================

template<typename T>
void StatisticCoord<T>::setInput(const QPair<T,T>& Input)
{
	//qDebug() << EngFilter << "[ STAT COORD SET ] " << Input.first << Input.second << "AVARAGE: " << CoordAvarage.first << CoordAvarage.second;

     CoordAvarage.first -= (*CurrentElement).first/Size;
    CoordAvarage.second -= (*CurrentElement).second/Size; 
	
     CoordAvarage.first += Input.first/Size;
    CoordAvarage.second += Input.second/Size;

	*CurrentElement = Input; 
	 CurrentElement++; if(CurrentElement == EndPos) { CurrentElement = StatSample.begin(); CalcDispersion(); StatLoaded = true;}

};


template<typename T>
void StatisticCoord<T>::CalcDispersion()
{
	CoordDispersion = QPair<T, T>(0, 0);
	DeviationAvarage = 0;

	QPair<T, T> CoordDeviation;
	std::vector<T> DeviationMeasures;

	for (auto& Coord : StatSample)
	{
		CoordDispersion.first += std::pow(Coord.first - CoordAvarage.first, 2) / StatSample.size();
		CoordDispersion.second += std::pow(Coord.second - CoordAvarage.second, 2) / StatSample.size();

		CoordDeviation = (Coord - CoordAvarage);
		DeviationAvarage += std::hypot(CoordDeviation.first,CoordDeviation.second)/StatSample.size();

		DeviationMeasures.push_back(std::hypot(CoordDeviation.first,CoordDeviation.second));
	}
		NormDispersion = std::hypot(CoordDispersion.first,CoordDispersion.second);
	    DeviationMax = *(std::max_element(DeviationMeasures.begin(),DeviationMeasures.end()));


	//qDebug() <<  "[ STAT COORD DEVIATE ] " << CoordDispersion.first << CoordDispersion.second 
	//		 <<  "[ AVARAGE ] " << CoordAvarage.first << CoordAvarage.second;

}

//================================================================================

template<typename T = float>
class StatisticGroup
{
public:
    StatisticGroup(int Count, int WindowSize) {};
    StatisticGroup(const StatisticGroup& Group) {};
    void operator=(const StatisticGroup& Group) {};

    std::map<int,StatisticNode<T>> Statistics;

    void Reset() {};
    bool IsBestStatisticFaund() { return BestStatNumber != -1; };
    bool IsStatisticsLoaded() { return Statistics[Statistics.size()-1].IsCoordLoaded();};
    void PerformAvailableData() {};

    friend void operator>>(double NewValue, StatisticGroup& StatObj) {};
    friend void operator>>(QPair<float,float> NewValue, StatisticGroup& StatObj) {};


    int BestStatNumber = 1;
    int BestStatNumberValue = 0;
    //std::map<int,StatisticNode<T> >::iterator CurrentStatistic;
    //std::map<int,StatisticNode<T> >::iterator endStatistic;
    //std::map<int,StatisticNode<T> >::iterator BeginStatistic;

    //std::function<int (std::map<int,StatisticNode<T>>::iterator,std::map<int,StatisticNode<T> >::iterator)> FindBestStatisticValue;
    //std::function<int (std::map<int,StatisticNode<T>>::iterator,std::map<int,StatisticNode<T> >::iterator)> FindBestStatisticCoord;
private:
    int GetBestStatisticsCoord() { return 0;};
    int GetBestStatisticsValue() { return 0;};


};


#endif // STAT_enGINE_H
