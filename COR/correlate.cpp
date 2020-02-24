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

#include "correlate.h"
#include "ui_correlate.h"

Correlate::Correlate(QMainWindow *mainWindow,Inventory *inventory,QImage* wmap,QString str) :
    mainW(mainWindow),inv(inventory),worldmap(wmap),proxy(str),
    ui(new Ui::Correlate)
{

    inv->readData(inv->get_currentCore());
    resources=new Resources();
    ui->setupUi(this);
    this->setWindowTitle("Visual Alignment for Core : "+inv->get_Core(inv->get_currentCore())+" : "+inv->get_Species(inv->get_currentCore())+" at "+QString::number(inv->get_Longitude(inv->get_currentCore()))+"° :"+QString::number(inv->get_Latitude(inv->get_currentCore()))+"° in "+QString::number(inv->get_Water_Depth(inv->get_currentCore()))+"m");

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);

    // Estimate data colummn
    ndata=0;
    for (int i=0;i<inv->access_data_length();i++) if (inv->access_data_type(i).contains("Correlate")) ndata=i;

    // prepare drop box for dataselection
    ui->comboBox_2->clear();
    for (int i=0;i<inv->access_data_length();i++){
        QString str="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(i)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int j=0;j<list.size();j++) if(list[j].contains("%"+inv->access_data_name(i)+"=")) if (list[j].split("=").size()>0) str=list[j].split("=").at(1);
        } else {
            str=inv->access_data_name(i);
        }

        ui->comboBox_2->addItem(str);
        if (inv->access_data_name(i)=="Sample Thickness" || inv->access_data_name(i)=="Age"|| inv->access_data_name(i)=="Use Flag"||inv->access_data_name(i).contains("Error")||inv->access_data_name(i).contains("Comment")||inv->access_data_name(i)=="Depth"){
            // Deactivate/Hide the entry
            QListView* view = qobject_cast<QListView *>(ui->comboBox_2->view());
            Q_ASSERT(view != nullptr);
            view->setRowHidden(i, true);
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->comboBox_2->model());
            Q_ASSERT(model != nullptr);
            QStandardItem* item = model->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            if (ui->comboBox_2->currentIndex()==i) ui->comboBox_2->setCurrentIndex(-1);
        } else {
            if (ui->comboBox_2->currentIndex()==-1) ui->comboBox_2->setCurrentIndex(i);
        }

    }

    if (ndata>0) ui->comboBox_2->setCurrentIndex(ndata);

    // prepare drop box for dataselection
    ui->comboBox_3->clear();
    for (int i=0;i<inv->access_data_length();i++){
        QString str="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(i)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int j=0;j<list.size();j++) if(list[j].contains("%"+inv->access_data_name(i)+"=")) if (list[j].split("=").size()>0) str=list[j].split("=").at(1);
        } else {
            str=inv->access_data_name(i);
        }

        ui->comboBox_3->addItem(str);
        if (inv->access_data_name(i)=="Sample Thickness" || inv->access_data_name(i)=="Age"|| inv->access_data_name(i)=="Use Flag"||inv->access_data_name(i).contains("Error")||inv->access_data_name(i).contains("Comment")||inv->access_data_name(i)=="Depth"){
            // Deactivate/Hide the entry
            QListView* view = qobject_cast<QListView *>(ui->comboBox_3->view());
            Q_ASSERT(view != nullptr);
            view->setRowHidden(i, true);
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->comboBox_3->model());
            Q_ASSERT(model != nullptr);
            QStandardItem* item = model->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            if (ui->comboBox_3->currentIndex()==i) ui->comboBox_3->setCurrentIndex(-1);
        } else {
            if (ui->comboBox_3->currentIndex()==-1) ui->comboBox_3->setCurrentIndex(i);
        }

    }

    if (ndata>0) ui->comboBox_3->setCurrentIndex(ndata);

    qDebug()<<"Read AMS";
    amdata=new AMSData(inv);
    amdata->AMSRead();
    qDebug()<<"finished";
    //inv->new_Age_Model();
    //inv->read_Age_Model();
    qDebug()<<"read Target Data";
    inv->readTarget();
    qDebug()<<"finshed";
    qDebug()<<"create Dorpbox with Stacks";
    ui->comboBox->clear();
    for(int i=0;i<inv->get_Target_Vari();i++) ui->comboBox->addItem(inv->get_Target_Name(i));
    ui->comboBox->addItem("Custom Target");
    // check if already a stack is present in the agemodel
    int changed=0;
    for (int i=0;i<amdata->get_Length();i++){
        if (amdata->get_Type(i)=="INA") {
            ui->comboBox->setCurrentText("Intermediate North Atlantic");
            changed=1;
        }

        if (amdata->get_Type(i)=="DNA") ui->comboBox->setCurrentText("Deep North Atlantic");
        if (amdata->get_Type(i)=="ISA") ui->comboBox->setCurrentText("Intermediate South Atlantic");
        if (amdata->get_Type(i)=="DSA") ui->comboBox->setCurrentText("Deep South Atlantic");
        if (amdata->get_Type(i)=="IP") ui->comboBox->setCurrentText("Intermediate Pacific");
        if (amdata->get_Type(i)=="DP") ui->comboBox->setCurrentText("Deep Pacific");
        if (amdata->get_Type(i)=="II") ui->comboBox->setCurrentText("Intermediate Indian");
        if (amdata->get_Type(i)=="DI") ui->comboBox->setCurrentText("Deep Indian");
        if (amdata->get_Type(i)=="EDML") ui->comboBox->setCurrentText("EDML");
        if (amdata->get_Type(i)=="NGRIP") ui->comboBox->setCurrentText("NGRIP");
        if (amdata->get_Type(i)=="LR04") ui->comboBox->setCurrentText("LR04");
    }
    if (amdata->get_Length()==0 || (ui->comboBox->currentIndex()==0 && changed==0)) {
        int basin=inv->get_Basin(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()));
        if (basin==1){
               if (inv->get_Water_Depth(inv->get_currentCore())<2000 && inv->get_Latitude(inv->get_currentCore())>0) ui->comboBox->setCurrentText("Intermediate North Atlantic");
               if (inv->get_Water_Depth(inv->get_currentCore())<2000 && inv->get_Latitude(inv->get_currentCore())<0) ui->comboBox->setCurrentText("Intermediate South Atlantic");
               if (inv->get_Water_Depth(inv->get_currentCore())>=2000 && inv->get_Latitude(inv->get_currentCore())>0) ui->comboBox->setCurrentText("Deep North Atlantic");
               if (inv->get_Water_Depth(inv->get_currentCore())>=2000 && inv->get_Latitude(inv->get_currentCore())<0) ui->comboBox->setCurrentText("Deep South Atlantic");
        }
        if (basin==2){
               if (inv->get_Water_Depth(inv->get_currentCore())<2000) ui->comboBox->setCurrentText("Intermediate Pacific");
               if (inv->get_Water_Depth(inv->get_currentCore())>=2000) ui->comboBox->setCurrentText("Deep Pacific");
        }
        if (basin==3){
               if (inv->get_Water_Depth(inv->get_currentCore())<2000) ui->comboBox->setCurrentText("Intermediate Indian");
               if (inv->get_Water_Depth(inv->get_currentCore())>=2000) ui->comboBox->setCurrentText("Deep Indian");
        }
    }
    select=amdata->getSelect();

