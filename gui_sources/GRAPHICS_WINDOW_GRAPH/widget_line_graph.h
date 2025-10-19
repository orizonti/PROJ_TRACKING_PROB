// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef WIDGET_LINEGRAPH_H
#define WIDGET_LINEGRAPH_H

#include <QWidget>
#include <QQuickWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QQmlContext>
#include "linegraph.h"
#include "AIM_IMAGE_IMITATION/sinus_generator_class.h"
#include <QThread>
#include "widget_adjustable.h"

class WidgetLineGraph : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetLineGraph(QWidget *parent = nullptr);
    ~WidgetLineGraph();

    void InitGraphWidget();
    SinusGeneratorClass* SinusGenerator;
    QThread threadGenerator;

private:
    QQuickWidget *PlotWidget;
    QHBoxLayout *Layout;

    GraphPointsLineInterface *GraphPointsStorage;
    GraphPointsLineInterface *GraphPointsStorage2;
signals:
void StartGenerateSinus(bool);
void StopThreads();
};

#endif 
