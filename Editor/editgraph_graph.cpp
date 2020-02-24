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

#include "editgraph_graph.h"
#include <QDebug>

EditGraph_Graph::EditGraph_Graph(QWidget *dialog,float *d,int c,int r)
    : diag(dialog)
{

    plot_Size_X=100;
    plot_Size_Y=100;

    focus=0;

    moveFlag=0;
    number=1;
    // Setting up scene
    this->setSceneRect(-plot_Size_X/2+1,-plot_Size_Y/2+1,plot_Size_X-2,plot_Size_Y-2);

    // Generate plots
    x_min.reserve(number);
    y_min.reserve(number);
    x_max.reserve(number);
    y_max.reserve(number);

    x_axis_type.reserve(number);
    y_axis_type.reserve(number);
    ticks_x.reserve(number);
    ticks_y.reserve(number);

    ggrid.reserve(number);
    gdata.reserve(number);
    data.reserve(number);
    data_err.reserve(number);
    data_err2.reserve(number);
    error.reserve(number);
    error2.reserve(number);
    column.reserve(number);
    row.reserve(number);
    titel.reserve(number);
    titel_x.reserve(number);
    titel_y.reserve(number);
    path.reserve(number);
    textsize.reserve(number);
    titelsize.reserve(number);
    margin.reserve(number);
    dat_com.reserve(number);
    com_mode.reserve(number);
    com_hide.reserve(number);
    dat_label.reserve(number);
    label_mode.reserve(number);
    usemode.reserve(number);
    useflag.reserve(number);
    factor_x.reserve(number);
    factor_y.reserve(number);
    fix_x.reserve(number);
    fix_y.reserve(number);
    col_dat.reserve(number);
    color.reserve(number);
    data_mark.reserve(number);
    marker.reserve(number);
    symbol_size.reserve(number);
    line_width.reserve(number);
    line_style.reserve(number);
    line_color.reserve(number);
    marker_x.reserve(number);
    marker_y.reserve(number);
    marker_mode.reserve(number);

    setcolor_mode.reserve(number);
    setlinecolor_mode.reserve(number);
    setlinewidth_mode.reserve(number);
    setsymbolsize_mode.reserve(number);
    setsymboltype_mode.reserve(number);
    setlinestyle_mode.reserve(number);
    setcolor_color.reserve(number);
    setlinecolor_color.reserve(number);
    setsymbolsize.reserve(number);
    setsymboltype.reserve(number);
    setlinewidth.reserve(number);
    setlinestyle.reserve(number);

    setpoly_mode.reserve(number);
    setpoly.reserve(number);
    setarea_mode.reserve(number);
    setarea_x.reserve(number);
    setarea_y.reserve(number);

    background.reserve(number);
    background_filename.reserve(number);
    background_fileimage.reserve(number);
    background_image.reserve(number);
    background_x1.reserve(number);
    background_x2.reserve(number);
    background_y1.reserve(number);
    background_y2.reserve(number);

    repeat_x.reserve(number);
    repeat_x1.reserve(number);
    repeat_x2.reserve(number);
    repeat_y.reserve(number);
    repeat_y1.reserve(number);
    repeat_y2.reserve(number);

    fold_x.reserve(number);
    fold_y.reserve(number);

    limit_x1.reserve(number);
    limit_x2.reserve(number);
    limit_y1.reserve(number);
    limit_y2.reserve(number);

    lock_aspect.reserve(number);

    colorLegend_type.reserve(number);
    colorLegend_size_x.reserve(number);
    colorLegend_size_y.reserve(number);
    colorLegend_titel.reserve(number);
    colorLegend_min.reserve(number);
    colorLegend_max.reserve(number);

    colorScheme.reserve(number);

    autosize.reserve(number);
    autosize_mode.reserve(number);

    for (int i=0;i<number;i++){
        data[i]=d;
        column[i]=c;
        row[i]=r;

        ggrid[i]=new GraphGrid(this,data[i],column[i],row[i]);
        ggrid[i]->setPos(0,-plot_Size_Y);
        this->addItem(ggrid[i]);


        gdata[i]=new GraphData(this,data[i],column[i],row[i]);
        gdata[i]->setPos(0,-plot_Size_Y);
        this->addItem(gdata[i]);




        ggrid[i]->setSize(plot_Size_X,plot_Size_Y/number);
        gdata[i]->setSize(plot_Size_X,plot_Size_Y/number);

        ggrid[i]->setPos(0,-plot_Size_Y/2+i*(plot_Size_Y/number)+(plot_Size_Y/number)/2);
        gdata[i]->setPos(0,-plot_Size_Y/2+i*(plot_Size_Y/number)+(plot_Size_Y/number)/2);

        titel[i]=new QString;
        titel_x[i]=new QString;
        titel_y[i]=new QString;
        path[i]=new QString;
        titel[i]->append("Plot");
        titel_x[i]->append("X(Y)");
        titel_y[i]->append("Y(X)");
        path[i]->append("");
        textsize[i]=12;
        titelsize[i]=12;
        margin[i]=0;
        dat_com[i]=NULL;
        com_mode[i]=0;
        com_hide[i]=0;
        dat_label[i]=NULL;
        label_mode[i]=0;
        usemode[i]=0;
        useflag[i]=0;
        factor_x[i]=1;
        factor_y[i]=1;
        fix_x[i]=false;
        fix_y[i]=false;
        col_dat[i]=NULL;
        color[i]=0;
        data_err[i]=0;
        data_err2[i]=0;
        error[i]=0;
        error2[i]=0;
        data_mark[i]=0;
        marker[i]=0;
        symbol_size[i]=5;
        line_width[i]=1;
        line_style[i]=Qt::SolidLine;
        line_color[i]=Qt::black;
        marker_x[i]=0;
        marker_y[i]=0;
        marker_mode[i]=0;

        setcolor_mode[i]=0;
        setlinecolor_mode[i]=0;
        setlinewidth_mode[i]=0;
        setsymbolsize_mode[i]=0;
        setsymboltype_mode[i]=0;
        setlinestyle_mode[i]=0;
        setcolor_color[i]=NULL;
        setlinecolor_color[i]=NULL;
        setsymbolsize[i]=NULL;
        setlinewidth[i]=NULL;
        setlinestyle[i]=NULL;
        autosize_mode[i]=0;
        autosize[i]=NULL;


        setpoly_mode[i]=0;
        setpoly[i]=NULL;
        setarea_mode[i]=0;
        setarea_x[i]=NULL;
        setarea_y[i]=NULL;

        x_min[i]=-1;
        y_min[i]=-1;
        x_max[i]=1;
        y_max[i]=1;

        x_axis_type[i]=0;
        y_axis_type[i]=0;
        ticks_x[i]=5;
        ticks_y[i]=5;

        background[i]=0;
        background_filename[i]=new QString();
        background_filename[i]->append("");
        background_fileimage[i]=NULL;
        background_image[i]=NULL;

        background_x1[i]=0;
        background_x2[i]=0;
        background_y1[i]=0;
        background_y2[i]=0;

        repeat_x[i]=0;
        repeat_x1[i]=-3.40282e+38;
        repeat_x2[i]=3.40282e+38;
        repeat_y[i]=0;
        repeat_y1[i]=-3.40282e+38;
        repeat_y2[i]=3.40282e+38;

        limit_x1[i]=-3.40282e+38;
        limit_x2[i]=3.40282e+38;
        limit_y1[i]=-3.40282e+38;
        limit_y2[i]=3.40282e+38;

        lock_aspect[i]=0;
        fold_x[i]=0;
        fold_y[i]=0;
        colorLegend_type[i]=0;
        colorLegend_size_x[i]=0;
        colorLegend_size_y[i]=0;
        colorLegend_titel[i]=new QString();
        colorLegend_titel[i]->append("");
        colorLegend_min[i]=0;
        colorLegend_max[i]=1;

        colorScheme[i]=-1;
    }
    key_flag=0;
    frame_x1=0;
    frame_x2=0;
    frame_y1=0;
    frame_y2=0;



    gobject=new GraphObject(this);
    gobject->setPos(0,0);
    this->addItem(gobject);

    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

    this->autoSize();


    header=new QStringList();
    header->clear();
    err=new float[0];



    ggg=new GraphObject(this);
    gg=new GraphGrid(this,data[0],column[0],row[0]);
    gd=new GraphData(this,data[0],column[0],row[0]);


}

EditGraph_Graph::~EditGraph_Graph()
{
    deleteAll();

    delete gobject;// is added to this-> maybe not necessary

    delete[] err;


    delete ggg;
    delete gg;
    delete gd;


}

void EditGraph_Graph::deleteAll(){
    for (int i=0;ggrid.size();i++) delete ggrid[i];
    for (int i=0;gdata.size();i++) delete gdata[i];


    for (int i=0;titel.size();i++) delete titel[i];
    for (int i=0;titel_x.size();i++) delete titel_x[i];
    for (int i=0;titel_y.size();i++) delete titel_y[i];
    for (int i=0;path.size();i++) delete path[i];

    for (int i=0;background_filename.size();i++) delete background_filename[i];
}

void EditGraph_Graph::setNumber(int n){
    deleteAll();
    this->clear();
    number=n;

    // Generate plots
    x_min.reserve(number);
    y_min.reserve(number);
    x_max.reserve(number);
    y_max.reserve(number);
    x_axis_type.reserve(number);
    y_axis_type.reserve(number);
    ticks_x.reserve(number);
    ticks_y.reserve(number);
    ggrid.reserve(number);
    gdata.reserve(number);
    data.reserve(number);
    data_err.reserve(number);
    data_err2.reserve(number);
    error.reserve(number);
    error2.reserve(number);
    column.reserve(number);
    row.reserve(number);
    titel.reserve(number);
    titel_x.reserve(number);
    titel_y.reserve(number);
    path.reserve(number);
    textsize.reserve(number);
    titelsize.reserve(number);
    margin.reserve(number);
    dat_com.reserve(number);
    com_mode.reserve(number);
    com_hide.reserve(number);
    dat_label.reserve(number);
    label_mode.reserve(number);
    usemode.reserve(number);
    useflag.reserve(number);
    factor_x.reserve(number);
    factor_y.reserve(number);
    fix_x.reserve(number);
    fix_y.reserve(number);
    col_dat.reserve(number);
    color.reserve(number);
    data_mark.reserve(number);
    marker.reserve(number);
    symbol_size.reserve(number);
    line_width.reserve(number);
    line_style.reserve(number);
    line_color.reserve(number);
    marker_x.reserve(number);
    marker_y.reserve(number);
    marker_mode.reserve(number);
    setcolor_mode.reserve(number);
    setlinecolor_mode.reserve(number);
    setlinewidth_mode.reserve(number);
    setsymbolsize_mode.reserve(number);
    setsymboltype_mode.reserve(number);
    setlinestyle_mode.reserve(number);
    setcolor_color.reserve(number);
    setlinecolor_color.reserve(number);
    setsymbolsize.reserve(number);
    setsymboltype.reserve(number);
    setlinewidth.reserve(number);
    setlinestyle.reserve(number);
    setpoly_mode.reserve(number);
    setpoly.reserve(number);
    setarea_mode.reserve(number);
    setarea_x.reserve(number);
    setarea_y.reserve(number);
    background.reserve(number);
    background_filename.reserve(number);
    background_fileimage.reserve(number);
    background_image.reserve(number);
    background_x1.reserve(number);
    background_x2.reserve(number);
    background_y1.reserve(number);
    background_y2.reserve(number);
    repeat_x.reserve(number);
    repeat_x1.reserve(number);
    repeat_x2.reserve(number);
    repeat_y.reserve(number);
    repeat_y1.reserve(number);
    repeat_y2.reserve(number);
    limit_x1.reserve(number);
    limit_x2.reserve(number);
    limit_y1.reserve(number);
    limit_y2.reserve(number);
    fold_x.reserve(number);
    fold_y.reserve(number);
    lock_aspect.reserve(number);
    colorLegend_type.reserve(number);
    colorLegend_size_x.reserve(number);
    colorLegend_size_y.reserve(number);
    colorLegend_titel.reserve(number);
    colorLegend_min.reserve(number);
    colorLegend_max.reserve(number);
    colorScheme.reserve(number);

    autosize.reserve(number);
    autosize_mode.reserve(number);
    for (int i=0;i<number;i++){
        data[i]=0;
        column[i]=0;
        row[i]=0;

        ggrid[i]=new GraphGrid(this,data[i],column[i],row[i]);
        ggrid[i]->setPos(0,-plot_Size_Y);
        this->addItem(ggrid[i]);


        gdata[i]=new GraphData(this,data[i],column[i],row[i]);
        gdata[i]->setPos(0,-plot_Size_Y);
        this->addItem(gdata[i]);

        ggrid[i]->setSize(plot_Size_X,plot_Size_Y/number);
        gdata[i]->setSize(plot_Size_X,plot_Size_Y/number);

        ggrid[i]->setPos(0,-plot_Size_Y/2+i*(plot_Size_Y/number)+(plot_Size_Y/number)/2);
        gdata[i]->setPos(0,-plot_Size_Y/2+i*(plot_Size_Y/number)+(plot_Size_Y/number)/2);


        titel[i]=new QString;
        titel_x[i]=new QString;
        titel_y[i]=new QString;
        path[i]=new QString;
        titel[i]->append("Plot");
        titel_x[i]->append("X(Y)");
        titel_y[i]->append("Y(X)");
        path[i]->append("");
        textsize[i]=12;
        titelsize[i]=12;
        margin[i]=0;
        dat_com[i]=NULL;
        com_mode[i]=0;
        com_hide[i]=0;
        dat_label[i]=NULL;
        label_mode[i]=0;
        usemode[i]=0;
        useflag[i]=0;
        factor_x[i]=1;
        factor_y[i]=1;
        fix_x[i]=false;
        fix_y[i]=false;
        col_dat[i]=NULL;
        color[i]=0;
        data_err[i]=0;
        data_err2[i]=0;
        error[i]=0;
        error2[i]=0;
        data_mark[i]=0;
        marker[i]=0;
        symbol_size[i]=5;
        line_width[i]=1;
        line_style[i]=Qt::SolidLine;
        line_color[i]=Qt::black;
        marker_x[i]=0;
        marker_y[i]=0;
        marker_mode[i]=0;
        setcolor_mode[i]=0;
        setlinecolor_mode[i]=0;
        setlinewidth_mode[i]=0;
        setsymbolsize_mode[i]=0;
        setsymboltype_mode[i]=0;
        setlinestyle_mode[i]=0;
        setcolor_color[i]=NULL;
        setlinecolor_color[i]=NULL;
        setsymbolsize[i]=NULL;
        setsymboltype[i]=NULL;
        setlinewidth[i]=NULL;
        setlinestyle[i]=NULL;
        setpoly_mode[i]=0;
        setpoly[i]=NULL;
        setarea_mode[i]=0;
        setarea_x[i]=NULL;
        setarea_y[i]=NULL;
        x_min[i]=-1;
        y_min[i]=-1;
        x_max[i]=1;
        y_max[i]=1;
        x_axis_type[i]=0;
        y_axis_type[i]=0;
        ticks_x[i]=5;
        ticks_y[i]=5;
        background[i]=0;
        background_filename[i]=new QString();
        background_filename[i]->append("");
        background_fileimage[i]=NULL;
        background_image[i]=NULL;
        background_x1[i]=0;
        background_x2[i]=0;
        background_y1[i]=0;
        background_y2[i]=0;
        repeat_x[i]=0;
        repeat_x1[i]=-3.40282e+38;
        repeat_x2[i]=3.40282e+38;
        repeat_y[i]=0;
        repeat_y1[i]=-3.40282e+38;
        repeat_y2[i]=3.40282e+38;

        limit_x1[i]=-3.40282e+38;
        limit_x2[i]=3.40282e+38;
        limit_y1[i]=-3.40282e+38;
        limit_y2[i]=3.40282e+38;
        lock_aspect[i]=0;
        fold_x[i]=0;
        fold_y[i]=0;

        colorLegend_type[i]=0;
        colorLegend_size_x[i]=0;
        colorLegend_size_y[i]=0;
        colorLegend_titel[i]=new QString();
        colorLegend_titel[i]->append("");
        colorLegend_min[i]=0;
        colorLegend_max[i]=1;

        colorScheme[i]=-1;


        autosize_mode[i]=0;
        autosize[i]=NULL;

    }

    gobject=new GraphObject(this);
    gobject->setPos(0,0);
    this->addItem(gobject);
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

}

