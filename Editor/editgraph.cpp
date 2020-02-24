#include "editgraph.h"
#include "ui_editgraph.h"

EditGraph::EditGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditGraph)
{
    ui->setupUi(this);
    this->setWindowTitle("Edit Graph Parameters");
    resources=new Resources();
    // creating default
    number=1;
    x_min.reserve(number);
    y_min.reserve(number);
    x_max.reserve(number);
    y_max.reserve(number);

    x_axis_type.reserve(number);
    y_axis_type.reserve(number);
    ticks_x.reserve(number);
    ticks_y.reserve(number);


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
        data[i]=nullptr;
        column[i]=0;
        row[i]=0;

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
        dat_com[i]=nullptr;
        com_mode[i]=0;
        com_hide[i]=0;
        dat_label[i]=nullptr;
        label_mode[i]=0;
        usemode[i]=0;
        useflag[i]=nullptr;
        factor_x[i]=1;
        factor_y[i]=1;
        fix_x[i]=false;
        fix_y[i]=false;
        col_dat[i]=nullptr;
        color[i]=0;
        data_err[i]=nullptr;
        data_err2[i]=nullptr;
        error[i]=0;
        error2[i]=0;
        data_mark[i]=nullptr;
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
        setcolor_color[i]=nullptr;
        setlinecolor_color[i]=nullptr;
        setsymbolsize[i]=nullptr;
        setlinewidth[i]=nullptr;
        setlinestyle[i]=nullptr;
        autosize_mode[i]=0;
        autosize[i]=nullptr;


        setpoly_mode[i]=0;
        setpoly[i]=nullptr;
        setarea_mode[i]=0;
        setarea_x[i]=nullptr;
        setarea_y[i]=nullptr;

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
        background_fileimage[i]=nullptr;
        background_image[i]=nullptr;

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
    object_mode=0;
    object_line_x1.clear();
    object_line_y1.clear();
    object_line_n1.clear();
    object_line_x2.clear();
    object_line_y2.clear();
    object_line_n2.clear();
    object_line_color.clear();
    object_line_width.clear();
    object_line_style.clear();

    graph=new EditGraph_Graph(this,data[0],column[0],row[0]);
    ui->graphicsView->setScene(graph);

    setupDataTab();
    setupGlobalTab();

    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateData()));
    connect(modelData,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editData(QModelIndex,QModelIndex)));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(updateAll()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(copy()));

    // Global Settings
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(applyGlobal()));
    connect(ui->lineEdit_2,SIGNAL(returnPressed()),this,SLOT(applyGlobal()));
    connect(ui->lineEdit_3,SIGNAL(returnPressed()),this,SLOT(applyGlobal()));
    connect(ui->spinBox_2,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_3,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_4,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_9,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_10,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_2,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_5,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(changeColor()));
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_3,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_4,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_12,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_4,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_2,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_3,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_5,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_6,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_4,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_3,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_7,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_8,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));

    //connect(ui->comboBox_9,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_5,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_6,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_7,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_8,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_5,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_9,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_10,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_11,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_12,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_7,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->spinBox_8,SIGNAL(valueChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_8,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_9,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->checkBox_10,SIGNAL(stateChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_15,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_16,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_17,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_18,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->comboBox_10,SIGNAL(currentIndexChanged(int)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_13,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->doubleSpinBox_14,SIGNAL(valueChanged(double)),this,SLOT(applyGlobal()));
    connect(ui->lineEdit_4,SIGNAL(returnPressed()),this,SLOT(applyGlobal()));
    // Local Settings
    connect(ui->checkBox_6,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_7,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_11,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_13,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_14,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_15,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_16,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_17,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));
    connect(ui->checkBox_18,SIGNAL(stateChanged(int)),this,SLOT(applyLocal()));

    connect(ui->checkBox_19,SIGNAL(stateChanged(int)),this,SLOT(applyObjects()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(saveSettings()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(deleteSettings()));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),this,SLOT(readSettings()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(saveImage()));
}

EditGraph::~EditGraph()
{
    delete ui;
    deleteAll();
    delete graph;
}

void EditGraph::paintEvent(QPaintEvent *)
{
    graph->setSize(ui->graphicsView->width(),ui->graphicsView->height());
}

void EditGraph::updateAllTabs(){
    setupDataTab();
    setupGlobalTab();
    setupLocalTab();
    setupObjectTab();
    setupOptionsTab();
}


void EditGraph::setNumber(int n){
    // Should allways be the fist to define
    // initialize all values
    deleteAll();
    number=n;

    x_min.reserve(number);
    y_min.reserve(number);
    x_max.reserve(number);
    y_max.reserve(number);
    x_axis_type.reserve(number);
    y_axis_type.reserve(number);
    ticks_x.reserve(number);
    ticks_y.reserve(number);
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
        data[i]=nullptr;
        column[i]=0;
        row[i]=0;



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
        dat_com[i]=nullptr;
        com_mode[i]=0;
        com_hide[i]=0;
        dat_label[i]=nullptr;
        label_mode[i]=0;
        usemode[i]=0;
        useflag[i]=nullptr;
        factor_x[i]=1;
        factor_y[i]=1;
        fix_x[i]=false;
        fix_y[i]=false;
        col_dat[i]=nullptr;
        color[i]=0;
        data_err[i]=nullptr;
        data_err2[i]=nullptr;
        error[i]=0;
        error2[i]=0;
        data_mark[i]=nullptr;
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
        setcolor_color[i]=nullptr;
        setlinecolor_color[i]=nullptr;
        setsymbolsize[i]=nullptr;
        setsymboltype[i]=nullptr;
        setlinewidth[i]=nullptr;
        setlinestyle[i]=nullptr;
        setpoly_mode[i]=0;
        setpoly[i]=nullptr;
        setarea_mode[i]=0;
        setarea_x[i]=nullptr;
        setarea_y[i]=nullptr;
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
        background_fileimage[i]=nullptr;
        background_image[i]=nullptr;
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
        autosize[i]=nullptr;

    }
    object_mode=0;
    object_line_x1.clear();
    object_line_y1.clear();
    object_line_n1.clear();
    object_line_x2.clear();
    object_line_y2.clear();
    object_line_n2.clear();
    object_line_color.clear();
    object_line_width.clear();
    object_line_style.clear();

    // Transfering this to graph
    graph->setNumber(number);

    for (int i=0;i<number;i++){
        setData(data[i],column[i],row[i],i);
        setTitel(*titel[i],*titel_x[i],*titel_y[i],i);
    }
    ui->spinBox->setMaximum(number-1);
}

void EditGraph::deleteAll(){
    for (int i=0;titel.size();i++) delete titel[i];
    for (int i=0;titel_x.size();i++) delete titel_x[i];
    for (int i=0;titel_y.size();i++) delete titel_y[i];
    for (int i=0;path.size();i++) delete path[i];

    for (int i=0;background_filename.size();i++) delete background_filename[i];
}

void EditGraph::setData(float *d,int c,int r, int n){
    if (n<number){
        data[n]=d;
        column[n]=c;
        row[n]=r;
        graph->setData(data[n],column[n],row[n],n);
    }
}

void EditGraph::setUse(bool *u,int m,int n){
    useflag[n]=u;
    usemode[n]=m;
    graph->setUse(useflag[n],usemode[n],n);
}

void EditGraph::setError(float *d_e,int m,int n){
    data_err[n]=d_e;
    error[n]=m;
    graph->setError(data_err[n],error[n],n);
}

void EditGraph::setError2(float *d_e,int m,int n){
    data_err2[n]=d_e;
    error2[n]=m;
    graph->setError2(data_err2[n],error2[n],n);
}

void EditGraph::autoSize(int n){
    graph->autoSize(n);
}

void EditGraph::setTitel(QString t,QString tx,QString ty,int n){
    titel[n]->clear();
    titel_x[n]->clear();
    titel_y[n]->clear();
    titel[n]->append(t);
    titel_x[n]->append(tx);
    titel_y[n]->append(ty);
    graph->setTitel(*titel[n],*titel_x[n],*titel_y[n],n);

}

