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

"QPushButton::pressed{"
"background-color: rgba(32, 156, 19, 83);"
"border: 2px solid line;"
"border-radius: 6px;"
"border-color: rgb(30, 221, 78);}"


};

QString stylePressedButtons
{

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
"border-color: rgb(215, 50, 28); }"

"QPushButton:checked { background-color: rgba(204, 53, 10, 62);"
"border: 2px solid line;"
"border-radius: 6px;	"
"border-color: rgb(30, 221, 78); }"

"QPushButton:pressed { background-color: rgba(204, 53, 10, 62);"
"border: 2px solid line;"
"border-radius: 6px;	"
"border-color: rgb(30, 221, 78); }"

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



WidgetDeviceControl::WidgetDeviceControl(QString name, Qt::Orientation orientation, QWidget* parent) : WidgetAdjustable(parent)
{
    this->setStyleSheet(styleBaseWidget);
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

    groupButtonsLevel->setLayout(layouts[1]); groupButtonsLevel->setStyleSheet(styleToggledButtons); 
    groupButtonsOnOff->setLayout(layouts[2]); groupButtonsOnOff->setStyleSheet(styleToggledButtons);
          groupArrows->setLayout(layouts[3]);       groupArrows->setStyleSheet(styleArrowButtons);
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
    this->layout()->addWidget(groupButtonsOnOff);
    this->layout()->addWidget(groupButtonsLevel);
    this->layout()->addWidget(groupArrows);

    this->layout()->setSpacing(2); this->layout()->setContentsMargins(2,2,2,2);

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

     buttonLeft->setMinimumWidth(minArrowsSize.width());  buttonLeft->setMaximumSize(maxArrowsSize);
    buttonRight->setMinimumWidth(minArrowsSize.width()); buttonRight->setMaximumSize(maxArrowsSize);
       buttonUp->setMinimumWidth(minArrowsSize.width());    buttonUp->setMaximumSize(maxArrowsSize);
     buttonDown->setMinimumWidth(minArrowsSize.width());  buttonDown->setMaximumSize(maxArrowsSize);

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

    this->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum));

    mainLayout->addSpacerItem(new QSpacerItem(10,20,QSizePolicy::Minimum, QSizePolicy::Fixed));
    }

    if(orientationWidget == Qt::Vertical)
    {
      maxButtonsSize = QSize(90,60);
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


    this->setMinimumSize(minLabelsSize.width()+1, minLabelsSize.height()*2);
    this->setMaximumSize(maxLabelsSize.width()+1, maxLabelsSize.height()*10);
    this->setFixedWidth(55);
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
      buttonsLevel.last()->setMinimumSize(minLevelsSize); buttonsLevel.last()->setMaximumSize(maxLevelsSize);
      buttonsLevel.last()->setAutoExclusive(true);
      buttonsLevel.last()->setCheckable(true); 
    }
      if( !buttonsLevel.isEmpty()) buttonsLevel.first()->setChecked(true);

    for(int n = 0; n < numberDevice; n++)
    {
      buttonsOnOff.push_back(new QPushButton(QString("DEV_%1").arg(n))); groupButtonsOnOff->layout()->addWidget(buttonsOnOff.last());
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
    groupArrows->layout()->addWidget(buttonLeft);
    groupArrows->layout()->addWidget(buttonRight);
    }
    this->schemeArrowsControl = schemeArrows;

}

void WidgetDeviceControl::setButtonsName(QVector<QString> names)
{
                                                 auto name = names.begin();
    for(auto button: buttonsOnOff) { button->setText(*name); name++; if(name == names.end()) break;  }
}

void WidgetDeviceControl::addSpace(int group)
{
  auto width = buttonsOnOff.first()->width();
  auto spacer = new QSpacerItem(100,20,QSizePolicy::Fixed, QSizePolicy::Fixed);
  ((QHBoxLayout*)groupButtonsOnOff->layout())->addSpacerItem(spacer);
}

void WidgetDeviceControl::setButtonsMode(QVector<bool> modes)
{
                                                      auto mode = modes.begin();
    for(auto button: buttonsOnOff) {    button->setCheckable(*mode); mode++; if(mode == modes.end()) break;  }
    for(auto button: buttonsOnOff) { if(button->isCheckable()) continue; button->setStyleSheet(stylePressedButtons);  }
}

