#ifndef DATATHREADSPROCESSING_H
#define DATATHREADSPROCESSING_H
#include <QPair>
#include <QDebug>
#include <chrono>
#include <cmath>
#include <debug_output_filter.h>
#include <set>
#include "engine_statistics.h"
#include "interface_pass_coord.h"
#include "interface_pass_value.h"
#include "module_period_measure.h"
#include <span>


template<typename T> class NodeValueThinning;
template<typename T> class NodeValueEnd;
template<typename T> class QQueue;
template<typename T> class passValueVoid;

template<typename T = float> 
class NodeCoordPassNop : public PassCoordClass<T>
{
  public: 
  QPair<T,T>& operator>>(QPair<T, T>& Coord)     override { return Coord;}

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Reciever) override {qDebug() << "PASS NOP"; return *this;}
};

template<typename T = float> 
class NodeCoordEnd : public PassCoordClass<T>
{
  public: 
  void operator>>(QPair<T, T>& Coord)     override {}
  void operator>>(QPair<int, int>& Coord) override {}

  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}
  void setInput(const QPair<T,T>& Input) { qDebug() << "[ END ]";}

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Reciever) override { return *this;}
};

template <class T = float>
class NodeCoordSubstract : public PassCoordClass<float>
{
public:
  std::vector<QPair<T,T>> InputCoords{QPair<T,T>(0,0), QPair<T,T>(0,0)};

	int InputCount = 0;

	 const QPair<T,T>& getOutput() override { return PassCoordClass<float>::OutputCoord;}

	 void setInput(const QPair<T,T>& Coord) override
	 {
                                           InputCoords[InputCount] = Coord; 
                                                       InputCount++; 
       if(InputCount >= 2) 
       {  InputCount = 0;
          PassCoordClass<T>::OutputCoord = InputCoords[0] - InputCoords[1]; PassCoordClass<T>::passCoord(); }

	 }

};

template <class T = float>
class NodeCoordTimeDifference : public PassCoordClass<float>
{
public:
   QPair<T,T> LastCoord{0,0};
   bool EnableOutput{false};
   NodeCoordPassNop<T> NopNode;

	 const QPair<T,T>& getOutput() override { return PassCoordClass<float>::OutputCoord;}

	 void setInput(const QPair<T,T>& Coord) override
	 {
          if(EnableOutput) { PassCoordClass<T>::OutputCoord = Coord - LastCoord; PassCoordClass<T>::passCoord(); }
             EnableOutput = true;                                     LastCoord = Coord; 
	 }

  bool FilterOpened = false;

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
                         { if(!EnableOutput) return NopNode; PassCoordClass<T>::OutputCoord >> Receiver; return Receiver; }

  QPair<T,T>& operator>>(QPair<T,T>& Receiver) override  
                         { if(EnableOutput) Receiver = PassCoordClass<T>::OutputCoord; return Receiver; };

};

template <class T = float>
class NodeCoordVelocity : public PassCoordClass<T>
{
public:
   QPair<T,T> LastCoord{0,0};
   bool EnableOutput{false};
   NodeCoordPassNop<T> NopNode;

   float Period;
   std::chrono::time_point<std::chrono::high_resolution_clock> LastTimePoint;
   std::chrono::time_point<std::chrono::high_resolution_clock> NewTimePoint;

	 const QPair<T,T>& getOutput() override { return PassCoordClass<float>::OutputCoord;}

	 void setInput(const QPair<T,T>& Coord) override
	 {
          NewTimePoint = std::chrono::high_resolution_clock::now();
          Period = std::chrono::duration_cast<std::chrono::microseconds>(NewTimePoint - LastTimePoint).count()/1000000.0;
          if(EnableOutput && Period > 0) { PassCoordClass<T>::OutputCoord = (Coord - LastCoord)/(T)Period; PassCoordClass<T>::passCoord();} 

          LastCoord = Coord; 
          LastTimePoint = NewTimePoint;
          EnableOutput = true;                                   
	 }

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
                         { if(!EnableOutput) return NopNode; PassCoordClass<T>::OutputCoord >> Receiver; return Receiver; }

  QPair<T,T>& operator>>(QPair<T,T>& Receiver) override  
                         { if(EnableOutput) Receiver = PassCoordClass<T>::OutputCoord; return Receiver; };
};

template <class T = float>
class NodeCoordSum : public PassCoordClass<float>
{
public:
  QPair<T, T> SumCoord;

  const QPair<T,T>& getOutput() override
  { PassCoordClass<T>::OutputCoord = SumCoord; SumCoord.first = 0; SumCoord.second = 0; return PassCoordClass<T>::OutputCoord; }

  void setInput(const QPair<T,T>& Coord) override
  { SumCoord.first += Coord.first; SumCoord.second += Coord.second; PassCoordClass<T>::passCoord();}

};

template<typename T>
class NodeCoordAbsolute : public PassCoordClass<T>
{
	public:
	NodeCoordAbsolute(){};

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord.first = std::abs(Coord.first);
    PassCoordClass<T>::OutputCoord.second = std::abs(Coord.second);
    PassCoordClass<T>::passCoord();
	};
};

template<typename T>
class NodeCoordGain : public PassCoordClass<T>
{
	public:
	NodeCoordGain(){};
  NodeCoordGain(T Value): GainFirst(Value), GainSecond(Value) {}; 
  NodeCoordGain(T Value1, T Value2): GainFirst(Value1), GainSecond(Value2) {}; 
  T GainFirst  = 1;
  T GainSecond = 1;

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord.first = Coord.first * GainFirst;
    PassCoordClass<T>::OutputCoord.second = Coord.second * GainSecond;
    PassCoordClass<T>::passCoord();
	};

	NodeCoordGain<T>& operator()(T Value) { GainFirst = Value; GainSecond = Value; return *this; }
	NodeCoordGain<T>& operator()(T Value1, T Value2) { GainFirst = Value1; GainSecond = Value2; return *this; }
};

