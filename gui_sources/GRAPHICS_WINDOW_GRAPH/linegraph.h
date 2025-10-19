// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef LineGraph_H
#define LineGraph_H

#include <QObject>
#include <QLineSeries>
#include <QSplineSeries>

class GraphPointsLineInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QLineSeries *SeriesPoints READ GetSeries WRITE SetSeries NOTIFY SignalSeriesChanged FINAL)
public:
    explicit GraphPointsLineInterface(QObject *parent = nullptr);
    ~GraphPointsLineInterface();

    QLineSeries *GetSeries(); 
            void SetSeries(QLineSeries *series);
    QLineSeries* ListPoints;
    double Scale = 1;
    int counter = 0;
public Q_SLOTS:
    void SlotClearSeries();
    void SlotAddNewPoint(QPair<float,float> Coord);

Q_SIGNALS:
    void SignalSeriesChanged();

};


#endif 
