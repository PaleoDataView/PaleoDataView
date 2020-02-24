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

#include "dtw.h"
#include "ui_dtw.h"

DTW::DTW(QMainWindow *mainWindow,Inventory *i) :
    mainW(mainWindow),inv(i),
    ui(new Ui::DTW)
{
    resources=new Resources();
    ui->setupUi(this);
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    //sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);

    amdata=new AMSData(inv);
    amdata->AMSRead();
    ui->label_2->setText("Carbon Isotope Data : "+inv->get_Core(inv->get_currentCore())+" : "+inv->get_Species(inv->get_currentCore()));
    inv->readTarget();
    ui->comboBox->clear();
    for(int i=0;i<inv->get_Target_Vari();i++) ui->comboBox->addItem(inv->get_Target_Name(i));

    selected=amdata->getSelect();

    // create Age model plot
    float *data1=new float[0];
    am_plot=new Graph(this,data1,0,0);
    createAMPlot();

    // create Pattern plot
    float *data5=new float[0];
    pat_plot=new Graph(this,data5,0,0);
    createPatPlot();

    // create Dist plot
    float *data6=new float[0];
    dist_plot=new Graph(this,data6,0,0);
    createDistPlot(0,0);

    // create Cost plot
    float *data7=new float[0];
    cost_plot=new Graph(this,data7,0,0);
    createCostPlot(0,0);

    // create result of tuning plot
    float *data2=new float[0];
    result_plot=new Graph(this,data2,0,0);
    createResultPlot();

    float *data4=new float[0];
    cor_plot=new Graph(this,data4,0,0);
    cor_plot->setNumber(2);
    createCorPlot(1);

    setupTable();

    connect(ui->comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(target_Selected(QString)));
    connect(ui->comboBox_3,SIGNAL(currentTextChanged(QString)),this,SLOT(pattern_Selected(QString)));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(tableSelected(QModelIndex)));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(calcGDTW()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(redraw()));
    changes=false;
}

DTW::~DTW()
{
    delete ui;
}

void DTW::paintEvent(QPaintEvent *)
{
    if (am_plot->getSelected_Y()>=0 && am_plot->getSelected_Y()<amdata->get_Length()){
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        for (int i=0;i<amdata->get_Length();i++){
            if (model->item(i,0)->text().toInt()==am_plot->getSelected_Y()){
                select=i;
                break;
            }
        }
        if (select>=0&&select<=amdata->get_Length()) {
            amdata->setSelect(select);
            setupTable();
        }
        am_plot->setSelected_Y(-1);
        createAMPlot();
    }
    if (cor_plot->getSelectLine()!=-1 && cor_plot->getSelectLine()<amdata->get_Length()){
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        for (int i=0;i<amdata->get_Length();i++){
            if (model->item(i,0)->text().toInt()==cor_plot->getSelectLine()){
                select=i;
                break;
            }
        }
        if (select>=0&&select<=amdata->get_Length()) {
            amdata->setSelect(select);
            setupTable();
        }
    }
    createLines();
    cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    am_plot->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    pat_plot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    dist_plot->setSize(ui->graphicsView_5->width(),ui->graphicsView_5->height());
    cost_plot->setSize(ui->graphicsView_6->width(),ui->graphicsView_6->height());
    result_plot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());
}

void DTW::setupTable(){


    // create the model for AMData
    modelData = new QStandardItemModel(amdata->get_Length(),12,this);
    modelData->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelData->setHorizontalHeaderItem(1, new QStandardItem(QString("Depth")));
    modelData->setHorizontalHeaderItem(2, new QStandardItem(QString("Label")));
    modelData->setHorizontalHeaderItem(3, new QStandardItem(QString("Type")));
    modelData->setHorizontalHeaderItem(4, new QStandardItem(QString("Age dated\n[ka]")));
    modelData->setHorizontalHeaderItem(5, new QStandardItem(QString("Age UCL\n[ka+]")));
    modelData->setHorizontalHeaderItem(6, new QStandardItem(QString("Age LCL\n[ka-]")));
    modelData->setHorizontalHeaderItem(7, new QStandardItem(QString("Res. Age\n[ka]")));
    modelData->setHorizontalHeaderItem(8, new QStandardItem(QString("Cal yrs\n[wm ka BP]")));
    modelData->setHorizontalHeaderItem(9, new QStandardItem(QString("Cal yrs min\n[95%]")));
    modelData->setHorizontalHeaderItem(10, new QStandardItem(QString("Cal yrs max\n[95%]")));
    modelData->setHorizontalHeaderItem(11, new QStandardItem(QString("Use Flag")));
    modelData->setHorizontalHeaderItem(12, new QStandardItem(QString("Comments")));

    ui->tableView->setModel(modelData);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStandardItem *var_Index = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Depth = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Label = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Type = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Age_dated = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Age_UCL = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Age_LCL = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Age_Res = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Cal = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Cal_Min = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Cal_Max = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Use_Flag = new QStandardItem[amdata->get_Length()];
    QStandardItem *var_Comment = new QStandardItem[amdata->get_Length()];

    for (int i=0;i<amdata->get_Length();i++){
            var_Index[i].setData(i,Qt::EditRole);
            modelData->setItem(i,0,&var_Index[i]);

            var_Depth[i].setData(amdata->get_Depth(i),Qt::EditRole);
            modelData->setItem(i,1,&var_Depth[i]);

            var_Label[i].setText(amdata->get_LabID(i));
            modelData->setItem(i,2,&var_Label[i]);

            var_Type[i].setText(amdata->get_Type(i));
            modelData->setItem(i,3,&var_Type[i]);

            var_Age_dated[i].setData(amdata->get_Data(0,i),Qt::EditRole);
            modelData->setItem(i,4,&var_Age_dated[i]);

            var_Age_UCL[i].setData(amdata->get_Data(1,i),Qt::EditRole);
            modelData->setItem(i,5,&var_Age_UCL[i]);

            var_Age_LCL[i].setData(amdata->get_Data(2,i),Qt::EditRole);
            modelData->setItem(i,6,&var_Age_LCL[i]);

            var_Age_Res[i].setData(amdata->get_Data(3,i),Qt::EditRole);
            modelData->setItem(i,7,&var_Age_Res[i]);

            var_Cal[i].setData(amdata->get_Data(4,i),Qt::EditRole);
            modelData->setItem(i,8,&var_Cal[i]);

            var_Cal_Min[i].setData(amdata->get_Data(5,i),Qt::EditRole);
            modelData->setItem(i,9,&var_Cal_Min[i]);

            var_Cal_Max[i].setData(amdata->get_Data(6,i),Qt::EditRole);
            modelData->setItem(i,10,&var_Cal_Max[i]);

            var_Use_Flag[i].setData(amdata->get_Data(7,i),Qt::EditRole);
            var_Use_Flag[i].setCheckable(true);
            var_Use_Flag[i].setText(QString::number(amdata->get_Data(7,i)));
            if (amdata->get_Data(7,i)) var_Use_Flag[i].setCheckState(Qt::Checked);
            modelData->setItem(i,11,&var_Use_Flag[i]);
            modelData->setData(modelData->index(i, 11), Qt::AlignCenter,Qt::TextAlignmentRole);

            var_Comment[i].setText(amdata->get_Age_Comment(i));
            modelData->setItem(i,12,&var_Comment[i]);




            // coloring
            if (amdata->get_Data(7,i)){
                modelData->setData(modelData->index(i, 11), QColor(Qt::green), Qt::BackgroundRole);
            } else {
                modelData->setData(modelData->index(i, 11), QColor(Qt::red), Qt::BackgroundRole);
            }


    }
    ui->tableView->setSortingEnabled(1);
    ui->tableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    for (int i=0;i<13;i++)if (i!=11)modelData->setData(modelData->index(amdata->getSelect(),i), QColor(Qt::gray), Qt::BackgroundRole);
    ui->tableView->scrollTo(modelData->index(amdata->getSelect(),0));
}

