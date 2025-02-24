#ifndef WIDGET_ADJUSTABLE_H
#define WIDGET_ADJUSTABLE_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QGraphicsScene>
#include <QRegularExpression>


class WidgetAdjustable : public QWidget
{
	Q_OBJECT
    public:
	WidgetAdjustable(QWidget* parent);
    void ChangeFont(int FontSize);

    int WIDGET_FONT_SIZE = 12;

public slots:
void SlotSetWindowSize(int FontSize);
void AdjustWindow();
signals:
void SignalHideWidget();
};

class AdjustableLabel : public QLabel
{
	Q_OBJECT

    public:
	AdjustableLabel(QWidget* parent);
    void ChangeFont(int FontSize);

    int WIDGET_FONT_SIZE = 12;

public slots:
void SlotSetWindowSize(int FontSize);
void AdjustWindow();
};

#endif // WIDGET_ADJUSTABLE_H