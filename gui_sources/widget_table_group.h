#ifndef TABLE_CONTAINER_WIDGET_H
#define TABLE_CONTAINER_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QLine>
#include "CAMERA_INTERFACE/camera_interface_class.h"
#include "widget_adjustable.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetTableGroup; }
QT_END_NAMESPACE


class WidgetTableGroup : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetTableGroup(QWidget* parent = 0);

    void AddWidget(WidgetAdjustable* Widget);
    std::vector<QHBoxLayout*> layouts;
    std::vector<QHBoxLayout*>::iterator current_layout;

public  slots:
signals:

private:
Ui::WidgetTableGroup *ui;
};

#endif 
