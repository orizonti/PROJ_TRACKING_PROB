
#include "link_line.h"
#include "node.h"

#include <cstddef>
#include <math.h>

#include <QPainter>
#include <QDialog>
#include <QGraphicsSceneMouseEvent>


int LinkLine::LinkCount = 0;
static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

LinkLine::LinkLine(WidgetScenenodeInterface* sourcePortNode, WidgetScenenodeInterface* destPortNode, int PortOutput, int PortIntput)
    : arrowSize(10)
{
    setAcceptedMouseButtons(Qt::RightButton);
    source = sourcePortNode;
    dest = destPortNode;

    sourcePort = sourcePortNode->PortNodes[PortOutput];
    destPort = destPortNode->PortNodes[PortIntput];

    this->setEnabled(Qt::ItemIsEnabled);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);

    adjust();

    LinkCount++; NumberLink = LinkCount;
}
Node* LinkLine::sourcePortNode() const { return sourcePort; }
Node* LinkLine::destPortNode() const { return destPort; }

WidgetScenenodeInterface* LinkLine::sourcenode() const { return source; }
WidgetScenenodeInterface* LinkLine::destNode() const  {return dest; }

void LinkLine::adjust()
{
    if (!sourcePort || !destPort) return;

    //========================================
                    sourcePoint = mapFromItem(sourcePort,0,0);
                    destPoint = mapFromItem(destPort,0,0);
                    

                   double Cx1 = sourcePoint.x();
                   double Cy1 = destPoint.y();

                   double Cx2 = destPoint.x();
                   double Cy2 = sourcePoint.y();

                   if (destPort->Number == 1 || destPort->Number == 2)
                   {
                   Cx1 = destPoint.x();
                   Cy1 = sourcePoint.y();

                   Cx2 = sourcePoint.x();
                   Cy2 = destPoint.y();
                   }

                    QPointF C1 = QPointF(Cx1,Cy1);
                    QPointF C2 = QPointF(Cx2,Cy2);

                    this->Curve = QPainterPath(mapFromItem(sourcePort,0,0));
                          //Curve.lineTo(destPoint);
                    Curve.cubicTo(C1, C2, destPoint);

    //========================================
    prepareGeometryChange();

    if(boundingRect().isEmpty()) return;
    if(this->scene() == nullptr) return;

    //this->scene()->update(boundingRect());
    //this->scene()->update(this->sceneBoundingRect());
}

QRectF LinkLine::boundingRect() const
{
    if (!sourcePort || !destPort) return QRectF();

    QPointF sourcePoint = mapFromItem(sourcePort,0,0);
    QPointF destPoint = mapFromItem(destPort,0,0);

	int AvarageX = sourcePoint.x()/2 + destPoint.x()/2;
	int AvarageY = sourcePoint.y()/2 + destPoint.y()/2;

	int LengthX = abs(sourcePoint.x() - destPoint.x());
	int LengthY = abs(sourcePoint.y() - destPoint.y());

	QPoint TopLeft = QPoint(AvarageX - LengthX / 2 - 10, AvarageY - LengthY / 2 - 10);

	return QRect(TopLeft, QSize(LengthX+30, LengthY+30));
}

void LinkLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!sourcePort || !destPort)
        return;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    painter->setPen(QPen(this->LinkLineColor, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(Curve);

    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;
}

void LinkLine::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
  this->hide();
  this->setEnabled(false);

  for(int n = 0; n < source->LinkLineList.size(); n++) if(source->LinkLineList[n] == this) { source->LinkLineList.removeAt(n); 
                                                                                               dest->LinkLineList.removeAt(n);}
  LinkLine::LinkCount--;
  qDebug() << "DELET LINK: "<< this->NumberLink << "COUNT: " << LinkLine::LinkCount; 
  this->scene()->removeItem(this);
  //QPen pen; pen.setWidth(3); pen.setColor(Qt::red);
  //this->scene()->addRect(this->boundingRect(),pen);

  //QMessageBox dialog;
  //auto widget = this->scene()->addWidget(&dialog);
  //     widget->setPos(400,400);
  //dialog.exec();
  //dialog.show();
  //qDebug( ) << "RESULT : " << dialog.result();

}


std::tuple<int,int,int,int> LinkLine::GetLinkScheme()
{
    return std::make_tuple(source->NumberWidget, sourcePort->Number-1,dest->NumberWidget, destPort->Number-1);
}

QString LinkLine::PrintLinkLineScheme()
{
  return QString("%1 %2 : %3 %4").arg(source->NumberWidget).arg(sourcePort->Number-1).arg(dest->NumberWidget).arg(destPort->Number-1);
}