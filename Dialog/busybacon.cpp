#include "busybacon.h"
#include "ui_busybacon.h"

BusyBacon::BusyBacon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BusyBacon)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(abort()));
}

BusyBacon::~BusyBacon()
{
    delete ui;
}


void BusyBacon::abort(){
    qDebug()<<"Button pressed";
    emit(abortBacon());
}
