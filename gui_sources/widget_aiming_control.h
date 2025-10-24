#ifndef AIMINGBLOCKWINDOW_H
#define AIMINGBLOCKWINDOW_H

#include "ui_widget_aiming_control.h"
#include "widget_adjustable.h"
#include "state_block_enum.h"
#include "aiming_class.h"
#include <QTimer>

class WidgetAimingControl : public WidgetAdjustable
{
	Q_OBJECT

public:
	WidgetAimingControl(QWidget *parent = 0);
	~WidgetAimingControl();

	void LinkToModule(std::shared_ptr<AimingClass> Module);

	std::vector<std::shared_ptr<AimingClass>> Modules;
	QTimer timerDisplayState;

private:
	Ui::WidgetAimingControl ui;

public slots:
    void slotDisplayState();
};

#endif // AIMINGBLOCKWINDOW_H
