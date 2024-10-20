#ifndef LINKLINE_H
#define LINKLINE_H
#include <QGraphicsObject>
#include <QDebug>
#include "graphicwidgetnode.h"


class Node;

class LinkLine : public QGraphicsObject
{
Q_OBJECT
public:
    LinkLine(GraphicWidgetNode *sourcePortNode, GraphicWidgetNode *destPortNode, int PortOutput, int PortInput);
	void SetColor(QColor Color) { LinkLineColor = Color; };
    int NumberLink = 0;
    static int LinkCount;

    Node *sourcePortNode() const;
    Node *destPortNode() const;
    GraphicWidgetNode* sourceNode() const;
    GraphicWidgetNode* destNode() const;
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
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    GraphicWidgetNode* source;
    GraphicWidgetNode* dest;
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
