#ifndef PassCoordClass<double>_H
#define PassCoordClass<double>_H

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


class PassCoordClass<double>
{
public:
	PassCoordClass<double>* Link = 0;

	virtual const QPair<double, double>& GetOutput() = 0;
	virtual void SetInput(const QPair<double,double>& Coord) = 0;
	void SetLink(PassCoordClass<double>* NewLink);
	
	friend QPair<double,double>& operator >>(PassCoordClass<double>& Sender, QPair<double, double>& Coord);
    friend PassCoordClass<double>& operator >>(PassCoordClass<double>& Sender, PassCoordClass<double>& Reciever);
	friend PassCoordClass<double>& operator >>(QPair<double,double> Coord, PassCoordClass<double>& Reciever);
	friend std::pair<double,double>& operator >>(PassCoordClass<double>& Sender, std::pair<double,double>& Coord);
	friend PassCoordClass<double>& operator >>(std::pair<double, double> Coord, PassCoordClass<double>& Reciever);
};
#endif //PassCoordClass<double>_H
