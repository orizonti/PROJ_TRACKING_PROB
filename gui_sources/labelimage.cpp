#include "labelimage.h"
#include "qevent.h"
#include "qdebug.h"
#include "register_settings.h"
#include <QObject>
#include <QSizePolicy>

LabelImage::LabelImage(QWidget *parent)
	: AdjustableLabel(parent)
{
	this->setObjectName("ImageLabelDisplay");
}

     LabelImage::~LabelImage() { }
void LabelImage::mouseMoveEvent(QMouseEvent * ev)
{
	const auto& LabelSize = this->size();
	double x = ev->position().x()/LabelSize.width();
	double y = ev->position().y()/LabelSize.height();
	//qInfo()<<"[LABEL   ]" << "POS MOVE - " << x << y;
}

void LabelImage::mousePressEvent(QMouseEvent * ev)
{
	const auto& LabelSize = this->size();
	PosPressed.first  = ev->position().x()/LabelSize.width();
	PosPressed.second = ev->position().y()/LabelSize.height();
	//qInfo() <<"[LABEL    ]"<< "POS PRESS - " << PosPressed.first << PosPressed.second << "SIZE: " << this->size();

	emit signalPosPressed(PosPressed);
}

