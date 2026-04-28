#include "sinus_generator_class.h"
#include "interface_pass_coord.h"
#include <random>
#include <math.h>
#include "register_settings.h"
#include "debug_output_filter.h"
#include "QThread"
#include <QString>
#include <iostream>

#define TAG "[ SINUS Gen   ]" 

SinusGeneratorClass::SinusGeneratorClass(QObject* Obj) : QObject(Obj)
{
                     this->TimerGenerateSinus = new QChronoTimer(std::chrono::nanoseconds(TimerPeriod*1000000));
                     this->TimerGenerateSinus->setTimerType(Qt::PreciseTimer);
	QObject::connect(this->TimerGenerateSinus, SIGNAL(timeout()), this, SLOT(slotCalculateValue()));

	 QObject::connect(this, SIGNAL(signalStartGenerate()), TimerGenerateSinus, SLOT(start()));
	 QObject::connect(this, SIGNAL(signalStopGenerate()) , TimerGenerateSinus, SLOT(stop() ));

    auto OutputGain = SettingsRegister::getValue("GAIN_ANGLE_DAC");
         ScaleOutput = TransformCoordClass(1,0);
}

void SinusGeneratorClass::moveToAnotherThread()
{
    //qDebug() << "[ SINUS GENERATOR MOVE TO THREAD ]";
	QObject::disconnect(this->TimerGenerateSinus, SIGNAL(timeout()), this, SLOT(slotCalculateValue()));
	QObject::disconnect(this, SIGNAL(signalStartGenerate()), TimerGenerateSinus, SLOT(start()));
	QObject::disconnect(this, SIGNAL(signalStopGenerate()) , TimerGenerateSinus, SLOT(stop() ));

                            TimerThread = new QThread;
                            TimerGenerateSinus->moveToThread(TimerThread);
	 QObject::connect(this->TimerGenerateSinus, SIGNAL(timeout()), this, SLOT(slotCalculateValue()), Qt::DirectConnection);

	 QObject::connect(this, SIGNAL(signalStartGenerate()), TimerGenerateSinus, SLOT(start()), Qt::QueuedConnection);
	 QObject::connect(this, SIGNAL(signalStopGenerate()) , TimerGenerateSinus, SLOT(stop() ), Qt::QueuedConnection);

	 QObject::connect(this, SIGNAL(signalEndWork()), TimerGenerateSinus, SLOT(stop()), Qt::QueuedConnection);
	 QObject::connect(this, SIGNAL(signalEndWork()), TimerGenerateSinus, SLOT(deleteLater()), Qt::QueuedConnection);

	 QObject::connect(this, SIGNAL(signalEndWork()), TimerThread, SLOT(quit())       , Qt::QueuedConnection);
	 QObject::connect(this, SIGNAL(signalEndWork()), TimerThread, SLOT(deleteLater()), Qt::QueuedConnection);
     TimerThread->start();
     TimerThread->setPriority(QThread::NormalPriority);
}


SinusGeneratorClass::~SinusGeneratorClass()
{
    emit signalEndWork();
    QThread::currentThread()->msleep(200);
}

void SinusGeneratorClass::setInput(const QPair<float,float>& Coord) { }
void SinusGeneratorClass::slotSetFrequency(float Freq1, float Freq2) 
{ 
Freq.first  = Freq1; Freq.second = Freq2; 
Step.first  = TimerPeriod*Freq.first*2*M_PI/1000.0;
Step.second = TimerPeriod*Freq.second*2*M_PI/1000.0;

Period.first  = 2*M_PI/Step.first;
Period.second = 2*M_PI/Step.second;
qDebug() << "=====================================";
qDebug() << "[ SINUS SET FREQ   ]" << Freq1 << Freq2;
qDebug() << "[ SINUS SET STEP   ]" << Step.first << Step.second;
qDebug() << "[ SINUS SET PERIOD ]" << Period.first << Period.second;
qDebug() << "=====================================";
}

void SinusGeneratorClass::slotSetAmplitude(float Ampl1, float Ampl2) { Amplitude.first = Ampl1; 
                                                                       Amplitude.second = Ampl2; }
void SinusGeneratorClass::slotSetAmplitudeNoize(float Ampl) { this->AmplitudeNoize = Ampl; }

void SinusGeneratorClass::slotEnableChannel(bool Enable , int Channel)
{
    if(Channel == 1) { FlagEnableOutput1 = Enable; return;} 
    if(Channel == 2) { FlagEnableOutput2 = Enable; return;} 
    if(Channel == 3) Enable = Enable;
}



void SinusGeneratorClass::slotCalculateValue()
{


     std::chrono::duration<double> Dur = std::chrono::high_resolution_clock::now() - TimePoint; //DURATION SECONDS
     auto PeriodMeasure2 = std::chrono::duration_cast<std::chrono::milliseconds>(Dur).count(); //DURATION MILLISECONDS
     TimePoint = std::chrono::high_resolution_clock::now();

    //qDebug() << OutputFilter::Filter(100) << "SINUS  : " << CurrentOutput.first 
    //         << "COUNTER: " << CounterStep.first<< "PERIOD: " << PeriodMeasure2;


    CounterStep.first++ ; if(CounterStep.first  > Period.first ) CounterStep.first  = 0;
    CounterStep.second++; if(CounterStep.second > Period.second) CounterStep.second = 0;

    CurrentOutput.first  = Offset.first  + Amplitude.first *std::sin(CounterStep.first *Step.first);
    CurrentOutput.second = Offset.second + Amplitude.second*std::cos(CounterStep.second*Step.second);


    Noize.first  = AmplitudeNoize*std::rand()/RAND_MAX - AmplitudeNoize/2;
    Noize.second = AmplitudeNoize*std::rand()/RAND_MAX - AmplitudeNoize/2;

    if(!FlagEnableNoize) { Noize.first = 0; Noize.second = 0; }

    if(!FlagEnableOutput1) CurrentOutput.first  = 0;
    if(!FlagEnableOutput2) CurrentOutput.second = 0;

    emit signalNewCoord(CurrentOutput);
    //CurrentOutput >> ScaleOutput >> CurrentOutput;

    PassCoordClass<float>::passCoord();

//    std::cout << "SINUS  : " << CurrentOutput.first << " "
//              << "COUNTER: " << CounterStep.first<< "PERIOD: " << PeriodMeasure2 << std::endl;
    
}

void SinusGeneratorClass::slotStartGenerate(bool StartStop)
{
    //qDebug() << "[ SINUS GENERATOR ] START GENERATE  [ " << StartStop << " ]";
	if (StartStop) emit signalStartGenerate();
	          else emit signalStopGenerate() ;
}
