#ifndef STATISTICS_TRACK
#define STATISTICS_TRACK

#include "interface_pass_coord.h"
#include "interface_pass_value.h"
#include <math.h>
#include <functional>
#include <QQueue>
#include <QDebug>
#include "debug_output_filter.h"

#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Core/Matrix.h>
#include "engine_statistics.h"
#include "thread_operation_nodes.h"
#include "nodes_track_approximation.h"
#include "module_period_measure.h"

template<typename T = float, int type=2>
class EstimatorDispersionCoordSpan: public PassCoordClass<T>
{
  public:
  EstimatorDispersionCoordSpan(int SizeWindow) { };

  QPair<T,T> CoordSlowed{0,0};

  NodeCoordAvarageStep<T> NodeAvarageStep{5};
  StatisticCoord<T> StatFast {3}; 
  StatisticCoord<T> StatLong {300}; 
  StatisticCoord<T> StatFast2{20}; 
  StatisticCoord<T> StatLong2{50}; 
  StatisticCoord<T> StatTest {10}; 
  StatisticCoord<T> StatTest2{20}; 

  StatisticValue<T> Stat1_1{10}; 
  StatisticValue<T> Stat1_2{10}; 

  NodeCoordPassThinning<T> Thinning;
     NodeCoordPassValue<T> PickValue;
     NodeCoordDifference<T> Substract;

  NodeCoordGain<T> Gain;
  NodeCoordGain<T> Inversion{-1};

  NodeCoordJoinValue<T> Join;
   NodeCoordAbsolute<T> Abs;
   NodeCoordMultiply<T> Multiply;

  std::pair<T,T> RangeStepMax;
  std::pair<T,T> RangeStepCurrent;
  std::pair<T,T> Scale;
  std::pair<T,T> RangeDispersion;

  std::pair<T,T> Step;
  float gain = 0.9;

	const QPair<T,T>& getOutput() override { return PassCoordClass<T>::OutputCoord;};
	void setInput(const QPair<T,T>& Coord) override  
  {
     Coord >> NodeAvarageStep >> Substract;       
                        Coord >> Substract >> CoordSlowed >> PickValue(0) >> Stat1_1; 
                                              CoordSlowed >> PickValue(1) >> Stat1_2;
     //qDebug() << "COORD SLOWED: " << CoordSlowed.first << CoordSlowed.second;
     NodeAvarageStep >> Abs >> StatLong; 
     NodeAvarageStep >> Abs >> StatFast; 

     if(Stat1_1.isLoaded())
     {

      StatFast.GetAvarageCoord() >> Substract;
      StatLong.GetMaxCoord()     >> Substract >> RangeStepCurrent;                   
                                                 RangeStepMax  = StatLong.GetRange(); StatLong.resetOnLoaded();
      Step = StatFast.GetAvarageCoord();

      RangeDispersion.first  = Stat1_1.GetRange(); Stat1_1.reset(); 
      RangeDispersion.second = Stat1_2.GetRange(); Stat1_2.reset();

                        Scale = (RangeStepCurrent/RangeStepMax)*gain + (1-gain);
      RangeDispersion = Scale*RangeDispersion; 
      RangeDispersion >> StatLong2;
      RangeDispersion = StatLong2.GetMaxCoord(); 

      if(StatLong2.isLoaded()) qDebug() << "[ COORD SPAN ]" <<  RangeDispersion.first << RangeDispersion.second;
         StatLong2.resetOnLoaded();
     }
  };
};

template<typename T>
class EstimatorDispersionCoordSpan<T,1>: public PassValueClass<T>
{
  public:
  EstimatorDispersionCoordSpan(int SizeWindow) { };

  T ValueSlowed{0};

  NodeValueAvarageStep<T> NodeAvarageStep{5};
  StatisticValue<T> StatFast {3}; 
  StatisticValue<T> StatLong {300}; 
  StatisticValue<T> StatFast2{20}; 
  StatisticValue<T> StatLong2{50}; 
  StatisticValue<T> StatTest {10}; 
  StatisticValue<T> StatTest2{20}; 

  StatisticValue<T> StatFast3{10}; 

  NodeValueDifference<T> Substract;
  NodeValueInversion<T> Inversion;

  NodeValueAbsolutization<T> Abs;
  NodeValueMultiply<T> Multiply;

  T RangeStepMax;
  T RangeStepCurrent;
  T Scale;
  T RangeDispersion;
  T Step;
  float gain = 0.9;
  T OutputValue;
  T ValueTest;

