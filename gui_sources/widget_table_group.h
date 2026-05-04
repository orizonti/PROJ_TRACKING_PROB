#ifndef TABLE_CONTAINER_WIDGET_H
#define TABLE_CONTAINER_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QLine>
#include "widget_adjustable.h"
#include <QPushButton>
#include <QHBoxLayout>


class WidgetTableGroup : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetTableGroup(QWidget* parent = 0);

    void AddWidget(WidgetAdjustable* Widget);

};

#endif 
