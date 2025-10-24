#include "widget_process_controller.h"
#include "ui_widget_process_controller.h"
WidgetProcessController::WidgetProcessController(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetProcessController)
{
    ui->setupUi(this);
}


void WidgetProcessController::LinkTo(ProcessControllerClass* ProcessController)
{
   connect(ui->butRegimAiming,      &QPushButton::toggled, ProcessController, &ProcessControllerClass::slotSetProcessAiming);
   connect(ui->butRegimImitator,    &QPushButton::toggled, ProcessController, &ProcessControllerClass::slotSetProcessImitation);
   //connect(ui->butRegimTestSignal,  &QPushButton::toggled, ProcessController, &ProcessControllerClass::slotSetProcessTestSignal);
   //connect(ui->butRegimCalibration, &QPushButton::toggled, ProcessController, &ProcessControllerClass::slotStartProcessRotFind);
}



