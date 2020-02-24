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

#include "graphdata.h"
#include <QDebug>
GraphData::GraphData(QGraphicsScene *graphScene,float *d,int c,int r)
    : graph(graphScene),data(d),column(c),row(r)
{
    plot_Size_X=100;
    plot_Size_Y=100;
    textsize=10;
    titelsize=1;
    margin=8;
    x_min=-1;
    x_max=1;
    y_min=-1;
    y_max=1;
    x_axis_type=0;
    y_axis_type=0;
    usemode=0;
    error=0;
    error2=0;
    selected_x=-1;
    selected_y=-1;
    select_Rect=0;
    sel_x1=0;
    sel_x2=0;
    sel_y1=0;
    sel_y2=0;
    factor_x=1;
    factor_y=1;
    color=false;
    symbolsize=5;
    com_mode=0;
    com_hide=0;
    marker=0;
    line_width=1;
    line_color=Qt::black;
    line_style=Qt::DashDotLine;
    marker_x=NAN;
    marker_y=NAN;
    marker_mode=0;
    setcolor_mode=0;
    setlinecolor_mode=0;
    setlinewidth_mode=0;
    setsymbolsize_mode=0;
    setsymboltype_mode=0;
    setlinestyle_mode=0;
    setpoly_mode=0;
    setarea_mode=0;
    x_axis_type=0;
    y_axis_type=0;
    label_mode=0;
    repeat_x=0;
    repeat_x1=-1e38;
    repeat_x2=1e38;
    repeat_y=0;
    repeat_y1=-1e38;
    repeat_y2=1e38;
    colorLegend_type=0;
    colorLegend_size_x=0;
    colorLegend_size_y=0;
    colorScheme=-1;
}

