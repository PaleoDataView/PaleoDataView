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

#ifndef GRAPHGRID_H
#define GRAPHGRID_H


#include <QGraphicsScene>
#include <QPainter>
#include <QFont>
#include <QGraphicsItem>
#include <math.h>
#include "General/colorscheme.h"


class GraphGrid : public QGraphicsItem
{
public:
    GraphGrid(QGraphicsScene *graphScene=0,float *d=0,int c=0,int r=0);
    ~GraphGrid();
    void setSize(int x,int y);
    void setData(float *d,int c,int r);
    void setTitel(QString t,QString tx,QString ty);
    QString getTitel();
    QString getTitelX();
    QString getTitelY();
    void setMultiplicator(double fx,double fy);
    void setTicks(int tx,int ty);
    void setView(double x1,double x2,double y1,double y2);
    void setTextSize(int texts, int titels,int margins);
    int getTextSize();
    int getTitelSize();
    int getMargin();
    void setAxisType(int x,int y);
    void calcFrame();
    void setBackground(QString filename, float x1,float x2,float y1,float y2,int mode);
    void setBackground(QImage *i, float x1,float x2,float y1,float y2,int mode);
    void setBackground2(QImage *i, float x1,float x2,float y1,float y2,int mode);
    void setRepeat(int x,float x1,float x2,int y,float y1,float y2);
    void setColorLegend(int type,QString t,float min,float max);
    void setColorScheme(int i);
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

private:
    QGraphicsScene *graph;
    int plot_Size_X;
    int plot_Size_Y;
    QString titel;
    QString titel_x;
    QString titel_y;
    int textsize;
    int titelsize;
    int margin;
    double x_min,y_min,x_max,y_max;
    int ticks_x,ticks_y;
    int grid_flag;

    double factor_x,factor_y;

    float *data;
    int row,column;
    int x_axis_type;
    int y_axis_type;

    int background;
    QString background_filename;
    float background_x1;
    float background_x2;
    float background_y1;
    float background_y2;
    QImage *background_image;
    QImage *background_image2;

    int repeat_x=0;
    float repeat_x1=-3.40282e+38;
    float repeat_x2=3.40282e+38;
    int repeat_y=0;
    float repeat_y1=-3.40282e+38;
    float repeat_y2=3.40282e+38;

    int frame_left;
    int frame_right;
    int frame_size_x;
    int frame_top;
    int frame_bottom;
    int frame_size_y;

    int colorLegend_type=0;
    float colorLegend_min;
    float colorLegend_max;
    QString colorLegend_titel;
    int colorLegend_size_x=0;
    int colorLegend_size_y=0;
    int colorScheme;

    ColorScheme* colorSchemeClass;


};

#endif // GRAPHGRID_H
