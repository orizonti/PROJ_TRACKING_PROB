#ifndef LASER_CONTROL_CLASS_H
#define LASER_CONTROL_CLASS_H

#include "rotate_vector_class.h"
#include "state_block_enum.h"

#include <QSettings>
#include <QTimer>
#include "engine_udp_interface.h"
#include "engine_tcp_interface.h"
#include "message_struct_generic_ext.h"
#include "message_header_generic_ext.h"

#include "debug_output_filter.h"
#include "engine_tcp_interface.h"
#include "device_generic_interface.h"
#include <QDebug>
#include <QTimer>


template<typename T_CONNECTION, int NUM_DEVICE>
class DeviceLaserInterface : public DeviceGenericInterface<T_CONNECTION, 
                                                           MessageGenericExt<CommandDeviceRedux<NUM_DEVICE>  ,MESSAGE_HEADER_EXT>, 
                                                           RequestDeviceRedux<NUM_DEVICE>>
{
public:
    using COMMAND_TYPE = CommandDeviceRedux<NUM_DEVICE>; 
    using MESSAGE_TYPE = MessageGenericExt<COMMAND_TYPE, MESSAGE_HEADER_EXT>;
    using REQUEST_TYPE = RequestDeviceRedux<NUM_DEVICE>; 
    using DEVICE_INTERFACE = DeviceGenericInterface<T_CONNECTION, MESSAGE_TYPE, REQUEST_TYPE>; 

    explicit DeviceLaserInterface(std::shared_ptr<T_CONNECTION> Connection, QString Name = "[ DEVICE ]");
	          ~DeviceLaserInterface();
	QString DISPLAY_NAME{"Силовой лазер"};

	public:
    void loadSettings(){};
	  void setParam(uint16_t ID, float Param) override;

    void putMessage(REQUEST_TYPE message) override; 
	QString getName() { return DISPLAY_NAME; }

	public:

  void setCheckProcedure();
	void setReady(bool OnOff)       { setParam(LASER_MODULE,OnOff); };
	void setPowerEnable(bool OnOff) { setParam(LASER_MODULE_BEAM ,OnOff); };
	void setPilotEnable(bool OnOff) { setParam(LASER_MODULE_PILOT,OnOff); };

    //========================================================
    //DEVICE_GENERIC_HANDLE_CONTROL
	void setLevel(uint32_t Level) override { if(Level == 1) setPowerLow(); if(Level == 2) setPowerHigh(); };
  void setValue(float Value) override { setPower(90*Value); }
	void setEnable(bool OnOff, uint16_t Number = 0) override
  {
        if(Number == 1) setPowerEnable(OnOff); 
        if(Number == 2) setPilotEnable(OnOff); 
  }
    //========================================================

	void setPower(uint16_t Value)   
    { 
        QTimer::singleShot(10,   [this]()       { this->setPowerEnable(false); });
        QTimer::singleShot(1000, [this]()       { this->setReady(false);       });
        QTimer::singleShot(2000, [this]()       { this->setCheckProcedure();   });
        QTimer::singleShot(3000, [this]()       { this->setReady(true);        });
        QTimer::singleShot(4000, [this,Value]() { this->setParam(LASER_MODULE_POWER, Value); });
    };
	void setPowerHigh()   { setPower(90); };
	void setPowerLow()    { setPower(15);  };
	bool getState() { return Request.Param1 == 0 ? false : true; }
private:
    std::map<uint8_t, uint8_t> KEY_MODULE;       //GET NUMBER MODULE FROM COMMAND CODE
    std::map<uint8_t, uint8_t> KEY_MODULE_PARAM; //KEY PARAM VALUE FROM COMMAND CODE
	std::map<uint8_t, std::map<uint8_t,uint8_t>> ID_PARAM_KEY; //GET COMMAND CODE FROM [COMMAND_ID PARAM]
    MESSAGE_TYPE Command;
    REQUEST_TYPE Request;
};

template<typename T_CONNECTION, int NUM_DEVICE>
DeviceLaserInterface<T_CONNECTION,NUM_DEVICE>::~DeviceLaserInterface() { qDebug() << this->TAG_NAME << "DELETE"; }

template<typename T_CONNECTION, int NUM_DEVICE>
void DeviceLaserInterface<T_CONNECTION, NUM_DEVICE>::setParam(uint16_t ID, float Value)
{
	uint8_t param = Value > 0 ? 1 : 0;  
	Command.DATA.Command = ID_PARAM_KEY[ID][param];
	Command.DATA.Param1  = Value;
    this->sendCommand(Command);
}

template<typename T_CONNECTION, int NUM_DEVICE>
void DeviceLaserInterface<T_CONNECTION, NUM_DEVICE>::setCheckProcedure()
{
	Command.DATA.Command  = 0x20;
	Command.DATA.Param1   = 0;
	this->sendCommand(Command);
}

template<typename T_CONNECTION, int NUM_DEVICE>
void DeviceLaserInterface<T_CONNECTION, NUM_DEVICE>::putMessage(REQUEST_TYPE message)
{
	qDebug() << "[ LASER MODULE GET MESSAGE ]";
}

template<typename T_CONNECTION, int NUM_DEVICE>
DeviceLaserInterface<T_CONNECTION, NUM_DEVICE>::DeviceLaserInterface(std::shared_ptr<T_CONNECTION> Connection, QString Name): 
DEVICE_INTERFACE(Connection, Name)
{
  QTimer::singleShot(100, [this]()  { this->setCheckProcedure(); });

  DISPLAY_NAME = Name;
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