	void setValue(T Value ) override  
  {
     Value >> NodeAvarageStep >> Substract;       
                        Value >> Substract >> ValueSlowed >> StatFast3; 

     //qDebug() << "[ CALC SPAN ]" << Value << " SLOWED: " << ValueSlowed;
     NodeAvarageStep >> Abs >> StatLong; 
     NodeAvarageStep >> Abs >> StatFast; 

     if(StatFast3.isLoaded())
     {

      StatFast.GetAvarageValue() >> Substract ;
      StatLong.GetMaxValue()     >> Substract >> RangeStepCurrent;                   
                                                 RangeStepMax  = StatLong.GetRange(); StatLong.resetOnLoaded();
      Step = StatFast.GetAvarageValue();

      RangeDispersion  = StatFast3.GetRange(); StatFast3.reset(); 

                        Scale = (RangeStepCurrent/RangeStepMax)*gain + (1-gain);
      RangeDispersion = Scale*RangeDispersion; 
      RangeDispersion >> StatLong2;
      RangeDispersion = StatLong2.GetMaxValue(); 
      PassValueClass<T>::Value = RangeDispersion;

      //if(StatLong2.isLoaded()) qDebug() << "[ COORD SPAN ]" <<  RangeDispersion;
         StatLong2.resetOnLoaded();
     }
  };
};

template<typename T = float>
class EstimatorVelocityAvarageStep: public PassCoordClass<T>
{
  public:
   EstimatorVelocityAvarageStep() { }

	void setInput(const QPair<T,T>& Coord) override  
  {

    Coord >> Thinning1(2)  >> NodeAvarageStep1 >> Norm >> Velocity1 >> Categorizer1 >> Velocity1Level;
    Coord >> Thinning2(4)  >> NodeAvarageStep2 >> Norm >> Velocity2 >> Categorizer2 >> Velocity2Level;
    Coord >> Thinning3(6) >> NodeAvarageStep3 >> Norm >> Velocity3 >> Categorizer3 >> Velocity3Level;

    probabiltyMoving = 100*(Velocity1Level/10 + Velocity2Level/10 + Velocity3Level/10)/3;
    //qDebug() << OutputFilter::Filter(20) << "[ VELOCITY ESTIMATOR ]" << Velocity1 << Velocity2 << Velocity3 
    //                                     << "[CAT]" << Velocity1Level 
    //                                                << Velocity2Level
    //                                                << Velocity3Level;
  }
  void reset() { probabiltyMoving = 0; NodeAvarageStep1.reset(); NodeAvarageStep2.reset(); NodeAvarageStep3.reset(); }

    float getMovingProbability() { return probabiltyMoving;        }
                 bool isMoving() { return probabiltyMoving > 50;}

    T Velocity1{0};
    T Velocity2{0};
    T Velocity3{0};

    T Velocity1Level{0};
    T Velocity2Level{0};
    T Velocity3Level{0};
    T probabiltyMoving{0};

        NodeCoordDifference<T> Substract;
          NodeCoordPassNorm<T> Norm;
    NodeValueAbsolutization<T> Abs;

    NodeCoordPassThinning<T> Thinning1{2};
    NodeCoordPassThinning<T> Thinning2{4};
    NodeCoordPassThinning<T> Thinning3{8};

    NodeValueCategorizer<T> Categorizer1{5,10};
    NodeValueCategorizer<T> Categorizer2{10,10};
    NodeValueCategorizer<T> Categorizer3{10,10};

    NodeCoordAvarageStep<T> NodeAvarageStep1{10};
    NodeCoordAvarageStep<T> NodeAvarageStep2{10};
    NodeCoordAvarageStep<T> NodeAvarageStep3{10};
};

class EstimatorObjectMovingParams: public PassCoordClass<float>
{
  public:
    EstimatorObjectMovingParams() 
    { 
      Statistic1.setModeSingle(true);
      Statistic2.setModeSingle(true);
      Statistic3.setModeSingle(true);
    };

    //======================================
    QPair<float,float> CoordProlong{0,0};
    QPair<float,float> CoordAvarage{0,0};
    QPair<float,float> Velocity; 
    QPair<float,float> Acceleration; 
                float  VelocityNorm = 0; 
                float  AccelerationNorm = 0; 
                float  DispersionCoord = 0; 
                float  DispersionCoordSpan = 0; 
    //======================================

    private:
    StatisticCoord<float> Statistic1{10};  
    StatisticCoord<float> Statistic2{10}; 
    StatisticCoord<float> Statistic3{10}; 

    EstimatorDispersionCoordSpan<float,1> StatisticSpan{10};

