#include <QSpinBox>
#include "widget_device_control.h"
#include <QTimer>
#include <QSpacerItem>

QString styleToggledButtons{
"QPushButton{"
"background-color: rgba(204, 53, 10, 62);"
"border: 2px solid line;"
"border-radius: 6px;"	
"border-color: rgb(215, 50, 28); }"

"QPushButton::checked{"
"background-color: rgba(32, 156, 19, 83);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(30, 221, 78);}"

};

QString styleArrowButtons{

"QPushButton {"
"background-color: rgba(209, 142, 34, 60);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(214, 136, 41); }"

"QPushButton:pressed {"
"background-color: rgba(209, 142, 34, 110);"
"border: 4px solid line;"
"border-radius: 6px;"
"border-color: rgb(171, 86, 38); }"
};


QString styleBaseWidget
{
"QWidget { background-color: rgb(43, 43, 43); }"

"QPushButton { background-color: rgba(32, 156, 19, 83);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(30, 221, 78); }"

"QPushButton:checked { background-color: rgba(204, 53, 10, 62);"
"border: 2px solid line;"
"border-radius: 6px;	"
"border-color: rgb(215, 50, 28); }"

"QLineEdit { background-color: rgba(32, 156, 19, 83);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(30, 221, 78); }"

"QLabel { background-color: rgba(209, 142, 34, 60);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(214, 136, 41); }"

"QGroupBox { border: 0px solid line; border-color: green; border-radius: 6px; }"

"QLabel { background-color: rgba(209, 142, 34, 60);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(214, 136, 41); }"


"QSpinBox { background-color: rgba(209, 142, 34, 60);"
"   border-radius: 6px;"
"   border: 2px solid line;"
"   border-color: rgb(214, 136, 41);"
"   padding-right: 0px; }"

"QSpinBox::up-button { subcontrol-origin: border;"
"    subcontrol-position: top right; "
"    width: 28px; "
"    border-width: 1px; }"


"QSpinBox::down-button {"
"subcontrol-origin: border;"
"subcontrol-position: bottom right; "
"width: 28px; "
"border-width: 1px;}"

"QSpinBox::up-button:hover { }"
"QSpinBox::up-button:pressed { }"

"QSpinBox::down-arrow { image: url(D:/PICTURES/ImagesToPrograms/arrow_down.png);"
"width: 20px;"
"height: 40px; }"

"QSpinBox::up-arrow { image: url(D:/PICTURES/ImagesToPrograms/arrow_up.png);"
"width: 20px;"
"height: 40px; }"

"QSpinBox::up-arrow:disabled, QSpinBox::up-arrow:off { image: url(:/images/up_arrow_disabled.png); }"
"QSpinBox::down-button:hover { border-image: url(:/images/spindown_hover.png) 1; }"
"QSpinBox::down-button:pressed { border-image: url(:/images/spindown_pressed.png) 1; }"
};

QString styleBaseWidget2
{
"QWidget { background-color: rgb(46, 65, 83);"
"  border: 0px solid #455364;"
"  padding: 0px;"
"  color: #E0E1E3;"
"  selection-background-color: #346792;"
"  selection-color: #E0E1E3; }"

"QWidget:disabled { background-color: #19232D;"
"  color: #9DA9B5;"
"  selection-background-color: #26486B;"
"  selection-color: #9DA9B5; }"

"QWidget::item:selected { background-color: #346792; }"
"QWidget::item:hover:!selected { background-color: #1A72BB; }"


"QLabel { border: 2px solid line black;"
"background-color: rgb(99, 69, 44);"
"color: rgb(210, 204, 182);"
"font: 600 12pt Ubuntu;"
"border-radius: 4px; }"


"QSpinBox { border: 2px solid line black;"
"font: 600 14pt Ubuntu Sans;"
"background-color: rgba(152, 106, 68,120);"
"border-radius: 4px; }"

"QSpinBox::up-button { subcontrol-origin: border;"
"subcontrol-position: top right; "
"width: 16px; "
"border-radius: 4px;"
"border-width: 1px;"
"border: 1px solid line black;"
"background-color: rgba(181, 131, 90,200); }"

"QSpinBox::down-button { subcontrol-origin: border;"
"subcontrol-position: bottom right; "
"width: 16px;"
"border-image: url(:/images/spindown.png) 1;"
"border-width: 1px;"
"border-top-width: 0;"
"border: 1px solid line black;"
"border-radius: 4px;"
"background-color: rgba(181, 131, 90,200); }"
};

