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

#ifndef GRAPHDATA_H
#define GRAPHDATA_H


#include <QGraphicsScene>
#include <QPainter>
#include <QFont>
#include <QGraphicsItem>
#include <QToolTip>
#include <math.h>

class GraphData : public QGraphicsItem
{

public:
    GraphData(QGraphicsScene *graphScene=0,float *d=0,int c=0,int r=0);
    void setSize(int x,int y);
    void setData(float *d,int c,int r);
    int getColumn();
    int getRow();
    float getData(int c,int r);
    void setError(float *d_e,int m);
    void setError2(float *d_e, int m);
    void setComment(QString *s,int m);
    void setLabel(QString *s,int m);
    void setUse(bool *u,int m);
    void setColor(QColor *c,bool m);
    void setMultiplicator(double fx,double fy);
    void setTextSize(int texts, int titels,int margins);
    void setSymbol(int size);
    void setMark(int *d_m,int m);
    int getSymbolSize();
    void addMarker(float x,float y,int m);
    void setAxisType(int x,int y);
    void calcFrame();
    void setRepeat(int x,float x1,float x2,int y,float y1,float y2);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;

    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

    double get_x_min();
    double get_x_max();
    double get_y_min();
    double get_y_max();
    void setSelected_X(int n);
    void setSelected_Y(int n);
    int getSelected_X();
    int getSelected_Y();
    void setComHide(int i);
    void setView(double x1,double x2,double y1,double y2);
    void setRect(int x1,int x2, int y1, int y2, int mode);
    void setLineWidth(int i);
    int getLineWidth();
    void setLineStyle(Qt::PenStyle sty);
    Qt::PenStyle getLineStyle();
    QColor getLineColor();
    void setLineColor(QColor c);
    void setSetColor(QColor *c,int m);
    void setSetLineColor(QColor *c,int m);
    void setSetLineWidth(int *s,int m);
    void setSetSymbolsize(int *s,int m);
    void setSetSymboltype(int *t,int m);
    void setSetLineStyle(Qt::PenStyle *s, int m);
    void setSetPoly(int *s,int m);
    void setSetArea(float* area_x,float* area_y, int mode);
    void setColorLegend(int type);
    void setColorScheme(int i);

private:
    QGraphicsScene *graph;
    int plot_Size_X;
    int plot_Size_Y;
    int textsize;
    int titelsize;
    int margin;
    double x_min,y_min,x_max,y_max;

    int selected_x,selected_y;
    int select_Rect;
    int sel_x1,sel_x2,sel_y1,sel_y2;
    double factor_x,factor_y;

    float *data;
    int column,row;
    bool *useflag;
    int usemode;

    QColor *col_dat;

    bool color;
    float *data_err;
    float *data_err2;
    QString *data_com;
    QString *data_label;
    int com_mode;
    int label_mode;
    int error;
    int error2;
    int *data_mark;
    int marker;
    int symbolsize;
    int com_hide;

    int line_width;
    QColor line_color;
    Qt::PenStyle line_style;

    float marker_x;
    float marker_y;
    int marker_mode;

    int setcolor_mode;
    int setlinecolor_mode;
    int setlinewidth_mode;
    int setsymbolsize_mode;
    int setsymboltype_mode;
    int setlinestyle_mode;
    int setpoly_mode;
    QColor *setcolor_color;
    QColor *setlinecolor_color;
    int *setsymbolsize;
    int *setsymboltype;
    int *setlinewidth;
    Qt::PenStyle *setlinestyle;
    int *setpoly;
    int setarea_mode;
    float* setarea_x;
    float* setarea_y;
    int x_axis_type,y_axis_type;

    int frame_left;
    int frame_right;
    int frame_size_x;
    int frame_top;
    int frame_bottom;
    int frame_size_y;

    int repeat_x=0;
    float repeat_x1=-3.40282e+38;
    float repeat_x2=3.40282e+38;
    int repeat_y=0;
    float repeat_y1=-3.40282e+38;
    float repeat_y2=3.40282e+38;

    int colorLegend_type;
    int colorLegend_size_x;
    int colorLegend_size_y;
    int colorScheme;


};


#endif // GRAPHDATA_H
