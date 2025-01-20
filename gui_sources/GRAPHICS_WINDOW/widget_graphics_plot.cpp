#include "widget_graphics_plot.h"


WidgetGraphicsPlot::WidgetGraphicsPlot(QCustomPlot* Graphics, int NumberChannels)
{
	GraphicsDisplay = Graphics;
	this->TimeFromStartMs = 0;
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

void WidgetGraphicsPlot::CreateGraphics(int Number)
{
	
	for(int n = 0; n < Number; n++)
	{
		GraphicsDisplay->addGraph(); 
		GraphicsDisplay->graph(n)->setPen(Pens[n]);
		GraphicsDisplay->graph(n)->setBrush(Brushes[n]);
        //GraphicsDisplay->graph(n)->setAntialiasedFill(false);

		DisplayFlags.push_back(true);
	}

    GraphicsDisplay->yAxis->setRange(-3000,3000);
	GraphicsDisplay->xAxis->setTickLength(1);
	GraphicsDisplay->xAxis->ticker()->setTickCount(10);
    GraphicsDisplay->axisRect()->setupFullAxesBox();
    GraphicsDisplay->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}


GraphicsWindow::GraphicsWindow(int NumberChannels, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);       
	QBrush brh(QColor(170, 170, 170));
	
	ui.graphWidget1->setStyleSheet("QWidget{ border: 2px solid line black }");
	ui.graphWidget1->setBackground(brh);

	Graph1 = new WidgetGraphicsPlot(ui.graphWidget1, NumberChannels);

}

WidgetGraphicsPlot::~WidgetGraphicsPlot()
{
	this->GraphicsDisplay->clearGraphs();
}
GraphicsWindow::~GraphicsWindow()
{
	qDebug() << "[ GRAPHICS WINDOW DELETE ]";
	delete Graph1;
}

void GraphicsWindow::DisplayDataVector(std::vector<uint16_t> Data) { Data >> *Graph1;};
void GraphicsWindow::DisplayDataVector(std::vector<int> Data) { Data >> *Graph1;};
void GraphicsWindow::DisplayDataVector(std::vector<float> Data) { Data >> *Graph1;};
void GraphicsWindow::DisplayDataVector(std::vector<double> Data) { Data >> *Graph1;};

void GraphicsWindow::ChangeDataLimit(double DataLimitup, double DataLimitDown)
{
	Graph1->GraphicsDisplay->yAxis->setRange(DataLimitup, DataLimitDown);
}



