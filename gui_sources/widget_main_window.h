#ifndef WidgetMainWindow_H
#define WidgetMainWindow_H

//#include "ImageStruct.h"
//#include "DataDeviceStructureCommon.h"
#include "ui_widget_main_window.h"

#include "widget_output_image_display.h"
#include "widget_output_text_display.h"
#include "debug_output_filter.h"

#include "widget_scene_node_interface.h"
#include "widget_adjustable.h"

#include <QProcess>
#include <QMainWindow>


enum InterfaceScheme {InterfaceWorkScheme = 0, InterfaceDebugScheme = 2};
enum InterfaceSizes {SmallSize = 0, BigSize = 1};

class WidgetMainWindow : public QMainWindow
{
	Q_OBJECT 

public:
	WidgetMainWindow(QWidget *parent = 0);
	~WidgetMainWindow();

    InterfaceScheme InterfaceType     = InterfaceWorkScheme; 
	InterfaceSizes  InterfaceSizeType = BigSize;
		 int CurrentInterfaceScheme   = (int)InterfaceSizeType + (int)InterfaceType;
public:
	Ui::WidgetMainWindow ui;
public:
	//friend void operator>>(const DataDeviceStructure& DataToDisplay,WidgetMainWindow &MainWindow);
	//void ConnectControlSignals(HandleControlInterface* Control);
	
	QTimer TimerDisplayDataSlow;
	QTimer TimerDisplayDataFast;

	void AddWidgetToDisplay(WidgetAdjustable* widget);
	void AddWidgetToDisplay(AdjustableLabel* widget);
	void AddProxyWidgetToDisplay(QWidget* widget);

	void SaveWidgetsLinks();
	void LoadWidgetsLinks();
	void LoadWidgetsPosition();

	void setenableWidget(int Number, bool enable);
	void setenableWidgets(QVector<int> Numbers, bool enable);

	WidgetSceneNodeInterface* GetLastWidget() { return ModuleWidgets.last();};

private:
    void SetWidgetsPosition(int group_number);
	void SetGuiFontSize(int FontSize);

	//MainBlockWindow* MainBlockDisplay;
	//engineControlWindow* engineControlDisplay1;

	WidgetSceneNodeInterface* MainBlock1;
	WidgetOutputTextDisplay* CriticalMessageOutput;
	WidgetOutputImageDisplay* ImageDisplay;

	QGraphicsScene* Scene;
protected:
    void closeEvent(QCloseEvent *event) override;
private:

	QVector<QVector<QPair<int,int>>> WidgetsPositionList;
	QList<WidgetSceneNodeInterface*> ModuleWidgets;
	QList<LinkLine*> LinkLineList;

	int LastWidgetPressedNumber = 0;

private slots:
    void SlotUpdateScene(); // repaint Scene when widget size has changed

public slots:
  void SlotWindowDisplayThreadClosed();
  void SlotDeinitMainWindow();
  void SlotCloseMainWindow();


  //==============================================
  void SlotSceneWidgetPressed(int NumberWidget);
  void SlotConnectTwoWidgets(int NumberWidget1, int NumberWidget2);
  void SlotChangeInterfaceSize();
  void SlotChangeInterfaceType();
  void slotSetInterfaceSize(int GuiSize);
  void SlotSaveWidgetsPosition();
  //==============================================
  
  //void DisplayImage(DataImageProcStructure Data);
  //void DisplayCoordData(DataCoordStructure Data);
  //void DisplayAimingData(DataAimingErrorStructure Data);
  //void DisplayCriticalMessageBrief(StateEnumBlock state, TypeEnumBlock block, int number);


  //void DisplayRawImage(uchar* Image);
  void DisplayCriticalMessage(QString Message);

  signals:
  void WindowClosedSignal();
};

#endif // WidgetMainWindow_H

