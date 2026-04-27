#pragma once
#include "interface_pass_coord.h"
#include "transform_coord_class.h"
#include <QTime>
#include <chrono>
#include <QChronoTimer>

class SinusGeneratorClass : public QObject, public PassCoordClass<float>
{
	Q_OBJECT
public:
	SinusGeneratorClass(QObject* Obj = 0);
	~SinusGeneratorClass();

	std::string TAG_NAME{"[ SIN_GEN ]"};

	PassCoordClass<float>* LinkedReceiver = 0;
	void linkToDevice(PassCoordClass<float>& Receiver) { LinkedReceiver = &Receiver;}
	bool isActive() { return TimerGenerateSinus->isActive(); }

	int TimerPeriod = 10;
	QChronoTimer* TimerGenerateSinus;
	     QThread* TimerThread;

	std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

	void moveToAnotherThread();

	QPair<float,float> CurrentOutput;
	QPair<float,float> Amplitude = QPair<float,float>(20000,20000);

	QPair<float,float>    Offset = QPair<float,float>(0.0, 0.0);
	QPair<float,float>      Freq = QPair<float,float>(0.2, 0.2);
	QPair<float,float>      Step = QPair<float,float>(TimerPeriod*Freq.first*2*M_PI/1000.0, TimerPeriod*Freq.second*2*M_PI/1000.0);
	QPair<int  ,int  >    Period = QPair<int  ,int  >(2*M_PI/Step.first, 2*M_PI/Step.second);
    QPair<float,float>     Noize = QPair<float,float>(0,0);
	QPair<int  ,int  > CounterStep = QPair<int,int>(0,0);

	float AmplitudeNoize = 3;
	bool FlagEnableOutput1 = true;
	bool FlagEnableOutput2 = true;
    bool FlagEnableNoize  = false;

	TransformCoordClass ScaleOutput;

	                     void setInput(const QPair<float,float>& Coord);
	const QPair<float,float>& getOutput() override { return CurrentOutput;};

	public slots:
	void slotSetFrequency(float Freq1) { slotSetFrequency(Freq1, Freq1); }
	void slotSetFrequency(float Freq1, float Freq2);

	void slotSetAmplitude(float Ample1) { slotSetAmplitude(Ample1, Ample1); }
	void slotSetAmplitude(float Ampl1, float Ampl2);

	void slotSetOffset(float Value1, float Value2 ) { Offset.first = Value1; Offset.second = Value2; };
	void slotSetOffset(float Value) { slotSetOffset(Value,Value); };

	void slotSetAmplitudeNoize(float Ampl);
	void slotEnableChannel(bool Enable , int Channel);

	void slotCalculateValue();
	void slotStartGenerate(bool StartStop);

	signals:
	void signalEndWork();
	void signalStartGenerate();
	void signalStopGenerate();
    void signalNewCoord(QPair<float,float>);
};

