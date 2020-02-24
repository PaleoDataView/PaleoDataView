#include "busy.h"
#include "ui_busy.h"

Busy::Busy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Busy)
{
    ui->setupUi(this);
}

Busy::~Busy()
{
    delete ui;
}
