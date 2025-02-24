#include "widget_main_window.h"
#include <QAction>
#include <QObject>
#include <QGraphicsItem>
#include <algorithm>
#include "link_line.h"
#include "widget_output_image_display.h"
#define TAG "[ MAIN   WIND ]" 
#include "register_settings.h"
#include "debug_output_filter.h"
#include <QThread>
#include <QFile>


WidgetMainWindow::WidgetMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
  
	ui.setupUi(this);

   WidgetsPositionList.resize(4);


   ImageDisplay = new WidgetOutputImageDisplay; 
   ImageDisplay->SetOutputSize(512, 128);


   CriticalMessageOutput = new WidgetOutputTextDisplay;
   CriticalMessageOutput->setAutoClear(40000);
   CriticalMessageOutput->setDisplayTime(true);
   CriticalMessageOutput->SetOutputSize(600,500);




	//QObject::connect(               this, SIGNAL(SignalCriticalMessageBrief(QString) ), CriticalMessageOutput,SLOT(SlotPrintString(QString)));
	//QObject::connect(LaserControlDisplay, SIGNAL(SignalDisplayMessage(QString) ), CriticalMessageOutputBig,SLOT(SlotPrintString(QString)));
	//QObject::connect(               this, SIGNAL(SignalCriticalMessage(QString) ), CriticalMessageOutputBig,SLOT(SlotPrintString(QString)));

	//QObject::connect(this, SIGNAL(SignalNewImage(DataImageProcStructure)),       this, SLOT(DisplayImage(DataImageProcStructure)));
	//QObject::connect(this, SIGNAL(SignalNewAimingData(DataAimingErrorStructure)), this, SLOT(DisplayAimingData(DataAimingErrorStructure)));
	//QObject::connect(this, SIGNAL(SignalNewAimingData(DataAimingErrorStructure)), GraphicsWidget, SLOT(DisplayAimingData(DataAimingErrorStructure)));
	//QObject::connect(this, SIGNAL(SignalNewCoordsData(DataCoordStructure)),this,SLOT(DisplayCoordData(DataCoordStructure)));
	//QObject::connect(this, SIGNAL(SignalDispalyFullImage(uchar*)),this,SLOT(DisplayRawImage(uchar*)));

	QObject::connect(ui.actionGuiSizeSmall, &QAction::triggered,this, &WidgetMainWindow::SlotChangeInterfaceSize);
	QObject::connect(ui.actionGuiSizeBig, &QAction::triggered,this, &WidgetMainWindow::SlotChangeInterfaceSize);
	QObject::connect(ui.actionGuiDebug, &QAction::triggered,this, &WidgetMainWindow::SlotChangeInterfaceType);
	QObject::connect(ui.actionGuiWork, &QAction::triggered,this, &WidgetMainWindow::SlotChangeInterfaceType);
	QObject::connect(ui.actionSaveGuiScheme, &QAction::triggered,this, &WidgetMainWindow::SlotSaveWidgetsPosition);
		
	QObject::connect(this->thread(), &QThread::finished,this,&WidgetMainWindow::SlotWindowDisplayThreadClosed);

	Scene = new QGraphicsScene();
	Scene->setSceneRect(0, 0, 2200, 2400);

    LoadWidgetsPosition();

    //this->AddWidgetToDisplay(MainBlockDisplay); 
    this->AddWidgetToDisplay(CriticalMessageOutput);

    //CriticalMessageOutput->SetOutputSize(380,280);
	CriticalMessageOutput->SetLabel("ОТЛАДКА");

	ui.graphicsView->setScene(Scene);
	ui.graphicsView->centerOn(20, 20);

	SlotSetInterfaceSize((int)InterfaceSizeType + (int)InterfaceType);
  QObject::connect(this, SIGNAL(WindowClosedSignal()), this, SLOT(SlotDeinitMainWindow()));
}


WidgetMainWindow::~WidgetMainWindow() 
{ 
  qDebug() << TAG << "[ DELETE MAIN WINDOW ]"; 
}