QString styleToggledButtons2{
"QPushButton { background-color: rgba(220, 59, 23,110);"
"  color: #E0E1E3;"
"  border-radius: 4px;"
"  padding: 2px;"
"  outline: none;"
"  border: none;"
"	font: 14pt Ubuntu; }"

"QPushButton:disabled { background-color: #455364;"
"  color: #9DA9B5;"
"  border-radius: 4px;"
"  padding: 2px; }"

"QPushButton:checked { background-color: rgba(35, 102, 12,160);"
"  border-radius: 4px;"
"  padding: 2px;"
"  outline: none;"
"font: 14pt Ubuntu; }"

"QPushButton:checked:disabled { background-color: #60798B;"
"  color: #9DA9B5;"
"  border-radius: 4px;"
"  padding: 2px;"
"  outline: none; }"
"QPushButton:checked:selected { background: #60798B; }"
"QPushButton:hover { background-color: #54687A; color: #E0E1E3; }"
"QPushButton:pressed { background-color: #60798B; }"
"QPushButton:selected { background: #60798B; color: #E0E1E3;}"

"QPushButton::menu-indicator { subcontrol-origin: padding;"
"  subcontrol-position: bottom right;"
"  bottom: 4px;"
"	background-color: rgb(210, 204, 182); }"
};


WidgetDeviceControl::WidgetDeviceControl(QString name, Qt::Orientation orientation, QWidget* parent) : WidgetAdjustable(parent)
{
    this->setStyleSheet(styleBaseWidget2);
    orientationWidget = orientation;

    QVector<QBoxLayout*> layouts;
    if(orientation == Qt::Horizontal) for(int n = 0; n < 5; n++) layouts.push_back(new QHBoxLayout());
    if(orientation == Qt::Vertical)   for(int n = 0; n < 5; n++) layouts.push_back(new QVBoxLayout());
                                                    mainLayout = layouts[0];

    labelName  = new QLabel(name); 
    labelState = new QLabel("0000.00\n0000.00"); 
    spinParam = new QSpinBox();

    groupButtonsLevel = new QGroupBox;
    groupButtonsOnOff = new QGroupBox;
          groupArrows = new QGroupBox;

    groupButtonsLevel->setLayout(layouts[1]); groupButtonsLevel->setStyleSheet(styleToggledButtons2); 
    groupButtonsOnOff->setLayout(layouts[2]); groupButtonsOnOff->setStyleSheet(styleToggledButtons2);
          groupArrows->setLayout(layouts[3]);       groupArrows->setStyleSheet(styleToggledButtons2);
    groupButtonsLevel->layout()->setSpacing(2); groupButtonsLevel->layout()->setContentsMargins(1,1,1,1);
    groupButtonsOnOff->layout()->setSpacing(2); groupButtonsOnOff->layout()->setContentsMargins(1,1,1,1);
          groupArrows->layout()->setSpacing(2);       groupArrows->layout()->setContentsMargins(1,1,1,1);

    buttonLeft  = new QPushButton(QIcon(":/buttonImages/arrow_left_hollow.png"),"");
    buttonRight = new QPushButton(QIcon(":/buttonImages/arrow_right_hollow.png"),"");
    buttonUp    = new QPushButton(QIcon(":/buttonImages/arrow_up_hollow.png"),"");
    buttonDown  = new QPushButton(QIcon(":/buttonImages/arrow_down_hollow.png"),"");

    this->setLayout(mainLayout);
    this->layout()->addWidget(labelName);
    this->layout()->addWidget(labelState);
    this->layout()->addWidget(spinParam);
    this->layout()->addWidget(groupButtonsLevel);
    this->layout()->addWidget(groupButtonsOnOff);
    this->layout()->addWidget(groupArrows);

    this->layout()->setSpacing(2); this->layout()->setContentsMargins(1,1,1,1);

    this->setSizes();

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

void WidgetDeviceControl::setSizes()
{
    if(orientationWidget == Qt::Horizontal)

    {
                                                          this->setMinimumWidth(2*minLabelsSize.width());
     this->labelName->setMaximumSize(maxLabelsSize); this->labelName->setMinimumWidth(minLabelsSize.width());
    this->labelState->setMaximumSize(maxLabelsSize); this->labelState->setMinimumWidth(minLabelsSize.width());
     this->spinParam->setMaximumSize(maxLabelsSize); this->spinParam->setMinimumWidth(minLabelsSize.width());

     buttonLeft->setMinimumWidth(minButtonsSize.width());  buttonLeft->setMaximumSize(maxButtonsSize);
    buttonRight->setMinimumWidth(minButtonsSize.width()); buttonRight->setMaximumSize(maxButtonsSize);
       buttonUp->setMinimumWidth(minButtonsSize.width());    buttonUp->setMaximumSize(maxButtonsSize);
     buttonDown->setMinimumWidth(minButtonsSize.width());  buttonDown->setMaximumSize(maxButtonsSize);

    groupButtonsLevel->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    groupButtonsOnOff->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
          groupArrows->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
            labelName->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
           labelState->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
            spinParam->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
           buttonLeft->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
          buttonRight->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
             buttonUp->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
           buttonDown->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    this->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

    //mainLayout->addSpacerItem(new QSpacerItem(10,20,QSizePolicy::Minimum, QSizePolicy::Fixed));
    }

    if(orientationWidget == Qt::Vertical)
    {
      maxButtonsSize = QSize(60,60);
      minButtonsSize = QSize(30,30);
     this->labelName->setMaximumSize(maxButtonsSize);
    this->labelState->setMaximumSize(maxButtonsSize); this->labelState->setMinimumHeight(minLabelsSize.height());
     this->spinParam->setMaximumSize(maxButtonsSize);  this->spinParam->setMinimumHeight(minLabelsSize.height());

     buttonLeft->setMinimumHeight(minButtonsSize.height());  buttonLeft->setMaximumSize(maxButtonsSize);
    buttonRight->setMinimumHeight(minButtonsSize.height()); buttonRight->setMaximumSize(maxButtonsSize);
       buttonUp->setMinimumHeight(minButtonsSize.height());    buttonUp->setMaximumSize(maxButtonsSize);
     buttonDown->setMinimumHeight(minButtonsSize.height());  buttonDown->setMaximumSize(maxButtonsSize);

    groupButtonsLevel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    groupButtonsOnOff->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
          groupArrows->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
            labelName->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
           labelState->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
            spinParam->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
           buttonLeft->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
          buttonRight->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
             buttonUp->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
           buttonDown->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

           groupButtonsLevel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));


    this->setMinimumSize(minLabelsSize.width()+1, 30);
    this->setMaximumSize(maxLabelsSize.width()+1, 52);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum));
    }
}


