#include "controller_process_class.h"
#include "interface_pass_coord.h"
#include "thread_operation_nodes.h"
#include "widget_imitator_control.h"

#define NOMINMAX

#include <QDebug>
#include <QApplication>

//#include "TorchProcessing.h"
#include "controller_process_class.h"

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
#include "widget_table_group.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include "TRACKER_ROBUST/Tracker.h"
#include "GRAPHICS_WINDOW/widget_graphics_plot.h"
#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"

//#define CAMERA_WORK 
#define IMITATOR_WORK 

#define PROJECT_PROB_ENV 1
//#define PROJECT_EDGE_PROCESS 1 
//#define PROJECT_TEST 1 

void OpenCL_Init();

int main(int argc, char* argv[])
{

  QApplication app(argc,argv);
  SettingsRegister::LoadSettings();
  qRegisterMetaType<const QImage&>();

  #ifdef PROJECT_TEST
  WidgetGraphisPlot WindowGraphics;
                 WindowGraphics.show();
  SinusGeneratorClass SinusGenerator;
                      SinusGenerator.SetLink(WindowGraphics.Graph1);
                      SinusGenerator.SlotStartGenerate(true);
  #endif

  #ifdef PROJECT_EDGE_PROCESS
  OpenCL_Init();
  
  auto WindowImageProcessingControl = new WidgetProcessingImageControl; WindowImageProcessingControl->HideLabel();
  auto WindowImageProcessingDisplay = new WidgetProcessingImage("ОБРАБОТКА");

  WidgetTableGroup WindowTableGroup;
  WindowTableGroup.AddWidget(WindowImageProcessingDisplay);
  WindowTableGroup.AddWidget(WindowImageProcessingControl);
  WindowTableGroup.move(300,300);

  ProcessControllerClass*  ProcessController = ProcessControllerClass::GetInstance();
                           ProcessController->setParent(&WindowTableGroup);
                           ProcessControllerClass::ModuleImitatorImage->SlotStartWork();

   WindowImageProcessingControl->LinkToModule(ProcessControllerClass::ModuleImageProc);
   WindowImageProcessingDisplay->LinkToModule(ProcessControllerClass::ModuleImageProc);    

  WindowTableGroup.show();
  #endif


  #ifdef PROJECT_PROB_ENV
  ProcessControllerClass*  ProcessController = ProcessControllerClass::GetInstance();

  WidgetProcessController  WindowProcessController;
                           WindowProcessController.LinkTo(ProcessController);


  WidgetProcessingImageControl    WindowImageProcessingControl; 
  WidgetProcessingImageControl    WindowImageProcessingControl2; 

  WidgetContainerGroup WidgetGroup;
  WidgetGroup.AddWidget(WindowImageProcessingControl);
  WidgetGroup.AddWidget(WindowImageProcessingControl2);
  WidgetGroup.setMaximumHeight(250);

  WidgetCameraControl      WindowCameraControl; 
  WidgetScanatorControl    WindowScanatorInterface; 
  WidgetAimingControl      WindowAimingControl;

  WidgetProcessingImage WindowCameraDisplay("КАМЕРА");
  WidgetProcessingImage WindowImageProcessingDisplay("ОБРАБОТКА");

           WindowCameraControl.LinkToDevice(ProcessControllerClass::DeviceCamera);
           WindowCameraDisplay.LinkToModule(ProcessControllerClass::DeviceCamera);


  WindowImageProcessingControl.LinkToModule(ProcessControllerClass::ModuleImageProc);
  WindowImageProcessingControl2.LinkToModule(ProcessControllerClass::ModuleImageProc2);

  WindowImageProcessingDisplay.LinkToModule(ProcessControllerClass::ModuleImageProc);
  WindowImageProcessingDisplay.LinkToModule(ProcessControllerClass::ModuleImageProc2);

      WindowScanatorInterface.LinkToDevice(ProcessControllerClass::DeviceScanator);
          WindowAimingControl.LinkToModule(ProcessControllerClass::ModuleAiming1);
          WindowAimingControl.LinkToModule(ProcessControllerClass::ModuleAiming2);


  auto AimingPort = &ProcessController->ModuleAiming1->PortSignalSetAiming; 
  QObject::connect(&WindowImageProcessingDisplay, SIGNAL(SignalPosPressed(QPair<double,double>)), AimingPort,SLOT(SlotSetCoord(QPair<double,double>))) ;

  WidgetGraphisPlot WindowGraphics;

  //=================================================
  WidgetMainWindow MainWindow;
                   MainWindow.AddWidgetToDisplay(&WindowCameraDisplay);
                   MainWindow.AddWidgetToDisplay(&WindowImageProcessingDisplay);
                    
                   MainWindow.AddWidgetToDisplay(&WidgetGroup);
                   MainWindow.AddWidgetToDisplay(&WindowCameraControl);
                   MainWindow.AddWidgetToDisplay(&WindowScanatorInterface);
                   MainWindow.AddWidgetToDisplay(&WindowAimingControl);

                   MainWindow.AddWidgetToDisplay(&WindowProcessController);
                   MainWindow.AddWidgetToDisplay(WindowImageProcessingDisplay.LinkedWidget); 
                   MainWindow.AddWidgetToDisplay(&WindowGraphics);

                   MainWindow.LoadWidgetsLinks();
//                   MainWindow.GetLastWidget()->HideNodes();
                   MainWindow.show();
                   ProcessController->setParent(&MainWindow);

  QObject::connect(&WidgetGroup,                  SIGNAL(SignalChannelChanged(int)),&WindowImageProcessingDisplay, SLOT(SlotSetActiveChannel(int)));
  QObject::connect(&WindowImageProcessingDisplay, SIGNAL(SignalChannelChanged(int)),&WidgetGroup,                  SLOT(SlotSetActiveChannel(int)));

  ProcessControllerClass::ModuleImitatorImage->SlotStartWork();
  #endif 


  app.exec();
}


