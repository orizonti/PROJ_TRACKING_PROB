#ifndef PassCoordClass<float>_H
#define PassCoordClass<float>_H

#include <QPair>

std::pair<double, double> operator+(const std::pair<double, double>& x, const std::pair<double, double>& y);
std::pair<double, double> operator-(const std::pair<double, double>& x, const std::pair<double, double>& y);
QPair<float,float> operator+(const QPair<float,float>& x, const QPair<float,float>& y);
QPair<float,float> operator-(const QPair<float,float>& x, const QPair<float,float>& y);
QPair<float,float> operator*(const QPair<float,float>& x, double Scale);
QPair<float,float> operator+(const QPair<float,float>& x, double addition);
//QPair<float,float> operator/(const QPair<float,float>& x, double Scale);
void operator+=(QPair<float,float>& x, const QPair<float,float>& y);
void operator-=(QPair<float,float>& x, const QPair<float,float>& y);


class PassCoordClass<float>
{
public:
	PassCoordClass<float>* Link = 0;

	virtual const QPair<float,float>& getOutput() = 0;
	virtual void setInput(const QPair<float,float>& Coord) = 0;
	void setLink(PassCoordClass<float>* NewLink);
	
	friend QPair<float,float>& operator >>(PassCoordClass<float>& Sender, QPair<float,float>& Coord);
    friend PassCoordClass<float>& operator >>(PassCoordClass<float>& Sender, PassCoordClass<float>& Reciever);
	friend PassCoordClass<float>& operator >>(QPair<float,float> Coord, PassCoordClass<float>& Reciever);
	friend std::pair<double,double>& operator >>(PassCoordClass<float>& Sender, std::pair<double,double>& Coord);
	friend PassCoordClass<float>& operator >>(std::pair<double, double> Coord, PassCoordClass<float>& Reciever);
};
#endif //PassCoordClass<float>_H