qDebug()<<"create plots";
    // create Age model plot
    data1=new float[0];
    data_use1=new bool[0];
    data_error1=new float[0];
    data_error21=new float[0];
    col1=new QColor[0];
    mark1=new int[0];
    am_plot=new Graph(this,data1,0,0);
    createAMPlot();

    // create result of tuning plot
    data2=new float[0];
    use2=new bool[0];
    comment2=new QString[0];
    result_plot=new Graph(this,data2,0,0);
    createResultPlot();

    // create sedimentation rate plot
    data3=new float[0];
    col3=new QColor[0];
    sr_plot=new Graph(this,data3,0,0);
    createSRPlot();

    // create correlation plot
    data4=new float[0];
    use4=new bool[0];

    data5=new float[0];
    data_error5=new float[0];
    data_error25=new float[0];
    use5=new bool[0];
    c5=new QColor[0];
    lw5=new int[0];
    ss5=new int[0];
    ps5=new Qt::PenStyle[0];

    cor_plot=new Graph(this,data4,0,0);

    cor_plot->setNumber(2);
    createCorPlot(1);


    qDebug()<<"GUI Actions";

    //modelData=new QStandardItemModel(0,0,this);
    setupTable(0);

    edit=new AMEdit(this,amdata,select);
    attd=new attDialog(this,amdata->get_Age_Comment(0),QString::number(0));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(addPoint()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(newPoint()));

    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(removePoint()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(editPoint()));
    connect(ui->pushButton_7,SIGNAL(clicked(bool)),this,SLOT(selectTarget()));
    ui->pushButton_7->setEnabled(false);
    ui->comboBox_3->setEnabled(false);
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(tableSelected(QModelIndex)));
    //connect(ui->comboBox_2,SIGNAL(currentTextChanged(QString)),this,SLOT(int_Style_Selected(QString)));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(apply()));
    //connect(ui->comboBox_3,SIGNAL(currentTextChanged(QString)),this,SLOT(data_Selected(QString)));
    connect(ui->comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(target_Selected(QString)));
    connect(ui->comboBox_2,SIGNAL(currentTextChanged(QString)),this,SLOT(data_Selected(QString)));
    connect(ui->comboBox_3,SIGNAL(currentTextChanged(QString)),this,SLOT(data_Selected2(QString)));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),this,SLOT(applyAM()));
    //connect(cor_plot,SIGNAL(selectSignal()),this,SLOT(createLines()));
    connect(cor_plot,SIGNAL(click(int,float,float)),this,SLOT(clickRepaint(int,float,float)));
    connect(am_plot,SIGNAL(click(int,float,float)),this,SLOT(clickRepaint(int,float,float)));
    connect(sr_plot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(cor_plot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(am_plot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(result_plot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    connect(ui->checkBox_2,SIGNAL(stateChanged(int)),this,SLOT(updateGraph()));

    connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(updateGraph()));

    changes=false;
}

Correlate::~Correlate()
{
    delete ui;
    delete resources;
    delete amdata;
    //delete modelData;
    delete edit;
    delete attd;

    delete[] data1;
    delete[] data_use1;
    delete[] data_error1;
    delete[] data_error21;
    delete[] col1;
    delete[] mark1;

    delete[] data2;
    delete[] use2;
    delete[] comment2;

    delete[] data3;
    delete[] col3;

    delete[] data4;
    delete[] use4;

    delete[] data5;
    delete[] data_error5;
    delete[] data_error25;
    delete[] use5;
    delete[] c5;
    delete[] lw5;
    delete[] ss5;
    delete[] ps5;
}

void Correlate::paintEvent(QPaintEvent *)
{



    createLines();
    cor_plot->setSize(ui->graphicsView->width()-30,ui->graphicsView->height());
    am_plot->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    sr_plot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    result_plot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());


}

void Correlate::setupTable(int column){


    // create the model for AMData
    //delete modelData;
    modelData = new QStandardItemModel(amdata->get_Length(),14,this);
    modelData->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelData->setHorizontalHeaderItem(1, new QStandardItem(QString("Depth")));
    modelData->setHorizontalHeaderItem(2, new QStandardItem(QString("Sample\nThickness")));
    modelData->setHorizontalHeaderItem(3, new QStandardItem(QString("Label")));
    modelData->setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    modelData->setHorizontalHeaderItem(5, new QStandardItem(QString("Age dated\n[ka]")));
    modelData->setHorizontalHeaderItem(6, new QStandardItem(QString("Age std\n[ka+]")));
    modelData->setHorizontalHeaderItem(7, new QStandardItem(QString("Age std\n[ka-]")));
    modelData->setHorizontalHeaderItem(8, new QStandardItem(QString("Res. Age\n[ka]")));
    modelData->setHorizontalHeaderItem(9, new QStandardItem(QString("Res. Age\n Error\n[ka]")));
    modelData->setHorizontalHeaderItem(10, new QStandardItem(QString("Cal yrs\n[wm ka BP]")));
    modelData->setHorizontalHeaderItem(11, new QStandardItem(QString("Cal yrs min\n[95%]")));
    modelData->setHorizontalHeaderItem(12, new QStandardItem(QString("Cal yrs max\n[95%]")));
    modelData->setHorizontalHeaderItem(13, new QStandardItem(QString("Use Flag")));
    modelData->setHorizontalHeaderItem(14, new QStandardItem(QString("Comments")));

    ui->tableView->setModel(modelData);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView->setFont(font);
    ui->tableView->horizontalHeader()->setFont(font);
    ui->tableView->verticalHeader()->setFont(font);


    for (int i=0;i<amdata->get_Length();i++){

            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            modelData->setItem(i,0,var_Index);

            QStandardItem *var_Depth=new QStandardItem(QString::number(amdata->get_Depth(i)));
            modelData->setItem(i,1,var_Depth);

            QStandardItem *var_Sample=new QStandardItem(QString::number(amdata->get_Sample_Thickness(i)));
            modelData->setItem(i,2,var_Sample);

            QStandardItem *var_Label=new QStandardItem(amdata->get_LabID(i));
            modelData->setItem(i,3,var_Label);

            QStandardItem *var_Type=new QStandardItem(amdata->get_Type(i));
            modelData->setItem(i,4,var_Type);

            QStandardItem *var_Age_dated=new QStandardItem(QString::number(amdata->get_Data(0,i)));
            modelData->setItem(i,5,var_Age_dated);

            QStandardItem *var_Age_UCL=new QStandardItem(QString::number(amdata->get_Data(1,i)));
            modelData->setItem(i,6,var_Age_UCL);

            QStandardItem *var_Age_LCL=new QStandardItem(QString::number(amdata->get_Data(2,i)));
            modelData->setItem(i,7,var_Age_LCL);

            QStandardItem *var_Age_Res=new QStandardItem(QString::number(amdata->get_Data(3,i)));
            modelData->setItem(i,8,var_Age_Res);

            QStandardItem *var_Age_Res_Err=new QStandardItem(QString::number(amdata->get_Reservoir_Error(i)));
            modelData->setItem(i,9,var_Age_Res_Err);

            QStandardItem *var_Cal=new QStandardItem(QString::number(amdata->get_Data(4,i)));
            modelData->setItem(i,10,var_Cal);

            QStandardItem *var_Cal_Min=new QStandardItem(QString::number(amdata->get_Data(5,i)));
            modelData->setItem(i,11,var_Cal_Min);

            QStandardItem *var_Cal_Max=new QStandardItem(QString::number(amdata->get_Data(6,i)));
            modelData->setItem(i,12,var_Cal_Max);

            QStandardItem *var_Use_Flag=new QStandardItem(QString::number(amdata->get_Data(7,i)));
            var_Use_Flag->setCheckable(true);
            if (amdata->get_Data(7,i)) var_Use_Flag->setCheckState(Qt::Checked);
            modelData->setItem(i,13,var_Use_Flag);
            modelData->setData(modelData->index(i, 13), Qt::AlignCenter,Qt::TextAlignmentRole);

            QStandardItem *var_Comment=new QStandardItem(amdata->get_Age_Comment(i));
            modelData->setItem(i,14,var_Comment);




            // coloring
            if (amdata->get_Data(7,i)){
                modelData->setData(modelData->index(i, 13), QColor(Qt::green), Qt::BackgroundRole);
            } else {
                modelData->setData(modelData->index(i, 13), QColor(Qt::red), Qt::BackgroundRole);
            }


    }
    //ui->tableView->setSortingEnabled(1);
    //ui->tableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    for (int i=0;i<15;i++)if (i!=13)modelData->setData(modelData->index(amdata->getSelect(),i), QColor(Qt::gray), Qt::BackgroundRole);
    ui->tableView->scrollTo(modelData->index(amdata->getSelect(),column));
    ui->tableView->repaint();
}

void Correlate::addPoint(){
    int tl=0;
    if (ui->comboBox->currentText()!="Custom Target") tl=inv->get_Target_Length(ui->comboBox->currentIndex()); else tl=target_length;
    if ((cor_plot->getSelected_Y(0)>=0 && cor_plot->getSelected_Y(0)<inv->get_Length()) && (cor_plot->getSelected_Y(1)>=0 && cor_plot->getSelected_Y(1)<tl)){
        // get selected points
        double depth=0;
        double age=0;
        if(ui->comboBox->currentText()!="Custom Target"){
            depth=inv->get_data_Depth(cor_plot->getSelected_Y(0));
            age=inv->get_Target_Age(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex());
        } else {
            depth=inv->get_data_Depth(cor_plot->getSelected_Y(0));
            age=target_age[0]->at(cor_plot->getSelected_Y(1)).toFloat();
        }

        // check if valid
        int ok=1;

        // check if it will be a reversal
        for (int i=0;i<amdata->get_Length();i++){
            if (amdata->get_Data(7,i)==1){
                float depth_2=amdata->get_Depth(i);
                float age_2=amdata->get_Data(4,i);
                if (age<age_2 && depth>depth_2) ok=0;
                if (age>age_2 && depth<depth_2) ok=0;
                if (age==age_2 || depth==depth_2) ok=0;
            }
        }

        if (ok){

            amdata->addEntry();

            if (cor_plot->getSelected_Y(0)>=0) {
                 amdata->set_Depth(0,inv->get_data_Depth(cor_plot->getSelected_Y(0)));
            } else {
                amdata->set_Depth(0,0);
            }
            amdata->set_Type(0,"COR");
            if (ui->comboBox->currentText()=="Intermediate North Atlantic") amdata->set_Type(0,"INA");
            if (ui->comboBox->currentText()=="Deep North Atlantic") amdata->set_Type(0,"DNA");
            if (ui->comboBox->currentText()=="Intermediate South Atlantic") amdata->set_Type(0,"ISA");
            if (ui->comboBox->currentText()=="Deep South Atlantic") amdata->set_Type(0,"DSA");
            if (ui->comboBox->currentText()=="Intermediate Pacific") amdata->set_Type(0,"IP");
            if (ui->comboBox->currentText()=="Deep Pacific") amdata->set_Type(0,"DP");
            if (ui->comboBox->currentText()=="Intermediate Indian") amdata->set_Type(0,"II");
            if (ui->comboBox->currentText()=="Deep Indian") amdata->set_Type(0,"DI");
            if (ui->comboBox->currentText()=="EDML") amdata->set_Type(0,"EDML");
            if (ui->comboBox->currentText()=="NGRIP") amdata->set_Type(0,"NGRIP");
            if (ui->comboBox->currentText()=="LR04") amdata->set_Type(0,"LR04");
             if (ui->comboBox->currentText()=="Custom Target") amdata->set_Type(0,"CT");
            amdata->set_LabID(0,"User");
            if (ui->comboBox->currentText()!="Custom Target")amdata->set_Age_Comment("Manually tuned with :"+ui->comboBox->currentText(),0); else {
                QStringList str=ui->lineEdit->text().split("/");

                amdata->set_Age_Comment("Manually tuned with :"+str.at(str.size()-1),0);
            }
            if (cor_plot->getSelected_Y(1)>=0) {
                amdata->set_Data(0,0,NAN);
                amdata->set_Data(1,0,NAN);
                amdata->set_Data(2,0,NAN);
                amdata->set_Data(3,0,NAN);
                if (ui->comboBox->currentText()!="Custom Target"){
                    amdata->set_Data(4,0,inv->get_Target_Age(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()));
                    amdata->set_Data(5,0,inv->get_Target_Age(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex())-2*inv->get_Target_Age_Error(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()));
                    amdata->set_Data(6,0,inv->get_Target_Age(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex())+2*inv->get_Target_Age_Error(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()));
                } else {
                    amdata->set_Data(4,0,target_age[0]->at(cor_plot->getSelected_Y(1)).toFloat());
                    amdata->set_Data(5,0,target_age[2]->at(cor_plot->getSelected_Y(1)).toFloat());
                    amdata->set_Data(6,0,target_age[3]->at(cor_plot->getSelected_Y(1)).toFloat());
                }
                amdata->set_Data(7,0,1);
                amdata->set_Sample_Thickness(0,NAN);
                amdata->set_Reservoir_Error(0,NAN);
            } else {
                amdata->set_Data(4,0,0);
            }
            amdata->sort(0,amdata->get_Length()-1);

            createLines();


            createAMPlot();
            createSRPlot();
            createResultPlot();
            createCorPlot(0);


            setupTable(0);
            update();

        } else {
            QMessageBox msgBox;
                    msgBox.setText("Adding this connection would create a reversal.");
                    msgBox.setInformativeText("Please deactivate the conflicting connection.");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
        }
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
                setupTable(0);

            }
            am_plot->setSelected_Y(-1);
            createAMPlot();
        } else {
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
                    setupTable(0);

                }
                createAMPlot();
            }
        }
        ui->tableView->repaint();
        changes=true;
    }
}

