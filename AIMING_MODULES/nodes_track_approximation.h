#ifndef TRACK_APPROXIMATION_CLASS_H
#define TRACK_APPROXIMATION_CLASS_H
#include <QDebug>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Core/Matrix.h>
#include <interface_pass_coord.h>
#include <initializer_list>
#include <iomanip>
#include "thread_operation_nodes.h"
#include "module_period_measure.h"

class GenericApproximation: public PassCoordClass<float>
{
  public:
  GenericApproximation(int Size = 100, int SizeRollbak=10, int StepsFuture = 2) 
  {
      TrackInput.setSize(Size); 
      TrackFuture.setSize(Size); 
      SizeWindow = Size;
      StepsRollback = SizeRollbak;
      StepsForecasting = StepsFuture;
  }
  int   SizeWindow = 100;
  int   StepsRollback = SizeWindow/10;
  float StepTimeScale = 0.5;
  int   StepsForecasting = 4;

  NodeValueAvarageStep<float> NodeAvarageStep{10};
  NodeCoordStorage<float> TrackInput;
  NodeCoordStorage<float> TrackFuture;

  std::pair<float,float> posFuture{0,0};
  std::pair<float,float> posLast{0,0};

  double RMSE = std::numeric_limits<double>::quiet_NaN();
  bool isResultValid = false;

    MeasurePeriodNode MeasurePeriod;
    void setInput(const QPair<float,float>& Coord) override  
    {   
                Coord >> TrackInput; Coord.first >> NodeAvarageStep >> StepTimeScale; if(!TrackInput.isLoaded()) return; 
        getApproximation(TrackInput); 
                         TrackInput.skipLoaded(); 
                         TrackInput.rollbackStore(SizeWindow - StepsRollback);  
                                        
    };
    void flushTrack() { TrackInput.skipLoaded(); };

	  const QPair<float, float>& getOutput() override { return getFutureStep();};

    virtual bool isLoaded() { return TrackInput.isLoaded();}
    virtual std::tuple<float,float,float,float,bool> getResult() = 0;

    friend void operator>>(const std::vector<std::pair<float,float>>& coords, GenericApproximation& receiver) { for(auto& coord: coords) coord >> receiver;}

    virtual const std::pair<float,float>& getFutureStep() = 0;
    virtual NodeCoordStorage<float>& getFuture() = 0;

    virtual std::vector<float> getApproximation(std::vector<std::pair<float,float>>& track) = 0;  
    virtual std::vector<float> getApproximation(NodeCoordStorage<float>& track) = 0;  
    virtual std::vector<float> getApproximation(std::span<std::pair<float,float>> track) = 0;  
};

template<int NUM_PARAM>
class PolynomApproximation: public GenericApproximation
{
  public:
    PolynomApproximation(int Size = 100, int SizeRollback = 10, int StepsFuture = 2) : GenericApproximation(Size, SizeRollback, StepsFuture)
    { 
      A_MAT1 = Eigen::MatrixXd(static_cast<Eigen::Index>(Size), 1);
      A_MAT3 = Eigen::MatrixXd(static_cast<Eigen::Index>(Size), 3);
       Y_VEC = Eigen::VectorXd(static_cast<Eigen::Index>(Size));

       TrackInput.setRollbackAuto(false);
       TrackFuture.setRollbackAuto(false);
    };

    void operator=(const PolynomApproximation<NUM_PARAM>& copy);
    bool isLoaded() override { return IndexInput == SizeWindow;}

    void reset(); 

    Eigen::MatrixXd A_MAT1;
    Eigen::MatrixXd A_MAT3;
    Eigen::VectorXd Y_VEC;
    std::vector<float> sums{0,0,0,0};

    std::vector<float> trackPolynom{0,0,0,0};

    Eigen::Index IndexInput = 0;

    void setInput(const QPair<float,float>& Coord) override;