void OpenCL_Init()
{
  #ifdef PROJECT_EDGE_PROCESS
  qDebug() << "======================================";
  qDebug() << "TEST OPENCL INIT";
  cv::ocl::Context ctx;
  qDebug() <<" CL DEVICE NUMBER" << ctx.ndevices();

  auto result = ctx.create(cv::ocl::Device::TYPE_GPU);
  qDebug() <<" CRETE GPU DEVICE:  " << result ;

   cv::ocl::Device device;
        if(result) device = ctx.device(0);

  qDebug() << "DEVICE       : " << device.vendorName().c_str();
  qDebug() << "DEVICE DRIVER: " << device.driverVersion().c_str();
  qDebug() << "TEST OPENCL INIT";

  cv::ocl::setUseOpenCL(true);
  qDebug() << "OPENCL USED: " << cv::ocl::useOpenCL();

  cv::ocl::Context context = cv::ocl::Context::getDefault();

  qDebug() << "OPENCL AVAILABLE DEVICES";
  for (int i = 0; i < context.ndevices(); i++) 
  {

      cv::ocl::Device device = context.device(i);
      qDebug() << "[ DEVICE ]" << i << ": " << device.name().c_str();
  }

  qDebug() << "HAS SVM: " << cv::ocl::haveSVM();
  qDebug() << "======================================";
  #endif
}

  //
  //
  //QHBoxLayout lay_base;
  //QVBoxLayout lay;   lay.addWidget(&WindowCameraDisplay);
  //                   lay.addWidget(&WindowCameraControl);
  //QVBoxLayout lay2;  
  //                   lay2.addWidget(&WindowImageProcessingDisplay);
  //                   lay2.addWidget(&WindowImageProcessingControl);
  //            lay_base.addLayout(&lay);
  //            lay_base.addLayout(&lay2);

  //QGroupBox box; box.setLayout(&lay_base);
  //          box.setStyleSheet(WindowCameraDisplay.styleSheet());
  //box.show();