void EditGraph::setTextSize(int texts, int titels,int margins,int n){
    textsize[n]=texts;
    titelsize[n]=titels;
    margin[n]=margins;
    qDebug()<<QString::number(textsize[n])+":"+QString::number(margin[n]);
    graph->setTextSize(textsize[n],titelsize[n],margin[n],n);
}

void EditGraph::setSymbol(int size,int n){
    symbol_size[n]=size;
    graph->setSymbol(symbol_size[n],n);
}

void EditGraph::setColor(QColor *c,bool m,int n){
    col_dat[n]=c;
    color[n]=m;
    graph->setColor(col_dat[n],color[n],n);
}

void EditGraph::setComment(QString *s,int m,int n){
    dat_com[n]=s;
    com_mode[n]=m;
    graph->setComment(dat_com[n],com_mode[n],n);
}

void EditGraph::setComHide(int i, int n){
    com_hide[n]=i;
    graph->setComHide(n,com_hide[n]);
}

void EditGraph::setLineWidth(int i,int n){
    line_width[n]=i;
    graph->setLineWidth(line_width[n],n);
}

void EditGraph::setLineStyle(Qt::PenStyle sty,int n){
    line_style[n]=sty;
    graph->setLineStyle(line_style[n],n);
}

void EditGraph::setLineColor(QColor c,int n){
    line_color[n]=c;
    graph->setLineColor(line_color[n],n);
}

void EditGraph::setLabel(QString *s,int m,int n){
    dat_label[n]=s;
    label_mode[n]=m;
    graph->setLabel(dat_label[n],label_mode[n],n);
}

void EditGraph::setMultiplicator(double fx, double fy,int n){
    factor_x[n]=fx;
    factor_y[n]=fy;
    graph->setMultiplicator(factor_x[n],factor_y[n],n);
}

void EditGraph::setMark(int *d_m,int m,int n){
    data_mark[n]=d_m;
    marker[n]=m;
    graph->setMark(data_mark[n],marker[n],n);
}

void EditGraph::addMarker(float x,float y,int m,int n){
    marker_x[n]=x;
    marker_y[n]=y;
    marker_mode[n]=m;
    graph->addMarker(marker_x[n],marker_y[n],marker_mode[n],n);
}

void EditGraph::setAxisType(int x,int y,int n){
    x_axis_type[n]=x;
    y_axis_type[n]=y;
    graph->setAxisType(x_axis_type[n],y_axis_type[n],n);
}

void EditGraph::setTicks(int tx,int ty,int n){
    ticks_x[n]=tx;
    ticks_y[n]=ty;
    graph->setTicks(tx,ty);
}

void EditGraph::fixRange(bool x,bool y,int n){
    fix_x[n]=x;
    fix_y[n]=y;
    graph->fixRange(fix_x[n],fix_y[n],n);
}

void EditGraph::setLockAspect(int mode,int n){
    lock_aspect[n]=mode;
    graph->setLockAspect(lock_aspect[n],n);
}

void EditGraph::setFolding(int x,int y,int n){
    fold_x[n]=x;
    fold_y[n]=y;
    graph->setFolding(fold_x[n],fold_y[n],n);
}

void EditGraph::setBackground(QString filename,float x1,float x2,float y1,float y2,int mode,int n){
    background_filename[n]->clear();
    background_filename[n]->append(filename);
    background_fileimage[n]=nullptr;
    background_x1[n]=x1;
    background_x2[n]=x2;
    background_y1[n]=y1;
    background_y2[n]=y2;
    background[n]=mode;
    graph->setBackground(*background_filename[n],background_x1[n],background_x2[n],background_y1[n],background_y2[n],background[n],n);
}

void EditGraph::setBackground(QImage *i,float x1,float x2,float y1,float y2,int mode,int n){
    background_fileimage[n]=i;
    background_filename[n]->clear();
    background_filename[n]->append("");
    background_x1[n]=x1;
    background_x2[n]=x2;
    background_y1[n]=y1;
    background_y2[n]=y2;
    background[n]=mode;
    graph->setBackground(background_fileimage[n],background_x1[n],background_x2[n],background_y1[n],background_y2[n],background[n],n);
}

void EditGraph::setBackground2(QImage *i,float x1,float x2,float y1,float y2,int mode,int n){
    background_image[n]=i;
    background_x1[n]=x1;
    background_x2[n]=x2;
    background_y1[n]=y1;
    background_y2[n]=y2;
    background[n]=mode;
    graph->setBackground2(background_image[n],background_x1[n],background_x2[n],background_y1[n],background_y2[n],background[n],n);
}

void EditGraph::setRepeat(int x,float x1,float x2,int y,float y1, float y2,int n){
    repeat_x[n]=x;
    repeat_x1[n]=x1;
    repeat_x2[n]=x2;
    repeat_y[n]=y;
    repeat_y1[n]=y1;
    repeat_y2[n]=y2;
    graph->setRepeat(repeat_x[n],repeat_x1[n],repeat_x2[n],repeat_y[n],repeat_y1[n],repeat_y2[n],n);
}

void EditGraph::setLimit(float x1, float x2, float y1, float y2,int n){
    limit_x1[n]=x1;
    limit_x2[n]=x2;
    limit_y1[n]=y1;
    limit_y2[n]=y2;
    graph->setLimit(limit_x1[n],limit_x2[n],limit_y1[n],limit_y2[n],n);
}

void EditGraph::setColorLegend(int type,QString t,float min,float max,int n){
    colorLegend_type[n]=type;
    colorLegend_size_x[n]=0;
    colorLegend_size_y[n]=0;
    if(colorLegend_type[n]==1) colorLegend_size_y[n]=textsize[n]*4.7;
    if(colorLegend_type[n]==2) colorLegend_size_x[n]=textsize[n]*4.7;
    colorLegend_titel[n]->clear();
    colorLegend_titel[n]->append(t);
    colorLegend_min[n]=min;
    colorLegend_max[n]=max;
    graph->setColorLegend(colorLegend_type[n],*colorLegend_titel[n],colorLegend_min[n],colorLegend_max[n],n);
}

void EditGraph::setColorScheme(int n,int i){
    graph->setColorScheme(n,i);
}

void EditGraph::setAutosize(int* as,int mode,int n){
    autosize[n]=as;
    autosize_mode[n]=mode;
    graph->setAutosize(autosize[n],autosize_mode[n],n);
}

void EditGraph::setSetColor(QColor *c,int m,int n){
    setcolor_mode[n]=m;
    setcolor_color[n]=c;
    graph->setSetColor(setcolor_color[n],setcolor_mode[n],n);
}

void EditGraph::setSetLineColor(QColor *c,int m,int n){
    setlinecolor_mode[n]=m;
    setlinecolor_color[n]=c;
    graph->setSetLineColor(setlinecolor_color[n],setlinecolor_mode[n],n);
}

void EditGraph::setSetLineWidth(int *s,int m,int n){
    setlinewidth_mode[n]=m;
    setlinewidth[n]=s;
    graph->setSetLineWidth(setlinewidth[n],setlinewidth_mode[n],n);
}

void EditGraph::setSetSymbolsize(int *s,int m,int n){
    setsymbolsize_mode[n]=m;
    setsymbolsize[n]=s;
    graph->setSetSymbolsize(setsymbolsize[n],setsymbolsize_mode[n],n);
}

void EditGraph::setSetLineStyle(Qt::PenStyle *s, int m,int n){
    setlinestyle_mode[n]=m;
    setlinestyle[n]=s;
    graph->setSetLineStyle(setlinestyle[n],setlinestyle_mode[n],n);
}

void EditGraph::setSetSymboltype(int *t,int m,int n){
    setsymboltype_mode[n]=m;
    setsymboltype[n]=t;
    graph->setSetSymboltype(setsymboltype[n],setsymboltype_mode[n],n);
}

void EditGraph::setSetPoly(int *s,int m,int n){
    setpoly_mode[n]=m;
    setpoly[n]=s;
    graph->setSetPoly(setpoly[n],setpoly_mode[n],n);
}

