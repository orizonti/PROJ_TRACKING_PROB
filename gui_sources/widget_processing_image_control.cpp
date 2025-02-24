#include "widget_processing_image_control.h"
#include "ui_widget_processing_image_control.h"
#include <QSpinBox>
WidgetProcessingImageControl::WidgetProcessingImageControl(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetProcessingImageControl)
{
    ui->setupUi(this);
}

void WidgetProcessingImageControl::LinkToModule(std::shared_ptr<CVImageProcessing> ControlInterface)
{
  connect(ui->spinThreshold,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this, ControlInterface](int value)
  {
    ControlInterface->SetThreshold(value);
  });

  connect(ui->butProcessCentroid,&QPushButton::toggled,[this, ControlInterface](bool StartStop)
  {
    qDebug() << "CONTROL PROCESS CenTROID: "  << StartStop;
  });

  connect(ui->butProcessTemplate,&QPushButton::toggled,[this, ControlInterface](bool StartStop)
  {
    qDebug() << "CONTROL PROCESS TEMPLATE: "  << StartStop;
  });
}

