#ifndef WIGET_GRAPHICS_PLOT
#define WIGET_GRAPHICS_PLOT

#include "ui_widget_graphics_plot.h"
#include "qcustomplot.h"


class SmoothenodeClass
{
public:
	QQueue<QPair<double, double>> MeasureMassive;

	int Size = 10;
	QPair<double, double> Avarage;

    QPair<double, double> GetAvarage()
    {
        return Avarage;
    };

    int MAX_VALUE = 0;
    void CHECK_MAX(QPair<double,double> Coord) { if(std::abs(Coord.first) > MAX_VALUE)MAX_VALUE = Coord.first;
                                                 if(std::abs(Coord.second) > MAX_VALUE)MAX_VALUE = Coord.second;}

    friend void operator>>(SmoothenodeClass& Object, QPair<double,double>& Coord)
    {
        Coord = Object.Avarage;
    };

	friend void operator>>(QPair<double, double> NewValue, SmoothenodeClass& StatObj)
	{
		StatObj.MeasureMassive.enqueue(NewValue);

		StatObj.Avarage.first += NewValue.first/StatObj.Size;
		StatObj.Avarage.second += NewValue.second/StatObj.Size;
        StatObj.CHECK_MAX(NewValue);


		if (StatObj.MeasureMassive.size() > StatObj.Size)
		{
			QPair<double,double> FirstValue =  StatObj.MeasureMassive.dequeue();
			
		StatObj.Avarage.first -= FirstValue.first/StatObj.Size;
		StatObj.Avarage.second -= FirstValue.second/StatObj.Size;
		}
	};

};

class WidgetGraphicsPlot : public QObject
{
	Q_OBJECT
public:
	WidgetGraphicsPlot(QCustomPlot* Graphics, int NumberChannels = 1);
	~WidgetGraphicsPlot();

	void CreateGraphics(int NumberChannel);

	QCustomPlot* GraphicsDisplay;
	QElapsedTimer TimeMeasurePeriod;

	double TimeFromStartMs;
	double TimePeriod= 0;
	double PlotPeriod= 0;
	double TimePoint;
    int ChannelsCount = 1;

	std::vector<QPen> Pens;
	std::vector<QBrush> Brushes;

	std::vector<SmoothenodeClass> GraphSmootheFilters;

	std::vector<bool>  DisplayFlags;
	std::vector<bool>  DisplaySmootheFlags;
	std::array<bool,2> DisplayAxis{true,true};

	QPair<int, int> RangeY;

	template<typename V> friend void operator>>(std::vector<V> Data, WidgetGraphicsPlot& Display) { Display.DisplayGraph(Data); } 
	template<typename V> void DisplayGraph(std::vector<V> Data)
		{
			
			TimePeriod = TimeMeasurePeriod.restart();
			TimeFromStartMs += TimePeriod;
			PlotPeriod += TimePeriod;
			TimePoint = TimeFromStartMs/1000;  
			int DataCount = Data.size(); if(DataCount > ChannelsCount) DataCount = ChannelsCount;

			for(int n = 0; n < DataCount; n++) { GraphicsDisplay->graph(n)->addData(TimePoint, Data[n]); }

			if(TimeFromStartMs > 4200) 
			for(int n = 0; n < ChannelsCount; n++) { GraphicsDisplay->graph(n)->data()->clear(); TimeFromStartMs = 0;}

			GraphicsDisplay->xAxis->setRange(TimePoint, 4, Qt::AlignRight);
			GraphicsDisplay->replot();
			PlotPeriod = 0;
		}

};

//template<typename V>
//void WidgetGraphicsPlot::DisplayGraph(std::vector<V>& Data)


class GraphicsWindow : public QWidget
{
	Q_OBJECT

public:
	GraphicsWindow(int NumberChannels = 1, QWidget *parent = 0);
	~GraphicsWindow();

public slots:
	void DisplayDataVector(std::vector<uint16_t> Data);
	void DisplayDataVector(std::vector<int> Data);
	void DisplayDataVector(std::vector<float> Data);
	void DisplayDataVector(std::vector<double> Data);
	void ChangeDataLimit(double DataLimitup, double DataLimitDown);

private:
	Ui::GraphicsWindow ui;

	WidgetGraphicsPlot* Graph1;
};

#endif // WIGET_GRAPHICS_PLOT
