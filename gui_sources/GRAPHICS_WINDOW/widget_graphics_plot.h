#ifndef WIGET_GRAPHICS_PLOT
#define WIGET_GRAPHICS_PLOT

#include "interface_pass_coord.h"
#include "ui_widget_graphics_plot.h"
#include "qcustomplot.h"
#include "widget_adjustable.h"


class PlotGraphicsInterface : public QObject, public PassCoordClass<float>
{
	Q_OBJECT
public:
	PlotGraphicsInterface(QCustomPlot* Graphics, int NumberChannels = 1);
	~PlotGraphicsInterface();

	void CreateGraphics(int NumberChannel);

	QCustomPlot* GraphicsDisplay;
	QElapsedTimer TimeMeasurePeriod;
	std::vector<QPen> Pens;
	std::vector<QBrush> Brushes;


    template<typename INPUT_TYPE>
    void DisplaySingleSeriesGeneric(const std::vector<INPUT_TYPE>& Values);
    template<typename INPUT_TYPE, typename OUTPUT_TYPE>
    void DisplaySeriesPairGeneric(const std::vector<INPUT_TYPE>& Values1,const std::vector<OUTPUT_TYPE>& Values2);
    template<typename INPUT_TYPE, typename OUTPUT_TYPE>
    void DisplaySeriesPairGeneric(const std::vector<INPUT_TYPE>& Values1,const std::vector<OUTPUT_TYPE>& Values2, 
                                                                         const std::vector<OUTPUT_TYPE>& Values3);
    template<typename INPUT_TYPE, typename OUTPUT_TYPE>
    void DisplayTimeSeriesGeneric(const std::vector<INPUT_TYPE>& Times, const std::vector<OUTPUT_TYPE>& Values);


    void SetTimeScale(double StepMilli, double LimitMilli) { TimePos = 0; TimeStep = StepMilli; TimeLimit = LimitMilli; }

  friend void operator>>(QPair<float,float> Point, PlotGraphicsInterface& Graph); 
  void DisplayPair(double pos,double pos2);

	void setInput(const QPair<float,float>& Coord) 
  {
    OutputCoord = Coord;
    DisplayPair(Coord.first, Coord.second);
  };
  void GenerateTimeSeries(double TimeStep,int length);

    int ChannelsCount = 1;

    double TimePos = 0;
    double TimeStep = 1;
    double TimeLimit = 300;

    QVector<double> InputSeries1;
    QVector<double> InputSeries2;
    QVector<double> InputSeries3;
    QVector<double>  TimeSeries;

public slots:

  void slotDisplayCoord(QPair<float,float> Point);

  void slotDisplayTimeSeries();
  void slotDisplayTimePairSeries();

  //void SetLabelAxisX(QString label_str);

signals: 
  void SignalSeriesPlot();
  void SignalSeriesPairPlot();
};

//template<typename V>
//void PlotGraphicsInterface::DisplayGraph(std::vector<V>& Data)
template<typename INPUT_TYPE>
void PlotGraphicsInterface::DisplaySingleSeriesGeneric(const std::vector<INPUT_TYPE>& Values)
{

   if(TimeSeries.size() != Values.size())
   {
    GenerateTimeSeries(1,Values.size());
     InputSeries1.resize(Values.size());  
   }

   std::copy(InputSeries1,InputSeries1.end(), Values);

   emit SignalSeriesPlot();

}


template<typename INPUT_TYPE, typename OUTPUT_TYPE>
void PlotGraphicsInterface::DisplayTimeSeriesGeneric(const std::vector<INPUT_TYPE>& Times, const std::vector<OUTPUT_TYPE>& Values)
{

   if(TimeSeries.size() != Times.size())
   {
      TimeSeries.resize(Times.size()); 
    InputSeries1.resize(Times.size());  
   }

   for(int n = 0; n < Times.size(); n++)
   {
      TimeSeries[n] =  Times[n]; 
    InputSeries1[n] = Values[n]; 
   }

   emit SignalSeriesPlot();

}


template<typename INPUT_TYPE, typename OUTPUT_TYPE>
void PlotGraphicsInterface::DisplaySeriesPairGeneric(const std::vector<INPUT_TYPE>& Values1,const std::vector<OUTPUT_TYPE>& Values2)
{
   if(TimeSeries.size() != Values1.size() || InputSeries1.size() != TimeSeries.size())
   {
    InputSeries1.resize(Values1.size());  
    InputSeries2.resize(Values1.size()); 
      TimeSeries.resize(Values1.size());
   }  

   double TimePos = 0;
   for(int n = 0; n < Values1.size(); n++)
   {
    InputSeries1[n] = Values1[n]; 
    InputSeries2[n] = Values2[n]; 

                      TimePos += TimeStep;
      TimeSeries[n] = TimePos;
   }

   emit SignalSeriesPairPlot();
}

template<typename INPUT_TYPE, typename OUTPUT_TYPE>
void PlotGraphicsInterface::DisplaySeriesPairGeneric(const std::vector<INPUT_TYPE>& Values1,const std::vector<OUTPUT_TYPE>& Values2,
                                                                             const std::vector<OUTPUT_TYPE>& Values3)
{

   if(TimeSeries.size() != Values1.size() || InputSeries1.size() != TimeSeries.size())
   {
    InputSeries1.resize(Values1.size());  
    InputSeries2.resize(Values1.size()); 
    InputSeries3.resize(Values1.size()); 
      TimeSeries.resize(Values1.size());
   }  

   double TimePos = 0;
   for(int n = 0; n < Values1.size(); n++)
   {
    InputSeries1[n] = Values1[n]; 
    InputSeries2[n] = Values2[n]; 
    InputSeries3[n] = Values3[n]; 

                      TimePos += TimeStep;
      TimeSeries[n] = TimePos;
   }

   emit SignalSeriesPairPlot();
}


class WidgetGraphisPlot : public WidgetAdjustable, public PassCoordClass<float> 
{
	Q_OBJECT

public:
	WidgetGraphisPlot(int NumberChannels = 1, QWidget *parent = 0);
	~WidgetGraphisPlot();

	PlotGraphicsInterface* Graph1 = nullptr;

private:
	Ui::WidgetGraphisPlot ui;

};

#endif // WIGET_GRAPHICS_PLOT