void DTW::redraw(){
    setupTable();
    pat_plot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    dist_plot->setSize(ui->graphicsView_5->width(),ui->graphicsView_5->height());
    cost_plot->setSize(ui->graphicsView_6->width(),ui->graphicsView_6->height());
}
void DTW::target_Selected(QString str){
    createCorPlot(1);
    cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());

    update();
}

void DTW::pattern_Selected(QString str){

    createPatPlot();
    pat_plot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());

    update();
}


void DTW::createAMPlot(){
    float *data1=new float[amdata->get_Length()*2];
    bool *data_use=new bool[amdata->get_Length()*2];
    float *data_error=new float[amdata->get_Length()*2];
    float *data_error2=new float[amdata->get_Length()*2];
    QColor *col=new QColor[amdata->get_Length()*2];

    int *mark=new int[amdata->get_Length()*2];
    QString *comment=new QString[amdata->get_Length()*2];
    for (int i=0;i<amdata->get_Length();i++){
        //data1[i+0*amdata->get_Length()]=amdata->get_Depth(i);
        //data1[i+1*amdata->get_Length()]=amdata->get_Data(0,i);
        data1[i+0*amdata->get_Length()]=amdata->get_Depth(i);
        data1[i+1*amdata->get_Length()]=amdata->get_Data(4,i);
        //data_use[i+0*amdata->get_Length()]=amdata->get_Data(7,i);
        //data_use[i+1*amdata->get_Length()]=amdata->get_Data(7,i);
        data_use[i+0*amdata->get_Length()]=amdata->get_Data(7,i);
        data_use[i+1*amdata->get_Length()]=amdata->get_Data(7,i);
        //data_error[i+0*amdata->get_Length()]=NAN;
        //data_error[i+1*amdata->get_Length()]=amdata->get_Data(2,i);
        data_error[i+0*amdata->get_Length()]=NAN;
        if (amdata->get_Data(5,i)!=0) data_error[i+1*amdata->get_Length()]=abs(amdata->get_Data(4,i)-amdata->get_Data(5,i)); else data_error[i+1*amdata->get_Length()]=NAN;
        //data_error2[i+0*amdata->get_Length()]=NAN;
        //data_error2[i+1*amdata->get_Length()]=amdata->get_Data(1,i);
        data_error2[i+0*amdata->get_Length()]=NAN;
        if (amdata->get_Data(6,i)!=0) data_error2[i+1*amdata->get_Length()]=abs(amdata->get_Data(4,i)-amdata->get_Data(6,i)); else data_error2[i+1*amdata->get_Length()]=NAN;
        //col_r[i+0*amdata->get_Length()]=255;
        //col_r[i+1*amdata->get_Length()]=255;
        col[i+0*amdata->get_Length()]=QColor(0,0,0);
        col[i+1*amdata->get_Length()]=QColor(0,0,255);

        //if (amdata->get_Age_Comment(i).simplified()!="NaN") comment[i+2*amdata->get_Length()]=amdata->get_Age_Comment(i);
        //mark[i+0*amdata->get_Length()]=0;
        //mark[i+1*amdata->get_Length()]=0;
        mark[i+0*amdata->get_Length()]=0;
        mark[i+1*amdata->get_Length()]=0;
    }
    //mark[select+0*amdata->get_Length()]=1;
    //mark[select+1*amdata->get_Length()]=1;
    mark[select+0*amdata->get_Length()]=1;
    mark[select+1*amdata->get_Length()]=1;
    am_plot->setData(data1,2,amdata->get_Length());
    am_plot->setUse(data_use,1);
    am_plot->setError(data_error,2);
    am_plot->setError2(data_error2,2);
    am_plot->setColor(col,1);
    am_plot->setMark(mark,1);
    //am_plot->setComment(comment,1);
    am_plot->setTitel("","Depth [m]","Age in [kyr]");
    am_plot->setMultiplicator(1,1);
    am_plot->setTextSize(resources->get_Pointsize(),1,0);
    am_plot->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    am_plot->setSymbol(3);

    am_plot->autoSize();
    am_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_AgeModel.txt",0);
    ui->graphicsView_3->setScene(am_plot);
}

