#ifndef LASER_DATASTRUCTURES_H
#define LASER_DATASTRUCTURES_H
#include <QDebug>
#include <QDataStream>
struct HEADERStruct
{
    quint8 HEADER1 = 0xF1;
    quint8 HEADER2 = 0xC1;
    quint32 DataSize = 4;
    friend QDataStream& operator>>(QDataStream& Stream, HEADERStruct& HEADER)
    {
        Stream >> HEADER.HEADER1;
        Stream >> HEADER.HEADER2;
        Stream.skipRawData(2);
        Stream >> HEADER.DataSize;
        qDebug() <<"HEADER - " << Qt::hex << HEADER.HEADER1 << HEADER.HEADER2 << "size - " << HEADER.DataSize ;
        return Stream;
    }

    friend QDataStream& operator<<(QDataStream& stream, HEADERStruct& HEADER) { stream << HEADER.HEADER1 << HEADER.HEADER2 << HEADER.DataSize; return stream;};
    bool isValid() { return (HEADER1 == 0xF1);};
};

struct LaserStateStruct
{
    quint8  channel = 0;
    quint8  enable = 0;
    quint16 duty = 0; 
    quint16 ManualControl = 0; 

    friend void operator>>(QDataStream& Stream, LaserStateStruct& LaserState)
    {
        Stream >> LaserState.channel;
        Stream >> LaserState.enable;
        Stream >> LaserState.duty;
        Stream >> LaserState.ManualControl;

        qDebug() <<"CHANNEL: " << LaserState.channel
                 <<"enABLE: " << LaserState.enable
                 <<"DUTY: " << LaserState.duty
                 <<"MANUAL: " << LaserState.ManualControl;
    }
};

struct ControlCommandLaser
{
    ControlCommandLaser()
    {
        Header.HEADER1 = 0xF1;
        Header.HEADER2 = 0xC7;
        Header.DataSize = sizeof(ControlCommandLaser);
    }
    HEADERStruct Header;
    quint8  Channel = 0;
    quint8  enable = 0;
    quint16 Duty = 20;
    quint16 ManualControl = 0;

    friend QDataStream& operator<<(QDataStream& stream , ControlCommandLaser& laserCommand)
    {
        stream << laserCommand.Header << laserCommand.Channel << laserCommand.enable << laserCommand.Duty << laserCommand.ManualControl;
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream , ControlCommandLaser& laserCommand)
    {
        stream >> laserCommand.Header >> laserCommand.Channel >> laserCommand.enable >> laserCommand.Duty >> laserCommand.ManualControl;
        return stream;
    }
};

struct TestCommand
{
    TestCommand()
    {
        Header.HEADER1 = 0xF1;
        Header.HEADER2 = 0xC2;
        Header.DataSize = 6;
    }
    HEADERStruct Header;
    quint16 FLAG = 0xC2C4;
};
#endif // LASER_DATASTRUCTURES_H
