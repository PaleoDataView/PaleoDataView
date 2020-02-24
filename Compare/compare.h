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

#ifndef COMPARE_H
#define COMPARE_H

#include <QDialog>
#include <QDebug>
#include <QMainWindow>
#include "General/inventory.h"
#include "Hydro/hydrodatabase.h"
#include <QStandardItemModel>
#include <QColorDialog>
#include "Editor/graph.h"
#include "AMS/amsdata.h"
#include <QApplication>
#include "General/resources.h"
#include <QMenu>
#include <QClipboard>
#include "General/colorscheme.h"
namespace Ui {
class Compare;
}

class Compare : public QDialog
{
    Q_OBJECT

public:
    explicit Compare(QMainWindow *mainWindow,Inventory *inventory,QImage *image,QString str);
    ~Compare();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    void setupIsotope();
    void setInfo();
    void paintEvent(QPaintEvent*);

    void createMultiPlot();
    void createMap(int first);

private slots:
    void applyIsotope(int n);
    void isotopeSelected(QModelIndex mi);
    void updateList();
    void selectPlot();
    void colorSelect();
    void autoColor();
    void updatePlot();
    void selection_changed(int plot,QList<int> x,QList<int> y);
    void selected_changed(int plot,int x,int y);
    void updateGraph();
    void copy(QPoint p);

private:
    Ui::Compare *ui;
    QMainWindow *mainW;
    Inventory *inv_main;
    Inventory *inv;
    HydroDatabase *hydro;


    ColorScheme *colorScheme;

    QImage *mapimage;

    QStandardItemModel *modelIsotopeList;
    int selected=0;

    int *color_red;
    int *color_green;
    int *color_blue;
    int *plot;

    Graph *multiplot;
    AMSData *amsdata;
    QByteArray sp,sp_2,sp_3;
    Resources *resources;
    Graph *map;
    int label_flag=1;

    float *data1;
    bool *data_use1;
    QColor *col1;
    QString *com1;
    float *data2;
    QString *com2;
    QString *lab2;
    QColor *col2;
    int *size;

    int oldcore;
    QString proxy;

    int depth;
    int age;
};

#endif // COMPARE_H