template<typename T>
class NodeCoordOffset : public PassCoordClass<T>
{
	public:
	NodeCoordOffset(){};
  NodeCoordOffset(T Value): OffsetFirst(Value), OffsetSecond(Value) {}; 
  NodeCoordOffset(T Value1, T Value2): OffsetFirst(Value1), OffsetSecond(Value2) {}; 
  T OffsetFirst = 0;
  T OffsetSecond = 0;

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord.first = Coord.first + OffsetFirst;
    PassCoordClass<T>::OutputCoord.second = Coord.second + OffsetSecond;
    PassCoordClass<T>::passCoord();
	};

	NodeCoordOffset<T>& operator()(T Value) { OffsetFirst = Value; OffsetSecond = Value; return *this; }
	NodeCoordOffset<T>& operator()(T Value1, T Value2) { OffsetFirst = Value1; OffsetSecond = Value2; return *this; }
};



template<typename T>
class NodeCoordDetector : public PassCoordClass<T>
{
	public:
	NodeCoordDetector(){};
	NodeCoordDetector(double tolerance_distance){ tolerance = tolerance_distance; }
	NodeCoordDetector(double tolerance_distance, QPair<T,T> Wait){ tolerance = tolerance_distance;
                                                             WaitCoord = Wait;};

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord = Coord; CheckCoord(Coord); PassCoordClass<T>::passCoord();
	};
  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}

  void CheckCoord(const QPair<T,T>& Coord)
  {
    CoordDiff = Coord - WaitCoord; distance = std::hypot(CoordDiff.first, CoordDiff.second);
    isCoordDetected = distance < tolerance;
  };
  void reset() { isCoordDetected = false;}
    
  QPair<T,T> WaitCoord{0,0};
  QPair<T,T> CoordDiff{0,0};

  bool isCoordDetected = false;
  double tolerance = 0.01;
  double distance = 0;

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
  {
    PassCoordClass<T>::OutputCoord >> Receiver; return Receiver; 
  }
  void operator>>(QPair<T,T>& Receiver) override  { Receiver = PassCoordClass<T>::OutputCoord; };

	NodeCoordDetector<T>& operator()(double tolerance_distance, QPair<T,T> Wait) { tolerance = tolerance_distance; WaitCoord = Wait; return *this;}
	NodeCoordDetector<T>& operator()(QPair<T,T> Wait) {WaitCoord = Wait; return *this;}
};



template<typename T>
class NodeCoordPassShutter : public PassCoordClass<T>
{
	public:
	NodeCoordPassShutter(){};

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord = Coord; if(ShutterOpened) PassCoordClass<T>::passCoord();
	};
  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}
    
  NodeCoordPassNop<T> NopNode;
  bool ShutterOpened = false;

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
  {
    if(!ShutterOpened) return NopNode; PassCoordClass<T>::OutputCoord >> Receiver; return Receiver;
  }
  void operator>>(QPair<T,T>& Receiver) override  { if(ShutterOpened) Receiver = PassCoordClass<T>::OutputCoord; };

	NodeCoordPassShutter<T>& operator()(bool Opened) { ShutterOpened =  Opened; return *this; }
	NodeCoordPassShutter<T>& operator()(const NodeCoordDetector<T>& Detector) { ShutterOpened =  Detector.isCoordDetected; return *this; }
};


template <class T = float>
class NodeCoordAxisInversion : public PassCoordClass<float>
{
public:
  enum class AXIS_TYPE { X_AXIS = 0, Y_AXIS = 1, ALL_AXIS = 2, NONE_AXIS = 3 };
  QPair<T, T> SumCoord;
  int AxisXDirection = 1;
  int AxisYDirection = 1;
  NodeCoordAxisInversion(int Axis) { SetInversion(Axis);};
  NodeCoordAxisInversion(AXIS_TYPE Axis) { SetInversion((int)Axis);};

	void SetInversion(int Axis)
  { 
    if(Axis == 0) { AxisXDirection = -1; AxisYDirection =  1;}
    if(Axis == 1) { AxisXDirection = 1;  AxisYDirection = -1;}
    if(Axis == 2) { AxisXDirection = -1; AxisYDirection = -1;}
    if(Axis == 3) { AxisXDirection = 1; AxisYDirection = 1;}
  };

	NodeCoordAxisInversion<T>& operator()(int Axis) { SetInversion(Axis); return *this;}
	NodeCoordAxisInversion<T>& operator()(AXIS_TYPE Axis) { SetInversion(Axis); return *this;}

  void setInput(const QPair<T,T>& Coord) override
  {
    PassCoordClass<T>::OutputCoord.first = Coord.first*AxisXDirection; 
    PassCoordClass<T>::OutputCoord.second = Coord.second*AxisYDirection; 
    PassCoordClass<T>::passCoord();
  }

};

template<typename T>
class NodeCoordPassFilter : public PassCoordClass<T>
{
	public:
	NodeCoordPassFilter(){};
	NodeCoordPassFilter(double LimitNorm){ CoordNormLimit = LimitNorm;};

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord = Coord;  FilterOpened = StatisticCoord<float>::Norm(Coord) < CoordNormLimit;
    if(FilterOpened) PassCoordClass<T>::passCoord();
	};
    
  NodeCoordPassNop<T> NopNode;
  bool FilterOpened = false;
  double CoordNormLimit = 1000;

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
                         { if(!FilterOpened) return NopNode; PassCoordClass<T>::OutputCoord >> Receiver; return Receiver; }
  QPair<T,T>& operator>>(QPair<T,T>& Receiver) override  
                         { if(FilterOpened) Receiver = PassCoordClass<T>::OutputCoord; return Receiver; };

	NodeCoordPassFilter<T>& operator()(double Limit) { CoordNormLimit =  Limit; return *this; }
};

template<typename T>
class NodeCoordPassThinning : public PassCoordClass<T>
{
	public:
	NodeCoordPassThinning(){};
	NodeCoordPassThinning(int value){ peak = value;};
  int counter = 0;
  int peak = 10;
  NodeCoordPassNop<T> NopNode;
  bool FilterOpened = false;

