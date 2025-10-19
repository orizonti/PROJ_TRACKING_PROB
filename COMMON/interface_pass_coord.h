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

	virtual const QPair<V, V>& getOutput() { return OutputCoord;};
	virtual void setInput(const QPair<V, V>& Coord) {OutputCoord = Coord;};

    void setLink(PassCoordClass<V>* NewLink) { NodesLinked.push_back(NewLink); }
	void passCoord() { if(!isLinked() || PassBlocked) return; for(auto& link: NodesLinked) {*this >> *link;} }
	
	virtual QPair<V,V>& operator >>(QPair<V, V>& Coord) { Coord = getOutput(); return Coord;}

    virtual PassCoordClass& operator >>(PassCoordClass& Reciever)
    { 
        Reciever.setInput(getOutput()); if(Reciever.isLinked()) Reciever.passCoord(); return Reciever; 
    }

	friend PassCoordClass& operator >>(const     QPair<V,V>&  Coord, PassCoordClass& Reciever)
    { Reciever.setInput(Coord); if(Reciever.isLinked()) Reciever.passCoord(); return Reciever; } 

    friend PassCoordClass& operator | (PassCoordClass& Sender, PassCoordClass& Reciever)
    { Sender.setLink(&Reciever); return Reciever; }

    friend std::shared_ptr<PassCoordClass>  operator | (std::shared_ptr<PassCoordClass> Sender, 
	                                                    std::shared_ptr<PassCoordClass> Reciever)
    { Sender->setLink(Reciever.get()); return Reciever; }

    friend PassCoordClass& operator | (std::shared_ptr<PassCoordClass> Sender, PassCoordClass& Reciever)
    { Sender->setLink(&Reciever); return Reciever; }

};


template<typename T>
QPair<T, T> operator*(QPair<T, T> x, const QPair<T, T>& y) { x.first *= y.first; x.second *= y.second; return x;}

template<typename T, typename V>
QPair<V, V> operator*(QPair<V, V> x, const QPair<T, T>& y) { x.first *= y.first; x.second *= y.second; return x;}

template<typename T> QPair<T, T> operator*(QPair<T, T> x, const T& Scale)   { x.first *= Scale; x.second *= Scale ; return x; }
template<typename T> QPair<T, T> operator+(QPair<T, T> x, const T& addition){ x.first += addition; x.second += addition;  return x; }

template<typename T> QPair<T, T> operator/(QPair<T, T> x, const T& Scale)   { x.first /= Scale; x.second /= Scale ; return x; }

template<typename T>
QPair<T, T> operator/(QPair<T, T> x, const QPair<T, T>& y) { x.first /= y.first; x.second /= y.second; return x;}
template<typename T, typename V>
QPair<V, V> operator/(QPair<V, V> x, const QPair<T, T>& y) { x.first /= y.first; x.second /= y.second; return x;}

template<typename T>
std::pair<T, T> operator+(std::pair<T, T> x, const std::pair<T, T>& y) { x.first += y.first; x.second += y.second; return x; }

template<typename T>
std::pair<double, double> operator+(std::pair<double, double> x, const std::pair<T, T>& y) { x.first += y.first; x.second += y.second; return x; }
template<typename T>
std::pair<double, double> operator+(const std::pair<T, T>& x, std::pair<double, double> y) { y.first += x.first; y.second += x.second; return y; }

template<typename T>
std::pair<T, T> operator-(std::pair<T, T> x, const std::pair<T, T>& y) { x.first -= y.first; x.second -= y.second; return x; }

template<typename T, typename V>
std::pair<T, T> operator-(std::pair<T, T> x, const std::pair<V, V>& y) { x.first -= y.first; x.second -= y.second; return x; }

template<typename T>
std::pair<T, T> abs_pair(std::pair<T, T> x) 
{ 
    x.first  = std::abs(x.first); 
    x.second = std::abs(x.second); 
    return x;
}


template <class T>
class PortAdapter : public PassCoordClass<float>
{
public:
    PortAdapter(){};
    void LinkAdapter(T* LinkObject,std::function<void(T&,QPair<float,float>)> SetFunction, std::function<QPair<float,float>(T&)> GetFunction)
    { InputFunction = SetFunction; OutputFunction = GetFunction; Receiver = LinkObject; };

    T* Receiver =  nullptr;

    std::function<void(T&,QPair<float,float>)> InputFunction = nullptr;
    std::function<QPair<float,float>(T&)> OutputFunction = nullptr;

    void setInput(const QPair<float,float>& coord) { if(InputFunction != nullptr) InputFunction(*Receiver,coord);};
    const QPair<float,float>& getOutput() { if(OutputFunction != nullptr) PassCoordClass<float>::OutputCoord = OutputFunction(*Receiver); 
                                                                   return PassCoordClass<float>::OutputCoord;};

};
