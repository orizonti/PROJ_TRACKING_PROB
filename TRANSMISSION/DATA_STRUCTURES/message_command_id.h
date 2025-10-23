#ifndef COMMAND_ID_H
#define COMMAND_ID_H
#include "message_command_structures.h"
#include "engine_type_register.h"
template<> constexpr int TypeRegister<CommandSetPosScanator    >::ID() { return 1; };
template<> constexpr int TypeRegister<CommandSetPosRotary      >::ID() { return 2; };
template<> constexpr int TypeRegister<CommandCalibration       >::ID() { return 3;}; 
template<> constexpr int TypeRegister<CommandDeviceController  >::ID() { return 4; };
template<> constexpr int TypeRegister<MessagePositionState<0>  >::ID() { return 5;}; 
template<> constexpr int TypeRegister<MessagePositionState<1>  >::ID() { return 6;}; 
template<> constexpr int TypeRegister<CommandCheckConnection   >::ID() { return 7;}; 
template<> constexpr int TypeRegister<MessageDeviceController  >::ID() { return 8; };
template<> constexpr int TypeRegister<CommandCloseConnection   >::ID() { return 9;}; 

template<> constexpr int TypeRegister<CommandDeviceLaserPower  >::ID() { return 0x110; };
template<> constexpr int TypeRegister<CommandDeviceLaserPointer>::ID() { return 0x120; };
template<> constexpr int TypeRegister<CommandDeviceFocusator   >::ID() { return 0x130; };

template<> constexpr int TypeRegister<MessageDeviceLaserPointer>::ID() { return 0x210; };
template<> constexpr int TypeRegister<MessageDeviceLaserPower  >::ID() { return 0x220; };
template<> constexpr int TypeRegister<MessageDeviceFocusator   >::ID() { return 0x230;}; 
#endif
