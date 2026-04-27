#ifndef DEVICE_LID_CONTROL_CLASS_H
#define DEVICE_LID_CONTROL_CLASS_H

#include "message_command_structures.h"
#include "device_generic_interface.h"


template<typename T_CONNECTION>
class DeviceLidControl : public DeviceGenericInterface<T_CONNECTION, CommandLidControlJson, CommandLidControlJson>
{
public:
  DeviceLidControl(std::shared_ptr<T_CONNECTION> Connection, QString name = "[LID]"): 
  DeviceGenericInterface<T_CONNECTION,CommandLidControlJson, CommandLidControlJson>(Connection, name) {};


	QString TAG_NAME{"[ LID ]"};
  void putMessage(CommandLidControlJson Message) { };

  void setEnable(bool OnOff, uint16_t Number = 0) override
  { 
    if(Number == 1) { if (OnOff) this->sendCommand(commandOpenRight); else this->sendCommand(commandCloseRight); }
    if(Number == 2) { if (OnOff) this->sendCommand(commandOpenLeft);  else this->sendCommand(commandCloseLeft); }

    //if(Number == 1) { if (OnOff) commandOpenRight.printCommand(); else commandCloseRight.printCommand(); }
    //if(Number == 2) { if (OnOff) commandOpenLeft.printCommand(); else commandCloseLeft.printCommand(); }
  };

  CommandLidControlJson commandOpenRight {"id: 1, type: rightCapOpen" };
  CommandLidControlJson commandCloseRight{"id: 1, type: rightCapClose"};

  CommandLidControlJson commandOpenLeft {"id: 1, type: leftCapOpen"  };
  CommandLidControlJson commandCloseLeft{"id: 1, type: leftCapClose" };

};

#endif 
