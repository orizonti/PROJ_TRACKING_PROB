#pragma once
#include <QWidget>
#include <QDebug>
#include <QGraphicsEffect>
#include <QPainter>
#include "widget_adjustable.h"
#include <QTimer>

#include "device_generic_interface.h"
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QBoxLayout>
#include "interface_node_signal_adapter.h"


class WidgetDeviceControl : public WidgetAdjustable, public PassCoordClass<float>
{
    Q_OBJECT
public:
    WidgetDeviceControl(QString name = "Устройство", Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = nullptr);
    ~WidgetDeviceControl();

    Qt::Orientation orientationWidget = Qt::Horizontal;
    //================================================================
    std::pair<float,float> State;
	const QPair<float, float>& getOutput() override { return State;};
	void setInput(const QPair<float, float>& Coord) override { State = Coord; labelState->setText(QString("%1 %2").arg(State.first).arg(State.second)); };
    NodeCoordSignalAdapter NodeSynchronizer{this};
    void linkToWidget(WidgetDeviceControl* widget) { NodeSynchronizer.linkPeers(&widget->NodeSynchronizer);};
    //================================================================

    void enableScheme(bool enableState, bool enableParam, bool enableLevels, bool enableOnOff, bool enableArrows, bool enableLabel = true);
    void setScheme(int numberLevels, int numberDevice, int schemeArrows = 0);
    void setButtonsName(QVector<QString> names);
    void setButtonsMode(QVector<bool> modes);
    void setLabelMode(int Mode);
    void setLevelsName(QVector<QString> names);
    void setName(QString name);
    void setSizes();
    int ModeLabel = 2;
    void addSpace(int group);
    void setParamList(int Range, int CurrentParam, int StepNumber);
    

    void linkToDevice(std::shared_ptr<DeviceGenericHandleControl> Device);
    std::shared_ptr<DeviceGenericHandleControl> DeviceLinked = nullptr;
    void linkSignals();

    public:
    QTimer timerCheckDevice;
    QTimer timerCheckDevice2;

      QLabel* labelName  = nullptr;
      QLabel* labelState = nullptr;
    QSpinBox* spinParam  = nullptr;

    QPushButton* buttonLeft  = nullptr;
    QPushButton* buttonRight = nullptr;
    QPushButton* buttonUp    = nullptr;
    QPushButton* buttonDown  = nullptr;

    QVector<QPushButton*> buttonsLevel;
    QVector<QPushButton*> buttonsOnOff;
    QVector<QPushButton*> buttonsArrow;

    QGroupBox* groupButtonsLevel = nullptr;
    QGroupBox* groupButtonsOnOff = nullptr;
    QGroupBox* groupArrows = nullptr;

    int schemeArrowsControl = 0;
    QSize minButtonsSize{80,40};
    QSize maxButtonsSize{100,50};

    QSize minArrowsSize{30,40};
    QSize maxArrowsSize{60,50};

    QSize minLevelsSize{30,40};
    QSize maxLevelsSize{60,50};


    QSize minLabelsSize{80,40};
    QSize maxLabelsSize{100,50};

    QBoxLayout* mainLayout = nullptr;
};
