#include "AdjustableWidget.h"
#include <qregularexpression.h>

AdjustableWidget::AdjustableWidget(QWidget* parent) : QWidget(parent)
{
}

void AdjustableWidget::ChangeFont(int FontSize)
{
  QString Style = this->styleSheet();

  Style.replace(QRegularExpression("font: \\d+pt"), QString("font: %1pt").arg(FontSize));
  this->setStyleSheet(Style);
  this->resize(0,0);
}

void AdjustableWidget::SlotSetWindowSize(int FontSize)
{
    ChangeFont(FontSize);
    QTimer::singleShot(10,this,&AdjustableWidget::AdjustWindow);
}

void AdjustableWidget::AdjustWindow()
{
    this->resize(0,0);
}

AdjustableLabel::AdjustableLabel(QWidget* parent) : QLabel(parent)
{
}

void AdjustableLabel::ChangeFont(int FontSize)
{
  QString Style = this->styleSheet();

  Style.replace(QRegularExpression("font: \\d+pt"), QString("font: %1pt").arg(FontSize));
  this->setStyleSheet(Style);
  this->resize(0,0);
}

void AdjustableLabel::SlotSetWindowSize(int FontSize)
{
    ChangeFont(FontSize);
    QTimer::singleShot(10,this,&AdjustableLabel::AdjustWindow);
}

void AdjustableLabel::AdjustWindow()
{
    this->resize(0,0);
}


  //auto Labels = this->findChildren<QLabel*>();
  //auto Checks = this->findChildren<QCheckBox*>();

  //for(QLabel* Label: Labels)
  // Label->setStyleSheet("QLabel{font: 10pt Sans Serif; background-color: rgb(218, 206, 161);}");

  //for(QLabel* Label: Labels)
  //  ChangeFontToWidget(Label,10);

  // for(QLabel* Label: Labels)
  //    qInfo() << TAG << "LABEL: " << Label->styleSheet(); 