void Correlate::newPoint(){



        amdata->addEntry();


        select=0;

        createLines();


        createAMPlot();
        createSRPlot();
        createResultPlot();
        createCorPlot(0);


        setupTable(0);
        update();


        ui->tableView->repaint();
        changes=true;





}



void Correlate::removePoint(){
    if (select==0 && amdata->get_Length()==1){
        amdata->set_Depth(select,NAN);
        amdata->set_LabID(select,"Unknown");
        amdata->set_Type(select,"Unknown");
        amdata->set_Age_Comment("default Entry: Please Edit",select);
        for (int i=0;i<7;i++) amdata->set_Data(i,select,NAN);
        amdata->set_Data(7,select,0);
        amdata->set_Sample_Thickness(0,NAN);
        amdata->set_Reservoir_Error(0,NAN);
    }
    if (select>=0 && amdata->get_Length()>1){
        //qDebug() << "remove"+QString::number(amdata->get_Length());

        amdata->deleteEntry(select);
    }

    if (select>=amdata->get_Length()) select--;
        amdata->setSelect(select);
        createLines();

        createAMPlot();
        createSRPlot();
        createResultPlot();
        createCorPlot(0);
        setupTable(0);
        update();
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
                setupTable(0);

            }
            am_plot->setSelected_Y(-1);
            createAMPlot();
        } else {
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
                    setupTable(0);

                }
                createAMPlot();
            }
        }
        ui->tableView->repaint();
    changes=true;
}

void Correlate::editPoint(){

    if (select>=0&&amdata->get_Length()>select){
    delete edit;
    edit=new AMEdit(this,amdata,select,proxy);
    edit->setModal(true);
    edit->show();
    }
    amdata->sort(0,amdata->get_Length()-1);

    createLines();

    createAMPlot();
    createSRPlot();
    createResultPlot();
    createCorPlot(0);
    setupTable(0);
    update();
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
            setupTable(0);

        }
        am_plot->setSelected_Y(-1);
        createAMPlot();
    } else {
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
                setupTable(0);

            }
            createAMPlot();
        }
    }
    ui->tableView->repaint();
    changes=true;
}

void Correlate::tableSelected(QModelIndex mi){
    /*int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel);
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QString text = model->item(sel,0)->text();
    selected=text.toInt(0,10);*/
    int reset=1;
    int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QString text = model->item(sel,mi.column())->text();
    //qDebug() << text;
    int index=model->item(sel,0)->text().toInt();

    if (mi.column()==13){
        if (amdata->get_Data(7,sel)==1){
            amdata->set_Data(7,sel,0);
        } else {
            if (!isnan(amdata->get_Data(4,sel))) {
                // Check if activation will create reversal
                double depth=amdata->get_Depth(sel);
                double age=amdata->get_Data(4,sel);
                int ok=1;
                for (int i=0;i<amdata->get_Length();i++){
                    if (amdata->get_Data(7,i)==1){
                        float depth_2=amdata->get_Depth(i);
                        float age_2=amdata->get_Data(4,i);
                        if (age<age_2 && depth>depth_2) ok=0;
                        if (age>age_2 && depth<depth_2) ok=0;
                        if (age==age_2 || depth==depth_2) ok=0;
                    }
                }
                if (ok) amdata->set_Data(7,sel,1); else {
                    QMessageBox msgBox;
                            msgBox.setText("Activating this connection would create a reversal.");
                            msgBox.setInformativeText("Please deactivate the conflicting connection.");
                            msgBox.setIcon(QMessageBox::Warning);

                            msgBox.exec();
                }

            }
        }
        select=index;

        amdata->setSelect(index);
        setupTable(13);
        reset=0;

    }
    if (mi.column()==14){
        delete attd;
        attd=new attDialog(this,amdata->get_Age_Comment(index),QString::number(index));
        attd->setModal(true);
        attd->show();
        select=index;

        amdata->setSelect(index);
        setupTable(14);
        reset=0;
    }
    if (mi.column()<13){
        select=index;

        amdata->setSelect(index);
        setupTable(0);
        reset=0;
    }



    // recreate Age model plot
    createAMPlot();

    // recreate Sedimentation Rate Plot
    createSRPlot();

    // create result of tuning plot
    createResultPlot();

    // create interactive plots for Data and Traget tuning

    createCorPlot(reset);





    update();

}



