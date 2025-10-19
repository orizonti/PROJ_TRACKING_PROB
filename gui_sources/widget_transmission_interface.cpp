#include "WidgetTransmissionControl.h"
#include "ui_WidgetTransmissionControl.h"
#include <qnamespace.h>
WidgetTransmissionControl::WidgetTransmissionControl(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetTransmissionControl)
{
    ui->setupUi(this);
}