	void setInput(const QPair<T,T>& Coord) override
	{
                                     FilterOpened = false;
    counter++; if(counter >= peak) { FilterOpened = true; counter = 0; }
    PassCoordClass<T>::OutputCoord = Coord;  

    if(FilterOpened) PassCoordClass<T>::passCoord();
	};

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
                         { if(!FilterOpened) return NopNode; PassCoordClass<T>::OutputCoord >> Receiver; return Receiver; }

  QPair<T,T>& operator>>(QPair<T,T>& Receiver) override  
                         { if(FilterOpened) Receiver = PassCoordClass<T>::OutputCoord; return Receiver; };

	NodeCoordPassThinning<T>& operator()(int value) { peak =  value; return *this; }
};

template<typename T>
class NodeCoordRandomizer : public PassCoordClass<T>
{
	public:
	NodeCoordRandomizer(){};
  NodeCoordRandomizer(float Amplitude): AmplitudeNoize1(Amplitude), AmplitudeNoize2(Amplitude) {}; 
  NodeCoordRandomizer(float Amplitude1, float Amplitude2): AmplitudeNoize1(Amplitude1), AmplitudeNoize2(Amplitude2) {}; 

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord.first = Coord.first + AmplitudeNoize1*std::rand()/RAND_MAX;
    PassCoordClass<T>::OutputCoord.second = Coord.second + AmplitudeNoize2*std::rand()/RAND_MAX;
    PassCoordClass<T>::passCoord();
	};

  float AmplitudeNoize1 = 0; 
  float AmplitudeNoize2 = 0; 

	NodeCoordRandomizer<T>& operator()(float Amplitude) { AmplitudeNoize1 = Amplitude; AmplitudeNoize2 = Amplitude; return *this; }
	NodeCoordRandomizer<T>& operator()(float Amplitude1, float Amplitude2) 
  { AmplitudeNoize1 = Amplitude1; AmplitudeNoize2 = Amplitude2; return *this; }
};

template<typename T = float>
class NodeCoordSplitToTime : public PassCoordClass<T>
{
	public:
	NodeCoordSplitToTime(){};
  NodeCoordSplitToTime(int num) {channel_active = num;}; 
  MeasurePeriodNode MeasurePeriod;
  std::pair<T,T> coordOutput1;
  std::pair<T,T> coordOutput2;
  int channel_active = 0;
  int counter = 0;
  int counter_limit = 100;

  void setResetCounter(int value ) { counter_limit = value; qDebug() << "RESET COUNTER LIMIT: " << value;};
	void setInput(const QPair<T,T>& Coord) override
	{
    MeasurePeriod++;
    coordOutput1.second = Coord.first;
    coordOutput2.second = Coord.second;
    coordOutput1.first = 0.5*counter;
    coordOutput2.first = 0.5*counter; counter++; if(counter > counter_limit) counter = 0;
    if(channel_active == 1) PassCoordClass<T>::OutputCoord = coordOutput2; else PassCoordClass<T>::OutputCoord = coordOutput1;
    PassCoordClass<T>::passCoord();
	};

	NodeCoordSplitToTime<T>& operator()(int num) { channel_active = num; return *this; }
};

template<typename T>
class NodeCoordRelation : public PassCoordClass<T>
{
	public:

	void setInput(const QPair<T,T>& Coord) override
	{
    if(Coord.second == 0) { PassCoordClass<T>::OutputCoord = QPair<T,T>(0,0);}

    PassCoordClass<T>::OutputCoord.first = Coord.first/Coord.second;
    PassCoordClass<T>::OutputCoord.second = Coord.first/Coord.second;
    PassCoordClass<T>::passCoord();
	};
};

template<typename T>
class NodeCoordPassValue : public PassCoordClass<T>
{
	public:
  int channel = 0;
  NodeCoordPassValue() { };
  NodeCoordPassValue(int num) { channel = num; };
  NodeCoordPassValue& operator()(int num) { channel = num; return *this; };

  void setLink(PassValueClass<T>* NewLink) override { NodesValueLinked.push_back(NewLink); }
	                    std::vector<PassValueClass<T>*> NodesValueLinked ;

	void passValue() { if(NodesValueLinked.empty()) return; for(auto& link: NodesValueLinked) {*this >> *link;} }

	void setInput(const QPair<T, T>& Coord) override { PassCoordClass<T>::OutputCoord = Coord; passValue();};

  PassValueClass<T>& operator>>(PassValueClass<float>& dst )
	{
    if(channel == 0) dst.setValue(this->OutputCoord.first);
    if(channel == 1) dst.setValue(this->OutputCoord.second);
    return dst;
	};


};


template<typename T>
class NodeCoordAvaragePeriodic : public PassCoordClass<T>
{
	public:
	NodeCoordAvaragePeriodic(){};
	NodeCoordAvaragePeriodic(int size) { window_size = size;};

	void setInput(const QPair<T,T>& Coord) override
	{
      if(input_counter >= window_size) 
      {  input_counter = 0; PassCoordClass<T>::OutputCoord.first = 0; 
                            PassCoordClass<T>::OutputCoord.second = 0; }

     PassCoordClass<T>::OutputCoord.first += Coord.first/window_size;
     PassCoordClass<T>::OutputCoord.second += Coord.second/window_size; 
         input_counter++;
	};
  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}

	bool isLoaded() { return window_size == input_counter;}
    
	int window_size = 10;
	int input_counter = 0;
  NodeCoordPassNop<T> NopNode;

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
  { 
    if(isLoaded()) { this->getOutput() >> Receiver; return Receiver; }; return NopNode;
  };

  void operator>>(QPair<T,T>& Receiver) override  { if(isLoaded()) Receiver = PassCoordClass<T>::OutputCoord; };


	NodeCoordAvaragePeriodic & operator()(int size) { window_size = size; return *this;}
};

