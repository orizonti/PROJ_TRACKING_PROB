
#include "node.h"
#include "link_line.h"
#include "widget_scene_node_interface.h"

#define TAG "[ WIDGTES     ]" 
int WidgetSceneNodeInterface::CounterWidget = 0;

WidgetSceneNodeInterface::WidgetSceneNodeInterface(QGraphicsScene* Scene, int X_POS, int Y_POS,QWidget* Widget)
{
Init(Widget);

Scene->addItem(this);
Scene->addItem(PortNodes[0]); Scene->addItem(PortNodes[1]); Scene->addItem(PortNodes[2]); Scene->addItem(PortNodes[3]);
this->SetScenePosition(X_POS, Y_POS); this->UpdateWidget();
}

WidgetSceneNodeInterface::WidgetSceneNodeInterface(QGraphicsScene* Scene, int X_POS, int Y_POS, WidgetAdjustable *Widget)
{
Init(Widget);

Scene->addItem(this);
Scene->addItem(PortNodes[0]); Scene->addItem(PortNodes[1]); Scene->addItem(PortNodes[2]); Scene->addItem(PortNodes[3]);
this->SetScenePosition(X_POS, Y_POS); this->UpdateWidget();
connect(Widget, &WidgetAdjustable::SignalHideWidget, this, &WidgetSceneNodeInterface::SlotHideWidget);
}

void WidgetSceneNodeInterface::Init(QWidget* widget)
{
WindowNode = widget; 
NumberWidget = CounterWidget; CounterWidget++;
this->setWidget(WindowNode); 
this->setFlag(QGraphicsItem::ItemIsMovable, true);
this->setFlag(QGraphicsItem::ItemIsSelectable, true);
this->setFlag(QGraphicsItem::ItemIsFocusable, true);
this->setAcceptedMouseButtons(Qt::AllButtons);

PortNodes.push_back(new Node(1));
PortNodes.push_back(new Node(2));
PortNodes.push_back(new Node(3));
PortNodes.push_back(new Node(4));
this->setEnabled(true);
}


void WidgetSceneNodeInterface::HideNodes() { for(auto Node: PortNodes) Node->hide(); }
void WidgetSceneNodeInterface::HideLinks() { for(auto Line: LinkLineList) Line->hide(); }

void WidgetSceneNodeInterface::SlotHideWidget()
{
   qDebug() << "WIDGET NODE HIDE: " << NumberWidget;
   HideNodes(); HideLinks(); this->hide();
}

WidgetSceneNodeInterface::~WidgetSceneNodeInterface()
{
   delete PortNodes[0];
   delete PortNodes[1];
   delete PortNodes[2];
   delete PortNodes[3];
}

void WidgetSceneNodeInterface::setWidget(QWidget *widget)
{
    QGraphicsProxyWidget::setWidget(widget);
}


void WidgetSceneNodeInterface::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
   //qDebug() << TAG << "MOUSE DOUBLE: " << NumberWidget << " BUTTON: " << event->button();      
   emit SignalWidgetPressed(NumberWidget);
   QGraphicsProxyWidget::mouseDoubleClickEvent(event);
}

void WidgetSceneNodeInterface::focusInEvent(QFocusEvent *event)
{
	this->setWindowFlags(Qt::Window);
   return QGraphicsProxyWidget::focusInEvent(event);
}
 void WidgetSceneNodeInterface::focusOutEvent(QFocusEvent *event)
{

	this->scene()->update(this->scene()->sceneRect());
	this->setWindowFlags(Qt::Widget);
   return QGraphicsProxyWidget::focusOutEvent(event);
}
QVariant WidgetSceneNodeInterface::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
   //UpdateWidget();
   switch (change) { case ItemPositionHasChanged: UpdateWidget(); break; 
   default: break; };
   return QGraphicsProxyWidget::itemChange(change,value);
}

LinkLine* WidgetSceneNodeInterface::ConnectNode(int sourcePortNode ,WidgetSceneNodeInterface* Node, int destPortNode)
{
    LinkLineList.append(new LinkLine(this, Node,sourcePortNode,destPortNode)); 
    Node->LinkLineList.append(LinkLineList.last());

	 this->scene()->addItem(LinkLineList.last());


    LinkLineList.last()->adjust();
    this->scene()->update(LinkLineList.last()->boundingRect());

    //auto [Node1, Port1, Node2, Port2] = LinkLineList.last()->GetLinkScheme();
    return LinkLineList.last();
}


void WidgetSceneNodeInterface::UpdateWidget()
{
int offset_x = 4;
int offset_y = -4;


if (PortNodes.size() == 4)
{
	QPointF pos = this->pos();
	QRectF rect = this->rect();
   this->NodePositionX = this->pos().x();
   this->NodePositionY = this->pos().y();

	QPointF InpP = QPointF(pos.x() + offset_x, pos.y() + rect.size().height() / 2);
	QPointF OutP = QPointF(pos.x() + rect.size().width() + offset_y, pos.y() + rect.size().height() / 2);
	PortNodes[0]->setPos(InpP.x(), InpP.y());
	PortNodes[1]->setPos(OutP.x(), OutP.y());

     PortNodes[2]->setPos(pos.x()+this->rect().width()/2,pos.y()+offset_x);
     PortNodes[3]->setPos(pos.x()+this->rect().width()/2,pos.y()+this->rect().height()+offset_y);

foreach (LinkLine *link, LinkLineList) link->adjust(); // graph->itemMoved();

}
}

void WidgetSceneNodeInterface::SetScenePosition(int PosX, int PosY) 
{
   this->setPos(PosX,PosY); NodePositionX = PosX; NodePositionY = PosY; 
   //qDebug() << "WIDGET SET POS: " << PosX << PosY;
}

void WidgetSceneNodeInterface::SetWidgetActive(bool active)
{
    qDebug() << "[ MAIN WIND ] SET WIDGET NUMBER: " << NumberWidget << " ACTIVE: " << active; 
    if(active) this->show(); else hide();

    for(auto node: PortNodes) node->setNodeActive(active);
    for(auto link: LinkLineList) link->setLinkActive(active);
}
