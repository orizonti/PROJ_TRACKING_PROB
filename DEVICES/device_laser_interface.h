#ifndef LASER_CONTROL_CLASS_H
#define LASER_CONTROL_CLASS_H

#include "rotate_vector_class.h"
#include "state_block_enum.h"

#include <QSettings>
#include <QTimer>
#include "engine_udp_interface.h"
#include "engine_tcp_interface.h"

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
	QString DISPLAY_NAME{"Лазер"};

	public:
    void loadSettings(){};

	void setParam(uint16_t ID, float Param) override;

       void putMessage(REQUEST_TYPE message) override; 
	QString getName() { return DISPLAY_NAME; }

	public:

    void setReset()                 { this->ConnectionDevice->slotSendMessage(MAP_COMMAND[LASER_FAULT_RESET].toLatin1()); };
    void setPowerEnable(bool OnOff) 
    {
        qDebug() << this->TAG_NAME.toStdString().c_str() << "[ENABLE]" << OnOff;
        if(OnOff) this->ConnectionDevice->slotSendMessage(MAP_COMMAND[LASER_BEAM_ON].toLatin1());
        else      this->ConnectionDevice->slotSendMessage(MAP_COMMAND[LASER_BEAM_OFF].toLatin1());
    };
    void setPilotEnable(bool OnOff) 
    { 
        qDebug() << this->TAG_NAME.toStdString().c_str() << "[PILOT ENABLE]" << OnOff;
        if(OnOff) this->ConnectionDevice->slotSendMessage(MAP_COMMAND[LASER_PILOT_ON].toLatin1()); 
        else      this->ConnectionDevice->slotSendMessage(MAP_COMMAND[LASER_PILOT_OFF].toLatin1()); 
    };

	void setPower(uint16_t Value)   
    { 
        qDebug() << this->TAG_NAME.toStdString().c_str() << "[SET POWER]" << Value;
        QTimer::singleShot(10 , [this,Value]() { setPowerEnable(false); });
        command = QString("SDC: %1\r").arg(Value);
        QTimer::singleShot(100, [this,Value]() { this->ConnectionDevice->slotSendMessage(command.toLatin1()); });
    };

    //========================================================
    //DEVICE_GENERIC_HANDLE_CONTROL
	void setLevel(uint32_t Level) override {if(Level == 1) setPowerLow(); if(Level == 2) setPowerMiddle(); if(Level == 3) setPowerHigh(); };
    void setValue(float Value) override { setPower(90*Value); }
	void setEnable(bool OnOff, uint16_t Number = 0) override
    {
        qDebug() << this->TAG_NAME << "[ CHANNEL ]" << Number << OnOff;
        if(Number == 0) setPowerEnable(OnOff); 
        if(Number == 1) setPilotEnable(OnOff); 
    }
    //========================================================

	void setPowerHigh()   { setPower(98); };
	void setPowerMiddle() { setPower(50); };
	void setPowerLow()    { setPower(15); };
	bool getState() { return true; }
private:
    QString command = QString("SDC: %1\r");
    std::map<uint8_t, QString> MAP_COMMAND
    {
     {LASER_BEAM_ON    , "EMOFF\r"},
     {LASER_BEAM_OFF   , "EMON\r"},
     {LASER_PILOT_ON   , "ABN\r"},
     {LASER_PILOT_OFF  , "ABF\r"},
     {LASER_FAULT_RESET, "RERR\r"},
    };       
};

template<typename T_CONNECTION, int NUM_DEVICE>
DeviceLaserInterface<T_CONNECTION,NUM_DEVICE>::~DeviceLaserInterface() { qDebug() << this->TAG_NAME << "DELETE"; }

template<typename T_CONNECTION, int NUM_DEVICE>
void DeviceLaserInterface<T_CONNECTION, NUM_DEVICE>::setParam(uint16_t ID, float Value)
{
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
  setReset();
}

#endif 