// Draw the Data
void GraphData::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPen pen;
    int nothing=1; // nothing to plot?
   //qDebug()<<QString::number(column);
    if (column==0){
        QFont font_titel("Arial", textsize, QFont::Bold);
        font_titel.setPixelSize(textsize);
        QFontMetrics fm_titel(font_titel);
        int str_len=fm_titel.width("No Available Data");
        painter->setFont(font_titel);
        painter->drawText(-str_len/2,0,"No Available Data");
    }
    calcFrame();
    // for the repeat in x and y
    for (int xr=-repeat_x;xr<=repeat_x;xr++){
        for (int yr=-repeat_y;yr<=repeat_y;yr++){
            // draw data
            for (int j=0;j<column;j=j+2){
                int symbol_x,symbol_y;
                // select symbolsize
                if (setarea_mode==0){
                    if (setsymbolsize_mode==0){
                        symbol_x=symbolsize;
                        symbol_y=symbolsize;
                    } else {
                        symbol_x=setsymbolsize[j];
                        symbol_y=setsymbolsize[j];
                    }
                } else {
                    if (std::isnan(setarea_x[j])==false && std::isnan(setarea_y[j])==false){
                        symbol_x = (int) ( (double) (frame_size_x) * (double) (((setarea_x[j]*factor_x) ) / (x_max - x_min))+0.5);
                        symbol_y = (int) ( (double) (frame_size_y) * (double) (((setarea_y[j]*factor_y)) / (y_max - y_min))+0.5);
                    } else {
                        if (setsymbolsize_mode==0){
                            symbol_x=symbolsize;
                            symbol_y=symbolsize;
                        } else {
                            symbol_x=setsymbolsize[j];
                            symbol_y=setsymbolsize[j];
                        }
                    }
                }
                if (setpoly_mode==0){
                    // draw the lines between points
                    int oldx = NAN, oldy = NAN, oldisnan=true;
                    double linex = NAN, liney = NAN, oldlinex = NAN, oldliney = NAN;
                    for (int i=0;i<row;i++){
                        // get x and y cooridinates
                       int x = (int) (frame_left + (double) (frame_size_x) * (double) ((((data[i+j*row]+xr*(repeat_x2-repeat_x1))*factor_x) - x_min) / (x_max - x_min)));
                       int y = (int) (frame_top + (double) (frame_size_y) * (double) ((y_max - ((data[i+(j+1)*row]+yr*(repeat_y2-repeat_y1))*factor_y)) / (y_max - y_min)));
                       if (std::isnan(data[i+j*row])==false && std::isnan(data[i+(j+1)*row])==false){

                               if (i > 0) {
                                       linex = x;
                                       liney = y;
                                       oldlinex = oldx;
                                       oldliney = oldy;
                                       double slope = 0;
                                       if (linex - oldlinex != 0 && liney - oldliney != 0) {
                                           slope = (liney - oldliney) / (linex - oldlinex);
                                       } else {
                                           if (linex - oldlinex == 0) {
                                               slope = 999999999;
                                           }
                                           if (liney - oldliney == 0) {
                                               slope = 0;
                                           }
                                       }
                                       double offset = (liney - (linex * slope));

                                       int c = 0;
                                       while ((((linex < frame_left)
                                               || (liney < frame_top)
                                               || (linex > frame_right)
                                               || (liney > frame_bottom))
                                               || ((oldlinex < frame_left)
                                               || (oldliney < frame_top)
                                               || (oldlinex > frame_right)
                                               || (oldliney > frame_bottom)))
                                               && c < 3) {
                                           if (linex < frame_left) {
                                               linex = frame_left + 1;
                                               liney = ((linex * slope) + offset);
                                           }
                                           if (linex >  frame_right) {
                                               linex = frame_right - 1;
                                               liney = ((linex * slope) + offset);
                                           }
                                           if (liney < frame_top) {
                                               liney = frame_top + 1;
                                               linex = ((liney - offset) / slope);
                                           }
                                           if (liney > frame_bottom) {
                                               liney = frame_bottom - 1;
                                               linex = ((liney - offset) / slope);
                                           }
                                           if (oldlinex < frame_left) {
                                               oldlinex = frame_left + 1;
                                               oldliney = ((oldlinex * slope) + offset);
                                           }
                                           if (oldlinex > frame_right) {
                                               oldlinex = frame_right - 1;
                                               oldliney = ((oldlinex * slope) + offset);
                                           }
                                           if (oldliney < frame_top) {
                                               oldliney = frame_top + 1;
                                               oldlinex = ((oldliney - offset) / slope);
                                           }
                                           if (oldliney > frame_bottom) {
                                               oldliney = frame_bottom - 1;
                                               oldlinex = ((oldliney - offset) / slope);
                                           }
                                           c++;
                                       }
                                       if (linex != oldlinex || liney != oldliney) {
                                           if ((linex >= frame_left)
                                                   && (liney >= frame_top)
                                                   && (linex <= frame_right)
                                                   && (liney <= frame_bottom)
                                                   && (oldlinex >= frame_left)
                                                   && (oldliney >= frame_top)
                                                   && (oldlinex <= frame_right)
                                                   && (oldliney <= frame_bottom)) {


                                               if (setlinewidth_mode==0){
                                                   pen.setWidth(line_width);
                                               } else {
                                                   pen.setWidth(setlinewidth[j]);
                                               }

                                               if (setlinecolor_mode==0){
                                                   pen.setColor(line_color);
                                               } else {
                                                   pen.setColor(setlinecolor_color[j]);
                                               }
                                               if (setlinestyle_mode==0){
                                                   pen.setStyle(line_style);
                                               } else {
                                                   pen.setStyle(setlinestyle[j]);
                                               }
                                               painter->setPen(pen);
                                               painter->setRenderHint(QPainter::Antialiasing,true);

                                               if (usemode==0) {
                                                    if (oldisnan==false) {
                                                        painter->drawLine((int) linex, (int) liney, (int) oldlinex, (int) oldliney);
                                                    }
                                               }
                                               if (usemode>0) {
                                                    if (oldisnan==false && useflag[i+j*row]==true) {
                                                        painter->drawLine((int) linex, (int) liney, (int) oldlinex, (int) oldliney);
                                                    }
                                               }
                                               painter->setRenderHint(QPainter::Antialiasing,false);
                                               pen.setStyle(Qt::SolidLine);
                                               painter->setPen(pen);
                                           }
                                       }
                               }
                               if (usemode==0){
                                   oldisnan=false;
                                   oldx = x;
                                   oldy = y;
                               }
                               if (usemode>0){
                                   if (useflag[i+j*row]==true){
                                       oldisnan=false;
                                       oldx = x;
                                       oldy = y;
                                   }
                               }

                       }
                    }
                } else {// end of lines
                // draw as polygon with filling
                    // draw the lines between points
                    int oldx = NAN, oldy = NAN, oldisnan=true;
                    double linex = NAN, liney = NAN, oldlinex = NAN, oldliney = NAN;
                    QPolygon polygon;
                    for (int i=0;i<row;i++){
                        // get x and y cooridinates
                       int x = (int) (frame_left + (double) (frame_size_x) * (double) ((((data[i+j*row]+xr*(repeat_x2-repeat_x1))*factor_x) - x_min) / (x_max - x_min)));
                       int y = (int) (frame_top + (double) (frame_size_y) * (double) ((y_max - ((data[i+(j+1)*row]+yr*(repeat_y2-repeat_y1))*factor_y)) / (y_max - y_min)));
                       if (std::isnan(data[i+j*row])==false && std::isnan(data[i+(j+1)*row])==false){
                               nothing=0;
                               if (i > 0) {
                                       linex = x;
                                       liney = y;
                                       oldlinex = oldx;
                                       oldliney = oldy;
                                       double slope = 0;
                                       if (linex - oldlinex != 0 && liney - oldliney != 0) {
                                           slope = (liney - oldliney) / (linex - oldlinex);
                                       } else {
                                           if (linex - oldlinex == 0) {
                                               slope = 999999999;
                                           }
                                           if (liney - oldliney == 0) {
                                               slope = 0;
                                           }
                                       }
                                       double offset = (liney - (linex * slope));

                                       int c = 0;
                                       while ((((linex < frame_left)
                                               || (liney < frame_top)
                                               || (linex > frame_right)
                                               || (liney > frame_bottom))
                                               || ((oldlinex < frame_left)
                                               || (oldliney < frame_top)
                                               || (oldlinex > frame_right)
                                               || (oldliney > frame_bottom)))
                                               && c < 3) {
                                           if (linex < frame_left) {
                                               linex = frame_left + 1;
                                               liney = ((linex * slope) + offset);
                                           }
                                           if (linex >  frame_right) {
                                               linex = frame_right - 1;
                                               liney = ((linex * slope) + offset);
                                           }
                                           if (liney < frame_top) {
                                               liney = frame_top + 1;
                                               linex = ((liney - offset) / slope);
                                           }
                                           if (liney > frame_bottom) {
                                               liney = frame_bottom - 1;
                                               linex = ((liney - offset) / slope);
                                           }
                                           if (oldlinex < frame_left) {
                                               oldlinex = frame_left + 1;
                                               oldliney = ((oldlinex * slope) + offset);
                                           }
                                           if (oldlinex > frame_right) {
                                               oldlinex = frame_right - 1;
                                               oldliney = ((oldlinex * slope) + offset);
                                           }
                                           if (oldliney < frame_top) {
                                               oldliney = frame_top + 1;
                                               oldlinex = ((oldliney - offset) / slope);
                                           }
                                           if (oldliney > frame_bottom) {
                                               oldliney = frame_bottom - 1;
                                               oldlinex = ((oldliney - offset) / slope);
                                           }
                                           c++;
                                       }
                                       if (linex != oldlinex || liney != oldliney) {
                                           if ((linex >= frame_left)
                                                   && (liney >= frame_top)
                                                   && (linex <= frame_right)
                                                   && (liney <= frame_bottom)
                                                   && (oldlinex >= frame_left)
                                                   && (oldliney >= frame_top)
                                                   && (oldlinex <= frame_right)
                                                   && (oldliney <= frame_bottom)) {

                                               //QPen pen;
                                               if (setlinewidth_mode==0){
                                                   pen.setWidth(line_width);
                                               } else {
                                                   pen.setWidth(setlinewidth[j]);
                                               }

                                               if (setlinecolor_mode==0){
                                                   pen.setColor(line_color);
                                               } else {
                                                   pen.setColor(setlinecolor_color[j]);
                                               }
                                               if (setlinestyle_mode==0){
                                                   pen.setStyle(line_style);
                                               } else {
                                                   pen.setStyle(setlinestyle[j]);
                                               }
                                               painter->setPen(pen);
                                               painter->setRenderHint(QPainter::Antialiasing,true);

                                               if (usemode==0) {
                                                    if (oldisnan==false) {
                                                        if (setpoly[j]==0){
                                                            painter->drawLine((int) linex, (int) liney, (int) oldlinex, (int) oldliney);
                                                        }else{
                                                            // add to polygon
                                                            polygon.append(QPoint(oldlinex,oldliney));
                                                            polygon.append(QPoint(linex,liney));
                                                        }
                                                    }
                                               }
                                               if (usemode>0) {
                                                    if (oldisnan==false && useflag[i+j*row]==true) {
                                                        if (setpoly[j]==0){
                                                            painter->drawLine((int) linex, (int) liney, (int) oldlinex, (int) oldliney);
                                                        }else{
                                                            // add to polygon
                                                            polygon.append(QPoint(oldlinex,oldliney));
                                                            polygon.append(QPoint(linex,liney));
                                                        }
                                                    }
                                               }
                                               painter->setRenderHint(QPainter::Antialiasing,false);
                                               pen.setStyle(Qt::SolidLine);
                                               painter->setPen(pen);
                                           }
                                       }
                               }
                               if (usemode==0){
                                   oldisnan=false;
                                   oldx = x;
                                   oldy = y;
                               }
                               if (usemode>0){
                                   if (useflag[i+j*row]==true){
                                       oldisnan=false;
                                       oldx = x;
                                       oldy = y;
                                   }
                               }

                       }
                    }

                    QPainterPath tmppath;
                    tmppath.addPolygon(polygon);

                    QColor c;
                    if (setcolor_mode==0){
                        c.setRgb(0,128,255,128);
                        painter->setBrush(c);
                    } else {
                        c.setRgb(setcolor_color[j].red(),setcolor_color[j].green(),setcolor_color[j].blue(),setcolor_color[j].alpha());
                        painter->setBrush(c);
                    }
                    painter->fillPath(tmppath,c);

                    //QPen pen;
                    pen.setColor(Qt::black);
                    pen.setWidth(2);
                    painter->setPen(pen);
                    painter->drawPath(tmppath);

                }
                // Draw the Points
                for (int i=0;i<row;i++){
                    // get x and y cooridinates
                   int x = (int) (frame_left + (double) (frame_size_x) * (double) ((((data[i+j*row]+xr*(repeat_x2-repeat_x1))*factor_x) - x_min) / (x_max - x_min)));
                   int y = (int) (frame_top + (double) (frame_size_y) * (double) ((y_max - ((data[i+(j+1)*row]+yr*(repeat_y2-repeat_y1))*factor_y)) / (y_max - y_min)));


                   if (std::isnan(data[i+j*row])==false && std::isnan(data[i+(j+1)*row])==false){

                       nothing=0;

                       if (x+symbol_x>=frame_left && x-symbol_x<=frame_right && y-symbol_y<=frame_bottom && y+symbol_y>=frame_top){
                           //QPen pen;



                           pen.setColor(Qt::black);
                           pen.setWidth(1);
                           painter->setPen(pen);

                           // draw error cross
                           if (error!=4){
                           if (error>0 && error2==0){

                               int x_err = (int) ( (double) (frame_size_x) * (double) (((data_err[i+j*row]*factor_x) ) / (x_max - x_min)));
                               int y_err = (int) ( (double) (frame_size_y) * (double) (((data_err[i+(j+1)*row]*factor_y)) / (y_max - y_min)));
                               // draw y range
                               if (std::isnan(data_err[i+(j+1)*row])==false && error>1){
                                   int y_err_min=y-y_err;
                                   int y_err_max=y+y_err;
                                   int x_err_min=x-symbol_x-1;
                                   int x_err_max=x+symbol_y+1;
                                   if (y_err_min<frame_top) y_err_min=frame_top;
                                   if (y_err_max>frame_bottom) y_err_max=frame_bottom;
                                   if (x_err_min<frame_left) x_err_min=frame_left;
                                   if (x_err_max>frame_right) x_err_max=frame_right;

                                   if (usemode==0 || usemode==1) {
                                       painter->drawLine(x,y_err_min,x,y_err_max);
                                       painter->drawLine(x_err_min,y_err_min,x_err_max,y_err_min);
                                       painter->drawLine(x_err_min,y_err_max,x_err_max,y_err_max);
                                   }
                                   if (usemode==2){
                                       if (useflag[i+j*row]==true) {
                                           painter->drawLine(x,y_err_min,x,y_err_max);
                                           painter->drawLine(x_err_min,y_err_min,x_err_max,y_err_min);
                                           painter->drawLine(x_err_min,y_err_max,x_err_max,y_err_max);
                                       }
                                   }
                               }
                               // draw x range
                               if (std::isnan(data_err[i+j*row])==false && error!=2){

                                   int y_err_min=y-symbol_x-1;
                                   int y_err_max=y+symbol_y+1;
                                   int x_err_min=x-x_err;
                                   int x_err_max=x+x_err;
                                   if (y_err_min<frame_top) y_err_min=frame_top;
                                   if (y_err_max>frame_bottom) y_err_max=frame_bottom;
                                   if (x_err_min<frame_left) x_err_min=frame_left;
                                   if (x_err_max>frame_right) x_err_max=frame_right;
                                   if (usemode==0 || usemode==1) {
                                       painter->drawLine(x_err_min,y,x_err_max,y);
                                       painter->drawLine(x_err_min,y_err_min,x_err_min,y_err_max);
                                       painter->drawLine(x_err_max,y_err_min,x_err_max,y_err_max);
                                   }
                                   if (usemode==2){
                                       if (useflag[i+j*row]==true) {
                                           painter->drawLine(x_err_min,y,x_err_max,y);
                                           painter->drawLine(x_err_min,y_err_min,x_err_min,y_err_max);
                                           painter->drawLine(x_err_max,y_err_min,x_err_max,y_err_max);
                                       }
                                   }

                               }
                           }
                           if (error2>0){

                               int x_err = (int) ( (double) (frame_size_x) * (double) (((data_err[i+j*row]*factor_x) ) / (x_max - x_min)));
                               int y_err = (int) ( (double) (frame_size_y) * (double) (((data_err[i+(j+1)*row]*factor_y)) / (y_max - y_min)));
                               int x_err2 = (int) ( (double) (frame_size_x) * (double) (((data_err2[i+j*row]*factor_x) ) / (x_max - x_min)));
                               int y_err2 = (int) ( (double) (frame_size_y) * (double) (((data_err2[i+(j+1)*row]*factor_y)) / (y_max - y_min)));
                               // draw y range
                               if (std::isnan(data_err[i+(j+1)*row])==false && error>1){
                                   int y_err_min=y-y_err2;
                                   int y_err_max=y+y_err;
                                   int x_err_min=x-symbol_x-1;
                                   int x_err_max=x+symbol_y+1;
                                   if (y_err_min<frame_top) y_err_min=frame_top;
                                   if (y_err_max>frame_bottom) y_err_max=frame_bottom;
                                   if (x_err_min<frame_left) x_err_min=frame_left;
                                   if (x_err_max>frame_right) x_err_max=frame_right;

                                   if (usemode==0 || usemode==1) {
                                       painter->drawLine(x,y_err_min,x,y_err_max);
                                       painter->drawLine(x_err_min,y_err_min,x_err_max,y_err_min);
                                       painter->drawLine(x_err_min,y_err_max,x_err_max,y_err_max);
                                   }
                                   if (usemode==2){
                                       if (useflag[i+j*row]==true) {
                                           painter->drawLine(x,y_err_min,x,y_err_max);
                                           painter->drawLine(x_err_min,y_err_min,x_err_max,y_err_min);
                                           painter->drawLine(x_err_min,y_err_max,x_err_max,y_err_max);
                                       }
                                   }
                               }
                               // draw x range
                               if (std::isnan(data_err[i+j*row])==false && error!=2){

                                   int y_err_min=y-symbol_x-1;
                                   int y_err_max=y+symbol_y+1;
                                   int x_err_min=x-x_err;
                                   int x_err_max=x+x_err2;
                                   if (y_err_min<frame_top) y_err_min=frame_top;
                                   if (y_err_max>frame_bottom) y_err_max=frame_bottom;
                                   if (x_err_min<frame_left) x_err_min=frame_left;
                                   if (x_err_max>frame_right) x_err_max=frame_right;
                                   if (usemode==0 || usemode==1) {
                                       painter->drawLine(x_err_min,y,x_err_max,y);
                                       painter->drawLine(x_err_min,y_err_min,x_err_min,y_err_max);
                                       painter->drawLine(x_err_max,y_err_min,x_err_max,y_err_max);
                                   }
                                   if (usemode==2){
                                       if (useflag[i+j*row]==true) {
                                           painter->drawLine(x_err_min,y,x_err_max,y);
                                           painter->drawLine(x_err_min,y_err_min,x_err_min,y_err_max);
                                           painter->drawLine(x_err_max,y_err_min,x_err_max,y_err_max);
                                       }
                                   }

                               }
                           }
                           }//if error !=4 (allows hide error)

                           // draw Datapoints
                           if (color==false){
                               pen.setColor(Qt::black);
                               pen.setWidth(1);
                               if (setcolor_mode==0) {
                                   painter->setBrush(QColor(Qt::blue));

                               }else{
                                   painter->setBrush(setcolor_color[j]);
                               }
                           } else {
                               pen.setColor(col_dat[i+(j)*row]);
                               painter->setBrush(col_dat[i+(j+1)*row]);
                           }

                           pen.setWidth(1);

                           // Check for points crossing borders
                           int ymin=y-symbol_y;
                           int ymax=y+symbol_y;
                           int xmin=x-symbol_x;
                           int xmax=x+symbol_x;
                           if (ymin<frame_top) ymin=frame_top;
                           if (ymax>frame_bottom) ymax=frame_bottom;
                           if (xmin<frame_left)xmin=frame_left;
                           if (xmax>frame_right) xmax=frame_right;

                           if (selected_y==i && selected_x==j) {
                               pen.setColor(Qt::red);//painter->setBrush(QColor(Qt::red));
                               pen.setWidth(2);
                           }
                           painter->setPen(pen);
                           if (usemode==0) {
                               if (setsymboltype_mode==0){

                                   painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                               }else{
                                   if (setsymboltype[j]==0) painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                   if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                               }
                               if (selected_y==i && selected_x==j) {
                                   if (setsymboltype_mode==0){
                                       painter->drawRect(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                   }else{
                                       if (setsymboltype[j]==0) painter->drawRect(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                       if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                   }

                               }
                               // draw markers
                               if (marker>0){
                                   if (data_mark[i+j*row]>0 || data_mark[i+(j+1)*row]>0){
                                       painter->setBrush(Qt::NoBrush);
                                       pen.setColor(Qt::red);
                                       pen.setWidth(2);
                                       painter->setPen(pen);

                                       painter->drawRect(QRect(xmin-3,ymin-3,xmax-xmin+7,ymax-ymin+7));// draw standart Marker
                                       if (data_mark[i+j*row]==2 || data_mark[i+(j+1)*row]==2){
                                           if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                                       }
                                       if (data_mark[i+j*row]==3 || data_mark[i+(j+1)*row]==3){
                                                if (y<=frame_bottom && y>=-frame_top) painter->drawLine(frame_left,y,frame_right,y);
                                       }
                                       if (data_mark[i+j*row]==4 || data_mark[i+(j+1)*row]==4){
                                            if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                                            if (y<=frame_bottom && y>=-frame_top) painter->drawLine(frame_left,y,frame_right,y);
                                       }
                                   }
                               }
                               pen.setColor(Qt::black);
                               pen.setWidth(1);
                               painter->setPen(pen);

                               // Label
                               if (label_mode==1&&com_hide==0) {
                                   // put Text Right or Left of Point
                                   QFont font_text_x("Monospace", textsize,QFont::Bold);
                                   font_text_x.setPixelSize(textsize);
                                   font_text_x.setFamily("Courier");
                                   font_text_x.setStyleHint(QFont::Monospace);
                                   painter->setFont(font_text_x);
                                   QFontMetrics fm_text_x(font_text_x);
                                   int str_len=fm_text_x.width(data_label[i+j*row]);

                                   // Text gegebenenflalls kürzen
                                   int str_spc=0;
                                   if (x<0){
                                       str_spc=(frame_right)-(x+symbol_x*3);
                                   } else {
                                       str_spc=(x-symbol_x*3)-(frame_left);
                                   }
                                   QString str=data_label[i+j*row];
                                   while(str_len>=str_spc && str.length()>0){
                                       str.chop(1);
                                       str_len=fm_text_x.width(str+"...");
                                   }
                                   if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                   if (x+symbol_x*3+str_len<frame_right && str_len>0 && x<=0) {
                                       painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                       painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                       painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                   }
                                   if (x-symbol_x*3-str_len>frame_left && str_len>0 && x>0) {
                                       painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                       painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                       painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                   }
                               }
                               if (label_mode==2&&com_hide==0) {
                                   // put Text Right  of Point
                                   QFont font_text_x("Monospace", textsize,QFont::Bold);
                                   font_text_x.setPixelSize(textsize);
                                   font_text_x.setFamily("Courier");
                                   font_text_x.setStyleHint(QFont::Monospace);
                                   painter->setFont(font_text_x);
                                   QFontMetrics fm_text_x(font_text_x);
                                   int str_len=fm_text_x.width(data_label[i+j*row]);

                                   // Text gegebenenflalls kürzen
                                   int str_spc=0;
                                   if (x<0){
                                       str_spc=(frame_right)-(x+symbol_x*3);
                                   } else {
                                       str_spc=(x-symbol_x*3)-(frame_left);
                                   }
                                   QString str=data_label[i+j*row];
                                   while(str_len>=str_spc && str.length()>0){
                                       str.chop(1);
                                       str_len=fm_text_x.width(str+"...");
                                   }
                                   if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                   if (x+symbol_x*3+str_len<frame_right && str_len>0 ) {
                                       painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                       painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                       painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                   }

                               }
                               if (label_mode==3&&com_hide==0) {
                                   // put Text  Left of Point
                                   QFont font_text_x("Monospace", textsize,QFont::Bold);
                                   font_text_x.setPixelSize(textsize);
                                   font_text_x.setFamily("Courier");
                                   font_text_x.setStyleHint(QFont::Monospace);
                                   painter->setFont(font_text_x);
                                   QFontMetrics fm_text_x(font_text_x);
                                   int str_len=fm_text_x.width(data_label[i+j*row]);

                                   // Text gegebenenflalls kürzen
                                   int str_spc=0;
                                   if (x<0){
                                       str_spc=(frame_right)-(x+symbol_x*3);
                                   } else {
                                       str_spc=(x-symbol_x*3)-(frame_left);
                                   }
                                   QString str=data_label[i+j*row];
                                   while(str_len>=str_spc && str.length()>0){
                                       str.chop(1);
                                       str_len=fm_text_x.width(str+"...");
                                   }
                                   if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                   if (x-symbol_x*3-str_len>frame_left && str_len>0 ) {
                                       painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                       painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                       painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                   }

                               }
                           }
                           if (usemode==1) {
                               if (useflag[i+j*row]==true) {
                                   if (setsymboltype_mode==0){
                                       painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                   }else{
                                       if (setsymboltype[j]==0) painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                       if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                   }
                                   if (selected_y==i && selected_x==j) {
                                       if (setsymboltype_mode==0){
                                           painter->drawRect(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                       }else{
                                           if (setsymboltype[j]==0) painter->drawRect(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                           if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                       }

                                   }
                                   // draw markers
                                   if (marker>0){
                                       if (data_mark[i+j*row]>0 || data_mark[i+(j+1)*row]>0){
                                           painter->setBrush(Qt::NoBrush);
                                           pen.setColor(Qt::red);
                                           pen.setWidth(2);
                                           painter->setPen(pen);

                                           painter->drawRect(QRect(xmin-3,ymin-3,xmax-xmin+7,ymax-ymin+7));// draw standart Marker
                                           if (data_mark[i+j*row]==2 || data_mark[i+(j+1)*row]==2){
                                               if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                                           }
                                           if (data_mark[i+j*row]==3 || data_mark[i+(j+1)*row]==3){
                                                    if (y<=frame_bottom && y>=-frame_top) painter->drawLine(frame_left,y,frame_right,y);
                                           }
                                           if (data_mark[i+j*row]==4 || data_mark[i+(j+1)*row]==4){
                                                if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                                                if (y<=frame_bottom && y>=-frame_top) painter->drawLine(frame_left,y,frame_right,y);
                                           }

                                       }
                                   }
                                   // Label
                                   if (label_mode==1&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x+symbol_x*3+str_len<frame_right && str_len>0 && x<=0) {
                                           painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                           painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                       }
                                       if (x-symbol_x*3-str_len>frame_left && str_len>0 && x>0) {
                                           painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                           painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                       }
                                   }
                                   if (label_mode==2&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x+symbol_x*3+str_len<frame_right && str_len>0 ) {
                                           painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                           painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                       }

                                   }
                                   if (label_mode==3&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x-symbol_x*3-str_len>frame_left && str_len>0 ) {
                                           painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                           painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                       }

                                   }
                               }
                               if (useflag[i+j*row]==false) {
                                   if (setsymboltype_mode==0){
                                       painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                   }else{
                                       if (setsymboltype[j]==0) painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                       if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                   }
                                   painter->drawLine(xmin-2,ymin-2,xmax+2,ymax+2);
                                   painter->drawLine(xmin-2,ymax+2,xmax+2,ymin-2);

                                   if (selected_y==i && selected_x==j) {
                                       if (setsymboltype_mode==0){
                                           painter->drawRect(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                       }else{
                                           if (setsymboltype[j]==0) painter->drawRect(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                           if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin-2,ymin-2,(xmax-xmin)+4,(ymax-ymin)+4));
                                       }
                                       painter->drawLine(xmin-4,ymin-4,xmax+4,ymax+4);
                                       painter->drawLine(xmin-4,ymax+4,xmax+4,ymin-4);

                                   }
                                   // draw markers
                                   if (marker>0){
                                       if (data_mark[i+j*row]>0 || data_mark[i+(j+1)*row]>0){
                                           painter->setBrush(Qt::NoBrush);
                                           pen.setColor(Qt::red);
                                           pen.setWidth(1);
                                           painter->setPen(pen);

                                           painter->drawRect(QRect(xmin-3,ymin-3,xmax-xmin+7,ymax-ymin+7));// draw standart Marker
                                           if (data_mark[i+j*row]==2 || data_mark[i+(j+1)*row]==2){
                                               if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                                           }
                                           if (data_mark[i+j*row]==3 || data_mark[i+(j+1)*row]==3){
                                                    if (y<=frame_bottom && y>=-frame_top) painter->drawLine(frame_left,y,frame_right,y);
                                           }
                                           if (data_mark[i+j*row]==4 || data_mark[i+(j+1)*row]==4){
                                                if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                                                if (y<=frame_bottom && y>=-frame_top) painter->drawLine(frame_left,y,frame_right,y);
                                           }
                                       }
                                   }
                                   // Label
                                   if (label_mode==1&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x+symbol_x*3+str_len<frame_right && str_len>0 && x<=0) {
                                           painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                           painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                       }
                                       if (x-symbol_x*3-str_len>frame_left && str_len>0 && x>0) {
                                           painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                           painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                       }
                                   }
                                   if (label_mode==2&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x+symbol_x*3+str_len<frame_right && str_len>0 ) {
                                           painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                           painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                       }

                                   }
                                   if (label_mode==3&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x-symbol_x*3-str_len>frame_left && str_len>0 ) {
                                           painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                           painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                       }

                                   }

                               }
                           }
                           if (usemode==2) {
                               if (useflag[i+j*row]==true) {
                                   if (setsymboltype_mode==0){
                                       painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));

                                   }else{
                                       if (setsymboltype[j]==0) painter->drawRect(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                       if (setsymboltype[j]==1) painter->drawEllipse(QRect(xmin,ymin,xmax-xmin,ymax-ymin));
                                   }
                                   // Label
                                   if (label_mode==1&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x+symbol_x*3+str_len<frame_right && str_len>0 && x<=0) {
                                           painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                           painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                       }
                                       if (x-symbol_x*3-str_len>frame_left && str_len>0 && x>0) {
                                           painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                           painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                       }
                                   }
                                   if (label_mode==2&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x+symbol_x*3+str_len<frame_right && str_len>0 ) {
                                           painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x+symbol_x*3,y+(textsize/2),x+symbol_x*3+str_len,y+(textsize/2));
                                           painter->drawText(x+symbol_x*3,y+(textsize/2),str);
                                       }

                                   }
                                   if (label_mode==3&&com_hide==0) {
                                       // put Text Right or Left of Point
                                       QFont font_text_x("Monospace", textsize,QFont::Bold);
                                       font_text_x.setPixelSize(textsize);
                                       font_text_x.setFamily("Courier");
                                       font_text_x.setStyleHint(QFont::Monospace);
                                       painter->setFont(font_text_x);
                                       QFontMetrics fm_text_x(font_text_x);
                                       int str_len=fm_text_x.width(data_label[i+j*row]);

                                       // Text gegebenenflalls kürzen
                                       int str_spc=0;
                                       if (x<0){
                                           str_spc=(frame_right)-(x+symbol_x*3);
                                       } else {
                                           str_spc=(x-symbol_x*3)-(frame_left);
                                       }
                                       QString str=data_label[i+j*row];
                                       while(str_len>=str_spc && str.length()>0){
                                           str.chop(1);
                                           str_len=fm_text_x.width(str+"...");
                                       }
                                       if (str_len<fm_text_x.width(data_label[i+j*row])) str.append("...");



                                       if (x-symbol_x*3-str_len>frame_left && str_len>0 ) {
                                           painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                           painter->drawLine(x-symbol_x*3,y+(textsize/2),x-symbol_x*3-str_len,y+(textsize/2));
                                           painter->drawText(x-symbol_x*3-str_len,y+(textsize/2),str);
                                       }

                                   }
                               }
                           }
                           // Put Comments
                           // X Comment
                           if ((com_mode==1 || com_mode==3) && com_hide==0){
                               // put Text Left of Point
                               QFont font_text_x("Monospace", textsize,QFont::Bold);
                               font_text_x.setPixelSize(textsize);
                               font_text_x.setFamily("Courier");
                               font_text_x.setStyleHint(QFont::Monospace);
                               painter->setFont(font_text_x);
                               QFontMetrics fm_text_x(font_text_x);
                               int str_len=fm_text_x.width(data_com[i+j*row]);

                               // Text gegebenenflalls kürzen
                               int str_spc=0;
                               if (x<0){
                                   str_spc=(frame_right)-(x+symbol_x*3);
                               } else {
                                   str_spc=(x-symbol_x*3)-(frame_left);
                               }
                               QString str=data_com[i+j*row];
                               while(str_len>=str_spc && str.length()>0){
                                   str.chop(1);
                                   str_len=fm_text_x.width(str+"...");
                               }
                               if (str_len<fm_text_x.width(data_com[i+j*row])) str.append("...");



                               if (x+symbol_x*3+str_len<frame_right && str_len>0 && x<=0) {
                                   painter->drawLine(x,y,x+symbol_x*3,y+(textsize/2));
                                   painter->drawLine(x+symbol_x*3,y+(textsize/2),frame_right,y+(textsize/2));
                                   painter->drawText(frame_right-str_len-1,y+(textsize/2),str);
                               }
                               if (x-symbol_x*3-str_len>frame_left && str_len>0 && x>0) {
                                   painter->drawLine(x,y,x-symbol_x*3,y+(textsize/2));
                                   painter->drawLine(x-symbol_x*3,y+(textsize/2),frame_left,y+(textsize/2));
                                   painter->drawText(frame_left+1,y+(textsize/2),str);
                               }
                           }
                           // Y Comment
                           if ((com_mode==2 || com_mode==3)&&com_hide==0){
                               // put Text Top of Point
                               QFont font_text_x("Monospace", textsize,QFont::Bold);
                               font_text_x.setPixelSize(textsize);
                               font_text_x.setFamily("Courier");
                               font_text_x.setStyleHint(QFont::Monospace);
                               painter->setFont(font_text_x);
                               QFontMetrics fm_text_x(font_text_x);
                               int str_len=fm_text_x.width(data_com[i+(j+1)*row]);

                               // Text gegebenenflalls kürzen
                               int str_spc=0;
                               if (y>0){
                                   str_spc=(y-symbol_y*3)-(frame_top);
                               } else {
                                   str_spc=(frame_bottom)-(y+symbol_y*3);
                               }
                               QString str=data_com[i+(j+1)*row];
                               while(str_len>=str_spc && str.length()>0){
                                   str.chop(1);
                                   str_len=fm_text_x.width(str+"...");
                               }
                               if (str_len<fm_text_x.width(data_com[i+(j+1)*row])) str.append("...");


                               if (y-symbol_y*3-str_len>frame_top && str_len>0 && y>0) {
                                   painter->drawLine(x,y,x+(textsize/2),y-symbol_y*3);
                                   painter->drawLine(x+(textsize/2),y-symbol_y*3,x+(textsize/2),frame_top+1);
                                   painter->rotate(-90);
                                   painter->drawText((frame_top+1+str_len)*(-1),x+(textsize/2),str);
                                   painter->rotate(90);
                               }
                               if (y+symbol_y*3+str_len<frame_bottom && str_len>0 && y<=0) {
                                   painter->drawLine(x,y,x+(textsize/2),y+symbol_y*3);
                                   painter->drawLine(x+(textsize/2),y+symbol_y*3,x+(textsize/2),frame_bottom-1);
                                   painter->rotate(-90);
                                   painter->drawText((frame_bottom-1)*(-1),x+(textsize/2),str);
                                   painter->rotate(90);
                               }
                           }


                       }
                    }



                }





            }

            // Draw Marker

            if (marker_mode>0){

                int x = (int) (frame_left + (double) (frame_size_x) * (double) (((marker_x*factor_x) - x_min) / (x_max - x_min)));
                int y = (int) (frame_top + (double) (frame_size_y) * (double) ((y_max - (marker_y*factor_y)) / (y_max - y_min)));
                painter->setBrush(QColor(Qt::red));
                //QPen pen;
                pen.setWidth(2);
                pen.setColor(Qt::red);
                painter->setPen(pen);
                if (x>=frame_left && x<=frame_right && y<=frame_bottom && y>=frame_top){
                    painter->drawRect(QRect(x-3,y-3,6,6));
                }
                if (x>=frame_left && x<=frame_right) painter->drawLine(x,frame_top,x,frame_bottom);
                if (y<=frame_bottom && y>=frame_top) painter->drawLine(frame_left,y,frame_right,y);
            }
            // nothing message
            if (nothing==1&&column>0) {
                QFont font_titel("times", textsize, QFont::Bold);
                font_titel.setPixelSize(textsize);
                QFontMetrics fm_titel(font_titel);
                int str_len=fm_titel.width("No Available Numbers (NaN)");
                painter->setFont(font_titel);
                painter->drawText(-str_len/2,0,"No Available Numbers (NaN)");

            }

            // Draw Rect for Zoom
            if (select_Rect==1){
                //QPen pen;
                pen.setWidth(1);
                pen.setColor(Qt::red);
                painter->setBrush(Qt::NoBrush);
                painter->setPen(pen);
                painter->drawRect(QRect(sel_x1,sel_y1,(sel_x2-sel_x1),(sel_y2-sel_y1)));
            }
            // Draw Rect for Select
            if (select_Rect==2){
                //QPen pen;
                pen.setWidth(1);
                pen.setColor(Qt::blue);
                painter->setBrush(Qt::NoBrush);
                painter->setPen(pen);
                painter->drawRect(QRect(sel_x1,sel_y1,(sel_x2-sel_x1),(sel_y2-sel_y1)));
            }

        }
    }
}

