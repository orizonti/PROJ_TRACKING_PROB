#ifndef RING_BUFFER_GENERIC_H
#define RING_BUFFER_GENERIC_H


#include "message_command_structures.h"
#include "engine_type_register.h"
//#include "engine_type_register_manual.h"
#include "message_iterator_generic.h"
#include "message_struct_generic.h"
#include "message_header_generic.h"
#include <cstring>
#include <typeinfo>
#include <QByteArray>

template< typename H, size_t M_S, size_t M_N, IteratorMode I_T = IteratorMode::ChunkedFixed> 
class RingBufferGeneric
{
    public:
    RingBufferGeneric();
    ~RingBufferGeneric();
  uint8_t* DATA;
	std::size_t  BufferSize = M_S*M_N;

	MessageIteratorGeneric<H,I_T> MessagePointer;
	MessageIteratorGeneric<H,I_T> IncommingPointer;

  MessageGeneric<void*,H>* CurrentMessage = 0;

    bool isMessageAvailable();
    void AppendData(uint8_t* Data, uint16_t Size);
    void Reset();
    MessageGeneric<void*,H>* TakeMessagePtr();
	  MessageGeneric<void*,H>& TakeMessage();

    int CountMessagesInStore() { return MESSAGE_COUNTER;}

    int DataInStore() { return DATA_AVAILABLE; };
    int DataPassed() { return DATA_PASSED; };

    int MESSAGE_COUNTER = 0;
    int MAX_MESSAGE_IN_STORE = M_N-2;

    int MAX_MESSAGE_SIZE = M_S;
    int MIN_MESSAGE_SIZE = M_S;

    int DATA_PASSED = 0;
    int DATA_AVAILABLE = 0;
};

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
RingBufferGeneric<H,M_S,M_N,I_T>::RingBufferGeneric()
{
  DATA = new uint8_t[BufferSize]; 

  MessagePointer = MessageIteratorGeneric<H,I_T>(DATA,M_S,M_N);  
  IncommingPointer = MessageIteratorGeneric<H,I_T>(DATA,M_S,M_N);  

	MAX_MESSAGE_SIZE = TypeRegister<>::GetMaxTypeSize() + sizeof(H);
	MIN_MESSAGE_SIZE = TypeRegister<>::GetMinTypeSize() + sizeof(H);
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
void RingBufferGeneric<H,M_S,M_N,I_T>::Reset()
{
    MessagePointer.ResetIterator();
    IncommingPointer.ResetIterator();
    MESSAGE_COUNTER = 0;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
RingBufferGeneric<H,M_S,M_N,I_T>::~RingBufferGeneric() { delete DATA; }

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
void RingBufferGeneric<H,M_S,M_N,I_T>::AppendData(uint8_t* Data, uint16_t Size)
{
   DATA_PASSED += Size;
   IncommingPointer.LoadData(Data, Size); 
   MessagePointer.RangeLimits = IncommingPointer.RangeLimits;
   MESSAGE_COUNTER = MessagePointer.StepsTo(IncommingPointer);

   //qDebug() << "MESSAGE IN STORE: " << MESSAGE_COUNTER << "PASSED: " 
   //         << IncommingPointer.MessageNumber << "PROC: " 
   //         << MessagePointer.MessageNumber;

   if(MESSAGE_COUNTER > 8) MessagePointer++;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
MessageGeneric<void*,H>* RingBufferGeneric<H,M_S,M_N,I_T>::TakeMessagePtr()
{
  CurrentMessage = MessagePointer.GetMessagePtr(); if(!isMessageAvailable()) return CurrentMessage;

  MessagePointer++; MESSAGE_COUNTER  = MessagePointer.StepsTo(IncommingPointer); 
  
  if(IncommingPointer.Messagenumber > 10000) 
   { IncommingPointer.Messagenumber -= MessagePointer.Messagenumber; 
                                       MessagePointer.Messagenumber = 0;}

  DATA_AVAILABLE = DATA_PASSED - CurrentMessage->GetSize();

  return CurrentMessage;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
MessageGeneric<void*,H>& RingBufferGeneric<H,M_S,M_N,I_T>::TakeMessage()
{
  auto& Message = *MessagePointer; CurrentMessage = &Message; if(!isMessageAvailable()) return Message;

  MessagePointer++; MESSAGE_COUNTER  = MessagePointer.StepsTo(IncommingPointer); 

  if(IncommingPointer.MessageNumber > 10000) 
    {IncommingPointer.MessageNumber -= MessagePointer.MessageNumber; 
                                       MessagePointer.MessageNumber = 0;}

  DATA_AVAILABLE = DATA_PASSED - CurrentMessage->GetSize();
  return Message;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
bool RingBufferGeneric<H,M_S,M_N,I_T>::isMessageAvailable()
{
  return MessagePointer.StepsTo(IncommingPointer) > 0;
}


#endif //RING_BUFFER_GENERIC_H

