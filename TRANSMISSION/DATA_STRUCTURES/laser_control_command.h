#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H
#include <QDebug>
#include <QDataStream>

struct MessageLaserControl
{
    quint8  Channel = 0; 
    quint8  enable = 0;
    quint16 Duty = 20;
    quint16 ManualControl = 0;
};

#endif //DATASTRUCTURES_H
