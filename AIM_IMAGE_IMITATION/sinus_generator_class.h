#pragma once
#include "interface_pass_coord.h"
#include "widget_scene_node_interface.h"
#include <QTimer>
#include "transform_coord_class.h"


class SinusGeneratorClass :
	public QObject, public PassCoordClass<float>
{
	Q_OBJECT
public:
	SinusGeneratorClass(QObject* Obj = 0);
	~SinusGeneratorClass();

	std::string TAG_NAME{"[ SIN_GEN ]"};

	void DisplayControlWindow();
	void DisplayControlWindow(QGraphicsScene* Scene);
	PassCoordClass<float>* LinkedReceiver = 0;
	void LinkToDevice(PassCoordClass<float>& Receiver) { LinkedReceiver = &Receiver;}
	void SetTimerPeriod(int Period) { TimerPeriod = Period;};
	void MoveGeneratorToThread(QThread* Thread);

	TransformCoordClass ScaleOutput;

	uint16_t COUNTER = 0;
	uint16_t PERIOD = 360/2;
	int TimerPeriod = 5;

	bool BlockOutput1 = false;
	bool BlockOutput2 = false;

    int Offset = 0;
	QPair<float,float> CurrentOutput;

	QPair<float,float> Amplitude = QPair<float,float>(60*60,60*60);
	//QPair<float,float> Amplitude = QPair<float,float>(3000,3000);

	QPair<float,float> Freq = QPair<float,float>(1,1);
    QPair<float,float> Noize = QPair<float,float>(0,0);
                   double AmplitudeNoize = 3;

    bool FLAG_ENABLE_NOIZE = false;

	QTimer TimerGenerateSinus;

	void setInput(const QPair<float,float>& Coord);
	const QPair<float,float>& getOutput() { return CurrentOutput;};

	public slots:
	void SlotSetFrequency(double Freq1, double Freq2);
	void SlotSetAmplitude(double Ampl1, double Ampl2);
    void SlotSetAmplitudeNoize(double Ampl);
    void SlotEnableChannel(bool Enable , int Channel);

	void SlotCalculateValue();
	void SlotStartGenerate(bool StartStop);
};