void WidgetDeviceControl::setLevelsName(QVector<QString> names)
{
    auto name = names.begin();
    for(auto button: buttonsLevel)
    {
       button->setText(*name); name++; if(name == names.end()) break;  
    }
}
void WidgetDeviceControl::setLabelMode(int Mode)
{
    labelState->setText("0000.00"); 
    if(Mode == 2)
    labelState->setText("0000.00\n0000.00"); 
}

void WidgetDeviceControl::setName(QString name) { labelName->setText(name); }


void WidgetDeviceControl::linkToDevice(std::shared_ptr<DeviceGenericHandleControl> Device)
{
   if(Device == nullptr) { qDebug() << "[WIDGET CONTROL] LINK DEVICE FAIL [EMPTY DEVICE]"; return; }
   DeviceLinked = Device; 
               if(Device) linkSignals();
}

void WidgetDeviceControl::linkSignals()
{

    int level = 1;
    for(auto button: buttonsLevel)
    {
      connect(button, &QPushButton::toggled, [level, this](bool OnOff) 
           { if(OnOff) DeviceLinked->setLevel(level); qDebug() << "SET LEVEL: " << level << OnOff; } ); level++;
    }

    int device = 1;
    for(auto button: buttonsOnOff)
    {
      if(button->isCheckable()) 
      connect(button, &QPushButton::toggled, [device, this](bool OnOff) { DeviceLinked->setEnable(OnOff,device); } );
      else
      connect(button, &QPushButton::clicked, [device, this]() { DeviceLinked->setEnable(true,device); } ); 
                                              device++;

    }


      connect(spinParam, &QSpinBox::valueChanged, [this](int Value) { DeviceLinked->setValue(Value); } ); 
    //if(!groupArrows->isVisible()) return;

    State = DeviceLinked->getPair(); 

    if(ModeLabel == 2)
    labelState->setText(QString("%1\n%2").arg(State.first).arg(State.second));
    if(ModeLabel == 1)
    labelState->setText(QString("%1").arg(State.first));

    std::vector<QPair<float,float>> VelsVector;
    float VelocityScale = 0.50;
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

        QObject::connect(button, &QPushButton::released,
            [this]()      { DeviceLinked->setEnable(false);  timerCheckDevice.stop(); });
        }

        QObject::connect(&timerCheckDevice, &QTimer::timeout, [this]() mutable
        {
            NodeSynchronizer.synchronizePeers();
            State = DeviceLinked->getPair(); 
            labelState->setText(QString("%1\n%2").arg(State.first).arg(State.second));
        });
    }


    VelocityScale = 1;
    std::vector<float> VelsScalar;
    if(schemeArrowsControl == 2)
    {
        buttonsArrow.push_back(buttonLeft);  VelsScalar.push_back( VelocityScale );
        buttonsArrow.push_back(buttonRight); VelsScalar.push_back(-VelocityScale);

        for(int n = 0; n < 2; n++)
        {
          auto Velocity = VelsScalar[n];
          auto button = buttonsArrow[n];

          QObject::connect(button, &QPushButton::pressed,  [this, Velocity]() 
          {   
              DeviceLinked->setValue(Velocity); timerCheckDevice.start(1);
          });

          QObject::connect(button, &QPushButton::released, 
              [this]() { DeviceLinked->setEnable(false);  timerCheckDevice.stop(); });
          }
        }

                                            std::pair<float,float> Position;
        QObject::connect(&timerCheckDevice, &QTimer::timeout, [this, Position]() mutable
        {
            Position = DeviceLinked->getPair(); 
            labelState->setText(QString("%1\n%2").arg(Position.first).arg(Position.second));
        });

    if( buttonsOnOff.empty()) return;
    if(!buttonsOnOff[0]->isCheckable()) return;

    QObject::connect(&timerCheckDevice2, &QTimer::timeout, [this]() 
    {
                                          buttonsOnOff[0]->blockSignals(true);
         if(DeviceLinked->isTypeActive()) buttonsOnOff[0]->setChecked(DeviceLinked->isEnabled());
                                          buttonsOnOff[0]->blockSignals(false);
    });
                      timerCheckDevice2.start(1000);

}

WidgetDeviceControl::~WidgetDeviceControl() { }


void WidgetDeviceControl::setParamList(int Range, int CurrentParam, int StepNumber)
{
   spinParam->setRange(Range/StepNumber, Range);
   spinParam->setSingleStep(Range/StepNumber);
   spinParam->setValue(CurrentParam);
}
