
#include "widget_table_group.h"


static QString styleBaseWidget2
{
"QWidget { background-color: rgb(46, 65, 83);"
"  border: 0px solid #455364;"
"  padding: 0px;"
"  color: #E0E1E3;"
"  selection-background-color: #346792;"
"  selection-color: #E0E1E3; }"

"QWidget:disabled { background-color: #19232D;"
"  color: #9DA9B5;"
"  selection-background-color: #26486B;"
"  selection-color: #9DA9B5; }"

"QWidget::item:selected { background-color: #346792; }"
"QWidget::item:hover:!selected { background-color: #1A72BB; }"


"QLabel { border: 2px solid line black;"
"background-color: rgb(99, 69, 44);"
"color: rgb(210, 204, 182);"
"font: 600 12pt Ubuntu;"
"border-radius: 4px; }"


"QSpinBox { border: 2px solid line black;"
"font: 600 14pt Ubuntu Sans;"
"background-color: rgba(152, 106, 68,120);"
"border-radius: 4px; }"

"QSpinBox::up-button { subcontrol-origin: border;"
"subcontrol-position: top right; "
"width: 16px; "
"border-radius: 4px;"
"border-width: 1px;"
"border: 1px solid line black;"
"background-color: rgba(181, 131, 90,200); }"

"QSpinBox::down-button { subcontrol-origin: border;"
"subcontrol-position: bottom right; "
"width: 16px;"
"border-image: url(:/images/spindown.png) 1;"
"border-width: 1px;"
"border-top-width: 0;"
"border: 1px solid line black;"
"border-radius: 4px;"
"background-color: rgba(181, 131, 90,200); }"
};


WidgetTableGroup::WidgetTableGroup(QWidget* parent) : WidgetAdjustable(parent)
{
  this->setStyleSheet(styleBaseWidget2);
  this->setLayout(new QVBoxLayout);
  this->layout()->setSpacing(1);
  this->layout()->setContentsMargins(1,1,1,1);
}


void WidgetTableGroup::AddWidget(WidgetAdjustable* Widget)
{
      this->layout()->addWidget(Widget); 
}

