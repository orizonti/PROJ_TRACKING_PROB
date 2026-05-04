#ifndef AIMING_MONITORING
#define AIMING_MONITORING
#include "device_generic_interface.h"
#include "message_command_structures.h"
#include "image_processing_node.h"
#include "aiming_class.h"
#include <QTimer>
#include <thread>
#include <atomic>
#include <chrono>

using MessageAimingMonitoring = MessageGenericExt<CommandAiming<0>,MESSAGE_HEADER_GENERIC>;
template<typename T_CONNECTION>
class ModuleAimingMonitoring : public DeviceGenericInterface<T_CONNECTION, MessageAimingMonitoring,MessageAimingMonitoring>
{
public:
    using DEVICE_INTERFACE = DeviceGenericInterface<T_CONNECTION, MessageAimingMonitoring, MessageAimingMonitoring>; 
  explicit ModuleAimingMonitoring(std::shared_ptr<T_CONNECTION> Connection): DEVICE_INTERFACE(Connection, "[AIMING_MONITOR]")
	{

	}
	~ModuleAimingMonitoring() { FLAG_END = true; };
  std::jthread* thread_watch = nullptr;

  std::atomic_bool FLAG_END = false;
  std::atomic_bool FLAG_WORK = false;

  std::shared_ptr<ModuleImageProcessing> ImageProcessor = nullptr;
  std::shared_ptr<AimingClass> AimingNode = nullptr;

  void linkToModule(std::shared_ptr<AimingClass> Node) { AimingNode = Node; init(); }
  void linkToModule(std::shared_ptr<ModuleImageProcessing> Node) { ImageProcessor = Node; init(); }

	void setParam (uint16_t CommandID, float    CommandParam) override { }


  void startWork(bool OnOff) { FLAG_WORK = OnOff; qDebug() << TAG_NAME << "START WORK"; }  
  CommandAiming<0> StateAiming;

  void init()
  {
    if(AimingNode) 
    {
      thread_watch = new std::jthread([this] () 
          {
              while(FLAG_END)
              {
              if(FLAG_WORK) { StateAiming = AimingNode->GetAimingError(); this->sendCommand(StateAiming); }

              std::this_thread::sleep_for(std::chrono::milliseconds(1000/24));
              }
          });
    }

    if(ImageProcessor) 
    {
      thread_watch = new std::jthread([this] () 
          {
              while(!FLAG_END)
              {
              if(FLAG_WORK){ StateAiming = ImageProcessor->getPoints()[0]; this->sendCommand(StateAiming); } ;
              if(FLAG_WORK)
              { 
              StateAiming = ImageProcessor->getPoints()[0];
              } ;

              std::this_thread::sleep_for(std::chrono::milliseconds(1000/24));
              }
          });

    }
    thread_watch->detach();
  }

	QString TAG_NAME{"[ AIMING_MONITOR ]"};
};
//=========================================================

#endif
