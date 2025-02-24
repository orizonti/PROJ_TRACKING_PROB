#ifndef PROCESS_CONTROLLER_WIDGET_H
#define PROCESS_CONTROLLER_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QLine>
#include "widget_adjustable.h"
#include "widget_processing_image_generic.h"
#include "controller_process_class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetProcessController; }
QT_END_NAMESPACE


class WidgetProcessController : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetProcessController(QWidget* parent = 0);
    void LinkTo(ProcessControllerClass* ProcessController);

private:
Ui::WidgetProcessController *ui;
};

#endif // PROCESS_CONTROLLER_WIDGET_H