void DTW::createResultPlot(){
    float *data1=new float[inv->get_Length()*2];
    QString *comment=new QString[inv->get_Length()*2];
    for (int i=0;i<inv->get_Length();i++){
        data1[i+0*inv->get_Length()]=amdata->get_Int_Value_Age(inv->get_data_Isotopes(0,i),"linear");
            data1[i+1*inv->get_Length()]=inv->get_data_Isotopes(3,i);
            result_plot->setMultiplicator(1,-1);
            result_plot->setTitel("","Age in [kyr]","\u03b418O [\u2030 VPDB]");

        if (inv->get_data_Comment(i).simplified()!="NaN") comment[i+1*inv->get_Length()]=inv->get_data_Comment(i);
    }
    if (amdata->get_Length()>1)  {
        result_plot->setData(data1,2,inv->get_Length());
        result_plot->setComment(comment,1);
        result_plot->setTextSize(resources->get_Pointsize(),1,0);
        result_plot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());
        result_plot->setSymbol(3);
        result_plot->autoSize();
    }
    result_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_Result.txt",0);
    ui->graphicsView_4->setScene(result_plot);
}

void DTW::createPatPlot(){
    // Init StepPattern
    stepPattern(ui->comboBox_3->currentText());
    int nsteps=dtw_step_ix.size();
    int npats=dtw_step_npat.at(dtw_step_ix.size()-1);

    float *data1=new float[npats*2*2];

    for (int i=0;i<npats;i++){
        for (int j=0;j<2;j++){
            data1[i*2*2+j]=dtw_step_ix.at(i*2+j);
            data1[(i*2+1)*2+j]=dtw_step_iy.at(i*2+j);
            pat_plot->setTitel("StepPattern","Isotope Index","Target Index");
        }
    }

    pat_plot->setData(data1,npats*2,2);
    pat_plot->setTextSize(resources->get_Pointsize(),1,0);
    pat_plot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    pat_plot->setSymbol(0);
    pat_plot->setLineColor(QColor(255,0,0));
    pat_plot->setLineWidth(5);
    pat_plot->autoSize();

    pat_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_Pattern.txt",0);
    ui->graphicsView_2->setScene(pat_plot);
}


bool DTW::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->tableView||ui->graphicsView_3||obj==ui->graphicsView_4){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);
                ui->splitter_2->restoreState(sp_2);
                //ui->splitter_3->restoreState(sp_3);
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void DTW::createCorPlot(int autosize){

    // Get Isotope data
    int y_axis=0;
    //cor_plot->setNumber(2);
    float *data1=new float[inv->get_Length()*2];
    for (int i=0;i<inv->get_Length();i++){
        data1[i+0*inv->get_Length()]=inv->get_data_Isotopes(0,i);


            data1[i+1*inv->get_Length()]=inv->get_data_Isotopes(3,i);
            cor_plot->setMultiplicator(1,-1,0);
            cor_plot->setTitel("","Depth [m]","\u03b418O [\u2030 VPDB]",0);
            y_axis=3;

    }
    cor_plot->setData(data1,2,inv->get_Length(),0);
    cor_plot->setTextSize(resources->get_Pointsize(),1,0,0);
    cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    cor_plot->setSymbol(3,0);
    cor_plot->fixRange(0,1,0);

    if (autosize) cor_plot->autoSize(0);
    // Target Plot
    int length=inv->get_Target_Length(ui->comboBox->currentIndex());
    if (inv->get_Target_Length(ui->comboBox->currentIndex())<inv->get_Length()) length=inv->get_Length();

    float *data2=new float[length*4];
    float *data2_error=new float[length*4];
    for (int i=0;i<length*4;i++) data2[i]=NAN;
    for (int i=0;i<length*4;i++) data2_error[i]=NAN;
    // Calc Offsets
    double offset_1=0;
    double offset_2=0;
    for (int i=0;i<inv->get_Length();i++) offset_1+=inv->get_data_Isotopes(y_axis,i);
    offset_1/=inv->get_Length();
    int count=0;
    for (int i=0;i<inv->get_Target_Length(ui->comboBox->currentIndex());i++) {
        if (!isnan(inv->get_Target_Value(i,ui->comboBox->currentIndex()))){
            count++;
            offset_2+=inv->get_Target_Value(i,ui->comboBox->currentIndex());
        }
    }
    offset_2/=count;

    for (int i=0;i<inv->get_Length();i++){
        // Result Data
        data2[i+0*length]=amdata->get_Int_Value_Age(inv->get_data_Isotopes(0,i),"linear");


            data2[i+1*length]=inv->get_data_Isotopes(3,i)-offset_1+offset_2;


    }

    for (int i=0;i<inv->get_Target_Length(ui->comboBox->currentIndex());i++){
        // Target Data
        data2[i+2*length]=inv->get_Target_Age(i,ui->comboBox->currentIndex());
        data2[i+3*length]=inv->get_Target_Value(i,ui->comboBox->currentIndex());
        data2_error[i+3*length]=inv->get_Target_Error(i,ui->comboBox->currentIndex());

        cor_plot->setMultiplicator(1,-1,1);
        cor_plot->setTitel("","Age in [kyr]","Target",1);

    }
    cor_plot->setData(data2,4,length,1);
    cor_plot->setError(data2_error,2,1);

    cor_plot->setTextSize(resources->get_Pointsize(),1,0,1);
    cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    cor_plot->setSymbol(3,1);
    cor_plot->fixRange(0,1,1);
    QColor *c=new QColor[2*2];
    c[0]=Qt::red;
    c[1]=Qt::red;
    c[2]=Qt::black;
    c[3]=Qt::black;
    cor_plot->setSetLineColor(c,1,1);
    int *lw=new int[4];
    lw[0]=3;
    lw[1]=3;
    lw[2]=1;
    lw[3]=1;
    cor_plot->setSetLineWidth(lw,1,1);
    int *ss=new int[4];
    ss[0]=0;
    ss[1]=0;
    ss[2]=3;
    ss[3]=3;
    cor_plot->setSetSymbolsize(ss,1,1);
    Qt::PenStyle *ps=new Qt::PenStyle[4];
    ps[0]=Qt::SolidLine;
    ps[1]=Qt::SolidLine;
    ps[2]=Qt::DashDotLine;
    ps[3]=Qt::DashDotLine;
    cor_plot->setSetLineStyle(ps,1,1);
    if (autosize) cor_plot->autoSize(1);
    cor_plot->setAxisType(1,0,0);
    createLines();
    cor_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_Isotope.txt",0);
    cor_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_Target.txt",1);
    ui->graphicsView->setScene(cor_plot);
}

