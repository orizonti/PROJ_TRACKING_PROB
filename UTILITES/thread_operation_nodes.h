#ifndef DATATHREADSPROCESSING_H
#define DATATHREADSPROCESSING_H
#include <QPair>
#include <QDebug>
#include <chrono>
#include <cmath>
#include <debug_output_filter.h>
#include <set>
template<typename T> class ThinningFilter;
template<typename T> class endReceiver;
template<typename T> class QQueue;
template<typename T> class PassValueVoid;

class Statistic;

template <class Type>
class SubstractNode : public PassTwoCoordClass
{
public:
	QPair<Type, Type> CoordFirst;
	QPair<Type, Type> CoordSecond;
	QPair<Type, Type> CoordOutput;

	int InputCount = 0;

	 const QPair<double,double>& GetOutput() { return CoordOutput; }

	 void SetInput(const QPair<double,double>& Coord)
	 {
		 
			if (InputCount == 0) { CoordFirst = Coord; InputCount++; }
			else
			{
				CoordSecond = Coord;
				InputCount = 0;
			CoordOutput.first = CoordFirst.first - CoordSecond.first;
			CoordOutput.second = CoordFirst.second - CoordSecond.second;
			}
	 }

    SubstractNode<Type>& operator >>(SubstractNode<Type>& Reciever) {qDebug() << "PASS SUBSTRACT"; InputCount++; return *this;};

};



template <class Type>
class SumNode : public PassTwoCoordClass
{
public:
	QPair<Type, Type> CoordOutput;

	 const QPair<double,double>& GetOutput()
	 {
		 QPair<Type, Type> Return = CoordOutput;
		CoordOutput.first = 0;
		CoordOutput.second = 0;
		return Return;
	 }
	 void SetInput(const QPair<double,double>& Coord)
	 {
		 
		CoordOutput.first += Coord.first;
		CoordOutput.second += Coord.second;
	 }

	friend SumNode<Type>& operator<<(SumNode<Type>& SubObj,QPair<Type, Type> Coord)
    {

		SubObj.SetInput(Coord);
			return SubObj;

    }
	friend SumNode<Type>& operator<<( SumNode<Type>& SubObj,PassTwoCoordClass& Sender)
    {

		SubObj.SetInput(Sender.GetOutput());
			return SubObj;

    }
};

//===========================================================================
template<typename T = double> 
class PassValueClass
{
  public: 
    T Value;
    virtual void SetValue(T NewValue) { Value = NewValue;}
    virtual T& GetValue() { return Value;}

    template<typename M> friend PassValueClass<M>& operator>>(const M& Value, PassValueClass<M>& Receiver);
    virtual PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) { GetValue() >> Receiver; return Receiver;};
    virtual void operator>>(T& Receiver) { Receiver = GetValue();};
    virtual Statistic& operator>>(Statistic& receiver) { GetValue() >> receiver; return receiver; }
};

template<typename T>
PassValueClass<T>& operator>>(const T& Value, PassValueClass<T>& Receiver) { Receiver.SetValue(Value); return Receiver; }

//===========================================================================
template<typename T = double> 
class PassValuenop : public PassValueClass<T>
{
  public: 
    PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override { return *this; };
    void operator>>(T& Receiver) override  { };
    Statistic& operator>>(Statistic& receiver) override { return receiver;}
};
//===========================================================================
template< typename T = double>
class ValueDifference : public PassValueClass<T>
{
  public:
  T Diff;
  void SetValue(T NewValue) override;
  T& GetValue() override { return Diff;}
};

template<typename T>
void ValueDifference<T>::SetValue(T NewValue) { Diff = PassValueClass<T>::Value - NewValue; PassValueClass<T>::Value = NewValue; }

template<typename T = double>
class ValueSaturation : public PassValueClass<T>
{
  public:
  ValueSaturation() {};
  ValueSaturation(ValueSaturation<T>& Node) {this->Threshold = Node.Threshold;};
  void operator=(ValueSaturation<T>& Node) { this->Threshold = Node.Threshold;}

  double Threshold = 10;
  void SetValue(T NewValue) override;
  T& GetValue() override { return PassValueClass<T>::Value;}
  ValueSaturation<T>& operator()(double NewThreshold){ Threshold = NewThreshold; return *this;};
};

template<typename T = double>
class ValueInversion : public PassValueClass<T>
{
  public:
  double Threshold = 10;
  void SetValue(T NewValue) override;
  T& GetValue() override { return PassValueClass<T>::Value;}
  ValueInversion<T>& operator()(double NewThreshold){ Threshold = NewThreshold; return *this;};
};

template<typename T = double>
class ValueBinaryInversion : public PassValueClass<T>
{
  public:
  const T& GetValue() override { PassValueClass<T>::Value = (PassValueClass<T>::Value > 0) ? 0 : 1; return PassValueClass<T>::Value;}
};

