#ifndef RING_BUFFER_Dense_H
#define RING_BUFFER_Dense_H

#include "message_struct_generic.h"
#include <cstring>
#include "engine_type_register.h"

//template<typename H> class MessageIteratorDense;
class DataSourceClass
{
	public:
	virtual     int  GetData(uint8_t NeedDataAmount, uint8_t* InputBuffer) = 0;
	virtual uint8_t* GetData(uint8_t NeedDataAmount) = 0;
	virtual int& DataAvailable() = 0;
	virtual void FlushData() = 0;
};

template<typename H>
class MessageIteratorDense {

public:
  MessageIteratorDense();
  MessageIteratorDense(uint8_t* STORAGE, std::size_t Size);
  struct MessagesRange
  {
  	uint8_t* EndMessageBuffer;
  	uint8_t* LastMessage;
  };
  
  MessagesRange RangeLimits;
  uint32_t Messagenumber = 0;
  
  private:
  uint8_t* PtrMessageBegin;
  uint8_t* PtrMessageBeginPrevious;
  uint8_t* PtrDataend = 0;
  
  uint8_t* PtrBufferBegin;
  uint8_t* PtrBufferend;
  
  uint8_t  WaitDataAmount;

public:
  void PrintIterator();
  
  bool IsHeaderValid();
  bool IsMemoryatEnd();
  void MoveDataToBegin();
  
  int  StepsTo(const MessageIteratorDense<H>& It) { return It.Messagenumber - Messagenumber;}; 
  
  void ResetIterator();
  
  int DataAvailable();
  int MemoryAvailable();
  int MessageSize();
  H&  GetHeader();
  H&  GetHeader(uint8_t* PtrHeader);
  
  MessageStructGeneric<void*,H>* GetMessagePtr();
  MessageStructGeneric<void*,H>& operator*();
  
  MessageIteratorDense<H> operator++(int);
  
  void operator<<(DataSourceClass& DataSource);
  void operator=(const MessageIteratorDense& Message);
  bool operator==(MessageIteratorDense& Message);
  bool operator!=(MessageIteratorDense& Message);
  
  void LoadData(uint8_t* DataSourceBuffer, uint8_t BytesCountReceived);
  
  int MAX_MESSAGE_SIZE = 50;
};


template<typename H>
MessageIteratorDense<H>::MessageIteratorDense()
{
	PtrBufferBegin = 0; PtrBufferend = 0; PtrMessageBegin = 0; PtrDataend = 0;
	MAX_MESSAGE_SIZE = TypeRegister<>::GetMaxTypeSize() + sizeof(H);
}

template<typename H>
MessageIteratorDense<H>::MessageIteratorDense(uint8_t* STORAGE, std::size_t Size)
{
	PtrBufferBegin = STORAGE; PtrBufferend = STORAGE + Size;
	PtrMessageBegin = PtrBufferBegin; PtrDataend = PtrMessageBegin;
	RangeLimits.EndMessageBuffer = PtrBufferend;
	RangeLimits.LastMessage = PtrBufferend;
	MAX_MESSAGE_SIZE = TypeRegister<>::GetMaxTypeSize() + sizeof(H);
	qDebug() << "ITERATOR Dense MAX MESSAGE SIZE: " << MAX_MESSAGE_SIZE;
}


template<typename H>
MessageIteratorDense<H> MessageIteratorDense<H>::operator++(int) 
{
 if(PtrMessageBegin == RangeLimits.LastMessage) return *this;

 auto& Header = GetHeader(); if(!Header.isValid()) { ResetIterator(); return *this;};

 Messagenumber++;
 PtrMessageBegin += sizeof(H) + Header.DATA_SIZE; 

 if(PtrMessageBegin == RangeLimits.EndMessageBuffer) 
 PtrMessageBegin = PtrBufferBegin; 

 return* this;
}

template<typename H>
void MessageIteratorDense<H>::MoveDataToBegin()
{
	uint8_t DataRemain = DataAvailable();
	PtrDataend = PtrBufferBegin + DataRemain;
	std::memcpy(PtrBufferBegin,PtrMessageBegin,DataRemain); 
	PtrMessageBegin = PtrBufferBegin;
}

