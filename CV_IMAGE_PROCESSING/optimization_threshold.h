#ifndef OPTIMIZTION_THREDHOLD_H
#define OPTIMIZTION_THREDHOLD_H

#include <memory>
#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>
#include "engine_statistics.h"
#include "interface_pass_coord.h"
#include "thread_operation_nodes.h"
#include "debug_output_filter.h"
#include <QtConcurrent/QtConcurrent>
#include "contour_processing.h"

//class ContoursProcessorClass;

template<typename T = double>
class ValueSpongeShift : public PassValueClass<T>
{
  public:
  double Speed = 0.001;
  double InputValue = 0;
  T& GetValue() override {        Speed = StatNode2.GetAvarageValue(); 
                                  PassValueClass<T>::Value = InputValue - StatNode.GetAvarageValue()*Speed; 
                                  double(0) >> StatNode; double(0) >> StatNode;
                           return PassValueClass<T>::Value; }  

  void SetValue(T Value) override {InputValue = Value; Value >> TimeDiffer >> Saturation(Value/50) >> InversionLogic >> StatNode;
                                                                TimeDiffer >> StatNode2;}
  StatisticNode<double> StatNode{20};
  StatisticNode<double> StatNode2{20};
  ValueSaturation<double>      Saturation;
  ValueBinaryInversion<double> InversionLogic;
  ValueDifference<double>      TimeDiffer;

  ValueSpongeShift<T>& operator()(double SpongeSpeed){ Speed = SpongeSpeed; return *this;};
};

class OptimizationThreshold : public PassValueClass<double>
{
	public:

	double InputValue = 0;
	double ValueOffset = 0;
	bool DISABLED = false;

  void Disable(){ DISABLED = true;};
	bool isEnabled() { return !DISABLED;}
  void SetValue(double NewValue) override {InputValue = NewValue;}
  virtual void Reset() = 0;

  virtual void CalcThreshold(const cv::Mat& Image) = 0;

	friend OptimizationThreshold& operator>>(const cv::Mat& Image, OptimizationThreshold& Node) 
  {if(Node.DISABLED) return Node; Node.CalcThreshold(Image); return Node; }

	StatisticNode<double>  StatNode{40};
};

class ThresholdCalculationLumen : public OptimizationThreshold
{
	public:
  void CalcThreshold(const cv::Mat& Image) override;
  void Reset() override {};
};

class ThresholdAdjustionDispersion : public OptimizationThreshold
{
	public:
  void CalcThreshold(const cv::Mat& Image) override;
	    ValueSpongeShift<double> SpongeValue;
  void Reset() override {};
};

class ThresholdFindingParallelDispersion : public OptimizationThreshold
{
	public:
  ThresholdFindingParallelDispersion(int MinThreshold, int Step, int NumberGroup);
  void CalcThreshold(const cv::Mat& Image) override;

  int  GetResult();
  void SetParam(int MinThreshold, int Step, int NumberGroup);

  void Reset();
  int MinThreshold = 0;
  int Step = 10;
  int NumberGroup = 10;
  cv::Mat InputImage;

  std::vector<StatisticNode<double>> GroupStatistics;
  std::vector<ContoursProcessorClass> ControursProcessors;
  std::vector<double>    GroupDispersion;
  std::vector<cv::Mat>    GroupImages;

  std::vector<int> GroupThreshold;
	std::vector<std::function<void(int, cv::Mat)>> ProcessFunctionList;
};

enum class PROCESS_METHOD { PARALLEL_BY_DISPERSION, CALC_BY_LUMEN, RUNAWAY_FROM_DISPERSION};

class ThresholdOptimizatorEngine : public OptimizationThreshold
{
  public:
  ThresholdOptimizatorEngine(){CurrentProcess = &NodeDispersionRunaway;};
  ThresholdOptimizatorEngine(PROCESS_METHOD Method)
  {
     if(Method == PROCESS_METHOD::CALC_BY_LUMEN)           CurrentProcess = &NodeGetForLumen;
     if(Method == PROCESS_METHOD::PARALLEL_BY_DISPERSION)  CurrentProcess = &NodeFindingParallel;
     if(Method == PROCESS_METHOD::RUNAWAY_FROM_DISPERSION) CurrentProcess = &NodeDispersionRunaway;
  };

  ThresholdFindingParallelDispersion NodeFindingParallel{20,10,18};
        ThresholdAdjustionDispersion NodeDispersionRunaway;
        ThresholdCalculationLumen    NodeGetForLumen;
  void Reset() override { CurrentProcess->Reset();}
  void CalcThreshold(const cv::Mat& Image) override { Image >> *CurrentProcess >> Value; };

  void SetParam(int MinThreshold, int Step, int NumberGroup) {MinThreshold = MinThresholdFinding; MaxThresholdFinding = MinThreshold + Step*NumberGroup;  
                                                              NodeFindingParallel.SetParam(MinThreshold, Step, NumberGroup);};
  int MinThresholdFinding = 10;
  int MaxThresholdFinding = 250;
  
  OptimizationThreshold* CurrentProcess = &NodeFindingParallel;

};

#endif //OPTIMIZTION_THREDHOLD_H
