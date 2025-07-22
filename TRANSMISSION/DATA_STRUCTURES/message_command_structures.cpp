#include "message_command_structures.h"
#include <QDebug>

void ControlMessage1::PrintMessage() { qDebug() << "MESSAGE1: "; }
void ControlMessage2::PrintMessage() { qDebug() << "MESSAGE2: "; }
void ControlMessage3::PrintMessage() { qDebug() << "MESSAGE3: "; }
void ControlMessage4::PrintMessage() { qDebug() << "MESSAGE4: "; }
void ControlMessage5::PrintMessage() { qDebug() << "MESSAGE5: "; }
void ControlMessage6::PrintMessage() { qDebug() << "TIME MEASURE NUMBER: " << Number; }

std::string ControlMessage1::toString() { std::stringstream ss; ss << "MESSAGE1: ";  return ss.str(); }
std::string ControlMessage2::toString() { std::stringstream ss; ss << "MESSAGE2: " ; return ss.str();}
std::string ControlMessage3::toString() { std::stringstream ss; ss << "MESSAGE3: " ; return ss.str();}
std::string ControlMessage4::toString() { std::stringstream ss; ss << "MESSAGE4: " ; return ss.str();}
std::string ControlMessage5::toString() { std::stringstream ss; ss << "MESSAGE5: " ; return ss.str();}
std::string ControlMessage6::toString() { std::stringstream ss; ss << "TIME MEASURE MESSAGE: " ; return ss.str();}

void operator>>(QDataStream& stream, MESSAGE_HEADER& Header)
{
     stream >> Header.HEADER; 
     stream >> Header.MESSAGE_IDENT; 
     stream >> Header.MESSAGE_TYPE; 
     stream >> Header.MESSAGE_NUMBER; 
     stream >> Header.DATA_SIZE; 
     stream >> Header.DATA_SIZE2; 
}

void operator<<(QDataStream& stream, MESSAGE_HEADER& Header)
{
     stream << Header.HEADER; 
     stream << Header.MESSAGE_TYPE; 
     stream << Header.MESSAGE_IDENT; 
     stream << Header.MESSAGE_NUMBER; 
     stream << Header.DATA_SIZE; 
     stream << Header.DATA_SIZE2; 
}

QByteArray MESSAGE_HEADER::toByteArray()
{
QByteArray  RawData;
QDataStream out_stream(&RawData, QIODevice::ReadWrite);
            out_stream.setByteOrder(QDataStream::LittleEndian);
out_stream << *this;
return RawData;
};