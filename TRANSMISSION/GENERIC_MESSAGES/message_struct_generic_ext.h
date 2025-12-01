#ifndef MESSAGE_STRUCT_ENGINE_H
#define MESSAGE_STRUCT_ENGINE_H

#include <QDataStream>
#include <QIODevice>

#include <algorithm>
#include <set>
#include "engine_type_register.h"
#include "message_command_structures.h"


template<typename T, typename H>
class MessageGenericExt
{
  public:
    MessageGenericExt();
	public:
    H HEADER;
    T DATA; 

  public:

  friend void operator>>(QDataStream& stream, MessageGenericExt<T,H>& Message);
  friend void operator<<(QDataStream& stream, MessageGenericExt<T,H>& Message);
  //template<typename M, typename P> friend void operator<<(QDataStream& stream, MessageGenericExt<M,P>& Message);
  QByteArray toByteArray();
     QString toString();
	      bool isMessasge() { return (HEADER.isValid()); };
         int GetSize()    { return this->toByteArray().size(); };
         int GetSizeFromHeader(){ return HEADER.DATA_SIZE + sizeof(H);};
  static int GetSizeStatic() {return sizeof(T) + sizeof(H);};

         MessageGenericExt<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageGenericExt<void*,H>*>(this);  }
};

template<typename T,typename H> 
MessageGenericExt<T,H>::MessageGenericExt()
{ 
  HEADER.DATA_SIZE = sizeof(T);
  HEADER.MESSAGE_IDENT = TypeRegister<T>::GetTypeID();
};


template<typename T,typename H> QString MessageGenericExt<T,H>::toString() { return QString("VAL: %1").arg(DATA); }
template<typename T, typename H>
QByteArray MessageGenericExt<T,H>::toByteArray()
{
QByteArray  RawData;
QDataStream out_stream(&RawData, QIODevice::ReadWrite);
            out_stream.setByteOrder(QDataStream::LittleEndian);
            out_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

HEADER.MESSAGE_IDENT = TypeRegister<T>::GetTypeID();
out_stream << this->HEADER;
out_stream.writeRawData(reinterpret_cast<char*>(&DATA), sizeof(T));
return RawData;
};


template<typename T, typename H>
void operator>>(QDataStream& stream, MessageGenericExt<T,H>& Message)
{
  stream >> Message.HEADER; 
  stream.readRawData(reinterpret_cast<char*>(&Message.DATA), sizeof(Message.DATA));
}

template<typename M, typename H> void operator<<(QDataStream& stream, MessageGenericExt<M,H>& Message)
{
  stream << Message.HEADER; 
  stream.writeRawData(reinterpret_cast<char*>(&Message.DATA), sizeof(Message.DATA));
}

template<typename H>
class MessageGenericExt<CommandSetPosJson,H>
{
  public:
    MessageGenericExt(){};
	public:
    H HEADER;
    CommandSetPosJson DATA; 

  public:

  friend void operator>>(QDataStream& stream, MessageGenericExt<CommandSetPosJson,H>& Message) { stream << Message.DATA.toByteArray(); }
  friend void operator<<(QDataStream& stream, MessageGenericExt<CommandSetPosJson,H>& Message) 
  { 
    stream.writeBytes(Message.DATA.toByteArray().data(), Message.DATA.toByteArray().size());
  };
  //template<typename M, typename P> friend void operator<<(QDataStream& stream, MessageGenericExt<M,P>& Message);
  QByteArray toByteArray(){ return DATA.toByteArray(); };
     QString toString();
	      bool isMessasge() { return true; };
         int GetSize()    { return this->toByteArray().size(); };
         int GetSizeFromHeader(){ return this->toByteArray().size();};
  static int GetSizeStatic() {return 0;};

         MessageGenericExt<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageGenericExt<void*,H>*>(this);  }
};



#endif // MESSAGE_STRUCT_ENGINE_H
