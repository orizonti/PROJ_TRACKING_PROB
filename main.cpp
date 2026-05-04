#include "interface_pass_coord.h"
#include "thread_operation_nodes.h"

#define NOMINMAX

#include <QDebug>
#include <QApplication>

#include "controller_process_class.h"

#include <QThread>
#include "widget_processing_image_generic.h"
#include "register_settings.h"
#include "widget_processing_image_control.h"
#include "widget_container_group.h"
#include "widget_table_group.h"
#include "widget_device_control.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QWidget>
#include "arduino_json.h"

//#define PROJECT_COMPLEX 1
#define PROJECT_SIMPLE 1 


void OpenCL_Init();



template<> constinit const int TypeRegister<CommandSetPosRotary>      ::TYPE_ID{0 };
template<> constinit const int TypeRegister<CommandSetPosScanator>    ::TYPE_ID{1 };
template<> constinit const int TypeRegister<RequestPosRotary  >       ::TYPE_ID{2 };
template<> constinit const int TypeRegister<RequestPosScanator>       ::TYPE_ID{3 };
template<> constinit const int TypeRegister<CommandAiming1>           ::TYPE_ID{4 };
template<> constinit const int TypeRegister<CommandAiming2>           ::TYPE_ID{5 };
template<> constinit const int TypeRegister<RequestAiming >           ::TYPE_ID{6 };

template<> constinit const int TypeRegister<CommandDeviceLaserPointer>::TYPE_ID{0x110};
template<> constinit const int TypeRegister<CommandDeviceLaserPower  >::TYPE_ID{0x120};
template<> constinit const int TypeRegister<CommandDeviceFocusator   >::TYPE_ID{0x130};
template<> constinit const int TypeRegister<RequestDeviceLaserPointer>::TYPE_ID{0x210};
template<> constinit const int TypeRegister<RequestDeviceLaserPower  >::TYPE_ID{0x220};
template<> constinit const int TypeRegister<CommandCheckConnection   >::TYPE_ID{0x230};

template<> constinit const int TypeRegister<SystemState>              ::TYPE_ID{0xA001}; 
template<> constinit const int TypeRegister<ControlRX  >              ::TYPE_ID{0xA002};
template<> constinit const int TypeRegister<ControlTX  >              ::TYPE_ID{0xB001};
template<> constinit const int TypeRegister<MessageRotaryStateJson >  ::TYPE_ID{0x7B22};


template<> class TypeRegisterSizes<sizeof(MESSAGE_HEADER_GENERIC)>
{
  public:
      static constexpr std::array<int,100> SIZES
      {
          sizeof(CommandSetPosRotary      ),
          sizeof(CommandSetPosScanator    ),
          sizeof(RequestPosRotary         ),
          sizeof(RequestPosScanator       ),
          sizeof(CommandAiming1           ),
          sizeof(CommandAiming2           ),
          sizeof(RequestAiming            ),
          sizeof(CommandDeviceLaserPower  ),
          sizeof(CommandDeviceLaserPointer),
          sizeof(CommandDeviceFocusator   ),
          sizeof(RequestDeviceLaserPower  ),
          sizeof(RequestDeviceLaserPointer),
          sizeof(CommandCheckConnection   ),
      };

      static constexpr int HEADER_SIZE = sizeof(MESSAGE_HEADER_GENERIC);
      static constexpr int MinSize = TypeRegisterSizes<0>::GetMinTypeSize(SIZES) + HEADER_SIZE; 
      static constexpr int MaxSize = TypeRegisterSizes<0>::GetMaxTypeSize(SIZES) + HEADER_SIZE; 
};


class TestMessageTransmission
{
  using MessageType    = MessageGeneric<void*, MESSAGE_HEADER_GENERIC>;
  using BufferType     = RingBufferGeneric<MESSAGE_HEADER_GENERIC, TypeRegister<>::GetMinTypeSize<sizeof(MESSAGE_HEADER_GENERIC)>(), 20,IteratorMode::Continous>; 
  using DispatcherType = MessageDispatcher<MESSAGE_HEADER_GENERIC     ,BufferType>;
  public:
  TestMessageTransmission()
  {
            Connection = std::make_shared<UDPConnectionEngine>();
            RingBuffer = std::make_shared<BufferType>();
            Dispatcher = std::make_shared<DispatcherType>();
   *Connection | RingBuffer | Dispatcher;

    qDebug() << "[ TEST CONNECTION ] 192.168.1.75 3333 REMOTE: 192.168.1.59 2525";  
    Connection->listenTo("192.168.1.58",3333);
    Connection->connectTo("192.168.1.121",4444);
  }

  std::shared_ptr<UDPConnectionEngine>     Connection ;
  std::shared_ptr<MessageStorageInterface> RingBuffer ;
  std::shared_ptr<DispatcherType>          Dispatcher ;

