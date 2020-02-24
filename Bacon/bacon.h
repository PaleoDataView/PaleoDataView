/********************************************************************************/
/*    This file is part of PaleoDataView.                       					*/
/*                                                                      		*/
/*    PaleoDataView is free software: you can redistribute it and/or modify 		*/
/*    it under the terms of the GNU General Public License as published by  	*/
/*    the Free Software Foundation, either version 3 of the License, or     	*/
/*    (at your option) any later version.                                   	*/
/*                                                                          	*/
/*    PaleoDataView is distributed in the hope that it will be useful,          	*/
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of        	*/
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*/
/*    GNU General Public License for more details.                              */
/*                                                                              */
/*    You should have received a copy of the GNU General Public License         */
/*    along with PaleoDataView.  If not, see <http://www.gnu.org/licenses/>.        */
/********************************************************************************/

#ifndef BACON_H
#define BACON_H

#include <QDialog>
#include <QMainWindow>
#include "AMS/amsdata.h"
#include <QProcess>
#include <QDir>
#include "Editor/graph.h"
#include <QTime>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <vector>
#include "General/resources.h"
#include <QApplication>
#include <QDesktopServices>
#include <QTimer>
#include <Bacon/showbacon.h>
#include "Dialog/busybacon.h"
#include <QMenu>
#include <QClipboard>
namespace Ui {
class Bacon;
}

class Bacon : public QDialog
{
    Q_OBJECT

public:
    explicit Bacon(QMainWindow *mainWindow,Inventory *i=0);
    ~Bacon();
    void reject();
    void showEvent(QShowEvent *event);

private slots:
    void Bacon_Run();
    void Bacon_Output();
    void Bacon_Finish();
    void editData(QModelIndex r,QModelIndex c);
    void selected(QModelIndex mi);
    void selected2(QModelIndex mi);
    void refresh();
    void init();
    void reload();
    void reloadBacon();
    void save();
    void replotAMS();
    void openManual();
    void apply();
    void target();
    void updateGraph();
    void abort();

    void OpenBacon();

    void copy(QPoint p);
    void copy_2(QPoint p);
    void copy_3(QPoint p);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void createAMSPlot();
    void createAccPlot();
    void createMemPlot();
    void createItPlot();
    void loadResults();
    void calcResults();

    void setupTable();
    void setupParameters();
    void setupEnsamble();

    void paintEvent(QPaintEvent*);
    void createBaconFile();
    void showResults();
    QString getParameter(QString str);
    void setParameter(QString name,QString value);
    double gauss(double mu, double sigma);
    void checkParameters();
    void NetCDF_Error(QString fname,QString action,int value);
    void NetCDF_Error_Message();


    Ui::Bacon *ui;
    QProcess *p_bacon;

    AMSData *amsdata;
    Graph *amsplot;
    Graph *accplot;
    Graph *memplot;
    Graph *itplot;
    QStandardItemModel *modelData;
    QStandardItemModel *para;
    QStandardItemModel *enData;

    //QStandardItem *var_value;
    int select,select2;
    QStringList str_par;
    QStringList str_val;
    QStringList str_com;
    std::vector<float> out;
    int out_nrow;
    int out_nrow_real;
    int out_ncol;
    float *hist;
    int hist_nrow;
    int hist_ncol;
    float *agemodel;
    int agemodel_nrow;
    int agemodel_ncol;
    float *acchist;
    int acchist_ncol;
    int acchist_nrow;
    float *memhist;
    int memhist_ncol;
    int memhist_nrow;
    Resources *resources;
    int baconrestart=1;
    int bacon_invalid=0;
    int reloadB=0;
    QByteArray sp,sp_2,sp_3,sp_4;
    Inventory *inv;
    bool changes;

    float *data1;
    bool *data_use;
    float *data_error;
    float *data_error2;
    QColor *col;
    QColor *coll;
    Qt::PenStyle *lstyle;
    int *lwidth;
    int *symbol;
    int *mark;
    int *poly;
    float *area_x;
    float *area_y;

    float *data2;
    QColor *col2;
    int *poly2;

    float *data3;
    QColor *col3;
    int *poly3;

    float *data4;

    ShowBacon *showB;

    BusyBacon *busy;

    int quit=0;

    QMainWindow *mainW;

    int *baconflag;

    QString error_text;
    int show_error=0;
};

#endif // BACON_H