template<typename T = double>
class ValueRandomization : public PassValueClass<T>
{
  public:
  double Amplitude = 2;
  void SetValue(T NewValue) override;
  T& GetValue() override { return PassValueClass<T>::Value;}
  ValueRandomization<T>& operator()(double NewAmplitude){ Amplitude = NewAmplitude; return *this;};
};


template<typename T = double>
class ValueBreeder : public PassValueClass<T>
{
  public:
  double BreedMultiplier = 2;
  T& GetValue() override { return PassValueClass<T>::Value;}
  ValueBreeder<T>& operator()(double Multiplier){ BreedMultiplier = Multiplier; return *this;};
  PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override; 
};

template<typename T = double>
class ValueDetector : public PassValueClass<T>
{
  public:
  ValueDetector(){};
  ValueDetector(ValueDetector<T>& Detector);
  void operator=(ValueDetector<T>& Detector);
  void Reset() 
  { 
    Signal = 0; 
    FlagInversion = false; 
    if(LinkedDetector != 0) LinkedDetector->Reset();
  }

  T Threshold;
  T Signal;
  bool FlagInversion = false;

  void SetValue(T NewValue) override 
  { 
    PassValueClass<T>::Value = NewValue; NewValue >> SaturationDetector(Threshold) >> Signal;
  };
  bool isSignal() 
  { 
    if(LinkedDetector != 0)  return (Signal > 0.0001 ^ FlagInversion  ? true : false) || LinkedDetector->isSignal();

    return Signal > 0.0001 ^ FlagInversion  ? true : false;
  }

  ValueDetector<T>& operator&&(ValueDetector<T>& Detector) { LinkedDetector = &Detector; return *this; }

  ValueDetector<T>& operator()(double ThresholdValue){ Threshold = ThresholdValue; return *this;};
  ValueDetector<T>& operator!(){FlagInversion = true; return *this;}
  
  ValueSaturation<T> SaturationDetector;
  ValueDetector<T>* LinkedDetector = 0;
};

template<typename T>
ValueDetector<T>::ValueDetector(ValueDetector<T>& Detector)
{
  Threshold = Detector.Threshold;
  Signal = Detector.Signal;
  FlagInversion = Detector.FlagInversion;
  SaturationDetector = Detector.SaturationDetector;
  LinkedDetector = Detector.LinkedDetector;
};

template<typename T>
void ValueDetector<T>::operator=(ValueDetector<T>& Detector)
{
  Threshold = Detector.Threshold;
  Signal = Detector.Signal;
  FlagInversion = Detector.FlagInversion;
  SaturationDetector = Detector.SaturationDetector;
  LinkedDetector = Detector.LinkedDetector;
};


template<typename T = double>
class ValuePassShutter : public PassValueClass<T>
{
  public:
  bool Shutteropencved = true;
  bool FlagInversion = false;
  bool isOpen() { return Shutteropencved ^ FlagInversion;}
  ValueDetector<T>* LinkedDetector = 0;

  ValuePassShutter<T>& operator()(bool Flagopencved) { Shutteropencved = Flagopencved; return *this; };
  ValuePassShutter<T>& operator!() { FlagInversion = true; return *this;};

  ValuePassShutter<T>& operator()(ValueDetector<T>& Detector) 
  { 
    if(LinkedDetector == 0) LinkedDetector = &Detector;
    Shutteropencved = Detector.isSignal(); return *this; 
  };

  PassValuenop<T> PassNop;

  PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override
  { 
    if(LinkedDetector != 0) Shutteropencved = LinkedDetector->isSignal();
    if(isOpen())  { PassValueClass<T>::GetValue() >> Receiver; return Receiver; }
    return PassNop;
  };

};

template<typename T>
PassValueClass<T>& ValueBreeder<T>::operator>>(PassValueClass<T>& Receiver) 
{ 
  for(int n = 0; n < BreedMultiplier; n++) GetValue() >> Receiver; return Receiver;
};

template<typename T = double>
class ValueCategorizer : public PassValueClass<T>
{
  public:
  double LimitValue = 100;
  int    GroupCount = 10;   
  double GroupSpan = LimitValue/GroupCount;
  double InputValue = 0;

  void SetValue(T InputValue) override;
  T& GetValue() override { return PassValueClass<T>::Value;}
  void SetCategoryLimit(double Limit, int Count) {LimitValue = Limit; GroupCount = Count; GroupSpan = LimitValue/Count;}

  ValueCategorizer<T>& operator()(double Limit, int Count) { SetCategoryLimit(Limit,Count); return *this; };

  //PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override; 

};

