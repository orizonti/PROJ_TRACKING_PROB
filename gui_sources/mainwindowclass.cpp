#include "mainwindowclass.h"
#include <QAction>
#include <QObject>
#include <QGraphicsItem>
#include <algorithm>
#include "link_line.h"
#define TAG "[ MAIN   WIND ]" 
#include "SettingsEngine.h"
#include "DebugOutputFilter.h"
#include <QThread>
#include <QFile>


MainWindowClass::MainWindowClass(QWidget *parent)
	: QMainWindow(parent)
{
  
	ui.setupUi(this);

   WidgetsPositionList.resize(4);


   ImageDisplay = new OutputImageDisplayClass; 
   ImageDisplay->SetOutputSize(512, 128);


   CriticalMessageOutput = new OutputTextDisplayClass;
   CriticalMessageOutput->setAutoClear(40000);
   CriticalMessageOutput->setDisplayTime(true);




	//QObject::connect(               this, SIGNAL(SignalCriticalMessageBrief(QString) ), CriticalMessageOutput,SLOT(SlotPrintString(QString)));
	//QObject::connect(LaserControlDisplay, SIGNAL(SignalDisplayMessage(QString) ), CriticalMessageOutputBig,SLOT(SlotPrintString(QString)));
	//QObject::connect(               this, SIGNAL(SignalCriticalMessage(QString) ), CriticalMessageOutputBig,SLOT(SlotPrintString(QString)));

	//QObject::connect(this, SIGNAL(SignalNewImage(DataImageProcStructure)),       this, SLOT(DisplayImage(DataImageProcStructure)));
	//QObject::connect(this, SIGNAL(SignalNewAimingData(DataAimingErrorStructure)), this, SLOT(DisplayAimingData(DataAimingErrorStructure)));
	//QObject::connect(this, SIGNAL(SignalNewAimingData(DataAimingErrorStructure)), GraphicsWidget, SLOT(DisplayAimingData(DataAimingErrorStructure)));
	//QObject::connect(this, SIGNAL(SignalNewCoordsData(DataCoordStructure)),this,SLOT(DisplayCoordData(DataCoordStructure)));
	//QObject::connect(this, SIGNAL(SignalDispalyFullImage(uchar*)),this,SLOT(DisplayRawImage(uchar*)));

	QObject::connect(ui.actionGuiSizeSmall, &QAction::triggered,this, &MainWindowClass::SlotChangeInterfaceSize);
	QObject::connect(ui.actionGuiSizeBig, &QAction::triggered,this, &MainWindowClass::SlotChangeInterfaceSize);
	QObject::connect(ui.actionGuiDebug, &QAction::triggered,this, &MainWindowClass::SlotChangeInterfaceType);
	QObject::connect(ui.actionGuiWork, &QAction::triggered,this, &MainWindowClass::SlotChangeInterfaceType);
	QObject::connect(ui.actionSaveGuiScheme, &QAction::triggered,this, &MainWindowClass::SlotSaveWidgetsPosition);
		
	QObject::connect(this->thread(), &QThread::finished,this,&MainWindowClass::SlotWindowDisplayThreadClosed);

	Scene = new QGraphicsScene();
	Scene->setSceneRect(0, 0, 2400, 3500);

    LoadWidgetsPosition();

    //this->AddWidgetToDisplay(MainBlockDisplay); 
    this->AddWidgetToDisplay(CriticalMessageOutput);

    CriticalMessageOutput->SetOutputSize(220,240);
	CriticalMessageOutput->SetLabel("Неисправности");

	ui.graphicsView->setScene(Scene);
	ui.graphicsView->centerOn(20, 20);

  QObject::connect(this, SIGNAL(WindowClosedSignal()), this, SLOT(SlotDeinitMainWindow()));
}


MainWindowClass::~MainWindowClass() 
{ 
  qDebug() << TAG << "[ DELETE MAIN WINDOW ]"; 
}


