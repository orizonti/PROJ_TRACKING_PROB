#include "widget_imitator_control.h"
#include "ui_widget_imitator_control.h"
WidgetImageImitator::WidgetImageImitator(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetImageImitator)
{
    ui->setupUi(this);
}



void WidgetImageImitator::LinkToModule(std::shared_ptr<AimImageImitatorClass> Imitator)
{
   qDebug() << "LINK TO IMITATOR MODULE : ";
   ImitationModule = Imitator; 
   //Imitator->StartGenerate();
}


void WidgetImageImitator::on_butMoveRight_pressed(){if(!ImitationModule) return; ImitationModule->ManualControl.slotStartMoveAim(-1,0,2); }
void WidgetImageImitator::on_butMoveLeft_pressed() {if(!ImitationModule) return; ImitationModule->ManualControl.slotStartMoveAim( 1,0,2); }
void WidgetImageImitator::on_butMoveUp_pressed()   {if(!ImitationModule) return; ImitationModule->ManualControl.slotStartMoveAim(-1,1,2); }
void WidgetImageImitator::on_butMoveDown_pressed() {if(!ImitationModule) return; ImitationModule->ManualControl.slotStartMoveAim( 1,1,2); }

void WidgetImageImitator::on_butMoveRight_released(){if(!ImitationModule) return; ImitationModule->ManualControl.StopMove(); }
void WidgetImageImitator::on_butMoveLeft_released() {if(!ImitationModule) return; ImitationModule->ManualControl.StopMove(); }
void WidgetImageImitator::on_butMoveUp_released()   {if(!ImitationModule) return; ImitationModule->ManualControl.StopMove(); }
void WidgetImageImitator::on_butMoveDown_released() {if(!ImitationModule) return; ImitationModule->ManualControl.StopMove(); }

void WidgetImageImitator::on_butStartAimMoving_clicked() {if(!ImitationModule) return; ImitationModule->DynamicControl.StartMove(); }
void WidgetImageImitator::on_butStopAimMoving_clicked()  {if(!ImitationModule) return; ImitationModule->DynamicControl.StopMove(); }