template<typename T>
void ValueCategorizer<T>::SetValue(T InputValue)
{
      this->InputValue = InputValue;
      PassValueClass<T>::Value =  GroupCount - std::floor(InputValue/GroupSpan);

      if(InputValue < GroupSpan) PassValueClass<T>::Value = GroupCount;
      if(InputValue > LimitValue) PassValueClass<T>::Value = 0;
};



template<typename T = double>
class ValueDebugOutput : public PassValueClass<T>
{
  public:
  std::string TAG = "OUTPUT: "; 
  OutputFilter PrintFilter;
  int Thinning = 0;
  ValueDebugOutput<T>& operator()(std::string OUT_TAG, int ThinningParam){ TAG = OUT_TAG; Thinning = ThinningParam; return *this;};
  ValueDebugOutput<T>& operator()(QString OUT_TAG, int ThinningParam){ TAG = OUT_TAG.toStdString(); Thinning = ThinningParam; return *this;};
  void SetValue(T NewValue) override { qDebug()<< TAG.c_str() << NewValue;};
};

template<typename T = double>
class ValuePeriodMeasure : public PassValueClass<T>
{
  public:
  std::string TAG = "VALUE PERIOD : "; 
  ValuePeriodMeasure<T>& operator()(std::string OUT_TAG){ TAG = OUT_TAG; return *this;};

  void SetValue(T NewValue) override 
  { 
    NewTimePoint = std::chrono::high_resolution_clock::now();
    Period = std::chrono::duration_cast<std::chrono::milliseconds>(NewTimePoint - LastTimePoint);
    qDebug() << TAG.c_str() << Period.count() << " ms";
    LastTimePoint = NewTimePoint;
  };

  std::chrono::time_point<std::chrono::high_resolution_clock> LastTimePoint = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> NewTimePoint = std::chrono::high_resolution_clock::now();
  std::chrono::milliseconds Period;
};

template<typename T = double>
class ValueAbsolutization : public PassValueClass<T>
{
  public:
  void SetValue(T InputValue) override { PassValueClass<T>::Value = std::abs(InputValue);}
};

template<typename T>
void ValueRandomization<T>::SetValue(T NewValue) { PassValueClass<T>::Value = NewValue + Amplitude*std::rand()/RAND_MAX; }
template<typename T>
void ValueSaturation<T>::SetValue(T NewValue) { if(NewValue < Threshold) PassValueClass<T>::Value = 0; else PassValueClass<T>::Value =1; }

template<typename T>
void ValueInversion<T>::SetValue(T NewValue) { PassValueClass<T>::Value = -NewValue;}

//template<typename M> friend PassValueClass<M>& operator>>(const M& Value, PassValueClass<M>& Receiver);
//template<typename V> PassValueClass<T>& operator>>(V& Receiver) { GetValue() >> Receiver; return Receiver;};

template<typename V = double>
class ThinningFilter : public PassValueClass<V>
{
    public:
    ThinningFilter(int peak = 100) {this->peak = peak;}; 
    void SetPeak(int PeakValue) { peak = PeakValue;}
    int counter = -1;
    int peak;
    PassValuenop<V> PassNop;
    void SetValue(V NewValue) override
    {
     (*this)++; if(isOpen())  PassValueClass<V>::SetValue(NewValue);
    };

  	ThinningFilter& operator()(int size) {peak = size; return *this; };
    PassValueClass<V>& operator>>(PassValueClass<V>& Receiver) override
    { 
      if(isOpen())  { PassValueClass<V>::GetValue() >> Receiver; return Receiver; } return PassNop;
    };

    void operator>>(V& Receiver) override { if(isOpen())  { Receiver = PassValueClass<V>::Value; } };
    Statistic& operator>>(Statistic& receiver) override { if(isOpen()) PassValueClass<V>::GetValue() >> receiver; return receiver; }

    bool isOpen(){ return counter >= peak;}
    virtual void operator++(int) { counter++; if(counter > peak) counter = 0; }
    void reset() { counter = 0; }
};

template<typename V = double>
class ValueBlockCounter : public ThinningFilter<V>
{
  public:
  void operator++(int) {if(ThinningFilter<V>::counter >= ThinningFilter<V>::peak) return ; ThinningFilter<V>::counter++; }
};


//==========================================================
template<typename T = double>
class endReceiver: public PassValueClass<T>
{
    public:
    void SetValue(T NewValue) override;
};
template<typename T> void endReceiver<T>::SetValue(T NewValue) { qDebug() << "PASS TO enD: " << NewValue; }
//==========================================================

template<typename T>
void operator>>(const QPair<T,T>& Coord, QQueue<QPair<T,T>>& queue_stor) { queue_stor.enqueue(Coord);};
template<typename T>
void operator>>(const T& Coord, QQueue<T>& queue_stor) { queue_stor.enqueue(Coord);};
#endif  //DATATHREADSPROCESSING_H
