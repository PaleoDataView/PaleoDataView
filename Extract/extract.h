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

#ifndef EXTRACT_H
#define EXTRACT_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDebug>
#include <QMainWindow>
#include "General/inventory.h"
#include "AMS/amsdata.h"
#include <cstdlib>
#include <cmath>
#include <limits>
#include "Editor/graph.h"
#include "General/resources.h"
#include "QFileDialog"
#include <QDesktopServices>
#include "QtXlsx/src/xlsx/xlsxdocument.h"
#include <QMenu>
#include <QClipboard>
#include <QListView>
#include <QProgressDialog>
#include "General/colorscheme.h"
using namespace QXlsx;
namespace Ui {
class Extract;
}

class Extract : public QDialog
{
    Q_OBJECT

public:
    explicit Extract(QMainWindow *mainWindow,Inventory *inventory,QString str,QImage *wmap);
    ~Extract();



private:
    void setupInventory();
    void setupExtractTable();
    bool checkFilter(int i);
    void plot();
    void createmap(int first);
    QColor eval_Color(float v,float min,float max);
    void sort(float* data,int j,int count,int left,int right);
    void paintEvent(QPaintEvent*);

private slots:
    void setup();
    void calc();
    void read();
    void save();

    void updateGraph();
    void copy(QPoint p);
    void copy2(QPoint p);

    double gauss(double mu, double sigma);
    void invSelected(QModelIndex mi);
    void listSelected(QModelIndex mi);
    void selected_changed(int p,int x,int y);
    void sMoved(int v);
    void sPressed(int v);

    void sValue(double d);
    void sTab(int i);
    void sChanged(int state);
    void sChanged2(int state);
    void selectAll();
    void deselectAll();

    void sortVerify();

    void MapToATL();
    void MapToPAC();
    void MapToIND();
    void MapToMED();
    void MapToWorld();
    void setButtons();

    void AMSSummery();
signals:
    void selectionChanged();


protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Extract *ui;
    QMainWindow *mainW;
    Inventory *inv;
    Inventory *inv_main;
    QStandardItemModel *modelInventory;
    QStandardItemModel *modelList;
    AMSData *amsdata;
    QString proxy;

    // plot of data
    Graph* pl1;
    Graph* pl2;
    float *data;
    QColor *coll;
    Qt::PenStyle *lstyle;

    // map
    Graph* map;
    float* mapdata;
    QString *com;
    QString *lab;
    QColor *dotcol;
    int *dotsize;
     QImage *worldmap;

    ColorScheme* colorScheme;

    Resources *resources;

    //Data
    std::vector<float> result_min;
    std::vector<float> result_mid;
    std::vector<float> result_max;
    std::vector<float> result_skew;

    std::vector<float> result_age;
    //Meta
    QStringList result_core;
    QStringList result_species;
    std::vector<float> result_lat;
    std::vector<float> result_long;
    std::vector<float> result_wd;
    int selected_entry;

    int data_index=0;

    // color scaling properties
    float global_min;
    float global_max;

    int entries;
    int size;
    float start,inc,end;
    QByteArray sp,sp_2,sp_3,sp_4;
    int save_OK;

    AMSData *ams;
    // AMS Summary
    std::vector<float> ams_min_age;
    std::vector<float> ams_max_age;
    std::vector<float> ams_bacon_iter;

    int mapmode;

};

#endif // EXTRACT_H