template<typename T>
class NodeCoordAvargeGliding : public PassCoordClass<T>
{
	public:
	NodeCoordAvargeGliding(){ SamplePoints.resize(10); std::fill(SamplePoints.begin(), 
                                                                SamplePoints.end(), QPair<T,T>(0,0)); 
                                                                CurrentPoint = SamplePoints.begin();  
                                                                PassCoordClass<T>::OutputCoord = QPair<T,T>(0,0);};
	NodeCoordAvargeGliding(int size) {    window_size = size; 
                                         SamplePoints.resize(size); 
                               std::fill(SamplePoints.begin(), 
                                         SamplePoints.end(), QPair<T,T>(0,0)); 
                                         CurrentPoint = SamplePoints.begin(); 
                                         PassCoordClass<T>::OutputCoord = QPair<T,T>(0,0);};

	void setInput(const QPair<T,T>& Coord) override
	{
      PassCoordClass<T>::OutputCoord.first  -= *CurrentPoint.first/window_size;
      PassCoordClass<T>::OutputCoord.second -= *CurrentPoint.second/window_size; 
    
     *CurrentPoint.first = Coord.first/window_size; 
     *CurrentPoint.second = Coord.second/window_size;
      PassCoordClass<T>::OutputCoord.first += *CurrentPoint;

      CurrentPoint++; if(CurrentPoint == SamplePoints.end()) CurrentPoint = SamplePoints.begin();

	};
  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}

    
	int window_size = 10;
  std::vector<QPair<T,T>> SamplePoints;
  typename std::vector<QPair<T,T>>::iterator CurrentPoint;

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
  { 
     this->getOutput() >> Receiver; return Receiver; ; 
  };

  void operator>>(QPair<T,T>& Receiver) override  { Receiver = PassCoordClass<T>::OutputCoord; };
	NodeCoordAvargeGliding & operator()(int size) { window_size = size; return *this;}
};

template<typename T>
class NodeCoordPopInput : public PassCoordClass<T>
{
	public:
	NodeCoordPopInput(){ Coords.resize(2); std::fill(Coords.begin(), Coords.end() , QPair<T,T>(0,0)); CurrentCoord = Coords.begin();};
  void SetCoords(std::vector<QPair<T,T>> PopCoords) { Coords = PopCoords; CurrentCoord = Coords.begin(); };
  void SetCoords(QVector<QPair<T,T>> PopCoords) {Coords.resize(0); for(auto& Coord: PopCoords) Coords.push_back(Coord); CurrentCoord = Coords.begin(); };

	void setInput(const QPair<T,T>& Coord) override
	{
    PassCoordClass<T>::OutputCoord = *CurrentCoord; CurrentCoord++;
    if(CurrentCoord == Coords.end()) CurrentCoord = Coords.begin();
	};

  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}

  std::vector<QPair<T,T>> Coords;
  QPair<float,float> PopCoord() 
  {
    PassCoordClass<T>::OutputCoord = *CurrentCoord; CurrentCoord++;
    if(CurrentCoord == Coords.end()) CurrentCoord = Coords.begin();
    return PassCoordClass<T>::OutputCoord;
  }

  typename std::vector<QPair<T,T>>::iterator CurrentCoord; 
};


template<typename T = float>
class NodeCoordPassWait : public PassCoordClass<T>
{
	public:
  NodeCoordPassWait () {};
	NodeCoordPassWait (QPair<T,T> Wait) { WaitCoord = Wait;};
	void setInput(const QPair<float,float>& Coord) override
	{
    DiffCoord = Coord - WaitCoord;
    Distance = std::hypot(DiffCoord.first, DiffCoord.second);
    PassCoordClass<T>::OutputCoord = Coord;
	};
  const QPair<T,T>& getOutput() { return PassCoordClass<T>::OutputCoord;}

  NodeCoordPassNop<T> NopNode;

   QPair<T,T> WaitCoord{0,0}; 
   QPair<T,T> DiffCoord{0,0}; 
   double Distance = 0;
	 NodeCoordPassWait& operator()(QPair<T,T> Wait) {WaitCoord = Wait; return *this; }

  PassCoordClass<T>& operator>>(PassCoordClass<T>& Receiver) override 
  { 
    if(Distance < 2) {PassCoordClass<T>::OutputCoord >> Receiver; return Receiver; } else return NopNode;
  };

  void operator>>(QPair<T,T>& Receiver) override  { if(Distance < 2) Receiver = PassCoordClass<T>::OutputCoord; };
};

template<typename T = float>
class NodeCoordStorage : public PassCoordClass<T>
{
  public:
  template<typename V> class IteratorWrite;

  template<typename V = float>
  class IteratorRead
  {
    public:
    explicit IteratorRead(NodeCoordStorage<V>* store = nullptr) {if(store == nullptr) return; reset(store); };
    IteratorRead(IteratorRead<V>& It) 
    {                                    PosWrite = It.PosWrite; ExtraSpace = It.ExtraSpace; 
                                         PosIterator = It.PosIterator; PosLast = It.PosLast; 
                                         PosBegin = It.PosBegin;     PosEnd = It.PosEnd; };

    void reset(NodeCoordStorage<V>* store)      
    { PosWrite = &store->PosWrite; PosLast = &store->PosLast; ExtraSpace = store->ExtraSpace; 
      PosBegin = store->Coords.begin(); PosIterator = PosBegin; PosEnd = store->Coords.end(); }

    void operator=(const IteratorRead<V>&  It)  { PosIterator = It.PosIterator; }
    void operator=(const IteratorWrite<V>& It)  { PosIterator = It.PosIterator; }

     int  operator-(IteratorRead<V>& it) { return this->PosIterator - it.PosIterator;}

     void operator+=(int Offset)          
     { 
      if(PosEnd - PosIterator > Offset) {PosIterator += Offset; return; }  
                                         PosIterator = PosBegin + Offset - (PosEnd - PosIterator);
     }; 

    IteratorRead<V> operator+(int Offset)  
    { 
      IteratorRead<V> IT{*this};
      if(PosEnd - PosIterator > Offset) {IT.PosIterator += Offset; return IT; }  
                                         IT.PosIterator = PosBegin + Offset - (PosEnd - PosIterator);
                                                                   return IT;
    }; 

