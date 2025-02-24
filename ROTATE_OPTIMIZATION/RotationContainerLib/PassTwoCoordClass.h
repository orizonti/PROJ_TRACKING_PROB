#ifndef PASSTWOCOORDCLASS_H
#define PASSTWOCOORDCLASS_H

#include <QPair>

std::pair<double, double> operator+(const std::pair<double, double>& x, const std::pair<double, double>& y);
std::pair<double, double> operator-(const std::pair<double, double>& x, const std::pair<double, double>& y);
QPair<double, double> operator+(const QPair<double, double>& x, const QPair<double, double>& y);
QPair<double, double> operator-(const QPair<double, double>& x, const QPair<double, double>& y);
QPair<double, double> operator*(const QPair<double, double>& x, double Scale);
QPair<double, double> operator+(const QPair<double, double>& x, double addition);
//QPair<double, double> operator/(const QPair<double, double>& x, double Scale);
void operator+=(QPair<double, double>& x, const QPair<double, double>& y);
void operator-=(QPair<double, double>& x, const QPair<double, double>& y);


class PassTwoCoordClass
{
public:
	PassTwoCoordClass* Link = 0;

	virtual const QPair<double, double>& GetOutput() = 0;
	virtual void SetInput(const QPair<double,double>& Coord) = 0;
	void SetLink(PassTwoCoordClass* NewLink);
	
	friend QPair<double,double>& operator >>(PassTwoCoordClass& Sender, QPair<double, double>& Coord);
    friend PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender, PassTwoCoordClass& Reciever);
	friend PassTwoCoordClass& operator >>(QPair<double,double> Coord, PassTwoCoordClass& Reciever);
	friend std::pair<double,double>& operator >>(PassTwoCoordClass& Sender, std::pair<double,double>& Coord);
	friend PassTwoCoordClass& operator >>(std::pair<double, double> Coord, PassTwoCoordClass& Reciever);
};
#endif //PASSTWOCOORDCLASS_H
