#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H

#include <QDebug>

#include "message_command_structures.h"
#include "engine_type_register.h"

#include <QByteArray>
#include <QObject>

class MessageDispatcherClass : public QObject
{
Q_OBJECT
public:

    template< typename RingBufferType> void DispatchNextMessage(RingBufferType& RingBuffer);

    template< typename RingBufferType>
    friend void operator|(RingBufferType& RingBuffer, MessageDispatcherClass& Processor) { Processor.DispatchNextMessage(RingBuffer); }

    template<typename RingBufferType>
    void LinkToBuffer(RingBufferType& RingBuffer) { this->DispatchNextMessage(RingBuffer); }

signals:
void SignalNewMessage(ControlMessage1* Message);
void SignalNewMessage(ControlMessage2* Message);
void SignalNewMessage(ControlMessage3* Message);
void SignalNewMessage(ControlMessage4* Message);
void SignalNewMessage(ControlMessage5* Message);
};

    
template<typename RingBufferType>
void MessageDispatcherClass::DispatchNextMessage(RingBufferType& RingBuffer)
{
    if(!RingBuffer.isMessageAvailable()) return;

    const auto& MessageReturn = RingBuffer.TakeMessage();

    //switch(MessageReturn.HEADER.MESSAGE_IDENT)
    //{
    //    case (int)MessageTypes::MessageType1: SignalNewMessage((ControlMessage1*)(&MessageReturn.DATA)); break;  
    //    case (int)MessageTypes::MessageType2: SignalNewMessage((ControlMessage2*)(&MessageReturn.DATA)); break;  
    //    case (int)MessageTypes::MessageType3: SignalNewMessage((ControlMessage3*)(&MessageReturn.DATA)); break;  
    //    case (int)MessageTypes::MessageType4: SignalNewMessage((ControlMessage4*)(&MessageReturn.DATA)); break;  
    //    case (int)MessageTypes::MessageType5: SignalNewMessage((ControlMessage5*)(&MessageReturn.DATA)); break;  
    //    default: qDebug() << "DISPATCHER UNREGISTERED MESSAGE: " << MessageReturn.HEADER.MESSAGE_IDENT;
    //    break;
    //}

    DispatchNextMessage(RingBuffer);
}


#endif 
