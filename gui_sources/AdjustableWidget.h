#ifndef ADJUSTABLEWIDGET_H
#define ADJUSTABLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QGraphicsScene>
#include <QRegularExpression>


class AdjustableWidget : public QWidget
{
	Q_OBJECT

    public:
	AdjustableWidget(QWidget* parent);
    void ChangeFont(int FontSize);

    int WIDGET_FONT_SIZE = 12;

public slots:
void SlotSetWindowSize(int FontSize);
void AdjustWindow();
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

#endif // ADJUSTABLEWIDGET_H