void Correlate::apply(){



    amdata->AMSSave();
    changes=false;

    QMessageBox msgBox;
            msgBox.setText("Agedata was saved succesfully.");


            msgBox.exec();
}

void Correlate::applyAM(){
    //qDebug()<<"Apply Model";
    //amdata->apply_Age_Model("linear");
    for (int i=0;i<inv->get_Length();i++){
        double v=NAN;
        v=amdata->get_Int_Value_Age(inv->get_data_Depth(i),"linear");

        inv->set_data_Age(v,i);
    }
    inv->saveData();
    QMessageBox msgBox;
            msgBox.setText("Agedata was added to the proxy data. This enables plotting vs. age in other tools.");


            msgBox.exec();
}


void Correlate::target_Selected(QString str){
    if (str!="Custom Target"){
        if (str=="Govin")
            target_axis=3;
        if (str=="LH4") target_axis=4;
        //target=str;
        ndata=ui->comboBox_2->currentIndex();
        createCorPlot(1);
        cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
        ui->pushButton_7->setEnabled(false);
         ui->comboBox_3->setEnabled(false);
    } else {
        ui->pushButton_7->setEnabled(true);
         ui->comboBox_3->setEnabled(true);
        qDebug()<<"Custom Target Selected";
        if (ui->lineEdit->text()=="No Target Selected") {
            //Create empty target data
            target_meta.clear();
            target_length=0;

            target_data.clear();
            target_data.reserve(inv->access_data_length());
            for (int i=0;i<inv->access_data_length();i++) {
                target_data.append(new QStringList());
                for (int j=0;j<target_length;j++){
                    target_data[i]->append("");
                }
            }

            target_age.clear();
            target_data.reserve(4);
            for (int i=0;i<4;i++) {
                target_data.append(new QStringList());
                for (int j=0;j<target_length;j++){
                    target_data[i]->append("");
                }
            }
            ndata=ui->comboBox_2->currentIndex();
            createCorPlot(1);
            cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());

        } else {
            // Read data from targetfile

            readTarget();
            ndata=ui->comboBox_2->currentIndex();
            createCorPlot(1);
            cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
        }

    }
    update();
}

void Correlate::data_Selected(QString str){
        ui->comboBox_3->setCurrentIndex(ui->comboBox_2->currentIndex());
        ndata=ui->comboBox_2->currentIndex();
        createCorPlot(1);
        createResultPlot();
        cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
        result_plot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());



    setupTable(0);

    update();
}
void Correlate::data_Selected2(QString str){

        ndata=ui->comboBox_2->currentIndex();
        createCorPlot(1);
        createResultPlot();
        cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
        result_plot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());



    setupTable(0);

    update();
}

void Correlate::attChanged(QString text,QString origin){
        int sel=origin.toInt(0,10);
        //qDebug() << "Text:"+text;
        amdata->set_Age_Comment(text,sel);

        setupTable(14);
        changes=true;
}

void Correlate::redraw(){
    setupTable(0);
}

void Correlate::createAMPlot(){
    delete[] data1;
    data1=new float[amdata->get_Length()*2];
    delete[] data_use1;
    data_use1=new bool[amdata->get_Length()*2];
    delete [] data_error1;
    data_error1=new float[amdata->get_Length()*2];
    delete[] data_error21;
    data_error21=new float[amdata->get_Length()*2];
    delete[] col1;
    col1=new QColor[amdata->get_Length()*2];
    delete[] mark1;
    mark1=new int[amdata->get_Length()*2];

    for (int i=0;i<amdata->get_Length();i++){
        //data1[i+0*amdata->get_Length()]=amdata->get_Depth(i);
        //data1[i+1*amdata->get_Length()]=amdata->get_Data(0,i);
        data1[i+0*amdata->get_Length()]=amdata->get_Depth(i);
        data1[i+1*amdata->get_Length()]=amdata->get_Data(4,i);
        //data_use[i+0*amdata->get_Length()]=amdata->get_Data(7,i);
        //data_use[i+1*amdata->get_Length()]=amdata->get_Data(7,i);
        data_use1[i+0*amdata->get_Length()]=amdata->get_Data(7,i);
        data_use1[i+1*amdata->get_Length()]=amdata->get_Data(7,i);
        //data_error[i+0*amdata->get_Length()]=NAN;
        //data_error[i+1*amdata->get_Length()]=amdata->get_Data(2,i);
        data_error1[i+0*amdata->get_Length()]=NAN;
        if (amdata->get_Data(5,i)!=0) data_error1[i+1*amdata->get_Length()]=abs(amdata->get_Data(4,i)-amdata->get_Data(5,i)); else data_error1[i+1*amdata->get_Length()]=NAN;
        //data_error2[i+0*amdata->get_Length()]=NAN;
        //data_error2[i+1*amdata->get_Length()]=amdata->get_Data(1,i);
        data_error21[i+0*amdata->get_Length()]=NAN;
        if (amdata->get_Data(6,i)!=0) data_error21[i+1*amdata->get_Length()]=abs(amdata->get_Data(4,i)-amdata->get_Data(6,i)); else data_error21[i+1*amdata->get_Length()]=NAN;
        //col_r[i+0*amdata->get_Length()]=255;
        //col_r[i+1*amdata->get_Length()]=255;
        col1[i+0*amdata->get_Length()]=QColor(0,0,0);
        col1[i+1*amdata->get_Length()]=QColor(0,0,255);

        //if (amdata->get_Age_Comment(i).simplified()!="NaN") comment[i+2*amdata->get_Length()]=amdata->get_Age_Comment(i);
        //mark[i+0*amdata->get_Length()]=0;
        //mark[i+1*amdata->get_Length()]=0;
        mark1[i+0*amdata->get_Length()]=0;
        mark1[i+1*amdata->get_Length()]=0;
    }
    //mark[select+0*amdata->get_Length()]=1;
    //mark[select+1*amdata->get_Length()]=1;
    mark1[select+0*amdata->get_Length()]=1;
    mark1[select+1*amdata->get_Length()]=1;
    am_plot->setData(data1,2,amdata->get_Length());
    am_plot->setUse(data_use1,1);
    am_plot->setError(data_error1,2);
    am_plot->setError2(data_error21,2);
    am_plot->setColor(col1,1);
    am_plot->setMark(mark1,1);
    //am_plot->setComment(comment,1);
    am_plot->setTitel("","Depth [m]","Age [ka BP]");
    am_plot->setMultiplicator(1,1);
    am_plot->setTextSize(resources->get_Pointsize(),1,12);
    am_plot->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    am_plot->setSymbol(3);

    am_plot->autoSize();
    am_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Correlate_AgeModel.txt",0);
    ui->graphicsView_3->setScene(am_plot);
    ui->graphicsView_3->repaint();
}

