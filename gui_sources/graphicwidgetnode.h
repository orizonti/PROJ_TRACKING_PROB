#ifndef GRAPHICWIDGETNODE_H
#define GRAPHICWIDGETNODE_H

#include "node.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>

class LinkLine;

class GraphicWidgetNode : public QGraphicsProxyWidget
{
Q_OBJECT
public:
    GraphicWidgetNode(QGraphicsScene* Scene, int X_POS, int Y_POS, QWidget *Widget);
    ~GraphicWidgetNode();


    void SetScenePosition(int PosX, int PosY);
    void SetWidgetActive(bool active);
    std::vector<Node*> PortNodes;
    QList<LinkLine*> LinkLineList;

    QWidget* WindowNode = 0;
    void setWidget(QWidget* widget);
	LinkLine* ConnectNode(int sourcePortNode ,GraphicWidgetNode* Node, int destPortNode);

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

#endif // GRAPHICWIDGETNODE_H
