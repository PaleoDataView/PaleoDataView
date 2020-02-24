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

#ifndef EDITGRAPH_GRAPH_H
#define EDITGRAPH_GRAPH_H


#include <QDialog>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include "graphgrid.h"
#include "graphdata.h"
#include "graphobject.h"
#include <QKeyEvent>
#include <QMenu>
#include <QFile>
#include <vector>



class EditGraph_Graph : public QGraphicsScene
{
    Q_OBJECT
public:
    EditGraph_Graph(QWidget *dialog=0,float *d=0,int c=0,int r=0);
    ~EditGraph_Graph();
    void setNumber(int n);

    void deleteAll();
    void setSettings(QString p,int n);
    void setSettings(QString p);
    void readSettings(int n);
    void saveSettings(int n);

    void setData(float *d,int c,int r);
    void setData(float *d,int c,int r,int n);

    void setComment(QString *s,int m);
    void setComment(QString *s,int m,int n);

    void setLabel(QString *s,int m);
    void setLabel(QString *s,int m,int n);

    void setColor(QColor *c,bool m);
    void setColor(QColor *c,bool m,int n);

    void setUse(bool *u,int m);
    void setUse(bool *u,int m,int n);

    void setSize(int plot_width,int plot_height);

    void setTitel(QString t,QString tx,QString ty);
    void setTitel(QString t,QString tx,QString ty,int n);

    void setTextSize(int texts, int titels,int margins);
    void setTextSize(int texts, int titels,int margins,int n);

    void setMultiplicator(double fx, double fy);
    void setMultiplicator(double fx, double fy,int n);



    void autoSize();
    void autoSize(int n);

    int getSelected_X();
    int getSelected_Y();
    void setSelected_X(int x);
    void setSelected_Y(int y);
    int getSelected_X(int n);
    int getSelected_Y(int n);
    void setSelected_X(int x,int n);
    void setSelected_Y(int y,int n);

    void setError(float *d_e,int m);
    void setError(float *d_e,int m,int n);

    void setError2(float *d_e,int m);
    void setError2(float *d_e,int m,int n);

    void setMark(int *d_m,int m);
    void setMark(int *d_m,int m,int n);

    void setLineWidth(int i);
    void setLineWidth(int i,int n);

    void setLineStyle(Qt::PenStyle sty);
    void setLineStyle(Qt::PenStyle sty,int n);

    void setLineColor(QColor c);
    void setLineColor(QColor c,int n);

    void setSymbol(int size);
    void setSymbol(int size,int n);
    int getSymbol();
    int getSymbol(int n);

    void addMarker(float x,float y,int m);
    void addMarker(float x,float y,int m,int n);

    void setSetColor(QColor *c,int m);
    void setSetLineColor(QColor *c,int m);
    void setSetLineWidth(int *s,int m);
    void setSetSymbolsize(int *s,int m);
    void setSetLineStyle(Qt::PenStyle *s, int m);
    void setSetColor(QColor *c,int m,int n);
    void setSetLineColor(QColor *c,int m,int n);
    void setSetLineWidth(int *s,int m,int n);
    void setSetSymbolsize(int *s,int m,int n);
    void setSetLineStyle(Qt::PenStyle *s, int m,int n);
    void setSetSymboltype(int *t,int m);
    void setSetSymboltype(int *t,int m,int n);

    void setSetPoly(int *s,int m);
    void setSetArea(float* area_x,float* area_y,int area_mode);
    void setSetPoly(int *s,int m,int n);
    void setSetArea(float* area_x,float* area_y,int area_mode,int n);

    void fixRange(bool x,bool y);
    void fixRange(bool x,bool y,int n);

    void addLine(float x1,float y1,int n1,float x2,float y2,int n2,QColor c,int w,Qt::PenStyle s);
    void setAddMode(int i);
    void addLineClear();
    void setAxisType(int x,int y);
    void setAxisType(int x,int y,int i);
    void setTicks(int tx,int ty);
    void setTicks(int tx,int ty,int i);
    void calcFrame(int i);
    int getSelectLine();

    void comHide(int i);
    void setComHide(int i, int n);
    void getBack(int n);

    void setLockAspect(int mode,int n);
    void setLockAspect(int mode);

    void setFolding(int x,int y,int n);
    void setFolding(int x,int y);


    void setBackground(QString filename,float x1,float x2,float y1,float y2,int mode,int n);
    void setBackground(QString filename,float x1,float x2,float y1,float y2,int mode);
    void setBackground(QImage *i,float x1,float x2,float y1,float y2,int mode,int n);
    void setBackground(QImage *i,float x1,float x2,float y1,float y2,int mode);
    void setBackground2(QImage *i,float x1,float x2,float y1,float y2,int mode,int n);
    void setBackground2(QImage *i,float x1,float x2,float y1,float y2,int mode);
    void setRepeat(int x,float x1,float x2,int y,float y1, float y2,int n);
    void setRepeat(int x,float x1,float x2,int y,float y1, float y2);
    void setLimit(float x1,float x2, float y1,float y2,int n);
    void setLimit(float x1, float x2, float y1, float y2);
    void setView(int x1,int x2,int y1,int y2,int n);
    void setView(int x1,int x2,int y1,int y2);

