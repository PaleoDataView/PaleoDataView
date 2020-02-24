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
#include "graphgrid.h"
#include <QDebug>
GraphGrid::GraphGrid(QGraphicsScene *graphScene, float *d, int c, int r)
    : graph(graphScene),data(d),column(c),row(r)
{
    titel="Plot";
    titel_x="X(Y)";
    titel_y="Y(X)";
    plot_Size_X=100;
    plot_Size_Y=100;
    textsize=10;
    titelsize=1;
    margin=8;
    x_min=-1;
    x_max=1;
    y_min=-1;
    y_max=1;
    ticks_x=5;
    ticks_y=5;
    grid_flag=1;
    factor_x=1;
    factor_y=1;
    x_axis_type=0;
    y_axis_type=0;
    background=0;
    background_image=new QImage();
    background_image2=new QImage();
    colorScheme=-1;
    colorSchemeClass=new ColorScheme();
}

GraphGrid::~GraphGrid()
{
    delete background_image;
    delete background_image2;
}

void GraphGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
if (column>0){
    calcFrame();
    // draw Background
    if (background==1||background==3){

        // scale image

        QImage mapsection(background_image->copy((int)((x_min-background_x1)/((background_x2-background_x1)/(background_image->width()))),
                                                (int)((background_y2-y_max)/((background_y2-background_y1)/(background_image->height()))),
                                                (int)((x_max-x_min)/((background_x2-background_x1)/(background_image->width()))),
                                                (int)((y_max-y_min)/((background_y2-background_y1)/(background_image->height())))));
        mapsection=mapsection.scaled(frame_size_x,frame_size_y,Qt::IgnoreAspectRatio,Qt::FastTransformation);

        painter->drawImage(frame_left,frame_top,mapsection);

    }
    // draw Background2
        if (background>1){

            // scale image

            QImage mapsection2(background_image2->copy((int)((x_min-background_x1)/((background_x2-background_x1)/(background_image2->width()))),
                                                    (int)((background_y2-y_max)/((background_y2-background_y1)/(background_image2->height()))),
                                                    (int)((x_max-x_min)/((background_x2-background_x1)/(background_image2->width()))),
                                                    (int)((y_max-y_min)/((background_y2-background_y1)/(background_image2->height())))));
            mapsection2=mapsection2.scaled(frame_size_x,frame_size_y,Qt::IgnoreAspectRatio,Qt::FastTransformation);

            painter->drawImage(frame_left,frame_top,mapsection2);

        }


    // draw titel
    QFont font_titel("Arial", titelsize, QFont::Bold);
    font_titel.setPixelSize(titelsize);
    QFontMetrics fm_titel(font_titel);
    int str_len=fm_titel.width(titel);
    painter->setFont(font_titel);
    painter->drawText(-str_len/2,-plot_Size_Y/2+titelsize+margin,titel);

    // draw titel x-axis
    QFont font_titel_xy("Arial", textsize);
    font_titel_xy.setPixelSize(textsize);
    QFontMetrics fm_titel_xy(font_titel_xy);
    str_len=fm_titel_xy.width(titel_x);
    painter->setFont(font_titel_xy);
    if (x_axis_type==0||x_axis_type==2) painter->drawText(frame_left+frame_size_x/2-str_len/2,frame_bottom+5+2*textsize,titel_x);
    if (x_axis_type==1||x_axis_type==2) painter->drawText(frame_left+frame_size_x/2-str_len/2,frame_top-5-1*textsize,titel_x);

    // draw titel y-axis

    painter->setFont(font_titel_xy);
    str_len=fm_titel_xy.width(titel_y);
    painter->rotate(-90);//-frame_size_y/2+str_len/2+colorLegend_size_y/2
    if (y_axis_type==0||y_axis_type==2) painter->drawText(-(frame_top+frame_size_y/2)-str_len/2,frame_left-5-1*textsize,titel_y);
    if (y_axis_type==1||y_axis_type==2) painter->drawText(-(frame_top+frame_size_y/2)-str_len/2,frame_right+5+2*textsize,titel_y);
    painter->rotate(90);

    // draw Frame of Plot
    painter->drawRect(QRect(frame_left,frame_top,frame_size_x,frame_size_y));

    // draw x-Axis

    QFont font_axis_x("Arial", textsize);
    font_axis_x.setPixelSize(textsize);
    if (x_axis_type==3) font_axis_x.setBold(true);
    QFontMetrics fm_axis_x(font_axis_x);
    painter->setFont(font_axis_x);

    // estimate resolution
    double dx = (x_max - x_min) / ticks_x;
    if (dx == 0) {
         dx = x_max;
    }
    // automatic tick sizes
    if (fabs(dx) > 1) {
        dx = (int) (dx / pow(10, (int) ((1 / log(10)) * log(dx)))) * pow(10, (int) ((1 / log(10)) * log(dx)));
    } else {
        int i = 0;
        while ((int) dx == 0) {
            dx = dx * 10;
            i = i + 1;
        }
        dx = (int) dx;
        dx = dx / (pow(10, i));
    }
    if (dx == 0) {
        dx = (x_max - x_min) / ticks_x;
    }

    // draw lines and numbers
    double view_x_min = (ceil(x_min / dx)) * dx;
    double view_x_max = (floor(x_max / dx)) * dx;
    int len = plot_Size_X;

    for (int i = 0; view_x_min + dx * i <= x_max; i++) {
        int x = (int) (frame_left + (double) (frame_size_x) * (double) ((view_x_min + dx * i) - x_min) / (x_max - x_min));
        if (x_axis_type==0||x_axis_type==2) painter->drawLine(x, frame_bottom, x,frame_bottom + 3);
        if (x_axis_type==1||x_axis_type==2) painter->drawLine(x, frame_top, x,frame_top - 3);
        QString val = QString::number(floor((view_x_min + dx * i) * 1000000000) / 1000000000*factor_x);
        if (repeat_x>0) if(val.toDouble()>=0) {
            val=QString::number((val.toDouble())-(int)((val.toDouble()-repeat_x1)/(repeat_x2-repeat_x1))*(repeat_x2-repeat_x1));
        }else{
            val=QString::number((val.toDouble())-(int)((val.toDouble()-repeat_x2)/(repeat_x2-repeat_x1))*(repeat_x2-repeat_x1));
        }

        // prevent overlaping numbers
        str_len=fm_axis_x.width(val);

        len = len + (int) ((double) (frame_size_x) * (double) (dx / (view_x_max - view_x_min)));
        if (str_len < len - textsize ) {
            len = 0;
            if (x_axis_type==0||x_axis_type==2) painter->drawText(x - str_len / 2, frame_bottom+5+textsize,val);
            if (x_axis_type==1||x_axis_type==2) painter->drawText(x - str_len / 2, frame_top-5,val);
            if (x_axis_type==3) painter->drawText(x , frame_bottom-frame_size_y/2,val);
        }
        /*
        if (str_len + 5 < len - textsize && view_x_min + dx * i > 0) {
            len = 0;
            if (x_axis_type==0||x_axis_type==2) painter->drawText(x - str_len / 2, frame_bottom+5+textsize,val);
            if (x_axis_type==1||x_axis_type==2) painter->drawText(x - str_len / 2, frame_top-5,val);
            if (x_axis_type==3) painter->drawText(x , frame_bottom-frame_size_y/2,val);
        }
        if (str_len + 18 < len - textsize && view_x_min + dx * i == 0) {
            len = 0;
            if (x_axis_type==0||x_axis_type==2) painter->drawText(x - str_len / 2, frame_bottom+5+textsize,val);
            if (x_axis_type==1||x_axis_type==2) painter->drawText(x - str_len / 2, frame_top-5,val);
            if (x_axis_type==3) painter->drawText(x , frame_bottom-frame_size_y/2,val);
        }
*/
        // draw Grid
        if (grid_flag && (x > frame_left + 1) && (x < frame_right - 1)) {
            painter->setPen(QColor(Qt::gray));
            painter->drawLine(x, frame_top+1, x, frame_bottom-1);
            painter->setPen(QColor(Qt::black));
        }
    }

    // draw y-Axis

    QFont font_axis_y("Arial", textsize);
    font_axis_y.setPixelSize(textsize);
    if (y_axis_type==3) font_axis_y.setBold(true);
    QFontMetrics fm_axis_y(font_axis_y);
    painter->setFont(font_axis_y);
    // estimate resolution
    double dy = (y_max - y_min) / ticks_y;
    if (dy == 0) {
        dy = y_max;
    }

    if (fabs(dy) > 1) {
        dy = (int) (dy / pow(10, (int) ((1 / log(10)) * log(dy)))) * pow(10, (int) ((1 / log(10)) * log(dy)));
    } else {
        int i = 0;
        while ((int) dy == 0) {
            dy = dy * 10;
            i = i + 1;
        }
        dy = (int) dy;
        dy = dy / (pow(10, i));
    }
    if (dy == 0) {
        dy = (y_max - y_min) / ticks_y;
    }

    // draw lines and numbers
    double view_min_y = (ceil(y_min / dy)) * dy;
    double view_max_y = (floor(y_max / dy)) * dy;
    len = plot_Size_Y;

    for (int i = 0; view_max_y - dy * i >= y_min; i++) {
        int y = (int) (frame_top+ (double) (frame_size_y) * (double) ((y_max - (view_max_y - dy * i)) / (y_max - y_min)));
        if (grid_flag && y > (frame_top + 1) && y < (frame_bottom - 1)) {
            painter->setPen(QColor(Qt::gray));
            painter->drawLine(frame_left + 1, y, frame_right -1, y);
            painter->setPen(QColor(Qt::black));
        }
        if (y_axis_type==0||y_axis_type==2) painter->drawLine(frame_left, y, frame_left - 3, y);
        if (y_axis_type==1||y_axis_type==2) painter->drawLine(frame_right, y, frame_right + 3, y);

        QString val = QString::number(floor((view_max_y - dy * i) * 1000000000) / 1000000000*factor_y);
        if (repeat_y>0) if(val.toDouble()>=0) {
            val=QString::number((val.toDouble())-(int)((val.toDouble()-repeat_y1)/(repeat_y2-repeat_y1))*(repeat_y2-repeat_y1));
        }else{
            val=QString::number((val.toDouble())-(int)((val.toDouble()-repeat_y2)/(repeat_y2-repeat_y1))*(repeat_y2-repeat_y1));
        }
        // prevent overlaping numbers
        str_len=fm_axis_y.width(val);

        len = len + (int) ((double) (plot_Size_Y-2*margin-4*textsize-titelsize) * (double) (dy / (view_max_y - view_min_y)));
        painter->rotate(-90);
        if (str_len < len - textsize ) {
            len = 0;
            if (y_axis_type==0||y_axis_type==2) painter->drawText( (int) -y - str_len / 2,frame_left-5,val);
            if (y_axis_type==1||y_axis_type==2) painter->drawText( (int) -y - str_len / 2,frame_right+5+textsize,val);
            if (y_axis_type==3) painter->drawText( (int) -y,frame_left+frame_size_x/2,val);
        }/*
        if (str_len + 2*textsize < len - textsize && view_max_y - dy * i >= 0) {
            len = 0;
            if (y_axis_type==0||y_axis_type==2) painter->drawText( (int) -y - str_len / 2,frame_left-5,val);
            if (y_axis_type==1||y_axis_type==2) painter->drawText( (int) -y - str_len / 2,frame_right+5+textsize,val);
            if (y_axis_type==3) painter->drawText( (int) -y,frame_left+frame_size_x/2,val);
        }
        if (str_len + 3*textsize < len - textsize && view_max_y - dy * i == 0) {
            len = 0;
            if (y_axis_type==0||y_axis_type==2) painter->drawText( (int) -y - str_len / 2,frame_left-5,val);
            if (y_axis_type==1||y_axis_type==2) painter->drawText( (int) -y - str_len / 2,frame_right+5+textsize,val);
            if (y_axis_type==3) painter->drawText( (int) -y,frame_left+frame_size_x/2,val);
        }*/
        painter->rotate(90);
    }




}
    // Color Legend Display
    if (colorLegend_type>0){
        if (colorLegend_type==1){
            int colorLegend_start_y=frame_bottom+2.5*textsize;
            if (x_axis_type==3) colorLegend_start_y=frame_bottom;
            // draw titel color legend
            QFont font_titel_z("Arial", textsize);
            font_titel_z.setPixelSize(textsize);
            QFontMetrics fm_titel_z(font_titel_z);
            int z_len=fm_titel_z.width(colorLegend_titel);
            painter->setFont(font_titel_z);
            painter->drawText(-z_len/2,colorLegend_start_y+4*textsize+5,colorLegend_titel);

            // draw z-Axis
            QFont font_axis_z("Arial", textsize);
            font_axis_z.setPixelSize(textsize);
            QFontMetrics fm_axis_z(font_axis_z);
            painter->setFont(font_axis_z);

            // draw Frame of Plot
            // generate colors
            for (int i=0;i<frame_size_x-10;i++){
                float v=1-(float)(i)/(float)(frame_size_x-10);


                painter->setPen(colorSchemeClass->getColor(v,0,1,colorScheme));
                painter->drawLine(frame_right-i-5,colorLegend_start_y+textsize,frame_right-i-5,colorLegend_start_y+textsize*2);


            }

            painter->setPen(QColor(0,0,0));
            painter->drawRect(QRect(frame_left+5,colorLegend_start_y+textsize,frame_size_x-10,textsize));


            // estimate resolution
            double dz = (colorLegend_max - colorLegend_min) / ticks_x;
            if (dz == 0) {
                 dz = colorLegend_max;
            }

            if (fabs(dz) > 1) {
                dz = (int) (dz / pow(10, (int) ((1 / log(10)) * log(dz)))) * pow(10, (int) ((1 / log(10)) * log(dz)));
            } else {
                int i = 0;
                while ((int) dz == 0) {
                    dz = dz * 10;
                    i = i + 1;
                }
                dz = (int) dz;
                dz = dz / (pow(10, i));
            }
            if (dz == 0) {
                dz = (colorLegend_max - colorLegend_min) / ticks_x;
            }

            // draw lines and numbers
            double view_z_min = (ceil(colorLegend_min / dz)) * dz;
            double view_z_max = (floor(colorLegend_max / dz)) * dz;
            int len = plot_Size_X-10;

            for (int i = 0; view_z_min + dz * i < view_z_max+dz; i++) {
                int x = (int) (frame_left+5 + (double) (frame_size_x-10) * (double) ((view_z_min + dz * i) - colorLegend_min) / (colorLegend_max - colorLegend_min));
                painter->drawLine(x, colorLegend_start_y+textsize, x,colorLegend_start_y+textsize*2 + 3);

                QString val = QString::number(floor((view_z_min + dz * i) * 1000000000) / 1000000000);


                // prevent overlaping numbers
                z_len=fm_axis_z.width(val);
                len = len + (int) ((double) (frame_size_x-10) * (double) (dz / (view_z_max - view_z_min)));
                if (z_len < len - textsize ) {
                    len = 0;
                    painter->drawText(x - z_len / 2, colorLegend_start_y+3*textsize+5,val);
                }
                /*
                if (z_len + 5 < len - textsize && view_z_min + dz * i > 0) {
                    len = 0;
                    painter->drawText(x - z_len / 2, colorLegend_start_y+3*textsize+5,val);
                }
                if (z_len + 18 < len - textsize && view_z_min + dz * i == 0) {
                    len = 0;
                    painter->drawText(x - z_len / 2, colorLegend_start_y+3*textsize+5,val);
                }*/


            }
        }
    }

}

