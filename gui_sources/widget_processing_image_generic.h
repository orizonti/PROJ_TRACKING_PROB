#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "cv_image_processing.h"
#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
#include <QPainter>
#include <QLine>
#include "widget_adjustable.h"
#include "labelimage.h"
#include <QBrush>
#include <QMoveEvent>
#include <QVBoxLayout>
#include "labelimage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetProcessingImage; }
QT_END_NAMESPACE


class WidgetMiniLabelsGroup : public WidgetAdjustable
{
    Q_OBJECT
    public: 
    WidgetMiniLabelsGroup(QWidget* parent = 0): WidgetAdjustable(parent) 
    {
        this->setLayout(new QVBoxLayout);
    };
    void AddLabel(LabelImage* label); 
    
    int NumberChannel = -1;

    QVector<LabelImage*> Labels;
public slots:
    void SlotSetActiveChannel(int Number);
    void SlotDisplayImage(const QImage& Image, int Channel);
signals: 
void SignalChannelChanged(int);

};

class WidgetProcessingImage : public WidgetAdjustable
{
    Q_OBJECT
public:
    WidgetProcessingImage(QString ModuleName = "ДИСПЛЕЙ", QWidget* parent = 0);
    QString TAG_NAME{"[ IMG_CTRL ]"};

void LinkToModule(std::shared_ptr<ImageSourceInterface> ImageSource);

int Counter = 0;
int ImageCounter = 0;
QRect  ObjectPosRect;
QRect  ObjectFindRect;
QLineF ObjectVelLine;
QLineF ArrowLeft;
QLineF ArrowRight;
QTimer timerDisplayMiniLabels;

        QImage  DisplayImage;
QVector<QImage> DisplayImagesMini;

std::shared_ptr<ImageSourceInterface> ImageSourceActive;
std::vector<std::shared_ptr<ImageSourceInterface>> ImageSources;

QPolygonF Trajectory;

void SetName(QString name); 
void CopyImageToDisplayImage(const QImage& Image);
bool FLAG_PAINT_TRAJECTORY = false;

QPen pen1{QBrush(Qt::green) ,1}; 
QPen pen2{QBrush(Qt::red)   ,1, Qt::DashLine}; 
QPen pen3{QBrush(Qt::yellow),1}; 
QPen pen4{QBrush(Qt::black) ,1}; 
QPen pen5{QBrush(Qt::white) ,1}; 
std::vector<QPen> pens{pen1, pen2, pen4, pen4, pen4, pen4};

QString strFreq; 
QString strPeriodProcess; 
QString strDisplayData; 

WidgetAdjustable* LinkedWidget = 0;
int NumberActiveChannel = -1;

void AddMiniLabel();
void moveEvent(QMoveEvent* event);

signals:
void SignalPosPressed(QPair<double,double>);
void SignalChannelChanged(int);

public  slots:
void SlotDisplayImage();
void SlotDisplayMiniLabels();
void SlotDisplayImage(const QImage& Image);
void SlotDisplayString(QString InfoString);
void SlotStartPaintTrajectory(bool OnOff);
void SlotPosPressed(int x, int y);
void SlotSetActiveChannel(int Channel);

private:
Ui::WidgetProcessingImage *ui;
};

#endif // MYWIDGET_H
