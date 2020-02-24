#ifndef CALIB_H
#define CALIB_H

#include <QDialog>
#include <QMainWindow>
#include "General/inventory.h"
#include "Editor/graph.h"
#include "General/resources.h"
#include <QApplication>
namespace Ui {
class Calib;
}

class Calib : public QDialog
{
    Q_OBJECT

public:
    explicit Calib(QMainWindow *parent = nullptr,Inventory *inventory=nullptr,QString str=nullptr);
    ~Calib();

    void createPlot();
    void createSSTPlot();
    void setupTable();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void apply();
    void save();
    void updateGraph();

private:
    void paintEvent(QPaintEvent*);
    QMainWindow mainW;
    Ui::Calib *ui;
    Inventory *inv;
    QString proxy;

    Graph *pl1;
    float *data1;
    QString *com1;
    bool *use1;

    Graph *pl2;
    float *data2;
    QString *com2;
    bool *use2;

    int source=-1;
    int target=-1;

    QStandardItemModel *modelIsotope;

    QByteArray sp,sp_2;

    Resources *resources;
};

#endif // CALIB_H