//void WidgetMainWindow::DisplayCoordData(DataCoordStructure Data)             { ShowImageDisplay->DisplayCoordAim(Data.Coords, Data.CoordsROI); }
//void WidgetMainWindow::DisplayCameraData(DataCamerasStructure DataStructure) { CameraControlBlockDisplay->DisplayState(DataStructure.State); }
//void WidgetMainWindow::DisplaySensorData(DataKLPSensorMeasures Data) { SensorDisplay->SlotDisplayState(Data); }
//
//void WidgetMainWindow::DisplayAimingData(DataAimingErrorStructure DataStructure)
//{
//if (DataStructure.NumberBlock == 1) this->AimingBlockDisplay1->DisplayState(DataStructure.State,DataStructure.AimingState,DataStructure.PIDParam);
//if (DataStructure.NumberBlock == 2) this->AimingBlockDisplay2->DisplayState(DataStructure.State,DataStructure.AimingState,DataStructure.PIDParam);
//if (DataStructure.NumberBlock == 3) this->AimingBlockDisplay3->DisplayState(DataStructure.State,DataStructure.AimingState,DataStructure.PIDParam);
//this->PIDControl->DisplayState(DataStructure.PIDState);
//}

//void WidgetMainWindow::DisplayengineData(DataengineStructure DataStructure)
//{
//this->engineControlDisplay1->DisplayState(DataStructure.engine1);
//this->engineControlDisplay2->DisplayState(DataStructure.engine2);
//this->engineControlDisplay3->DisplayState(DataStructure.engine3);
//}

void WidgetMainWindow::DisplayCriticalMessage(QString Message) {}


//void operator>>(const DataDeviceStructure& DataToDisplay, WidgetMainWindow &MainWindow)
//{
//	MainWindow.DisplayCriticalMessageBrief(DataToDisplay.State, DataToDisplay.TypeBlock, DataToDisplay.NumberBlock);
//	switch (DataToDisplay.TypeBlock)
//	{
//	default: qDebug()<< TAG << "UNKNOWN TYPE BLOCK TO DISPLAY"; break;
//	}
//}

//void WidgetMainWindow::ConnectControlSignals(HandleControlInterface* Control)
//{
//	   MainBlockDisplay->ConnectControlSignals(Control);
//}

void WidgetMainWindow::SlotDeinitMainWindow()
{
qDebug() << TAG << " [ DEINIT ]";
TimerDisplayDataSlow.stop();
TimerDisplayDataFast.stop();
this->SlotSaveWidgetsPosition();
this->SaveWidgetsLinks();
Scene->clear();
}

void WidgetMainWindow::SlotCloseMainWindow()
{
qDebug() << TAG << "[ START CLOSE PROCEDURE ]";

emit WindowClosedSignal();
this->thread()->sleep(6);

this->close();
Scene->deleteLater();
this->deleteLater();
}

void WidgetMainWindow::closeEvent(QCloseEvent *event)
{
qDebug() << TAG << "[ MAIN WINDOW CLOSED ]";
QMainWindow::closeEvent(event);
}

void WidgetMainWindow::SlotChangeInterfaceType()
{
   QAction* Action = dynamic_cast<QAction*>(QObject::sender());

   if(Action->objectName() == "actionGuiDebug") {InterfaceType = InterfaceDebugScheme;};
   if(Action->objectName() == "actionGuiWork")  {InterfaceType = InterfaceWorkScheme;};
   SlotSetInterfaceSize((int)InterfaceSizeType + (int)InterfaceType);
}

void WidgetMainWindow::SlotChangeInterfaceSize()
{
   QAction* Action = dynamic_cast<QAction*>(QObject::sender());

	if(Action->objectName() == "actionGuiSizeSmall") InterfaceSizeType = SmallSize;
	if(Action->objectName() == "actionGuiSizeBig")   InterfaceSizeType = BigSize;
	SlotSetInterfaceSize((int)InterfaceSizeType + (int)InterfaceType);
   
}

void WidgetMainWindow::SetGuiFontSize(int GuiSize)
{
	int FontSize = 8; 
	if(GuiSize == 1) FontSize = 12;
	if(GuiSize == 3) FontSize = 12;

    QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
    WidgetSceneNodeInterface* node = dynamic_cast<WidgetSceneNodeInterface*>(Item);
		if (node != nullptr)
		{
			WidgetAdjustable* widget = dynamic_cast<WidgetAdjustable*>(node->WindowNode);
			AdjustableLabel* label = dynamic_cast<AdjustableLabel*>(node->WindowNode);

			if ( widget != nullptr) widget->SlotSetWindowSize(FontSize);
			if ( label != nullptr) label->SlotSetWindowSize(FontSize);
		}
	}
}