  template<typename T> void addType() 
  { 
    Dispatcher->AppendCallback<T> ( [](MessageType& Message)
    {
     auto data = DispatcherType::ExtractData<T>(&Message); qDebug() << "GET REQUEST: " << TypeRegister<T>::GetTypeName() << data->print();
    });
  };

  template<typename T> void pushMessage(const T& Data)
  {
     auto Message = new MessageGeneric<T,MESSAGE_HEADER_GENERIC>;   
          Message->DATA = Data;

    qDebug() << "PUT MESSAGE: " << TypeRegister<T>::GetTypeName() 
             << " ID: "   << Message->HEADER.MESSAGE_IDENT 
             << " SIZE: " << Message->HEADER.DATA_SIZE << " SIZE_MESSAGE: " << Message->GetSizeMessage();
             
    Connection->slotSendMessage((const char*)Message, Message->GetSizeMessage(),0);
    delete Message;
  }
};




int main(int argc, char* argv[])
{

  QApplication app(argc,argv);
  SettingsRegister::LoadSettings("/home/orangepi/SETTINGS");

  qRegisterMetaType<const QImage&>("QImage");
  qRegisterMetaType<std::pair<float,float>>("std::pair");


  //std::string info = cv::getBuildInformation();
  //std::cout << info << std::endl;

  qDebug() << "";


  TypeRegister<CommandSetPosRotary  >    ::registerType("SET_POS_ROTARY"); 
  TypeRegister<CommandSetPosScanator>    ::registerType("SET_POS_SCANATOR");
  TypeRegister<RequestPosRotary  >       ::registerType("REQUEST_POS_SCANATOR");
  TypeRegister<RequestPosScanator>       ::registerType("REQUEST_POS_SCANATOR");
  TypeRegister<CommandAiming1>           ::registerType("COMMAND_AIMING1");
  TypeRegister<CommandAiming2>           ::registerType("COMMAND_AIMING2");
  TypeRegister<RequestAiming >           ::registerType("REQUEST_AIMING");
  TypeRegister<CommandDeviceLaserPower  >::registerType("COMMAND_LASER_POWER");
  TypeRegister<CommandDeviceLaserPointer>::registerType("COMMAND_LASER_POINTER");
  TypeRegister<CommandDeviceFocusator   >::registerType("COMMAND_LASER_FOCUSATOR");
  TypeRegister<RequestDeviceLaserPower  >::registerType("REQUEST_LASER_POWER");
  TypeRegister<RequestDeviceLaserPointer>::registerType("REQUEST_LASER_POINTER");
   TypeRegister<CommandCheckConnection  >::registerType("CHECK_CONNECTION");
                TypeRegister<SystemState>::registerType("SystemStateRotary"); 
               TypeRegister <ControlTX  >::registerType("ControlTXRotary");
                TypeRegister<ControlRX  >::registerType("ControlRXRotary");

  TypeRegister<>::TYPES_INFO.printTypesSignature();
  qDebug() << "[ MIN ] " << TypeRegister<>::GetMinTypeSize<sizeof(MESSAGE_HEADER_GENERIC)>() 
           << "[ MAX ]"  << TypeRegister<>::GetMaxTypeSize<sizeof(MESSAGE_HEADER_GENERIC)>() ;

              //====================================================
                //TestMessageTransmission TestTransmission;
                //TestTransmission.addType<CommandSetPosRotary>();
                //TestTransmission.addType<CommandSetPosScanator>();
                //TestTransmission.addType<RequestPosRotary>();
                //TestTransmission.addType<RequestPosScanator>();
                //TestTransmission.addType<CommandAiming1>();
                //TestTransmission.addType<CommandAiming2>();
                //TestTransmission.addType<CommandDeviceLaserPower>();
                //TestTransmission.addType<CommandDeviceLaserPointer>();
                //TestTransmission.addType<RequestDeviceLaserPower>();
                //TestTransmission.addType<RequestDeviceLaserPointer>();

                //TestTransmission.pushMessage(CommandSetPosScanator(20,33));
                //TestTransmission.pushMessage(CommandSetPosScanator(20.2,11.2));
                //TestTransmission.pushMessage(CommandSetPosScanator(2,3));
                //TestTransmission.pushMessage(CommandSetPosScanator(3,5));

                //TestTransmission.pushMessage(CommandAiming1(20,33,1,1));
                //TestTransmission.pushMessage(CommandAiming1(20.2,11.2,2,2));
                //TestTransmission.pushMessage(CommandAiming1(2,3,0,0));
                //TestTransmission.pushMessage(CommandAiming1(3,5,0,0));
                //====================================================

  qDebug() << "IS REGISTERED 1 : " << TypeRegisterID<>::isRegistered(1);
  qDebug() << "IS REGISTERED 2 : " << TypeRegisterID<>::isRegistered(2);
  qDebug() << "IS REGISTERED 5 : " << TypeRegisterID<>::isRegistered(3);
  qDebug() << "IS REGISTERED 45 : " << TypeRegisterID<>::isRegistered(45);


  #ifdef PROJECT_SIMPLE
  auto WindowImageProcessingDisplay = new WidgetProcessingImage("ОБРАБОТКА");
 
  auto WindowProc1Control  = new WidgetDeviceControl("[ШАБЛОН  ]"); 
  auto WindowProc2Control  = new WidgetDeviceControl("[ЦЕНТРОИД]");
  auto WindowProc3Control  = new WidgetDeviceControl("[ДЕТЕКТОР]");
  auto WindowAimingControl = new WidgetDeviceControl("[КОНТУР  ]");

  WindowProc1Control->enableScheme(0,1,0,1,0); WindowProc1Control->setScheme(0,2,0);
  WindowProc2Control->enableScheme(0,1,0,1,0); WindowProc2Control->setScheme(0,2,0);
  WindowProc3Control->enableScheme(0,1,0,1,0); WindowProc3Control->setScheme(0,2,0);
  WindowAimingControl->enableScheme(1,0,0,1,0); WindowAimingControl->setScheme(0,2,0);
  WindowProc1Control->setParamList(1000,50,100);
  WindowProc2Control->setParamList(1000,50,100);
  WindowProc3Control->setParamList(1000,50,100);

  WindowProc1Control->setButtonsName({"ПУСК", "СБРОС"});
  WindowProc2Control->setButtonsName({"ПУСК", "СБРОС"});
  WindowProc3Control->setButtonsName({"ПУСК", "СБРОС"});
  WindowAimingControl->setButtonsName({"ПУСК", "СБРОС"});

  WidgetTableGroup WindowTableGroup;
  WindowTableGroup.AddWidget(WindowImageProcessingDisplay);
  WindowTableGroup.AddWidget(WindowProc1Control);
  WindowTableGroup.AddWidget(WindowProc2Control);
  WindowTableGroup.AddWidget(WindowProc3Control);
  WindowTableGroup.AddWidget(WindowAimingControl);

  WindowTableGroup.setMaximumSize(720*1.2,540*1.2 + 300);
  WindowTableGroup.setMinimumSize(640*0.7,480*0.6 + 300);
  WindowTableGroup.move(300,300);
  WindowTableGroup.linkToWidget(WindowImageProcessingDisplay->LinkedWidget);

  ProcessControllerClass*  ProcessController = ProcessControllerClass::GetInstance();
                           ProcessController->setParent(&WindowTableGroup);

  ProcessControllerClass::DeviceCamera->CameraSetZoom(5);
  ProcessControllerClass::DeviceCamera->CameraSetExposure(1500);
  ProcessControllerClass::DeviceCamera->CameraSetGain(0);
  ProcessControllerClass::DeviceCamera->CameraStartStream(true);

                           ProcessController->slotSetProcessAiming(true);

  WindowProc1Control->linkToDevice(ProcessControllerClass::ModuleImageProc );
  WindowProc2Control->linkToDevice(ProcessControllerClass::ModuleImageProc2 );
  WindowAimingControl->linkToDevice(ProcessControllerClass::ModuleAiming1 );

  WindowImageProcessingDisplay->linkToModule(ProcessControllerClass::ModuleImageProc);    
  WindowImageProcessingDisplay->linkToModule(ProcessControllerClass::ModuleImageProc2);    



  auto AimingPort = &ProcessController->ModuleAiming1->PortSignalSetAiming; 
  QObject::connect(WindowImageProcessingDisplay->LabelImageAiming,SIGNAL(signalPosPressed2(QPair<float,float>)), 
                                                       AimingPort,SLOT  (slotSetCoord    (QPair<float,float>))) ;

  QObject::connect(WindowImageProcessingDisplay->LabelImageAiming,SIGNAL(signalPosPressed(QPair<float,float>)), 
                                                       ProcessControllerClass::ModuleImageProc.get(),SLOT  (SlotSelectObject (QPair<float,float>))) ;

  QObject::connect(WindowImageProcessingDisplay->LabelImageAiming,SIGNAL(signalPosPressed(QPair<float,float>)), 
                                                       ProcessControllerClass::ModuleImageProc2.get(),SLOT  (SlotSelectObject (QPair<float,float>))) ;

  ProcessController->slotStartProcessRTSP(true);
  WindowTableGroup.show();
  #endif
  

  #ifdef PROJECT_COMPLEX
  #endif 


  app.exec();
}