     void operator-=(int Offset)          
     { 
      if(PosIterator - PosBegin >= Offset ) { PosIterator -= Offset; return; }  
                                              PosIterator = PosEnd - Offset + (PosIterator - PosBegin);
     }; 

    IteratorRead<V> operator-(int Offset)  
    { 

      IteratorRead<V> IT{*this};
      if(PosIterator - PosBegin >= Offset ) { IT.PosIterator -= Offset; return IT; }  
                                              IT.PosIterator = PosEnd + (PosIterator - PosBegin) - Offset;
                                                                        return IT;
    }; 

    const QPair<float,float>& operator*() const { return *PosIterator; }

    int16_t DistanceToWrite1 = 0;
    int16_t DistanceToWrite2 = 0;
    uint16_t ExtraSpace = 0;
    uint16_t getDistance(IteratorWrite<V>& it ) 
    { 
       DistanceToWrite1 = it.PosIterator - PosIterator;
       DistanceToWrite2 = (PosEnd - PosIterator) + (it.PosIterator - PosBegin);
       
       if(DistanceToWrite1 >= 0 ) return DistanceToWrite1; return DistanceToWrite2; 
    };
    int getRemain() { return PosEnd - PosIterator;}

    IteratorRead<V>& operator++(int) 
    { 
      if(PosIterator == PosLast->PosIterator) return *this;
         PosIterator++; 
      if(PosIterator == PosEnd) PosIterator = PosBegin; return *this; 
    }

    IteratorRead<V>& operator++() { PosIterator++; if(PosIterator == PosEnd) PosIterator = PosBegin; return *this; }

    bool operator!=(const IteratorRead<V>& other) const { return PosIterator != other.PosIterator; }
    bool operator==(const IteratorRead<V>& other) const { return PosIterator == other.PosIterator; }

    bool operator!=(const IteratorWrite<V>& other) const { return PosIterator != other.PosIterator; }
    bool operator==(const IteratorWrite<V>& other) const { return PosIterator == other.PosIterator; }

    typename std::vector<QPair<T,T>>::iterator PosIterator ; 
    typename std::vector<QPair<T,T>>::iterator PosBegin ; 
    typename std::vector<QPair<T,T>>::iterator PosEnd ; 
    typename std::vector<QPair<T,T>>::iterator PosFuture ; 

    IteratorWrite<V>* PosWrite = nullptr;
    IteratorRead<V>*  PosLast = nullptr;
  };

  template<typename V = float>
  class IteratorWrite
  {
    public:
    explicit IteratorWrite(NodeCoordStorage<V>* store = nullptr) {if(store == nullptr) return; reset(store); };
    void reset(NodeCoordStorage<V>* store)  
    { 
      SizeStorage = store->SizeStorage;  
      PosBegin = store->Coords.begin(); PosIterator = PosBegin; PosEnd = store->Coords.end(); 
      PosRead = &store->PosRead; PosLast = &store->PosLast;}

    void operator=(const IteratorWrite& it) { PosIterator = it.PosIterator; }

    QPair<float,float> operator*() const { return *PosIterator; }

    int SizeStorage = 10;
    int PosDistance = 0;

    int getRemain() { return PosEnd - PosIterator;}
    void operator++(int) 
    { 
        PosDistance = PosRead->getDistance(*this);
        if(PosDistance >= SizeStorage) return;
                                              *PosLast = *this; 
        PosIterator++; 
        if(PosIterator == PosEnd  ) PosIterator = PosBegin; 
    }


    void setData(const QPair<V,V>& Coord) { *PosIterator = Coord; }

    bool operator!=(const IteratorRead<V>& other)  const { return PosIterator != other.PosIterator; }
    bool operator==(const IteratorRead<V>& other)  const { return PosIterator == other.PosIterator; }
    bool operator!=(const IteratorWrite<V>& other) const { return PosIterator != other.PosIterator; }
    bool operator==(const IteratorWrite<V>& other) const { return PosIterator == other.PosIterator; }

    typename std::vector<QPair<T,T>>::iterator PosIterator; 
    typename std::vector<QPair<T,T>>::iterator PosBegin; 
    typename std::vector<QPair<T,T>>::iterator PosEnd; 

    IteratorRead<V>* PosRead = nullptr;
    IteratorRead<V>* PosLast = nullptr;
  };

	public:
  enum class INPUT_MODE { SINGLE_LOAD = 0, CONTINOUS_LOAD = 1, AUTO_SKIP_LOAD = 2};
  NodeCoordStorage ()         {  setSize(10); };
	NodeCoordStorage (int size) {  setSize(size ); };
  void setSize(int size)      {  Coords.resize(size + ExtraSpace); SizeStorage = size; reset(); } 
  void reset()                {  PosRead.reset(this); PosLast.reset(this); PosWrite.reset(this); PassedNum = 0;};
  int size() { return  SizeStorage;}

  int SizeStorage    = 10;
  int LengthRollback = 5;
  INPUT_MODE MODE{INPUT_MODE::SINGLE_LOAD};

  bool RollbackAuto = false;

  uint8_t ExtraSpace = 4;

   int PassedNum = 0;

  std::vector<QPair<T,T>> Coords;

   IteratorRead<T> PosRollback {nullptr}; 
   IteratorRead<T> PosRead     {nullptr}; 
   IteratorRead<T> PosLast     {nullptr}; 
  IteratorWrite<T> PosWrite    {nullptr}; 
  IteratorRead<T>& begin()  { return   PosRead;}
  IteratorRead<T>   end()  {auto Pos = PosLast; return ++Pos;}

   int getAvailable() { return PosRead.getDistance(PosWrite); }
   int getPassed()    { return PassedNum; }
  bool isLoaded()     { return PosRead.getDistance(PosWrite) >= SizeStorage; }
  void skipLoaded()   { PosRead = PosWrite; }
  void setAutoSkip(bool OnOff) { this->MODE=INPUT_MODE::AUTO_SKIP_LOAD;}
  void setContinousMode(bool OnOff) {this->MODE=INPUT_MODE::SINGLE_LOAD; if(OnOff) this->MODE=INPUT_MODE::CONTINOUS_LOAD; }

