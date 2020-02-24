#ifndef TARGETSELECT_H
#define TARGETSELECT_H

#include <QDialog>
#include <QDir>
#include "General/resources.h"
#include "General/inventory.h"
#include "Editor/graph.h"

namespace Ui {
class TargetSelect;
}

class TargetSelect : public QDialog
{
    Q_OBJECT

public:
    explicit TargetSelect(QDialog *parent = nullptr,Inventory *i=nullptr,QString pr="",QImage *map=nullptr);
    ~TargetSelect();
    void paintEvent(QPaintEvent *);
    QString getFilename();
    void readTarget();
    void createList();
    void createMeta();
    void createTargetPlot();
    void createAgeModelPlot();
    void createMap(int first);
    void NetCDF_Error(QString fname,QString action,int value);
    void NetCDF_Error_Message();

private slots:
    void targetSelected();
    void rejected();
    void accepted();
    void updateGraph();
    void selected_changed(int plot,int x,int y);

signals:
    void targetS(QString);

private:
    Ui::TargetSelect *ui;
    QImage *worldmap;

    QString filename;

    Resources *resources;

    QString proxy;

    QDialog *diag;

    Inventory *inv;

    // Custom Target Data
     QVector <QStringList*> target_data;
     QStringList target_meta;
     QVector <QStringList*> target_age;
     int target_length;

    // inventory
     int entries;
     QVector <QStringList*> target_inv;
     int longitude;
     int latitude;
     int corename;
     int type;

     QString error_text;
     int show_error;

     Graph* pl1;
     QVector<float*> data1;
     QVector<QString*> com1;
     QVector<bool*> use1;
     QVector <QColor*> col1;
     int plotdata_1;
     Qt::PenStyle *style;
     int *stype;

     Graph* pl2;
     QVector<float*> data2;
     QVector<QString*> com2;
     QVector<bool*> use2;
     QVector <QColor*> col2;
     Qt::PenStyle *style2;
    int *symbolsize;

    Graph *map;
    float *data;
    QString *com;
    QString *lab;
    QColor *col;


    Inventory *inv2;




};

#endif // TARGETSELECT_H
