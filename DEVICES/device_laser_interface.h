#ifndef LASER_CONTROL_CLASS_H
#define LASER_CONTROL_CLASS_H

#include "rotate_vector_class.h"
#include "state_block_enum.h"

#include <QSettings>
#include <QTimer>
#include "engine_udp_interface.h"
#include "engine_tcp_interface.h"
#include "message_command_structures.h"
#include "message_struct_generic_ext.h"
#include "message_header_generic_ext.h"

#include "debug_output_filter.h"
#include "engine_tcp_interface.h"
#include "device_generic_interface.h"

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
class DeviceLaserControl : public DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>, public DeviceLaserInterface
{
public:
    using DEVICE_BASE_TYPE = DeviceGenericInterface<T_CONNECTION, T_COMMAND, T_MESSAGE>; 
    explicit DeviceLaserControl(std::shared_ptr<T_CONNECTION> Connection, QString Name = "[ DEVICE ]");
	QString TAG_NAME{"[ LASER ]"};

	~DeviceLaserControl();

	public:
  void loadSettings(){};
	void setParam(uint8_t ID, uint16_t Value) override;
	void setParams(uint16_t Value1, uint16_t Value2) override {};
	void setParams(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t Value4) override {}; 

  void putMessage(T_MESSAGE Message) override; 

	public:
	void setEnable(bool OnOff)      override {};
	void setPowerEnable(bool OnOff) override {};
	void setPilotEnable(bool OnOff) override {};
	void setPower(uint16_t Value)   override {};
	bool getState() { return messageState.Param1 == 0 ? false : true; }
  void transmitCommand(T_COMMAND Command);
  T_MESSAGE messageState;
private:
    std::map<uint8_t, uint8_t> KEY_MODULE;       //GET NUMBER MODULE FROM COMMAND CODE
    std::map<uint8_t, uint8_t> KEY_MODULE_PARAM; //KEY PARAM VALUE FROM COMMAND CODE
	std::map<uint8_t, std::map<uint8_t,uint8_t>> ID_PARAM_KEY; //GET COMMAND CODE FROM [COMMAND_ID PARAM]
};

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
DeviceLaserControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::~DeviceLaserControl() { qDebug() << TAG_NAME << "DELETE"; }

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceLaserControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::setParam(uint8_t ID, uint16_t Value)
{
	uint8_t param = Value > 0 ? 1 : 0;  

	DEVICE_BASE_TYPE::Command.DATA.Command  = ID_PARAM_KEY[ID][param];
	DEVICE_BASE_TYPE::Command.DATA.Param    = Value;
	DEVICE_BASE_TYPE::sendCommand();
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceLaserControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::putMessage(T_MESSAGE message)
{
    //ControlWindow->setState(1);
    //ControlWindow->setParam(0, 0);
	messageState = message;
	qDebug() << "[ LASER MODULE GET MESSAGE ]";
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
void DeviceLaserControl<T_CONNECTION,T_COMMAND,T_MESSAGE>::transmitCommand(T_COMMAND Command)
{
  qDebug() << "DEVICE LASER TRANSMIT: " << Qt::hex << TypeRegister<T_COMMAND>::TYPE_ID;
	DEVICE_BASE_TYPE::sendCommand(Command);
}

template<typename T_CONNECTION, typename T_COMMAND, typename T_MESSAGE>
DeviceLaserControl<T_CONNECTION, T_COMMAND, T_MESSAGE>::DeviceLaserControl(std::shared_ptr<T_CONNECTION> Connection, QString Name): 
DeviceGenericInterface<T_CONNECTION,T_COMMAND, T_MESSAGE>(Connection, Name)
{
  KEY_MODULE[LASER_FAULT]     = LASER_MODULE;
  KEY_MODULE[LASER_ON]        = LASER_MODULE;       KEY_MODULE[LASER_OFF]       = LASER_MODULE;
  KEY_MODULE[LASER_BEAM_ON]   = LASER_MODULE_BEAM ; KEY_MODULE[LASER_BEAM_OFF]  = LASER_MODULE_BEAM;
  KEY_MODULE[LASER_PILOT_ON]  = LASER_MODULE_PILOT; KEY_MODULE[LASER_PILOT_OFF] = LASER_MODULE_PILOT;
  KEY_MODULE[LASER_SET_POWER] = LASER_MODULE_POWER;

  KEY_MODULE_PARAM[LASER_ON]       = 1; KEY_MODULE_PARAM[LASER_OFF]       = 0; KEY_MODULE[LASER_FAULT] = 1;
  KEY_MODULE_PARAM[LASER_BEAM_ON]  = 1; KEY_MODULE_PARAM[LASER_BEAM_OFF]  = 0;
  KEY_MODULE_PARAM[LASER_PILOT_ON] = 1; KEY_MODULE_PARAM[LASER_PILOT_OFF] = 0;
  //==========================================================================
  ID_PARAM_KEY[LASER_MODULE]      [1] = LASER_ON;       ID_PARAM_KEY[LASER_MODULE]      [0] = LASER_OFF;
  ID_PARAM_KEY[LASER_MODULE_BEAM] [1] = LASER_BEAM_ON;  ID_PARAM_KEY[LASER_MODULE_BEAM] [0] = LASER_BEAM_OFF;
  ID_PARAM_KEY[LASER_MODULE_PILOT][1] = LASER_PILOT_ON; ID_PARAM_KEY[LASER_MODULE_PILOT][0] = LASER_PILOT_OFF;
  ID_PARAM_KEY[LASER_MODULE_POWER][1] = LASER_SET_POWER;ID_PARAM_KEY[LASER_MODULE_POWER][0] = LASER_SET_POWER;
}

#endif 