// Sets the data array. Its one dim, but you have to give columns and rows
void EditGraph_Graph::setData(float *d,int c,int r){
    data[0]=d;
    column[0]=c;
    row[0]=r;

    ggrid[0]->setData(data[0],column[0],row[0]);
    gdata[0]->setData(data[0],column[0],row[0]);
    error[0]=0;
    error2[0]=0;
    // maybe all must be reset to defaults!!!!

    update();

}

void EditGraph_Graph::setData(float *d,int c,int r,int n){
    data[n]=d;
    column[n]=c;
    row[n]=r;

    ggrid[n]->setData(data[n],column[n],row[n]);
    gdata[n]->setData(data[n],column[n],row[n]);
    error[n]=0;
    error2[n]=0;

    update();

}


void EditGraph_Graph::setError(float *d_e,int m){
    data_err[0]=d_e;
    error[0]=m;
    gdata[0]->setError(data_err[0],error[0]);
}

void EditGraph_Graph::setError2(float *d_e,int m){
    data_err2[0]=d_e;
    error2[0]=m;
    gdata[0]->setError2(data_err2[0],error2[0]);
}

void EditGraph_Graph::setError(float *d_e,int m,int n){
    data_err[n]=d_e;
    error[n]=m;
    gdata[n]->setError(data_err[n],error[n]);
}

void EditGraph_Graph::setError2(float *d_e,int m,int n){
    data_err2[n]=d_e;
    error2[n]=m;
    gdata[n]->setError2(data_err2[n],error2[n]);
}

void EditGraph_Graph::setComment(QString *s, int m){
    dat_com[0]=s;
    com_mode[0]=m;
    gdata[0]->setComment(s,m);

}

void EditGraph_Graph::setComment(QString *s, int m,int n){
    dat_com[n]=s;
    com_mode[n]=m;
    gdata[n]->setComment(s,m);
}

void EditGraph_Graph::setLabel(QString *s, int m){
    dat_label[0]=s;
    label_mode[0]=m;
    gdata[0]->setLabel(s,m);

}

void EditGraph_Graph::setLabel(QString *s, int m,int n){
    dat_label[n]=s;
    label_mode[n]=m;
    gdata[n]->setLabel(s,m);
}

void EditGraph_Graph::setColor(QColor *c,bool m){
    col_dat[0]=c;
    color[0]=m;
    gdata[0]->setColor(col_dat[0],color[0]);
}

void EditGraph_Graph::setColor(QColor *c,bool m,int n){
    col_dat[n]=c;
    color[n]=m;
    gdata[n]->setColor(col_dat[n],color[n]);
}

// Set Use Condition : true means to use point
// mode: 0 = no Use Mode
//       1 = Points Visible but crossed
//       2 = Points ignored completly
void EditGraph_Graph::setUse(bool *u,int m){
    useflag[0]=u;
    usemode[0]=m;
    gdata[0]->setUse(u,m);
}

void EditGraph_Graph::setUse(bool *u,int m,int n){
    useflag[n]=u;
    usemode[n]=m;
    gdata[n]->setUse(u,m);
}

// set Size of Plot in pixels
void EditGraph_Graph::setSize(int x,int y){
    plot_Size_X=x;
    plot_Size_Y=y;
    this->setSceneRect(-plot_Size_X/2+1,-plot_Size_Y/2+1,plot_Size_X-2,plot_Size_Y-2);
    for (int i=0;i<number;i++){
        ggrid[i]->setSize(x,y/number);
        gdata[i]->setSize(x,y/number);

        ggrid[i]->setPos(0,-plot_Size_Y/2+i*(plot_Size_Y/number)+(plot_Size_Y/number)/2);
        gdata[i]->setPos(0,-plot_Size_Y/2+i*(plot_Size_Y/number)+(plot_Size_Y/number)/2);


        x_min[i]=gdata[i]->get_x_min();
        x_max[i]=gdata[i]->get_x_max();
        y_min[i]=gdata[i]->get_y_min();
        y_max[i]=gdata[i]->get_y_max();

        double new_x_min=x_min[i],new_y_min=y_min[i],new_x_max=x_max[i],new_y_max=y_max[i];
        calcFrame(i);

        // check for limits
        if (new_x_min<limit_x1[i]) {
            new_x_max=new_x_max-(new_x_min-limit_x1[i]);
            if (new_x_max>limit_x2[i]) new_x_max=limit_x2[i];
            new_x_min=limit_x1[i];
        }
        if (new_x_max>limit_x2[i]){
            new_x_min=new_x_min+(limit_x2[i]-new_x_max);
            if (new_x_min<limit_x1[i]) new_x_min=limit_x1[i];
            new_x_max=limit_x2[i];
        }
        if (new_y_min<limit_y1[i]){
            new_y_max=new_y_max-(new_y_min-limit_y1[i]);
            if (new_y_max>limit_y2[i]) new_y_max=limit_y2[i];
            new_y_min=limit_y1[i];
        }
        if (new_y_max>limit_y2[i]){
            new_y_min=new_y_min+(limit_y2[i]-new_y_max);
            if (new_y_min<limit_y1[i]) new_y_min=limit_y1[i];
            new_y_max=limit_y2[i];
        }

        // adjust aspect_ratio
        if (lock_aspect[i]==1) {// x is bound to y
            float res=(new_y_max-new_y_min)/(float)frame_size_y;
            float x=new_x_min+(new_x_max-new_x_min)/2.0;
            new_x_min=x-((float)frame_size_x*res)/2.0;
            new_x_max=x+((float)frame_size_x*res)/2.0;
        }
        if (lock_aspect[i]==2) {// y is bound to x
            float res=(new_x_max-new_x_min)/(float)frame_size_x;
            float y=new_y_min+(new_y_max-new_y_min)/2.0;
            new_y_min=y-((float)frame_size_y*res)/2.0;
            new_y_max=y+((float)frame_size_y*res)/2.0;
        }

        if (new_x_min>=limit_x1[i] && new_x_max<=limit_x2[i] && new_y_min>=limit_y1[i] && new_y_max<=limit_y2[i]){
            ggrid[i]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
            gdata[i]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
        }

        gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

    }
}

// Set the titels of Plot and x-y Axis
void EditGraph_Graph::setTitel(QString t,QString tx,QString ty){
    ggrid[0]->setTitel(t,tx,ty);
    titel[0]->clear();
    titel_x[0]->clear();
    titel_y[0]->clear();

    titel[0]->append(t);
    titel_x[0]->append(tx);
    titel_y[0]->append(ty);

}
// Set the titels of Plot and x-y Axis
void EditGraph_Graph::setTitel(QString t,QString tx,QString ty,int n){
    ggrid[n]->setTitel(t,tx,ty);
    titel[n]->clear();
    titel_x[n]->clear();
    titel_y[n]->clear();
    titel[n]->append(t);
    titel_x[n]->append(tx);
    titel_y[n]->append(ty);
}
// sets multiplicators for axes (use 1 for normal or -1 for invert axis)
void EditGraph_Graph::setMultiplicator(double fx, double fy){
    factor_x[0]=fx;
    factor_y[0]=fy;
    gdata[0]->setMultiplicator(fx,fy);
    ggrid[0]->setMultiplicator(fx,fy);
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);
}

void EditGraph_Graph::setMultiplicator(double fx, double fy,int n){
    factor_x[n]=fx;
    factor_y[n]=fy;
    gdata[n]->setMultiplicator(fx,fy);
    ggrid[n]->setMultiplicator(fx,fy);
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

}

void EditGraph_Graph::setTextSize(int texts, int titels, int margins){
    textsize[0]=texts;
    titelsize[0]=titels;
    margin[0]=margins;
    gdata[0]->setTextSize(textsize[0],titelsize[0],margin[0]);
    ggrid[0]->setTextSize(textsize[0],titelsize[0],margin[0]);
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

}

void EditGraph_Graph::setTextSize(int texts, int titels, int margins,int n){
    textsize[n]=texts;
    titelsize[n]=titels;
    margin[n]=margins;
    gdata[n]->setTextSize(textsize[n],titelsize[n],margin[n]);
    ggrid[n]->setTextSize(textsize[n],titelsize[n],margin[n]);
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

}

void EditGraph_Graph::setSymbol(int size){
    symbol_size[0]=size;
    gdata[0]->setSymbol(symbol_size[0]);
}
void EditGraph_Graph::setSymbol(int size,int n){
    symbol_size[n]=size;
    gdata[n]->setSymbol(symbol_size[n]);
}
int EditGraph_Graph::getSymbol(){
    return getSymbol(0);
}
int EditGraph_Graph::getSymbol(int n){
    return symbol_size[n];
}

void EditGraph_Graph::setMark(int *d_m,int m){
    marker[0]=m;
    data_mark[0]=d_m;
    gdata[0]->setMark(data_mark[0],marker[0]);
}

void EditGraph_Graph::setMark(int *d_m,int m,int n){
    marker[n]=m;
    data_mark[n]=d_m;
    gdata[n]->setMark(data_mark[n],marker[n]);
}

void EditGraph_Graph::setLineWidth(int i){
    line_width[0]=i;
    gdata[0]->setLineWidth(line_width[0]);
}

void EditGraph_Graph::setLineStyle(Qt::PenStyle sty){
    line_style[0]=sty;
    gdata[0]->setLineStyle(line_style[0]);
}

void EditGraph_Graph::setLineColor(QColor c){
    line_color[0]=c;
    gdata[0]->setLineColor(line_color[0]);
}

void EditGraph_Graph::setLineWidth(int i,int n){
    line_width[n]=i;
    gdata[n]->setLineWidth(line_width[n]);
}

void EditGraph_Graph::setLineStyle(Qt::PenStyle sty,int n){
    line_style[n]=sty;
    gdata[n]->setLineStyle(line_style[n]);
}

void EditGraph_Graph::setLineColor(QColor c,int n){
    line_color[n]=c;
    gdata[n]->setLineColor(line_color[n]);
}

void EditGraph_Graph::addMarker(float x,float y,int m){
    marker_x[0]=x;
    marker_y[0]=y;
    marker_mode[0]=m;
    gdata[0]->addMarker(marker_x[0],marker_y[0],marker_mode[0]);
    update();
}

void EditGraph_Graph::addMarker(float x,float y,int m,int n){
    marker_x[n]=x;
    marker_y[n]=y;
    marker_mode[n]=m;
    gdata[n]->addMarker(marker_x[n],marker_y[n],marker_mode[n]);
    update();
}

void EditGraph_Graph::setSetColor(QColor *c,int m){
    setcolor_mode[0]=m;
    setcolor_color[0]=c;
    gdata[0]->setSetColor(setcolor_color[0],setcolor_mode[0]);
}

void EditGraph_Graph::setSetLineColor(QColor *c,int m){
    setlinecolor_mode[0]=m;
    setlinecolor_color[0]=c;
    gdata[0]->setSetLineColor(setlinecolor_color[0],setlinecolor_mode[0]);
}

void EditGraph_Graph::setSetLineWidth(int *s,int m){
    setlinewidth_mode[0]=m;
    setlinewidth[0]=s;
    gdata[0]->setSetLineWidth(setlinewidth[0],setlinewidth_mode[0]);
}

void EditGraph_Graph::setSetSymbolsize(int *s,int m){
    setsymbolsize_mode[0]=m;
    setsymbolsize[0]=s;
    gdata[0]->setSetSymbolsize(setsymbolsize[0],setsymbolsize_mode[0]);
}

void EditGraph_Graph::setSetLineStyle(Qt::PenStyle *s, int m){
    setlinestyle_mode[0]=m;
    setlinestyle[0]=s;
    gdata[0]->setSetLineStyle(setlinestyle[0],setlinestyle_mode[0]);
}

void EditGraph_Graph::setSetColor(QColor *c,int m,int n){
    setcolor_mode[n]=m;
    setcolor_color[n]=c;
    gdata[n]->setSetColor(setcolor_color[n],setcolor_mode[n]);
}

void EditGraph_Graph::setSetLineColor(QColor *c,int m,int n){
    setlinecolor_mode[n]=m;
    setlinecolor_color[n]=c;
    gdata[n]->setSetLineColor(setlinecolor_color[n],setlinecolor_mode[n]);
}

void EditGraph_Graph::setSetLineWidth(int *s,int m,int n){
    setlinewidth_mode[n]=m;
    setlinewidth[n]=s;
    gdata[n]->setSetLineWidth(setlinewidth[n],setlinewidth_mode[n]);
}

void EditGraph_Graph::setSetSymbolsize(int *s,int m,int n){
    setsymbolsize_mode[n]=m;
    setsymbolsize[n]=s;
    gdata[n]->setSetSymbolsize(setsymbolsize[n],setsymbolsize_mode[n]);
}

void EditGraph_Graph::setSetSymboltype(int *t,int m){
    setsymboltype_mode[0]=m;
    setsymboltype[0]=t;
    gdata[0]->setSetSymboltype(setsymboltype[0],setsymboltype_mode[0]);
}

void EditGraph_Graph::setSetSymboltype(int *t,int m,int n){
    setsymboltype_mode[n]=m;
    setsymboltype[n]=t;
    gdata[n]->setSetSymboltype(setsymboltype[n],setsymboltype_mode[n]);
}

void EditGraph_Graph::setSetLineStyle(Qt::PenStyle *s, int m,int n){
    setlinestyle_mode[n]=m;
    setlinestyle[n]=s;
    gdata[n]->setSetLineStyle(setlinestyle[n],setlinestyle_mode[n]);
}


void EditGraph_Graph::setSetPoly(int *s,int m){
    setpoly_mode[0]=m;
    setpoly[0]=s;
    gdata[0]->setSetPoly(setpoly[0],setpoly_mode[0]);
}

void EditGraph_Graph::setSetArea(float* area_x,float* area_y,int area_mode){
    setarea_mode[0]=area_mode;
    setarea_x[0]=area_x;
    setarea_y[0]=area_y;
    gdata[0]->setSetArea(setarea_x[0],setarea_y[0],setarea_mode[0]);
}

void EditGraph_Graph::setSetPoly(int *s,int m,int n){
    setpoly_mode[n]=m;
    setpoly[n]=s;
    gdata[n]->setSetPoly(setpoly[n],setpoly_mode[n]);
}

void EditGraph_Graph::setSetArea(float* area_x,float* area_y,int area_mode,int n){
    setarea_mode[n]=area_mode;
    setarea_x[n]=area_x;
    setarea_y[n]=area_y;
    gdata[n]->setSetArea(setarea_x[n],setarea_y[n],setarea_mode[n]);
}