//void MainWindowClass::DisplayCoordData(DataCoordStructure Data)             { ShowImageDisplay->DisplayAimingCoord(Data.Coords, Data.CoordsROI); }
//void MainWindowClass::DisplayCameraData(DataCamerasStructure DataStructure) { CameraControlBlockDisplay->DisplayState(DataStructure.State); }
//void MainWindowClass::DisplaySensorData(DataKLPSensorMeasures Data) { SensorDisplay->SlotDisplayState(Data); }
//
//void MainWindowClass::DisplayAimingData(DataAimingErrorStructure DataStructure)
//{
//if (DataStructure.NumberBlock == 1) this->AimingBlockDisplay1->DisplayState(DataStructure.State,DataStructure.AimingState,DataStructure.PIDParam);
//if (DataStructure.NumberBlock == 2) this->AimingBlockDisplay2->DisplayState(DataStructure.State,DataStructure.AimingState,DataStructure.PIDParam);
//if (DataStructure.NumberBlock == 3) this->AimingBlockDisplay3->DisplayState(DataStructure.State,DataStructure.AimingState,DataStructure.PIDParam);
//this->PIDControl->DisplayState(DataStructure.PIDState);
//}

//void MainWindowClass::DisplayEngineData(DataEngineStructure DataStructure)
//{
//this->EngineControlDisplay1->DisplayState(DataStructure.Engine1);
//this->EngineControlDisplay2->DisplayState(DataStructure.Engine2);
//this->EngineControlDisplay3->DisplayState(DataStructure.Engine3);
//}

void MainWindowClass::DisplayCriticalMessage(QString Message) {}


//void operator>>(const DataDeviceStructure& DataToDisplay, MainWindowClass &MainWindow)
//{
//	MainWindow.DisplayCriticalMessageBrief(DataToDisplay.State, DataToDisplay.TypeBlock, DataToDisplay.NumberBlock);
//	switch (DataToDisplay.TypeBlock)
//	{
//	default: qDebug()<< TAG << "UNKNOWN TYPE BLOCK TO DISPLAY"; break;
//	}
//}

//void MainWindowClass::ConnectControlSignals(HandleControlInterface* Control)
//{
//	   MainBlockDisplay->ConnectControlSignals(Control);
//}

void MainWindowClass::SlotDeinitMainWindow()
{
qDebug() << TAG << " [ DEINIT ]";
TimerDisplayDataSlow.stop();
TimerDisplayDataFast.stop();
this->SlotSaveWidgetsPosition();
this->SaveWidgetsLinks();
Scene->clear();
}

void MainWindowClass::SlotCloseMainWindow()
{
qDebug() << TAG << "[ START CLOSE PROCEDURE ]";

emit WindowClosedSignal();
this->thread()->sleep(6);

this->close();
Scene->deleteLater();
this->deleteLater();
}

void MainWindowClass::closeEvent(QCloseEvent *event)
{
qDebug() << TAG << "[ MAIN WINDOW CLOSED ]";
QMainWindow::closeEvent(event);
}

void MainWindowClass::SlotChangeInterfaceType()
{
   QAction* Action = dynamic_cast<QAction*>(QObject::sender());

   if(Action->objectName() == "actionGuiDebug") {InterfaceType = InterfaceDebugScheme;};
   if(Action->objectName() == "actionGuiWork")  {InterfaceType = InterfaceWorkScheme;};
   SlotSetInterfaceSize((int)InterfaceSizeType + (int)InterfaceType);
}

void MainWindowClass::SlotChangeInterfaceSize()
{
   QAction* Action = dynamic_cast<QAction*>(QObject::sender());

	if(Action->objectName() == "actionGuiSizeSmall") InterfaceSizeType = SmallSize;
	if(Action->objectName() == "actionGuiSizeBig")   InterfaceSizeType = BigSize;
	SlotSetInterfaceSize((int)InterfaceSizeType + (int)InterfaceType);
   
}

void MainWindowClass::SetGuiFontSize(int GuiSize)
{
	int FontSize = 8; 
	if(GuiSize == 1) FontSize = 12;
	if(GuiSize == 3) FontSize = 12;

    QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
    GraphicWidgetNode* node = dynamic_cast<GraphicWidgetNode*>(Item);
		if (node != nullptr)
		{
			AdjustableWidget* widget = dynamic_cast<AdjustableWidget*>(node->WindowNode);
			AdjustableLabel* label = dynamic_cast<AdjustableLabel*>(node->WindowNode);

			if ( widget != nullptr) widget->SlotSetWindowSize(FontSize);
			if ( label != nullptr) label->SlotSetWindowSize(FontSize);
		}
	}
}

