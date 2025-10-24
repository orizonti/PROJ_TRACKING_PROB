#include "widget_container_group.h"
#include "ui_widget_container_group.h"
WidgetContainerGroup::WidgetContainerGroup(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetContainerGroup)
{
    ui->setupUi(this);
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(1));
}


void WidgetContainerGroup::slotSetActiveChannel(int Channel)
{
    if(Channel == NumberChannel) return;
    qDebug() << "[ WIDGET GROUP CHANNEL ]" << Channel;

    buttons[NumberChannel]->blockSignals(true);
    buttons[NumberChannel]->setChecked(false);
    buttons[NumberChannel]->blockSignals(false);

    buttons[Channel]->blockSignals(true);
    buttons[Channel]->setChecked(true);
    buttons[Channel]->blockSignals(false);

                  NumberChannel = Channel;

    ui->stackedWidget->setCurrentIndex(Channel+1);
             emit SignalChannelChanged(Channel);
}

void WidgetContainerGroup::AddWidget(WidgetAdjustable& Widget)
{
    ui->stackedWidget->addWidget(&Widget);
    if(!buttons.empty()) buttons.last()->setChecked(false);
    auto button = new QPushButton;
         button->setCheckable(true);
         button->setChecked(true);
         button->setAutoExclusive(true);

         button->setText(QString("%1").arg(NumberChannel+1));
         button->setMinimumSize(30,30);
         buttons.append(button);

    ui->horizontalGroupBox->layout()->addWidget(button);

    int Channel = ++NumberChannel; ui->stackedWidget->setCurrentIndex(Channel);
    QObject::connect(button, &QPushButton::toggled, [this, Channel](bool OnOff) { if(OnOff) slotSetActiveChannel(Channel);});
}