    void setColorLegend(int type,QString t,float min,float max);
    void setColorLegend(int type,QString t,float min,float max,int i);

    void setColorScheme(int n);
    void setColorScheme(int n,int i);

    void setAutosize(int* as,int mode);
    void setAutosize(int* as,int mode,int n);

signals:
    void selection(int plot,QList<int> listx,QList<int> listy);
    void selected(int plot,int x,int y);
    void click(int plot,float x,float y);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent1) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent2) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent3) Q_DECL_OVERRIDE;
    void wheelEvent(QGraphicsSceneWheelEvent *mouseEvent4) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent5) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
private:

    QWidget *diag;




    int plot_Size_X;
    int plot_Size_Y;


    int moveFlag;

    int key_flag;
    int frame_x1,frame_x2,frame_y1,frame_y2;

    QList<int> select_x;
    QList<int> select_y;
    int select_plot;

//******* Individual Plot Parameter *********************
    std::vector<double> x_min;
    std::vector<double> y_min;
    std::vector<double> y_max;
    std::vector<double> x_max;

    std::vector<GraphGrid*> ggrid;
    std::vector<GraphData*> gdata;
    GraphObject* gobject;

    std::vector<QString*> titel;
    std::vector<QString*> titel_x;
    std::vector<QString*> titel_y;

    std::vector<QString*> path;

    std::vector<int> textsize;
    std::vector<int> titelsize;
    std::vector<int> margin;

    std::vector<double> factor_x;
    std::vector<double> factor_y;

    std::vector<bool> fix_x;
    std::vector<bool> fix_y;

    std::vector<float*> data;
    std::vector<int> column;
    std::vector<int> row;

    std::vector<bool*> useflag;
    std::vector<int> usemode;

    std::vector<QString*> dat_com;
    std::vector<int> com_mode;
    std::vector<int> com_hide;

    std::vector<QString*> dat_label;
    std::vector<int> label_mode;

    std::vector<QColor*> col_dat;
    std::vector<bool> color;

    std::vector<float*> data_err;
    std::vector<float*> data_err2;
    std::vector<int> error;
    std::vector<int> error2;

    std::vector<int*> data_mark;
    std::vector<int> marker;

    std::vector<int> symbol_size;
    std::vector<int> line_width;
    std::vector<Qt::PenStyle> line_style;
    std::vector<QColor> line_color;

    std::vector<float> marker_x;
    std::vector<float> marker_y;
    std::vector<int> marker_mode;

    std::vector<int> setcolor_mode;
    std::vector<int> setlinecolor_mode;
    std::vector<int> setlinewidth_mode;
    std::vector<int> setsymbolsize_mode;
    std::vector<int> setsymboltype_mode;
    std::vector<int> setlinestyle_mode;
    std::vector<QColor*> setcolor_color;
    std::vector<QColor*> setlinecolor_color;
    std::vector<int*> setsymbolsize;
    std::vector<int*> setsymboltype;
    std::vector<int*> setlinewidth;
    std::vector<Qt::PenStyle*> setlinestyle;

    std::vector<int> setpoly_mode;
    std::vector<int*> setpoly;
    std::vector<int> setarea_mode;
    std::vector<float*> setarea_x;
    std::vector<float*> setarea_y;

    std::vector<int> x_axis_type;
    std::vector<int> y_axis_type;

    std::vector<int> ticks_x;
    std::vector<int> ticks_y;

    std::vector<QString*> background_filename;
    std::vector<QImage*> background_fileimage;
    std::vector<QImage*> background_image;
    std::vector<float> background_x1;
    std::vector<float> background_x2;
    std::vector<float> background_y1;
    std::vector<float> background_y2;
    std::vector<int> background;

    std::vector<int> repeat_x;
    std::vector<float> repeat_x1;
    std::vector<float> repeat_x2;
    std::vector<int> repeat_y;
    std::vector<float> repeat_y1;
    std::vector<float> repeat_y2;

    std::vector<int> fold_x;
    std::vector<int> fold_y;

    std::vector<float> limit_x1;
    std::vector<float> limit_x2;
    std::vector<float> limit_y1;
    std::vector<float> limit_y2;
    std::vector<int> lock_aspect;

    std::vector<int> colorLegend_type;
    std::vector<int> colorLegend_size_x;
    std::vector<int> colorLegend_size_y;
    std::vector<QString*> colorLegend_titel;
    std::vector<float> colorLegend_min;
    std::vector<float> colorLegend_max;

    std::vector<int> colorScheme;

    std::vector<int*> autosize;
    std::vector<int> autosize_mode;

//**********************************
    int number;
    int focus;
    int frame_left;
    int frame_right;
    int frame_size_x;
    int frame_top;
    int frame_bottom;
    int frame_size_y;


    QStringList *header;
    float *err;

    GraphObject* ggg;
    GraphGrid* gg;
    GraphData* gd;

};

#endif // EDITGRAPH_GRAPH_H
