#include "widget_sinus_source.h"
#include <qnamespace.h>
#include <QPushButton>

WidgetSinusSource::WidgetSinusSource(QWidget *parent)
	: WidgetAdjustable(parent)
{
	ui.setupUi(this);
}

WidgetSinusSource::~WidgetSinusSource()
{
}



void WidgetSinusSource::LinkToModule(std::shared_ptr<SinusGeneratorClass> SinusGenerator)
{
    qDebug() << "WINDOW SINUS SOURCE CONNECT TO GENERATOR";

	QVector<QSpinBox*> Spins;

    QObject::connect(this, SIGNAL(SignalStart(bool)), SinusGenerator.get(), SLOT(SlotStartGenerate(bool)), Qt::QueuedConnection);


    connect(ui.SpinAmp,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    [this, SinusGenerator]()
    {
        SinusGenerator->SlotSetAmplitude(ui.SpinAmp->value(), ui.SpinAmp->value());
    });

	connect(ui.SpinFreq, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		[this, SinusGenerator]()
	    {
		SinusGenerator->SlotSetFrequency(ui.SpinFreq->value(), ui.SpinAmp->value());
		});


	//SinusGenerator->SlotSetFrequency(ui.SpinFreqY->value(),ui.SpinFreq->value());
    //SinusGenerator->SlotSetAmplitude(ui.SpinAmpY->value(),ui.SpinAmp->value());

    //spinNoizeAmplitude

    connect(ui.checkChannel1, &QPushButton::toggled,
            [=](bool Checked)
            {
                SinusGenerator->SlotEnableChannel(Checked,1);
            });
    connect(ui.checkChannel2, &QPushButton::toggled,
            [=](bool Checked)
            {
                SinusGenerator->SlotEnableChannel(Checked,2);
            });

    connect(ui.checkWorkBlock, &QPushButton::toggled,
        [=,this](bool Checked)
    {
        emit SignalStart(Checked);
    });

}
