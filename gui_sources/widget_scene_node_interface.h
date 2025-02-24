#ifndef WIDGET_SCENE_NODE_INTERFACE_H
#define WIDGET_SCENE_NODE_INTERFACE_H

#include "node.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include "widget_adjustable.h"

class LinkLine;

class WidgetSceneNodeInterface : public QGraphicsProxyWidget
{
Q_OBJECT
public:
    WidgetSceneNodeInterface(QGraphicsScene* Scene, int X_POS, int Y_POS, QWidget *Widget);
    WidgetSceneNodeInterface(QGraphicsScene* Scene, int X_POS, int Y_POS, WidgetAdjustable *Widget);
    ~WidgetSceneNodeInterface();

    void SetScenePosition(int PosX, int PosY);
    void SetWidgetActive(bool active);
    std::vector<Node*> PortNodes;
    QList<LinkLine*> LinkLineList;

    QWidget* WindowNode = 0;
    void setWidget(QWidget* widget);
	LinkLine* ConnectNode(int sourcePortNode ,WidgetSceneNodeInterface* Node, int destPortNode);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    int NodePositionX = 0;
    int NodePositionY = 0;

     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
     void UpdateWidget();

	 void focusInEvent(QFocusEvent *event);
	 void focusOutEvent(QFocusEvent *event);
     void HideNodes();
     void HideLinks();

     static int CounterWidget;
     int NumberWidget = 0;

private:
    void Init(QWidget* widget);

signals:
void SignalWidgetPressed(int NumberWidget);
public slots:
void SlotHideWidget();

};

#endif // WIDGET_SCenE_NODE_INTERFACE_H
