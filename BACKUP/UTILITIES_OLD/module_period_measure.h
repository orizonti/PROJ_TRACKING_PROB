#ifndef PERIOD_MEASURE_H
#define PERIOD_MEASURE_H
#include <chrono>
#include <QString>

class MeasurePeriodNode
{
    public:
    void PushTick()
    {
    if(Counter >= 1)  {                 TimePoint =   std::chrono::high_resolution_clock::now();
                            Duration  = TimePoint - TimePointLast; 
                        TimePointLast = TimePoint;

                           TickPeriod = Duration.count()*1000;
                        TickFrequency = 1/Duration.count();    Counter = 0;
                        return;
                        }

     TimePointLast = std::chrono::high_resolution_clock::now(); Counter++; 
    }

    void operator++(int) { PushTick();}
    float TickPeriod    {0};
    float TickFrequency {0};

    float getMilliseconds() { return TickPeriod;      };
    float getMicroseconds() { return TickPeriod*1000; };
    float getSeconds()      { return TickPeriod/1000; };

    QString printPeriod()
    {
      return QString("[ PROCESS PERIOD MILLI ] %1").arg(TickPeriod); 
    };

    private:
    std::chrono::time_point<std::chrono::high_resolution_clock> TimePointLast;
    std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    std::chrono::duration<double> Duration;
                              int Counter = 0;
};

#endif
