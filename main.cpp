#include "interface_pass_coord.h"
#include "thread_operation_nodes.h"
#include "widget_imitator_control.h"

#define NOMINMAX

#include <QDebug>
#include <QApplication>

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
#include "engine_can_interface.h"
#include "TRACKER_ROBUST/Tracker.h"
//#include "GRAPHICS_WINDOW/widget_graphics_plot.h"
#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include <QWidget>

//#define CAMERA_WORK 
#define IMITATOR_WORK 

//#define PROJECT_PROB_ENV 1
#define PROJECT_EDGE_PROCESS 1 
//#define PROJECT_EDGE_IMITATOR 1 
//#define PROJECT_TEST 1 
//
//#define PROJECT_TEST_CAN 1 

#include "message_command_id.h"

int ID1 =  TypeRegister<CommandSetPosScanator    >::RegisterType();
int ID2 =  TypeRegister<CommandSetPosRotary      >::RegisterType();
int ID3 =  TypeRegister<CommandDeviceController  >::RegisterType();
int ID4 =  TypeRegister<CommandDeviceLaserPower  >::RegisterType();
int ID5 =  TypeRegister<CommandDeviceLaserPointer>::RegisterType();
int ID6 =  TypeRegister<CommandDeviceFocusator   >::RegisterType();
int ID7 =  TypeRegister<MessageDeviceController  >::RegisterType();
int ID8 =  TypeRegister<MessageDeviceLaserPower  >::RegisterType();
int ID9 =  TypeRegister<MessageDeviceLaserPointer>::RegisterType();
int ID10 = TypeRegister<CommandSetPosScanator    >::RegisterType();
int ID11 = TypeRegister<CommandSetPosScanator    >::RegisterType();
int ID12 = TypeRegister<MessagePositionState<0>  >::RegisterType();
int ID13 = TypeRegister<MessagePositionState<1>  >::RegisterType();
int ID14 = TypeRegister<CommandCheckConnection   >::RegisterType();
int ID15 = TypeRegister<CommandCloseConnection   >::RegisterType();
void printRegisteredTypes();


void OpenCL_Init();