void Correlate::createResultPlot(){
    delete[] data2;
    data2=new float[inv->get_Length()*2];
    delete[] use2;
    use2=new bool[inv->get_Length()*2];
    delete[] comment2;
    comment2=new QString[inv->get_Length()*2];
    for (int i=0;i<inv->get_Length();i++){
            data2[i+0*inv->get_Length()]=amdata->get_Int_Value_Age(inv->get_data_Depth(i),"linear");
            data2[i+1*inv->get_Length()]=inv->access_data_value(ndata,i).toFloat();

            use2[i+0*inv->get_Length()]=inv->get_data_Use_Flag(i);
            use2[i+1*inv->get_Length()]=inv->get_data_Use_Flag(i);
            float multy=1;
            float multx=1;
            if (inv->access_data_type(ndata).contains("multx")){
                QStringList str=inv->access_data_type(ndata).split(";");
                for (int j=0;j<str.size();j++) {
                    if (str[j].contains("multx")) {
                        QStringList str2=str[j].split("=");
                        multx=str2[1].toFloat();
                    }
                }
            }
            if (inv->access_data_type(ndata).contains("multy")){
                QStringList str=inv->access_data_type(ndata).split(";");
                for (int j=0;j<str.size();j++) {
                    if (str[j].contains("multy")) {
                        QStringList str2=str[j].split("=");
                        multy=str2[1].toFloat();
                    }
                }
            }
            result_plot->setMultiplicator(multx,multy);
            QString str=inv->access_data_name(ndata);
            if (inv->access_data_unit(ndata)!="") str.append(" ["+inv->access_data_unit(ndata)+"]");
            result_plot->setTitel("","Age in [kyr]",str);



        if (inv->get_data_Comment(i).simplified()!="NaN") comment2[i+1*inv->get_Length()]=inv->get_data_Comment(i);
    }
    if (amdata->get_Length()>1)  {
        result_plot->setData(data2,2,inv->get_Length());
        result_plot->setUse(use2,1);
        result_plot->setComment(comment2,1);
        result_plot->setTextSize(resources->get_Pointsize(),1,12);
        result_plot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());
        result_plot->setSymbol(3);
        result_plot->autoSize();
    }
    result_plot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Correlate_Result_"+inv->access_proxy_abbreviation()+".txt",0);
    ui->graphicsView_4->setScene(result_plot);
    ui->graphicsView_4->repaint();
}

void Correlate::createSRPlot(){
    // Calculate sedimentation rate
    // create subset of active Flag only
    // get length
    int active_count=0;
    for (int i=0;i<amdata->get_Length();i++) if (amdata->get_Data(7,i)==1) active_count++;
    double *temp_age;
    double *temp_depth;
    temp_age=new double[active_count];
    temp_depth=new double[active_count];
    int n=0;
    for (int i=0;i<amdata->get_Length();i++){
        if (amdata->get_Data(7,i)==1){
            temp_age[n]=amdata->get_Data(4,i);
            temp_depth[n]=amdata->get_Depth(i);
            n++;
        }
    }

    // calculate derivertive
    if (active_count>1){
        float *age=new float[active_count-1];
        float *sr=new float[active_count-1];
        float *age_area=new float[active_count-1];
        for (int i=0;i<active_count-1;i++){
            age[i]=temp_age[i]+(temp_age[i+1]-temp_age[i])/2;
            age_area[i]=(temp_age[i+1]-temp_age[i])/2;
            sr[i]=(temp_depth[i+1]-temp_depth[i])/(temp_age[i+1]-temp_age[i]);
        }
        // create Plot
        delete[] data3;
        data3=new float[(active_count-1)*2*2];
        delete[] col3;
        col3=new QColor[(active_count-1)*2*2];

        for (int i=0;i<active_count-1;i++){
            data3[i*2+0*(active_count-1)*2]=age[i]-age_area[i];
            data3[i*2+1*(active_count-1)*2]=sr[i];
            data3[(i*2+1)+0*(active_count-1)*2]=age[i]+age_area[i];
            data3[(i*2+1)+1*(active_count-1)*2]=sr[i];
            col3[i*2+0*(active_count-1)*2].QColor::red();
            col3[i*2+1*(active_count-1)*2].QColor::red();
            col3[(i*2+1)+0*(active_count-1)*2].QColor::red();
            col3[(i*2+1)+1*(active_count-1)*2].QColor::red();

        }
        sr_plot->setData(data3,2,(active_count-1)*2);
        sr_plot->setTitel("","Age [ka BP]","Rate [m/ka]");
        sr_plot->setMultiplicator(1,1);
        sr_plot->setTextSize(resources->get_Pointsize(),1,12);
        sr_plot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        sr_plot->setSymbol(1);
        sr_plot->setColor(col3,1);
        sr_plot->setLineWidth(3);
        sr_plot->setLineStyle(Qt::SolidLine);
        sr_plot->setLineColor(Qt::red);
        sr_plot->autoSize();
        sr_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Correlation_Sedimentation.txt",0);
        ui->graphicsView_2->setScene(sr_plot);

        delete[] age;
        delete[] sr;
        delete[] age_area;
    } else {
        delete[] data3;
        data3=new float[0];
        sr_plot->setData(data3,0,0);
        sr_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Correlation_Sedimentation.txt",0);
        ui->graphicsView_2->setScene(sr_plot);
    }
    delete[] temp_age;
    delete[] temp_depth;
    ui->graphicsView_2->repaint();
}