	void setInput(const QPair<float,float>& Coord) override 
  { 
    if(this->isLoaded()) switch(this->MODE)
                         {
                          case INPUT_MODE::AUTO_SKIP_LOAD: skipLoaded(); break;
                          case INPUT_MODE::CONTINOUS_LOAD: PosRead++; break;
                          case INPUT_MODE::SINGLE_LOAD: return; }

    PosWrite.setData(Coord); 
    PosWrite++; PassedNum++; 

    //qDebug() << "POS READ: " << PosRead.getRemain() << "DISTANCE: " << PosRead.getDistance(PosWrite);
    //passToLink();
  };

  void passToLink()
  {
                              if(!this->isLoaded() || !this->isLinked()) return; 
    for(auto& link: this->NodesLinked) { for(auto& coord: *this) coord >> *link; } 
                     skipLoaded(); 
    if(RollbackAuto) rollbackStore(LengthRollback);
    //qDebug() << "POS READ: " << PosRead.getRemain() << "DISTANCE: " << PosRead.getDistance(PosWrite);
  }

  void setRollbackAuto(bool OnOff, int Value = 0)   { RollbackAuto = OnOff; LengthRollback = Value;  };
  void rollbackStore(int length) { PosRead -= length; };

  void operator>>(std::vector<QPair<T,T>>& Storage) { for(auto& coord: *this) Storage.push_back(coord); }


  const QPair<T,T>& getLast()       { return *PosLast;}
  const QPair<T,T>& getOutput() override { auto& data = *PosRead; PosRead++; return data; };

  void operator=(NodeCoordStorage<T>& dst) { *this = dst.Cooords; PosRead = dst.PosRead; PosWrite = dst.PosWrite; }

};

template<typename T = float>
class NodeCoordVectorAdapter : public PassCoordClass<T>
{
	public:
  NodeCoordVectorAdapter (int size = 10): SizeLimit(size) {};
  NodeCoordVectorAdapter (std::vector<QPair<T,T>>& store, int size = 10): SizeLimit(size) { receiver = &store;};
  void setLink(std::vector<QPair<T,T>>& store) { receiver = &store; }
  PassCoordClass<T> operator()(std::vector<QPair<T,T>>& store) { receiver = &store; return *this;};

  int SizeLimit = 10;
	void setInput(const QPair<float,float>& Coord) override
	{
    qDebug() << "[ PASS TO VECTOR ]" << Coord.first << Coord.second;
    receiver->push_back(Coord);
	};

  std::vector<QPair<T,T>>* receiver = nullptr;
};


template<typename T = float>
class TestNodeCoordStorage
{
  public:
  TestNodeCoordStorage() 
  {
    qDebug() << "==============================================";
    qDebug() << "[ TEST NODE_COORD_STORAGE ]";
    for(int n = 1; n <= 40; n++) Data.push_back(QPair<T,T>(n,n));
    for(auto& pos: Data) qDebug() << "[ INPUT VECTOR ]" << pos.first << pos.second;
    qDebug() << "==============================================";

    Storage.setRollbackAuto(true,10);
    Storage | PassToVector; 

    qDebug() << "==============================================";
    qDebug() << "[ PASS PROCESS ]";
    for(auto& pos: Data) pos >> Storage;  
    qDebug() << "==============================================";

    for(auto& pos: DataOutput) qDebug() << "[ OUTPUT VECTOR ]" << pos.first << pos.second;
    qDebug() << "==============================================";
  }
  NodeCoordVectorAdapter<T> PassToVector{DataOutput,20};
        NodeCoordStorage<T> Storage{20}; 
    std::vector<QPair<T,T>> Data;
    std::vector<QPair<T,T>> DataOutput;
};

//======================================================================
template<typename T = float> 
class NodeValueNop : public PassValueClass<T>
{
  public: 
    PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) { return *this; };
    void operator>>(T& Receiver) { };
    void operator >>(int& OutputValue) {}
    void operator >>(uint32_t& OutputValue) {}

    //StatisticNode<float> operator>>(StatisticNode<float> receiver) override { return receiver;}
};
//===========================================================================
template< typename T = float>
class NodeValueDifference : public PassValueClass<T>
{
  public:
  T Diff{0};
  T LastValue{0};
  NodeValueNop<T> NodeNop;
  bool EnablePass{false};
  const T& getValue() override { return Diff;}

  void setValue(T NewValue) 
  { 
    if(EnablePass) Diff = NewValue - LastValue; 
                          LastValue = NewValue; 
    EnablePass = true;
  }

  PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) 
  { 
    if(EnablePass)  { PassValueClass<T>::getValue() >> Receiver; return Receiver; }
    return NodeNop;
  };
};

template <class T = float>
class NodeValueVelocity : public PassValueClass<T>
{
public:
  std::chrono::milliseconds Period;
  std::chrono::time_point<std::chrono::high_resolution_clock> LastTimePoint;
  std::chrono::time_point<std::chrono::high_resolution_clock> NewTimePoint;

  T Diff{0};
  T LastValue{0};
  NodeValueNop<T> NodeNop;
  bool EnablePass{false};
  const T& getValue() override { return Diff;}

  void setValue(T NewValue) 
  { 
    NewTimePoint = std::chrono::high_resolution_clock::now();
    Period = std::chrono::duration_cast<std::chrono::milliseconds>(NewTimePoint - LastTimePoint);
    if(EnablePass) Diff = (NewValue - LastValue)/Period.count(); 

    LastValue = NewValue; 
    LastTimePoint = NewTimePoint;
    EnablePass = true;
  }

  //PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override
  //{ 
  //  if(EnablePass)  { PassValueClass<T>::getValue() >> Receiver; return Receiver; }
  //  return NodeNop;
  //};


};


template<typename T = float>
class NodeValueSaturation : public PassValueClass<T>
{
  public:
  NodeValueSaturation() {};
  NodeValueSaturation(NodeValueSaturation<T>& Node) {this->Threshold = Node.Threshold;};
  void operator=(NodeValueSaturation<T>& Node) { this->Threshold = Node.Threshold;}

