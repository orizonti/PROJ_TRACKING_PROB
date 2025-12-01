#pragma once
#include <stdint.h>
#include <string>
#include <sstream>
#include <QDataStream>
#include <QIODevice>
#include <QByteArray>
#include <QDebug>

template<int NUM_DEV>
struct CommandDevice
{
  public:
  uint16_t DeviceID = 0;
  uint16_t Command = 0;
  uint16_t Param = 0;
  void clearStruct() { std::memset(this,0,sizeof(CommandDevice)); }
};

using CommandDeviceController   = CommandDevice<0>;

template<int NUM_DEV>
struct CommandDeviceRedux
{
  public:
  uint8_t Command = 0;
  uint8_t Param = 0;

  uint8_t Reserv1 = 0;
  uint8_t Reserv2 = 0;
  uint8_t Reserv3 = 0;
  uint8_t Reserv4 = 0;
  uint8_t Reserv5 = 0;
  uint8_t Reserv6 = 0;
  void clearStruct() { std::memset(this,0,sizeof(CommandDeviceRedux)); }
};

using CommandDeviceLaserPower   = CommandDeviceRedux<0>;
using CommandDeviceLaserPointer = CommandDeviceRedux<1>;
using CommandDeviceFocusator    = CommandDeviceRedux<2>;

template<int NUM_DEV>
struct MessageDevice
{
   uint8_t DeviceID = 0;
   uint8_t Module = 0;
   uint16_t Param1 = 0;
   uint16_t Param2 = 0;
   uint16_t Param3 = 0;
};

using MessageDeviceController   = MessageDevice<0>;
using MessageDeviceLaserPower   = MessageDevice<1>;
using MessageDeviceLaserPointer = MessageDevice<2>;
using MessageDeviceFocusator    = MessageDevice<3>;


template<int N_CHAN>
struct CommandSetSpeed
{
   public:
   int16_t Param1 = 0;
   int16_t Param2 = 0;
   void clearStruct() { std::memset(this,0,sizeof(CommandSetSpeed)); }
};
using CommandSetSpeedRotary   = CommandSetSpeed<0>;
using CommandSetSpeedScanator = CommandSetSpeed<1>;

template<int N_CHAN>
struct MessageMoveState
{
   public:
   int16_t Param1 = 0;
   int16_t Param2 = 0;
   uint16_t Param3 = 1;
   uint16_t Param4 = 1;
};

using MessageStateRotary   = MessageMoveState<0>;
using MessageStateScanator = MessageMoveState<1>;

template<int N_CHAN>
struct MessagePositionState
{
    public:
    uint16_t Param1;
    uint16_t Param2;
    uint16_t Param3;
    uint16_t Param4;
};

using MessagePosStateRotary   = MessagePositionState<0>;
using MessagePosStateScanator = MessagePositionState<1>;

struct CommandCheckConnection  { uint8_t Connect  = 0xC5; uint8_t Connect2 = 0xC6; };
struct CommandCloseConnection  { uint8_t Close1   = 0xC6; uint8_t Close2   = 0xC6; };

struct CommandCalibration
{
    public:
    uint16_t  NodeType    = 0xA0;
    uint16_t  Command     = 0xA1;
    uint16_t  Channel     = 1;
    uint16_t  Amplitude   = 1;

    uint16_t  PeriodProcess = 1;
    uint16_t  TimeMeasure   = 1;
    uint16_t  NumberSteps   = 1;
    uint16_t  Reserve3      = 1;
};


#define LASER_CHECK 0x20
#define LASER_ON    0x22
#define LASER_OFF   0x26

#define LASER_SET_POWER 0x23
#define LASER_BEAM_ON   0x24
#define LASER_BEAM_OFF  0x25
#define LASER_PILOT_ON  0x27
#define LASER_PILOT_OFF 0x28

#define LASER_FAULT 0x39

#define LASER_MODULE       0 
#define LASER_MODULE_BEAM  1
#define LASER_MODULE_PILOT 2
#define LASER_MODULE_POWER 3

#include "arduino_json.h"
class CommandSetPosJson
{
  public:
  JsonDocument command;
  JsonDocument slave1; 
  JsonDocument slave2; 
  float Param1 = 0;
  float Param2 = 0;
  std::string json_doc;
  QByteArray array;

  template<typename T> void operator=(const QPair<T,T>& Pos) 
  { 
    Param1 = Pos.first; Param2 = Pos.second; 
    slave1["target"] = Param1;
    slave2["target"] = Param2;

    command["slaves"][0] = slave1;
    command["slaves"][1] = slave2;

    int size = serializeJson(command,json_doc); 
                    array.setRawData(json_doc.c_str(), size); 

    qDebug() << "ROTARY SET COMMAND" << json_doc.c_str(); 
  };

  CommandSetPosJson()
  {
   command["id"] = 123;
   command["dtg"] = 1111;
   
   slave1["name"] = "joint1";
   slave1["mode"] = "position";
   slave1["class"] = "TDrive";
   slave1["target"] = Param1;

   slave2["name"] = "joint2";
   slave2["mode"] = "position";
   slave2["class"] = "TDrive";
   slave2["target"] = Param2;

   command["slaves"][0] = slave1;
   command["slaves"][1] = slave2;
  }
  QByteArray toByteArray() { return array;};

};

template<int N_CHAN>
struct CommandSetPos
{
   public:
   int16_t Param1 = 0;
   int16_t Param2 = 0;
   void clearStruct() { std::memset(this,0,sizeof(CommandSetPos)); }
   template<typename T>
   void operator=(const QPair<T,T>& Pos) { Param1 = Pos.first; Param2 = Pos.second; };
   QPair<int,int> toPair() { return QPair<int,int>(Param1,Param2); }
};


using CommandSetPosRotary   = CommandSetPos<0>;
using CommandSetPosScanator = CommandSetPos<1>;