bool Correlate::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->tableView||obj==ui->graphicsView_2||ui->graphicsView_3||obj==ui->graphicsView_4){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);
                ui->splitter_2->restoreState(sp_2);
                ui->splitter_3->restoreState(sp_3);
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void Correlate::createCorPlot(int autosize){
    float multy=1;
    float multx=1;
    if (inv->access_data_type(ndata).contains("multx")){
        QStringList str=inv->access_data_type(ndata).split(";");
        for (int j=0;j<str.size();j++) {
            if (str[j].contains("multx")) {
                QStringList str2=str[j].split("=");
                multx=str2[1].toFloat();
            }
        }
    }
    if (inv->access_data_type(ndata).contains("multy")){
        QStringList str=inv->access_data_type(ndata).split(";");
        for (int j=0;j<str.size();j++) {
            if (str[j].contains("multy")) {
                QStringList str2=str[j].split("=");
                multy=str2[1].toFloat();
            }
        }
    }
    // Get Isotope data
    int y_axis=0;
    //cor_plot->setNumber(2);
    delete[] data4;
    data4=new float[inv->get_Length()*2];
    delete[] use4;
    use4=new bool[inv->get_Length()*2];
    // Data to plot
    for (int i=0;i<inv->get_Length();i++){

            data4[i+0*inv->get_Length()]=inv->get_data_Depth(i);
            data4[i+1*inv->get_Length()]=inv->access_data_value(ndata,i).toFloat();
            use4[i+0*inv->get_Length()]=inv->get_data_Use_Flag(i);
            use4[i+1*inv->get_Length()]=inv->get_data_Use_Flag(i);

    }

    cor_plot->setMultiplicator(multx,multy);
    QString str="";
    if (inv->get_att_Optional().contains("%"+inv->access_data_name(ndata)+"=")) {
        QStringList list=inv->get_att_Optional().split(";");
        for(int j=0;j<list.size();j++) if(list[j].contains("%"+inv->access_data_name(ndata)+"=")) if (list[j].split("=").size()>0) str=list[j].split("=").at(1);
    } else {
        str=inv->access_data_name(ndata);
    }

    if (inv->access_data_unit(ndata)!="") str.append(" ["+inv->access_data_unit(ndata)+"]");
    cor_plot->setTitel("","Depth [m]",str);
    y_axis=5;

    cor_plot->setData(data4,2,inv->get_Length(),0);
    cor_plot->setUse(use4,1,0);
    cor_plot->setTextSize(resources->get_Pointsize(),1,0,0);

    cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    cor_plot->setSymbol(3,0);
    cor_plot->fixRange(0,1,0);

    if (autosize) cor_plot->autoSize(0);
    qDebug()<<"Data Plot finished";
    // Target Plot
    int ndata2=ui->comboBox_3->currentIndex();
    multy=1;
    multx=1;
    if (inv->access_data_type(ndata2).contains("multx")){
        QStringList str=inv->access_data_type(ndata2).split(";");
        for (int j=0;j<str.size();j++) {
            if (str[j].contains("multx")) {
                QStringList str2=str[j].split("=");
                multx=str2[1].toFloat();
            }
        }
    }
    if (inv->access_data_type(ndata2).contains("multy")){
        QStringList str=inv->access_data_type(ndata2).split(";");
        for (int j=0;j<str.size();j++) {
            if (str[j].contains("multy")) {
                QStringList str2=str[j].split("=");
                multy=str2[1].toFloat();
            }
        }
    }



    int length=0;
    if (ui->comboBox->currentText()!="Custom Target") {
        length=inv->get_Target_Length(ui->comboBox->currentIndex());
        if (inv->get_Target_Length(ui->comboBox->currentIndex())<inv->get_Length()) length=inv->get_Length();
    } else {
        length=target_length;
        if (target_length<inv->get_Length()) length=inv->get_Length();
    }

    delete[] data5;
    delete[] data_error5;
    delete[] data_error25;
    delete[] use5;
    delete[] c5;
    delete[] lw5;
    delete[] ss5;
    delete[] ps5;
    data5=new float[length*4];
    data_error5=new float[length*4];
    data_error25=new float[length*4];
    use5=new bool[length*4];
    c5=new QColor[4];
    lw5=new int[4];
    ss5=new int[4];
    ps5=new Qt::PenStyle[4];

    for (int i=0;i<length*4;i++) data5[i]=NAN;
    for (int i=0;i<length*4;i++) data_error5[i]=NAN;
    for (int i=0;i<length*4;i++) data_error25[i]=NAN;
    for (int i=0;i<length*4;i++) use5[i]=true;
    // Calc Offsets
    double offset_1=0;
    double offset_2=0;
    double min_1=999999;
    double min_2=999999;
    double max_1=-999999;
    double max_2=-999999;
    for (int i=0;i<inv->get_Length();i++){
        if (isnan(inv->access_data_value(ndata,i).toFloat())==false){
            if (ui->checkBox_2->isChecked()){
                offset_1+=inv->access_data_value(ndata,i).toFloat()*multy;
                if (min_1>inv->access_data_value(ndata,i).toFloat()*multy) min_1=inv->access_data_value(ndata,i).toFloat()*multy;
                if (max_1<inv->access_data_value(ndata,i).toFloat()*multy) max_1=inv->access_data_value(ndata,i).toFloat()*multy;
            } else {
                offset_1+=inv->access_data_value(ndata,i).toFloat()*multy*(-1);
                if (min_1>inv->access_data_value(ndata,i).toFloat()*multy*(-1)) min_1=inv->access_data_value(ndata,i).toFloat()*multy*(-1);
                if (max_1<inv->access_data_value(ndata,i).toFloat()*multy*(-1)) max_1=inv->access_data_value(ndata,i).toFloat()*multy*(-1);
            }
        }

    }
    offset_1/=inv->get_Length();

    int count=0;
    if (ui->comboBox->currentText()!="Custom Target"){
        for (int i=0;i<inv->get_Target_Length(ui->comboBox->currentIndex());i++) {
            if (!isnan(inv->get_Target_Value(i,ui->comboBox->currentIndex()))){
                count++;
                offset_2+=inv->get_Target_Value(i,ui->comboBox->currentIndex());
                if (min_2>inv->get_Target_Value(i,ui->comboBox->currentIndex())) min_2=inv->get_Target_Value(i,ui->comboBox->currentIndex());
                if (max_2<inv->get_Target_Value(i,ui->comboBox->currentIndex())) max_2=inv->get_Target_Value(i,ui->comboBox->currentIndex());
            }
        }
    offset_2/=count;
    } else {
        for (int i=0;i<target_length;i++) {
            if (!isnan(target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat())){
                count++;
                offset_2+=target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat();
                if (min_2>target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat()) min_2=target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat();
                if (max_2<target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat()) max_2=target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat();
            }
        }
    offset_2/=count;
    }

    double f=(max_1-min_1)/(max_2-min_2);
    qDebug()<<QString::number(f);

    for (int i=0;i<inv->get_Length();i++){
        // Result Data
        data5[i+0*length]=amdata->get_Int_Value_Age(inv->get_data_Depth(i),"linear");
        if (ui->checkBox_2->isChecked()) data5[i+1*length]=(((inv->access_data_value(ndata,i).toFloat()*multy)-offset_1)/f+offset_2);
        else data5[i+1*length]=(((inv->access_data_value(ndata,i).toFloat()*multy*(-1))-offset_1)/f+offset_2);
        use5[i+0*length]=inv->get_data_Use_Flag(i);
        use5[i+1*length]=inv->get_data_Use_Flag(i);
    }

    if (ui->comboBox->currentText()!="Custom Target"){
        for (int i=0;i<inv->get_Target_Length(ui->comboBox->currentIndex());i++){
            // Target Data
            data5[i+2*length]=inv->get_Target_Age(i,ui->comboBox->currentIndex());
            data5[i+3*length]=inv->get_Target_Value(i,ui->comboBox->currentIndex());
            data_error5[i+2*length]=inv->get_Target_Age_Error(i,ui->comboBox->currentIndex());
            data_error5[i+3*length]=inv->get_Target_Error(i,ui->comboBox->currentIndex());

            if (ui->checkBox->isChecked()) cor_plot->setMultiplicator(1,1,1); else cor_plot->setMultiplicator(1,-1,1);

            cor_plot->setTitel("","Age [ka BP]","Target",1);

        }
    } else {
        qDebug()<<"test";
        for (int i=0;i<target_length;i++){
            // Target Data
            data5[i+2*length]=target_age[0]->at(i).toFloat();
            data5[i+3*length]=target_data[ui->comboBox_3->currentIndex()]->at(i).toFloat();
            data_error25[i+2*length]=target_age[3]->at(i).toFloat()-target_age[0]->at(i).toFloat();
            data_error5[i+2*length]=target_age[0]->at(i).toFloat()-target_age[2]->at(i).toFloat();
            // estimate errordataposition for chosen values
            QString str=inv->access_data_type(ui->comboBox_3->currentIndex());
            int error=-1;
            if (str.contains("error=")){
                QStringList str_list=str.split(";");
                for (int i=0;i<str_list.size();i++) if (str_list.at(i).contains("error=")) error=str_list.at(i).split("=").at(1).toInt();
            }

            if (error!=-1) data_error5[i+3*length]=target_data[error]->at(i).toFloat(); else data_error5[i+3*length]=NAN;

            if (ui->checkBox->isChecked()) cor_plot->setMultiplicator(1,1,1); else cor_plot->setMultiplicator(1,-1,1);

            cor_plot->setTitel("","Age [ka BP]","Target",1);


        }
    }
    cor_plot->setData(data5,4,length,1);
    cor_plot->setUse(use5,2,1);
    cor_plot->setError(data_error5,4,1);
    if (ui->comboBox->currentText()=="Custom Target") cor_plot->setError2(data_error25,1,1);
        else cor_plot->setError2(data_error25,0,1);
    cor_plot->setTextSize(resources->get_Pointsize(),1,0,1);
    cor_plot->setSize(ui->graphicsView->width()-30,ui->graphicsView->height());
    cor_plot->setSymbol(3,1);
    cor_plot->fixRange(0,1,1);

    c5[0]=Qt::red;
    c5[1]=Qt::red;
    c5[2]=Qt::black;
    c5[3]=Qt::black;
    cor_plot->setSetLineColor(c5,1,1);

    lw5[0]=3;
    lw5[1]=3;
    lw5[2]=1;
    lw5[3]=1;
    cor_plot->setSetLineWidth(lw5,1,1);

    ss5[0]=0;
    ss5[1]=0;
    ss5[2]=3;
    ss5[3]=3;
    cor_plot->setSetSymbolsize(ss5,1,1);

    ps5[0]=Qt::SolidLine;
    ps5[1]=Qt::SolidLine;
    ps5[2]=Qt::DashDotLine;
    ps5[3]=Qt::DashDotLine;
    cor_plot->setSetLineStyle(ps5,1,1);
    if (autosize) {
        cor_plot->autoSize(1);
        qDebug()<<QString::number(cor_plot->get_Min_X(1));
        if (cor_plot->get_Max_X(1)>150) cor_plot->setView(-10,160,cor_plot->get_Min_Y(1),cor_plot->get_Max_Y(1),1);
    }
    cor_plot->setAxisType(1,0,0);

    createLines();

    ss5[0]=0;
    ss5[1]=0;
    ss5[2]=cor_plot->getSymbol(1);
    ss5[3]=cor_plot->getSymbol(1);

    cor_plot->setSetSymbolsize(ss5,1,1);
    cor_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Correlate_Isotope_"+inv->access_proxy_abbreviation()+".txt",0);
    cor_plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Correlate_Target.txt",1);
    ui->graphicsView->setScene(cor_plot);
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->repaint();
    qDebug()<<"Cor Plot finished";
}