void MainWindowClass::SlotSetInterfaceSize(int GuiSize)
{
	CurrentInterfaceScheme = GuiSize;
    SetGuiFontSize(CurrentInterfaceScheme);
    SetWidgetsPosition(CurrentInterfaceScheme);

    QTimer::singleShot(30,this,&MainWindowClass::SlotUpdateScene);
}

void MainWindowClass::SetWidgetsPosition(int group_number)
{
  QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
     GraphicWidgetNode* node = dynamic_cast<GraphicWidgetNode*>(Item); 
	   if (node != nullptr)
	   {
      node->SetScenePosition(WidgetsPositionList[group_number][node->NumberWidget].first,WidgetsPositionList[group_number][node->NumberWidget].second);  	
	   }
	}
}

void MainWindowClass::SaveWidgetsLinks()
{
QString name = qgetenv("USER");
QString FileName = QString("/home/%1/DATA/TrainerData/WidgetLinks.txt").arg(name);

QFile data(FileName);
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

void MainWindowClass::SlotSaveWidgetsPosition()
{
QString name = qgetenv("USER");
QString FileName = QString("/home/%1/DATA/TrainerData/WidgetsPosition.txt").arg(name);

QFile data(FileName);
data.open(QIODevice::WriteOnly); data.flush();

QTextStream out(&data);

    QList<QGraphicsItem*> items = Scene->items();
	for(auto Item: items)
	{
    GraphicWidgetNode* node = dynamic_cast<GraphicWidgetNode*>(Item);
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

void MainWindowClass::LoadWidgetsPosition()
{
	QFile file(SettingsPath::GetPath("WIDGETS_SETTINGS"));
    //qDebug() << Qt::endl <<TAG << "LOAD WIDGETS POS: " << file.fileName();

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
		if(POS_X < 0) POS_X = 0;
		if(POS_Y < 0) POS_Y = 0;
		WidgetsPositionList[positions_group].append(QPair<int,int>(POS_X,POS_Y));
    }
}

void MainWindowClass::SlotUpdateScene()
{
for(auto Node: ModuleWidgets) Node->UpdateWidget();

Scene->update(Scene->sceneRect());
ui.graphicsView->repaint();
}

void MainWindowClass::AddWidgetToDisplay(AdjustableWidget* widget)
{
    auto Position = WidgetsPositionList[CurrentInterfaceScheme][this->ModuleWidgets.size()];
	this->ModuleWidgets.append(new GraphicWidgetNode(Scene, Position.first, Position.second+40,widget));
	QObject::connect(ModuleWidgets.last(),SIGNAL(SignalWidgetPressed(int)),this,SLOT(SlotSceneWidgetPressed(int)));
}

void MainWindowClass::AddWidgetToDisplay(AdjustableLabel* widget)
{
    auto Position = WidgetsPositionList[CurrentInterfaceScheme][this->ModuleWidgets.size()];
	this->ModuleWidgets.append(new GraphicWidgetNode(Scene, Position.first, Position.second+40,widget));
}

void MainWindowClass::LoadWidgetsLinks()
{
  QString WidgetLinks = "2  1  : 1  0";
            
  QString FileName = SettingsPath::GetPath("LINKS_POS");
  QFile file(FileName);
  
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


void MainWindowClass::setEnableWidget(int Number, bool Enable) { ModuleWidgets[Number]->SetWidgetActive(Enable); }
void MainWindowClass::setEnableWidgets(QVector<int> Numbers, bool Enable) { for(auto number: Numbers) setEnableWidget(number, Enable); }

//================================================================================================================
// CREATE WIDGETS LINKS
void MainWindowClass::SlotSceneWidgetPressed(int NumberWidget)
{
  if(LastWidgetPressedNumber == 0) { LastWidgetPressedNumber = NumberWidget; return;}
  SlotConnectTwoWidgets(LastWidgetPressedNumber, NumberWidget);
                        LastWidgetPressedNumber = 0;
}

void MainWindowClass::SlotConnectTwoWidgets(int NumberWidget1, int NumberWidget2)
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

void MainWindowClass::SlotWindowDisplayThreadClosed()
{
  qDebug() << "MAIN WINDOW THREAD FINISHED";
}