  double Threshold = 10;
  void setValue(T NewValue) override;
  NodeValueSaturation<T>& operator()(double NewThreshold){ Threshold = NewThreshold; return *this;};
};

template<typename T = float>
class NodeValueInversion : public PassValueClass<T>
{
  public:
  double Threshold = 10;
  void setValue(T NewValue) override { PassValueClass<T>::Value = -NewValue;};
  NodeValueInversion<T>& operator()(double NewThreshold){ Threshold = NewThreshold; return *this;};
};

template<typename T = float>
class NodeValueInversionBinary : public PassValueClass<T>
{
  public:
	virtual void setValue(T InputValue) { PassValueClass<T>::Value = (InputValue > 0) ? 0 : 1;};
};

template<typename T = float>
class NodeValueRandomizer : public PassValueClass<T>
{
  public:
  NodeValueRandomizer(T Value) { Amplitude = Value;}
  T Amplitude = 2;
  void setValue(T NewValue) override;
  NodeValueRandomizer<T>& operator()(T NewAmplitude){ Amplitude = NewAmplitude; return *this;};
};


template<typename T = float>
class NodeValueBreeder : public PassValueClass<T>
{
  public:
  double BreedMultiplier = 2;
  NodeValueBreeder<T>& operator()(double Multiplier){ BreedMultiplier = Multiplier; return *this;};
  PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override 
  {
  for(int n = 0; n < BreedMultiplier; n++) PassValueClass<T>::getValue() >> Receiver; return Receiver;
  }
};


template<typename T = float>
class NodeValueDetector : public PassValueClass<T>
{
  public:
  NodeValueDetector(){};
  NodeValueDetector(NodeValueDetector<T>& Detector);
  void operator=(NodeValueDetector<T>& Detector);
  void reset() 
  { 
    Signal = 0; 
    FlagInversion = false; 
    if(LinkedDetector != 0) LinkedDetector->reset();
  }

  T Threshold;
  T Signal;
  bool FlagInversion = false;

  void setValue(T NewValue) override 
  { 
    PassValueClass<T>::Value = NewValue; NewValue >> SaturationDetector(Threshold) >> Signal;
  };
  bool isSignal() 
  { 
    if(LinkedDetector != 0)  return ((Signal > 0.0001) ^ FlagInversion  ? true : false) || LinkedDetector->isSignal();

    return (Signal > 0.0001) ^ FlagInversion  ? true : false;
  }

  NodeValueDetector<T>& operator&&(NodeValueDetector<T>& Detector) { LinkedDetector = &Detector; return *this; }

  NodeValueDetector<T>& operator()(double ThresholdValue){ Threshold = ThresholdValue; return *this;};
  NodeValueDetector<T>& operator!(){FlagInversion = true; return *this;}
  
  NodeValueSaturation<T> SaturationDetector;
  NodeValueDetector<T>*  LinkedDetector = 0;
};

template<typename T>
NodeValueDetector<T>::NodeValueDetector(NodeValueDetector<T>& Detector)
{
  Threshold = Detector.Threshold;
  Signal = Detector.Signal;
  FlagInversion = Detector.FlagInversion;
  SaturationDetector = Detector.SaturationDetector;
  LinkedDetector = Detector.LinkedDetector;
};

template<typename T>
void NodeValueDetector<T>::operator=(NodeValueDetector<T>& Detector)
{
  Threshold = Detector.Threshold;
  Signal = Detector.Signal;
  FlagInversion = Detector.FlagInversion;
  SaturationDetector = Detector.SaturationDetector;
  LinkedDetector = Detector.LinkedDetector;
};


template<typename T = float>
class NodeValuePassShutter : public PassValueClass<T>
{
  public:
  bool Shutteropencved = true;
  bool FlagInversion = false;
  bool isOpen() { return Shutteropencved ^ FlagInversion;}
  NodeValueDetector<T>* LinkedDetector = 0;

  NodeValuePassShutter<T>& operator()(bool Flagopencved) { Shutteropencved = Flagopencved; return *this; };
  NodeValuePassShutter<T>& operator!() { FlagInversion = true; return *this;};

  NodeValuePassShutter<T>& operator()(NodeValueDetector<T>& Detector) 
  { 
    if(LinkedDetector == 0) LinkedDetector = &Detector;
    Shutteropencved = Detector.isSignal(); return *this; 
  };

  NodeValueNop<T> PassNop;

  PassValueClass<T>& operator>>(PassValueClass<T>& Receiver) override
  { 
    if(LinkedDetector != 0) Shutteropencved = LinkedDetector->isSignal();
    if(isOpen())  { PassValueClass<T>::getValue() >> Receiver; return Receiver; }
    return PassNop;
  };

};


template<typename T = float>
class NodeValueCategorizer : public PassValueClass<T>
{
  public:
  double LimitValue = 100;
  int    GroupCount = 10;   
  double GroupSpan = LimitValue/GroupCount;
  double InputValue = 0;

      void setValue(T InputValue) override;
  const T& getValue() override {return PassValueClass<T>::Value;}
  void SetCategoryLimit(double Limit, int Count) {LimitValue = Limit; GroupCount = Count; GroupSpan = LimitValue/Count;}

  NodeValueCategorizer<T>& operator()(double Limit, int Count) { SetCategoryLimit(Limit,Count); return *this; };
};

template<typename T>
void NodeValueCategorizer<T>::setValue(T InputValue)
{
      this->InputValue = InputValue;
      PassValueClass<T>::Value =  GroupCount - std::floor(InputValue/GroupSpan);

      if(InputValue < GroupSpan) PassValueClass<T>::Value = GroupCount;
      if(InputValue > LimitValue) PassValueClass<T>::Value = 0;
};


template<typename StorType = std::vector<float>, typename T = float>
class NodeValueStorage : public PassValueClass<T>
{
	public:
  NodeValueStorage()         { SetSize(10); };
	NodeValueStorage(int size) { SetSize(size); };
  void SetSize(int size) { Values.resize(size); CurrentInputValue = Values.begin();
                                                CurrentOutputValue = Values.begin(); } 

