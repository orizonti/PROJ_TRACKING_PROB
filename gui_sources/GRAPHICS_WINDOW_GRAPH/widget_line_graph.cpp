// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "widget_line_graph.h"
#include <qnamespace.h>

WidgetLineGraph::WidgetLineGraph(QWidget *parent): WidgetAdjustable(parent)
{
    Q_UNUSED(parent)
    GraphPointsStorage = new GraphPointsLineInterface;
    GraphPointsStorage2 = new GraphPointsLineInterface;

    GraphPointsStorage->Scale = 0.02;
    GraphPointsStorage2->Scale = 0.01;

    PlotWidget = new QQuickWidget;

                    Layout = new QHBoxLayout;
                    Layout->addWidget(PlotWidget, 1);
    this->setLayout(Layout);

    InitGraphWidget();

    SinusGenerator = new SinusGeneratorClass;
    SinusGenerator->MoveGeneratorToThread(&threadGenerator);
    SinusGenerator->TimerPeriod = 5;
    threadGenerator.start();

    QObject::connect(SinusGenerator, SIGNAL(SignalNewCoord(QPair<float,float>)),GraphPointsStorage, SLOT(SlotAddNewPoint(QPair<float,float>)));
    QObject::connect(SinusGenerator, SIGNAL(SignalNewCoord(QPair<float,float>)),GraphPointsStorage2, SLOT(SlotAddNewPoint(QPair<float,float>)));
    QObject::connect(this, SIGNAL(StartGenerateSinus(bool)),SinusGenerator, SLOT(slotStartGenerate(bool)),Qt::QueuedConnection);

    emit StartGenerateSinus(true);

}

WidgetLineGraph::~WidgetLineGraph()
{
    qDebug() << "CLOSE LINE GRAPH WIDGET";
    emit StartGenerateSinus(false);
    QThread::currentThread()->sleep(3);
    threadGenerator.quit();
    delete PlotWidget;
    delete Layout;
    delete GraphPointsStorage;
    delete GraphPointsStorage2;
    delete SinusGenerator;
}


void WidgetLineGraph::InitGraphWidget()
{
    QString qmlPath(QStringLiteral("%1/../gui_sources/GRAPHICS_WINDOW_GRAPH/main.qml"));
            qmlPath = qmlPath.arg(QGuiApplication::applicationDirPath());
    PlotWidget->engine()->addImportPath(qmlPath);
    qDebug() << "             INIT QML: " << qmlPath;

    QQmlContext *context = PlotWidget->engine()->rootContext();
    context->setContextProperty("GraphPointsStorage", GraphPointsStorage);
    context->setContextProperty("GraphPointsStorage2", GraphPointsStorage2);

    //insertSeries(int index, AbstractSeries series)
    //QObject* model = PlotWidget->findChild<QObject*>("GraphObject");
    //QMetaObject::invokeMethod(model,"insertSeries",0, GraphPointsStorage->ListPoints);

    PlotWidget->setSource(QUrl(qmlPath));
    PlotWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    PlotWidget->resize(500,500);
}

