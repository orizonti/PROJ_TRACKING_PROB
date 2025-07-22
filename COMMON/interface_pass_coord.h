#pragma once
#include <QPair>
#include <memory>
#include <vector>
#include <functional>
#include <QDebug>

template<typename V>
class PassCoordClass
{
public:
	std::vector<PassCoordClass*> NodesLinked ;
	bool isLinked() { return !NodesLinked.empty();}
    QPair<V,V> OutputCoord{0,0};
    bool PassBlocked = false;

	virtual const QPair<V, V>& GetOutput() { return OutputCoord;};
	virtual void SetInput(const QPair<V, V>& Coord) {OutputCoord = Coord;};

    void SetLink(PassCoordClass<V>* NewLink) { NodesLinked.push_back(NewLink); }
	void PassCoord() { if(!isLinked() || PassBlocked) return; for(auto& link: NodesLinked) {*this >> *link;} }
	
	virtual QPair<V,V>& operator >>(QPair<V, V>& Coord) { Coord = GetOutput(); return Coord;}
	virtual QPair<int,int>& operator >>(QPair<int, int>& Coord) { Coord = GetOutput(); return Coord;}

    virtual PassCoordClass& operator >>(PassCoordClass& Reciever)
    { 
        Reciever.SetInput(GetOutput()); if(Reciever.isLinked()) Reciever.PassCoord(); return Reciever; 
    }

	friend PassCoordClass& operator >>(const     QPair<V,V>&  Coord, PassCoordClass& Reciever)
    { Reciever.SetInput(Coord); if(Reciever.isLinked()) Reciever.PassCoord(); return Reciever; } 

    friend PassCoordClass& operator | (PassCoordClass& Sender, PassCoordClass& Reciever)
    { Sender.SetLink(&Reciever); return Reciever; }

    friend std::shared_ptr<PassCoordClass>  operator | (std::shared_ptr<PassCoordClass> Sender, 
	                                                    std::shared_ptr<PassCoordClass> Reciever)
    { Sender->SetLink(Reciever.get()); return Reciever; }

    friend PassCoordClass& operator | (std::shared_ptr<PassCoordClass> Sender, PassCoordClass& Reciever)
    { Sender->SetLink(&Reciever); return Reciever; }

};


template<typename T>
QPair<T, T> operator*(QPair<T, T>& x,const QPair<T, T>& y) { return QPair<T,T>(x.first * y.first, x.second * y.second); }
template<typename T>
QPair<T, T> operator*(const QPair<T, T>& x, T Scale)       { return QPair<T,T>(x.first * Scale, x.second * Scale); }
template<typename T>
QPair<T, T> operator+(const QPair<T, T>& x, T addition)    { return QPair<T,T>(x.first + addition, x.second * addition); }
template<typename T>
QPair<T, T> operator/(const QPair<T, T>& x, T Scale)       { return QPair<T,T>(x.first / Scale, x.second / Scale); }
template<typename T>
void operator+=(QPair<T, T> x, const QPair<T, T>& y)      { x.first = x.first + y.first; x.second = x.second + y.second; }
template<typename T>
void operator-=(QPair<T, T> x, const QPair<T, T>& y)      { x.first = x.first - y.first; x.second = x.second - y.second; }

template<typename T>
std::pair<T, T> operator+(const std::pair<T, T>& x, const std::pair<T, T>& y)
{
    return std::make_pair(x.first + y.first, x.second + y.second);
}

template<typename T>
std::pair<T, T> operator-(const std::pair<T, T>& x, const std::pair<T, T>& y)
{
    return std::make_pair(x.first - y.first, x.second - y.second);
}

template <class T>
class PortAdapter : public PassCoordClass<double>
{
public:
    PortAdapter(){};
    void LinkAdapter(T* LinkObject,std::function<void(T&,QPair<double,double>)> SetFunction, std::function<QPair<double,double>(T&)> GetFunction)
    { InputFunction = SetFunction; OutputFunction = GetFunction; Receiver = LinkObject; };

    T* Receiver =  nullptr;

    std::function<void(T&,QPair<double,double>)> InputFunction = nullptr;
    std::function<QPair<double,double>(T&)> OutputFunction = nullptr;

    void SetInput(const QPair<double,double>& coord) { if(InputFunction != nullptr) InputFunction(*Receiver,coord);};
    const QPair<double,double>& GetOutput() { if(OutputFunction != nullptr) PassCoordClass<double>::OutputCoord = OutputFunction(*Receiver); 
                                                           return PassCoordClass<double>::OutputCoord;};

};