	void setValue(T Value) override
	{
    if(CurrentInputValue == Values.end() && ContinousLoad) 
       CurrentInputValue  = Values.begin();

    if(CurrentInputValue == Values.end() && !ContinousLoad) return;

      *CurrentInputValue  = Value; CurrentInputValue++; 

	};


  friend void operator>>(const std::vector<T>& Storage, NodeValueStorage& Receiver) { std::copy_n(Storage.begin(), Receiver.Values.size(), Receiver.Values.begin()); }
  friend void operator>>(const QVector<T>& Storage, NodeValueStorage& Receiver)     { std::copy_n(Storage.begin(), Receiver.Values.size(), Receiver.Values.begin()); }

  T getOutput() 
  { 
    PassValueClass<T>::Value = *CurrentOutputValue; 
    if(CurrentOutputValue != CurrentInputValue) CurrentOutputValue++; 
    if(CurrentOutputValue == Values.end()) CurrentOutputValue = Values.end();

    return PassValueClass<T>::Value;
  }

  StorType& getValues() { CurrentInputValue  = Values.begin(); 
                          CurrentOutputValue = Values.begin(); return Values; }

  template<typename OutputStore>
  void operator>>(OutputStore Storage) 
  { 
         if(Storage.size() < Values.size()) Storage.resize(Values.size());
    std::copy(Values.begin(), Values.end(), Storage.begin());

    CurrentInputValue  = Values.begin(); 
    CurrentOutputValue = Values.begin(); 
  }


  void SetContinousLoad(bool OnOff) { ContinousLoad = OnOff;};

  void reset() { CurrentInputValue = Values.begin(); 
                 CurrentOutputValue = Values.begin(); std::fill(Values.begin(),Values.end(),0); };

  bool isLoaded()  { return CurrentInputValue == Values.end();}
  T GetLastInput() { return *CurrentInputValue; }

  StorType Values{10};
  bool ContinousLoad = false;

  typename StorType::iterator CurrentInputValue; 
  typename StorType::iterator CurrentOutputValue; 

  PassValueClass<T>& operator()(int size) { if(Values.size() != size) SetSize(size); return *this; }
};


template<typename T = float>
class NodeValueDebugOutput : public PassValueClass<T>
{
  public:
  std::string TAG = "OUTPUT: "; 
  OutputFilter PrintFilter;
  int Thinning = 0;
  NodeValueDebugOutput<T>& operator()(std::string OUT_TAG, int ThinningParam){ TAG = OUT_TAG; Thinning = ThinningParam; return *this;};
  NodeValueDebugOutput<T>& operator()(QString OUT_TAG, int ThinningParam){ TAG = OUT_TAG.toStdString(); Thinning = ThinningParam; return *this;};
  void setValue(T NewValue) override { qDebug()<< TAG.c_str() << NewValue;};
};

template<typename T = float>
class ValuePeriodMeasure : public PassValueClass<T>
{
  public:
  std::string TAG = "VALUE PERIOD : "; 
  ValuePeriodMeasure<T>& operator()(std::string OUT_TAG){ TAG = OUT_TAG; return *this;};

  void setValue(T NewValue) override 
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

template<typename T = float>
class NodeValueAbsolutization : public PassValueClass<T>
{
  public:
  void setValue(T InputValue) override { PassValueClass<T>::Value = std::abs(InputValue);}
};

template<typename T>
void NodeValueRandomizer<T>::setValue(T NewValue) { PassValueClass<T>::Value = NewValue + Amplitude*std::rand()/RAND_MAX; }
template<typename T>
void NodeValueSaturation<T>::setValue(T NewValue) { if(NewValue < Threshold) PassValueClass<T>::Value = 0; else PassValueClass<T>::Value =1; }


//template<typename M> friend PassValueClass<M>& operator>>(const M& Value, PassValueClass<M>& Receiver);
//template<typename V> PassValueClass<T>& operator>>(V& Receiver) { getValue() >> Receiver; return Receiver;};

template<typename V = double>
class NodeValueThinning : public PassValueClass<V>
{
    public:
    NodeValueThinning(int peak = 100) {this->peak = peak;}; 
    void SetPeak(int PeakValue) { peak = PeakValue;}
    int counter = -1;
    int peak;
    NodeValueNop<V> PassNop;
    void setValue(V NewValue) override
    {
     (*this)++; if(isOpen())  PassValueClass<V>::setValue(NewValue);
    };

  	NodeValueThinning& operator()(int size) {peak = size; return *this; };
    PassValueClass<V>& operator>>(PassValueClass<V>& Receiver) 
    { 
      if(isOpen())  { PassValueClass<V>::getValue() >> Receiver; return Receiver; } return PassNop;
    };

    void operator>>(V& Receiver) { if(isOpen())  { Receiver = PassValueClass<V>::Value; } };

    bool isOpen(){ return counter >= peak;}
    virtual void operator++(int) { counter++; if(counter > peak) counter = 0; }
    void reset() { counter = 0; }
};

template<typename V = double>
class NodeValueBlockCounter : public NodeValueThinning<V>
{
  public:
  void operator++(int) {if(NodeValueThinning<V>::counter >= NodeValueThinning<V>::peak) return ; NodeValueThinning<V>::counter++; }
};


//==========================================================
template<typename T = float>
class NodeValueEnd: public PassValueClass<T>
{
    public:
    void setValue(T NewValue) override;
};
template<typename T> void NodeValueEnd<T>::setValue(T NewValue) { qDebug() << "PASS TO END: " << NewValue; }
//==========================================================

template<typename T>
void operator>>(const QPair<T,T>& Coord, QQueue<QPair<T,T>>& queue_stor) { queue_stor.enqueue(Coord);};
template<typename T>
void operator>>(const T& Coord, QQueue<T>& queue_stor) { queue_stor.enqueue(Coord);};


#endif  //DATATHREADSPROCESSING_H

//===========================================================================
//TEST FUNCTION APPROXIMATION and GRAPH