void DTW::createLines(){
    int y_axis=0;


        y_axis=3;

    cor_plot->addLineClear();
    for(int i=0;i<amdata->get_Length();i++){
        // get index in data 1
        int i_1=-1;
        for (int j=0;j<inv->get_Length();j++){
             if (inv->get_data_Isotopes(0,j)>=amdata->get_Depth(i)){
                 i_1=j;
                 break;
             }
             i_1=j;
        }
        int i_2=-1;
        for (int j=0;j<inv->get_Target_Length(ui->comboBox->currentIndex());j++){
             if (inv->get_Target_Age(j,ui->comboBox->currentIndex())>=amdata->get_Data(4,i)){
                 i_2=j;
                 break;
             }
             i_2=j;
        }
        QColor c=Qt::darkGreen;
        if (amdata->get_Data(7,i)==0) c=Qt::red;
        int w=2;
        if (i==select) w=3;
        cor_plot->addLine(amdata->get_Depth(i),inv->get_data_Isotopes(y_axis,i_1),0,amdata->get_Data(4,i),inv->get_Target_Value(i_2,ui->comboBox->currentIndex()),1,c,w,Qt::SolidLine);
    }
    if (cor_plot->getSelected_Y(0)!=-1 && cor_plot->getSelected_Y(1)!=-1){
        cor_plot->addLine(inv->get_data_Isotopes(0,cor_plot->getSelected_Y(0)),inv->get_data_Isotopes(y_axis,cor_plot->getSelected_Y(0)),0,inv->get_Target_Age(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()),inv->get_Target_Value(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()),1,Qt::black,1,Qt::DashLine);
    }
    cor_plot->setAddMode(1);
    cor_plot->update();
}

