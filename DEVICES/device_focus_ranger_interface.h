#ifndef DEVICE_FOCUS_CONTROL_INTERFACE_H
#define DEVICE_FOCUS_CONTROL_INTERFACE_H

#include "state_block_enum.h"
#include <QSettings>
#include "device_generic_interface.h"
#include "widget_focus_ranger_control.h"


template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
class DeviceFocusRangerInterface : public DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>, public DeviceFocusGenericInterface
{
public:
    explicit DeviceFocusRangerInterface(T_CONNECTION* Connection, QString Name) :
             DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>(Connection,Name) { };
        	~DeviceFocusRangerInterface(){};

    using DEVICE_BASE_TYPE = DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>; 

    void linkToControlWindow(WidgetFocusRangerControl* Window ){ ControlWindow = Window;};
    WidgetFocusRangerControl* ControlWindow;

	void setParam(uint8_t ID, uint16_t Value) override
    {
        DEVICE_BASE_TYPE::Command.DATA.DeviceID = TypeRegister<T_COMMAND>::GetTypeID();
        DEVICE_BASE_TYPE::Command.DATA.Command  = ID;
        DEVICE_BASE_TYPE::Command.DATA.Param    = Value;
        DEVICE_BASE_TYPE::sendCommand();
    }
	void setParams(uint16_t Value1, uint16_t Value2) override {};
	void setParams(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t Value4) override {}; 

	void setEnable(bool OnOff)  override { if(OnOff) setParam(0,1); else setParam(0,0); }; 
    void setPos(uint16_t Value) override { setParam(1,Value); };

	uint16_t getDistance() override { return messageState.Param1; };
        void putMessage(T_MESSAGE Message) { messageState = Message;};
    T_MESSAGE messageState;
};

#endif 
