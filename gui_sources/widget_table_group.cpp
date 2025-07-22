#include "widget_table_group.h"
#include "ui_widget_table_group.h"
WidgetTableGroup::WidgetTableGroup(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetTableGroup)
{
    ui->setupUi(this);
    layouts.push_back(ui->layout1);
    layouts.push_back(ui->layout2);
    current_layout = layouts.begin();
}



void WidgetTableGroup::AddWidget(WidgetAdjustable* Widget)
{
     if(current_layout == layouts.end()) return;

     (*current_layout)->addWidget(Widget);
       current_layout++;
       Widget->setParent(this);
       
}

