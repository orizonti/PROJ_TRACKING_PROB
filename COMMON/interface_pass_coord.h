#pragma once
#include <QPair>
#include <memory>

std::pair<double, double> operator+(const std::pair<double, double>& x, const std::pair<double, double>& y);
std::pair<double, double> operator-(const std::pair<double, double>& x, const std::pair<double, double>& y);

QPair<double, double> operator+(const QPair<double, double>& x, const QPair<double, double>& y);
QPair<double, double> operator-(const QPair<double, double>& x, const QPair<double, double>& y);
QPair<double, double> operator+(const QPair<double, double>& x, double addition);

QPair<double, double> operator*(const QPair<double, double>& x, double Scale);
QPair<double, double> operator/(const QPair<double, double>& x, double Scale);

void operator+=(QPair<double, double>& x, const QPair<double, double>& y);
void operator-=(QPair<double, double>& x, const QPair<double, double>& y);


class PassTwoCoordClass
{
public:
	PassTwoCoordClass* Link = nullptr;
	bool isLinked() { return Link != nullptr;}

	virtual const QPair<double, double>& GetOutput() = 0;
	virtual void SetInput(const QPair<double, double>& Coord) = 0;
	void SetLink(PassTwoCoordClass* NewLink);
	void PassCoord() { *this >> *Link; }
	
	friend PassTwoCoordClass& operator >>(const     QPair<double,double>&  Coord, PassTwoCoordClass& Reciever);
	friend PassTwoCoordClass& operator >>(const std::pair<double, double>& Coord, PassTwoCoordClass& Reciever);

	friend PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender, std::pair<double,double>& Coord);
	friend PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender,     QPair<double, double>& Coord);

    friend PassTwoCoordClass& operator >>(PassTwoCoordClass& Sender, PassTwoCoordClass& Reciever);
    friend PassTwoCoordClass& operator | (PassTwoCoordClass& Sender, PassTwoCoordClass& Reciever);

    friend std::shared_ptr<PassTwoCoordClass>  operator | (std::shared_ptr<PassTwoCoordClass> Sender, 
	                                                       std::shared_ptr<PassTwoCoordClass> Reciever);
    friend PassTwoCoordClass& operator | (std::shared_ptr<PassTwoCoordClass> Sender, PassTwoCoordClass& Reciever);


};
