#include "widget_graphics_plot.h"


PlotGraphicsInterface::PlotGraphicsInterface(QCustomPlot* Graphics, int NumberChannels)
{
	GraphicsDisplay = Graphics;
	qDebug() << "[ CREATE GRAPHICS INTERFACE SIZE ]" << NumberChannels;

    Pens.push_back(QPen(QColor(24, 148, 200)));  Pens.back().setWidth(2);
    Pens.push_back(QPen(QColor(130, 248, 10)));  Pens.back().setWidth(2);
    Pens.push_back(QPen(QColor(74, 48, 100)));   Pens.back().setWidth(2);
    Pens.push_back(QPen(QColor(184, 100, 130))); Pens.back().setWidth(2);
    Pens.push_back(QPen(QColor(124, 148, 110))); Pens.back().setWidth(2);

    Brushes.push_back(QBrush(QColor(24 + 80, 148 + 30, 200,50)));  
    Brushes.push_back(QBrush(QColor(130+ 80, 188 + 30, 10,50)));  
    Brushes.push_back(QBrush(QColor(74 + 80, 48  + 30, 100,50))); 
    Brushes.push_back(QBrush(QColor(184+ 80, 100 + 30, 130,50))); 
    Brushes.push_back(QBrush(QColor(124+ 80, 148 + 30, 110,50))); 

    for(int n = Pens.size(); n < NumberChannels; n++)
	{
    Pens.push_back(QPen(QColor(5*n, 10*n, 8*n))); Pens.back().setWidth(5);
    Brushes.push_back(QBrush(QColor(5*n + 80, 10*n + 30, 8*n))); 
	}

	this->CreateGraphics(NumberChannels);
	ChannelsCount = NumberChannels;
}

void PlotGraphicsInterface::CreateGraphics(int Number)
{
	
	for(int n = 0; n < Number; n++)
	{
		GraphicsDisplay->addGraph(); 
		GraphicsDisplay->graph(n)->setPen(Pens[n]);
		GraphicsDisplay->graph(n)->setBrush(Brushes[n]);
        //GraphicsDisplay->graph(n)->setAntialiasedFill(false);

	}

    GraphicsDisplay->yAxis->setRange(-5000,5000);
    GraphicsDisplay->xAxis->setRange(0,360);
	GraphicsDisplay->xAxis->setTickLength(1);
	GraphicsDisplay->xAxis->ticker()->setTickCount(10);
    GraphicsDisplay->axisRect()->setupFullAxesBox();
    GraphicsDisplay->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}



PlotGraphicsInterface::~PlotGraphicsInterface()
{
	this->GraphicsDisplay->clearGraphs();
}

void PlotGraphicsInterface::GenerateTimeSeries(double TimeStep, int length)
{
   TimeSeries.resize(length); 
   double TimePos = 0;
   std::generate(TimeSeries.begin(),TimeSeries.end(), [TimeStep, &TimePos]() mutable ->double 
                                                                 {TimePos += TimeStep; return TimePos;});
}


//void Widget::SetLabelAxisX(QString label_str) { ui->GraphicsDisplay->xAxis->setLabel(label_str); }

void PlotGraphicsInterface::SlotDisplayTimeSeries()
{
  GraphicsDisplay->graph(0)->setData(TimeSeries,InputSeries1);
  GraphicsDisplay->replot();
}

void PlotGraphicsInterface::SlotDisplayTimePairSeries()
{

  GraphicsDisplay->graph(0)->setData(TimeSeries,InputSeries1);
  GraphicsDisplay->graph(1)->setData(TimeSeries,InputSeries2);
  GraphicsDisplay->replot();
}

void PlotGraphicsInterface::SlotDisplayCoord(QPair<double,double> Point)
{
	DisplayPair(Point.first, Point.second);
}

void operator>>(QPair<double,double> Point, PlotGraphicsInterface& Graph) 
{
 Graph.DisplayPair(Point.first,Point.second);
}

void PlotGraphicsInterface::DisplayPair(double pos,double pos2)
{
    GraphicsDisplay->graph(0)->addData(TimePos,pos);
    GraphicsDisplay->graph(1)->addData(TimePos,pos2);
    GraphicsDisplay->replot();

       TimePos += TimeStep;
    if(TimePos >= TimeLimit)   
    { 
       TimePos = 0;
      GraphicsDisplay->graph(0)->data()->clear();
      GraphicsDisplay->graph(1)->data()->clear();
    }
}


//========================================================================
WidgetGraphisPlot::WidgetGraphisPlot(int NumberChannels, QWidget *parent)
	: WidgetAdjustable(parent)
{
	ui.setupUi(this);       
	QBrush brh(QColor(46, 65, 83,100));
	
	ui.graphWidget1->setStyleSheet("QWidget{ border: 2px solid line black }");
	ui.graphWidget1->setBackground(brh);

	Graph1 = new PlotGraphicsInterface(ui.graphWidget1,2);

}

WidgetGraphisPlot::~WidgetGraphisPlot()
{
	qDebug() << "[ GRAPHICS WINDOW DELETE ]";
	delete Graph1;
}