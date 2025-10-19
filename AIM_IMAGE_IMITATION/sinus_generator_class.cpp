#include "sinus_generator_class.h"
#include "interface_pass_coord.h"
#include "widget_sinus_source.h"
#include <random>
#include <math.h>
#include "register_settings.h"

#define TAG "[ SINUS Gen   ]" 

SinusGeneratorClass::SinusGeneratorClass(QObject* Obj) : QObject(Obj)
{
	QObject::connect(&this->TimerGenerateSinus, SIGNAL(timeout()), this, SLOT(SlotCalculateValue()));

    auto OutputGain = SettingsRegister::GetValue("GAIN_ANGLE_DAC");
    ScaleOutput = TransformCoordClass(1,0);

    qDebug() << TAG_NAME << "OUTPUT GAIN: " << OutputGain;
}

void SinusGeneratorClass::MoveGeneratorToThread(QThread* Thread)
{
	QObject::disconnect(&this->TimerGenerateSinus, SIGNAL(timeout()), this, SLOT(SlotCalculateValue()));
    this->moveToThread(Thread);
    TimerGenerateSinus.moveToThread(Thread);
	QObject::connect(&this->TimerGenerateSinus, SIGNAL(timeout()), this, SLOT(SlotCalculateValue()));
}


SinusGeneratorClass::~SinusGeneratorClass()
{
}

void SinusGeneratorClass::setInput(const QPair<float,float>& Coord) { }
void SinusGeneratorClass::SlotSetFrequency(double Freq1, double Freq2) { Freq.first = Freq1; Freq.second = Freq2; PERIOD = 360/Freq1;}
void SinusGeneratorClass::SlotSetAmplitude(double Ampl1, double Ampl2) { Amplitude.first = Ampl1*60*5; 
                                                                         Amplitude.second = Ampl2*60*5; }
void SinusGeneratorClass::SlotSetAmplitudeNoize(double Ampl) { this->AmplitudeNoize = Ampl; }

void SinusGeneratorClass::SlotEnableChannel(bool Enable , int Channel)
{
    qDebug() << "SINUS GEN ENABLE: " << Enable << Channel;
    if(Channel == 1) { BlockOutput1 = !Enable; return;} 
    if(Channel == 2) { BlockOutput2 = !Enable; return;} 

    if(Channel == 3) Enable = Enable;
}



void SinusGeneratorClass::SlotCalculateValue()
{
  COUNTER++; if(COUNTER > PERIOD) COUNTER = 0;

    CurrentOutput.first = Offset + Amplitude.first*std::sin(COUNTER*2*M_PI/PERIOD);
    CurrentOutput.second = Offset + Amplitude.second*std::cos(COUNTER*2*M_PI/PERIOD);

    //CurrentOutput >> ScaleOutput >> CurrentOutput;

    //if(BlockOutput1) CurrentOutput.first  = 0;
    //if(BlockOutput2) CurrentOutput.second = 0;

    PassCoordClass<float>::passCoord();

    if(FLAG_ENABLE_NOIZE)
    {
        Noize.first = AmplitudeNoize*std::rand()/RAND_MAX - AmplitudeNoize/2;
        Noize.second = AmplitudeNoize*std::rand()/RAND_MAX - AmplitudeNoize/2;
    }
    else { Noize.first = 0; Noize.second =0; }
    
}

void SinusGeneratorClass::SlotStartGenerate(bool StartStop)
{
    qDebug() << "SINUS GENERATOR START GENERATE : " << StartStop;
	if (StartStop) this->TimerGenerateSinus.start(50);
	else this->TimerGenerateSinus.stop();
}
