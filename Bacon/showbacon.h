#ifndef SHOWBACON_H
#define SHOWBACON_H

#include <QDialog>
#include <QStandardItemModel>
#include <QMenu>
#include <QClipboard>

namespace Ui {
class ShowBacon;
}

class ShowBacon : public QDialog
{
    Q_OBJECT

public:
    explicit ShowBacon(QWidget *parent = 0,float* out=NULL,int ncol=0,int nrow=0,float min=0,float by=0,float max=0,int k=0);
    ~ShowBacon();
    void setupOut();

    float* out_Data;
    int out_ncol;
    int out_nrow;
    float dmin;
    float dby;
    float dmax;
    int K;
    QStandardItemModel *outData;

private:
    Ui::ShowBacon *ui;

private slots:
    void copy(QPoint p);
};

#endif // SHOWBACON_H
