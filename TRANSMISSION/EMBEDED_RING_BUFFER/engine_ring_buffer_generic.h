#ifndef RING_BUFFER_GENERIC_H
#define RING_BUFFER_GENERIC_H


#include "message_command_structures.h"
#include "engine_type_register.h"
#include "message_iterator_generic.h"
#include "message_struct_generic.h"
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

  MessageStructGeneric<void*,H>* CurrentMessage = 0;

    bool isMessageAvailable();
    void AppendData(uint8_t* Data, uint8_t Size);
    MessageStructGeneric<void*,H>* TakeMessagePtr();
	  MessageStructGeneric<void*,H>& TakeMessage();

    int CountMessagesInStore() { return MESSAGE_COUNTER;}

    int MESSAGE_COUNTER = 0;
    int MAX_MESSAGE_IN_STORE = M_N-2;

    int MAX_MESSAGE_SIZE = M_S;
    int MIN_MESSAGE_SIZE = M_S;

    template<typename T> void RegisterMessage(T MESSAGE);
};

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
RingBufferGeneric<H,M_S,M_N,I_T>::RingBufferGeneric()
{
  DATA = new uint8_t[BufferSize]; 

  qDebug() << "=================================================";
  qDebug() << "[ CREATE RING BUFFER FIXED ]";
  qDebug() << "MAX MESSAGE: " << MAX_MESSAGE_SIZE
           << "MESSAGE NUMBER: " << M_N
           << "ITERATION_TYPE:" << (int)I_T
           << "SIZE IN BYTES: " << BufferSize;

  MessagePointer = MessageIteratorGeneric<H,I_T>(DATA,M_S,M_N);  
  IncommingPointer = MessageIteratorGeneric<H,I_T>(DATA,M_S,M_N);  

	MAX_MESSAGE_SIZE = TypeRegister<>::GetMaxTypeSize() + sizeof(H);
	MIN_MESSAGE_SIZE = TypeRegister<>::GetMinTypeSize() + sizeof(H);
	qDebug() << "RING BUFFER GENERIC SIZE RANGE: "<< MIN_MESSAGE_SIZE << MAX_MESSAGE_SIZE;
  qDebug() << "=================================================";
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
RingBufferGeneric<H,M_S,M_N,I_T>::~RingBufferGeneric() { delete DATA; }

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
void RingBufferGeneric<H,M_S,M_N,I_T>::AppendData(uint8_t* Data, uint8_t Size)
{
   H* HEADER = (H*)Data; if(!HEADER->isValid()) { qDebug() << "HEADER NO VALID SKIP DATA" << Qt::hex << HEADER->HEADER; return; }

                                IncommingPointer.LoadData(Data, Size); 
   MessagePointer.RangeLimits = IncommingPointer.RangeLimits;
   MESSAGE_COUNTER = MessagePointer.StepsTo(IncommingPointer);

   //qDebug() << "MESSAGE NOT PROCESSED: " << MESSAGE_COUNTER 
   //         << "TOTAL: " << IncommingPointer.Messagenumber 
   //         << "PROCESSED: " << MessagePointer.Messagenumber;

     MessagePointer++;
   //if(MESSAGE_COUNTER > 8) MessagePointer++;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
bool RingBufferGeneric<H,M_S,M_N,I_T>::isMessageAvailable()
{
  //qDebug() << "[ MESSAGE COUNT ]" << MessagePointer.StepsTo(IncommingPointer);
  return MessagePointer.StepsTo(IncommingPointer) > 0;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
MessageStructGeneric<void*,H>* RingBufferGeneric<H,M_S,M_N,I_T>::TakeMessagePtr()
{
  qDebug() << "TAKE MESSAGE";
  CurrentMessage = MessagePointer.GetMessagePtr(); if(!isMessageAvailable()) return CurrentMessage;

  MessagePointer++; MESSAGE_COUNTER  = MessagePointer.StepsTo(IncommingPointer); 
  
  if(IncommingPointer.Messagenumber > 10000) { IncommingPointer.Messagenumber -= MessagePointer.Messagenumber; MessagePointer.Messagenumber = 0;}
  return CurrentMessage;
}

template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
MessageStructGeneric<void*,H>& RingBufferGeneric<H,M_S,M_N,I_T>::TakeMessage()
{
  qDebug() << "TAKE MESSAGE";
      auto& Message = *MessagePointer; CurrentMessage = &Message; if(!isMessageAvailable()) return Message;
                       MessagePointer++; 
    MESSAGE_COUNTER  = MessagePointer.StepsTo(IncommingPointer); 

  if(IncommingPointer.Messagenumber > 10000) { IncommingPointer.Messagenumber -= MessagePointer.Messagenumber; 
                                                                                 MessagePointer.Messagenumber = 0;}
  return Message;
}



template<typename H, size_t M_S, size_t M_N, IteratorMode I_T>
template<typename T>
void RingBufferGeneric<H,M_S,M_N,I_T>::RegisterMessage(T MESSAGE)
{
    if(sizeof(MessageStructGeneric<T,H>) < MIN_MESSAGE_SIZE)
                                           MIN_MESSAGE_SIZE = sizeof(MessageStructGeneric<T,H>);

    TypeRegister<T>::RegisterType();
	MAX_MESSAGE_SIZE = TypeRegister<>::GetMaxTypeSize() + sizeof(H);
	MIN_MESSAGE_SIZE = TypeRegister<>::GetMinTypeSize() + sizeof(H);

  qDebug() << "REGISTER MESSAGE SIZE: " << sizeof(MessageStructGeneric<T,H>) << "MIN SIZE: " << MIN_MESSAGE_SIZE;
}

//================================================================
template<typename T, size_t MOD>
class TestInterface
{
  public:
  void GetHeader() { qDebug() << "GET HEADER MOD: " << MOD;};
  void PrintHeader() { qDebug() << "PRINT: " << MOD << sizeof(T);};
};

template<typename T>
class TestInterface<T,2>
{
  public:
  void GetHeader();
  void PrintHeader();
};

template<typename T> void TestInterface<T,2>::GetHeader() { qDebug() << "GET HEADER MODER: " << 2;};
template<typename T> void TestInterface<T,2>::PrintHeader() { qDebug() << "PRINT MODER: " << 2 << sizeof(T);};

template<typename T, size_t MOD>
class Worker: public TestInterface<T,MOD>
{
  public:
  void Print() { TestInterface<T,MOD>::PrintHeader(); }
};

  //Check TakeMessage code
  ////=====================================================================
  //auto* MessagePtr = (MessageStructGeneric<ControlMessage2,MESSAGE_HEADER>*)(MessagePointer.GetMessagePtr());
  //auto* DataPtr = &MessagePtr->DATA;
  //qDebug() << "TAKE MESSAGE : " << QByteArray((char*)MessagePointer.GetMessagePtr(),32).toHex();
  //qDebug() << "TAKE MESSAGE2: " << MessagePtr->toByteArray().toHex();
  //qDebug() << "TAKE DATA    : " << QByteArray((char*)DataPtr,16).toHex();
  //      MessagePtr = (MessageStructGeneric<ControlMessage2,MESSAGE_HEADER>*)(&Message);
  //      DataPtr = &MessagePtr->DATA;
  //qDebug() << "CONF TAKE MESSAGE: " <<  MessagePtr->toByteArray().toHex();
  //qDebug() << "CONF TAKE DATA: " <<  QByteArray((char*)DataPtr,16).toHex();

  //=====================================================================
#endif //RING_BUFFER_GENERIC_H

