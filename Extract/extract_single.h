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

#ifndef EXTRACT_SINGLE_H
#define EXTRACT_SINGLE_H

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
using namespace QXlsx;
namespace Ui {
class Extract_Single;
}

class Extract_Single : public QDialog
{
    Q_OBJECT

public:
    explicit Extract_Single(QMainWindow *mainWindow,Inventory *inventory,QString str);
    ~Extract_Single();



private:
    void setupInventory();
    bool checkFilter(int i);
    void plot();
    void sort(float* data,int j,int count,int left,int right);

    void AgeModel(AMSData *ams);

private slots:
    void setup();
    void calc();
    void read();
    void save();

    void updateGraph();
    void copy(QPoint p);

    double gauss(double mu, double sigma);
    void invSelected(QModelIndex mi);

    void sortVerify();

signals:
    void selectionChanged();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::Extract_Single *ui;
    QMainWindow *mainW;
    Inventory *inv;
    Inventory *inv_main;
    QStandardItemModel *modelInventory;
    AMSData *amsdata;
    QString proxy;

    Graph* pl1;
    float *data;
    QColor *coll;
    Qt::PenStyle *lstyle;

    Resources *resources;
    /*
    std::vector<float> result_Q05_O;
    std::vector<float> result_Q50_O;
    std::vector<float> result_Q95_O;
    std::vector<float> result_age_O;
    std::vector<float> result_Q05_C;
    std::vector<float> result_Q50_C;
    std::vector<float> result_Q95_C;
    std::vector<float> result_age_C;
    */
    std::vector<float> result_Q05;
    std::vector<float> result_Q50;
    std::vector<float> result_Q95;
    std::vector<float> result_age;
    std::vector<float> result_age05;
    std::vector<float> result_age95;
    std::vector<float> result_depth;

    int data_index=0;

    void paintEvent(QPaintEvent*);


    QByteArray sp,sp_2;
    int save_OK;
    int view_only;
};

#endif // EXTRACT_SINGLE_H
