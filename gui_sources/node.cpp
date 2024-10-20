
#include "link_line.h"
#include "node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

Node::Node()
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}
Node::Node(int NumberInWidget): Node() { this->Number = NumberInWidget; }

void Node::setNodeActive(bool active) { if(active)  this->hide(); else this->hide(); this->setActive(active); }
bool Node::advance()                  { if(newPos == pos()) return false; setPos(newPos); return true; }

QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-7, -7, 10, 10);
    return path;
}
void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);
	painter->drawEllipse(-7, -7, 14, 14);

	QRadialGradient gradient(-3, -3, 10);
	if (option->state & QStyle::State_Sunken) {
		gradient.setCenter(3, 3);
		gradient.setFocalPoint(3, 3);
		gradient.setColorAt(1, QColor(Qt::lightGray));
		gradient.setColorAt(0, QColor(Qt::darkGray));
	}
	else {
		gradient.setColorAt(0, Qt::lightGray);
		gradient.setColorAt(1, Qt::darkGray);
	}
	painter->setBrush(gradient);

	painter->setPen(QPen(Qt::black, 0));
	painter->drawEllipse(-7, -7, 14, 14);
}


QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

