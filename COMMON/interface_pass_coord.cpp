#include "interface_pass_coord.h"
#include <QDebug>


PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender, PassTwoCoordClass& Reciever) 
{ 
    Reciever.SetInput(Sender.GetOutput()); if(Reciever.isLinked()) Reciever.PassCoord(); 
    return Reciever; 
}

PassTwoCoordClass& operator >>(const QPair<double, double>& Coord, PassTwoCoordClass& Reciever) 
{ 
    Reciever.SetInput(Coord); if(Reciever.isLinked()) Reciever.PassCoord(); return Reciever; 
}
PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender, QPair<double,double>& Coord) 
{ 
    Coord = Sender.GetOutput(); return Sender; 
}

void PassTwoCoordClass::SetLink(PassTwoCoordClass* NewLink) { Link = NewLink; }
PassTwoCoordClass& operator | (PassTwoCoordClass& Sender, PassTwoCoordClass& Reciever) 
                                { 
                                    qDebug() << "LINK: " << Sender.GetID()<< " TO " << Reciever.GetID() << "PTR";
                                    Sender.SetLink(&Reciever); return Reciever; 
                                }

 std::shared_ptr<PassTwoCoordClass> operator | (std::shared_ptr<PassTwoCoordClass> Sender, 
                               std::shared_ptr<PassTwoCoordClass> Reciever)
                               {
                                qDebug() << "LINK: " << Sender->GetID()<< " TO " << Reciever->GetID();
                                Sender->SetLink(Reciever.get()); return Reciever; 
                               }

QPair<double, double> operator*(QPair<double, double>& x,const QPair<double, double>& y) { return QPair<double,double>(x.first * y.first, x.second * y.second); }
QPair<double, double> operator*(const QPair<double, double>& x, double Scale)            { return QPair<double,double>(x.first * Scale, x.second * Scale); }
QPair<double, double> operator+(const QPair<double, double>& x, double addition)         { return QPair<double,double>(x.first + addition, x.second * addition); }
QPair<double, double> operator/(const QPair<double, double>& x, double Scale)            { return QPair<double,double>(x.first / Scale, x.second / Scale); }
void operator+=(QPair<double, double>& x, const QPair<double, double>& y)                { x.first = x.first + y.first; x.second = x.second + y.second; }
void operator-=(QPair<double, double>& x, const QPair<double, double>& y)                { x.first = x.first - y.first; x.second = x.second - y.second; }

std::pair<double, double> operator+(const std::pair<double, double>& x, const std::pair<double, double>& y)
{
    return std::make_pair(x.first + y.first, x.second + y.second);
}

std::pair<double, double> operator-(const std::pair<double, double>& x, const std::pair<double, double>& y)
{
    return std::make_pair(x.first - y.first, x.second - y.second);
}

//QPair<double, double> operator+(const QPair<double, double>& x, const QPair<double, double>& y)
//{
//    return QPair<double,double>(x.first + y.first, x.second + y.second);
//}
//
//QPair<double, double> operator-(const QPair<double, double>& x, const QPair<double, double>& y)
//{
//    return QPair<double,double>(x.first - y.first, x.second - y.second);
//}
//
//PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender, std::pair<double,double>& Coord)
//{
//	Coord.first = Sender.GetOutput().first;
//	Coord.second = Sender.GetOutput().second;
//	return Sender;
//}
//
//PassTwoCoordClass& operator >>(std::pair<double, double> Coord, PassTwoCoordClass& Reciever)
//{
//
//	Reciever.SetInput(const QPair<double,double>&(Coord.first,Coord.second));
//	return Reciever;
//}