void Correlate::createLines(){
    qDebug()<<"Creating lines";
    int y_axis=0;


        y_axis=5;

    cor_plot->addLineClear();
    for(int i=0;i<amdata->get_Length();i++){
        // get index in data 1
        int i_1=-1;

        for (int j=0;j<inv->get_Length();j++){
             if (inv->get_data_Depth(j)>=amdata->get_Depth(i)){
                 i_1=j;

                 break;
             }

        }


        float v1=NAN;


        v1=inv->access_data_value(ndata,0).toFloat();
        if (i_1==-1 && amdata->get_Depth(amdata->get_Length()-1)>inv->get_data_Depth(inv->get_Length()-1)) v1=inv->access_data_value(ndata,inv->get_Length()-1).toFloat();



        if (i_1>0 && i_1<inv->get_Length()){
                v1=inv->access_data_value(ndata,i_1-1).toFloat()+(inv->access_data_value(ndata,i_1).toFloat()-inv->access_data_value(ndata,i_1-1).toFloat())/(inv->get_data_Depth(i_1)-inv->get_data_Depth(i_1-1))*(amdata->get_Depth(i)-inv->get_data_Depth(i_1-1));
        }
        int i_2=-1;
        float v2=NAN;
        if (ui->comboBox->currentText()!="Custom Target"){

            for (int j=0;j<inv->get_Target_Length(ui->comboBox->currentIndex());j++){
                 if (inv->get_Target_Age(j,ui->comboBox->currentIndex())>=amdata->get_Data(4,i)){
                     i_2=j;
                     break;
                 }

            }

            v2=inv->get_Target_Value(0,ui->comboBox->currentIndex());
            if (i_2==-1 && amdata->get_Data(4,amdata->get_Length()-1)>inv->get_Target_Age(inv->get_Target_Length(ui->comboBox->currentIndex())-1,ui->comboBox->currentIndex())) v2=inv->get_Target_Value(inv->get_Target_Length(ui->comboBox->currentIndex())-1,ui->comboBox->currentIndex());

            if (i_2>0 && i_2<inv->get_Target_Length(ui->comboBox->currentIndex())){
                v2=inv->get_Target_Value(i_2-1,ui->comboBox->currentIndex())+(inv->get_Target_Value(i_2,ui->comboBox->currentIndex())-inv->get_Target_Value(i_2-1,ui->comboBox->currentIndex()))/(inv->get_Target_Age(i_2,ui->comboBox->currentIndex())-inv->get_Target_Age(i_2-1,ui->comboBox->currentIndex()))*(amdata->get_Data(4,i)-inv->get_Target_Age(i_2-1,ui->comboBox->currentIndex()));
            }
        } else {
            if (target_length>0){
                for (int j=1;j<target_length;j++){
                    if (!isnan(target_age[0]->at(j).toFloat())&&!isnan(target_age[0]->at(j-1).toFloat())){
                     if (target_age[0]->at(j).toFloat()>=amdata->get_Data(4,i)){
                         i_2=j;
                         break;
                     }
                    }

                }
                qDebug()<<QString::number(i_2);
                int min=target_length-1;
                int max=0;
                for (int j=0;j<target_length;j++){
                    if (!isnan(target_age[0]->at(j).toFloat()) && j<min) min =j;
                    if (!isnan(target_age[0]->at(j).toFloat()) && j>max) max =j;
                }
                qDebug()<<QString::number(min)+":"+QString::number(max);
                v2=target_data[ui->comboBox_2->currentIndex()]->at(min).toFloat();
                if (i_2==-1 && amdata->get_Data(4,amdata->get_Length()-1)>target_age[0]->at(max).toFloat()) v2=target_data[ui->comboBox_2->currentIndex()]->at(max).toFloat();

                if (i_2>0 && i_2<target_length){
                    v2=target_data[ui->comboBox_2->currentIndex()]->at(i_2-1).toFloat()+(target_data[ui->comboBox_2->currentIndex()]->at(i_2).toFloat()-target_data[ui->comboBox_2->currentIndex()]->at(i_2-1).toFloat())/(target_age[0]->at(i_2).toFloat()-target_age[0]->at(i_2-1).toFloat())*(amdata->get_Data(4,i)-target_age[0]->at(i_2-1).toFloat());
                }
            } else {
                i_2=-1;
                v2=0;
            }
        }

        QColor c=Qt::darkGreen;
        if (amdata->get_Type(i)=="AMS") c=Qt::lightGray;
        if (amdata->get_Data(7,i)==0) c=Qt::red;
        int w=2;
        if (i==select) w=3;
        Qt::PenStyle p=Qt::SolidLine;
        if (amdata->get_Type(i)=="AMS") p=Qt::DashLine;
        cor_plot->addLine(amdata->get_Depth(i),v1,0,amdata->get_Data(4,i),v2,1,c,w,p);
    }
    if (cor_plot->getSelected_Y(0)!=-1 && cor_plot->getSelected_Y(1)!=-1){
         if (ui->comboBox->currentText()!="Custom Target"){
             cor_plot->addLine(inv->get_data_Depth(cor_plot->getSelected_Y(0)),inv->access_data_value(ndata,cor_plot->getSelected_Y(0)).toFloat(),0,inv->get_Target_Age(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()),inv->get_Target_Value(cor_plot->getSelected_Y(1),ui->comboBox->currentIndex()),1,Qt::black,1,Qt::DashLine);
         } else {
             cor_plot->addLine(inv->get_data_Depth(cor_plot->getSelected_Y(0)),inv->access_data_value(ndata,cor_plot->getSelected_Y(0)).toFloat(),0,target_age[0]->at(cor_plot->getSelected_Y(1)).toFloat(),target_data[ui->comboBox_2->currentIndex()]->at(cor_plot->getSelected_Y(1)).toFloat(),1,Qt::black,1,Qt::DashLine);
         }
    }
    cor_plot->setAddMode(1);
    cor_plot->update();
}

void Correlate::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    if (changes) {
        resBtn = QMessageBox::question( this,"PaleoDataView - Correleate",
                                        tr("There are unsaved changes.\nAre you sure?\n"),
                                        QMessageBox::Cancel | QMessageBox::Yes,
                                        QMessageBox::Yes);
    }
    if (resBtn == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void Correlate::clickRepaint(int i,float x,float y){
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
            setupTable(0);

        }
        am_plot->setSelected_Y(-1);
        createAMPlot();
    } else {
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
                setupTable(0);

            }
            createAMPlot();
        }
    }
    ui->tableView->repaint();
}

void Correlate::updateGraph(){

    createAMPlot();
    createResultPlot();
    createSRPlot();
    createCorPlot(1);
}



void Correlate::copy(QPoint pos){

    QPoint globalPos = ui->tableView->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView->model();
            if (tableModel!=nullptr){
            int iRows = tableModel->rowCount();
            int iCols = tableModel->columnCount();
            for (int i=0;i<iCols;i++){
                QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                txt.replace("\n"," ");
                clipboardString.append(txt+"\t");
            }
            clipboardString.append("\n");

                QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
                qSort(selectedIndexes);
                for (int i = 0; i < selectedIndexes.count(); ++i){
                    QModelIndex current = selectedIndexes[i];
                    QString displayText = current.data(Qt::DisplayRole).toString();

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
        }
    }
}

