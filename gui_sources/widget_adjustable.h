#ifndef WIDGET_ADJUSTABLE_H
#define WIDGET_ADJUSTABLE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QGraphicsScene>
#include <QRegularExpression>
#include <qevent.h>
#include <qnamespace.h>


class WidgetAdjustable : public QWidget
{
	Q_OBJECT
    public:
	WidgetAdjustable(QWidget* parent);
    void ChangeFont(int FontSize);

    int WIDGET_FONT_SIZE = 12;

void linkToWidget(WidgetAdjustable* Link) 
{ 
    LinkedWidget = Link;
    LinkedWidget->move(pos() + QPoint(this->width() + 5,0) );
    QObject::connect(this, SIGNAL(SignalClosed()), LinkedWidget, SLOT(close()));
};
WidgetAdjustable* LinkedWidget = nullptr;

void moveEvent(QMoveEvent* event)
{
  if(LinkedWidget != nullptr) LinkedWidget->move(LinkedWidget->pos() + event->pos() - event->oldPos());
  QWidget::moveEvent(event);
}
void closeEvent(QCloseEvent* event)
{
  emit SignalClosed();
  QWidget::closeEvent(event);
}


public slots:
void slotSetWindowSize(int FontSize);
void AdjustWindow();
signals:
void SignalHideWidget();
void SignalClosed();
};

class AdjustableLabel : public QLabel
{
	Q_OBJECT

    public:
	AdjustableLabel(QWidget* parent);
    void ChangeFont(int FontSize);

    int WIDGET_FONT_SIZE = 12;

public slots:
void slotSetWindowSize(int FontSize);
void AdjustWindow();
};

#endif // WIDGET_ADJUSTABLE_H
