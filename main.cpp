#include "controller_process_class.h"
#include "widget_imitator_control.h"
#include <qthread.h>
#define NOMINMAX

#include <QDebug>
#include <QApplication>

//#include "TorchProcessing.h"

#include <QThread>
#include "widget_processing_image_generic.h"
#include "widget_main_window.h"
#include "register_settings.h"
#include "widget_imitator_control.h"
#include "widget_processing_image_control.h"
#include "widget_camera_control.h"
#include "widget_scanator_control.h"
#include "WindowLaserControl.h"
#include "AIM_IMAGE_IMITATION/widget_sinus_source.h"
#include "widget_process_controller.h"
#include "widget_aiming_control.h"
#include "widget_container_group.h"



int main(int argc, char* argv[])
{

  QApplication app(argc,argv);
  SettingsRegister::LoadSettings();
  qRegisterMetaType<const QImage&>();

  qDebug() << "CAMERA IMAGE SIZE : " << SettingsRegister::GetPair("CAMERA_IMAGE_SIZE");
  ProcessControllerClass*  ProcessController = ProcessControllerClass::GetInstance();

  WidgetProcessController  WindowProcessController;
                           WindowProcessController.LinkTo(ProcessController);

  WidgetImageImitator      WindowImageImitator; 
  WidgetSinusSource        WindowSinusSource;
  WidgetProcessingImageControl    WindowImageProcessingControl; 
  WidgetProcessingImageControl    WindowImageProcessingControl2; 

  WidgetContainerGroup WidgetGroup;
  WidgetGroup.AddWidget(WindowImageProcessingControl);
  WidgetGroup.AddWidget(WindowImageProcessingControl2);
  WidgetGroup.setMaximumHeight(150);

  WidgetCameraControl      WindowCameraControl; 
  WidgetScanatorControl    WindowScanatorInterface; 
  WidgetAimingControl      WindowAimingControl;

  WidgetProcessingImage WindowCameraDisplay("КАМЕРА");
  WidgetProcessingImage WindowImitatorDisplay("ИМИТАТОР");
  WidgetProcessingImage WindowImageProcessingDisplay("ОБРАБОТКА");

           WindowCameraControl.LinkToDevice(ProcessControllerClass::DeviceCamera);
           WindowCameraDisplay.LinkToModule(ProcessControllerClass::DeviceCamera);

           WindowImageImitator.LinkToModule(ProcessControllerClass::ModuleImitatorImage);
         WindowImitatorDisplay.LinkToModule(ProcessControllerClass::ModuleImitatorImage);

  WindowImageProcessingControl.LinkToModule(ProcessControllerClass::ModuleImageProc);
 WindowImageProcessingControl2.LinkToModule(ProcessControllerClass::ModuleImageProc2);

  WindowImageProcessingDisplay.LinkToModule(ProcessControllerClass::ModuleImageProc);
  //WindowImageProcessingDisplay.LinkToModule(ProcessControllerClass::ModuleImageProc2);

      WindowScanatorInterface.LinkToDevice(ProcessControllerClass::DeviceScanator);
          WindowAimingControl.LinkToModule(ProcessControllerClass::ModuleAimingLoop);

            WindowSinusSource.LinkToModule(ProcessControllerClass::ModuleSinusGenerator);

  auto AimingPort = &ProcessController->ModuleAimingLoop->PortSignalSetAiming; 
  QObject::connect(&WindowImageProcessingDisplay, SIGNAL(SignalPosPressed(QPair<double,double>)), AimingPort,SLOT(SlotSetCoord(QPair<double,double>))) ;

  //=================================================
  WidgetMainWindow MainWindow;
                  MainWindow.AddWidgetToDisplay(&WindowCameraDisplay);
                  MainWindow.AddWidgetToDisplay(&WindowImitatorDisplay);
                  MainWindow.AddWidgetToDisplay(&WindowImageProcessingDisplay);
                  
                  MainWindow.AddWidgetToDisplay(&WindowImageImitator);
                  MainWindow.AddWidgetToDisplay(&WidgetGroup);
                  MainWindow.AddWidgetToDisplay(&WindowCameraControl);
                  MainWindow.AddWidgetToDisplay(&WindowScanatorInterface);
                  MainWindow.AddWidgetToDisplay(&WindowSinusSource);
                  MainWindow.AddWidgetToDisplay(&WindowAimingControl);

                  //MainWindow.AddWidgetToDisplay(ProcessControllerClass::DeviceLaser->WindowControl);
                  //MainWindow.AddWidgetToDisplay(ProcessControllerClass::DeviceImitatorMotor->WindowControl);
                  MainWindow.AddWidgetToDisplay(&WindowProcessController);

                  MainWindow.AddWidgetToDisplay(WindowImageProcessingDisplay.LinkedWidget); 

                  MainWindow.LoadWidgetsLinks();

                  MainWindow.GetLastWidget()->HideNodes();
                  MainWindow.show();

  QObject::connect(&WidgetGroup,                  SIGNAL(SignalChannelChanged(int)),&WindowImageProcessingDisplay, SLOT(SlotSetActiveChannel(int)));
  QObject::connect(&WindowImageProcessingDisplay, SIGNAL(SignalChannelChanged(int)),&WidgetGroup,                  SLOT(SlotSetActiveChannel(int)));

  app.exec();
}

  //SubstractNode<double> Substract;
  //QPair<double,double> Coord(220,932.2);
  //QPair<double,double> Coord2(12,22.1);
  //QPair<double,double> CoordOut(12,22.1);
  // Coord >> Substract;
  //Coord2 >> Substract >> Substract;
  //             Coord2 >> Substract >> CoordOut;
  //qDebug() << "TEST COORD OUT: " << CoordOut;

  //=================================================