template<typename H>
void MessageIteratorDense<H>::LoadData(uint8_t* DataSourceBuffer, uint8_t BytesCountReceived)
{
	std::memcpy(PtrDataend,DataSourceBuffer,BytesCountReceived); PtrDataend += BytesCountReceived;

  while(DataAvailable() >= MessageSize())
	{
        if(!IsHeaderValid()) {qDebug() << "HEADER NOT VALID"; ResetIterator(); return; }

        qDebug()<< "ADD MESSAGE SIZE: " << MessageSize();
		PtrMessageBeginPrevious = PtrMessageBegin; 
		PtrMessageBegin += MessageSize(); RangeLimits.LastMessage = PtrMessageBegin;
	    Messagenumber++;

		if(IsMemoryatEnd()) 
		{
			RangeLimits.EndMessageBuffer = PtrMessageBegin;
			MoveDataToBegin();
		} 
		if(DataAvailable() < sizeof(H)) return;

	}

}


template<typename H>
void MessageIteratorDense<H>::operator=(const MessageIteratorDense<H>& Message)
{
	PtrMessageBegin = Message.PtrMessageBegin; 
	     PtrDataend = Message.PtrDataend;
	 PtrBufferBegin = Message.PtrBufferBegin;
 	   PtrBufferend = Message.PtrBufferend;
	   RangeLimits.EndMessageBuffer = Message.RangeLimits.EndMessageBuffer;
	   RangeLimits.LastMessage      = Message.RangeLimits.LastMessage;
}


template<typename H> void MessageIteratorDense<H>::operator<<(DataSourceClass& DataSource) { }

template<typename H> bool MessageIteratorDense<H>::operator!=(MessageIteratorDense<H>& Message) { return !(Message == *this); }
template<typename H> bool MessageIteratorDense<H>::operator==(MessageIteratorDense<H>& Message) { return PtrMessageBegin == Message.PtrMessageBegin; }

template<typename H> bool MessageIteratorDense<H>::IsHeaderValid() { return GetHeader().isValid(); }
template<typename H>   H& MessageIteratorDense<H>::GetHeader()     { return *reinterpret_cast<H*>(PtrMessageBegin); }
template<typename H>   H& MessageIteratorDense<H>::GetHeader(uint8_t* PtrHeader) { return *reinterpret_cast<H*>(PtrHeader); }

template<typename H> int MessageIteratorDense<H>::MessageSize()      
{ 
	auto& Header = GetHeader(); 
	if(!Header.isValid()) return 0; 
						  return Header.DATA_SIZE + sizeof(H);
};

template<typename H> bool MessageIteratorDense<H>::IsMemoryatEnd()  { return (PtrBufferend - PtrMessageBegin) < 2*MAX_MESSAGE_SIZE; }
template<typename H> int MessageIteratorDense<H>::MemoryAvailable() { return (PtrBufferend - PtrMessageBegin) - 2*MAX_MESSAGE_SIZE; }
template<typename H> int MessageIteratorDense<H>::DataAvailable()   { return std::abs(PtrDataend - PtrMessageBegin); }

template<typename H> void MessageIteratorDense<H>::ResetIterator()  { PtrMessageBegin = PtrBufferBegin; PtrDataend = PtrBufferBegin; }


template<typename H> 
MessageStructGeneric<void*,H>& MessageIteratorDense<H>::operator*() { return *reinterpret_cast<MessageStructGeneric<void*,H>* >(PtrMessageBegin); }
template<typename H> 
MessageStructGeneric<void*,H>* MessageIteratorDense<H>::GetMessagePtr() { return reinterpret_cast<MessageStructGeneric<void*,H>* >(PtrMessageBegin); }


template<typename H>
void MessageIteratorDense<H>::PrintIterator()
{
qDebug() << "HEADER: " << IsHeaderValid() << "BEGIN: " << PtrMessageBegin << "BEGIN REL: " << PtrMessageBegin - PtrBufferBegin;
}
//====================================================================================================================

