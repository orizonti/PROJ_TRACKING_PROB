#ifndef LINKLINE_H
#define LINKLINE_H
#include <QGraphicsObject>
#include <QDebug>
#include "widget_scene_node_interface.h"


class Node;

class LinkLine : public QGraphicsObject
{
Q_OBJECT
public:
    LinkLine(WidgetSceneNodeInterface *sourcePortNode, WidgetSceneNodeInterface *destPortNode, int PortOutput, int PortInput);
	void SetColor(QColor Color) { LinkLineColor = Color; };
    int NumberLink = 0;
    static int LinkCount;

    Node *sourcePortNode() const;
    Node *destPortNode() const;
    WidgetSceneNodeInterface* sourcenode() const;
    WidgetSceneNodeInterface* destNode() const;
	QColor LinkLineColor = QColor(Qt::black);
    std::tuple<int,int,int,int> GetLinkScheme();
    QString PrintLinkLineScheme();

    void adjust();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    void setLinkActive(bool active) 
    { 
        if(active) this->show(); else this->hide(); this->setActive(active);
        qDebug() << "SET LINK ACTIVE: " << active;
    }
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    WidgetSceneNodeInterface* source;
    WidgetSceneNodeInterface* dest;
    Node* sourcePort;
    Node* destPort;

    QPainterPath Curve;

    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize;
signals:
void SignalLinkPressed();
};

#endif // LINKLINE_H