int main(int argc, char* argv[])
{

  QApplication app(argc,argv);
  SettingsRegister::LoadSettings();
  qRegisterMetaType<const QImage&>();
  //OpenCL_Init();
  //
  

  #ifdef PROJECT_TEST
  WidgetGraphisPlot WindowGraphics;
                 WindowGraphics.show();
  SinusGeneratorClass SinusGenerator;
                      SinusGenerator.setLink(WindowGraphics.Graph1);
                      SinusGenerator.SlotStartGenerate(true);
  #endif



  #ifdef PROJECT_EDGE_PROCESS

  auto WindowImageProcessingControl = new WidgetProcessingImageControl; WindowImageProcessingControl->HideLabel();
  auto WindowImageProcessingDisplay = new WidgetProcessingImage("ОБРАБОТКА");
  WidgetTableGroup WindowTableGroup;
  WindowTableGroup.AddWidget(WindowImageProcessingDisplay);
  WindowTableGroup.AddWidget(WindowImageProcessingControl);
  WindowTableGroup.setFixedSize(400,500);
  WindowTableGroup.move(300,300);

  ProcessControllerClass*  ProcessController = ProcessControllerClass::GetInstance();
                           ProcessController->setParent(&WindowTableGroup);
                           ProcessController->SlotSetProcessAiming(true);

  ProcessControllerClass::DeviceCamera->SetZoom(3);
  ProcessControllerClass::DeviceCamera->StartCameraStream(true);

  WindowImageProcessingControl->LinkToModule(ProcessControllerClass::ModuleImageProc);
  //WindowImageProcessingDisplay->LinkToModule(ProcessControllerClass::ModuleImageProc);    
  WindowImageProcessingDisplay->LinkToModule(ProcessControllerClass::DeviceCamera);    

  auto AimingPort = &ProcessController->ModuleAiming1->PortSignalSetAiming; 
  QObject::connect(WindowImageProcessingDisplay, SIGNAL(signalPosPressed(QPair<float,float>)), AimingPort,SLOT(slotSetCoord(QPair<float,float>))) ;

  ProcessController->SlotStartProcessRTSP(true);
  WindowTableGroup.show();
  #endif

  
  #ifdef PROJECT_EDGE_IMITATION
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
                           ProcessController->SlotSetProcessImitation(true);

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
  QObject::connect(&WindowImageProcessingDisplay, SIGNAL(SignalPosPressed(QPair<float,float>)), AimingPort,SLOT(SlotSetCoord(QPair<float,float>))) ;

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

  std::string TAG_NAME = QString("[ %1 ] ").arg("INFO").toStdString();

  cv::ocl::Context ctx;
  auto result = ctx.create(cv::ocl::Device::TYPE_GPU);

   cv::ocl::Device device;
        if(result) device = ctx.device(0);

  cv::ocl::setUseOpenCL(true);
  cv::ocl::Context context = cv::ocl::Context::getDefault();

  qDebug() << "======================================";
  qDebug() << TAG_NAME.c_str() << "TEST OPENCL INIT";
  qDebug() << TAG_NAME.c_str() <<" CL DEVICE NUMBER" << ctx.ndevices();
  qDebug() << TAG_NAME.c_str() <<" CRETE GPU DEVICE:  " << result ;
  qDebug() << TAG_NAME.c_str() << "DEVICE       : " << device.vendorName().c_str();
  qDebug() << TAG_NAME.c_str() << "DEVICE DRIVER: " << device.driverVersion().c_str();
  qDebug() << TAG_NAME.c_str() << "OPENCL USED: " << cv::ocl::useOpenCL();
  qDebug() << TAG_NAME.c_str() << "HAS SVM: "     << cv::ocl::haveSVM();

  qDebug() << TAG_NAME.c_str() << "OPENCL AVAILABLE DEVICES";
  for (int i = 0; i < context.ndevices(); i++) 
  {
      cv::ocl::Device device = context.device(i);
      qDebug() << TAG_NAME.c_str() << "[ DEVICE ]" << i << ": " << device.name().c_str();
  }

  qDebug() << "======================================";
  qDebug() << Qt::endl;

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
  //
void printRegisteredTypes()
{
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandSetPos<0>>::TYPE_ID).arg(TypeRegister<CommandSetPosScanator>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandSetPos<1>>::TYPE_ID).arg(TypeRegister<CommandSetPosRotary>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandDevice<0>>::TYPE_ID).arg(TypeRegister<CommandDevice<0>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandDevice<1>>::TYPE_ID).arg(TypeRegister<CommandDevice<1>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandDevice<2>>::TYPE_ID).arg(TypeRegister<CommandDevice<2>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandDevice<3>>::TYPE_ID).arg(TypeRegister<CommandDevice<3>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<MessageDevice<0>>::TYPE_ID).arg(TypeRegister<MessageDevice<0>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<MessageDevice<1>>::TYPE_ID).arg(TypeRegister<MessageDevice<1>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<MessageDevice<2>>::TYPE_ID).arg(TypeRegister<MessageDevice<2>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<MessageDevice<3>>::TYPE_ID).arg(TypeRegister<MessageDevice<3>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandCalibration>::TYPE_ID).arg(TypeRegister<CommandCalibration>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<MessagePositionState<0>>::TYPE_ID).arg(TypeRegister<MessagePositionState<0>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<MessagePositionState<1>>::TYPE_ID).arg(TypeRegister<MessagePositionState<0>>::GetTypeSize());
  qDebug() << QString("REGISTER TYPE %1 SIZE %2").arg(TypeRegister<CommandCheckConnection>::TYPE_ID).arg(TypeRegister<CommandCheckConnection>::GetTypeSize());
}
//template<> void CommandDispatcherGeneric<TypeRegister<CommandDevice<0>>::ID()>::dispatchCommand(const QByteArray& Command) 
//{
//  qDebug() << "DISPATCH COMMAND: " << TypeRegister<CommandDevice<0>>::TYPE_ID << " DEV 0";
//};
//
//template<> void CommandDispatcherGeneric<TypeRegister<CommandSetPosRotary>::ID()>::dispatchCommand(const QByteArray& Command) 
//{
//  qDebug() << "DISPATCH COMMAND: " << TypeRegister<CommandSetPosRotary>::TYPE_ID << " POS ROTARY";
//};