void GraphData::setUse(bool *u,int m){
    useflag=u;
    usemode=m;
}

void GraphData::setError(float *d_e,int m){
    data_err=d_e;
    error=m;
}

void GraphData::setError2(float *d_e,int m){
    data_err2=d_e;
    error2=m;
}

void GraphData::setComment(QString *s,int m){
    data_com=s;
    com_mode=m;
}

void GraphData::setLabel(QString *s,int m){
    data_label=s;
    label_mode=m;
}


void GraphData::setColor(QColor *c,bool m){
    col_dat=c;
    color=m;
}

// sets plot size in pixel
void GraphData::setSize(int x,int y){
    plot_Size_X=x;
    plot_Size_Y=y;
}

// sets Data Array
void GraphData::setData(float *d,int c,int r){
    data=d;
    column=c;
    row=r;
}

// Sets Zoom Factors
void GraphData::setMultiplicator(double fx,double fy){
    factor_x=fx;
    factor_y=fy;
}

void GraphData::setTextSize(int texts, int titels, int margins){
    textsize=texts;
    titelsize=titels;
    margin=margins;
}

void GraphData::setSymbol(int size){
    symbolsize=size;
}

void GraphData::setMark(int *d_m,int m){
    data_mark=d_m;
    marker=m;
}

QRectF GraphData::boundingRect() const
{
}

