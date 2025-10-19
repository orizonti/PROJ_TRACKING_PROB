#ifndef MESSAGE_DISPATCHER_EXT_H
#define MESSAGE_DISPATCHER_EXT_H

#include "message_command_structures.h"
#include "message_struct_generic.h"
#include "engine_type_register.h"
#include "message_header_generic.h"


template<typename H, typename RingBufferType>
class MessageDispatcher 
{
public:
    MessageDispatcher() { CallList.resize(30); std::fill(CallList.begin(), CallList.end(), nullptr);}
    using MessageCall   = std::function<void (MessageGeneric<void*, H>&)>;

    void DispatchNextMessage(RingBufferType& RingBuffer);

    friend void operator|(RingBufferType& RingBuffer, MessageDispatcher& Processor) 
                                                                       { Processor.DispatchNextMessage(RingBuffer); }

    void LinkToBuffer(RingBufferType& RingBuffer) { this->DispatchNextMessage(RingBuffer); }

    //void DispatchNextMessage(RingBufferType& RingBuffer);

    template<typename T>
    void AppendCallback(const MessageCall& Call)
    {
       if(CallList.size() < TypeRegister<>::TypeCount)
       {
          auto List = CallList; CallList.resize(TypeRegister<>::TypeCount+2); 
                      CallList.insert(CallList.begin(), List.begin(), List.end());
       }
       CallList[TypeRegister<T>::ID()] = Call;
       qDebug() << "APPEND CALLBACK TO : "<< typeid(T).name() << " ID:" << TypeRegister<T>::ID();
    }

    template< typename T, typename MessageType> 
    static T* ExtractData(MessageType* Message) {
    return &(reinterpret_cast<MessageGeneric<T,MESSAGE_HEADER_GENERIC>*>(Message)->DATA); 
    }

    std::vector<MessageCall> CallList;
};

template<typename H,typename RingBufferType> 
void MessageDispatcher<H,RingBufferType>::DispatchNextMessage(RingBufferType& RingBuffer)
{
                if(!RingBuffer.isMessageAvailable()) return;
    auto& Message = RingBuffer.TakeMessage();

    if(!TypeRegister<>::isTypeValid(Message.HEADER.MESSAGE_IDENT)) 
    {
    qDebug() << "MESSAGE NOT REGISTERED:" << Message.HEADER.MESSAGE_IDENT; return;
    }
    
    if(CallList[Message.HEADER.MESSAGE_IDENT] != nullptr && 
                Message.HEADER.MESSAGE_IDENT  <= CallList.size())
       CallList[Message.HEADER.MESSAGE_IDENT](Message);
    else
    qDebug() << "CALLBACK NOT REGISTERED TO MESSAGE:" << Message.HEADER.MESSAGE_IDENT;


    if(RingBuffer.isMessageAvailable()) DispatchNextMessage(RingBuffer);
}

template<int ID_NUM>
class CommandDispatcherGeneric
{
   public:
   static const int ID{ID_NUM};
   static void dispatchCommand(const QByteArray& Command) 
   {
      qDebug() << "[COMMAND DISPATCHER ] ID: " << ID << "COMMAND NOT REGISTERED";
   };
};


#endif //MESSAGE_DISPATCHER_EXT_H
