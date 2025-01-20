#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include <QSettings>
#include <QTimer>
#include <QSharedPointer>

#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include "scanator_control_class.h"
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
#include "CV_IMAGE_PROCESSING/cv_image_processing.h"
#include "CAMERA_INTERFACE/camera_interface_class.h"
//#include "motor_imitator_control_interface.h"
#include "rotation_find_executor.h"
#include "aiming_class.h"
#include "WindowLaserControl.h"

enum class ProcessStateList  { ProcessAiming, ProcessImitation, ProcessTestSignal, ProcessCalibration}; 

class ProcessControllerClass : public QObject
{
    Q_OBJECT
public:
std::string TAG_NAME{"[CONTROLLER]"};
static ProcessControllerClass* GetInstance();

static std::shared_ptr<CameraInterfaceClassAravis> DeviceCamera;
static std::shared_ptr<ScanatorControlClass>       DeviceScanator;
static std::shared_ptr<LaserControlClass>          DeviceLaser;
//static std::shared_ptr<MotorInterface>             DeviceImitatorMotor;

static std::shared_ptr<CVImageProcessing>          ModuleImageProc;
static std::shared_ptr<AimImageImitatorClass>      ModuleImitatorImage;
static std::shared_ptr<AimingClass>                ModuleAiming;
static std::shared_ptr<SinusGeneratorClass>        ModuleSinusGenerator;

static std::shared_ptr<RotationFindProcessClass>  ProcessFindRotation;

ProcessStateList ProcessState{ProcessStateList::ProcessAiming};

private:
    ProcessControllerClass(QObject* parrent = 0);
    ProcessControllerClass(const ProcessControllerClass& Copy) = delete;
    void operator=(const ProcessControllerClass& Copy) = delete;

    static ProcessControllerClass* ProcessControllerInstance;

QThread processThread;
QThread utiliteThread;

public slots:
void SlotStartProcessRotFind(bool OnOff);
void SlotStartProcessRotFindRevert(bool OnOff);

void SlotSetProcessAiming(bool OnOff);
void SlotSetProcessImitation(bool OnOff);
void SlotSetProcessTestSignal(bool OnOff);

signals:
void SignalProcessEnd();


private:
void DisplayState();
};


#endif //PROCESS_CONTROLLER_H