QPainterPath GraphData::shape() const
{
}

// return the borders of the viewport in Cooridinates
double GraphData::get_x_max(){
    return x_max;
}

double GraphData::get_x_min(){
    return x_min;
}
double GraphData::get_y_min(){
    return y_min;
}

double GraphData::get_y_max(){
    return y_max;
}

// set selected Point X
void GraphData::setSelected_X(int n){
    selected_x=n;
    qDebug()<<"X"+QString::number(n);
}

// sets selected Point Y
void GraphData::setSelected_Y(int n){
    selected_y=n;
    qDebug()<<"Y"+QString::number(n);
}

// sets the Viewport in Cooridinates
void GraphData::setView(double x1,double x2,double y1,double y2){
    x_min=x1;
    x_max=x2;
    y_min=y1;
    y_max=y2;
}

// Sets the Zoom-Frame Cooridinates
void GraphData::setRect(int x1,int x2, int y1, int y2, int mode){
    select_Rect=mode;
    sel_x1=x1;
    sel_x2=x2;
    sel_y1=y1;
    sel_y2=y2;
}

// gets the x of selected Point
int GraphData::getSelected_X(){
     if (selected_x>=column || selected_x<0) selected_x=-1;
     return selected_x;
}
// gets the y of selected Point
int GraphData::getSelected_Y(){
    if (selected_y>row || selected_y<0) selected_y=-1;
    return selected_y;
}

