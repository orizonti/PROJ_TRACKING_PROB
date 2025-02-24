#include "widget_container_group.h"
#include "ui_widget_container_group.h"
WidgetContainerGroup::WidgetContainerGroup(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetContainerGroup)
{
    ui->setupUi(this);
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(1));
}



void WidgetContainerGroup::SlotSetChannel(int Channel)
{
    qDebug() << "WIDGET CONTAINER SET CHANNEL: " << Channel;
    ui->stackedWidget->setCurrentIndex(Channel);
    emit SignalChannelChanged(Channel);
}

void WidgetContainerGroup::AddWidget(WidgetAdjustable& Widget)
{
    ui->stackedWidget->addWidget(&Widget);
    auto button = new QPushButton;
    ui->horizontalGroupBox->layout()->addWidget(button);
    button->setCheckable(true);
    button->setCheckable(true);
    button->setAutoExclusive(true);

    //button->setStyleSheet(ui->pushButton1->styleSheet());
    button->setText(QString("%1").arg(NumberChannels+1));
    button->setMinimumSize(30,30);
    button->setChecked(true);
    qDebug() << "STACKED COUNT: " << ui->stackedWidget->count()-1;

    NumberChannels++; int Channel = NumberChannels; ui->stackedWidget->setCurrentIndex(Channel);
    QObject::connect(button, &QPushButton::toggled, [this, Channel]() {SlotSetChannel(Channel);});
}

