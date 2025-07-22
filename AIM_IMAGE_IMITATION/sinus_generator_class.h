#pragma once
#include "interface_pass_coord.h"
#include "widget_scene_node_interface.h"
#include <QTimer>
#include "transform_coord_class.h"


class SinusGeneratorClass :
	public QObject, public PassCoordClass<double>
{
	Q_OBJECT
public:
	SinusGeneratorClass(QObject* Obj = 0);
	~SinusGeneratorClass();

	QString TAG_NAME{"[ SIN_GEN ]"};

	void DisplayControlWindow();
	void DisplayControlWindow(QGraphicsScene* Scene);
	PassCoordClass<double>* LinkedReceiver = 0;
	void LinkToDevice(PassCoordClass<double>& Receiver) { LinkedReceiver = &Receiver;}
	void SetTimerPeriod(int Period) { TimerPeriod = Period;};
	void MoveGeneratorToThread(QThread* Thread);

	TransformCoordClass ScaleOutput;

	uint16_t COUNTER = 0;
	uint16_t PERIOD = 360/2;
	int TimerPeriod = 5;

	bool BlockOutput1 = false;
	bool BlockOutput2 = false;

    int Offset = 0;
	QPair<double, double> CurrentOutput;

	QPair<double, double> Amplitude = QPair<double,double>(60*60,60*60);
	//QPair<double, double> Amplitude = QPair<double,double>(3000,3000);

	QPair<double, double> Freq = QPair<double,double>(1,1);
    QPair<double, double> Noize = QPair<double,double>(0,0);
                   double AmplitudeNoize = 3;

    bool FLAG_ENABLE_NOIZE = false;

	QTimer TimerGenerateSinus;

	void SetInput(const QPair<double,double>& Coord);
	const QPair<double,double>& GetOutput() { return CurrentOutput;};

	public slots:
	void SlotSetFrequency(double Freq1, double Freq2);
	void SlotSetAmplitude(double Ampl1, double Ampl2);
    void SlotSetAmplitudeNoize(double Ampl);
    void SlotEnableChannel(bool Enable , int Channel);

	void SlotCalculateValue();
	void SlotStartGenerate(bool StartStop);
};

