#ifndef WIDGET_SCenE_NODE_INTERFACE_H
#define WIDGET_SCenE_NODE_INTERFACE_H

#include "node.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>

class LinkLine;

class WidgetScenenodeInterface : public QGraphicsProxyWidget
{
Q_OBJECT
public:
    WidgetScenenodeInterface(QGraphicsScene* Scene, int X_POS, int Y_POS, QWidget *Widget);
    ~WidgetScenenodeInterface();

    void SetScenePosition(int PosX, int PosY);
    void SetWidgetActive(bool active);
    std::vector<Node*> PortNodes;
    QList<LinkLine*> LinkLineList;

    QWidget* WindowNode = 0;
    void setWidget(QWidget* widget);
	LinkLine* ConnectNode(int sourcePortNode ,WidgetScenenodeInterface* Node, int destPortNode);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    int NodePositionX = 0;
    int NodePositionY = 0;

     QVariant itemChange(GraphicsItemChange change, const QVariant &value);
     void UpdateWidget();

	 void focusInEvent(QFocusEvent *event);
	 void focusOutEvent(QFocusEvent *event);

     static int CounterWidget;
     int NumberWidget = 0;

signals:
void SignalWidgetPressed(int NumberWidget);
public slots:

};

#endif // WIDGET_SCenE_NODE_INTERFACE_H
