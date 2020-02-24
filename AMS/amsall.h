#ifndef AMSALL_H
#define AMSALL_H

#include <QDialog>
#include <QDebug>
#include <QMainWindow>
#include <QStandardItemModel>
#include "General/inventory.h"
#include "amsdata.h"

#include "General/resources.h"
#include <QFileDialog>
#include "Dialog/attdialog.h"
#include "amsedit.h"
#include "Editor/graph.h"
#include "Bacon/bacon.h"
#include <QApplication>
#include <QFileInfo>

namespace Ui {
class AMSAll;
}

class AMSAll : public QDialog
{
    Q_OBJECT

public:
    /*******************************************************************************************************/
    //  This version goes through all proxies in the inventory and tries
    //  to calibrate 'AMS' labeled entries in the age files.
    //  Carefull!!! This recalibrates all files and saves the results imediately!!!
    //  This function should only be used to calibrate a new database for the first time
    //  The Constructor sets up the simple GUI.
    //  Parameters  :
    //  mainWindow  : child of mainwindow, because it is called form the menu
    //  inventory   : current inventory
    //  str         : current proxy
    /*******************************************************************************************************/
    explicit AMSAll(QMainWindow *mainWindow,Inventory *inventory,QString str);

    /*******************************************************************************************************/
    //  This version goes through all proxies in the inventory and tries'äääääääää
    /*******************************************************************************************************/
    ~AMSAll();
    void checkReversals();
    void calibrateAll();
    void reservoirAll();
    void reservoir(int mode);
    void reservoirLoad();

private slots:
    void set_CalibData();
    void start();
signals:
    void refresh();

private:
    Ui::AMSAll *ui;
    QMainWindow *mainW;
    Inventory *inv_main;
    QString proxy;
    Inventory *inv;
    AMSData *amsdata;
    int select;
    Resources *resources;

};

#endif // AMSALL_H
