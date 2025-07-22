#include "widget_processing_image_control.h"
#include "ui_widget_processing_image_control.h"
#include <QSpinBox>
#include <qnamespace.h>
WidgetProcessingImageControl::WidgetProcessingImageControl(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetProcessingImageControl)
{
    ui->setupUi(this);
}

void WidgetProcessingImageControl::HideLabel() 
{
    ui->label->hide();
}

void WidgetProcessingImageControl::LinkToModule(std::shared_ptr<ModuleImageProcessing> ControlInterface)
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

  connect(ui->butResetProcessing,&QPushButton::clicked,[this, ControlInterface](bool StartStop)
  {
    emit SignalResetProcessing();
  });
  connect(this, SIGNAL(SignalResetProcessing()), ControlInterface.get(), SLOT(SlotResetProcessing()), Qt::QueuedConnection);

}

