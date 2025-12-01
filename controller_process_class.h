#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include <QSettings>
#include <QTimer>
#include <QSharedPointer>

#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
//#include "CAMERA_INTERFACE/camera_interface_class.h"
#include "CAMERA_INTERFACE/interface_camera_hik.h"
#include "aiming_class.h"
//#include "rotation_find_executor.h"

#include "CV_IMAGE_PROCESSING/tracker_centroid.h"
#include "CV_IMAGE_PROCESSING/tracker_template.h"

#include "engine_udp_interface.h"
#include "engine_can_interface.h"
#include "image_processing_node.h"
#include "interface_camera_rtsp.h"
#include "message_command_structures.h"
#include "device_rotary_interface.h"
#include "device_laser_interface.h"

enum class ProcessStateList  { ProcessAiming, ProcessImitation, ProcessTestSignal, ProcessCalibration}; 

using DeviceTypeLaserPointer = DeviceLaserControl<CANEngineInterface, CommandDeviceLaserPointer, MessageDeviceLaserPointer>;
using DeviceTypeLaserPower   = DeviceLaserControl<CANEngineInterface, CommandDeviceLaserPower, MessageDeviceLaserPower>;

//using CameraType = CameraInterfaceHIK;
using CameraType = CameraInterfaceUniversal;

class VideoStreamRTSP;
class ProcessControllerClass : public QObject
{
    Q_OBJECT
public:
      ~ProcessControllerClass();
std::string TAG_NAME = QString("[ %1 ] ").arg("CONTROLLER").toStdString();
static ProcessControllerClass* GetInstance(QObject* parent = nullptr);


static std::shared_ptr<CameraType> DeviceCamera;

static std::shared_ptr<DeviceTypeLaserPower>   DeviceLaserPower;
static std::shared_ptr<DeviceTypeLaserPointer> DeviceLaserPointer;

static std::shared_ptr<ModuleImageProcessing > ModuleImageProc;
static std::shared_ptr<ModuleImageProcessing>  ModuleImageProc2;


static std::shared_ptr<AimImageImitatorClass>   ModuleImitatorImage;

static std::shared_ptr<DeviceRotaryInterface> DeviceRotary;

static std::shared_ptr<AimingClass>          ModuleAiming1;
static std::shared_ptr<AimingClass>          ModuleAiming2;
static std::shared_ptr<VideoStreamRTSP>      ModuleVideoOutput;

static std::shared_ptr<UDPEngineInterface> ConnectionUDP;
static std::shared_ptr<CANEngineInterface> ConnectionCAN;

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
void slotSetProcessImitation(bool OnOff);
void slotSetProcessAiming(bool OnOff);
void slotSetProcessAiming2(bool OnOff);
void slotStartProcessRTSP(bool OnOff);


signals:
void SignalProcessEnd();

};


#endif //PROCESS_CONTROLLER_H