void EditGraph::setSetArea(float* area_x,float* area_y,int area_mode,int n){
    setarea_mode[n]=area_mode;
    setarea_x[n]=area_x;
    setarea_y[n]=area_y;
    graph->setSetArea(setarea_x[n],setarea_y[n],setarea_mode[n],n);
}

void EditGraph::addLineClear(){
    graph->addLineClear();
    object_mode=0;
    object_line_x1.clear();
    object_line_y1.clear();
    object_line_n1.clear();
    object_line_x2.clear();
    object_line_y2.clear();
    object_line_n2.clear();
    object_line_color.clear();
    object_line_width.clear();
    object_line_style.clear();
}

void EditGraph::addLine(float x1,float y1,int n1,float x2,float y2,int n2,QColor c,int w,Qt::PenStyle s){

    object_line_x1.push_back(x1);
    object_line_y1.push_back(y1);
    object_line_n1.push_back(n1);
    object_line_x2.push_back(x2);
    object_line_y2.push_back(y2);
    object_line_n2.push_back(n2);
    object_line_color.push_back(c);
    object_line_width.push_back(w);
    object_line_style.push_back(s);
    graph->addLine(x1,y1,n1,x2,y2,n2,c,w,s);
}

void EditGraph::setAddMode(int i){
    object_mode=i;
    graph->setAddMode(object_mode);
}

void EditGraph::setupDataTab(){
    // set checkbox
    int plot_n=ui->spinBox->value();
    ui->label_2->setText("Plot Titel : "+titel[plot_n]->toLatin1()+" X-Axis : "+titel_x[plot_n]->toLatin1()+" Y-Axis : "+titel_y[plot_n]->toLatin1());
    QString datatype=ui->comboBox->currentText();
    {
    const QSignalBlocker blocker(ui->checkBox);
    ui->checkBox->setCheckState(Qt::Unchecked);
    }
    if (datatype=="Data") ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Use" && usemode[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Error +" && error[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Error -" && error2[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Color" && color[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Marker" && marker[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Comment" && com_mode[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Label" && label_mode[plot_n]>0) ui->checkBox->setCheckState(Qt::Checked);
    if (datatype=="Data") {
        // set data table
        modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
        // set Headers
        for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
        // set Data
        for (int c=0;c<column[plot_n];c++){
            for (int r=0;r<row[plot_n];r++){
                QStandardItem *var=new QStandardItem(QString::number(data[plot_n][r+c*row[plot_n]]));
                modelData->setItem(r,c,var);
            }
        }
    }
    if (datatype=="Use") {
        if (usemode[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(QString::number(useflag[plot_n][r+c*row[plot_n]]));
                    modelData->setItem(r,c,var);
                }
            }
        } else {
            modelData = new QStandardItemModel(0,0,this);
        }
    }
    if (datatype=="Error +") {
        if (error[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(QString::number(data_err[plot_n][r+c*row[plot_n]]));
                    modelData->setItem(r,c,var);
                }
            }
         } else {
           modelData = new QStandardItemModel(0,0,this);
         }
    }
    if (datatype=="Error -") {
        if (error2[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(QString::number(data_err2[plot_n][r+c*row[plot_n]]));
                    modelData->setItem(r,c,var);
                }
            }
        } else {
            modelData = new QStandardItemModel(0,0,this);
        }
    }
    if (datatype=="Color") {
        if (color[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(QString::number(col_dat[plot_n][r+c*row[plot_n]].red())+" : "+QString::number(col_dat[plot_n][r+c*row[plot_n]].green())+" : "+QString::number(col_dat[plot_n][r+c*row[plot_n]].blue()));

                    var->setBackground(col_dat[plot_n][r+c*row[plot_n]]);
                    modelData->setItem(r,c,var);
                }
            }
        } else {
             modelData = new QStandardItemModel(0,0,this);
        }

    }

    if (datatype=="Marker") {
        if (marker[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(QString::number(data_mark[plot_n][r+c*row[plot_n]]));
                    modelData->setItem(r,c,var);
                }
            }
        } else {
            modelData = new QStandardItemModel(0,0,this);
        }
    }
    if (datatype=="Comment") {
        if (com_mode[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(dat_com[plot_n][r+c*row[plot_n]]);
                    modelData->setItem(r,c,var);
                }
            }
        } else {
            modelData = new QStandardItemModel(0,0,this);
        }

    }
    if (datatype=="Label") {
        if(label_mode[plot_n]>0){
            // set data table
            modelData = new QStandardItemModel(row[plot_n],column[plot_n],this);
            // set Headers
            for (int i=0;i<column[plot_n];i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
            for (int i=0;i<row[plot_n];i++) modelData->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));
            // set Data
            for (int c=0;c<column[plot_n];c++){
                for (int r=0;r<row[plot_n];r++){
                    QStandardItem *var=new QStandardItem(dat_label[plot_n][r+c*row[plot_n]]);
                    modelData->setItem(r,c,var);
                }
            }
        } else {
            modelData = new QStandardItemModel(0,0,this);
        }

    }



    ui->tableView->setModel(modelData);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView->setFont(font);
    ui->tableView->horizontalHeader()->setFont(font);
    ui->tableView->verticalHeader()->setFont(font);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableView->setSortingEnabled(0);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    connect(modelData,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editData(QModelIndex,QModelIndex)));
}


void EditGraph::updateData(){
    setupDataTab();

}

void EditGraph::updateAll(){
    setupDataTab();
    setupGlobalTab();
    setupLocalTab();
    setupObjectTab();
    setupOptionsTab();
}

