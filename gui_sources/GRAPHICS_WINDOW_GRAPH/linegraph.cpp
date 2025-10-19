// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "linegraph.h"
#include <QRandomGenerator>
#include <qlineseries.h>

GraphPointsLineInterface::GraphPointsLineInterface(QObject *parent)
{
    Q_UNUSED(parent)

    ListPoints = new QLineSeries;
}


GraphPointsLineInterface::~GraphPointsLineInterface()
{
    delete ListPoints;
}

QLineSeries* GraphPointsLineInterface::GetSeries() 
{
    return ListPoints;
}


void GraphPointsLineInterface::SlotClearSeries() { ListPoints->clear(); }


void GraphPointsLineInterface::SetSeries(QLineSeries *series) { }

void GraphPointsLineInterface::SlotAddNewPoint(QPair<float,float> Coord)
{
   ListPoints->append(counter++, Coord.first*Scale);

   if(ListPoints->points().size() > 300) 
   {
     counter = 0;
     ListPoints->clear();
   }
}

//==================================================================
