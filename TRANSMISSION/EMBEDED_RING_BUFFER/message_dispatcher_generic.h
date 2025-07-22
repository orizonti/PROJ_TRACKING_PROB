#ifndef MESSAGE_DISPATCHER_GENERIC_H
#define MESSAGE_DISPATCHER_GENERIC_H

#include "message_command_structures.h"
#include "message_struct_generic.h"



//LIST_TYPE STRUCT TYPE THAT CONTAINS POINTERS TO ALL MESSAGE TYPE
template <typename T> struct DATA_TYPE_TAG{};
template<typename LIST_TYPE>
class MessageDispatcherGenericClass 
{
public:
    MessageDispatcherGenericClass(){};
    MessageDispatcherGenericClass(LIST_TYPE List) { MessagesList = List;};
    //template<typename RingBufferType, typename ListFunction> void DispatchNextMessage(RingBufferType& RingBuffer);
    template<typename RingBufferType> void DispatchNextMessage(RingBufferType& RingBuffer);
    template<typename RingBufferType> friend void operator|(RingBufferType& RingBuffer, MessageDispatcherGenericClass<LIST_TYPE>& Processor) { Processor.DispatchNextMessage(RingBuffer); }
    template<typename RingBufferType> void LinkToBuffer(RingBufferType& RingBuffer) { this->DispatchNextMessage(RingBuffer); }
    void SetDispatchList(LIST_TYPE MessageDispatchList) { MessagesList = MessageDispatchList;};

    template<typename DATA_TYPE, typename MESSAGE_TYPE> DATA_TYPE* ExtractData(MESSAGE_TYPE* Message, const DATA_TYPE_TAG<DATA_TYPE>& TAG)
    {
        return &(reinterpret_cast<MessageStructGeneric<DATA_TYPE,MESSAGE_HEADER_GENERIC>*>(Message)->DATA);
    };
    LIST_TYPE MessagesList;
};


template<> template<typename RingBufferType> 
void MessageDispatcherGenericClass<MessageListDispatcher>::DispatchNextMessage(RingBufferType& RingBuffer)
{
    if(!RingBuffer.isMessageAvailable()) return;
    auto& Message = RingBuffer.TakeMessage();
    qDebug() << "MESSAGE: " << Message.HEADER.MESSAGE_IDENT << "AVAILABLE: " << RingBuffer.isMessageAvailable();

    switch(Message.HEADER.MESSAGE_IDENT)
    {
        case (int)MessageTypes::MessageType1: 
           MessagesList.Message1 = ExtractData(&Message,DATA_TYPE_TAG<ControlMessage1>()); 
        if(MessagesList.Call1 != nullptr) MessagesList.Call1(MessagesList.Message1); //MessagesList.Message1->PrintMessage(); 
        break;
        case (int)MessageTypes::MessageType2: 
           MessagesList.Message2 = ExtractData(&Message,DATA_TYPE_TAG<ControlMessage2>()); 
        if(MessagesList.Call2 != nullptr) MessagesList.Call2(MessagesList.Message2); //MessagesList.Message2->PrintMessage(); 
        break;
        case (int)MessageTypes::MessageType3: 
           MessagesList.Message3 = ExtractData(&Message,DATA_TYPE_TAG<ControlMessage3>()); 
        if(MessagesList.Call3 != nullptr) MessagesList.Call3(MessagesList.Message3); //MessagesList.Message3->PrintMessage(); 
        break;
        case (int)MessageTypes::MessageType4: 
           MessagesList.Message4 = ExtractData(&Message,DATA_TYPE_TAG<ControlMessage4>()); 
        if(MessagesList.Call4 != nullptr) MessagesList.Call4(MessagesList.Message4); //MessagesList.Message4->PrintMessage(); 
        break;
        case (int)MessageTypes::MessageType5: 
           MessagesList.Message5 = ExtractData(&Message,DATA_TYPE_TAG<ControlMessage5>()); 
        if(MessagesList.Call5 != nullptr) MessagesList.Call5(MessagesList.Message5); //MessagesList.Message5->PrintMessage(); 
        break;
        case (int)MessageTypes::MessageType6: 
           MessagesList.Message6 = ExtractData(&Message,DATA_TYPE_TAG<ControlMessage6>()); 
        if(MessagesList.Call6 != nullptr) MessagesList.Call6(MessagesList.Message6); MessagesList.Message6->PrintMessage(); 
        break;
        default: qDebug() << "UNREGISTERED MESSAGE";
        break;
    }

    if(RingBuffer.isMessageAvailable()) DispatchNextMessage(RingBuffer);
}


//template<typename M, typename LIST_TYPE>  //WHY I CANT PUT FRIenD METHOD DEFINITION OUT OFF CLASS ???
//void operator|(RingBufferengine<M>& RingBuffer, MessageDispatcherGenericClass<LIST_TYPE>& Processor) 
//{ Processor.DispatchNextMessage(RingBuffer); }


#endif //MESSAGE_DISPATCHER_GENERIC_H


