#ifndef MAINWINDOWCLASS_H
#define MAINWINDOWCLASS_H

//#include "ImageStruct.h"
//#include "DataDeviceStructureCommon.h"
#include "ui_mainwindowclass.h"

#include "OutputImageDisplayClass.h"
#include "OutputTextDisplayClass.h"
#include "DebugOutputFilter.h"

#include "graphicwidgetnode.h"
#include "AdjustableWidget.h"

#include <QProcess>
#include <QMainWindow>


enum InterfaceScheme {InterfaceWorkScheme = 0, InterfaceDebugScheme = 2};
enum InterfaceSizes {SmallSize = 0, BigSize = 1};

class MainWindowClass : public QMainWindow
{
	Q_OBJECT 

public:
	MainWindowClass(QWidget *parent = 0);
	~MainWindowClass();

    InterfaceScheme InterfaceType     = InterfaceDebugScheme; 
	InterfaceSizes  InterfaceSizeType = BigSize;
		 int CurrentInterfaceScheme   = (int)InterfaceSizeType + (int)InterfaceType;
public:
	Ui::MainWindowClass ui;
public:
	//friend void operator>>(const DataDeviceStructure& DataToDisplay,MainWindowClass &MainWindow);
	//void ConnectControlSignals(HandleControlInterface* Control);
	
	QTimer TimerDisplayDataSlow;
	QTimer TimerDisplayDataFast;

	void ConvertImage(QImage& GrayImage, QImage& ColorImage);

	void AddWidgetToDisplay(AdjustableWidget* widget);
	void AddWidgetToDisplay(AdjustableLabel* widget);

	void SaveWidgetsLinks();
	void LoadWidgetsLinks();

	void setEnableWidget(int Number, bool Enable);
	void setEnableWidgets(QVector<int> Numbers, bool Enable);

private:
	void LoadWidgetsPosition();
    void SetWidgetsPosition(int group_number);
	void SetGuiFontSize(int FontSize);

	//MainBlockWindow* MainBlockDisplay;
	//EngineControlWindow* EngineControlDisplay1;

	GraphicWidgetNode* MainBlock1;
	OutputTextDisplayClass* CriticalMessageOutput;
	OutputImageDisplayClass* ImageDisplay;

	QGraphicsScene* Scene;
protected:
    void closeEvent(QCloseEvent *event) override;
private:

	QVector<QVector<QPair<int,int>>> WidgetsPositionList;
	QList<GraphicWidgetNode*> ModuleWidgets;
	QList<LinkLine*> LinkLineList;

	int LastWidgetPressedNumber = 0;

private slots:
    void SlotUpdateScene(); // repaint scene when widget size has changed

public slots:
  void SlotWindowDisplayThreadClosed();
  void SlotDeinitMainWindow();
  void SlotCloseMainWindow();


  //==============================================
  void SlotSceneWidgetPressed(int NumberWidget);
  void SlotConnectTwoWidgets(int NumberWidget1, int NumberWidget2);
  void SlotChangeInterfaceSize();
  void SlotChangeInterfaceType();
  void SlotSetInterfaceSize(int GuiSize);
  void SlotSaveWidgetsPosition();
  //==============================================
  
  //void DisplayImage(DataImageProcStructure Data);
  //void DisplayCoordData(DataCoordStructure Data);
  //void DisplayAimingData(DataAimingErrorStructure Data);
  //void DisplayCriticalMessageBrief(StateBlocksEnum state, TypeBlocksEnum block, int number);


  //void DisplayRawImage(uchar* Image);
  void DisplayCriticalMessage(QString Message);

  signals:
  void WindowClosedSignal();
};

#endif // MAINWINDOWCLASS_H

