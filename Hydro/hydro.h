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

#ifndef HYDRO_H
#define HYDRO_H

#include <QDialog>
#include <QDebug>
#include <QMainWindow>
#include "General/inventory.h"

#include "hydrodatabase.h"
#include "General/colorscheme.h"

#include "Editor/graph.h"
#include <QApplication>
#include "General/resources.h"
namespace Ui {
class Hydro;
}

class Hydro : public QDialog
{
    Q_OBJECT

public:
    explicit Hydro(QMainWindow *mainWindow,Inventory *inventory,QImage *map);
    ~Hydro();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void depth_changed(int i);
    void set_focus(bool b);
    void getCompare(int plot,float x,float y);
    void up_date();
    void newCompare();
    void updateGraph();
private:
    void paintEvent(QPaintEvent*);
    void createPlot();
    void createTDPlot();
    void createSDPlot();
    void create18oPlot();
    void createDCPlot();
    void createCPlot();
    void createDCMPlot();
    void createMap(int i);
    void createMapT(int i);
    void createMapS(int i);
    void createMap18o(int i);
    void createMapDC(int i);
    void createMapC(int i);
    void createMapDCM(int i);
    void createTSPlot();

    Ui::Hydro *ui;
    QMainWindow *mainW;
    Inventory *inv;
    HydroDatabase *hydro;
    ColorScheme *colorScheme;

    Graph *map;
    float map_min;
    float map_max;


    int box_x,box_y;

    Graph *plotTS;

    Graph *plot;
    QByteArray sp,sp_2,sp_3;
    Resources *resources;
    int label_flag=1;
    float comp_latt=0;
    float comp_long=0;
    int r_col,g_col,b_col;

    QImage *mask;
    QImage *worldmap;

    float *plotdata,*mapdata,*tsdata;
    QString *mapcom,*tscom;
    QColor *mapcol,*plotcol,*tscol;
    QString *maplab;
    bool *mapuse;
    int *size;
    int *autosize;

};

#endif // HYDRO_H
