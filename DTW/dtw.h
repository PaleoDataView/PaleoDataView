
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
#ifndef DTW_H
#define DTW_H

#include <QDialog>
#include <QMainWindow>
#include <QDebug>
#include "General/inventory.h"
#include <QStandardItemModel>
#include "Editor/graph.h"
#include <QColor>
#include <QApplication>
#include "General/resources.h"
#include "AMS/amsdata.h"
#include "Dialog/attdialog.h"

namespace Ui {
class DTW;
}

class DTW : public QDialog
{
    Q_OBJECT

public:
    explicit DTW(QMainWindow *mainWindow,Inventory *i=0);
    ~DTW();
    void reject();

private:
    void paintEvent(QPaintEvent*);
    void setupTable();

    float proxy(int i,int j);
    void stepPattern(QString s);
    void addPoint(int i1,int i2);
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void redraw();
    void createAMPlot();
    void createResultPlot();
    void createCorPlot(int autosize);
    void createLines();
    void createPatPlot();
    void createDistPlot(int n,int m);
    void createCostPlot(int n,int m);
    void target_Selected(QString);
    void pattern_Selected(QString);
    void tableSelected(QModelIndex mi);
    void calcGDTW();
    void calcDTW(bool openbegin,bool openend);


private:
    Ui::DTW *ui;
    QMainWindow *mainW;
    Inventory *inv;
    Graph *cor_plot;
    Graph *am_plot;
    Graph *result_plot;
    Graph *pat_plot;
    Graph *dist_plot;
    Graph *cost_plot;
    QStandardItemModel *agemodelList;
    QStandardItemModel *modelData;
    QStandardItemModel *modelDist;
    QStandardItemModel *modelCost;
    int selected=-1;
    int select=0;

    AMSData *amdata;
    QString target="Govin";
    int source_axis=2;// 2 is O : 3 is C Isotope data
    int target_axis=3;// 2:LH 3:Govin
    QByteArray sp,sp_2;
    Resources *resources;
    bool changes;

    // DTW
    // partial
    std::vector<float> dtw_x;
    std::vector<float> dtw_y;
    std::vector<float> dtw_D;
    std::vector<float> dtw_C;
    std::vector<int> dtw_S;
    std::vector<int> dtw_i1;
    std::vector<int> dtw_i2;
    // global
    std::vector<float> dtw_DG;
    std::vector<float> dtw_CG;
    std::vector<int> dtw_SG;
    std::vector<int> dtw_i1G;
    std::vector<int> dtw_i2G;
    // step.pattern
    std::vector<int> dtw_step_npat;
    std::vector<int> dtw_step_ix;
    std::vector<int> dtw_step_iy;
    std::vector<float> dtw_step_pen;
    QString dtw_step_norm;

};

#endif // DTW_H
