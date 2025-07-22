#pragma once
#include <stdint.h>
#include <string>
#include <sstream>
#include <QDataStream>
#include <QIODevice>
#include <QByteArray>
#include "laser_control_command.h"

class MESSAGE_HEADER_GENERIC
{
  public:
    uint16_t HEADER = 0x8220; 
    uint8_t  MESSAGE_TYPE  = 0xF1; 
    uint8_t  MESSAGE_IDENT = 1; 
    uint32_t MESSAGE_NUMBER = 0x0; 
    uint16_t DATA_SIZE = 1; 
    uint16_t DATA_SIZE2 = 1; 

    bool isValid() const { return HEADER == 0x8220 || HEADER == 0x2082; }
};

class MESSAGE_HEADER
{
  public:
    uint16_t HEADER = 0x8220; 
    uint8_t  MESSAGE_TYPE  = 0; 
    uint8_t  MESSAGE_IDENT = 0; 
    uint32_t MESSAGE_NUMBER = 0x0; 
    uint16_t DATA_SIZE = 1; 
    uint16_t DATA_SIZE2 = 1; 

    friend void operator>>(QDataStream& stream, MESSAGE_HEADER& Header);
    friend void operator<<(QDataStream& stream, MESSAGE_HEADER& Header);
    bool isValid() const { return HEADER == 0x8220 || HEADER == 0x2082; }
    QByteArray toByteArray();
};

struct AimStateStruct
{
    float Position = 0; 
    float Velocity = 0;
    float Acceleration = 0;
    float PositionRel = 0;

    void ClearStruct() { Position = 0; Velocity = 0; Acceleration = 0; PositionRel = 0;}
};

struct AimStateStructInt
{
public:
    uint16_t Position = 20; 
    uint16_t Velocity = 20;
    uint16_t Acceleration = 20;
    uint16_t PositionRel = 20;
};


struct ControlMessage1
{
    public:
    ControlMessage1() {};
    AimStateStruct StateChannel1;
    AimStateStruct StateChannel2;

    void PrintMessage();
    std::string toString();
};

struct ControlMessage2
{
    public:
    ControlMessage2() {};
    AimStateStructInt StateChannel1;
    AimStateStructInt StateChannel2;
    void PrintMessage();
    std::string toString();
};

struct ControlMessage3
{
    AimStateStruct StateChannel1;
    void PrintMessage();
    std::string toString();
};

struct ControlMessage4
{
    AimStateStructInt StateChannel1;
    void PrintMessage();
    std::string toString();
};

struct ControlMessage5
{
    uint8_t Command = 0; 
    void PrintMessage();
    std::string toString();
};

struct ControlMessage6
{
public:
    int Number = 0; 
    void PrintMessage();
    std::string toString();
};

typedef  ControlMessage1 MessageScanatorControl;
typedef  ControlMessage2 MessageScanatorControlInt;
typedef  ControlMessage3 MessageengineControl;
typedef  ControlMessage4 MessageengineControlInt;
typedef  ControlMessage5 MessageConnectionControl;
typedef  ControlMessage5 MessageConnectionControl;
typedef  ControlMessage6 MessageTimeMeasure;

enum class MessageTypes { MessageType1 = 1,
                          MessageType2 = 2, 
                          MessageType3 = 3,  
                          MessageType4 = 4,  
                          MessageType5 = 5,  
                          MessageType6 = 6,  
                          type_count};

struct MessageListDispatcher
{
   MessageListDispatcher(){};
   MessageListDispatcher(
        void (*DispatchCall1)(ControlMessage1*),
        void (*DispatchCall2)(ControlMessage2*),
        void (*DispatchCall3)(ControlMessage3*),
        void (*DispatchCall4)(ControlMessage4*),
        void (*DispatchCall5)(ControlMessage5*),
        void (*DispatchCall6)(ControlMessage6*)
   ): 
   Call1(DispatchCall1),
   Call2(DispatchCall2),
   Call3(DispatchCall3),
   Call4(DispatchCall4),
   Call5(DispatchCall5),
   Call6(DispatchCall6){};

ControlMessage1* Message1;
ControlMessage2* Message2;
ControlMessage3* Message3;
ControlMessage4* Message4;
ControlMessage5* Message5;
ControlMessage6* Message6;

void (*Call1)(ControlMessage1*) = nullptr;
void (*Call2)(ControlMessage2*) = nullptr;
void (*Call3)(ControlMessage3*) = nullptr;
void (*Call4)(ControlMessage4*) = nullptr;
void (*Call5)(ControlMessage5*) = nullptr;
void (*Call6)(ControlMessage6*) = nullptr;
};