void WidgetMainWindow::SlotSetInterfaceSize(int GuiSize)
{
	CurrentInterfaceScheme = GuiSize;
    SetGuiFontSize(CurrentInterfaceScheme);
    SetWidgetsPosition(CurrentInterfaceScheme);

    QTimer::singleShot(30,this,&WidgetMainWindow::SlotUpdateScene);
}

void WidgetMainWindow::SetWidgetsPosition(int group_number)
{
  QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
     WidgetSceneNodeInterface* node = dynamic_cast<WidgetSceneNodeInterface*>(Item); 
	   if (node != nullptr)
	   {
      node->SetScenePosition(WidgetsPositionList[group_number][node->NumberWidget].first,WidgetsPositionList[group_number][node->NumberWidget].second);  	
	   }
	}
}

void WidgetMainWindow::SaveWidgetsLinks()
{
QString name = qgetenv("USER");
QString Filename = QString("/home/%1/DATA/TrackingProject/WidgetLinks.txt").arg(name);

QFile data(Filename);
data.open(QFile::WriteOnly); data.flush();

QString outString;
QTextStream out(&outString);
LinkLineList.clear(); 
QVector<int> LinksNumberAtStore;

    std::tuple<int,int,int,int> LinkScheme;
    QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
		LinkLine* line = dynamic_cast<LinkLine*>(Item); 
		if (line == nullptr) continue;
		if (LinksNumberAtStore.contains(line->NumberLink)) continue;

		LinkLineList.append(line);
	    LinksNumberAtStore.append(line->NumberLink);
	}

	for(auto Link: LinkLineList)
	{
		LinkScheme = Link->GetLinkScheme(); 
		out << qSetFieldWidth(4) << std::get<0>(LinkScheme) << std::get<1>(LinkScheme)<< ":" << std::get<2>(LinkScheme) << std::get<3>(LinkScheme) << Qt::endl;
	}

data.write(outString.toUtf8());
data.close();
}

void WidgetMainWindow::SlotSaveWidgetsPosition()
{
QString name = qgetenv("USER");
QString Filename = QString("/home/%1/DATA/TrackingProject/WidgetsPosition.txt").arg(name);

QFile data(Filename);
data.open(QIODevice::WriteOnly); data.flush();

QTextStream out(&data);

    QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
    WidgetSceneNodeInterface* node = dynamic_cast<WidgetSceneNodeInterface*>(Item);
		if ( node != nullptr)
		{
		WidgetsPositionList[CurrentInterfaceScheme][node->NumberWidget].first = node->NodePositionX;
		WidgetsPositionList[CurrentInterfaceScheme][node->NumberWidget].second = node->NodePositionY;
		}
	}

out << "          NUMBER      POSITION" << Qt::endl;
int number = 0;
for(auto group: WidgetsPositionList)
{
    for(int n = 0; n < group.size(); n++) out << qSetFieldWidth(10) << Qt::right << n << group[n].first << group[n].second  << Qt::endl; number++;
	out << "==============================" << Qt::endl;
}
data.close();

this->SaveWidgetsLinks();
}

void WidgetMainWindow::LoadWidgetsPosition()
{
	QFile file(SettingsRegister::GetString("FILE_WIDGETS_POS"));
    qDebug() << Qt::endl <<TAG << "LOAD WIDGETS POS: " << file.fileName();

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QString line = in.readLine();

    int positions_group = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QTextStream stream(&line);

        if(line.contains("==")) { positions_group++; continue; }

        int number; int POS_X;int POS_Y;
		stream >> number >> POS_X >> POS_Y;
		if(POS_X < 0) POS_X = 50;
		if(POS_Y < 0) POS_Y = 50;
		//qDebug() << "LOAD POS GROUP: " << positions_group << " POS: " << POS_X << POS_Y;
		WidgetsPositionList[positions_group].append(QPair<int,int>(POS_X,POS_Y));
    }
}

void WidgetMainWindow::SlotUpdateScene()
{
for(auto Node: ModuleWidgets) Node->UpdateWidget();

Scene->update(Scene->sceneRect());
ui.graphicsView->repaint();
}

