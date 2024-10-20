#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>

class LinkLine;

class Node : public QGraphicsItem
{
public:
    Node();
    Node(int NumberInWidget);

    int Number = 0;
    void setNodeActive(bool active);
    bool advance();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QPointF newPos;
};

#endif // NODE_H