void WidgetDeviceControl::enableScheme(bool enableState, 
                                       bool enableParam, 
                                       bool enableLevels, 
                                       bool enableOnOff, 
                                       bool enableArrows,
                                       bool enableLabel)
{
    if(!enableLabel)  labelName->hide();
    if(!enableState)  labelState->hide();
    if(!enableParam)  spinParam->hide();
    if(!enableLevels) groupButtonsLevel->hide();
    if(!enableOnOff)  groupButtonsOnOff->hide();
    if(!enableArrows) groupArrows->hide();

}
void WidgetDeviceControl::setScheme(int numberLevels, int numberDevice, int schemeArrows) {

    for(int n = 0; n < numberLevels; n++)
    {
      buttonsLevel.push_back(new QPushButton(QString("X%1").arg(n))); groupButtonsLevel->layout()->addWidget(buttonsLevel.last());
      buttonsLevel.last()->setMinimumSize(minButtonsSize); buttonsLevel.last()->setMaximumSize(maxButtonsSize);
      buttonsLevel.last()->setAutoExclusive(true);
      buttonsLevel.last()->setCheckable(true); 
    }
      if( !buttonsLevel.isEmpty()) buttonsLevel.first()->setChecked(true);

    for(int n = 0; n < numberDevice; n++)
    {
      buttonsOnOff.append(new QPushButton(QString("ПУСК %1").arg(n))); groupButtonsOnOff->layout()->addWidget(buttonsOnOff.last());
      buttonsOnOff.last()->setMinimumSize(minButtonsSize); buttonsOnOff.last()->setMaximumSize(maxButtonsSize);
      buttonsOnOff.last()->setCheckable(true); 
    }

    if(schemeArrows == 4)
    {
    groupArrows->layout()->addWidget(buttonLeft);
    groupArrows->layout()->addWidget(buttonRight);
    groupArrows->layout()->addWidget(buttonUp);
    groupArrows->layout()->addWidget(buttonDown);
    }


    if(schemeArrows == 2)
    {
    buttonLeft  = new QPushButton;
    buttonRight = new QPushButton;

    groupArrows->layout()->addWidget(buttonLeft);
    groupArrows->layout()->addWidget(buttonRight);
    }
    this->schemeArrowsControl = schemeArrows;

}