template< typename H, std::size_t S_M, std::size_t N_M> 
class RingBufferDense
{
    public:
    RingBufferDense();
    ~RingBufferDense();
  uint8_t* DATA;
	std::size_t  BufferSize = S_M*N_M;

	MessageIteratorDense<H> MessagePointer;
	MessageIteratorDense<H> IncommingPointer;

  MessageStructGeneric<void*,H>* CurrentMessage = 0;

    bool isMessageAvailable();
    void AppendData(uint8_t* Data, uint8_t Size);
    MessageStructGeneric<void*,H>* TakeMessagePtr();
	  MessageStructGeneric<void*,H>& TakeMessage();

    int CountMessagesInStore() { return MESSAGE_COUNTER;}
    int MESSAGE_COUNTER = 0;
    int MAX_MESSAGE_IN_STORE = 40;
    int MAX_MESSAGE_SIZE = S_M;
    int MIN_MESSAGE_SIZE = S_M/2;
};

template<typename H, std::size_t S_M, std::size_t N_M>
RingBufferDense<H,S_M,N_M>::RingBufferDense()
{
  DATA = new uint8_t[S_M*N_M]; 
  MessagePointer = MessageIteratorDense<H>(DATA,S_M*N_M);  
  IncommingPointer = MessageIteratorDense<H>(DATA,S_M*N_M);  

  MAX_MESSAGE_IN_STORE = N_M - 2;
  qDebug() << "CREATE MESSAGE RING MESSAGE NUMBER: " << N_M 
           << "MAX MESSAGE: " << MAX_MESSAGE_SIZE
           << "MIN MESSAGE: " << MIN_MESSAGE_SIZE
           << "SIZE IN BYTES: " << N_M*S_M;
}

template<typename H, std::size_t S_M, std::size_t N_M>
RingBufferDense<H,S_M,N_M>::~RingBufferDense() { delete DATA; }

template<typename H, std::size_t S_M, std::size_t N_M>
void RingBufferDense<H,S_M,N_M>::AppendData(uint8_t* Data, uint8_t Size)
{
   IncommingPointer.LoadData(Data, Size); 
   MessagePointer.RangeLimits = IncommingPointer.RangeLimits;
   MESSAGE_COUNTER = MessagePointer.StepsTo(IncommingPointer);

   qDebug() << "MESSAGE IN STORE: " << MESSAGE_COUNTER << "PASSED: " 
            << IncommingPointer.Messagenumber << "PROC: " 
            << MessagePointer.Messagenumber;

   if(MESSAGE_COUNTER > 12) MessagePointer++;
}

template<typename H, std::size_t S_M, std::size_t N_M>
MessageStructGeneric<void*,H>* RingBufferDense<H,S_M,N_M>::TakeMessagePtr()
{
  CurrentMessage = MessagePointer.GetMessagePtr(); if(!isMessageAvailable()) return CurrentMessage;

  MessagePointer++; MESSAGE_COUNTER  = MessagePointer.StepsTo(IncommingPointer); 
  
  if(IncommingPointer.Messagenumber > 10000) { IncommingPointer.Messagenumber -= MessagePointer.Messagenumber; MessagePointer.Messagenumber = 0;}
  return CurrentMessage;
}

template<typename H, std::size_t S_M, std::size_t N_M>
MessageStructGeneric<void*,H>& RingBufferDense<H,S_M,N_M>::TakeMessage()
{
  auto& Message = *MessagePointer; CurrentMessage = &Message; if(!isMessageAvailable()) return Message;

  MessagePointer++; MESSAGE_COUNTER  = MessagePointer.StepsTo(IncommingPointer); 

  if(IncommingPointer.Messagenumber > 10000) { IncommingPointer.Messagenumber -= MessagePointer.Messagenumber; MessagePointer.Messagenumber = 0;}
  return Message;
}

template<typename H, std::size_t S_M, std::size_t N_M>
bool RingBufferDense<H,S_M,N_M>::isMessageAvailable()
{
  return MessagePointer.StepsTo(IncommingPointer) > 0;
}

#endif //RING_BUFFER_Dense_H