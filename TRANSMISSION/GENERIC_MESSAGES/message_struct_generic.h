#ifndef MESSAGE_STRUCT_ENGINE_H
#define MESSAGE_STRUCT_ENGINE_H

#include <QDataStream>
#include <QIODevice>

#include <algorithm>
#include <set>
#include "message_command_structures.h"
#include "engine_type_register.h"

//==========================================================


template<typename T, typename H>
class MessageStructGeneric
{
  public:
    MessageStructGeneric();
	public:
    H HEADER;
    T DATA; 

  public:

  friend void operator>>(QDataStream& stream, MessageStructGeneric<T,H>& Message);
  friend void operator<<(QDataStream& stream, MessageStructGeneric<T,H>& Message);
  //template<typename M, typename P> friend void operator<<(QDataStream& stream, MessageStructGeneric<M,P>& Message);
  QByteArray toByteArray();
     QString toString();
	      bool isMessasge() { return (HEADER.isValid()); };
         int GetSize()    { return this->toByteArray().size(); };
         int GetSizeFromHeader(){ return HEADER.DATA_SIZE + sizeof(H);};
  static int GetSizeStatic() {return sizeof(T) + sizeof(uint8_t) + sizeof(H);};

         MessageStructGeneric<void*,H>& toGenericMessage() { return *reinterpret_cast<MessageStructGeneric<void*,H>*>(this);  }

};

template<typename T,typename H> 
MessageStructGeneric<T,H>::MessageStructGeneric()
{ 
  HEADER.DATA_SIZE = sizeof(T);
  HEADER.MESSAGE_IDenT = TypeRegister<T>::GetTypeID();
  qDebug() << "CREATE MESSAGE: " << HEADER.MESSAGE_IDenT << "SIZE: " << HEADER.DATA_SIZE;
};


template<typename T,typename H> QString MessageStructGeneric<T,H>::toString() { return QString("VAL: %1").arg(DATA); }
template<typename T, typename H>
QByteArray MessageStructGeneric<T,H>::toByteArray()
{
QByteArray  RawData;
QDataStream out_stream(&RawData, QIODevice::ReadWrite);
            out_stream.setByteOrder(QDataStream::LittleEndian);
            out_stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
out_stream << this->HEADER;
out_stream.writeRawData(reinterpret_cast<char*>(&DATA), sizeof(T));
return RawData;
};



template<typename T, typename H>
void operator>>(QDataStream& stream, MessageStructGeneric<T,H>& Message)
{
  //const std::type_info& ti = typeid(T);
  //qDebug() << "COMMAND REC: " << ti.name();
  stream >> Message.HEADER; 
  stream.readRawData(reinterpret_cast<char*>(&Message.DATA), sizeof(Message.DATA));
}

template<typename M, typename H> void operator<<(QDataStream& stream, MessageStructGeneric<M,H>& Message)
{
  stream << Message.HEADER; 
  stream.writeRawData(reinterpret_cast<char*>(&Message.DATA), sizeof(Message.DATA));
}


#endif // MESSAGE_STRUCT_enGINE_H
