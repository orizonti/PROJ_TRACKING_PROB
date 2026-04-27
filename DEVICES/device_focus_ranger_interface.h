#ifndef DEVICE_FOCUS_CONTROL_INTERFACE_H
#define DEVICE_FOCUS_CONTROL_INTERFACE_H

#include "state_block_enum.h"
#include <QSettings>
#include "device_generic_interface.h"
#include "message_command_structures.h"
#include <typeinfo>

template<typename T_CONNECTION, int NUM_DEVICE>
class DeviceFocusRangerInterface : public DeviceGenericInterface<T_CONNECTION, 
                                                                 MessageGenericExt<CommandDeviceRedux<NUM_DEVICE>, MESSAGE_HEADER_EXT>, 
                                                                 RequestDeviceRedux<NUM_DEVICE>>
{
public:

    using COMMAND_TYPE = CommandDeviceRedux<NUM_DEVICE>; 
    using MESSAGE_TYPE = MessageGenericExt<COMMAND_TYPE, MESSAGE_HEADER_EXT>;
    using REQUEST_TYPE = RequestDeviceRedux<NUM_DEVICE>; 
    using DEVICE_INTERFACE = DeviceGenericInterface<T_CONNECTION, MESSAGE_TYPE, REQUEST_TYPE>; 

    explicit DeviceFocusRangerInterface(std::shared_ptr<T_CONNECTION> Connection, QString Name = "[ DEVICE ]") :
             DEVICE_INTERFACE(Connection,Name) 
             {
              //commandArray = QByteArray((char*)(&DEVICE_INTERFACE::Message.DATA), 4);
             };
        	~DeviceFocusRangerInterface() { };

	void setParam(uint16_t ID, uint32_t Param) 
    {
                QDataStream stream(&commandArray, QIODevice::WriteOnly);
                 stream << ID;
        qDebug() << "[ FOCUSATOR ] [ SEND COMMAND ]" << commandArray.toHex(); 
        //DEVICE_INTERFACE::sendCommand(commandArray);
    }

	void setEnable(bool OnOff, uint16_t Number = 0)  override { if(OnOff) setParam(0,1); else setParam(0,0); }; 
    void setValue(float Value) override 
    { 
       //            int8_t* DataPtr = (int8_t*)&Value+1;
       //          QDataStream stream(&commandArray, QIODevice::WriteOnly);

       // qDebug() << "[ FOCUSATOR ] [ SEND COMMAND ]" << this->Message.toByteArray().toHex(); 
       // DEVICE_INTERFACE::sendCommand(this->Message);
    };

	float getDistance() { return (float)requestMessage.Param; };
	float getValue() override { return getDistance(); };
    void putMessage(REQUEST_TYPE Message) { };

    REQUEST_TYPE requestMessage;
    QByteArray commandArray;
};

#endif 
