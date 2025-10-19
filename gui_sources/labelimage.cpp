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
	SizeImage = SettingsRegister::GetPair("CAMERA_IMAGE_SIZE");
	this->setBaseSize(SizeImage.first,SizeImage.second);
	//qDebug() << "AIMING IMAGE SIZE: " << SizeImage.first << SizeImage.second;
}

LabelImage::~LabelImage()
{
}

void LabelImage::mouseMoveEvent(QMouseEvent * ev)
{
	
	const auto& LabelSize = this->size();
	double x = ev->position().x()*(float)SizeImage.first/LabelSize.width();
	double y = ev->position().y()*(float)SizeImage.second/LabelSize.height();
	//qInfo()<<"[LABEL   ]" << "POS MOVE - " << x << y;

	emit SignalMovePos(x,y);
	
}

void LabelImage::mousePressEvent(QMouseEvent * ev)
{
	const auto& LabelSize = this->size();
	X_Pressed = ev->position().x()*(float)SizeImage.first/LabelSize.width();
	Y_Pressed = ev->position().y()*(float)SizeImage.second/LabelSize.height();
	qInfo() <<"[LABEL    ]"<< "POS PRESS - " << X_Pressed << Y_Pressed << "SIZE: " << this->size();

	emit SignalPressedPos(X_Pressed,Y_Pressed);
	
}