void WidgetDeviceControl::setButtonsName(QVector<QString> names)
{

    auto name = names.begin();
    for(auto button: buttonsOnOff)
    {
       button->setText(*name); name++; if(name == names.end()) break;  
    }

}
void WidgetDeviceControl::setLevelsName(QVector<QString> names)
{
    auto name = names.begin();
    for(auto button: buttonsLevel)
    {
       button->setText(*name); name++; if(name == names.end()) break;  
    }
}

void WidgetDeviceControl::setName(QString name) { labelName->setText(name); }

void WidgetDeviceControl::linkToDevice(std::shared_ptr<DeviceGenericHandleControl> Device)
{
   DeviceLinked = Device; 
               if(Device) linkSignals();
}

void WidgetDeviceControl::linkSignals()
{
    int level = 1;
    for(auto button: buttonsLevel)
    {
      connect(button, &QPushButton::toggled, [level, this](bool OnOff) { if(OnOff) DeviceLinked->setLevel(level); } ); level++;
    }

    int device = 0;
    for(auto button: buttonsOnOff)
    {
      connect(button, &QPushButton::toggled, [device, this](bool OnOff) { DeviceLinked->setEnable(OnOff,device); } ); device++;
    }

      connect(spinParam, &QSpinBox::valueChanged, [this](int Value) { DeviceLinked->setValue(Value); } ); 


    //if(!groupArrows->isVisible()) return;

    std::vector<QPair<float,float>> VelsVector;
    float VelocityScale = 0.5;
    if(schemeArrowsControl == 4)
    {
        buttonsArrow.push_back(buttonLeft);  VelsVector.push_back(QPair<float,float>(-VelocityScale, 0));
        buttonsArrow.push_back(buttonRight); VelsVector.push_back(QPair<float,float>( VelocityScale, 0));
        buttonsArrow.push_back(buttonUp);    VelsVector.push_back(QPair<float,float>( 0, VelocityScale));
        buttonsArrow.push_back(buttonDown);  VelsVector.push_back(QPair<float,float>( 0,-VelocityScale));

        for(int n = 0; n < 4; n++)
        {
        auto Velocity = VelsVector[n];
        auto button = buttonsArrow[n];
        QObject::connect(button, &QPushButton::pressed,  [this, Velocity]() 
        {   
            qDebug() << "MOVE VELOCITY: " << Velocity; 
            DeviceLinked->setPair(Velocity); timerCheckDevice.start(1);
        });
        QObject::connect(button, &QPushButton::released, [this     ]()      { DeviceLinked->setEnable(false);  timerCheckDevice.stop(); });
        }

                                            std::pair<float,float> Position;
        QObject::connect(&timerCheckDevice, &QTimer::timeout, [this, Position]() mutable
        {
            Position = DeviceLinked->getPair(); 
            labelState->setText(QString("%1 %2").arg(Position.first).arg(Position.second));
        });
    }


    std::vector<float> VelsScalar;
    if(schemeArrowsControl == 2)
    {
        buttonsArrow.push_back(buttonLeft);  VelsScalar.push_back(VelocityScale );
        buttonsArrow.push_back(buttonRight); VelsScalar.push_back(-VelocityScale);

        auto velocityValue = VelsScalar.begin();
        for(auto but: buttonsArrow)
        {
        QObject::connect(but, &QPushButton::pressed,  [this, velocityValue]() 
        {   
            DeviceLinked->setValue(*velocityValue); timerCheckDevice.start(1);
        });
        QObject::connect(but, &QPushButton::released, [this     ]()      { DeviceLinked->setEnable(false);  timerCheckDevice.stop(); });
        }

    }

                                            std::pair<float,float> Position;
        QObject::connect(&timerCheckDevice, &QTimer::timeout, [this, Position]() mutable
        {
            Position = DeviceLinked->getPair(); 
            labelState->setText(QString("%1 %2").arg(Position.first).arg(Position.second));
        });

}

WidgetDeviceControl::~WidgetDeviceControl() { }


void WidgetDeviceControl::setParamList(int Range, int CurrentParam, int StepNumber)
{
   spinParam->setRange(Range/StepNumber, Range);
   spinParam->setSingleStep(Range/StepNumber);
   spinParam->setValue(CurrentParam);
}
