#ifndef WidgetSinusSource_H
#define WidgetSinusSource_H

#include <QWidget>
#include "ui_widget_sinus_source.h"
#include "widget_scene_node_interface.h"
#include "sinus_generator_class.h"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "widget_adjustable.h"

class WidgetSinusSource : public WidgetAdjustable
{
	Q_OBJECT
public:
	WidgetSinusSource(QWidget *parent = 0);
	~WidgetSinusSource();

	void LinkToModule(std::shared_ptr<SinusGeneratorClass> SinusGenerator);

void StartGenerateSignal(bool StartStop) { emit SignalStart(StartStop);}
signals:
void SignalStart(bool);
private:
	Ui::WidgetSinusSource ui;
};

#endif // WidgetSinusSource_H