void GraphGrid::setSize(int x,int y){
    plot_Size_X=x;
    plot_Size_Y=y;
}

void GraphGrid::setData(float *d,int c,int r){
    data=d;
    column=c;
    row=r;
}

void GraphGrid::setTitel(QString t,QString tx,QString ty){
    titel=t;
    titel_x=tx;
    titel_y=ty;

}

void GraphGrid::setMultiplicator(double fx,double fy){
    factor_x=fx;
    factor_y=fy;
}

void GraphGrid::setTextSize(int texts, int titels, int margins){
    textsize=texts;
    titelsize=titels;
    margin=margins;
}

void GraphGrid::setView(double x1,double x2,double y1,double y2){
    x_min=x1;
    x_max=x2;
    y_min=y1;
    y_max=y2;
}

QRectF GraphGrid::boundingRect() const
{
}

QPainterPath GraphGrid::shape() const
{
}

QString GraphGrid::getTitel(){
    return titel;
}

int GraphGrid::getTitelSize(){
    return titelsize;
}

int GraphGrid::getMargin(){
    return margin;
}

int GraphGrid::getTextSize(){
    return textsize;
}

QString GraphGrid::getTitelX(){
    return titel_x;
}

QString GraphGrid::getTitelY(){
    return titel_y;
}

