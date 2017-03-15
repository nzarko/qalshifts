#include "legendform.h"
#include "ui_legendform.h"

LegendForm::LegendForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LegendForm)
{
    ui->setupUi(this);
}

LegendForm::~LegendForm()
{
    delete ui;
}
