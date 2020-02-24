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

#ifndef CORRELATE_H
#define CORRELATE_H

#include <QDialog>
#include <QDebug>
#include <QMainWindow>
#include "General/inventory.h"
#include <QStandardItemModel>
#include "AMS/amsdata.h"
#include "Dialog/attdialog.h"
#include "amedit.h"
#include "Editor/graph.h"
#include <QColor>
#include <QApplication>
#include "General/resources.h"
#include <QMenu>
#include <QClipboard>
#include <QListView>
#include "targetselect.h"

namespace Ui {
class Correlate;
}

class Correlate : public QDialog
{
    Q_OBJECT

public:
    explicit Correlate(QMainWindow *mainWindow,Inventory *inventory,QImage* wmap,QString str);
    ~Correlate();
    void reject();

private:
    void paintEvent(QPaintEvent*);
    void setupTable(int column);
    void NetCDF_Error(QString fname,QString action,int value);
    void NetCDF_Error_Message();

protected:
    bool eventFilter(QObject *obj, QEvent *event);


private slots:
    void addPoint();
    void newPoint();
    void removePoint();
    void editPoint();
    void tableSelected(QModelIndex mi);

    void data_Selected(QString);
    void data_Selected2(QString);
    void target_Selected(QString);
    void apply();
    void applyAM();
    void attChanged(QString text,QString origin);
    void redraw();
    void createAMPlot();
    void createResultPlot();
    void createSRPlot();
    void createCorPlot(int autosize);
    void createLines();
    void clickRepaint(int i,float x,float y);
    void updateGraph();

    void copy(QPoint p);
    void readTarget();
    void selectTarget();
    void targetS(QString str);

private:
    QString proxy;
    int ndata;

    Ui::Correlate *ui;

    QMainWindow *mainW;
    Inventory *inv;
    Graph *cor_plot;
    Graph *am_plot;
    Graph *sr_plot;
    Graph *result_plot;
    QStandardItemModel *agemodelList;
    QStandardItemModel *modelData;
    //int selected=-1;
    int select=0;
    attDialog *attd;
    AMSData *amdata;
    QString target="Govin";
    int source_axis=2;// 2 is O : 3 is C Isotope data
    int target_axis=3;// 2:LH 3:Govin
    QByteArray sp,sp_2,sp_3;
    Resources *resources;
    bool changes;
    AMEdit *edit;
    attDialog *sttd;

    float *data1;
    bool *data_use1;
    float *data_error1;
    float *data_error21;
    QColor *col1;
    int *mark1;

    float *data2;
    bool *use2;
    QString *comment2;

    float *data3;
    QColor *col3;

    float *data4;
    bool *use4;

    float *data5;
    float *data_error5;
    float *data_error25;
    bool *use5;
    QColor *c5;
    int *lw5;
    int *ss5;
    Qt::PenStyle *ps5;

    // Custom Target Data
     QVector <QStringList*> target_data;
     QStringList target_meta;
     QVector <QStringList*> target_age;
     int target_length;

     QString error_text;
     int show_error;

     QImage *worldmap;



};

#endif // CORRELATE_H