void GraphData::setComHide(int i){
    com_hide=i;
}


int GraphData::getColumn(){
    return column;
}

int GraphData::getRow(){
    return row;
}

float GraphData::getData(int c,int r){
    return data[r+c*row];
}

int GraphData::getSymbolSize(){
    return symbolsize;
}

void GraphData::setLineWidth(int i){
    line_width=i;
}

int GraphData::getLineWidth(){
    return line_width;
}

void GraphData::setLineStyle(Qt::PenStyle sty){
    line_style=sty;
}

Qt::PenStyle GraphData::getLineStyle(){
    return line_style;
}

QColor GraphData::getLineColor(){
    return line_color;
}

void GraphData::setLineColor(QColor c){
    line_color=c;
}

void GraphData::addMarker(float x,float y,int m){
    marker_x=x;
    marker_y=y;
    marker_mode=m;
}


void GraphData::setSetColor(QColor *c,int m){
    setcolor_color=c;
    setcolor_mode=m;
}

void GraphData::setSetLineColor(QColor *c,int m){
    setlinecolor_color=c;
    setlinecolor_mode=m;
}

void GraphData::setSetLineWidth(int *s,int m){
    setlinewidth_mode=m;
    setlinewidth=s;
}

void GraphData::setSetSymbolsize(int *s,int m){
    setsymbolsize_mode=m;
    setsymbolsize=s;
}
void GraphData::setSetSymboltype(int *t,int m){
    setsymboltype_mode=m;
    setsymboltype=t;
}

