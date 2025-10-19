#ifndef DEVICE_GENERIC_CLASS_H
#define DEVICE_GENERIC_CLASS_H

#include "message_command_structures.h"
#include "message_struct_generic_ext.h"
#include "message_header_generic_ext.h"
#include "interface_pass_coord.h"
#include "message_struct_generic.h"


template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
class DeviceGenericInterface 
{
public:
    DeviceGenericInterface(T_CONNECTION* Connection, QString Name = "[ DEVICE ]"): TAG_NAME(Name) 
	{ 
		ConnectionDevice = Connection;
		init();
	};
	QString TAG_NAME;

	~DeviceGenericInterface() {};

	virtual void setParam (uint8_t ID, uint16_t Value) = 0; 
	virtual void setParams(uint16_t Value1, uint16_t Value2) = 0;
	virtual void setParams(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t Value4) = 0; 

    void sendCommand() { ConnectionDevice->slotSendMessage(Command.toByteArray());};
	void sendCommand(T_COMMAND commandDevice) { Command = commandDevice;  ConnectionDevice->SlotSendCommand(Command.toByteArray());};

    virtual void putMessage(T_MESSAGE Message) = 0;
	void init()
	{
    //auto callBack = [this](MessageGeneric<void*, T_CONNECTION::HeaderType>& Message)
    //{ 
    //auto MessageData = ConnectionDevice->Dispatcher->ExtractData<T_MESSAGE>(&Message);
    //putMessage(*MessageData);
    //}; 
    //if(ConnectionDevice) ConnectionDevice->Dispatcher->AppendCallback<T_MESSAGE>(callBack);
	}
	
protected:
    T_CONNECTION* ConnectionDevice = nullptr;
    MessageGenericExt<T_COMMAND,MESSAGE_HEADER_EXT> Command;
};

class DeviceLaserGenericInterface
{
	public:
	virtual void setEnable(bool OnOff) = 0;
	virtual void setPowerEnable(bool OnOff) = 0;
	virtual void setPilotEnable(bool OnOff) = 0;
	virtual void setPower(uint16_t Value) = 0;
};

class DeviceFocusGenericInterface
{
	public:
	virtual void setEnable(bool OnOff) = 0;
	virtual void setPos(uint16_t Value) = 0;
	virtual uint16_t getDistance() = 0;
};

class DeviceRotaryGenericInterface: public PassCoordClass<float>
{
	public:
	virtual void moveOnStep(const QPair<int, int>& Pos) = 0;
	virtual void moveToPos(const QPair<int, int>& Pos) = 0;
	virtual void moveToPosRelative(const QPair<int, int>& Pos) = 0;
	virtual void      setVelocity(const QPair<int, int>& Velocity) = 0;
	virtual void moveWithVelocity(const QPair<int, int>& Velocity) = 0;
	virtual void stopMove() = 0;

	const QPair<float, float>& getOutput() {OutputCoord = getPos(); return PassCoordClass<float>::OutputCoord;};
	void setInput(const QPair<float, float>& Coord) { moveToPos(Coord);};

	virtual const QPair<int,int>& getPos() = 0;
	virtual const QPair<int,int>& getVelocity() = 0;

virtual	          bool checkRangeOffset(QPair<int,int> Pos) = 0;
virtual	QPair<int,int> getLimits(int axis) = 0;
virtual	QPair<int,int> getRange() = 0;
};


#endif 
