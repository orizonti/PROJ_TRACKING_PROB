#ifndef DEVICE_GENERIC_CLASS_H
#define DEVICE_GENERIC_CLASS_H

#include "interface_pass_coord.h"
#include <typeinfo>
#include "engine_type_register.h"
#include "message_struct_generic.h"
#include "message_header_generic.h"
#include "message_command_structures.h"

class DeviceGenericHandleControl
{
	public:
 	virtual void setLevel ( uint32_t Level) {};
	virtual void setValue ( float    Value) { setParam(0,Value); };
	virtual void setPair  ( std::pair<float,float> Coord)    {};
	virtual void setEnable(bool OnOff, uint16_t Number = 0) {};

	virtual std::pair<float,float> getPair()  { return std::pair<float,float>(0,0); };
	virtual                  float getValue() { return 0; };

	virtual void  setParam (uint16_t CommandID, float    CommandParam) {} ;
	virtual float getParam (uint16_t CommandID) { return 0;} ;
};


template<typename T_CONNECTION, typename T_COMMAND, typename T_REQUEST>
class DeviceGenericInterface : public DeviceGenericHandleControl
{
public:
  DeviceGenericInterface(std::shared_ptr<T_CONNECTION> Connection, QString Name = "[ DEVICE ]"): TAG_NAME(Name) 
	{ 
		ConnectionDevice = Connection;
	};
	QString TAG_NAME;

	~DeviceGenericInterface() {};

  virtual void putMessage(T_REQUEST Request) {};

	void setParam (uint16_t CommandID, float    CommandParam) override {} ;

  void transmitMessage(const char* Message, int size, uint16_t param) { ConnectionDevice->slotSendMessage(Message,size, param); }

	void sendCommand(QByteArray command) { ConnectionDevice->slotSendMessage(command); };
	void sendCommand(T_COMMAND& commandToSend) 
	{ 
				  Command = commandToSend;
				  Command.dumpToByteArray(MessageOutputBuffer);
		ConnectionDevice->slotSendMessage(MessageOutputBuffer);
    //qDebug() << TAG_NAME << "[ SEND COMMAND ] " << MessageOutputBuffer.toHex(); 
	};

	template<typename T> 
	void sendCommand(const QPair<T,T>& data) { 
								 Command.setData(data); 
								 Command.dumpToByteArray(MessageOutputBuffer);
			           ConnectionDevice->slotSendMessage(MessageOutputBuffer); 
    //qDebug() << TAG_NAME << "[ SEND COMMAND ] " << MessageOutputBuffer.toHex(); 
  };

	template<typename T> 
	void sendCommand(const QPair<T,T>& data, const QPair<T,T>& data2) { 
								 Command.setData(data,data2); 
								 Command.dumpToByteArray(MessageOutputBuffer);
			           ConnectionDevice->slotSendMessage(MessageOutputBuffer); 

    //qDebug() << TAG_NAME << "[ SEND COMMAND ] " << MessageOutputBuffer.toHex(); 
  };

  template<COMMAND_STANDART TYPE>
	void sendCommand(TYPE& commandToSend) 
	{
				  Command.DATA = commandToSend;
		ConnectionDevice->slotSendMessage(Command.castToByteArray());
		//qDebug()<< TAG_NAME << "[ SEND STANDART COMMAND ]" << typeid(commandToSend).name();
	};
	
protected:
    std::shared_ptr<T_CONNECTION> ConnectionDevice = nullptr;
    T_COMMAND Command;
    T_REQUEST Request;

    QByteArray MessageOutputBuffer;
};



//=========================================================
template<typename T, typename H> class MessageGenericExt; 
                                 class MESSAGE_HEADER_EXT; 

template<int NUM_DEVICE> struct CommandDevice;
template<int NUM_DEVICE> struct RequestDevice;
template<int NUM_DEVICE>  class MessageDeviceGeneric : public MessageGenericExt<CommandDevice<NUM_DEVICE>   ,MESSAGE_HEADER_EXT> { public: };

template<typename T_CONNECTION, int NUM_DEVICE>
class DeviceGenericControl : public DeviceGenericInterface<T_CONNECTION, MessageDeviceGeneric<NUM_DEVICE> ,MessageDeviceGeneric<NUM_DEVICE> >
{
public:
    using DEVICE_INTERFACE = DeviceGenericInterface<T_CONNECTION, MessageDeviceGeneric<NUM_DEVICE>, MessageDeviceGeneric<NUM_DEVICE>>; 
    explicit DeviceGenericControl(std::shared_ptr<T_CONNECTION> Connection, QString Name = "[ DEVICE ]") : DEVICE_INTERFACE(Connection, Name)
	{

	}
	~DeviceGenericControl()
	{

	};


	void setParam (uint16_t CommandID, uint32_t CommandParam) override
	{
      qDebug() << "DEVICE SET PARAM: " << CommandParam;     
	}
	void setParam (uint16_t CommandID, float    CommandParam) override
	{

	}

	QString TAG_NAME{"[ DEVICE_ANY ]"};
	QString DISPLAY_NAME{"Устройство"};
};
//=========================================================
template<int NUM> class CommandAiming;

template<int NUM_DEVICE> class MessageAimingDevice : public MessageGenericExt<CommandAiming<NUM_DEVICE>   ,MESSAGE_HEADER_EXT> { public: };


#endif 