void EditGraph::editData(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    QString datatype=ui->comboBox->currentText();
    ui->checkBox->setCheckState(Qt::Unchecked);
    if (datatype=="Data") {
        data[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text().toFloat();
        graph->setData(data[plot_n],column[plot_n],row[plot_n],0);
        graph->autoSize();
    }
    if (datatype=="Use" && usemode[plot_n]>0){
        useflag[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text().toInt();
        graph->setUse(useflag[plot_n],usemode[plot_n]);
        graph->autoSize();
    }

    if (datatype=="Error +" && error[plot_n]>0) {
        data_err[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text().toFloat();
        graph->setError(data_err[plot_n],error[plot_n]);
        graph->autoSize();
    }
    if (datatype=="Error -" && error2[plot_n]>0) {
        data_err2[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text().toFloat();
        graph->setError2(data_err2[plot_n],error2[plot_n]);
        graph->autoSize();
    }
    if (datatype=="Color" && color[plot_n]>0) {
        QString str=modelData->item(r.row(),r.column())->text();
        QStringList strlist=str.split(":");
        if (strlist.length()==3){
            QColor col=QColor(strlist[0].toInt(),strlist[1].toInt(),strlist[2].toInt());
            col_dat[plot_n][r.row()+r.column()*row[plot_n]]=col;
            graph->setColor(col_dat[plot_n],color[plot_n]);
            graph->autoSize();
            setupDataTab();
        }

    }
    if (datatype=="Marker" && marker[plot_n]>0) {
        data_mark[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text().toFloat();
        graph->setMark(data_mark[plot_n],marker[plot_n]);
        graph->autoSize();
    }
    if (datatype=="Comment" && com_mode[plot_n]>0){
        dat_com[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text();
        graph->setComment(dat_com[plot_n],com_mode[plot_n]);
        graph->autoSize();
    }
    if (datatype=="Label" && label_mode[plot_n]>0) {
        dat_label[plot_n][r.row()+r.column()*row[plot_n]]=modelData->item(r.row(),r.column())->text();
        graph->setLabel(dat_label[plot_n],label_mode[plot_n]);
        graph->autoSize();
    }
    update();
}

void EditGraph::copy(){
    int plot_n=ui->spinBox->value();
    QString clipboardString;
    /*for (int i=0;i<column[plot_n];i++) {
        QString str=QString::number(i);
        clipboardString.append(str.replace("\n"," : "));
        if (i<column[plot_n]-1) {
            clipboardString.append("\t");
        }else {
            clipboardString.append("\n");
        }
    }*/
        QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
        qSort(selectedIndexes);
        for (int i = 0; i < selectedIndexes.count(); ++i){
            QModelIndex current = selectedIndexes[i];
            QString displayText = current.data(Qt::DisplayRole).toString();
            //if (displayText=="nan") displayText="NAN";
             if (i + 1 < selectedIndexes.count()){
                QModelIndex next = selectedIndexes[i+1];
                if (next.row() != current.row()){
                    displayText.append("\n");
                }else{
                    displayText.append("\t");
                }
            }
            clipboardString.append(displayText);
        }
        qApp->clipboard()->setText(clipboardString);
}

void EditGraph::setupGlobalTab(){
    int plot_n=ui->spinBox->value();
    ui->lineEdit->setText(titel[plot_n]->toLatin1());
    ui->lineEdit_2->setText(titel_x[plot_n]->toLatin1());
    ui->lineEdit_3->setText(titel_y[plot_n]->toLatin1());

    {
    const QSignalBlocker blocker(ui->spinBox_2);
    ui->spinBox_2->setValue(titelsize[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->spinBox_3);
    ui->spinBox_3->setValue(textsize[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->spinBox_4);
    ui->spinBox_4->setValue(margin[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->doubleSpinBox);
    ui->doubleSpinBox->setValue(factor_x[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->doubleSpinBox_2);
    ui->doubleSpinBox_2->setValue(factor_y[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->spinBox_5);
    ui->spinBox_5->setValue(line_width[plot_n]);
    }


    ui->graphicsView_2->setScene(new QGraphicsScene);
    ui->graphicsView_2->setBackgroundBrush(line_color[plot_n]);

    {
    const QSignalBlocker blocker(ui->comboBox_2);
    if (line_style[plot_n]==Qt::NoPen) ui->comboBox_2->setCurrentIndex(0);
    if (line_style[plot_n]==Qt::SolidLine) ui->comboBox_2->setCurrentIndex(1);
    if (line_style[plot_n]==Qt::DashLine) ui->comboBox_2->setCurrentIndex(2);
    if (line_style[plot_n]==Qt::DotLine) ui->comboBox_2->setCurrentIndex(3);
    if (line_style[plot_n]==Qt::DashDotLine) ui->comboBox_2->setCurrentIndex(4);
    if (line_style[plot_n]==Qt::DashDotDotLine) ui->comboBox_2->setCurrentIndex(5);
    }

    {
    const QSignalBlocker blocker(ui->spinBox_6);
    ui->spinBox_6->setValue(symbol_size[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->doubleSpinBox_3);
    ui->doubleSpinBox_3->setValue(marker_x[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->doubleSpinBox_4);
    ui->doubleSpinBox_4->setValue(marker_y[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_12);
    if (marker_mode[plot_n]) ui->checkBox_12->setCheckState(Qt::Checked); else ui->checkBox_12->setCheckState(Qt::Unchecked);
    }

    {
    const QSignalBlocker blocker(ui->comboBox_4);
    ui->comboBox_4->setCurrentIndex(marker[plot_n]);
    if (marker[plot_n]==0) ui->comboBox_4->setEnabled(false);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_2);
    if (fix_x[plot_n]) ui->checkBox_2->setCheckState(Qt::Checked); else ui->checkBox_2->setCheckState(Qt::Unchecked);
    }
    {
    const QSignalBlocker blocker(ui->checkBox_3);
    if (fix_y[plot_n]) ui->checkBox_3->setCheckState(Qt::Checked); else ui->checkBox_3->setCheckState(Qt::Unchecked);
    }

    {
    const QSignalBlocker blocker(ui->comboBox_5);
    ui->comboBox_5->setCurrentIndex(x_axis_type[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->comboBox_6);
    ui->comboBox_6->setCurrentIndex(y_axis_type[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->spinBox_9);
    ui->spinBox_9->setValue(ticks_x[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->spinBox_10);
    ui->spinBox_10->setValue(ticks_y[plot_n]);
    }


    {
    const QSignalBlocker blocker(ui->checkBox_4);
    if (com_mode[plot_n]) ui->checkBox_4->setCheckState(Qt::Checked); else ui->checkBox_4->setCheckState(Qt::Unchecked);
    }

    {
    const QSignalBlocker blocker(ui->comboBox_3);
    ui->comboBox_3->setCurrentIndex(com_hide[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->comboBox_7);
    ui->comboBox_7->setCurrentIndex(label_mode[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->comboBox_8);
    ui->comboBox_8->setCurrentIndex(usemode[plot_n]);
    }

    //{
    //const QSignalBlocker blocker(ui->comboBox_9);
    //ui->comboBox_9->setCurrentIndex(error[plot_n]);
    //}

    {
    const QSignalBlocker blocker(ui->doubleSpinBox_5);
    ui->doubleSpinBox_5->setValue(limit_x1[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_6);
    ui->doubleSpinBox_6->setValue(limit_x2[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_7);
    ui->doubleSpinBox_7->setValue(limit_y1[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_8);
    ui->doubleSpinBox_8->setValue(limit_y2[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->checkBox_5);
    if (lock_aspect[plot_n]) ui->checkBox_5->setCheckState(Qt::Checked); else ui->checkBox_5->setCheckState(Qt::Unchecked);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_9);
    ui->doubleSpinBox_9->setValue(repeat_x1[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_10);
    ui->doubleSpinBox_10->setValue(repeat_x2[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_11);
    ui->doubleSpinBox_11->setValue(repeat_y1[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_12);
    ui->doubleSpinBox_12->setValue(repeat_y2[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->spinBox_7);
    ui->spinBox_7->setValue(repeat_x[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->spinBox_8);
    ui->spinBox_8->setValue(repeat_y[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->checkBox_8);
    if (fold_x[plot_n]) ui->checkBox_8->setCheckState(Qt::Checked); else ui->checkBox_8->setCheckState(Qt::Unchecked);
    }
    {
    const QSignalBlocker blocker(ui->checkBox_9);
    if (fold_y[plot_n]) ui->checkBox_9->setCheckState(Qt::Checked); else ui->checkBox_9->setCheckState(Qt::Unchecked);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_10);
    if (background[plot_n]) ui->checkBox_10->setCheckState(Qt::Checked); else ui->checkBox_10->setCheckState(Qt::Unchecked);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_15);
    ui->doubleSpinBox_15->setValue(background_x1[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_16);
    ui->doubleSpinBox_16->setValue(background_x2[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_17);
    ui->doubleSpinBox_17->setValue(background_y1[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_18);
    ui->doubleSpinBox_18->setValue(background_y2[plot_n]);
    }

    {
    const QSignalBlocker blocker(ui->comboBox_10);
    ui->comboBox_10->setCurrentIndex(colorLegend_type[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_13);
    ui->doubleSpinBox_13->setValue(colorLegend_min[plot_n]);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_14);
    ui->doubleSpinBox_14->setValue(colorLegend_max[plot_n]);
    }
    ui->lineEdit_4->setText(colorLegend_titel[plot_n]->toLatin1());
    //{
    //const QSignalBlocker blocker(ui->comboBox_9);
    //ui->comboBox_9->setCurrentIndex(colorScheme[plot_n]);
    //}
}

void EditGraph::applyGlobal(){
    int plot_n=ui->spinBox->value();
    setTitel(ui->lineEdit->text(),ui->lineEdit_2->text(),ui->lineEdit_3->text(),plot_n);
    setTextSize(ui->spinBox_3->value(),ui->spinBox_2->value(),ui->spinBox_4->value(),plot_n);
    setMultiplicator(ui->doubleSpinBox->value(),ui->doubleSpinBox_2->value(),plot_n);
    setLineWidth(ui->spinBox_5->value(),plot_n);
    if (ui->comboBox_2->currentIndex()==0) setLineStyle(Qt::NoPen,plot_n);
    if (ui->comboBox_2->currentIndex()==1) setLineStyle(Qt::SolidLine,plot_n);
    if (ui->comboBox_2->currentIndex()==2) setLineStyle(Qt::DashLine,plot_n);
    if (ui->comboBox_2->currentIndex()==3) setLineStyle(Qt::DotLine,plot_n);
    if (ui->comboBox_2->currentIndex()==4) setLineStyle(Qt::DashDotLine,plot_n);
    if (ui->comboBox_2->currentIndex()==5) setLineStyle(Qt::DashDotDotLine,plot_n);
    setSymbol(ui->spinBox_6->value(),plot_n);
    addMarker(ui->doubleSpinBox_3->value(),ui->doubleSpinBox_4->value(),ui->checkBox_12->checkState(),plot_n);
    setMark(data_mark[plot_n],ui->comboBox_4->currentIndex(),plot_n);
    fixRange(ui->checkBox_2->checkState(),ui->checkBox_3->checkState(),plot_n);
    setAxisType(ui->comboBox_5->currentIndex(),ui->comboBox_6->currentIndex(),plot_n);
    setComment(dat_com[plot_n],ui->checkBox_4->checkState(),plot_n);
    setComHide(ui->comboBox_3->currentIndex(),plot_n);
    setLabel(dat_label[plot_n],ui->comboBox_7->currentIndex(),plot_n);
    setUse(useflag[plot_n],ui->comboBox_8->currentIndex(),plot_n);
    //setError(data_err[plot_n],ui->comboBox_9->currentIndex(),plot_n);
    setTicks(ui->spinBox_9->value(),ui->spinBox_10->value(),plot_n);
    setLimit(ui->doubleSpinBox_5->value(),ui->doubleSpinBox_6->value(),ui->doubleSpinBox_7->value(),ui->doubleSpinBox_8->value(),plot_n);
    setLockAspect(ui->checkBox_5->checkState(),plot_n);
    setRepeat(ui->spinBox_7->value(),ui->doubleSpinBox_9->value(),ui->doubleSpinBox_10->value(),ui->spinBox_8->value(),ui->doubleSpinBox_11->value(),ui->doubleSpinBox_12->value(),plot_n);
    setFolding(ui->checkBox_8->checkState(),ui->checkBox_9->checkState(),plot_n);
    //if (*background_filename[plot_n]!="" && background[plot_n]>0) setBackground(*background_filename[plot_n],ui->doubleSpinBox_15->value(),ui->doubleSpinBox_16->value(),ui->doubleSpinBox_17->value(),ui->doubleSpinBox_18->value(),ui->checkBox_10->checkState(),plot_n);
    //if (background_fileimage[plot_n]!=nullptr && background[plot_n]>0) setBackground(background_fileimage[plot_n],ui->doubleSpinBox_15->value(),ui->doubleSpinBox_16->value(),ui->doubleSpinBox_17->value(),ui->doubleSpinBox_18->value(),ui->checkBox_10->checkState(),plot_n);
    //if (background_image[plot_n]!=nullptr && background[plot_n]>0) setBackground2(background_image[plot_n],ui->doubleSpinBox_15->value(),ui->doubleSpinBox_16->value(),ui->doubleSpinBox_17->value(),ui->doubleSpinBox_18->value(),ui->checkBox_10->checkState(),plot_n);
    setColorLegend(ui->comboBox_10->currentIndex(),ui->lineEdit_4->text().toLatin1(),ui->doubleSpinBox_13->value(),ui->doubleSpinBox_14->value(),plot_n);
    //setColorScheme(ui->comboBox_9->currentIndex(),plot_n);
    autoSize(plot_n);
}

void EditGraph::changeColor(){
    QColorDialog cdiag;
    QColor col(cdiag.getColor(ui->graphicsView_2->backgroundBrush().color(),this));
    if (col.isValid()){
        setLineColor(col,ui->spinBox->value());
        ui->graphicsView_2->setBackgroundBrush(col);
    }
    autoSize(ui->spinBox->value());
}

void EditGraph::setupLocalTab(){
    int plot_n=ui->spinBox->value();

    {
    const QSignalBlocker blocker(ui->checkBox_6);
        if (setsymbolsize_mode[plot_n]) ui->checkBox_6->setCheckState(Qt::Checked); else ui->checkBox_6->setCheckState(Qt::Unchecked);
    }
    if (setsymbolsize[plot_n]!=nullptr && setsymbolsize_mode[plot_n]==1){
        modelSymbolsize = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelSymbolsize->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelSymbolsize->setVerticalHeaderItem(0, new QStandardItem("Symbolsize"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setsymbolsize[plot_n][c]));
            modelSymbolsize->setItem(0,c,var);
        }
        ui->tableView_2->setModel(modelSymbolsize);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_2->setFont(font);
        ui->tableView_2->horizontalHeader()->setFont(font);
        ui->tableView_2->verticalHeader()->setFont(font);
        ui->tableView_2->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_2->setSortingEnabled(0);
        ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
        connect(modelSymbolsize,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editSymbolsize(QModelIndex,QModelIndex)));
    } else {
        modelSymbolsize = new QStandardItemModel(0,0,this);
        ui->tableView_2->setModel(modelSymbolsize);
    }


    {
    const QSignalBlocker blocker(ui->checkBox_7);
        if (setsymboltype_mode[plot_n]) ui->checkBox_7->setCheckState(Qt::Checked); else ui->checkBox_7->setCheckState(Qt::Unchecked);
    }

    if (setsymboltype[plot_n]!=nullptr && setsymboltype_mode[plot_n]==1){
        modelSymboltype = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelSymboltype->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelSymboltype->setVerticalHeaderItem(0, new QStandardItem("Symboltype"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setsymboltype[plot_n][c]));
            modelSymboltype->setItem(0,c,var);
        }
        ui->tableView_3->setModel(modelSymboltype);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_3->setFont(font);
        ui->tableView_3->horizontalHeader()->setFont(font);
        ui->tableView_3->verticalHeader()->setFont(font);
        ui->tableView_3->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_3->setSortingEnabled(0);
        ui->tableView_3->verticalHeader()->setDefaultSectionSize(ui->tableView_3->verticalHeader()->minimumSectionSize());
        connect(modelSymboltype,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editSymboltype(QModelIndex,QModelIndex)));
    } else {
        modelSymboltype = new QStandardItemModel(0,0,this);
        ui->tableView_3->setModel(modelSymboltype);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_11);
        if (setcolor_mode[plot_n]) ui->checkBox_11->setCheckState(Qt::Checked); else ui->checkBox_11->setCheckState(Qt::Unchecked);
    }


    {
    const QSignalBlocker blocker(ui->tableView_4);
    if (setcolor_color[plot_n]!=nullptr){
        modelSymbolcolor = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelSymbolcolor->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelSymbolcolor->setVerticalHeaderItem(0, new QStandardItem("Symbolcolor"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setcolor_color[plot_n][c].red())+" : "+QString::number(setcolor_color[plot_n][c].green())+" : "+QString::number(setcolor_color[plot_n][c].blue()));
            var->setBackground(setcolor_color[plot_n][c]);
            modelSymbolcolor->setItem(0,c,var);
        }
        ui->tableView_4->setModel(modelSymbolcolor);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_4->setFont(font);
        ui->tableView_4->horizontalHeader()->setFont(font);
        ui->tableView_4->verticalHeader()->setFont(font);
        ui->tableView_4->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_4->setSortingEnabled(0);
        ui->tableView_4->verticalHeader()->setDefaultSectionSize(ui->tableView_4->verticalHeader()->minimumSectionSize());
        connect(modelSymbolcolor,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editSymbolcolor(QModelIndex,QModelIndex)));
    } else {
        modelSymbolcolor = new QStandardItemModel(0,0,this);
        ui->tableView_4->setModel(modelSymbolcolor);
    }
    }

    {
    const QSignalBlocker blocker(ui->checkBox_13);
        if (setlinewidth_mode[plot_n]) ui->checkBox_13->setCheckState(Qt::Checked); else ui->checkBox_13->setCheckState(Qt::Unchecked);
    }
    if (setlinewidth[plot_n]!=nullptr && setlinewidth_mode[plot_n]==1){
        modelLinewidth = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelLinewidth->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelLinewidth->setVerticalHeaderItem(0, new QStandardItem("Linewidth"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setlinewidth[plot_n][c]));
            modelLinewidth->setItem(0,c,var);
        }
        ui->tableView_5->setModel(modelLinewidth);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_5->setFont(font);
        ui->tableView_5->horizontalHeader()->setFont(font);
        ui->tableView_5->verticalHeader()->setFont(font);
        ui->tableView_5->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_5->setSortingEnabled(0);
        ui->tableView_5->verticalHeader()->setDefaultSectionSize(ui->tableView_5->verticalHeader()->minimumSectionSize());
        connect(modelLinewidth,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editLinewidth(QModelIndex,QModelIndex)));
    } else {
        modelLinewidth = new QStandardItemModel(0,0,this);
        ui->tableView_5->setModel(modelLinewidth);
    }


    {
    const QSignalBlocker blocker(ui->checkBox_14);
        if (setlinestyle_mode[plot_n]) ui->checkBox_14->setCheckState(Qt::Checked); else ui->checkBox_14->setCheckState(Qt::Unchecked);
    }

    if (setlinestyle[plot_n]!=nullptr && setlinestyle_mode[plot_n]==1){
        modelLinestyle = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelLinestyle->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelLinestyle->setVerticalHeaderItem(0, new QStandardItem("Linestyle"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setlinestyle[plot_n][c]));
            modelLinestyle->setItem(0,c,var);
        }
        ui->tableView_6->setModel(modelLinestyle);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_6->setFont(font);
        ui->tableView_6->horizontalHeader()->setFont(font);
        ui->tableView_6->verticalHeader()->setFont(font);
        ui->tableView_6->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_6->setSortingEnabled(0);
        ui->tableView_6->verticalHeader()->setDefaultSectionSize(ui->tableView_6->verticalHeader()->minimumSectionSize());
        connect(modelLinestyle,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editLinestyle(QModelIndex,QModelIndex)));
    } else {
        modelLinestyle = new QStandardItemModel(0,0,this);
        ui->tableView_6->setModel(modelLinestyle);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_15);
        if (setlinecolor_mode[plot_n]) ui->checkBox_15->setCheckState(Qt::Checked); else ui->checkBox_15->setCheckState(Qt::Unchecked);
    }


    {
    const QSignalBlocker blocker(ui->tableView_7 );
    if (setlinecolor_color[plot_n]!=nullptr && setlinecolor_mode[plot_n]==1){
        modelLinecolor = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelLinecolor->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelLinecolor->setVerticalHeaderItem(0, new QStandardItem("Linecolor"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setlinecolor_color[plot_n][c].red())+" : "+QString::number(setlinecolor_color[plot_n][c].green())+" : "+QString::number(setlinecolor_color[plot_n][c].blue()));
            var->setBackground(setlinecolor_color[plot_n][c]);
            modelLinecolor->setItem(0,c,var);
        }
        ui->tableView_7->setModel(modelLinecolor);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_7->setFont(font);
        ui->tableView_7->horizontalHeader()->setFont(font);
        ui->tableView_7->verticalHeader()->setFont(font);
        ui->tableView_7->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_7->setSortingEnabled(0);
        ui->tableView_7->verticalHeader()->setDefaultSectionSize(ui->tableView_7->verticalHeader()->minimumSectionSize());
        connect(modelLinecolor,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editLinecolor(QModelIndex,QModelIndex)));
    } else {
        modelLinecolor = new QStandardItemModel(0,0,this);
        ui->tableView_7->setModel(modelLinecolor);
    }
    }

    {
    const QSignalBlocker blocker(ui->checkBox_16);
        if (setpoly_mode[plot_n]) ui->checkBox_16->setCheckState(Qt::Checked); else ui->checkBox_16->setCheckState(Qt::Unchecked);
    }
    if (setpoly[plot_n]!=nullptr&&setpoly_mode[plot_n]==1){
        modelPoly = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelPoly->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelPoly->setVerticalHeaderItem(0, new QStandardItem("Polymode"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setpoly[plot_n][c]));
            modelPoly->setItem(0,c,var);
        }
        ui->tableView_8->setModel(modelPoly);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_8->setFont(font);
        ui->tableView_8->horizontalHeader()->setFont(font);
        ui->tableView_8->verticalHeader()->setFont(font);
        ui->tableView_8->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_8->setSortingEnabled(0);
        ui->tableView_8->verticalHeader()->setDefaultSectionSize(ui->tableView_8->verticalHeader()->minimumSectionSize());
        connect(modelPoly,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editPoly(QModelIndex,QModelIndex)));
    } else {
        modelPoly = new QStandardItemModel(0,0,this);
        ui->tableView_8->setModel(modelPoly);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_17);
        if (setarea_mode[plot_n]) ui->checkBox_17->setCheckState(Qt::Checked); else ui->checkBox_17->setCheckState(Qt::Unchecked);
    }
    if (setarea_x[plot_n]!=nullptr && setarea_y[plot_n]!=nullptr&&setarea_mode[plot_n]==1){
        modelArea = new QStandardItemModel(2,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelArea->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelArea->setVerticalHeaderItem(0, new QStandardItem("Areasize X"));
        modelArea->setVerticalHeaderItem(1, new QStandardItem("Areasize Y"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(setarea_x[plot_n][c]));
            modelSymbolsize->setItem(0,c,var);
            QStandardItem *var2=new QStandardItem(QString::number(setarea_y[plot_n][c]));
            modelSymbolsize->setItem(1,c,var2);
        }
        ui->tableView_9->setModel(modelArea);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_9->setFont(font);
        ui->tableView_9->horizontalHeader()->setFont(font);
        ui->tableView_9->verticalHeader()->setFont(font);
        ui->tableView_9->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_9->setSortingEnabled(0);
        ui->tableView_9->verticalHeader()->setDefaultSectionSize(ui->tableView_9->verticalHeader()->minimumSectionSize());
        connect(modelArea,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editArea(QModelIndex,QModelIndex)));
    } else {
        modelArea = new QStandardItemModel(0,0,this);
        ui->tableView_9->setModel(modelArea);
    }

    {
    const QSignalBlocker blocker(ui->checkBox_18);
        if (autosize_mode[plot_n]) ui->checkBox_18->setCheckState(Qt::Checked); else ui->checkBox_18->setCheckState(Qt::Unchecked);
    }
    if (autosize[plot_n]!=nullptr&&autosize_mode[plot_n]==1){
        modelAutosize = new QStandardItemModel(1,column[plot_n],this);
        for (int i=0;i<column[plot_n];i++) modelAutosize->setHorizontalHeaderItem(i, new QStandardItem(QString::number(i)));
        modelAutosize->setVerticalHeaderItem(0, new QStandardItem("Autosize"));
        for (int c=0;c<column[plot_n];c++){
            QStandardItem *var=new QStandardItem(QString::number(autosize[plot_n][c]));
            modelAutosize->setItem(0,c,var);
        }
        ui->tableView_10->setModel(modelAutosize);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_10->setFont(font);
        ui->tableView_10->horizontalHeader()->setFont(font);
        ui->tableView_10->verticalHeader()->setFont(font);
        ui->tableView_10->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_10->setSortingEnabled(0);
        ui->tableView_10->verticalHeader()->setDefaultSectionSize(ui->tableView_10->verticalHeader()->minimumSectionSize());
        connect(modelAutosize,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editAutosize(QModelIndex,QModelIndex)));
    } else {
        modelAutosize = new QStandardItemModel(0,0,this);
        ui->tableView_10->setModel(modelAutosize);
    }
}

void EditGraph::applyLocal(){
    int plot_n=ui->spinBox->value();
    if (ui->checkBox_6->checkState()==Qt::Unchecked) graph->setSetSymbolsize(setsymbolsize[plot_n],ui->checkBox_6->checkState(),plot_n);
    if (ui->checkBox_6->checkState()==Qt::Checked && setsymbolsize[plot_n]!=nullptr) graph->setSetSymbolsize(setsymbolsize[plot_n],ui->checkBox_6->checkState(),plot_n);
    if (ui->checkBox_7->checkState()==Qt::Unchecked) graph->setSetSymboltype(setsymboltype[plot_n],ui->checkBox_7->checkState(),plot_n);
    if (ui->checkBox_7->checkState()==Qt::Checked && setsymboltype[plot_n]!=nullptr) graph->setSetSymboltype(setsymboltype[plot_n],ui->checkBox_7->checkState(),plot_n);
    if (ui->checkBox_11->checkState()==Qt::Unchecked) graph->setSetColor(setcolor_color[plot_n],ui->checkBox_11->checkState(),plot_n);
    if (ui->checkBox_11->checkState()==Qt::Checked && setcolor_color[plot_n]!=nullptr) graph->setSetColor(setcolor_color[plot_n],ui->checkBox_11->checkState(),plot_n);
    if (ui->checkBox_13->checkState()==Qt::Unchecked) graph->setSetLineWidth(setlinewidth[plot_n],ui->checkBox_13->checkState(),plot_n);
    if (ui->checkBox_13->checkState()==Qt::Checked && setlinewidth[plot_n]!=nullptr) graph->setSetLineWidth(setlinewidth[plot_n],ui->checkBox_13->checkState(),plot_n);
    if (ui->checkBox_14->checkState()==Qt::Unchecked) graph->setSetLineStyle(setlinestyle[plot_n],ui->checkBox_14->checkState(),plot_n);
    if (ui->checkBox_14->checkState()==Qt::Checked && setlinestyle[plot_n]!=nullptr) graph->setSetLineStyle(setlinestyle[plot_n],ui->checkBox_14->checkState(),plot_n);
    if (ui->checkBox_15->checkState()==Qt::Unchecked) graph->setSetLineColor(setlinecolor_color[plot_n],ui->checkBox_15->checkState(),plot_n);
    if (ui->checkBox_15->checkState()==Qt::Checked && setlinecolor_color[plot_n]!=nullptr) graph->setSetLineColor(setlinecolor_color[plot_n],ui->checkBox_15->checkState(),plot_n);
    if (ui->checkBox_16->checkState()==Qt::Unchecked) graph->setSetPoly(setpoly[plot_n],ui->checkBox_16->checkState(),plot_n);
    if (ui->checkBox_16->checkState()==Qt::Checked && setpoly[plot_n]!=nullptr) graph->setSetPoly(setpoly[plot_n],ui->checkBox_16->checkState(),plot_n);
    if (ui->checkBox_17->checkState()==Qt::Unchecked) graph->setSetArea(setarea_x[plot_n],setarea_y[plot_n],ui->checkBox_17->checkState(),plot_n);
    if (ui->checkBox_17->checkState()==Qt::Checked && setarea_x[plot_n]!=nullptr && setarea_y[plot_n]!=nullptr) graph->setSetArea(setarea_x[plot_n],setarea_y[plot_n],ui->checkBox_17->checkState(),plot_n);
    if (ui->checkBox_18->checkState()==Qt::Unchecked) graph->setAutosize(autosize[plot_n],ui->checkBox_18->checkState(),plot_n);
    if (ui->checkBox_18->checkState()==Qt::Checked && autosize[plot_n]!=nullptr) graph->setAutosize(autosize[plot_n],ui->checkBox_18->checkState(),plot_n);

    autoSize(plot_n);

}

void EditGraph::editSymbolsize(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    setsymbolsize[plot_n][r.column()]=modelSymbolsize->item(r.row(),r.column())->text().toInt();
    graph->setSetSymbolsize(setsymbolsize[plot_n],ui->checkBox_6->checkState(),plot_n);
    graph->autoSize();
    update();
}

void EditGraph::editSymboltype(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    setsymboltype[plot_n][r.column()]=modelSymboltype->item(r.row(),r.column())->text().toInt();
    graph->setSetSymboltype(setsymboltype[plot_n],ui->checkBox_7->checkState(),plot_n);
    graph->autoSize();
    update();
}

void EditGraph::editSymbolcolor(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    QString str=modelSymbolcolor->item(r.row(),r.column())->text();
    QStringList strlist=str.split(":");
    if (strlist.length()==3){
        QColor col=QColor(strlist[0].toInt(),strlist[1].toInt(),strlist[2].toInt());
        setcolor_color[plot_n][r.column()]=col;
        graph->setSetColor(setcolor_color[plot_n],ui->checkBox_11->checkState(),plot_n);
        graph->autoSize();
        update();
        setupLocalTab();
    }
}

void EditGraph::editLinewidth(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    setlinewidth[plot_n][r.column()]=modelLinewidth->item(r.row(),r.column())->text().toInt();
    graph->setSetLineWidth(setlinewidth[plot_n],ui->checkBox_13->checkState(),plot_n);
    graph->autoSize();
    update();
}

void EditGraph::editLinestyle(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    Qt::PenStyle pen=Qt::NoPen;
    if (modelLinestyle->item(r.row(),r.column())->text().toInt()==0) pen=Qt::NoPen;
    if (modelLinestyle->item(r.row(),r.column())->text().toInt()==1) pen=Qt::SolidLine;
    if (modelLinestyle->item(r.row(),r.column())->text().toInt()==2) pen=Qt::DotLine;
    if (modelLinestyle->item(r.row(),r.column())->text().toInt()==3) pen=Qt::DashLine;
    if (modelLinestyle->item(r.row(),r.column())->text().toInt()==4) pen=Qt::DashDotLine;
    if (modelLinestyle->item(r.row(),r.column())->text().toInt()==5) pen=Qt::DashDotDotLine;

    if (modelLinestyle->item(r.row(),r.column())->text().toInt()>=0&&modelLinestyle->item(r.row(),r.column())->text().toInt()<=5)setlinestyle[plot_n][r.column()]=pen;
    graph->setSetLineStyle(setlinestyle[plot_n],ui->checkBox_14->checkState(),plot_n);
    graph->autoSize();
    update();
}

void EditGraph::editLinecolor(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    QString str=modelLinecolor->item(r.row(),r.column())->text();
    QStringList strlist=str.split(":");
    if (strlist.length()==3){
        QColor col=QColor(strlist[0].toInt(),strlist[1].toInt(),strlist[2].toInt());
        setlinecolor_color[plot_n][r.column()]=col;
        graph->setSetLineColor(setlinecolor_color[plot_n],ui->checkBox_15->checkState(),plot_n);
        graph->autoSize();
        update();
        setupLocalTab();
    }
}

void EditGraph::editPoly(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    setpoly[plot_n][r.column()]=modelPoly->item(r.row(),r.column())->text().toInt();
    graph->setSetPoly(setpoly[plot_n],ui->checkBox_16->checkState(),plot_n);
    graph->autoSize();
    update();
}
void EditGraph::editArea(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    setarea_x[plot_n][r.column()]=modelArea->item(0,r.column())->text().toInt();
    setarea_y[plot_n][r.column()]=modelArea->item(1,r.column())->text().toInt();
    graph->setSetArea(setarea_x[plot_n],setarea_y[plot_n],ui->checkBox_17->checkState(),plot_n);
    graph->autoSize();
    update();
}
void EditGraph::editAutosize(QModelIndex r,QModelIndex c){
    int plot_n=ui->spinBox->value();
    autosize[plot_n][r.column()]=modelAutosize->item(r.row(),r.column())->text().toInt();
    graph->setAutosize(autosize[plot_n],ui->checkBox_18->checkState(),plot_n);
    graph->autoSize();
    update();
}

void EditGraph::setupObjectTab(){
    int plot_n=ui->spinBox->value();

    {
    const QSignalBlocker blocker(ui->checkBox_19);
        if (object_mode) ui->checkBox_19->setCheckState(Qt::Checked); else ui->checkBox_19->setCheckState(Qt::Unchecked);
    }
    if (object_line_x1.size()>0){
        modelObject = new QStandardItemModel(object_line_x1.size(),9,this);
        modelObject->setHorizontalHeaderItem(0, new QStandardItem("X1"));
        modelObject->setHorizontalHeaderItem(1, new QStandardItem("Y1"));
        modelObject->setHorizontalHeaderItem(2, new QStandardItem("Plot1"));
        modelObject->setHorizontalHeaderItem(3, new QStandardItem("X2"));
        modelObject->setHorizontalHeaderItem(4, new QStandardItem("Y2"));
        modelObject->setHorizontalHeaderItem(5, new QStandardItem("Plot2"));
        modelObject->setHorizontalHeaderItem(6, new QStandardItem("Color"));
        modelObject->setHorizontalHeaderItem(7, new QStandardItem("Width"));
        modelObject->setHorizontalHeaderItem(8, new QStandardItem("Style"));
        for (int i=0;i<object_line_x1.size();i++) modelObject->setVerticalHeaderItem(i, new QStandardItem(QString::number(i)));

        for (int r=0;r<object_line_x1.size();r++){
            QStandardItem *var=new QStandardItem(QString::number(object_line_x1[r]));
            modelObject->setItem(r,0,var);
            QStandardItem *var2=new QStandardItem(QString::number(object_line_y1[r]));
            modelObject->setItem(r,1,var2);
            QStandardItem *var3=new QStandardItem(QString::number(object_line_n1[r]));
            modelObject->setItem(r,2,var3);
            QStandardItem *var4=new QStandardItem(QString::number(object_line_x2[r]));
            modelObject->setItem(r,3,var4);
            QStandardItem *var5=new QStandardItem(QString::number(object_line_y2[r]));
            modelObject->setItem(r,4,var5);
            QStandardItem *var6=new QStandardItem(QString::number(object_line_n2[r]));
            modelObject->setItem(r,5,var6);
            QStandardItem *var7=new QStandardItem(QString::number(object_line_color[r].red())+" : "+QString::number(object_line_color[r].green())+" : "+QString::number(object_line_color[r].blue()));
            var7->setBackground(object_line_color[r]);
            modelObject->setItem(r,6,var7);
            QStandardItem *var8=new QStandardItem(QString::number(object_line_width[r]));
            modelObject->setItem(r,7,var8);
            QStandardItem *var9=new QStandardItem(QString::number(object_line_style[r]));
            modelObject->setItem(r,8,var9);
        }
        ui->tableView_11->setModel(modelObject);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView_11->setFont(font);
        ui->tableView_11->horizontalHeader()->setFont(font);
        ui->tableView_11->verticalHeader()->setFont(font);
        ui->tableView_11->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableView_11->setSortingEnabled(0);
        ui->tableView_11->verticalHeader()->setDefaultSectionSize(ui->tableView_11->verticalHeader()->minimumSectionSize());
        connect(modelObject,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editLines(QModelIndex,QModelIndex)));
    } else {
        modelObject = new QStandardItemModel(0,9,this);
        modelObject->setHorizontalHeaderItem(0, new QStandardItem("X1"));
        modelObject->setHorizontalHeaderItem(1, new QStandardItem("Y1"));
        modelObject->setHorizontalHeaderItem(2, new QStandardItem("Plot1"));
        modelObject->setHorizontalHeaderItem(3, new QStandardItem("X2"));
        modelObject->setHorizontalHeaderItem(4, new QStandardItem("Y2"));
        modelObject->setHorizontalHeaderItem(5, new QStandardItem("Plot2"));
        modelObject->setHorizontalHeaderItem(6, new QStandardItem("Color"));
        modelObject->setHorizontalHeaderItem(7, new QStandardItem("Width"));
        modelObject->setHorizontalHeaderItem(8, new QStandardItem("Style"));
        ui->tableView_11->setModel(modelObject);
    }

}

void EditGraph::applyObjects(){
    graph->addLineClear();

    for (int i=0;i<object_line_x1.size();i++) {

        graph->addLine(object_line_x1[i],object_line_y1[i],object_line_n1[i],object_line_x2[i],object_line_y2[i],object_line_n2[i],object_line_color[i],object_line_width[i],object_line_style[i]);
    }
    if (ui->checkBox_19->checkState()==Qt::Checked) object_mode=1;
    if (ui->checkBox_19->checkState()==Qt::Unchecked) object_mode=0;
    setAddMode(object_mode);
    for (int i=0;i<ui->spinBox->maximum()+1;i++) autoSize(i);
    setupObjectTab();
}

void EditGraph::editLines(QModelIndex r,QModelIndex c){


    if (r.column()==0) object_line_x1[r.row()]=modelObject->item(r.row(),r.column())->text().toFloat();
    if (r.column()==1) object_line_y1[r.row()]=modelObject->item(r.row(),r.column())->text().toFloat();
    if (r.column()==2) object_line_n1[r.row()]=modelObject->item(r.row(),r.column())->text().toInt();
    if (r.column()==3) object_line_x2[r.row()]=modelObject->item(r.row(),r.column())->text().toFloat();
    if (r.column()==4) object_line_y2[r.row()]=modelObject->item(r.row(),r.column())->text().toFloat();
    if (r.column()==5) object_line_n2[r.row()]=modelObject->item(r.row(),r.column())->text().toInt();
    if (r.column()==6) {
        QString str=modelObject->item(r.row(),r.column())->text();
        QStringList strlist=str.split(":");
        if (strlist.length()==3){
            QColor col=QColor(strlist[0].toInt(),strlist[1].toInt(),strlist[2].toInt());
            object_line_color[r.row()]=col;
        }
    }
    if (r.column()==7) object_line_width[r.row()]=modelObject->item(r.row(),r.column())->text().toInt();
    if (r.column()==8) {
        int style=modelObject->item(r.row(),r.column())->text().toInt();
        Qt::PenStyle pen=Qt::NoPen;
        if (style==0) pen=Qt::NoPen;
        if (style==1) pen=Qt::SolidLine;
        if (style==2) pen=Qt::DotLine;
        if (style==3) pen=Qt::DashLine;
        if (style==4) pen=Qt::DashDotLine;
        if (style==5) pen=Qt::DashDotDotLine;
        if (style>=0&&style<=5) object_line_style[r.row()]=pen;
    }


    applyObjects();
}

void EditGraph::setSettings(QString p,int n){
    path[n]->clear();
    path[n]->append(p);
    ui->lineEdit_5->setText(path[n]->toLatin1());
    graph->setSettings(*path[n]);
}

void EditGraph::setupOptionsTab(){
    int n=ui->spinBox->value();
     ui->lineEdit_5->setText(path[n]->toLatin1());
     update();
}

void EditGraph::saveSettings(){
    int n=ui->spinBox->value();
    graph->saveSettings(n);
    QMessageBox msgBox;
            msgBox.setText("The settings-file was saved");
            msgBox.setInformativeText("In future the new settings-file will be used.");
            msgBox.setIcon(QMessageBox::Warning);

            msgBox.exec();
}

void EditGraph::deleteSettings(){
    int n=ui->spinBox->value();
    QFile file (path[n]->toLatin1());
    file.remove();
    QMessageBox msgBox;
            msgBox.setText("The settings-file was deleted");
            msgBox.setInformativeText("In future the default settings will be used.");
            msgBox.setIcon(QMessageBox::Warning);

            msgBox.exec();
}
void EditGraph::readSettings(){
    int n=ui->spinBox->value();
    graph->readSettings(n);
    QMessageBox msgBox;
            msgBox.setText("The settings-file was reloaded");
            msgBox.setInformativeText("In future the new settings will be used.");
            msgBox.setIcon(QMessageBox::Warning);

            msgBox.exec();
}

void EditGraph::saveImage(){
    QString imagePath = QFileDialog::getSaveFileName(this,tr("Save File"),"",tr("JPEG (*.jpg *.jpeg);;PNG (*.png)" ));
    if (imagePath!=""){
        const QPixmap pixmap = ui->graphicsView->grab();
        pixmap.save(imagePath);
    }
}

void EditGraph::closeEvent (QCloseEvent *event)
{
    emit(graphChangedEdit());
}