void GraphData::setSetLineStyle(Qt::PenStyle *s, int m){
    setlinestyle_mode=m;
    setlinestyle=s;
}

void GraphData::setSetPoly(int *s, int m){
    setpoly_mode=m;
    setpoly=s;
}

void GraphData::setSetArea(float* area_x,float* area_y, int mode){
    setarea_mode=mode;
    setarea_x=area_x;
    setarea_y=area_y;
}

void GraphData::setAxisType(int x,int y){
    x_axis_type=x;
    y_axis_type=y;
}

void GraphData::calcFrame(){
    if (y_axis_type==0){// Left
        frame_right=plot_Size_X/2-margin-colorLegend_size_x;
        frame_left=-plot_Size_X/2+2.2*textsize+margin+5;
        frame_size_x=frame_right-frame_left;
    }
    if (y_axis_type==1){// Right
        frame_right=plot_Size_X/2-margin-2.2*textsize-colorLegend_size_x-5;
        frame_left=-plot_Size_X/2+margin;
        frame_size_x=frame_right-frame_left;
    }
    if (y_axis_type==2){// Both
        frame_right=plot_Size_X/2-margin-2.2*textsize-colorLegend_size_x-5;
        frame_left=-plot_Size_X/2+margin+2*textsize+5;
        frame_size_x=frame_right-frame_left;
    }
    if (y_axis_type==3){// Borderless
        frame_right=plot_Size_X/2-margin-colorLegend_size_x;
        frame_left=-plot_Size_X/2+margin;
        frame_size_x=frame_right-frame_left;
    }
    if (x_axis_type==0){// Bottom
        frame_top=-plot_Size_Y/2+margin+titelsize*1.3;
        frame_bottom=plot_Size_Y/2-margin-2.2*textsize-colorLegend_size_y-5;
        frame_size_y=frame_bottom-frame_top;
    }
    if (x_axis_type==1){// Top
        frame_top=-plot_Size_Y/2+margin+titelsize*1.3+2*textsize+5;
        frame_bottom=plot_Size_Y/2-margin-colorLegend_size_y;
        frame_size_y=frame_bottom-frame_top;
    }
    if (x_axis_type==2){// Both
        frame_top=-plot_Size_Y/2+margin+titelsize*1.3+2*textsize+5;
        frame_bottom=plot_Size_Y/2-margin-2.2*textsize-colorLegend_size_y-5;
        frame_size_y=frame_bottom-frame_top;
    }
    if (x_axis_type==3){// Borderless
        frame_top=-plot_Size_Y/2+margin;
        frame_bottom=plot_Size_Y/2-margin-colorLegend_size_y;
        frame_size_y=frame_bottom-frame_top;
    }
}

void GraphData::setRepeat(int x,float x1,float x2,int y,float y1,float y2){
    repeat_x=x;
    repeat_x1=x1;
    repeat_x2=x2;
    repeat_y=y;
    repeat_y1=y1;
    repeat_y2=y2;
}

void GraphData::setColorLegend(int type){
    colorLegend_type=type;
    colorLegend_size_x=0;
    colorLegend_size_y=0;
    if (colorLegend_type==1) colorLegend_size_y=textsize*4.7;
    if (colorLegend_type==2) colorLegend_size_x=textsize*4.7;
}

void GraphData::setColorScheme(int i){
    colorScheme=i;
}