void EditGraph_Graph::fixRange(bool x,bool y){
    fix_x[0]=x;
    fix_y[0]=y;
}

void EditGraph_Graph::fixRange(bool x,bool y,int n){
    fix_x[n]=x;
    fix_y[n]=y;
}

void EditGraph_Graph::setColorLegend(int type,QString t,float min, float max){
    setColorLegend(type,t,min,max,0);
}

void EditGraph_Graph::setColorLegend(int type,QString t, float min, float max, int i){
    ggrid[i]->setColorLegend(type,t,min,max);
    gdata[i]->setColorLegend(type);
    colorLegend_type[i]=type;
    colorLegend_size_x[i]=0;
    colorLegend_size_y[i]=0;
    if(colorLegend_type[i]==1) colorLegend_size_y[i]=textsize[i]*4.7;
    if(colorLegend_type[i]==2) colorLegend_size_x[i]=textsize[i]*4.7;
    gobject->setColorLegend(&colorLegend_type[0]);
}

void EditGraph_Graph::setColorScheme(int i){
    setColorScheme(i,0);
}

void EditGraph_Graph::setColorScheme(int i,int n){
    colorScheme[n]=i;
    ggrid[n]->setColorScheme(i);
    gdata[n]->setColorScheme(i);
    gobject->setColorScheme(i);
}

void EditGraph_Graph::setAutosize(int* as,int as_m){
    setAutosize(as,as_m,0);
}

void EditGraph_Graph::setAutosize(int* as,int as_m,int n){
    autosize[n]=as;
    autosize_mode[n]=as_m;
}
void EditGraph_Graph::setTicks(int tx,int ty){
    setTicks(tx,ty,0);
}

void EditGraph_Graph::setTicks(int tx,int ty,int n){
    ticks_x[n]=tx;
    ticks_y[n]=ty;
    ggrid[n]->setTicks(ticks_x[n],ticks_y[n]);
}

// Autosize on all Data
void EditGraph_Graph::autoSize(){
    autoSize(0);
}

void EditGraph_Graph::autoSize(int n){
    x_min[n]=3.40282e+38;
    x_max[n]=-3.40282e+38;
    y_min[n]=3.40282e+38;
    y_max[n]=-3.40282e+38;
    for (int j=0;j<column[n];j=j+2){
        if (autosize_mode[n]>0){
            if (autosize[n][j]==1){
            for (int i=0;i<row[n];i++){
                if (std::isnan(data[n][i+j*row[n]])==0 && std::isnan(data[n][i+(j+1)*row[n]])==0 && usemode[n]!=2){
                    if (error2[n]==0){
                        if (error[n]==0||error[n]==4){
                            if (x_min[n]>data[n][i+j*row[n]]*factor_x[n]) x_min[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (x_max[n]<data[n][i+j*row[n]]*factor_x[n]) x_max[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (y_min[n]>data[n][i+(j+1)*row[n]]*factor_y[n]) y_min[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                            if (y_max[n]<data[n][i+(j+1)*row[n]]*factor_y[n]) y_max[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                        } else {
                            float x_err=data_err[n][i+j*row[n]];
                            float y_err=data_err[n][i+(j+1)*row[n]];
                            if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                            if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                            if (error[n]==1){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                                if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            }
                            if (error[n]==2){
                                if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));
                            }
                            if (error[n]==3){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));
                            }

                        }
                    } else {


                        float x_err=data_err[n][i+j*row[n]];
                        float y_err=data_err[n][i+(j+1)*row[n]];
                        float x_err2=data_err2[n][i+j*row[n]];
                        float y_err2=data_err2[n][i+(j+1)*row[n]];
                        if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                        if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                        if (std::isnan(data_err2[n][i+(j)*row[n]])) x_err2=0;
                        if (std::isnan(data_err2[n][i+(j+1)*row[n]])) y_err2=0;
                        if (error2[n]==1){
                            if (x_min[n]>(data[n][i+j*row[n]]-x_err)*factor_x[n]) x_min[n]=(data[n][i+j*row[n]]-x_err)*factor_x[n];
                            if (x_max[n]<(data[n][i+j*row[n]]+x_err2)*factor_x[n]) x_max[n]=(data[n][i+j*row[n]]+x_err2)*factor_x[n];
                            if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                        }
                        if (error2[n]==2){
                            if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]-y_err)*factor_y[n]) y_min[n]=(data[n][i+(j+1)*row[n]]-y_err)*factor_y[n];
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]+y_err2)*factor_y[n]) y_max[n]=(data[n][i+(j+1)*row[n]]+y_err2)*factor_y[n];
                        }
                        if (error2[n]==3){

                            if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                            if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]))) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]));
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]))) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]));
                        }

                    }
                }
                if (std::isnan(data[n][i+j*row[n]])==0 && std::isnan(data[n][i+(j+1)*row[n]])==0 && usemode[n]==2){
                    if (error2[n]==0) {
                        if (error[n]==0||error[n]==4){
                            if (x_min[n]>data[n][i+j*row[n]]*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_min[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (x_max[n]<data[n][i+j*row[n]]*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_max[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (y_min[n]>data[n][i+(j+1)*row[n]]*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_min[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                            if (y_max[n]<data[n][i+(j+1)*row[n]]*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_max[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                        } else {
                            float x_err=data_err[n][i+j*row[n]];
                            float y_err=data_err[n][i+(j+1)*row[n]];
                            if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                            if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                            if (error[n]==1){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]
                                        && useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                                if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]
                                        && useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            }
                            if (error[n]==2){
                                if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]
                                        && useflag[n][i+j*row[n]]==true) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]
                                        && useflag[n][i+j*row[n]]==true) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));
                            }
                            if (error[n]==3){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));

                            }
                        }
                    } else {

                        float x_err=data_err[n][i+j*row[n]];
                        float y_err=data_err[n][i+(j+1)*row[n]];
                        float x_err2=data_err2[n][i+j*row[n]];
                        float y_err2=data_err2[n][i+(j+1)*row[n]];
                        if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                        if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                        if (std::isnan(data_err2[n][i+(j)*row[n]])) x_err2=0;
                        if (std::isnan(data_err2[n][i+(j+1)*row[n]])) y_err2=0;
                        if (error2[n]==1){
                            if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                            if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]));
                            if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                        }
                        if (error2[n]==2){
                            if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]));
                        }
                        if (error2[n]==3){
                            if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                            if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]));
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]));
                        }

                    }
                }
            }}
        } else {
            for (int i=0;i<row[n];i++){
                if (std::isnan(data[n][i+j*row[n]])==0 && std::isnan(data[n][i+(j+1)*row[n]])==0 && usemode[n]!=2){
                    if (error2[n]==0){
                        if (error[n]==0||error[n]==4){
                            if (x_min[n]>data[n][i+j*row[n]]*factor_x[n]) x_min[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (x_max[n]<data[n][i+j*row[n]]*factor_x[n]) x_max[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (y_min[n]>data[n][i+(j+1)*row[n]]*factor_y[n]) y_min[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                            if (y_max[n]<data[n][i+(j+1)*row[n]]*factor_y[n]) y_max[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                        } else {
                            float x_err=data_err[n][i+j*row[n]];
                            float y_err=data_err[n][i+(j+1)*row[n]];
                            if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                            if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                            if (error[n]==1){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                                if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            }
                            if (error[n]==2){
                                if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));
                            }
                            if (error[n]==3){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));
                            }

                        }
                    } else {


                        float x_err=data_err[n][i+j*row[n]];
                        float y_err=data_err[n][i+(j+1)*row[n]];
                        float x_err2=data_err2[n][i+j*row[n]];
                        float y_err2=data_err2[n][i+(j+1)*row[n]];
                        if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                        if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                        if (std::isnan(data_err2[n][i+(j)*row[n]])) x_err2=0;
                        if (std::isnan(data_err2[n][i+(j+1)*row[n]])) y_err2=0;
                        if (error2[n]==1){
                            if (x_min[n]>(data[n][i+j*row[n]]-x_err)*factor_x[n]) x_min[n]=(data[n][i+j*row[n]]-x_err)*factor_x[n];
                            if (x_max[n]<(data[n][i+j*row[n]]+x_err2)*factor_x[n]) x_max[n]=(data[n][i+j*row[n]]+x_err2)*factor_x[n];
                            if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                        }
                        if (error2[n]==2){
                            if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]-y_err)*factor_y[n]) y_min[n]=(data[n][i+(j+1)*row[n]]-y_err)*factor_y[n];
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]+y_err2)*factor_y[n]) y_max[n]=(data[n][i+(j+1)*row[n]]+y_err2)*factor_y[n];
                        }
                        if (error2[n]==3){

                            if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                            if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]))) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]));
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]))) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]));
                        }

                    }
                }
                if (std::isnan(data[n][i+j*row[n]])==0 && std::isnan(data[n][i+(j+1)*row[n]])==0 && usemode[n]==2){
                    if (error2[n]==0) {
                        if (error[n]==0||error[n]==4){
                            if (x_min[n]>data[n][i+j*row[n]]*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_min[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (x_max[n]<data[n][i+j*row[n]]*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_max[n]=data[n][i+j*row[n]]*factor_x[n];
                            if (y_min[n]>data[n][i+(j+1)*row[n]]*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_min[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                            if (y_max[n]<data[n][i+(j+1)*row[n]]*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_max[n]=data[n][i+(j+1)*row[n]]*factor_y[n];
                        } else {
                            float x_err=data_err[n][i+j*row[n]];
                            float y_err=data_err[n][i+(j+1)*row[n]];
                            if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                            if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                            if (error[n]==1){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]
                                        && useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                                if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]
                                        && useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            }
                            if (error[n]==2){
                                if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]
                                        && useflag[n][i+j*row[n]]==true) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]
                                        && useflag[n][i+j*row[n]]==true) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));
                            }
                            if (error[n]==3){
                                if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                                if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err*abs(factor_x[n]));
                                if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                                if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err*abs(factor_y[n]));

                            }
                        }
                    } else {

                        float x_err=data_err[n][i+j*row[n]];
                        float y_err=data_err[n][i+(j+1)*row[n]];
                        float x_err2=data_err2[n][i+j*row[n]];
                        float y_err2=data_err2[n][i+(j+1)*row[n]];
                        if (std::isnan(data_err[n][i+(j)*row[n]])) x_err=0;
                        if (std::isnan(data_err[n][i+(j+1)*row[n]])) y_err=0;
                        if (std::isnan(data_err2[n][i+(j)*row[n]])) x_err2=0;
                        if (std::isnan(data_err2[n][i+(j+1)*row[n]])) y_err2=0;
                        if (error2[n]==1){
                            if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                            if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]));
                            if (y_min[n]>(data[n][i+(j+1)*row[n]])*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                            if (y_max[n]<(data[n][i+(j+1)*row[n]])*factor_y[n]
                                    && useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]])*factor_y[n];
                        }
                        if (error2[n]==2){
                            if (x_min[n]>(data[n][i+j*row[n]])*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_min[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (x_max[n]<(data[n][i+j*row[n]])*factor_x[n]
                                    && useflag[n][i+j*row[n]]==true) x_max[n]=(data[n][i+j*row[n]])*factor_x[n];
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]));
                        }
                        if (error2[n]==3){
                            if (x_min[n]>(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_min[n]=(data[n][i+j*row[n]]*factor_x[n]-x_err*abs(factor_x[n]));
                            if (x_max[n]<(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]))&& useflag[n][i+(j+1)*row[n]]==true) x_max[n]=(data[n][i+j*row[n]]*factor_x[n]+x_err2*abs(factor_x[n]));
                            if (y_min[n]>(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_min[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]-y_err*abs(factor_y[n]));
                            if (y_max[n]<(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]))&& useflag[n][i+(j+1)*row[n]]==true) y_max[n]=(data[n][i+(j+1)*row[n]]*factor_y[n]+y_err2*abs(factor_y[n]));
                        }

                    }
                }
            }
        }
    }
    x_min[n]-=(x_max[n]-x_min[n])*0.05;
    x_max[n]+=(x_max[n]-x_min[n])*0.05;
    y_min[n]-=(y_max[n]-y_min[n])*0.05;
    y_max[n]+=(y_max[n]-y_min[n])*0.05;
    if (x_min[n]==x_max[n]) {
        x_min[n]-=1;
        x_max[n]+=1;
    }
    if (y_min[n]==y_max[n]){
        y_min[n]-=1;
        y_max[n]+=1;
    }

    calcFrame(n);
    // adjust aspect_ratio
    if (lock_aspect[n]==1) {// x is bound to y
        float res=(y_max[n]-y_min[n])/(float)frame_size_y;
        float x=x_min[n]+(x_max[n]-x_min[n])/2.0;
        x_min[n]=x-((float)frame_size_x*res)/2.0;
        x_max[n]=x+((float)frame_size_x*res)/2.0;
    }
    if (lock_aspect[n]==2) {// y is bound to x
        float res=(x_max[n]-x_min[n])/(float)frame_size_x;
        float y=y_min[n]+(y_max[n]-y_min[n])/2.0;
        y_min[n]=y-((float)frame_size_y*res)/2.0;
        y_max[n]=y+((float)frame_size_y*res)/2.0;
    }

    if (x_min[n]<limit_x1[n]) x_min[n]=limit_x1[n];
    if (x_max[n]>limit_x2[n]) x_max[n]=limit_x2[n];
    if (y_min[n]<limit_y1[n]) y_min[n]=limit_y1[n];
    if (y_max[n]>limit_y2[n]) y_max[n]=limit_y2[n];

    ggrid[n]->setView(x_min[n],x_max[n],y_min[n],y_max[n]);
    gdata[n]->setView(x_min[n],x_max[n],y_min[n],y_max[n]);
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

    update();
}