void GraphGrid::setAxisType(int x,int y){
    x_axis_type=x;
    y_axis_type=y;
}

void GraphGrid::calcFrame(){
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

void GraphGrid::setBackground(QString filename, float x1, float x2, float y1, float y2, int mode){
    background_filename=filename;
    background_x1=x1-repeat_x*(x2-x1);
    background_x2=x2+repeat_x*(x2-x1);
    background_y1=y1-repeat_y*(y2-y1);
    background_y2=y2+repeat_y*(y2-y1);
    background=mode;
    QImage basic;
    basic.load(background_filename);
    // create neibores for scrolling range
    delete background_image;
    background_image=new QImage((repeat_x*2+1)*basic.width(),(repeat_y*2+1)*basic.height(),QImage::Format_ARGB32_Premultiplied);
    background_image->fill(Qt::black);
    for (int i=0;i<repeat_x*2+1;i++){
        for (int j=0;j<repeat_y*2+1;j++) {
            QPainter painter(background_image);
            QPoint destPos(i*basic.width(),j*basic.height());
            painter.drawImage(destPos, basic);
            painter.end();
        }
    }

}

void GraphGrid::setBackground(QImage *image, float x1, float x2, float y1, float y2, int mode){

    background_x1=x1-repeat_x*(x2-x1);
    background_x2=x2+repeat_x*(x2-x1);
    background_y1=y1-repeat_y*(y2-y1);
    background_y2=y2+repeat_y*(y2-y1);
    background=mode;

    delete background_image;
    // create neibores for scrolling range
    background_image=new QImage((repeat_x*2+1)*image->width(),(repeat_y*2+1)*image->height(),QImage::Format_Grayscale8);
    background_image->fill(Qt::black);

    for (int i=0;i<repeat_x*2+1;i++){
        for (int j=0;j<repeat_y*2+1;j++) {
            QPainter painter(background_image);
            QPoint destPos(i*image->width(),j*image->height());
            painter.drawImage(destPos, *image);
            painter.end();
        }
    }


}

void GraphGrid::setBackground2(QImage *image, float x1, float x2, float y1, float y2, int mode){

    background_x1=x1-repeat_x*(x2-x1);
    background_x2=x2+repeat_x*(x2-x1);
    background_y1=y1-repeat_y*(y2-y1);
    background_y2=y2+repeat_y*(y2-y1);
    background=mode;

    // create neibores for scrolling range
    delete background_image2;
    background_image2=new QImage((repeat_x*2+1)*image->width(),(repeat_y*2+1)*image->height(),QImage::Format_ARGB6666_Premultiplied);
    background_image2->fill(QColor(0,0,0,0));
    for (int i=0;i<repeat_x*2+1;i++){
        for (int j=0;j<repeat_y*2+1;j++) {
            QPainter painter(background_image2);
            QPoint destPos(i*image->width(),j*image->height());
            painter.drawImage(destPos, *image);
            painter.end();
        }
    }

}

void GraphGrid::setRepeat(int x,float x1,float x2,int y,float y1,float y2){
    repeat_x=x;
    repeat_x1=x1;
    repeat_x2=x2;
    repeat_y=y;
    repeat_y1=y1;
    repeat_y2=y2;
}

void GraphGrid::setColorLegend(int type,QString t,float min,float max){
    // type 0 means nothing is painted
    // type 1 means Legend is painted under the graph
    // type 2 means Legend is painted right of graph
    colorLegend_type=type;
    colorLegend_min=min;
    colorLegend_max=max;
    colorLegend_titel=t;
    colorLegend_size_x=0;
    colorLegend_size_y=0;
    // reserve space in graph
    if (colorLegend_type==1) colorLegend_size_y=textsize*4.7;
    if (colorLegend_type==2) colorLegend_size_y=textsize*4.7;
}

void GraphGrid::setColorScheme(int i){
    colorScheme=i;
}

void GraphGrid::setTicks(int tx,int ty){
    ticks_x=tx;
    ticks_y=ty;
}
