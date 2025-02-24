#ifndef DEBUGOUTPUTFILTER_H
#define DEBUGOUTPUTFILTER_H
#include <QDebug>

#include <source_location>
 
class BlockCounterClass
{
  public:
  BlockCounterClass() {};
  BlockCounterClass(int Peak, bool isInverse) { this->Peak = Peak; this->FlagInverse = isInverse;}
  BlockCounterClass(const BlockCounterClass& Node) {this->Counter = Node.Counter; this->Peak = Node.Peak;};
  void operator=(const BlockCounterClass& Node) {this->Counter = Node.Counter; this->Peak = Node.Peak; this->FlagInverse = Node.FlagInverse;};
  void operator++(int){if(Counter < Peak) Counter++;}
  operator bool () {return isOpen();};
  BlockCounterClass& operator()(int NewPeak) {this->Peak = NewPeak; return *this;};

  int Counter = 0;
  int Peak = 10000;
  bool FlagInverse = false;
  bool isOpen() { return (Counter >= Peak) ^ FlagInverse;}
  void Increment() { Counter++;}
  void Reset() { Counter = 0;}

};

    //const std::source_location& location = std::source_location::current())
class OutputFilter
{
    public:
    OutputFilter(int peak = 100) {this->peak = peak;}; 
    int counter = 0;
    int peak;
    std::map<int,int> MapCounters;
    QDebug out_message = QDebug(QtDebugMsg);
  	OutputFilter& operator()(int size) {peak = size; return *this; };

    OutputFilter& operator<<(QString output) {if(counter == peak)  out_message << output; return *this; };
    OutputFilter& operator<<(const char* output) {if(counter == peak)  out_message << output; return *this; };

    template<typename T> friend OutputFilter& operator<<(OutputFilter& filter, T value); 
    template<typename T>
    OutputFilter& operator<<(QPair<T,T> Coord) {if(counter == peak)  out_message << Coord.first << Coord.second; return *this; };

    friend OutputFilter& operator<<(QDebug out, OutputFilter& filter);

    bool isOpen(){ return counter == peak;}
    //void operator++(int) { counter++; if(counter > peak) counter = 0; }
};

template<typename T> OutputFilter& operator<<(OutputFilter& filter, T value)
{
   if(filter.counter == filter.peak) filter.out_message << QString::number(value); return filter;
}
template<> OutputFilter& operator<<(OutputFilter& filter, QTextStream& (*control)(QTextStream &));


#endif //DEBUGOUTPUTFILTER_H
