
#include "widget_aiming_control.h"
#include <QPushButton>

WidgetAimingControl::WidgetAimingControl(QWidget *parent)
	: WidgetAdjustable(parent)
{
	ui.setupUi(this);
    
    QObject::connect(&timerDisplayState,SIGNAL(timeout()), this, SLOT(SlotDisplayState()));
    //this->setFixedHeight(250);
    this->setFixedWidth(290);

}


WidgetAimingControl::~WidgetAimingControl()
{
}
    

void WidgetAimingControl::SlotDisplayState()
{
    auto& BlockState      = Modules[0]->StateBlock;
    auto& BlockAimingType = Modules[0]->AimingState;
    auto& PIDParam        = Modules[0]->ModulePID.PIDParam;

	ui.checkWorkBlock->blockSignals(true);
	if (BlockState == StateBlockDisabled) ui.checkWorkBlock->setChecked(false);
	if (BlockState == StateBlockAtWork)  ui.checkWorkBlock->setChecked(true);

    //if(BlockAimingType == AimingFast)   ui.checkMoveFastPID->setChecked(true);
    //if(BlockAimingType == AimingSlow)   ui.checkMoveSlowPID->setChecked(true);
    //if(BlockAimingType == AimingTuning) ui.checkTuningMode->setChecked(true);
  
	ui.checkWorkBlock->blockSignals(false);
}


void WidgetAimingControl::LinkToModule(std::shared_ptr<AimingClass> Module)
{
    Modules.push_back(Module);
    qDebug() << "LINK CONTROL TO AIMING MODULE: " << Module->NumberChannel;

			connect(ui.checkWorkBlock, &QPushButton::toggled,
				[Module,this](bool checked)
			{
				if (checked)
				{
              		ui.checkWorkBlock->setText(QString::fromLocal8Bit("Работа"));
					Module->SetBlockEnabled(true);
				}
				else
				{
              		ui.checkWorkBlock->setText(QString::fromLocal8Bit("Отключено"));
					Module->SetBlockEnabled(false);
				}
			});

			QVector<QPushButton*> Buttons;
			Buttons.append(ui.butAimRegim1); 
            Buttons.append(ui.butAimRegim2); 
            Buttons.append(ui.butAimRegim3); 
            Buttons.append(ui.butAimRegim4); 
            Buttons.append(ui.butAimRegim5); 

        int TypeEnumAiming = 0;
        for (QPushButton* But : Buttons)
        {
            TypeEnumAiming++;
            connect(But, &QPushButton::toggled,
                [Module,this, TypeEnumAiming](bool checked)
            {
                if (checked)
                {
                    switch (TypeEnumAiming)
                    {
                    case 1: Module->SetAimingRegim(AimingLoop); break;
                    case 2: Module->SetAimingRegim(AimingLoop1); break;
                    case 3: Module->SetAimingRegim(AimingLoop2); break;
                    case 4: Module->SetAimingRegim(AimingLoop3); break;
                    case 5: Module->SetAimingRegim(AimingDirect); break;
                    }
                }
            });
        }
        //Module->GainList[0];
        ui.spinGain1->setSingleStep(0.01);
        ui.spinGain1->setValue(Module->GainList[0]);
        ui.spinGain2->setValue(Module->GainList[1]);
        if(Module->GainList[0] > 10) ui.spinGain1->setSingleStep(Module->GainList[0]/100);
        if(Module->GainList[1] > 10) ui.spinGain2->setSingleStep(Module->GainList[1]/100);

        connect(ui.butSetParam, &QPushButton::clicked,
            [Module,this]()
        {
            Module->SetGain(0,ui.spinGain1->value());
            Module->SetGain(1,ui.spinGain2->value());
            auto values = ui.linePosCorrection->text().split(":");
            Module->SetAimingCorrection(QPair<double,double>(values[0].toInt(), values[1].toInt()));
        });

        connect(ui.butReset, &QPushButton::clicked,
            [Module]()
        {
            Module->Reset();
        });


    timerDisplayState.start(300);
}