void Correlate::readTarget(){
    QString QFilename=ui->lineEdit->text();
    error_text.clear();
    QString action="Reading Proxy";



    // get file name


    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    qDebug() << QFilename;

    // Initialize ids and meta
    int ncid;

    int retval;// for Errors

    // Open the file
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       NetCDF_Error(filename,action,retval);

    target_meta.clear();

    for (int i=0;i<inv->access_meta_length();i++){
        if (inv->access_meta_type(i).contains("string") || inv->access_meta_type(i).contains("combo") ) {
           if (!inv->access_meta_netcdf(i).contains("ignore")){
                QString att_name=inv->access_meta_netcdf(i).split(";").at(0);
                size_t attlen;
                 QString err=filename;
                if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,att_name.toLatin1(),&attlen)))
                   NetCDF_Error(err+" : "+att_name,action,retval);
                qDebug()<<QString::number(attlen);
               unsigned char text[attlen+1];
               for (int ii=0;ii<attlen+1;ii++) text[ii]=' ';
                if ((retval = nc_get_att(ncid,NC_GLOBAL,att_name.toLatin1(),&text[0])))
                   NetCDF_Error(err+" : "+att_name,action,retval);
                QString str="";
                for (unsigned int j=0;j<attlen;j++){
                        str.append(text[j]);
                }
                target_meta.append(str);
                qDebug() << str;
           } else {
               target_meta.append("");
           }
        } else {
            if (!inv->access_meta_netcdf(i).contains("ignore")){
                QString att_name=inv->access_meta_netcdf(i).split(";").at(0);
                double att=0;
                QString err=filename;
                if ((retval = nc_get_att_double(ncid,NC_GLOBAL,att_name.toLatin1(),&att)))
                   NetCDF_Error(err+" : "+att_name,action,retval);
                target_meta.append(QString::number(att));
                qDebug() << QString::number(att);
            } else {
                target_meta.append("");
            }
        }
    }

    // read all data
    for (int i=0;i<inv->access_data_length();i++){
        // get length of variable
        target_length=0;
        size_t l=0;
        int lengthid;
        nc_inq_dimid(ncid,"Length",&lengthid);
        nc_inq_dimlen(ncid,lengthid,&l);
        target_length=l;
        qDebug() << "length "+QString::number(target_length);
        // create Stringlist
        target_data.append(new QStringList());
        for (int j=0;j<target_length;j++){
            target_data[i]->append("");
        }

        // read strings
        if (inv->access_data_type(i).contains("string") && inv->access_data_netcdf(i).contains("dim=")){

            // get dim of string
            size_t dimlen=0;
            QString dimname="";
            int dimid;
            // search for dimname
            if (inv->access_data_netcdf(i).contains("dim=")){
                QStringList l1=inv->access_data_netcdf(i).split(";");
                for (int ii=0;ii<l1.size();ii++){
                    if (l1.at(ii).contains("dim=")) {
                        QStringList l2=l1.at(ii).split("=");
                        dimname=l2.at(1);
                    }
                }
            }
            qDebug()<<dimname;
            // read dim
            if ((retval = nc_inq_dimid(ncid, dimname.toLatin1(), &dimid)))
               NetCDF_Error(filename,action,retval);
            if ((retval = nc_inq_dimlen(ncid, dimid, &dimlen)))
               NetCDF_Error(filename,action,retval);
            qDebug()<<QString::number(dimlen);
            //read string
            unsigned char text[dimlen*target_length];
            int varid;
            QString varname=inv->access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) NetCDF_Error(filename,action,retval);
            if ((retval = nc_get_var(ncid,varid,&text[0]))) NetCDF_Error(filename,action,retval);

            for (unsigned int ii=0;ii<target_length;ii++){
                QString str="";
                 for (unsigned int jj=0;jj<dimlen;jj++){
                     str.append(text[ii+target_length*jj]);
                 }
                 target_data[i]->replace(ii,str.simplified().toLatin1());
            //qDebug()<<data[i]->at(ii).toLatin1();
            }


        // read numbers
        } else {
            int varid;
            QString varname=inv->access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) NetCDF_Error(filename,action,retval);
            double* var=new double[target_length];
            if ((retval = nc_get_var_double(ncid,varid,&var[0]))) NetCDF_Error(filename,action,retval);
            for (unsigned int ii=0;ii<target_length;ii++){

            target_data[i]->replace(ii,QString::number(var[ii]));
            //qDebug()<<data[i]->at(ii).toLatin1();
            }
        }

    }
    // read Age model data
    // create Stringlist
    target_age.clear();

    for (int i=0;i<4;i++){
        target_age.append(new QStringList());
        for (int j=0;j<target_length;j++){
            target_age[i]->append("");
        }
    }

    {
        int varid;
        QString varname="Age Mean [ka]";
        qDebug()<<varname;
        if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) NetCDF_Error(filename,action,retval);
        double* var=new double[target_length];
        if ((retval = nc_get_var_double(ncid,varid,&var[0]))) NetCDF_Error(filename,action,retval);
        for (unsigned int ii=0;ii<target_length;ii++){

        target_age[0]->replace(ii,QString::number(var[ii]));
        //qDebug()<<data[i]->at(ii).toLatin1();
        }
    }
    {
        int varid;
        QString varname="Age Median [ka]";
        qDebug()<<varname;
        if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) NetCDF_Error(filename,action,retval);
        double* var=new double[target_length];
        if ((retval = nc_get_var_double(ncid,varid,&var[0]))) NetCDF_Error(filename,action,retval);
        for (unsigned int ii=0;ii<target_length;ii++){

        target_age[1]->replace(ii,QString::number(var[ii]));
        //qDebug()<<data[i]->at(ii).toLatin1();
        }
    }
    {
        int varid;
        QString varname="Age 05% [ka]";
        qDebug()<<varname;
        if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) NetCDF_Error(filename,action,retval);
        double* var=new double[target_length];
        if ((retval = nc_get_var_double(ncid,varid,&var[0]))) NetCDF_Error(filename,action,retval);
        for (unsigned int ii=0;ii<target_length;ii++){

        target_age[2]->replace(ii,QString::number(var[ii]));
        //qDebug()<<data[i]->at(ii).toLatin1();
        }
    }
    {
        int varid;
        QString varname="Age 95% [ka]";
        qDebug()<<varname;
        if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) NetCDF_Error(filename,action,retval);
        double* var=new double[target_length];
        if ((retval = nc_get_var_double(ncid,varid,&var[0]))) NetCDF_Error(filename,action,retval);
        for (unsigned int ii=0;ii<target_length;ii++){

        target_age[3]->replace(ii,QString::number(var[ii]));
        //qDebug()<<data[i]->at(ii).toLatin1();
        }
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       NetCDF_Error(filename,action,retval);

    delete[] filename;
    if (error_text!=""&& show_error==1) NetCDF_Error_Message();
    qDebug()<<"Target succesfully read";
}

void Correlate::NetCDF_Error(QString filename,QString action,int value){
    error_text.append(filename+" : "+action+" : "+nc_strerror(value)+" : "+QString::number(value)+"\n");

}

void Correlate::NetCDF_Error_Message(){
    QMessageBox msgBox;
            msgBox.setText("A NetCDF Error at :");
            if (error_text.size()>1000) error_text=error_text.left(1000)+"...";
            msgBox.setInformativeText(error_text);
            msgBox.setIcon(QMessageBox::Warning);
            QPushButton *okButton = msgBox.addButton("Ok",QMessageBox::ActionRole);
            QPushButton *abortButton = msgBox.addButton("Show no errors",QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == okButton) {

              } else if (msgBox.clickedButton() == abortButton) {
                  show_error=0;
              }

}

void Correlate::selectTarget(){
    QString file="";
    /*
    QString file = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources->path_PaleoDataView+"/"+resources->workdir+"/"+inv->access_proxy_name()+"/Derived Data/Targets",
                                             tr("Custom Target (*.tgt)"));*/
    TargetSelect *st=new TargetSelect(this,inv,proxy,worldmap);
    st->setModal(true);
    st->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    st->setAttribute( Qt::WA_DeleteOnClose );
    st->exec();
}

void Correlate::targetS(QString str){
    qDebug()<<"Filename :"+str;
    if (str!="") {
        ui->lineEdit->setText(str);
        update();
        // Read data from targetfile

        readTarget();
        ndata=ui->comboBox_2->currentIndex();
        createCorPlot(1);
        cor_plot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    } else {
        ui->lineEdit->setText("");
        update();
    }
}
