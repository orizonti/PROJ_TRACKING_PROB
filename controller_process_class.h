#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include <QSettings>
#include <QTimer>
#include <QSharedPointer>

//#include "CAMERA_INTERFACE/camera_interface_class.h"
#include "CAMERA_INTERFACE/interface_camera_hik.h"
#include "aiming_class.h"
//#include "rotation_find_executor.h"

#include "CV_IMAGE_PROCESSING/tracker_centroid.h"
#include "CV_IMAGE_PROCESSING/tracker_template.h"

#include "engine_udp_interface.h"
#include "engine_can_interface.h"
#include "image_processing_node.h"
#include "message_command_structures.h"
#include "device_rotary_interface.h"
#include "device_laser_interface.h"
#include "aiming_monitoring_module.h"
#include "/home/orangepi/SETTINGS/TrackingProject/SETTINGS_DEFINES.h"


enum class ProcessStateList  { ProcessAiming, ProcessImitation, ProcessTestSignal, ProcessCalibration}; 
//#define USE_HIK_CAMERA 1

#if USE_CAMERA_TYPE == CAMERA_TYPE_HIK
#include "interface_camera_hik.h"
using TypeCamera = CameraInterfaceHIK;
#endif

#if USE_CAMERA_TYPE == CAMERA_TYPE_RTSP
#include "interface_camera_rtsp.h"
using TypeCamera = CameraInterfaceUniversal;
#endif

//=================================================================================================
                             using CommandPlatform = MessageGenericExt<ControlTX, MESSAGE_HEADER_ROTARY>;
                             using CommandScanator = CommandSetPosScanator;

using TypeRotaryPlatform  = DeviceRotaryControl<UDPConnectionEngine, CommandPlatform, ControlRX>;
using TypeRotaryScanator  = DeviceRotaryControl<CANConnectionEngine, CommandScanator, RequestPosScanator>;

#if(USE_ROTARY_TYPE == ROTARY_TYPE_PLATFORM)
using TypeDeviceRotary    = TypeRotaryPlatform;
#endif

#if(USE_ROTARY_TYPE == ROTARY_TYPE_SCANATOR)
using TypeDeviceRotary    = TypeRotaryScanator;
#endif

//=================================================================================================
using TypeDeviceLaserPower = DeviceLaserInterface<UDPConnectionEngine,0>;
using TypeDeviceLaserIllum = DeviceLaserInterface<UDPConnectionEngine,1>; 
//=================================================================================================
using MessageType    = MessageGeneric<void*, MESSAGE_HEADER_GENERIC>;
using BufferType     = RingBufferGeneric<MESSAGE_HEADER_GENERIC, 12, 20,IteratorMode::Continous>; 
using DispatcherType = MessageDispatcher<MESSAGE_HEADER_GENERIC  ,BufferType>;

using MessageType2   = MessageGenericExt<ControlRX          , MESSAGE_HEADER_ROTARY     >;

using BufferType2     = RingBufferGeneric<MESSAGE_HEADER_ROTARY , MessageType2::GetSizeMessage(), 12,IteratorMode::Chunked>; 
using DispatcherType2 = MessageDispatcher<MESSAGE_HEADER_ROTARY , BufferType2>;

//=================================================================================================
using TypeAimingMonitoring = ModuleAimingMonitoring<UDPConnectionEngine>;


class VideoStreamRTSP;

class ProcessControllerClass : public QObject
{
    Q_OBJECT
public:
      ~ProcessControllerClass();
std::string TAG_NAME = QString("[ %1 ] ").arg("CONTROLLER").toStdString();
static ProcessControllerClass* GetInstance(QObject* parent = nullptr);


//=============================================================
static std::shared_ptr<TypeCamera>      DeviceCamera;
static constexpr int TypeCameraUsed = USE_CAMERA_TYPE; 
//=============================================================
static std::shared_ptr<TypeDeviceLaserPower> DeviceLaserPower;
static std::shared_ptr<TypeDeviceLaserIllum> DeviceLaserIllum;
//=============================================================
static std::shared_ptr<TypeDeviceRotary>     DeviceRotary;
static constexpr int TypeRotaryUsed = USE_ROTARY_TYPE; 
//=============================================================
static std::shared_ptr<ModuleImageProcessing > ModuleImageProc;
static std::shared_ptr<ModuleImageProcessing>  ModuleImageProc2;
static std::shared_ptr<AimingClass>            ModuleAiming1;
static std::shared_ptr<AimingClass>            ModuleAiming2;
//=============================================================
static std::shared_ptr<UDPConnectionEngine> ConnectionControlUDP;
static std::shared_ptr<UDPConnectionEngine> ConnectionRotaryUDP;
static std::shared_ptr<UDPConnectionEngine> ConnectionProcessorUDP;

static std::shared_ptr<CANConnectionEngine> ConnectionCAN;
static std::shared_ptr<MessageStorageInterface> RingBuffer ;
static std::shared_ptr<DispatcherType>          Dispatcher ;

static std::shared_ptr<MessageStorageInterface> RingBuffer2 ;
static std::shared_ptr<DispatcherType>          Dispatcher2 ;

static std::shared_ptr<MessageStorageInterface> RingBuffer3 ;
static std::shared_ptr<DispatcherType2>         Dispatcher3 ;
//=============================================================
static std::shared_ptr<VideoStreamRTSP>      ModuleVideoOutput;
static std::shared_ptr<TypeAimingMonitoring> ModuleAimingMonitor1;
static std::shared_ptr<TypeAimingMonitoring> ModuleAimingMonitor2;
static std::shared_ptr<TypeAimingMonitoring> ModuleAimingMonitor3;


    ProcessControllerClass(QObject* parrent = 0);
    ProcessControllerClass(const ProcessControllerClass& Copy) = delete;
    void operator=(const ProcessControllerClass& Copy) = delete;

ProcessStateList ProcessState{ProcessStateList::ProcessAiming};
void StopAllProcess();
void DeleteModulesLinks();

private:
    static ProcessControllerClass* ProcessControllerInstance;

QThread ThreadProcess;
QThread ThreadProcess2;

QThread ThreadCamera;
QThread ThreadUtilite;

public slots:

void slotSetProcessCamera(bool OnOff);
void slotSetProcessAiming(bool OnOff);
void slotSetProcessAiming2(bool OnOff);
void slotStartProcessRTSP(bool OnOff);


signals:
void SignalProcessEnd();

};


#endif //PROCESS_CONTROLLER_H