// Start drawing a frame if STRG is pressed : Starting Kooridinates
void EditGraph_Graph::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent1){

    // get current info from graph
    focus=(mouseEvent1->scenePos().y()+plot_Size_Y/2)/(plot_Size_Y/number);

    textsize[focus]=ggrid[focus]->getTextSize();
    titelsize[focus]=ggrid[focus]->getTitelSize();
    margin[focus]=ggrid[focus]->getMargin();

    if (key_flag==1){
        frame_x1=mouseEvent1->scenePos().x();
        frame_y1=((mouseEvent1->scenePos().y()));
        //qDebug()<<QString::number(frame_y1);
    }
    if (key_flag==2){
        frame_x1=mouseEvent1->scenePos().x();
        frame_y1=((mouseEvent1->scenePos().y()));
        //qDebug()<<QString::number(frame_y1);
    }

}
// Realizes Point Clicked and Zoom-Frame-finish
void EditGraph_Graph::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent2){

    //qDebug()<<QString::number(focus)+":"+QString::number(mouseEvent2->scenePos().y())+":"+QString::number((((mouseEvent2->scenePos().y()+plot_Size_Y/2)-(focus*(plot_Size_Y/number))-(plot_Size_Y/number)/2)));
    if(moveFlag==0 && key_flag==0){
        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        int x1=mouseEvent2->scenePos().x();
        int y1=mouseEvent2->scenePos().y();

        calcFrame(focus);
        //qDebug()<<QString::number(frame_top);
        float new_x_min=x_min[focus]+(mouseEvent2->scenePos().x()-symbol_size[focus]-frame_left)*(x_max[focus]-x_min[focus])/(frame_size_x);
        float new_y_min=y_max[focus]-(mouseEvent2->scenePos().y()+symbol_size[focus]-frame_top)*(y_max[focus]-y_min[focus])/(frame_size_y);
        float new_x_max=x_min[focus]+(mouseEvent2->scenePos().x()+symbol_size[focus]-frame_left)*(x_max[focus]-x_min[focus])/(frame_size_x);
        float new_y_max=y_max[focus]-(mouseEvent2->scenePos().y()-symbol_size[focus]-frame_top)*(y_max[focus]-y_min[focus])/(frame_size_y);

        // if area bigger than repeat area -> select whole range
        if (new_x_max-new_x_min>=repeat_x2[focus]-repeat_x1[focus]) {
            new_x_min=repeat_x1[focus];
            new_x_max=repeat_x2[focus];
        }
        if (new_y_max-new_y_min>=repeat_y2[focus]-repeat_y1[focus]) {
            new_y_min=repeat_y1[focus];
            new_y_max=repeat_y2[focus];
        }
        // fold x and y range into repeat area
        while(new_x_min>repeat_x2[focus]) new_x_min-=repeat_x2[focus]-repeat_x1[focus];
        while(new_x_min<repeat_x1[focus]) new_x_min+=repeat_x2[focus]-repeat_x1[focus];
        while(new_y_min>repeat_y2[focus]) new_y_min-=repeat_y2[focus]-repeat_y1[focus];
        while(new_y_min<repeat_y1[focus]) new_y_min+=repeat_y2[focus]-repeat_y1[focus];

        while(new_x_max>repeat_x2[focus]) new_x_max-=repeat_x2[focus]-repeat_x1[focus];
        while(new_x_max<repeat_x1[focus]) new_x_max+=repeat_x2[focus]-repeat_x1[focus];
        while(new_y_max>repeat_y2[focus]) new_y_max-=repeat_y2[focus]-repeat_y1[focus];
        while(new_y_max<repeat_y1[focus]) new_y_max+=repeat_y2[focus]-repeat_y1[focus];
        //qDebug()<<QString::number(new_x_min)+" - "+QString::number(new_x_max)+" : "+QString::number(new_y_min)+" - "+QString::number(new_y_max);
        // create QList of all selected points
        select_plot=focus;
        select_x.clear();
        select_y.clear();
        for (int i=0;i<column[focus];i=i+2){
            for (int j=0;j<row[focus];j++) {



                float x_pos=data[focus][j+i*row[focus]];
                float y_pos=data[focus][j+(i+1)*row[focus]];
                if (y_pos*factor_y[focus]<y_min[focus] ||
                        y_pos*factor_y[focus]>y_max[focus] ||
                        x_pos*factor_x[focus]>x_max[focus] ||
                        x_pos*factor_x[focus]<x_min[focus]) {
                    //qDebug()<<"Not in Range"+QString::number(y_min[focus])+" : "+QString::number(y_max[focus])+" : "+QString::number(x_min[focus])+" : "+QString::number(x_max[focus]);
                } else {
                    // fold position to repeat area
                    while(x_pos>repeat_x2[focus]) x_pos-=repeat_x2[focus]-repeat_x1[focus];
                    while(x_pos<repeat_x1[focus]) x_pos+=repeat_x2[focus]-repeat_x1[focus];
                    while(y_pos>repeat_y2[focus]) y_pos-=repeat_y2[focus]-repeat_y1[focus];
                    while(y_pos<repeat_y1[focus]) y_pos+=repeat_y2[focus]-repeat_y1[focus];

                    // if whole area inside repeat area
                    if (new_x_min<new_x_max &&new_y_min<new_y_max){
                        if (x_pos*factor_x[focus]<=new_x_max &&
                                x_pos*factor_x[focus]>=new_x_min &&
                                y_pos*factor_y[focus]>=new_y_min &&
                                y_pos*factor_y[focus]<=new_y_max){

                            select_x.append(i);
                            select_y.append(j);
                        }
                    }
                    // if x area crosses border
                    if (new_x_min>new_x_max && new_y_min<new_y_max){
                        if (x_pos*factor_x[focus]>=new_x_max &&
                                x_pos*factor_x[focus]<=repeat_x2[focus] &&
                                y_pos*factor_y[focus]>=new_y_min &&
                                y_pos*factor_y[focus]<=new_y_max){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_min &&
                                x_pos*factor_x[focus]>repeat_x1[focus] &&
                                y_pos*factor_y[focus]>=new_y_min &&
                                y_pos*factor_y[focus]<=new_y_max){

                            select_x.append(i);
                            select_y.append(j);
                        }

                    }
                    // if y area crosses border
                    if (new_x_min<new_x_max && new_y_min>new_y_max){
                        if (x_pos*factor_x[focus]<=new_x_max &&
                                x_pos*factor_x[focus]>=new_x_min &&
                                y_pos*factor_y[focus]>=new_y_max &&
                                y_pos*factor_y[focus]<=repeat_y2[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_max &&
                                x_pos*factor_x[focus]>=new_x_min &&
                                y_pos*factor_y[focus]<=new_y_min &&
                                y_pos*factor_y[focus]>repeat_y1[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }

                    }

                    // if x and y area crosses border
                    if (new_x_min>new_x_max && new_y_min>new_y_max){
                        if (x_pos*factor_x[focus]>=new_x_max &&
                                x_pos*factor_x[focus]<=repeat_x2[focus] &&
                                y_pos*factor_y[focus]>=new_y_max &&
                                y_pos*factor_y[focus]<=repeat_y2[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_min &&
                                x_pos*factor_x[focus]>repeat_x1[focus] &&
                                y_pos*factor_y[focus]<=new_y_min &&
                                y_pos*factor_y[focus]>repeat_y1[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]>=new_x_max &&
                                x_pos*factor_x[focus]<=repeat_x2[focus] &&
                                y_pos*factor_y[focus]<=new_y_min &&
                                y_pos*factor_y[focus]>repeat_y1[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_min &&
                                x_pos*factor_x[focus]>repeat_x1[focus] &&
                                y_pos*factor_y[focus]>=new_y_max &&
                                y_pos*factor_y[focus]<=repeat_y2[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }

                    }
                 }
            }
        }
        if (select_x.length()==0) gobject->checkSelect(x1,y1);
        if (select_x.length()>0){
            gdata[focus]->setSelected_X(select_x.at(select_x.length()-1));
            gdata[focus]->setSelected_Y(select_y.at(select_x.length()-1));
            QString str="";
            if (select_x.length()>1) str=str+QString::number(select_x.length())+" entries found at location\n\n";
            if (com_mode[focus]>0) {
                for (int i=0;i<select_x.length();i++){
                    if (dat_com[focus][select_y.at(i)+select_x.at(i)*row[focus]].simplified()!="") str=str+dat_com[focus][select_y.at(i)+select_x.at(i)*row[focus]]+"\n";
                    str=str+titel_x[focus]->simplified()+" : "+QString::number(data[focus][select_y.at(i)+select_x.at(i)*row[focus]])+"\n"+titel_y[focus]->simplified()+" : "+QString::number(data[focus][select_y.at(i)+(select_x.at(i)+1)*row[focus]]);
                    if (dat_com[focus][select_y.at(i)+(select_x.at(i)+1)*row[focus]].simplified()!="") str=str+"\n"+dat_com[focus][select_y.at(i)+(select_x.at(i)+1)*row[focus]];
                    if (i<select_x.length()-1) str=str+"\n\n";
                }
            } else {
                for (int i=0;i<select_x.length();i++){
                    str=str+titel_x[focus]->simplified()+" : "+QString::number(data[focus][select_y.at(i)+select_x.at(i)*row[focus]])+"\n"+titel_y[focus]->simplified()+" : "+QString::number(data[focus][select_y.at(i)+(select_x.at(i)+1)*row[focus]]);
                    if (i+1<select_x.length()) str=str+"\n\n";
                }
            }
            QPoint pos;
            pos.setX(mouseEvent2->screenPos().x());
            pos.setY(mouseEvent2->screenPos().y());
            QToolTip::showText(pos,str,diag);
            emit(selected(select_plot,select_x.at(select_x.length()-1),select_y.at(select_y.length()-1)));
        }
        // emit click event in any case
        float x=x_min[focus]+(mouseEvent2->scenePos().x()-frame_left)*(x_max[focus]-x_min[focus])/(frame_size_x);
        float y=y_max[focus]-(mouseEvent2->scenePos().y()-frame_top)*(y_max[focus]-y_min[focus])/(frame_size_y);

        emit(click(select_plot,x,y));


        update();
        diag->update();

    }

    moveFlag=0;

    // Frame is finished : Final Kooridinates-> Zoom in
    if (key_flag==1){
        frame_x2=mouseEvent2->scenePos().x();
        frame_y2=((mouseEvent2->scenePos().y()));
        // zoom
        if (frame_x1>frame_x2){
            int temp=frame_x1;
            frame_x1=frame_x2;
            frame_x2=temp;
        }
        if (frame_y1>frame_y2){
            int temp=frame_y1;
            frame_y1=frame_y2;
            frame_y2=temp;
        }


        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        calcFrame(focus);
        double new_x_min=x_min[focus],new_y_min=y_min[focus],new_x_max=x_max[focus],new_y_max=y_max[focus];
            double fx=(frame_size_x)/(x_max[focus]-x_min[focus]);
            if(fix_x[focus]==false) new_x_min=x_min[focus]+(1.0/fx)*(frame_x1-(frame_left));
            if(fix_x[focus]==false) new_x_max=x_min[focus]+(1.0/fx)*(frame_x2-(frame_left));
            double fy=(frame_size_y)/(y_max[focus]-y_min[focus]);
            if(fix_y[focus]==false) new_y_min=y_max[focus]-(1.0/fy)*(frame_y2-(frame_top));
            if(fix_y[focus]==false) new_y_max=y_max[focus]-(1.0/fy)*(frame_y1-(frame_top));
        if (new_x_min!=new_x_max && new_y_min!=new_y_max){
            // adjust aspect_ratio
            if (lock_aspect[focus]==1) {// x is bound to y
                float res=(new_y_max-new_y_min)/(float)frame_size_y;
                float x=new_x_min+(new_x_max-new_x_min)/2.0;
                new_x_min=x-((float)frame_size_x*res)/2.0;
                new_x_max=x+((float)frame_size_x*res)/2.0;
            }
            if (lock_aspect[focus]==2) {// y is bound to x
                float res=(new_x_max-new_x_min)/(float)frame_size_x;
                float y=new_y_min+(new_y_max-new_y_min)/2.0;
                new_y_min=y-((float)frame_size_y*res)/2.0;
                new_y_max=y+((float)frame_size_y*res)/2.0;
            }




            if (new_x_min>=limit_x1[focus]&&new_x_max<=limit_x2[focus]&&new_y_min>=limit_y1[focus]&&new_y_max<=limit_y2[focus]){
                ggrid[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
                gdata[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
            }
        }
        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

        key_flag=0;
    }
    // Frame is finished : Final Kooridinates-> create List of all points and sent it to parent
    if (key_flag==2){
        frame_x2=mouseEvent2->scenePos().x();
        frame_y2=((mouseEvent2->scenePos().y()+plot_Size_Y/2)-(focus*(plot_Size_Y/number))-(plot_Size_Y/number)/2);
        // zoom
        if (frame_x1>frame_x2){
            int temp=frame_x1;
            frame_x1=frame_x2;
            frame_x2=temp;
        }
        if (frame_y1>frame_y2){
            int temp=frame_y1;
            frame_y1=frame_y2;
            frame_y2=temp;
        }


        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        calcFrame(focus);
        double new_x_min=x_min[focus],new_y_min=y_min[focus],new_x_max=x_max[focus],new_y_max=y_max[focus];
        double fx=(frame_size_x)/(x_max[focus]-x_min[focus]);
        new_x_min=x_min[focus]+(1.0/fx)*(frame_x1-(frame_left));
        new_x_max=x_min[focus]+(1.0/fx)*(frame_x2-(frame_left));
        double fy=(frame_size_y)/(y_max[focus]-y_min[focus]);
        new_y_min=y_max[focus]-(1.0/fy)*(frame_y2-(frame_top));
        new_y_max=y_max[focus]-(1.0/fy)*(frame_y1-(frame_top));
        // if area bigger than repeat area -> select whole range
        if (new_x_max-new_x_min>=repeat_x2[focus]-repeat_x1[focus]) {
            new_x_min=repeat_x1[focus];
            new_x_max=repeat_x2[focus];
        }
        if (new_y_max-new_y_min>=repeat_y2[focus]-repeat_y1[focus]) {
            new_y_min=repeat_y1[focus];
            new_y_max=repeat_y2[focus];
        }
        // fold x and y range into repeat area
        while(new_x_min>repeat_x2[focus]) new_x_min-=repeat_x2[focus]-repeat_x1[focus];
        while(new_x_min<repeat_x1[focus]) new_x_min+=repeat_x2[focus]-repeat_x1[focus];
        while(new_y_min>repeat_y2[focus]) new_y_min-=repeat_y2[focus]-repeat_y1[focus];
        while(new_y_min<repeat_y1[focus]) new_y_min+=repeat_y2[focus]-repeat_y1[focus];

        while(new_x_max>repeat_x2[focus]) new_x_max-=repeat_x2[focus]-repeat_x1[focus];
        while(new_x_max<repeat_x1[focus]) new_x_max+=repeat_x2[focus]-repeat_x1[focus];
        while(new_y_max>repeat_y2[focus]) new_y_max-=repeat_y2[focus]-repeat_y1[focus];
        while(new_y_max<repeat_y1[focus]) new_y_max+=repeat_y2[focus]-repeat_y1[focus];
        // create QList of all selected points
        select_plot=focus;
        select_x.clear();
        select_y.clear();
        for (int i=0;i<column[focus];i=i+2){
            for (int j=0;j<row[focus];j++) {



                float x_pos=data[focus][j+i*row[focus]];
                float y_pos=data[focus][j+(i+1)*row[focus]];
                if (y_pos*factor_y[focus]<y_min[focus] ||
                        y_pos*factor_y[focus]>y_max[focus] ||
                        x_pos*factor_x[focus]>x_max[focus] ||
                        x_pos*factor_x[focus]<x_min[focus]) {

                } else {


                    // fold position to repeat area
                    while(x_pos>repeat_x2[focus]) x_pos-=repeat_x2[focus]-repeat_x1[focus];
                    while(x_pos<repeat_x1[focus]) x_pos+=repeat_x2[focus]-repeat_x1[focus];
                    while(y_pos>repeat_y2[focus]) y_pos-=repeat_y2[focus]-repeat_y1[focus];
                    while(y_pos<repeat_y1[focus]) y_pos+=repeat_y2[focus]-repeat_y1[focus];

                    // if whole area inside repeat area
                    if (new_x_min<new_x_max &&new_y_min<new_y_max){
                        if (x_pos*factor_x[focus]<=new_x_max &&
                                x_pos*factor_x[focus]>=new_x_min &&
                                y_pos*factor_y[focus]>=new_y_min &&
                                y_pos*factor_y[focus]<=new_y_max){

                            select_x.append(i);
                            select_y.append(j);
                        }
                    }
                    // if x area crosses border
                    if (new_x_min>new_x_max && new_y_min<new_y_max){
                        if (x_pos*factor_x[focus]>=new_x_max &&
                                x_pos*factor_x[focus]<=repeat_x2[focus] &&
                                y_pos*factor_y[focus]>=new_y_min &&
                                y_pos*factor_y[focus]<=new_y_max){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_min &&
                                x_pos*factor_x[focus]>repeat_x1[focus] &&
                                y_pos*factor_y[focus]>=new_y_min &&
                                y_pos*factor_y[focus]<=new_y_max){

                            select_x.append(i);
                            select_y.append(j);
                        }

                    }
                    // if y area crosses border
                    if (new_x_min<new_x_max && new_y_min>new_y_max){
                        if (x_pos*factor_x[focus]<=new_x_max &&
                                x_pos*factor_x[focus]>=new_x_min &&
                                y_pos*factor_y[focus]>=new_y_max &&
                                y_pos*factor_y[focus]<=repeat_y2[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_max &&
                                x_pos*factor_x[focus]>=new_x_min &&
                                y_pos*factor_y[focus]<=new_y_min &&
                                y_pos*factor_y[focus]>repeat_y1[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }

                    }

                    // if x and y area crosses border
                    if (new_x_min>new_x_max && new_y_min>new_y_max){
                        if (x_pos*factor_x[focus]>=new_x_max &&
                                x_pos*factor_x[focus]<=repeat_x2[focus] &&
                                y_pos*factor_y[focus]>=new_y_max &&
                                y_pos*factor_y[focus]<=repeat_y2[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_min &&
                                x_pos*factor_x[focus]>repeat_x1[focus] &&
                                y_pos*factor_y[focus]<=new_y_min &&
                                y_pos*factor_y[focus]>repeat_y1[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]>=new_x_max &&
                                x_pos*factor_x[focus]<=repeat_x2[focus] &&
                                y_pos*factor_y[focus]<=new_y_min &&
                                y_pos*factor_y[focus]>repeat_y1[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }
                        if (x_pos*factor_x[focus]<=new_x_min &&
                                x_pos*factor_x[focus]>repeat_x1[focus] &&
                                y_pos*factor_y[focus]>=new_y_max &&
                                y_pos*factor_y[focus]<=repeat_y2[focus]){

                            select_x.append(i);
                            select_y.append(j);
                        }

                    }
                }
            }
        }
        emit (selection(select_plot,select_x,select_y));
        key_flag=0;
    }
    gdata[focus]->setRect(frame_x1,frame_x2,frame_y1,frame_y2,0);
    update();
}

// Realizes Drag and Zoom-Frame
void EditGraph_Graph::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent3){
    QToolTip::hideText();
    // Allow Drag of View
    if (mouseEvent3->buttons()==Qt::LeftButton && key_flag==0){

        moveFlag=1;

        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        calcFrame(focus);
        double new_x_min=x_min[focus],new_y_min=y_min[focus],new_x_max=x_max[focus],new_y_max=y_max[focus];


            double fx=(frame_size_x)/(x_max[focus]-x_min[focus]);
            if(fix_x[focus]==false)new_x_min=x_min[focus]+(1.0/fx)*(mouseEvent3->lastScreenPos().x()-mouseEvent3->screenPos().x());
            if(fix_x[focus]==false)new_x_max=x_max[focus]+(1.0/fx)*(mouseEvent3->lastScreenPos().x()-mouseEvent3->screenPos().x());
            double fy=(frame_size_y)/(y_max[focus]-y_min[focus]);
            if(fix_y[focus]==false)new_y_min=y_min[focus]-(1.0/fy)*(mouseEvent3->lastScreenPos().y()-mouseEvent3->screenPos().y());
            if(fix_y[focus]==false)new_y_max=y_max[focus]-(1.0/fy)*(mouseEvent3->lastScreenPos().y()-mouseEvent3->screenPos().y());

        //qDebug() << QString::number(new_x_min)+":"+QString::number(new_x_max);
        //qDebug() << QString::number(new_y_min)+":"+QString::number(new_y_max);
            // check folding
            if (fold_x[focus]==1 && repeat_x2[focus]-new_x_min < new_x_max-repeat_x2[focus] && new_x_max>repeat_x2[focus]) {
                new_x_min=new_x_min-(repeat_x2[focus]-repeat_x1[focus]);
                new_x_max=new_x_max-(repeat_x2[focus]-repeat_x1[focus]);
            }
            if (fold_x[focus]==1 && new_x_min<repeat_x1[focus] && new_x_max-repeat_x1[focus]<repeat_x1[focus]-new_x_min) {
                new_x_min=new_x_min+(repeat_x2[focus]-repeat_x1[focus]);
                new_x_max=new_x_max+(repeat_x2[focus]-repeat_x1[focus]);
            }
            if (fold_y[focus]==1 && repeat_y2[focus]-new_y_min < new_y_max-repeat_y2[focus] && new_y_max>repeat_y2[focus]) {
                new_y_min=new_y_min-(repeat_y2[focus]-repeat_y1[focus]);
                new_y_max=new_y_max-(repeat_y2[focus]-repeat_y1[focus]);
            }
            if (fold_y[focus]==1 && new_y_min<repeat_y1[focus] && new_y_max-repeat_y1[focus]<repeat_y1[focus]-new_y_min) {
                new_y_min=new_y_min+(repeat_y2[focus]-repeat_y1[focus]);
                new_y_max=new_y_max+(repeat_y2[focus]-repeat_y1[focus]);
            }

            // check for limits
            if (new_x_min<limit_x1[focus]) {
                new_x_max=new_x_max-(new_x_min-limit_x1[focus]);
                if (new_x_max>limit_x2[focus]) new_x_max=limit_x2[focus];
                new_x_min=limit_x1[focus];
            }
            if (new_x_max>limit_x2[focus]){
                new_x_min=new_x_min+(limit_x2[focus]-new_x_max);
                if (new_x_min<limit_x1[focus]) new_x_min=limit_x1[focus];
                new_x_max=limit_x2[focus];
            }
            if (new_y_min<limit_y1[focus]){
                new_y_max=new_y_max-(new_y_min-limit_y1[focus]);
                if (new_y_max>limit_y2[focus]) new_y_max=limit_y2[focus];
                new_y_min=limit_y1[focus];
            }
            if (new_y_max>limit_y2[focus]){
                new_y_min=new_y_min+(limit_y2[focus]-new_y_max);
                if (new_y_min<limit_y1[focus]) new_y_min=limit_y1[focus];
                new_y_max=limit_y2[focus];
            }

            if (new_x_min>=limit_x1[focus]&&new_x_max<=limit_x2[focus]&&new_y_min>=limit_y1[focus]&&new_y_max<=limit_y2[focus]){
                ggrid[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
                gdata[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
            }
        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

        update();
    }
    // Allow to draw a frame if STRG is Pressed
    if (mouseEvent3->buttons()==Qt::LeftButton && key_flag==1){
        // draw Frame
        int x1=frame_x1;
        int x2=mouseEvent3->scenePos().x();
        int y1=frame_y1;
        int y2=((mouseEvent3->scenePos().y()));
        calcFrame(focus);

        if (fix_x[focus]==true) {
            x1=frame_left;
            x2=frame_right;
        }
        if (fix_y[focus]==true) {
            y1=frame_top-(focus*(plot_Size_Y/number))+plot_Size_Y/4;
            y2=frame_bottom-(focus*(plot_Size_Y/number))+plot_Size_Y/4;
        }

        // adjust aspect_ratio
        if (lock_aspect[focus]==1) {// x is bound to y
            x2=x1+(y2-y1)/2*frame_size_x/frame_size_y;
            x1=x1-(y2-y1)/2*frame_size_x/frame_size_y;
        }
        if (lock_aspect[focus]==2) {// y is bound to x
            y2=y1+(x2-x1)/2*frame_size_y/frame_size_x;
            y1=y1-(x2-x1)/2*frame_size_y/frame_size_x;
        }
        qDebug()<<QString::number(y1)+":"+QString::number(y2);
        gdata[focus]->setRect(x1,x2,y1,y2,1);
        update();
    }
    // Allow to draw a frame if ALT is Pressed
    if (mouseEvent3->buttons()==Qt::LeftButton && key_flag==2){
        // draw Frame
        int x1=frame_x1;
        int x2=mouseEvent3->scenePos().x();
        int y1=frame_y1;
        int y2=((mouseEvent3->scenePos().y()));
        calcFrame(focus);
        if (fix_x[focus]==true) {
            x1=frame_left;
            x2=frame_right;
        }
        if (fix_y[focus]==true) {
            y1=frame_top-(focus*(plot_Size_Y/number))+plot_Size_Y/4;
            y2=frame_bottom-(focus*(plot_Size_Y/number))+plot_Size_Y/4;
        }
        gdata[focus]->setRect(x1,x2,y1,y2,2);
        update();
    }

}

// Get Seleted Point(x)
int EditGraph_Graph::getSelected_X(){
    return gdata[focus]->getSelected_X();
}

// Get Seleted Point(y)
int EditGraph_Graph::getSelected_Y(){
    return gdata[focus]->getSelected_Y();
}
// Get Seleted Point(x)
int EditGraph_Graph::getSelected_X(int n){
    return gdata[n]->getSelected_X();
}

// Get Seleted Point(y)
int EditGraph_Graph::getSelected_Y(int n){
    return gdata[n]->getSelected_Y();
}

// Set Seleted Point(x)
void EditGraph_Graph::setSelected_X(int x){
    gdata[0]->setSelected_X(x);
}

// Set Seleted Point(y)
void EditGraph_Graph::setSelected_Y(int y){
    gdata[0]->setSelected_Y(y);
}
// Set Seleted Point(x)
void EditGraph_Graph::setSelected_X(int x,int n){
    gdata[n]->setSelected_X(x);
}

// Set Seleted Point(y)
void EditGraph_Graph::setSelected_Y(int y,int n){
    gdata[n]->setSelected_Y(y);
}

// MouseWheel for Changing Zoom
void EditGraph_Graph::wheelEvent(QGraphicsSceneWheelEvent *mouseEvent4){
    focus=(mouseEvent4->scenePos().y()+plot_Size_Y/2)/(plot_Size_Y/number);
    x_min[focus]=gdata[focus]->get_x_min();
    x_max[focus]=gdata[focus]->get_x_max();
    y_min[focus]=gdata[focus]->get_y_min();
    y_max[focus]=gdata[focus]->get_y_max();

    double new_x_min=x_min[focus],new_y_min=y_min[focus],new_x_max=x_max[focus],new_y_max=y_max[focus];

    if (mouseEvent4->delta()/30>0) {
        if (fix_x[focus]==false) new_x_min=x_min[focus]+(x_max[focus]-x_min[focus])*0.1;
        if (fix_x[focus]==false) new_x_max=x_max[focus]-(x_max[focus]-x_min[focus])*0.1;
        if (fix_y[focus]==false) new_y_min=y_min[focus]+(y_max[focus]-y_min[focus])*0.1;
        if (fix_y[focus]==false) new_y_max=y_max[focus]-(y_max[focus]-y_min[focus])*0.1;
    }
    if (mouseEvent4->delta()/30<0) {
        if (fix_x[focus]==false) new_x_min=x_min[focus]-(x_max[focus]-x_min[focus])*0.1;
        if (fix_x[focus]==false) new_x_max=x_max[focus]+(x_max[focus]-x_min[focus])*0.1;
        if (fix_y[focus]==false) new_y_min=y_min[focus]-(y_max[focus]-y_min[focus])*0.1;
        if (fix_y[focus]==false) new_y_max=y_max[focus]+(y_max[focus]-y_min[focus])*0.1;
    }



    // check for limits
    if (new_x_min<limit_x1[focus]) {
        new_x_max=new_x_max-(new_x_min-limit_x1[focus]);
        if (new_x_max>limit_x2[focus]) new_x_max=limit_x2[focus];
        new_x_min=limit_x1[focus];
    }
    if (new_x_max>limit_x2[focus]){
        new_x_min=new_x_min+(limit_x2[focus]-new_x_max);
        if (new_x_min<limit_x1[focus]) new_x_min=limit_x1[focus];
        new_x_max=limit_x2[focus];
    }
    if (new_y_min<limit_y1[focus]){
        new_y_max=new_y_max-(new_y_min-limit_y1[focus]);
        if (new_y_max>limit_y2[focus]) new_y_max=limit_y2[focus];
        new_y_min=limit_y1[focus];
    }
    if (new_y_max>limit_y2[focus]){
        new_y_min=new_y_min+(limit_y2[focus]-new_y_max);
        if (new_y_min<limit_y1[focus]) new_y_min=limit_y1[focus];
        new_y_max=limit_y2[focus];
    }

    // check folding
    if (fold_x[focus]==1 && repeat_x2[focus]-new_x_min < new_x_max-repeat_x2[focus] && new_x_max>repeat_x2[focus]) {
        new_x_min=new_x_min-(repeat_x2[focus]-repeat_x1[focus]);
        new_x_max=new_x_max-(repeat_x2[focus]-repeat_x1[focus]);
    }
    if (fold_x[focus]==1 && new_x_min<repeat_x1[focus] && new_x_max-repeat_x1[focus]<repeat_x1[focus]-new_x_min) {
        new_x_min=new_x_min+(repeat_x2[focus]-repeat_x1[focus]);
        new_x_max=new_x_max+(repeat_x2[focus]-repeat_x1[focus]);
    }
    if (fold_y[focus]==1 && repeat_y2[focus]-new_y_min < new_y_max-repeat_y2[focus] && new_y_max>repeat_y2[focus]) {
        new_y_min=new_y_min-(repeat_y2[focus]-repeat_y1[focus]);
        new_y_max=new_y_max-(repeat_y2[focus]-repeat_y1[focus]);
    }
    if (fold_y[focus]==1 && new_y_min<repeat_y1[focus] && new_y_max-repeat_y1[focus]<repeat_y1[focus]-new_y_min) {
        new_y_min=new_y_min+(repeat_y2[focus]-repeat_y1[focus]);
        new_y_max=new_y_max+(repeat_y2[focus]-repeat_y1[focus]);
    }
    // adjust aspect_ratio
    if (lock_aspect[focus]==1) {// x is bound to y
        float res=(new_y_max-new_y_min)/(float)frame_size_y;
        float x=new_x_min+(new_x_max-new_x_min)/2.0;
        new_x_min=x-((float)frame_size_x*res)/2.0;
        new_x_max=x+((float)frame_size_x*res)/2.0;
    }
    if (lock_aspect[focus]==2) {// y is bound to x
        float res=(new_x_max-new_x_min)/(float)frame_size_x;
        float y=new_y_min+(new_y_max-new_y_min)/2.0;
        new_y_min=y-((float)frame_size_y*res)/2.0;
        new_y_max=y+((float)frame_size_y*res)/2.0;
    }




    if (new_x_min>=limit_x1[focus] && new_x_max<=limit_x2[focus] && new_y_min>=limit_y1[focus] && new_y_max<=limit_y2[focus]){
        ggrid[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
        gdata[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
    }
    x_min[focus]=gdata[focus]->get_x_min();
    x_max[focus]=gdata[focus]->get_x_max();
    y_min[focus]=gdata[focus]->get_y_min();
    y_max[focus]=gdata[focus]->get_y_max();
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

    update();

}


// For external usage
void EditGraph_Graph::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent5){

    x_min[focus]=gdata[focus]->get_x_min();
    x_max[focus]=gdata[focus]->get_x_max();
    y_min[focus]=gdata[focus]->get_y_min();
    y_max[focus]=gdata[focus]->get_y_max();
    calcFrame(focus);
    float new_x_min=x_min[focus]+(mouseEvent5->scenePos().x()-symbol_size[focus]-frame_left)*(x_max[focus]-x_min[focus])/(frame_size_x);
    float new_y_min=y_max[focus]-((((mouseEvent5->scenePos().y()+symbol_size[focus]+plot_Size_Y/2)-(focus*(plot_Size_Y/number))-(plot_Size_Y/number)/2))-frame_top)*(y_max[focus]-y_min[focus])/(frame_size_y);
    float new_x_max=x_min[focus]+(mouseEvent5->scenePos().x()+symbol_size[focus]-frame_left)*(x_max[focus]-x_min[focus])/(frame_size_x);
    float new_y_max=y_max[focus]-((((mouseEvent5->scenePos().y()-symbol_size[focus]+plot_Size_Y/2)-(focus*(plot_Size_Y/number))-(plot_Size_Y/number)/2))-frame_top)*(y_max[focus]-y_min[focus])/(frame_size_y);
    // if area bigger than repeat area -> select whole range
    if (new_x_max-new_x_min>=repeat_x2[focus]-repeat_x1[focus]) {
        new_x_min=repeat_x1[focus];
        new_x_max=repeat_x2[focus];
    }
    if (new_y_max-new_y_min>=repeat_y2[focus]-repeat_y1[focus]) {
        new_y_min=repeat_y1[focus];
        new_y_max=repeat_y2[focus];
    }
    // fold x and y range into repeat area
    while(new_x_min>repeat_x2[focus]) new_x_min-=repeat_x2[focus]-repeat_x1[focus];
    while(new_x_min<repeat_x1[focus]) new_x_min+=repeat_x2[focus]-repeat_x1[focus];
    while(new_y_min>repeat_y2[focus]) new_y_min-=repeat_y2[focus]-repeat_y1[focus];
    while(new_y_min<repeat_y1[focus]) new_y_min+=repeat_y2[focus]-repeat_y1[focus];

    while(new_x_max>repeat_x2[focus]) new_x_max-=repeat_x2[focus]-repeat_x1[focus];
    while(new_x_max<repeat_x1[focus]) new_x_max+=repeat_x2[focus]-repeat_x1[focus];
    while(new_y_max>repeat_y2[focus]) new_y_max-=repeat_y2[focus]-repeat_y1[focus];
    while(new_y_max<repeat_y1[focus]) new_y_max+=repeat_y2[focus]-repeat_y1[focus];
    //qDebug()<<QString::number(new_x_min)+" - "+QString::number(new_x_max)+" : "+QString::number(new_y_min)+" - "+QString::number(new_y_max);
    // create QList of all selected points
    select_plot=focus;
    select_x.clear();
    select_y.clear();
    for (int i=0;i<column[focus];i=i+2){
        for (int j=0;j<row[focus];j++) {



            float x_pos=data[focus][j+i*row[focus]];
            float y_pos=data[focus][j+(i+1)*row[focus]];
            if (y_pos*factor_y[focus]<y_min[focus] ||
                    y_pos*factor_y[focus]>y_max[focus] ||
                    x_pos*factor_x[focus]>x_max[focus] ||
                    x_pos*factor_x[focus]<x_min[focus]) {

            } else {
                // fold position to repeat area
                while(x_pos>repeat_x2[focus]) x_pos-=repeat_x2[focus]-repeat_x1[focus];
                while(x_pos<repeat_x1[focus]) x_pos+=repeat_x2[focus]-repeat_x1[focus];
                while(y_pos>repeat_y2[focus]) y_pos-=repeat_y2[focus]-repeat_y1[focus];
                while(y_pos<repeat_y1[focus]) y_pos+=repeat_y2[focus]-repeat_y1[focus];

                // if whole area inside repeat area
                if (new_x_min<new_x_max &&new_y_min<new_y_max){
                    if (x_pos*factor_x[focus]<=new_x_max &&
                            x_pos*factor_x[focus]>=new_x_min &&
                            y_pos*factor_y[focus]>=new_y_min &&
                            y_pos*factor_y[focus]<=new_y_max){

                        select_x.append(i);
                        select_y.append(j);
                    }
                }
                // if x area crosses border
                if (new_x_min>new_x_max && new_y_min<new_y_max){
                    if (x_pos*factor_x[focus]>=new_x_max &&
                            x_pos*factor_x[focus]<=repeat_x2[focus] &&
                            y_pos*factor_y[focus]>=new_y_min &&
                            y_pos*factor_y[focus]<=new_y_max){

                        select_x.append(i);
                        select_y.append(j);
                    }
                    if (x_pos*factor_x[focus]<=new_x_min &&
                            x_pos*factor_x[focus]>repeat_x1[focus] &&
                            y_pos*factor_y[focus]>=new_y_min &&
                            y_pos*factor_y[focus]<=new_y_max){

                        select_x.append(i);
                        select_y.append(j);
                    }

                }
                // if y area crosses border
                if (new_x_min<new_x_max && new_y_min>new_y_max){
                    if (x_pos*factor_x[focus]<=new_x_max &&
                            x_pos*factor_x[focus]>=new_x_min &&
                            y_pos*factor_y[focus]>=new_y_max &&
                            y_pos*factor_y[focus]<=repeat_y2[focus]){

                        select_x.append(i);
                        select_y.append(j);
                    }
                    if (x_pos*factor_x[focus]<=new_x_max &&
                            x_pos*factor_x[focus]>=new_x_min &&
                            y_pos*factor_y[focus]<=new_y_min &&
                            y_pos*factor_y[focus]>repeat_y1[focus]){

                        select_x.append(i);
                        select_y.append(j);
                    }

                }

                // if x and y area crosses border
                if (new_x_min>new_x_max && new_y_min>new_y_max){
                    if (x_pos*factor_x[focus]>=new_x_max &&
                            x_pos*factor_x[focus]<=repeat_x2[focus] &&
                            y_pos*factor_y[focus]>=new_y_max &&
                            y_pos*factor_y[focus]<=repeat_y2[focus]){

                        select_x.append(i);
                        select_y.append(j);
                    }
                    if (x_pos*factor_x[focus]<=new_x_min &&
                            x_pos*factor_x[focus]>repeat_x1[focus] &&
                            y_pos*factor_y[focus]<=new_y_min &&
                            y_pos*factor_y[focus]>repeat_y1[focus]){

                        select_x.append(i);
                        select_y.append(j);
                    }
                    if (x_pos*factor_x[focus]>=new_x_max &&
                            x_pos*factor_x[focus]<=repeat_x2[focus] &&
                            y_pos*factor_y[focus]<=new_y_min &&
                            y_pos*factor_y[focus]>repeat_y1[focus]){

                        select_x.append(i);
                        select_y.append(j);
                    }
                    if (x_pos*factor_x[focus]<=new_x_min &&
                            x_pos*factor_x[focus]>repeat_x1[focus] &&
                            y_pos*factor_y[focus]>=new_y_max &&
                            y_pos*factor_y[focus]<=repeat_y2[focus]){

                        select_x.append(i);
                        select_y.append(j);
                    }

                }
            }
        }
    }
    emit (selection(select_plot,select_x,select_y));

}

// Check if STRG is Pressed for starting to drawing a frame
void EditGraph_Graph::keyPressEvent(QKeyEvent *event){
    //qDebug() << "Pressed :"+QString::number(event->key());
    if (event->key()==Qt::Key_Control){
        //qDebug() << "Control";
        key_flag=1;
    }
    if (event->key()==Qt::Key_Alt){
        //qDebug() << "Control";
        key_flag=2;
    }
    if (event->key()==Qt::Key_Plus || event->key()==Qt::Key_Minus){

        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();

        double new_x_min=x_min[focus],new_y_min=y_min[focus],new_x_max=x_max[focus],new_y_max=y_max[focus];

        if (event->key()==Qt::Key_Plus) {
            if (fix_x[focus]==false) new_x_min=x_min[focus]+(x_max[focus]-x_min[focus])*0.1;
            if (fix_x[focus]==false) new_x_max=x_max[focus]-(x_max[focus]-x_min[focus])*0.1;
            if (fix_y[focus]==false) new_y_min=y_min[focus]+(y_max[focus]-y_min[focus])*0.1;
            if (fix_y[focus]==false) new_y_max=y_max[focus]-(y_max[focus]-y_min[focus])*0.1;
        }
        if (event->key()==Qt::Key_Minus) {
            if (fix_x[focus]==false) new_x_min=x_min[focus]-(x_max[focus]-x_min[focus])*0.1;
            if (fix_x[focus]==false) new_x_max=x_max[focus]+(x_max[focus]-x_min[focus])*0.1;
            if (fix_y[focus]==false) new_y_min=y_min[focus]-(y_max[focus]-y_min[focus])*0.1;
            if (fix_y[focus]==false) new_y_max=y_max[focus]+(y_max[focus]-y_min[focus])*0.1;
        }



        // check for limits
        if (new_x_min<limit_x1[focus]) {
            new_x_max=new_x_max-(new_x_min-limit_x1[focus]);
            if (new_x_max>limit_x2[focus]) new_x_max=limit_x2[focus];
            new_x_min=limit_x1[focus];
        }
        if (new_x_max>limit_x2[focus]){
            new_x_min=new_x_min+(limit_x2[focus]-new_x_max);
            if (new_x_min<limit_x1[focus]) new_x_min=limit_x1[focus];
            new_x_max=limit_x2[focus];
        }
        if (new_y_min<limit_y1[focus]){
            new_y_max=new_y_max-(new_y_min-limit_y1[focus]);
            if (new_y_max>limit_y2[focus]) new_y_max=limit_y2[focus];
            new_y_min=limit_y1[focus];
        }
        if (new_y_max>limit_y2[focus]){
            new_y_min=new_y_min+(limit_y2[focus]-new_y_max);
            if (new_y_min<limit_y1[focus]) new_y_min=limit_y1[focus];
            new_y_max=limit_y2[focus];
        }

        // check folding
        if (fold_x[focus]==1 && repeat_x2[focus]-new_x_min < new_x_max-repeat_x2[focus] && new_x_max>repeat_x2[focus]) {
            new_x_min=new_x_min-(repeat_x2[focus]-repeat_x1[focus]);
            new_x_max=new_x_max-(repeat_x2[focus]-repeat_x1[focus]);
        }
        if (fold_x[focus]==1 && new_x_min<repeat_x1[focus] && new_x_max-repeat_x1[focus]<repeat_x1[focus]-new_x_min) {
            new_x_min=new_x_min+(repeat_x2[focus]-repeat_x1[focus]);
            new_x_max=new_x_max+(repeat_x2[focus]-repeat_x1[focus]);
        }
        if (fold_y[focus]==1 && repeat_y2[focus]-new_y_min < new_y_max-repeat_y2[focus] && new_y_max>repeat_y2[focus]) {
            new_y_min=new_y_min-(repeat_y2[focus]-repeat_y1[focus]);
            new_y_max=new_y_max-(repeat_y2[focus]-repeat_y1[focus]);
        }
        if (fold_y[focus]==1 && new_y_min<repeat_y1[focus] && new_y_max-repeat_y1[focus]<repeat_y1[focus]-new_y_min) {
            new_y_min=new_y_min+(repeat_y2[focus]-repeat_y1[focus]);
            new_y_max=new_y_max+(repeat_y2[focus]-repeat_y1[focus]);
        }
        // adjust aspect_ratio
        if (lock_aspect[focus]==1) {// x is bound to y
            float res=(new_y_max-new_y_min)/(float)frame_size_y;
            float x=new_x_min+(new_x_max-new_x_min)/2.0;
            new_x_min=x-((float)frame_size_x*res)/2.0;
            new_x_max=x+((float)frame_size_x*res)/2.0;
        }
        if (lock_aspect[focus]==2) {// y is bound to x
            float res=(new_x_max-new_x_min)/(float)frame_size_x;
            float y=new_y_min+(new_y_max-new_y_min)/2.0;
            new_y_min=y-((float)frame_size_y*res)/2.0;
            new_y_max=y+((float)frame_size_y*res)/2.0;
        }




        if (new_x_min>=limit_x1[focus] && new_x_max<=limit_x2[focus] && new_y_min>=limit_y1[focus] && new_y_max<=limit_y2[focus]){
            ggrid[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
            gdata[focus]->setView(new_x_min,new_x_max,new_y_min,new_y_max);
        }
        x_min[focus]=gdata[focus]->get_x_min();
        x_max[focus]=gdata[focus]->get_x_max();
        y_min[focus]=gdata[focus]->get_y_min();
        y_max[focus]=gdata[focus]->get_y_max();
        gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);

        update();
    }
}

// Check if STRG is Pressed for stopping drawing a frame
void EditGraph_Graph::keyReleaseEvent(QKeyEvent *event){
    //qDebug() << "Released :"+QString::number(event->key());
    if (event->key()==Qt::Key_Control){
        //qDebug() << "Control";
        key_flag=0;
    }
    if (event->key()==Qt::Key_Alt){
        //qDebug() << "Control";
        key_flag=0;
    }

}

void EditGraph_Graph::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

    QMenu menu;
    menu.addAction("Auto-Resize Graph");
    //menu.addAction("Show in Editor");
    //menu.addAction("Edit Graph");
    menu.addSeparator();
    //menu.addAction("Settings");
    if (error[focus]>0 && error[focus]==1) menu.addAction("Show Y-Error");
    if (error[focus]>0 && error[focus]==2) menu.addAction("Show X-Y Error");
    if (error[focus]>0 && error[focus]==3) menu.addAction("Hide Error");
    if (error[focus]>0 && error[focus]==4) menu.addAction("Show X-Error");
    if (com_mode[focus]>0 && com_hide[focus]==0) menu.addAction("Hide Comments");
    if (com_mode[focus]>0 && com_hide[focus]==1) menu.addAction("Show Comments");
    menu.addSeparator();
    //menu.addAction("Save as Image");
    QAction *a = menu.exec(event->screenPos());

    if (a)
    {
        if (a->iconText()=="Auto-Resize Graph"){
            this->autoSize(focus);
        }



        if (a->iconText()=="Show X-Error"){
            error[focus]=1;
            gdata[focus]->setError(data_err[focus],error[focus]);
            update();
        }
        if (a->iconText()=="Show Y-Error"){
            error[focus]=2;
            gdata[focus]->setError(data_err[focus],error[focus]);
            update();
        }
        if (a->iconText()=="Show X-Y Error"){
            error[focus]=3;
            gdata[focus]->setError(data_err[focus],error[focus]);
            update();
        }
        if (a->iconText()=="Hide Error"){
            error[focus]=4;
            gdata[focus]->setError(data_err[focus],error[focus]);
            update();
        }


        if (a->iconText()=="Hide Comments"){
            com_hide[focus]=1;
            gdata[focus]->setComHide(com_hide[focus]);
            update();
        }
        if (a->iconText()=="Show Comments"){
            com_hide[focus]=0;
            gdata[focus]->setComHide(com_hide[focus]);
            update();
        }


    }
}

// addObject Parameters
void EditGraph_Graph::addLine(float x1,float y1,int n1,float x2,float y2,int n2,QColor c,int w,Qt::PenStyle s){
    gobject->addLine(x1,y1,n1,x2,y2,n2,c,w,s);
}

void EditGraph_Graph::setAddMode(int i){
    gobject->setParameters(plot_Size_X,plot_Size_Y,number,&textsize[0],&titelsize[0],&margin[0],&x_min[0],&y_min[0],&x_max[0],&y_max[0],&factor_x[0],&factor_y[0]);
    gobject->setAxisType(&x_axis_type[0],&y_axis_type[0]);

    gobject->setAddMode(i);
}

void EditGraph_Graph::addLineClear(){
    gobject->addLineClear();
}
void EditGraph_Graph::setAxisType(int x,int y){
    x_axis_type[0]=x;
    y_axis_type[0]=y;
    ggrid[0]->setAxisType(x_axis_type[0],y_axis_type[0]);
    gdata[0]->setAxisType(x_axis_type[0],y_axis_type[0]);
    gobject->setAxisType(&x_axis_type[0],&y_axis_type[0]);
}
// 0 means left/bottom : 1 means right/top
void EditGraph_Graph::setAxisType(int x,int y,int i){
    x_axis_type[i]=x;
    y_axis_type[i]=y;

    ggrid[i]->setAxisType(x_axis_type[i],y_axis_type[i]);
    gdata[i]->setAxisType(x_axis_type[i],y_axis_type[i]);
    gobject->setAxisType(&x_axis_type[0],&y_axis_type[0]);

}

int EditGraph_Graph::getSelectLine(){
    return gobject->getSelect();
}

void EditGraph_Graph::calcFrame(int i){
    if (y_axis_type[i]==0){
        frame_right=plot_Size_X/2-margin[i]-colorLegend_size_x[i];
        frame_left=-plot_Size_X/2+2.2*textsize[i]+margin[i]+5;
        frame_size_x=frame_right-frame_left;
    }
    if (y_axis_type[i]==1){
        frame_right=plot_Size_X/2-margin[i]-2.2*textsize[i]-colorLegend_size_x[i]-5;
        frame_left=-plot_Size_X/2+margin[i];
        frame_size_x=frame_right-frame_left;
    }
    if (y_axis_type[i]==2){
        frame_right=plot_Size_X/2-margin[i]-2.2*textsize[i]-colorLegend_size_x[i]-5;
        frame_left=-plot_Size_X/2+margin[i]+2*textsize[i]+5;
        frame_size_x=frame_right-frame_left;
    }
    if (y_axis_type[i]==3){
        frame_right=plot_Size_X/2-margin[i]-colorLegend_size_x[i];
        frame_left=-plot_Size_X/2+margin[i];
        frame_size_x=frame_right-frame_left;
    }
    if (x_axis_type[i]==0){
        frame_top=-plot_Size_Y/2+i*(plot_Size_Y/number)+titelsize[i]*1.3+margin[i];
        frame_size_y=(plot_Size_Y/number)-2*margin[i]-2.2*textsize[i]-titelsize[i]*1.3-colorLegend_size_y[i]-5;
        frame_bottom=frame_top+frame_size_y;

    }
    if (x_axis_type[i]==1){
        frame_top=-plot_Size_Y/2+i*(plot_Size_Y/number)+titelsize[i]*1.3+margin[i]+2*textsize[i]+5;
        frame_size_y=(plot_Size_Y/number)-2*margin[i]-2*textsize[i]-titelsize[i]*1.3-colorLegend_size_y[i]-5;
        frame_bottom=frame_top+frame_size_y;
    }
    if (x_axis_type[i]==2){
        frame_top=-plot_Size_Y/2+i*(plot_Size_Y/number)+titelsize[i]*1.3+margin[i]+2*textsize[i]+5;
        frame_size_y=(plot_Size_Y/number)-2*margin[i]-2.2*textsize[i]-titelsize[i]*1.3-2*textsize[i]-colorLegend_size_y[i]-10;
        frame_bottom=frame_top+frame_size_y;
    }
    if (x_axis_type[i]==3){
        frame_top=-plot_Size_Y/2+i*(plot_Size_Y/number)+margin[i];
        frame_size_y=(plot_Size_Y/number)-2*margin[i]-colorLegend_size_y[i];
        frame_bottom=frame_top+frame_size_y;
    }

}

void EditGraph_Graph::comHide(int i){
    if (com_hide[i]){
        com_hide[i]=0;
        gdata[i]->setComHide(com_hide[i]);
    }else{
        com_hide[i]=1;
        gdata[i]->setComHide(com_hide[i]);
    }
}

void EditGraph_Graph::setComHide(int i,int n){
    com_hide[i]=n;
    gdata[i]->setComHide(n);
}

void EditGraph_Graph::setSettings(QString p,int n){
    path[n]->clear();
    path[n]->append(p);
    // read file if it exists
    readSettings(n);
}

void EditGraph_Graph::setSettings(QString p){
    setSettings(p,0);
}

void EditGraph_Graph::readSettings(int n){
    QString txt;
    // Open file
    QFile file(*path[n]);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //qDebug() <<  file.errorString();
    } else {
        txt.clear();
        QTextStream in(&file);
        while(!in.atEnd()) {
            txt.append(in.readLine().toLocal8Bit()+"\n");
        }
        file.close();
        //qDebug()<<txt;

        // Parse File
        QStringList lines=txt.split("\n");
        for (int i=0;i<lines.length();i++){
            QStringList line=lines[i].split("=");
            if (line.length()==2){
                QString var=line[0];
                QString val=line[1];
                //qDebug()<<var+"="+val;
                if (var=="Titel"){
                    titel[n]->clear();
                    titel[n]->append(val);    
                }
                if (var=="Titel_X"){
                    titel_x[n]->clear();
                    titel_x[n]->append(val);
                }
                if (var=="Titel_Y"){
                    titel_y[n]->clear();
                    titel_y[n]->append(val);
                }
                if (var=="Symbolsize"){
                    symbol_size[n]=val.toInt();
                }
                if (var=="Textsize"){
                    textsize[n]=val.toInt();
                }
                if (var=="Titelsize"){
                    titelsize[n]=val.toInt();
                }
                if (var=="Margin"){
                    margin[n]=val.toInt();
                }
                if (var=="Linewidth"){
                    line_width[n]=val.toInt();
                }
                if (var=="Linestyle"){
                    if (val=="SolidLine") line_style[n]=Qt::SolidLine;
                    if (val=="DashDotLine") line_style[n]=Qt::DashDotLine;
                    if (val=="DashLine") line_style[n]=Qt::DashLine;
                    if (val=="DotLine") line_style[n]=Qt::DotLine;
                    if (val=="NoPen") line_style[n]=Qt::NoPen;
                    if (val=="DashDotDotLine") line_style[n]=Qt::DashDotDotLine;
                }
                if (var=="Linecolor"){
                    QStringList vals=val.split(",");
                    if (vals.length()==3){
                        line_color[n]=QColor(vals[0].toInt(),vals[1].toInt(),vals[2].toInt());
                    }
                }

                if (var=="Multiplicator_X"){
                    factor_x[n]=val.toFloat();
                }
                if (var=="Multiplicator_Y"){
                    factor_y[n]=val.toFloat();
                }

                if (var=="Marker_X"){
                    marker_x[n]=val.toFloat();
                }
                if (var=="Marker_Y"){
                    marker_y[n]=val.toFloat();
                }
                if (var=="Marker_Mode"){
                    marker_mode[n]=val.toInt();
                }

                if (var=="Fix_Range_X"){
                    fix_x[n]=val.toInt();
                }
                if (var=="Fix_Range_Y"){
                    fix_y[n]=val.toInt();
                }

                if (var=="Lock_Aspect"){
                    lock_aspect[n]=val.toInt();
                }

                if (var=="Fold_X"){
                    fold_x[n]=val.toInt();
                }
                if (var=="Fold_Y"){
                    fold_y[n]=val.toInt();
                }

                if (var=="Repeat_X"){
                    repeat_x[n]=val.toInt();
                }
                if (var=="Repeat_X1"){
                    repeat_x1[n]=val.toFloat();
                }
                if (var=="Repeat_X2"){
                    repeat_x2[n]=val.toFloat();
                }
                if (var=="Repeat_Y"){
                    repeat_y[n]=val.toInt();
                }
                if (var=="Repeat_Y1"){
                    repeat_y1[n]=val.toFloat();
                }
                if (var=="Repeat_Y2"){
                    repeat_y2[n]=val.toFloat();
                }


                if (var=="Limit_X1"){
                    limit_x1[n]=val.toFloat();
                }
                if (var=="Limit_X2"){
                    limit_x2[n]=val.toFloat();
                }
                if (var=="Limit_Y1"){
                    limit_y1[n]=val.toFloat();
                }
                if (var=="Limit_Y2"){
                    limit_y2[n]=val.toFloat();
                }
                if (var=="Axistype_X"){
                    x_axis_type[n]=val.toInt();
                }
                if (var=="Axistype_Y"){
                    y_axis_type[n]=val.toInt();
                }
                if (var=="Ticks_X"){
                    ticks_x[n]=val.toInt();
                }
                if (var=="Ticks_Y"){
                    ticks_y[n]=val.toInt();
                }
                if (var=="Background"){
                    background[n]=val.toInt();
                }
                if (var=="Background_X1"){
                    background_x1[n]=val.toFloat();
                }
                if (var=="Background_Y1"){
                    background_y1[n]=val.toFloat();
                }
                if (var=="Background_X2"){
                    background_x2[n]=val.toFloat();
                }
                if (var=="Background_Y2"){
                    background_y2[n]=val.toFloat();
                }
                if (var=="Color_Legend_Type"){
                    colorLegend_type[n]=val.toInt();
                }
                if (var=="Color_Legend_Titel"){
                    colorLegend_titel[n]->clear();
                    colorLegend_titel[n]->append(val);
                }
                if (var=="Color_Legend_Min"){
                    colorLegend_min[n]=val.toFloat();
                }
                if (var=="Color_Legend_Max"){
                    colorLegend_max[n]=val.toFloat();
                }
                if (var=="Color_Scheme"){
                    colorScheme[n]=val.toInt();
                }

                if (var=="Poly_Mode"){
                    setpoly_mode[n]=val.toInt();
                }
                if (setpoly_mode[n]>0 && var=="Poly_Data"){
                    QStringList dat=val.split("\t");
                    setpoly[n]=new int[column[n]];
                    for (int i=0;i<dat.size();i++) setpoly[n][i]=dat[i].toInt();
                }
                if (var=="Area_Mode"){
                    setarea_mode[n]=val.toInt();
                }
                if (setarea_mode[n]>0 && var=="Area_Data_X"){
                    QStringList dat=val.split("\t");
                    setarea_x[n]=new float[column[n]];
                    for (int i=0;i<dat.size();i++) setarea_x[n][i]=dat[i].toFloat();
                }
                if (setpoly_mode[n]>0 && var=="Area_Data_Y"){
                    QStringList dat=val.split("\t");
                    setarea_y[n]=new float[column[n]];
                    for (int i=0;i<dat.size();i++) setarea_y[n][i]=dat[i].toFloat();
                }
                if (var=="Color_Mode"){
                    setcolor_mode[n]=val.toInt();
                }
                if (setcolor_mode[n]>0 && var=="Color_Data"){
                    QStringList dat=val.split("\t");
                    setcolor_color[n]=new QColor[column[n]];
                    for (int i=0;i<dat.size();i++) {
                        QStringList dat2=dat[i].split(":");
                        setcolor_color[n][i]=QColor(dat2[0].toInt(),dat2[1].toInt(),dat2[2].toInt());
                    }
                }
                if (var=="LineColor_Mode"){
                    setlinecolor_mode[n]=val.toInt();
                }
                if (setlinecolor_mode[n]>0 && var=="LineColor_Data"){
                    QStringList dat=val.split("\t");
                    setlinecolor_color[n]=new QColor[column[n]];
                    for (int i=0;i<dat.size();i++) {
                        QStringList dat2=dat[i].split(":");
                        setlinecolor_color[n][i]=QColor(dat2[0].toInt(),dat2[1].toInt(),dat2[2].toInt());
                    }
                }
                if (var=="LineWidth_Mode"){
                    setlinewidth_mode[n]=val.toInt();
                }
                if (setlinewidth_mode[n]>0 && var=="LineWidth_Data"){
                    QStringList dat=val.split("\t");
                    setlinewidth[n]=new int[column[n]];
                    for (int i=0;i<dat.size();i++) setlinewidth[n][i]=dat[i].toInt();
                }
                if (var=="Symbolsize_Mode"){
                    setsymbolsize_mode[n]=val.toInt();
                }
                if (setsymbolsize_mode[n]>0 && var=="Symbolsize_Data"){
                    QStringList dat=val.split("\t");
                    setsymbolsize[n]=new int[column[n]];
                    for (int i=0;i<dat.size();i++) setsymbolsize[n][i]=dat[i].toInt();
                }
                if (var=="LineStyle_Mode"){
                    setlinestyle_mode[n]=val.toInt();
                }
                if (setlinestyle_mode[n]>0 && var=="LineStyle_Data"){
                    QStringList dat=val.split("\t");
                    setlinestyle[n]=new Qt::PenStyle[column[n]];
                    Qt::PenStyle pen;
                    for (int i=0;i<dat.size();i++) {

                        if (dat[i].toInt()==0) pen=Qt::NoPen;
                        if (dat[i].toInt()==1) pen=Qt::SolidLine;
                        if (dat[i].toInt()==2) pen=Qt::DotLine;
                        if (dat[i].toInt()==3) pen=Qt::DashLine;
                        if (dat[i].toInt()==4) pen=Qt::DashDotLine;
                        if (dat[i].toInt()==5) pen=Qt::DashDotDotLine;
                        setlinestyle[n][i]=pen;
                    }
                }

                if (var=="Symboltype_Mode"){
                    setsymboltype_mode[n]=val.toInt();
                }
                if (setsymboltype_mode[n]>0 && var=="Symboltype_Data"){
                    QStringList dat=val.split("\t");
                    setsymboltype[n]=new int[column[n]];
                    for (int i=0;i<dat.size();i++) setsymboltype[n][i]=dat[i].toInt();
                }

                if (var=="Autosize_Mode"){
                    autosize_mode[n]=val.toInt();
                }
                if (autosize_mode[n]>0 && var=="Autosize_Data"){
                    QStringList dat=val.split("\t");
                    autosize[n]=new int[column[n]];
                    for (int i=0;i<dat.size();i++) autosize[n][i]=dat[i].toInt();
                }

                if (var=="AddLine_Mode"){
                    gobject->addLineClear();
                    gobject->setAddMode(val.toInt());
                }
                if (gobject->getAddMode()>0 && var=="AddLine_Data"){
                    QStringList dat=val.split("\t");
                    QStringList dat2=dat[6].split(":");
                    Qt::PenStyle pen;


                    if (dat[8].toInt()==0) pen=Qt::NoPen;
                    if (dat[8].toInt()==1) pen=Qt::SolidLine;
                    if (dat[8].toInt()==2) pen=Qt::DotLine;
                    if (dat[8].toInt()==3) pen=Qt::DashLine;
                    if (dat[8].toInt()==4) pen=Qt::DashDotLine;
                    if (dat[8].toInt()==5) pen=Qt::DashDotDotLine;

                    gobject->addLine(dat[0].toFloat(),dat[1].toFloat(),dat[2].toInt(),
                            dat[3].toFloat(),dat[4].toFloat(),dat[5].toInt(),
                            QColor(dat2[0].toInt(),dat2[1].toInt(),dat2[2].toInt()),
                            dat[7].toInt(),pen);

                }





            }

        }
    }

    setTitel(*titel[n],*titel_x[n],*titel_y[n],n);
    setSymbol(symbol_size[n],n);
    setTextSize(textsize[n],titelsize[n],margin[n],n);
    setLineColor(line_color[n],n);
    setLineStyle(line_style[n],n);
    setLineWidth(line_width[n],n);
    setLineColor(line_color[n],n);
    setLineStyle(line_style[n],n);
    setMultiplicator(factor_x[n],factor_y[n],n);
    addMarker(marker_x[n],marker_y[n],marker_mode[n],n);
    fixRange(fix_x[n],fix_y[n]);
    setLockAspect(lock_aspect[n],n);
    setFolding(fold_x[n],fold_y[n],n);
    setRepeat(repeat_x[n],repeat_x1[n],repeat_x2[n],repeat_y[n],repeat_y1[n],repeat_y2[n],n);
    setLimit(limit_x1[n],limit_x2[n],limit_y1[n],limit_y2[n],n);
    setAxisType(x_axis_type[n],y_axis_type[n],n);
    setTicks(ticks_x[n],ticks_y[n]);
    //if (*background_filename[n]!="" && background[n]>0) setBackground(*background_filename[n],background_x1[n],background_x2[n],background_y1[n],background_y2[n],background[n],n);
    //if (background_fileimage[n]!=nullptr && background[n]>0) setBackground(background_fileimage[n],background_x1[n],background_x2[n],background_y1[n],background_y2[n],background[n],n);
    //if (background_image[n]!=nullptr && background[n]>0) setBackground2(background_image[n],background_x1[n],background_x2[n],background_y1[n],background_y2[n],background[n],n);
    setColorLegend(colorLegend_type[n],*colorLegend_titel[n],colorLegend_min[n],colorLegend_max[n],n);
    setColorScheme(colorScheme[n],n);
    if (setpoly_mode[n]>0)setSetPoly(setpoly[n],setpoly_mode[n],n);
    if (setarea_mode[n]>0)setSetArea(setarea_x[n],setarea_y[n],setarea_mode[n],n);
    if (setcolor_mode[n]>0)setSetColor(setcolor_color[n],setcolor_mode[n],n);
    if (setlinecolor_mode[n]>0)setSetLineColor(setlinecolor_color[n],setlinecolor_mode[n],n);
    if (setlinewidth_mode[n]>0)setSetLineWidth(setlinewidth[n],setlinewidth_mode[n],n);
    if (setsymbolsize_mode[n]>0)setSetSymbolsize(setsymbolsize[n],setsymbolsize_mode[n],n);
    if (setlinestyle_mode[n]>0)setSetLineStyle(setlinestyle[n],setlinestyle_mode[n],n);
    if (setsymboltype_mode[n]>0)setSetSymboltype(setsymboltype[n],setsymboltype_mode[n],n);
    if (autosize_mode[n]>0)setAutosize(autosize[n],autosize_mode[n],n);
    autoSize(n);

    update();
}

void EditGraph_Graph::saveSettings(int n){
    if (*path[n]!=""){
        getBack(n);

        // Prepare txt
        QString txt;
        txt.clear();
        txt.append("Titel="+*titel[n]+"\n");
        txt.append("Titel_X="+*titel_x[n]+"\n");
        txt.append("Titel_Y="+*titel_y[n]+"\n");
        txt.append("Symbolsize="+QString::number(symbol_size[n])+"\n");
        txt.append("Titelsize="+QString::number(titelsize[n])+"\n");
        txt.append("Textsize="+QString::number(textsize[n])+"\n");
        txt.append("Margin="+QString::number(margin[n])+"\n");
        txt.append("Linewidth="+QString::number(line_width[n])+"\n");
        txt.append("Linecolor="+QString::number(line_color[n].red())+","+QString::number(line_color[n].green())+","+QString::number(line_color[n].blue())+"\n");
        txt.append("Linestyle="+QString::number(line_style[n])+"\n");
        txt.append("Multiplicator_X="+QString::number(factor_x[n])+"\n");
        txt.append("Multiplicator_Y="+QString::number(factor_y[n])+"\n");
        txt.append("Marker_X="+QString::number(marker_x[n])+"\n");
        txt.append("Marker_Y="+QString::number(marker_y[n])+"\n");
        txt.append("Marker_Mode="+QString::number(marker_mode[n])+"\n");
        txt.append("Fix_Range_X="+QString::number(fix_x[n])+"\n");
        txt.append("Fix_Range_Y="+QString::number(fix_y[n])+"\n");
        txt.append("Lock_Aspect="+QString::number(lock_aspect[n])+"\n");
        txt.append("Fold_X="+QString::number(fold_x[n])+"\n");
        txt.append("Fold_Y="+QString::number(fold_y[n])+"\n");
        txt.append("Repeat_X="+QString::number(repeat_x[n])+"\n");
        txt.append("Repeat_X1="+QString::number(repeat_x1[n])+"\n");
        txt.append("Repeat_X2="+QString::number(repeat_x2[n])+"\n");
        txt.append("Repeat_Y="+QString::number(repeat_y[n])+"\n");
        txt.append("Repeat_Y1="+QString::number(repeat_y1[n])+"\n");
        txt.append("Repeat_Y2="+QString::number(repeat_y2[n])+"\n");
        txt.append("Limit_X1="+QString::number(limit_x1[n])+"\n");
        txt.append("Limit_X2="+QString::number(limit_x2[n])+"\n");
        txt.append("Limit_Y1="+QString::number(limit_y1[n])+"\n");
        txt.append("Limit_Y2="+QString::number(limit_y2[n])+"\n");
        txt.append("Axistype_X="+QString::number(x_axis_type[n])+"\n");
        txt.append("Axistype_Y="+QString::number(y_axis_type[n])+"\n");
        txt.append("Ticks_X="+QString::number(ticks_x[n])+"\n");
        txt.append("Ticks_Y="+QString::number(ticks_y[n])+"\n");
        txt.append("Background="+QString::number(background[n])+"\n");
        txt.append("Background_X1="+QString::number(background_x1[n])+"\n");
        txt.append("Background_Y1="+QString::number(background_y1[n])+"\n");
        txt.append("Background_X2="+QString::number(background_x2[n])+"\n");
        txt.append("Background_Y2="+QString::number(background_y2[n])+"\n");
        txt.append("Color_Legend_Type="+QString::number(colorLegend_type[n])+"\n");
        txt.append("Color_Legend_Titel="+*colorLegend_titel[n]+"\n");
        txt.append("Color_Legend_Min="+QString::number(colorLegend_min[n])+"\n");
        txt.append("Color_Legend_Max="+QString::number(colorLegend_max[n])+"\n");
        txt.append("Color_Scheme="+QString::number(colorScheme[n])+"\n");
        if (setpoly_mode[n]>0){
            txt.append("Poly_Mode="+QString::number(setpoly_mode[n])+"\n");
            txt.append("Poly_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setpoly[n][i])+"\t");
            }   txt.append(QString::number(setpoly[n][column[n]-1])+"\n");
        }
        if (setarea_mode[n]>0){
            txt.append("Area_Mode="+QString::number(setarea_mode[n])+"\n");
            txt.append("Area_Data_X=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setarea_x[n][i])+"\t");
            }   txt.append(QString::number(setarea_x[n][column[n]-1])+"\n");
            txt.append("Area_Data_Y=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setarea_y[n][i])+"\t");
            }   txt.append(QString::number(setarea_y[n][column[n]-1])+"\n");
        }
        if (setcolor_mode[n]>0){
            txt.append("Color_Mode="+QString::number(setcolor_mode[n])+"\n");
            txt.append("Color_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setcolor_color[n][i].red())+":"+QString::number(setcolor_color[n][i].green())+":"+QString::number(setcolor_color[n][i].blue())+"\t");
            }   txt.append(QString::number(setcolor_color[n][column[n]-1].red())+":"+QString::number(setcolor_color[n][column[n]-1].green())+":"+QString::number(setcolor_color[n][column[n]-1].blue())+"\n");
        }
        if (setlinecolor_mode[n]>0){
            txt.append("LineColor_Mode="+QString::number(setlinecolor_mode[n])+"\n");
            txt.append("LineColor_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setlinecolor_color[n][i].red())+":"+QString::number(setlinecolor_color[n][i].green())+":"+QString::number(setlinecolor_color[n][i].blue())+"\t");
            }   txt.append(QString::number(setlinecolor_color[n][column[n]-1].red())+":"+QString::number(setlinecolor_color[n][column[n]-1].green())+":"+QString::number(setlinecolor_color[n][column[n]-1].blue())+"\n");
        }
        if (setlinewidth_mode[n]>0){
            txt.append("LineWidth_Mode="+QString::number(setlinewidth_mode[n])+"\n");
            txt.append("LineWidth_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setlinewidth[n][i])+"\t");
            }   txt.append(QString::number(setlinewidth[n][column[n]-1])+"\n");
        }
        if (setsymbolsize_mode[n]>0){
            txt.append("Symbolsize_Mode="+QString::number(setsymbolsize_mode[n])+"\n");
            txt.append("Symbolsize_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setsymbolsize[n][i])+"\t");
            }   txt.append(QString::number(setsymbolsize[n][column[n]-1])+"\n");
        }
        if (setlinestyle_mode[n]>0){
            txt.append("Linestyle_Mode="+QString::number(setlinestyle_mode[n])+"\n");
            txt.append("Linestyle_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setlinestyle[n][i])+"\t");
            }   txt.append(QString::number(setlinestyle[n][column[n]-1])+"\n");
        }
        if (setsymboltype_mode[n]>0){
            txt.append("Symboltype_Mode="+QString::number(setsymboltype_mode[n])+"\n");
            txt.append("Symboltype_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(setsymboltype[n][i])+"\t");
            }   txt.append(QString::number(setsymboltype[n][column[n]-1])+"\n");
        }
        if (autosize_mode[n]>0){
            txt.append("Autosize_Mode="+QString::number(autosize_mode[n])+"\n");
            txt.append("Autosize_Data=");
            for (int i=0;i<column[n]-1;i++){
                txt.append(QString::number(autosize[n][i])+"\t");
            }   txt.append(QString::number(autosize[n][column[n]-1])+"\n");
        }
        if (gobject->getAddMode()>0){
            txt.append("AddLine_Mode="+QString::number(gobject->getAddMode())+"\n");

            for (int i=0;i<gobject->getCount();i++){
                txt.append("AddLine_Data=");
                txt.append(QString::number(gobject->getLine_x1(i))+"\t");
                txt.append(QString::number(gobject->getLine_y1(i))+"\t");
                txt.append(QString::number(gobject->getLine_n1(i))+"\t");
                txt.append(QString::number(gobject->getLine_x2(i))+"\t");
                txt.append(QString::number(gobject->getLine_y2(i))+"\t");
                txt.append(QString::number(gobject->getLine_n2(i))+"\t");
                txt.append(QString::number(gobject->getLine_Color(i).red())+":"+QString::number(gobject->getLine_Color(i).green())+":"+QString::number(gobject->getLine_Color(i).blue())+"\t");
                txt.append(QString::number(gobject->getLine_Width(i))+"\t");
                txt.append(QString::number(gobject->getLine_Style(i))+"\t");
            }   txt.append("\n");
        }



        // Write Everything
        QFile f(*path[n]);
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() <<  f.errorString();
        } else {
        QTextStream out(&f);
        out<<txt;
        f.close();
        }
    }

}

void EditGraph_Graph::getBack(int n){
    titel[n]->clear();
    titel[n]->append(ggrid[n]->getTitel());
    titel_x[n]->clear();
    titel_x[n]->append(ggrid[n]->getTitelX());
    titel_y[n]->clear();
    titel_y[n]->append(ggrid[n]->getTitelY());
    titelsize[n]=ggrid[n]->getTitelSize();
    textsize[n]=ggrid[n]->getTextSize();
    margin[n]=ggrid[n]->getMargin();
    symbol_size[n]=gdata[n]->getSymbolSize();
    line_width[n]=gdata[n]->getLineWidth();
    line_color[n]=gdata[n]->getLineColor();
    line_style[n]=gdata[n]->getLineStyle();


}
void EditGraph_Graph::setBackground(QString filename, float x1, float x2, float y1, float y2, int mode){
   setBackground(filename, x1, x2, y1, y2, mode,0);
}

void EditGraph_Graph::setBackground(QString filename, float x1, float x2, float y1, float y2, int mode,int n){

    background[n]=mode;
    background_filename[n]->clear();
    background_filename[n]->append(filename);
    background_fileimage[n]=nullptr;
    background_x1[n]=x1;
    background_x2[n]=x2;
    background_y1[n]=y1;
    background_y2[n]=y2;

    ggrid[n]->setBackground(filename,x1,x2,y1,y2,mode);
}

void EditGraph_Graph::setBackground2(QImage *i, float x1, float x2, float y1, float y2, int mode){
   setBackground2(i, x1, x2, y1, y2, mode,0);
}

void EditGraph_Graph::setBackground2(QImage *i, float x1, float x2, float y1, float y2, int mode,int n){

    background[n]=mode;

    background_image[n]=i;
    background_x1[n]=x1;
    background_x2[n]=x2;
    background_y1[n]=y1;
    background_y2[n]=y2;

    ggrid[n]->setBackground2(i,x1,x2,y1,y2,mode);
    update();
}

void EditGraph_Graph::setBackground(QImage *i, float x1, float x2, float y1, float y2, int mode){
   setBackground(i, x1, x2, y1, y2, mode,0);
}

void EditGraph_Graph::setBackground(QImage *i, float x1, float x2, float y1, float y2, int mode,int n){

    background[n]=mode;
    background_filename[n]->clear();
    background_filename[n]->append("");
    background_fileimage[n]=i;
    background_x1[n]=x1;
    background_x2[n]=x2;
    background_y1[n]=y1;
    background_y2[n]=y2;

    ggrid[n]->setBackground(i,x1,x2,y1,y2,mode);
    update();
}

void EditGraph_Graph::setRepeat(int x,float x1,float x2,int y,float y1,float y2,int n){
    repeat_x[n]=x;
    repeat_x1[n]=x1;
    repeat_x2[n]=x2;
    repeat_y[n]=y;
    repeat_y1[n]=y1;
    repeat_y2[n]=y2;

    ggrid[n]->setRepeat(x,x1,x2,y,y1,y2);
    gdata[n]->setRepeat(x,x1,x2,y,y1,y2);
}

void EditGraph_Graph::setRepeat(int x,float x1,float x2,int y,float y1,float y2){
    setRepeat(x,x1,x2,y,y1,y2,0);
}

void EditGraph_Graph::setLimit(float x1, float x2, float y1, float y2, int n){
    limit_x1[n]=x1;
    limit_x2[n]=x2;
    limit_y1[n]=y1;
    limit_y2[n]=y2;
}

void EditGraph_Graph::setLimit(float x1, float x2, float y1, float y2){
    setLimit(x1,x2,y1,y2,0);
}

void EditGraph_Graph::setLockAspect(int mode,int n){
    lock_aspect[n]=mode;
}
void EditGraph_Graph::setLockAspect(int mode){
    setLockAspect(mode,0);
}

void EditGraph_Graph::setFolding(int x,int y,int n){
    fold_x[n]=x;
    fold_y[n]=y;
}

void EditGraph_Graph::setFolding(int x,int y){
    setFolding(x,y,0);
}

void EditGraph_Graph::setView(int x1,int x2,int y1,int y2,int n){
    x_max[n]=x2;
    x_min[n]=x1;
    y_min[n]=y1;
    y_max[n]=y2;
    ggrid[n]->setView(x_min[n],x_max[n],y_min[n],y_max[n]);
    gdata[n]->setView(x_min[n],x_max[n],y_min[n],y_max[n]);
    update();
}

void EditGraph_Graph::setView(int x1,int x2,int y1,int y2){
    setView(x1,x2,y1,y2,0);
}
