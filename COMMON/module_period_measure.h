#ifndef PERIOD_MEASURE_H
#define PERIOD_MEASURE_H
#include <chrono>
#include <QString>

class FramePeriodMeasure
{
    public:
    void PushTick()
    {
    if(counter >= 1)  { TimePoint =   std::chrono::high_resolution_clock::now();
                        Duration  = TimePoint - TimePointLast; 
                        TimePointLast = TimePoint;
                        FramePeriod   = Duration.count()*1000;
                        FrameFrequency = 1/Duration.count(); 
                        counter = 0;
                        return;
                        }

     TimePointLast = std::chrono::high_resolution_clock::now(); counter++; 

    }
    void operator++(int) { PushTick();}
    float FramePeriod{0};
    float FrameFrequency{0};
    int counter = 0;

    QString printPeriod()
    {
      return QString("[ PROCESS PERIOD ] %1").arg(FramePeriod*1000); 
    };

    private:
    std::chrono::time_point<std::chrono::high_resolution_clock> TimePointLast;
    std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    std::chrono::duration<double> Duration;
};

#endif