    std::vector<float> getApproximation(std::vector<std::pair<float,float>>& track) override  { std::span<std::pair<float,float>> span{track}; getApproximation(span); return this->trackPolynom; }
    std::vector<float> getApproximation(NodeCoordStorage<float>& track) override;  
    std::vector<float> getApproximation(std::span<std::pair<float,float>> track) override;  

    std::tuple<float,float,float,float,bool> getResult() override { return { trackPolynom[3], trackPolynom[2], trackPolynom[1], trackPolynom[0],true};};

    const std::pair<float,float>& getFutureStep() override;
    NodeCoordStorage<float>& getFuture() override;
};


template<int NUM_PARAM>
void PolynomApproximation<NUM_PARAM>::operator=(const PolynomApproximation<NUM_PARAM>& copy)
{
	SizeWindow = copy.SizeWindow;

	A_MAT1 = Eigen::MatrixXd(static_cast<Eigen::Index>(SizeWindow), 1);
	A_MAT3 = Eigen::MatrixXd(static_cast<Eigen::Index>(SizeWindow), 3);
	 Y_VEC = Eigen::VectorXd(static_cast<Eigen::Index>(SizeWindow));
};

template<int NUM_PARAM>
const std::pair<float,float>& PolynomApproximation<NUM_PARAM>::getFutureStep()
{
    posFuture.first = posLast.first + StepsForecasting*StepTimeScale;  
    posFuture.second = trackPolynom[2]*std::pow(posFuture.first,2) +
                       trackPolynom[1]*posFuture.first +
                       trackPolynom[0];
    //qDebug() << "POS LAST: " << posLast.first << posLast.second << "FUTURE: " << posFuture.first << posFuture.second; 
    return posFuture;
}

template<int NUM_PARAM>
NodeCoordStorage<float>& PolynomApproximation<NUM_PARAM>::getFuture()
{
    //float step = track[SizeWindow-1].first - track[SizeWindow-2].first; 
    
    if(!isLoaded()) return TrackFuture;

    auto pos1 = TrackInput.begin();
    auto pos2 = TrackInput.begin()+1;
    float step = 0;
    for(int n = 0; n < 10; n++) { step += ((*pos2).first - (*pos1).first)/10; pos1++; pos2++; } 

    TrackFuture.reset();
    for(auto coord: TrackInput)
    {
    coord.first = coord.first + 2*StepTimeScale;  
    coord.second = trackPolynom[2]*std::pow(coord.first,2) +
                   trackPolynom[1]*coord.first +
                   trackPolynom[0];
    coord >> TrackFuture;
    }

    return TrackFuture;
}


template<int NUM_PARAM>
class ApproximationTwoAxis: public PassCoordClass<float>
{
  public:
    explicit ApproximationTwoAxis(int Size = 100, int SizeRollback = 10, int StepsFuture = 2):
    trackApprox1(Size, SizeRollback, StepsFuture),
    trackApprox2(Size, SizeRollback, StepsFuture)
    { 
    };

    bool isLoaded() { return trackApprox1.isLoaded();}

    void reset() { trackApprox1.reset(); trackApprox2.reset(); }

       NodeCoordPassValue<float> PickValue;
       NodeCoordJoinValue<float> JoinValue;
     NodeCoordSplitToTime<float> SplitToTime; 

    PolynomApproximation<NUM_PARAM> trackApprox1;
    PolynomApproximation<NUM_PARAM> trackApprox2;

    void setInput(const QPair<float,float>& Coord) override
    {
     Coord >> SplitToTime(0) >> trackApprox1 >>  PickValue(1) >> JoinValue; 
              SplitToTime(1) >> trackApprox2 >>  PickValue(1) >> JoinValue >> PassCoordClass<float>::OutputCoord;  
    }

};


#endif 

    //Eigen::VectorXd residual = A_MAT * PARAMS_FUNC - Y_VEC;
    //const double mse = residual.squaredNorm() / static_cast<double>(SizeWindow);
    //RMSE = std::sqrt(mse);
