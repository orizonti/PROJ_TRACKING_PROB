#include "engine_type_register.h"

//std::map<long,long> MessagesMap;

std::map<int,QString> TypeTagRegister::TypesID;
std::map<int,int> TypeTagRegister::TypesSizes;
int TypeTagRegister::MaxTypeSize = 0;
int TypeTagRegister::MinTypeSize = 0;

template<> int TypeRegister<ControlMessage1>::GetTypeID() { return static_cast<int>(MessageTypes::MessageType1);}
template<> int TypeRegister<ControlMessage2>::GetTypeID() { return static_cast<int>(MessageTypes::MessageType2);}
template<> int TypeRegister<ControlMessage3>::GetTypeID() { return static_cast<int>(MessageTypes::MessageType3);}
template<> int TypeRegister<ControlMessage4>::GetTypeID() { return static_cast<int>(MessageTypes::MessageType4);}
template<> int TypeRegister<ControlMessage5>::GetTypeID() { return static_cast<int>(MessageTypes::MessageType5);}
template<> int TypeRegister<ControlMessage6>::GetTypeID() { return static_cast<int>(MessageTypes::MessageType6);}
