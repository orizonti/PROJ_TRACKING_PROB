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
    setParamRange(1000,100);
}

void WidgetProcessingImageControl::setParamRange(int Range, int Steps)
{
  ui->spinParam1->setRange(Range/Steps, Range);
  ui->spinParam1->setSingleStep(Range/Steps);
}

void WidgetProcessingImageControl::linkToModule(CameraControlInterface* ControlInterface)
{
  connect(ui->spinParam1,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this, ControlInterface](int value)
  {
    ControlInterface->CameraSetExposure(value);
  });
}

void WidgetProcessingImageControl::linkToModule(std::shared_ptr<ModuleImageProcessing> ControlInterface)
{
  connect(ui->spinParam1,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this, ControlInterface](int value)
  {
    ControlInterface->SetThreshold(value);
  });

  connect(ui->butResetProcessing,&QPushButton::clicked,[this, ControlInterface](bool StartStop)
  {
    ControlInterface->SlotResetProcessing();
  });

  connect(ui->pushButton,&QPushButton::toggled,[this, ControlInterface](bool StartStop)
  {
    if(StartStop)
    ControlInterface->SetStateActive();
    else
    ControlInterface->SetStateIdle();
  });
  
  connect(this, SIGNAL(SignalResetProcessing()), ControlInterface.get(), SLOT(SlotResetProcessing()), Qt::QueuedConnection);

}

