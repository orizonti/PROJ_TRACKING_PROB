#ifndef STAT_ENGINE_H
#define STAT_ENGINE_H

#include "interface_pass_coord.h"
#include <math.h>
#include <functional>
#include <QQueue>
#include <QDebug>

class Statistic
{
public:
	Statistic(const Statistic& Stat) { this->Size = Stat.Size;};
	Statistic() { Size = 10;};
	Statistic(int WindowSize) { Size = WindowSize;};
	void operator=(const Statistic& Stat) { this->Size = Stat.Size;}


	QQueue<QPair<double, double>> CoordMassive;
	QQueue<double> ValueMassive;

	double MinValue = 0;
	double MaxValue = 0;
	double AvarageValue = 0;
	double DispersionValue = 1000;

	int Size = 10;
	QPair<double, double> AvarageCoord;
	QPair<double, double> DispersionCoord{1000,1000};

	double DispersionCoordDistance = 1000;
	double AmplitudeCoordDeviation = 1000;
	int    Counter = 0;

	void CalcDispersion();
	void CalcDispersionValue();
	static double Norm(QPair<double,double> Coord);

	void Reset();
	bool IsValueLoaded() { if (ValueMassive.size() == Size) { return true; } else { return false; } };
	bool IsCoordLoaded() { if (CoordMassive.size() == Size) { return true; } else { return false; } };
	bool IsValueRising();

	friend void operator>>(double NewValue, Statistic& StatObj)
	{
		if (StatObj.MinValue > NewValue) StatObj.MinValue = NewValue;
		if (StatObj.MaxValue < NewValue) StatObj.MaxValue = NewValue;
		if (StatObj.MinValue == 0)       StatObj.MinValue = NewValue;

		StatObj.AvarageValue = StatObj.AvarageValue + NewValue / StatObj.Size;

		if (StatObj.IsValueLoaded()) 
		{
		StatObj.AvarageValue = StatObj.AvarageValue - StatObj.ValueMassive.dequeue() / StatObj.Size;
		StatObj.CalcDispersionValue();
		}

		StatObj.ValueMassive.enqueue(NewValue);
	};


	friend void operator>>(QPair<double, double> NewValue, Statistic& StatObj)
	{
		if (StatObj.IsCoordLoaded())
		{
		QPair<double,double> FirstValue =  StatObj.CoordMassive.dequeue();

		StatObj.AvarageCoord.first -= FirstValue.first/StatObj.Size;
		StatObj.AvarageCoord.second -= FirstValue.second/StatObj.Size;

		}

		StatObj.CoordMassive.enqueue(NewValue);

		StatObj.AvarageCoord.first += NewValue.first/StatObj.Size;
		StatObj.AvarageCoord.second += NewValue.second/StatObj.Size;

		StatObj.Counter++; // Dispersion calcs every size values


		if (StatObj.IsCoordLoaded())
		{
			if (StatObj.Counter >= StatObj.Size)
			{
		     StatObj.CalcDispersion();
			 StatObj.Counter = 0;
			}
		}
	};


	const double& GetDispersionValue() { return DispersionValue;};
	const double& GetAvarageValue() {    return AvarageValue;};
	QPair<double, double> GetAvarageCoord() { return AvarageCoord;};
	QPair<double, double> GetDispersionCoord() { return DispersionCoord;};
	      double GetDispersionNorm() { return Norm(DispersionCoord);};

	void SetSize(int SizeStat){ this->Size = SizeStat;};

};

class StatisticGroup
{
public:
    StatisticGroup(int Count, int WindowSize);
    StatisticGroup(const StatisticGroup& Group);
    void operator=(const StatisticGroup& Group);
    std::map<int,Statistic> Statistics;

    void Reset();
    bool IsBestStatisticFaund() { return BestStatNumber != -1; };
    bool IsStatisticsLoaded() { return Statistics[Statistics.size()-1].IsCoordLoaded();};
    void PerformAvailableData();

    friend void operator>>(double NewValue, StatisticGroup& StatObj);
    friend void operator>>(QPair<double, double> NewValue, StatisticGroup& StatObj);


    int BestStatNumber = 1;
    int BestStatNumberValue = 0;
    std::map<int,Statistic>::iterator CurrentStatistic;
    std::map<int,Statistic>::iterator endStatistic;
    std::map<int,Statistic>::iterator BeginStatistic;

    std::function<int (std::map<int,Statistic>::iterator,std::map<int,Statistic>::iterator)> FindBestStatisticValue;
    std::function<int (std::map<int,Statistic>::iterator,std::map<int,Statistic>::iterator)> FindBestStatisticCoord;
private:
    int GetBestStatisticsCoord();
    int GetBestStatisticsValue();


};

#endif // STAT_enGINE_H