      NodeCoordDifference<float> Substract;
        NodeCoordAbsolute<float> Abs;
        NodeCoordPassNorm<float> Norm;
       NodeCoordPassValue<float> PickValue;
       NodeCoordJoinValue<float> JoinValue;
     NodeCoordSplitToTime<float> SplitToTime; 
     NodeCoordGain<float> Gain;

    NodeCoordVelocity<float> NodeVelocity;
    NodeCoordVelocity<float> NodeAcceleration;

    ApproximationTwoAxis<3> trackApprox{100,10,4};


               MeasurePeriodNode MeasurePeriod;


  public:
  void reset() { }

	const QPair<float,float>& getOutput() override { return OutputCoord;};

	void setInput(const QPair<float,float>& Coord) override  
  {
     Coord >> trackApprox >> CoordProlong >> Substract;
                                    Coord >> Substract >> Statistic1;

                             CoordProlong >> NodeVelocity >> Gain(0.1) >> Statistic2; 
                                             NodeVelocity >> Gain(0.001) >> NodeAcceleration >> Statistic3;
     Coord >> Norm >> StatisticSpan >> DispersionCoordSpan;

      if(!Statistic1.isLoaded()) return;

       if(Statistic1.isLoaded()){ DispersionCoord = Statistic1.GetDispersionNorm(); 
                                  CoordAvarage = Statistic1.GetAvarageCoord();        Statistic1.reset();}

       if(Statistic2.isLoaded()){ Velocity = Statistic2.GetAvarageCoord();                       
                                  VelocityNorm = Statistic2.GetDispersionNorm();      Statistic2.reset();}

       if(Statistic3.isLoaded()){ Acceleration = Statistic3.GetAvarageCoord();                       
                                  AccelerationNorm = Statistic3.GetDispersionNorm();  Statistic3.reset();}

       printState(2);

  };

  void printState(int thinning)
  {

       //qDebug() << OutputFilter::Filter(thinning) << "[ TRACK STAT ]" <<"[DISP ]" << DispersionCoord;
       //qDebug() << OutputFilter::Filter(thinning) << "[ TRACK STAT ]" <<"[VEL  ]" << Velocity.first << Velocity.second;
       //qDebug() << OutputFilter::Filter(thinning) << "[ TRACK STAT ]" <<"[ACCEL]" << Acceleration.first << Acceleration.second;
       qDebug() << OutputFilter::Filter(thinning) << "[DISP ]" << DispersionCoord
                                                  << "[SPAN ]" << DispersionCoordSpan
                                                  << "[VEL  ]" << VelocityNorm;
                                                  //<< "[ACCEL]" << Acceleration.first;
                                            //<< "[AVG  ]" << CoordAvarage.first << CoordAvarage.second
  }
};

template<typename T = float>
class EstimatorTrackHold: public PassCoordClass<T>
{
  public:
   EstimatorTrackHold() { }

	void setInput(const QPair<T,T>& Coord) override  
  {
    Coord >> EstimatorVelocity;
    //Coord >> EstimatorStatParams; EstimatorStatParams.DispersionCoordSpan >> Categorizer1 >> DispersionLevel; 
    //isDispersionLimit = DispersionLevel < 5;

    //qDebug() << OutputFilter::Filter(20) << "[ MOVING ]" << EstimatorVelocity.isMoving() << EstimatorVelocity.getMovingProbability();
                                        //<< "[ DISP]" << EstimatorStatParams.DispersionCoordSpan << isDispersionLimit
                                        //<< "[ COMBINE ]" << (EstimatorVelocity.isMoving() && isDispersionLimit);
    PassCoordClass<T>::OutputCoord.first = EstimatorVelocity.getMovingProbability();
  }
  void reset() { EstimatorVelocity.reset();}

    EstimatorVelocityAvarageStep<float> EstimatorVelocity;
            EstimatorObjectMovingParams EstimatorStatParams;

       T DispersionLevel{0};
    bool isDispersionLimit{false};

    NodeValueCategorizer<T> Categorizer1{5,10};
    NodeValueCategorizer<T> Categorizer2{20,20};
    NodeValueCategorizer<T> Categorizer3{40,20};

    NodeCoordAvarageStep<T> NodeAvarageStep1{10};
    NodeCoordAvarageStep<T> NodeAvarageStep2{10};
    NodeCoordAvarageStep<T> NodeAvarageStep3{10};

           bool isMoving() { return EstimatorVelocity.isMoving(); } ;
    float getProbability() { return EstimatorVelocity.getMovingProbability(); }
};

#endif
