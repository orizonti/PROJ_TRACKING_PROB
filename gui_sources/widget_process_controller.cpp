#include "widget_process_controller.h"
#include "ui_widget_process_controller.h"
WidgetProcessController::WidgetProcessController(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetProcessController)
{
    ui->setupUi(this);
}


void WidgetProcessController::LinkTo(ProcessControllerClass* ProcessController)
{
   connect(ui->butRegimAiming,      &QPushButton::toggled, ProcessController, &ProcessControllerClass::SlotSetProcessAiming);
   connect(ui->butRegimImitator,    &QPushButton::toggled, ProcessController, &ProcessControllerClass::SlotSetProcessImitation);
   connect(ui->butRegimTestSignal,  &QPushButton::toggled, ProcessController, &ProcessControllerClass::SlotSetProcessTestSignal);
   connect(ui->butRegimCalibration, &QPushButton::toggled, ProcessController, &ProcessControllerClass::SlotStartProcessRotFind);
}