void DTW::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    if (changes) {
        resBtn = QMessageBox::question( this,"PaleoDataView - DTW",
                                        tr("There are unsaved changes.\nAre you sure?\n"),
                                        QMessageBox::Cancel | QMessageBox::Yes,
                                        QMessageBox::Yes);
    }
    if (resBtn == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void DTW::tableSelected(QModelIndex mi){
    /*int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel);
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QString text = model->item(sel,0)->text();
    selected=text.toInt(0,10);*/

    int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QString text = model->item(sel,mi.column())->text();
    //qDebug() << text;
    int index=model->item(sel,0)->text().toInt();

    if (mi.column()==11){
        if (amdata->get_Data(7,sel)==1){
            amdata->set_Data(7,sel,0);
        } else {
            amdata->set_Data(7,sel,1);
        }
    }
    if (mi.column()==12){
        attDialog *attd=new attDialog(this,amdata->get_Age_Comment(index),QString::number(index));
        attd->setModal(true);
        attd->show();
    }
    select=index;

    amdata->setSelect(index);
    setupTable();



    // recreate Age model plot
    createAMPlot();



    // create result of tuning plot
    createResultPlot();

    // create interactive plots for Data and Traget tuning

    createCorPlot(1);





    update();

}

void DTW::calcGDTW(){
    // Erase all old entries of earlier DTW
    for (int i=0;i<amdata->get_Length();i++){
        if (amdata->get_Type(i)=="DTW") {
            amdata->deleteEntry(i);

            i--;
        }
    }

    //Prepare Global data flields
    dtw_DG.clear();
    dtw_CG.clear();
    dtw_SG.clear();

    dtw_i1G.clear();
    dtw_i2G.clear();

    dtw_DG.reserve(inv->get_Length()*inv->get_Target_Length(ui->comboBox->currentIndex()));
    dtw_CG.reserve(inv->get_Length()*inv->get_Target_Length(ui->comboBox->currentIndex()));
    dtw_SG.reserve(inv->get_Length()*inv->get_Target_Length(ui->comboBox->currentIndex()));
    for (int i=0;i<inv->get_Length()*inv->get_Target_Length(ui->comboBox->currentIndex());i++) dtw_DG.push_back(NAN);
    for (int i=0;i<inv->get_Length()*inv->get_Target_Length(ui->comboBox->currentIndex());i++) dtw_CG.push_back(NAN);
    for (int i=0;i<inv->get_Length()*inv->get_Target_Length(ui->comboBox->currentIndex());i++) dtw_SG.push_back(NAN);

    if (!ui->checkBox_3->isChecked()){
        // prepare the data
        dtw_x.clear();
        dtw_x.reserve(inv->get_Length());
        dtw_y.clear();
        dtw_y.reserve(inv->get_Target_Length(ui->comboBox->currentIndex()));
        for (int i=0;i<inv->get_Length();i++) dtw_x.push_back(inv->get_data_Isotopes(3,i));
        for (int i=0;i<inv->get_Target_Length(ui->comboBox->currentIndex());i++) dtw_y.push_back(inv->get_Target_Value(i,ui->comboBox->currentIndex()));
        // do calculations
        bool openbegin=ui->checkBox->isChecked();
        bool openend=ui->checkBox_2->isChecked();
        calcDTW(openbegin,openend);
        // copy to global, just copy all, no index corrections necessary
        dtw_DG.clear();
        dtw_CG.clear();
        dtw_SG.clear();

        dtw_i1G.clear();
        dtw_i2G.clear();

        dtw_DG.reserve(dtw_D.size());
        dtw_CG.reserve(dtw_C.size());
        dtw_SG.reserve(dtw_S.size());
        for (int i=0;i<dtw_D.size();i++) dtw_DG.push_back(NAN);
        for (int i=0;i<dtw_C.size();i++) dtw_CG.push_back(NAN);
        for (int i=0;i<dtw_S.size();i++) dtw_SG.push_back(NAN);

        for(int i=0;i<dtw_D.size();i++) dtw_DG.at(i)=(dtw_D.at(i));
        for(int i=0;i<dtw_C.size();i++) dtw_CG.at(i)=(dtw_C.at(i));
        for(int i=0;i<dtw_S.size();i++) dtw_SG.at(i)=(dtw_S.at(i));
        for(int i=0;i<dtw_i1.size();i++) dtw_i1G.push_back(dtw_i1.at(i));
        for(int i=0;i<dtw_i2.size();i++) dtw_i2G.push_back(dtw_i2.at(i));
    } else {
        // segment borders for data(i1) and target(i2)
        int i1_start=0;
        int i1_end=0;
        int i2_start=0;
        int i2_end=0;
        // Assuming asscending order

        for(int i=0;i<amdata->get_Length();i++){

            if (amdata->get_Data(7,i)==1){// if entry set aktiv
                // find new index for i1_end
                i1_end=0;
                for (int j=i1_start;j<inv->get_Length();j++){
                    if (inv->get_data_Isotopes(0,j)>amdata->get_Depth(i) && j>i1_start){
                        i1_end=j;
                        break;
                    }
                }
                // find new index for i2_end
                i2_end=0;
                for (int j=i2_start;j<inv->get_Target_Length(ui->comboBox->currentIndex());j++){
                    if (inv->get_Target_Age(j,ui->comboBox->currentIndex())>amdata->get_Data(4,i) && j>i2_start){
                        i2_end=j;
                        break;
                    }
                }

                if (i1_end-i1_start>0 && i2_end-i2_start>0) {

                    // prepare fragment dtw_x and dtw_y
                    dtw_x.clear();
                    dtw_x.reserve(i1_end-i1_start);
                    for (int j=i1_start;j<=i1_end;j++) {
                        dtw_x.push_back(inv->get_data_Isotopes(3,j));
                    }
                    dtw_y.clear();
                    dtw_y.reserve(i2_end-i2_start);
                    for (int j=i2_start;j<=i2_end;j++) {
                        dtw_y.push_back(inv->get_Target_Value(j,ui->comboBox->currentIndex()));
                    }
                    // set openend and/or openbegin
                    bool openend=0;
                    bool openbegin=0;
                    if (i1_start==0) openbegin=1;
                    if (i1_end==inv->get_Length()) openend=1;

                    // do the calculations
                    calcDTW(openbegin,openend);

                    // put segment result to global result
                    // Distance Matrix
                    for(int j=0;j<=i1_end-i1_start;j++) {
                        for(int q=0;q<=i2_end-i2_start;q++){
                            if (openbegin){
                                dtw_DG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_D.at((j+1)*(i2_end-i2_start+1)+q);
                            }else{
                                dtw_DG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_D.at(j*(i2_end-i2_start+1)+q);
                            }
                        }
                    }
                    // Cost Matrix
                    for(int j=0;j<=i1_end-i1_start;j++) {
                        for(int q=0;q<=i2_end-i2_start;q++){
                            if (openbegin){
                                dtw_CG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_C.at((j+1)*(i2_end-i2_start+1)+q);
                            }else{
                                dtw_CG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_C.at(j*(i2_end-i2_start+1)+q);
                            }
                        }
                    }
                    // Path
                    for(int i=0;i<dtw_i1.size();i++) dtw_i1G.push_back(dtw_i1.at(i)+i1_start);
                    for(int i=0;i<dtw_i2.size();i++) dtw_i2G.push_back(dtw_i2.at(i)+i2_start);
                    // prepare for next segment
                    i1_start=i1_end;
                    i2_start=i2_end;
                }

            }

                }
        // add last segment
        if (i1_start<inv->get_Length()){
            // find new index for i1_end
            i1_end=inv->get_Length()-1;

            // find new index for i2_end
            i2_end=inv->get_Target_Length(ui->comboBox->currentIndex())-1;

            if (i1_end-i1_start>0 && i2_end-i2_start>0) {

                // prepare fragment dtw_x and dtw_y
                dtw_x.clear();
                dtw_x.reserve(i1_end-i1_start);
                for (int j=i1_start;j<=i1_end;j++) {
                    dtw_x.push_back(inv->get_data_Isotopes(3,j));
                }
                dtw_y.clear();
                dtw_y.reserve(i2_end-i2_start);
                for (int j=i2_start;j<=i2_end;j++) {
                    dtw_y.push_back(inv->get_Target_Value(j,ui->comboBox->currentIndex()));
                }
                // set openend and/or openbegin
                bool openend=1;
                bool openbegin=0;


                // do the calculations
                calcDTW(openbegin,openend);

                // put segment result to global result
                // Distance Matrix
                for(int j=0;j<=i1_end-i1_start;j++) {
                    for(int q=0;q<=i2_end-i2_start;q++){
                        if (openbegin){
                            dtw_DG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_D.at((j+1)*(i2_end-i2_start+1)+q);
                        }else{
                            dtw_DG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_D.at(j*(i2_end-i2_start+1)+q);
                        }
                    }
                }
                // Cost Matrix
                for(int j=0;j<=i1_end-i1_start;j++) {
                    for(int q=0;q<=i2_end-i2_start;q++){
                        if (openbegin){
                            dtw_CG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_C.at((j+1)*(i2_end-i2_start+1)+q);
                        }else{
                            dtw_CG.at((i1_start+j)*inv->get_Target_Length(ui->comboBox->currentIndex())+(i2_start+q))=dtw_C.at(j*(i2_end-i2_start+1)+q);
                        }
                    }
                }
                // Path
                for(int i=0;i<dtw_i1.size();i++) dtw_i1G.push_back(dtw_i1.at(i)+i1_start);
                for(int i=0;i<dtw_i2.size();i++) dtw_i2G.push_back(dtw_i2.at(i)+i2_start);
            }
        }



    }




    int n=inv->get_Length();
    int m=inv->get_Target_Length(ui->comboBox->currentIndex());


    // show result
    //setupDistTable(n,m);
    //setupCostTable(n,m);
    createDistPlot(n,m);
    createCostPlot(n,m);



    // add entries to Agemodel
    for (int i=dtw_i1G.size()-1;i>=0;i--){
        addPoint(dtw_i1G.at(i),dtw_i2G.at(i));
    }

    // align and update everything
    amdata->sort(0,amdata->get_Length()-1);

    createLines();


    createAMPlot();

    createResultPlot();
    createCorPlot(0);
    setupTable();
    update();
}

void DTW::calcDTW(bool openbegin,bool openend){
    // dtw_x/dtw_y must contain the data
    int n=dtw_x.size();
    int m=dtw_y.size();
    dtw_D.clear();
    dtw_D.reserve(n*m);


    // post processing for open beginning

    if (openbegin) {
        //  ensure proper normalization
        //if(is.na(norm) || norm != "N") {
        //  stop("Open-begin requires step patterns with 'N' normalization (e.g. asymmetric, or R-J types (c)). See papers in citation().");
        //}

        // prepend a null row to distance matrix
        for (int i=0;i<m;i++){
            dtw_D.push_back(0);
        }
        n++;
        // Calc dist Matrix
        for (int i=0;i<n-1;i++){
            for (int j=0;j<m;j++){
                dtw_D.push_back(proxy(i,j));
            }
        }
    } else {
        // Calc dist Matrix
        for (int i=0;i<n;i++){
            for (int j=0;j<m;j++){
                dtw_D.push_back(proxy(i,j));
            }
        }
    }

/*
    // create example
    dtw_D.clear();
    dtw_D.reserve(36);
    dtw_D.push_back(1);
    dtw_D.push_back(1);
    dtw_D.push_back(2);
    dtw_D.push_back(2);
    dtw_D.push_back(3);
    dtw_D.push_back(3);

    dtw_D.push_back(1);
    dtw_D.push_back(1);
    dtw_D.push_back(1);
    dtw_D.push_back(2);
    dtw_D.push_back(2);
    dtw_D.push_back(2);

    dtw_D.push_back(3);
    dtw_D.push_back(1);
    dtw_D.push_back(2);
    dtw_D.push_back(2);
    dtw_D.push_back(3);
    dtw_D.push_back(3);

    dtw_D.push_back(3);
    dtw_D.push_back(1);
    dtw_D.push_back(2);
    dtw_D.push_back(1);
    dtw_D.push_back(1);
    dtw_D.push_back(2);

    dtw_D.push_back(3);
    dtw_D.push_back(2);
    dtw_D.push_back(1);
    dtw_D.push_back(2);
    dtw_D.push_back(1);
    dtw_D.push_back(2);

    dtw_D.push_back(3);
    dtw_D.push_back(3);
    dtw_D.push_back(3);
    dtw_D.push_back(2);
    dtw_D.push_back(1);
    dtw_D.push_back(2);
    n=6;
    m=6;
    //end of example
*/


    /* From here calculate cumulative cost matrix*/
        // Init cost and step Matrix
        dtw_C.clear();
        dtw_S.clear();
        dtw_C.reserve(n*m);
        dtw_S.reserve(n*m);
        for (int i=0;i<n;i++){
            for (int j=0;j<m;j++){
                if (openbegin && i==0){
                    dtw_C.push_back(0);
                }else{
                    dtw_C.push_back(NAN);
                }
                dtw_S.push_back(NAN);
            }
        }
        if (!openbegin) dtw_C.at(0)=dtw_D.at(0);



    // Init StepPattern
    stepPattern(ui->comboBox_3->currentText());



    int nsteps=dtw_step_ix.size();
    int npats=dtw_step_npat.at(dtw_step_ix.size()-1);

    for (int j=0;j<m;j++){
        for (int i=0;i<n;i++){
            if (!isnan(dtw_C.at(i*m+j))){
                // skip
            } else {
                // list of steps
                std::vector<float> clist;
                clist.clear();
                clist.reserve(npats);
                for (int q=0;q<npats;q++) clist.push_back(NAN);// init is nan

                for (int s=0;s<nsteps;s++){
                    int p=dtw_step_npat.at(s)-1;

                    int ii=i-dtw_step_ix.at(s);
                    int jj=j-dtw_step_iy.at(s);
                    if (ii>=0&&jj>=0){
                        int cc=dtw_step_pen.at(s);
                        if (cc==-1){
                            clist.at(p)=dtw_C.at(ii*m+jj);
                        } else {
                            clist.at(p)=clist.at(p)+cc*dtw_D.at(ii*m+jj);
                        }
                    }
                }
                // find minimum
                float min=INFINITY;
                int min_index=-1;
                for (int q=0;q<npats;q++){
                    if (min>clist.at(q)&&!isnan(clist.at(q))){
                        min=clist.at(q);
                        min_index=q;

                    }


                }
                if (min_index>-1){
                    dtw_C.at(i*m+j)=min;
                    dtw_S.at(i*m+j)=min_index+1;
                }

            }
        }
    }

    // postprocessing for open end
    std::vector<float> lastrow;
    lastrow.clear();
    if (dtw_step_norm=="N"){
        for (int i=0;i<m;i++) lastrow.push_back(dtw_C.at((n-1)*m+i)/n);
    }
    if (dtw_step_norm=="N+M"){
        for (int i=0;i<m;i++) lastrow.push_back(dtw_C.at((n-1)*m+i)/(n+(i+1)));
    }
    // last element for global alignment
    int last=m-1;
    // for open end
    if (openend) {
        float min=INFINITY;
        int min_index=-1;
        for (int q=0;q<m;q++){
            if (min>lastrow.at(q)){
                min=lastrow.at(q);
                min_index=q;
            }
        }
        ui->label_8->setText("Last Element in Reference : "+QString::number(min_index));
        last=min_index;

    } else {
        ui->label_8->setText("Last Element in Reference : "+QString::number(m));
    }
    if (last>0) ui->label_6->setText("Distance : "+QString::number(dtw_C.at((n-1)*m+last)));
    if (last>0) ui->label_7->setText("Normalized Distance : "+QString::number(lastrow.at(last)));







    if (last>0){
        // generate list of index dependencies by backtracking
        dtw_i1.clear();
        dtw_i2.clear();
        int i1=n-1;
        int i2=last-1;
        dtw_i1.push_back(i1);
        dtw_i2.push_back(i2);
        while (i1!=0 || i2!=0){
            int s=dtw_S.at(i1*m+i2)-1;
            if (s<0) break;
            int di1=dtw_step_ix.at(s*2);
            int di2=dtw_step_iy.at(s*2);

            dtw_i1.push_back(i1-di1);
            dtw_i2.push_back(i2-di2);

            i1=i1-di1;
            i2=i2-di2;

        }

        dtw_i1.push_back(i1);
        dtw_i2.push_back(i2);
    }
}

void DTW::addPoint(int i1,int i2){
    // get selected points
    double depth=inv->get_data_Isotopes(0,i1);
    double age=inv->get_Target_Age(i2,ui->comboBox->currentIndex());


    // check if valid
    int ok=1;
    /*
    int pos =0;
    for (int i=0;i<amdata->get_Length();i++){
        if (amdata->get_Data(4,i)<age){
            pos++;
        } else {
            break;
        }
    }


    if (amdata->get_Length()==0) ok=1; // first element always ok

    if (pos>0){//if not first element
        // check previous element
        if ((amdata->get_Data(4,pos-1)<age && amdata->get_Depth(pos-1)<depth)){
            // check next element
            if (pos<amdata->get_Length()){// if not last element
                if (amdata->get_Data(4,pos)>age && amdata->get_Depth(pos)>depth) ok=1;
            } else { // if last element
                ok=1;
            }
        }
    } else {// if first element
        // check next element
        if (pos<amdata->get_Length()){// if not last element
            if (amdata->get_Data(4,pos)>age && amdata->get_Depth(pos)>depth) ok=1;
        } else { // if last element
            ok=1;
        }
    }*/
    // check if it will be a reversal
    for (int i=0;i<amdata->get_Length();i++){
        float depth_2=amdata->get_Depth(i);
        float age_2=amdata->get_Data(4,i);
        if (age<age_2 && depth>depth_2) ok=0;
        if (age>age_2 && depth<depth_2) ok=0;
        if (age==age_2 || depth==depth_2) ok=0;

    }

    if (ok){

        amdata->addEntry();

        if (i1>=0) {
            amdata->set_Depth(0,inv->get_data_Isotopes(0,i1));
        } else {
            amdata->set_Depth(0,0);
        }
        amdata->set_Type(0,"DTW");
        amdata->set_LabID(0,"User");
        amdata->set_Age_Comment("DTW tuned with :"+ui->comboBox->currentText(),0);
        amdata->set_Data(7,0,1);
        if (i2>=0) {
            amdata->set_Data(0,0,NAN);
            amdata->set_Data(1,0,NAN);
            amdata->set_Data(2,0,NAN);
            amdata->set_Data(3,0,NAN);
            amdata->set_Data(4,0,inv->get_Target_Age(i2,ui->comboBox->currentIndex()));
            amdata->set_Data(5,0,inv->get_Target_Age(i2,ui->comboBox->currentIndex()));
            amdata->set_Data(6,0,inv->get_Target_Age(i2,ui->comboBox->currentIndex()));
        } else {
            amdata->set_Data(4,0,0);
        }


    }
    changes=true;
}

float DTW::proxy(int i,int j){
    float v=0;
    if (ui->comboBox_2->currentText()=="Euclidian") v=abs(dtw_x.at(i)-dtw_y.at(j));
    return v;
}

void DTW::stepPattern(QString s){
    dtw_step_ix.clear();
    dtw_step_iy.clear();
    dtw_step_pen.clear();
    dtw_step_npat.clear();
    dtw_step_norm.clear();
    if (s=="Asymmetric"){
        dtw_step_ix.reserve(3*2);
        dtw_step_iy.reserve(3*2);
        dtw_step_pen.reserve(3*2);
        dtw_step_npat.reserve(3*2);
        /************1**********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(1);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(1);
        /************2**********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(1);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(2);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(2);
        /*************3*********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(2);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(3);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(3);
        dtw_step_norm.append("N+M");
    }
    if (s=="Asymmetric*2"){
        dtw_step_ix.reserve(5*2);
        dtw_step_iy.reserve(5*2);
        dtw_step_pen.reserve(5*2);
        dtw_step_npat.reserve(5*2);
        /************1**********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(1);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(1);
        /************2**********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(1);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(2);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(2);
        /*************3*********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(2);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(3);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(3);
        /*************4*********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(3);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(4);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(4);
        /*************5*********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(4);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(5);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(5);
        dtw_step_norm.append("N+M");
    }
    if (s=="Symmetric2"){
        dtw_step_ix.reserve(3*2);
        dtw_step_iy.reserve(3*2);
        dtw_step_pen.reserve(3*2);
        dtw_step_npat.reserve(3*2);
        /************1**********/
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(1);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(1);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(1);
        /************2**********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(1);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(2);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(2);
        /*************3*********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(3);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(3);
        dtw_step_norm.append("N");
    }
    if (s=="RabinerJuan Typ IV"){
        dtw_step_ix.reserve(4*2);
        dtw_step_iy.reserve(4*2);
        dtw_step_pen.reserve(4*2);
        dtw_step_npat.reserve(4*2);
        /************1**********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(2);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(1);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(1);
        /************2**********/
        dtw_step_ix.push_back(2);
        dtw_step_iy.push_back(2);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(2);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(2);
        /*************3*********/
        dtw_step_ix.push_back(1);
        dtw_step_iy.push_back(1);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(3);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(3);
        /*************4*********/
        dtw_step_ix.push_back(2);
        dtw_step_iy.push_back(1);
        dtw_step_pen.push_back(-1);
        dtw_step_npat.push_back(4);
        dtw_step_ix.push_back(0);
        dtw_step_iy.push_back(0);
        dtw_step_pen.push_back(1);
        dtw_step_npat.push_back(4);
        dtw_step_norm.append("N");
    }
}



void DTW::createDistPlot(int n,int m){

    float min=9999999999999;
    float max=-999999999999;

    for (int j=0;j<n*m;j++){
        if (std::isnan(dtw_DG.at(j))==false){
            if (dtw_DG.at(j)<min) min=dtw_DG.at(j);
            if (dtw_DG.at(j)>max) max=dtw_DG.at(j);
        }
    }

    float *data1=new float[n*m*2];
    QColor *col=new QColor[n*m*2];
    QString *comment=new QString[n*m*2];

    for (int i=0;i<n;i++){
        for (int j=0;j<m;j++){
            data1[i*m+j]=i;
            data1[i*m+j+n*m]=j;
            if (std::isnan(dtw_DG.at(i*m+j))==false){
                int c=200-(int)(((dtw_DG.at(i*m+j)-min)/(max-min))*200.0);
                col[i*m+j]=QColor(c,c,c);
                col[i*m+j+n*m]=QColor(c,c,c);
                comment[i*m+j]="Distance : "+QString::number(dtw_DG.at(i*m+j));
                //comment[i*m+j+n*m]=QString::number(dtw_D.at(i*m+j));
            } else {
                data1[i*m+j]=NAN;
                data1[i*m+j+n*m]=NAN;
            }
        }
    }
    dist_plot->setTitel("Distance Matrix","Isotope Index","Target Index");
    dist_plot->setData(data1,2,m*n);
    dist_plot->setTextSize(resources->get_Pointsize(),1,0);
    dist_plot->setComment(comment,1);
    dist_plot->setComHide(0,1);
    dist_plot->setSymbol(50);
    dist_plot->setLineStyle(Qt::NoPen);
    dist_plot->autoSize();
    dist_plot->setColor(col,1);

    float *area_x=new float[2];
    for (int i=0;i<2;i++) area_x[i]=0.5;

    float *area_y=new float[n*m];
    for (int i=0;i<n*m;i++) area_y[i]=0.5;

    dist_plot->setSetArea(area_x,area_y,1);
    dist_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_Dist.txt",0);
    ui->graphicsView_5->setScene(dist_plot);
    dist_plot->setSize(ui->graphicsView_5->width(),ui->graphicsView_5->height());
}

void DTW::createCostPlot(int n,int m){

    float min=9999999999999;
    float max=-999999999999;

    for (int j=0;j<n*m;j++){
        if (std::isnan(dtw_CG.at(j))==false){
            if (dtw_CG.at(j)<min) min=dtw_CG.at(j);
            if (dtw_CG.at(j)>max) max=dtw_CG.at(j);
        }
    }

    float *data1=new float[n*m*2];
    QColor *col=new QColor[n*m*2];
    QString *comment=new QString[n*m*2];

    for (int i=0;i<n;i++){
        for (int j=0;j<m;j++){
            data1[i*m+j]=i;
            data1[i*m+j+n*m]=j;
            if (std::isnan(dtw_CG.at(i*m+j))==false){
                int c=200-(int)(((dtw_CG.at(i*m+j)-min)/(max-min))*200.0);
                col[i*m+j]=QColor(c,c,c);
                col[i*m+j+n*m]=QColor(c,c,c);
                comment[i*m+j]="Cumulative Cost : "+QString::number(dtw_CG.at(i*m+j));
                //comment[i*m+j+n*m]=QString::number(dtw_C.at(i*m+j));
            } else {
                data1[i*m+j]=NAN;
                data1[i*m+j+n*m]=NAN;
            }
        }
    }
    // paint path blue
    for (int i=0;i<dtw_i1G.size();i++){
        if(dtw_i1G.at(i)<n&&dtw_i2G.at(i)<m){
            col[dtw_i1G.at(i)*m+dtw_i2G.at(i)]=Qt::blue;
            col[dtw_i1G.at(i)*m+dtw_i2G.at(i)+n*m]=Qt::blue;
        }
    }
    cost_plot->setTitel("Cumulated Cost Matrix","Isotope Index","Target Index");
    cost_plot->setData(data1,2,m*n);
    cost_plot->setTextSize(resources->get_Pointsize(),1,0);
    cost_plot->setComment(comment,1);
    cost_plot->setComHide(0,1);
    cost_plot->setSymbol(50);
    cost_plot->setLineStyle(Qt::NoPen);
    cost_plot->autoSize();
    cost_plot->setColor(col,1);

    float *area_x=new float[2];
    for (int i=0;i<2;i++) area_x[i]=0.5;

    float *area_y=new float[n*m];
    for (int i=0;i<n*m;i++) area_y[i]=0.5;

    cost_plot->setSetArea(area_x,area_y,1);
    cost_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/DTW_Cost.txt",0);
    ui->graphicsView_6->setScene(cost_plot);
    cost_plot->setSize(ui->graphicsView_6->width(),ui->graphicsView_6->height());
}