void WidgetMainWindow::AddProxyWidgetToDisplay(QWidget* widget)
{
    //auto Position = WidgetsPositionList[CurrentInterfaceScheme][this->ModuleWidgets.size()];
	this->ModuleWidgets.append(new WidgetSceneNodeInterface(Scene, 400, 400,widget));
}

void WidgetMainWindow::AddWidgetToDisplay(WidgetAdjustable* widget)
{
    auto Position = WidgetsPositionList[CurrentInterfaceScheme][this->ModuleWidgets.size()];
	this->ModuleWidgets.append(new WidgetSceneNodeInterface(Scene, Position.first, Position.second,widget));
	QObject::connect(ModuleWidgets.last(),SIGNAL(SignalWidgetPressed(int)),this,SLOT(SlotSceneWidgetPressed(int)));
	//qDebug() << TAG << "ADD NEW WIDGET : " << Position << " SCHEME: " << CurrentInterfaceScheme << "NUMBER: " << ModuleWidgets.size();
}

void WidgetMainWindow::AddWidgetToDisplay(AdjustableLabel* widget)
{
    auto Position = WidgetsPositionList[CurrentInterfaceScheme][this->ModuleWidgets.size()];
	this->ModuleWidgets.append(new WidgetSceneNodeInterface(Scene, Position.first, Position.second+40,widget));
}

void WidgetMainWindow::LoadWidgetsLinks()
{
  QString WidgetLinks = "2  1  : 1  0";
            
  QString Filename = SettingsRegister::GetString("FILE_LINKS_POS");
  qDebug() << TAG << "LOAD LINKS: " << Filename;
  QFile file(Filename);
  
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) WidgetLinks = file.readAll();

	QTextStream in_stream(&WidgetLinks);
	int module1, node1 , module2 , node2;
	QString delimeter;
	while(!in_stream.atEnd())
	{
		in_stream >> module1 >> node1 >> delimeter >> module2 >> node2;	

        if(module1 > ModuleWidgets.size() || module2 > ModuleWidgets.size()) continue;
		if(delimeter == ":")
		{
	    auto Link = ModuleWidgets[module1]->ConnectNode(node1,ModuleWidgets[module2],node2);
		     LinkLineList.append(Link);
		}
	}
}


void WidgetMainWindow::setenableWidget(int Number, bool enable) { ModuleWidgets[Number]->SetWidgetActive(enable); }
void WidgetMainWindow::setenableWidgets(QVector<int> Numbers, bool enable) { for(auto number: Numbers) setenableWidget(number, enable); }

//================================================================================================================
// CREATE WIDGETS LINKS
void WidgetMainWindow::SlotSceneWidgetPressed(int NumberWidget)
{
  if(LastWidgetPressedNumber == 0) { LastWidgetPressedNumber = NumberWidget; return;}
  SlotConnectTwoWidgets(LastWidgetPressedNumber, NumberWidget);
                        LastWidgetPressedNumber = 0;
}

void WidgetMainWindow::SlotConnectTwoWidgets(int NumberWidget1, int NumberWidget2)
{
  if(NumberWidget1 == NumberWidget2) return;

  //===========================================
  //FIND MOST CLOSE PORT NODES TO LINK
  std::vector<std::pair<Node*,Node*>> NodePairs;
  std::vector<int> distances;
  for(auto Node: ModuleWidgets[NumberWidget1]->PortNodes)
  {
	for(auto NodeDest: ModuleWidgets[NumberWidget2]->PortNodes)
	{
       NodePairs.push_back(std::make_pair(Node,NodeDest)); 
	   auto VectorOffset = Node->pos() - NodeDest->pos(); 
	   distances.push_back(std::sqrt(std::pow(VectorOffset.x(),2) + std::pow(VectorOffset.y(),2)));
	}
  }
  auto min = std::min_element(distances.begin(),distances.end());
  auto pos = std::distance(distances.begin(),min);
  //===========================================

  auto Link = ModuleWidgets[NumberWidget1]->ConnectNode(NodePairs[pos].first->Number-1,ModuleWidgets[NumberWidget2],NodePairs[pos].second->Number-1);
}
//================================================================================================================

void WidgetMainWindow::SlotWindowDisplayThreadClosed()
{
  qDebug() << "MAIN WINDOW THREAD FINISHED";
}
