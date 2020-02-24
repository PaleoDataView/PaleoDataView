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

#include "ams.h"
#include "ui_ams.h"

AMS::AMS(QMainWindow *mainWindow,Inventory *inventory) :
    mainW(mainWindow),inv(inventory),
    ui(new Ui::AMS)
{
    inv->readData(inv->get_currentCore());
    resources=new Resources();
    ui->setupUi(this);
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);
    this->setWindowTitle("Radiocarbon Calibration for Core : "+inv->get_Core(inv->get_currentCore())+" : "+inv->get_Species(inv->get_currentCore())+" at "+QString::number(inv->get_Longitude(inv->get_currentCore()))+"° :"+QString::number(inv->get_Latitude(inv->get_currentCore()))+"° in "+QString::number(inv->get_Water_Depth(inv->get_currentCore()))+"m");
    select=0;
    // Read AgeModel
    amsdata=new AMSData(inv);
    amsdata->AMSRead();



    // create Age model plot
    amsplot_data=new float[0];

    data_use=new bool[0];
    data_error=new float[0];
    data_error2=new float[0];
    col=new QColor[0];
    mark=new int[0];
    comment=new QString[0];

    amsplot=new Graph(this,amsplot_data,0,0);

    createAMSPlot();


    // Read Calibration Data
    ui->label->setText(resources->get_filename_C14_Cal());
    amsdata->ReadCalib();
    // Plot Calib Data
    plot_data=new float[0];
    plot_col=new QColor[0];
    plot_style=new Qt::PenStyle[0];
    plot_width=new int[0];
    calib=new Graph(this,plot_data,0,0);
    createPlot();

    value_off=0;
    ui->label_2->setText("Dated Age : ");
    ui->label_3->setText("UCL : ");
    ui->label_4->setText("LCL : ");
    ui->label_6->setText("Reservoir Age : ");
    ui->label_7->setText("Calender Age (youngest mean): ");
    ui->label_8->setText("Calender UCL : ");
    ui->label_9->setText("Calender LCL : ");
    ui->label_10->setText("Calender Age (median) :");
    ui->label_11->setText("Calender Age (MidAge) :");
    ui->label_12->setText("Calender Age (weighted mean) :");
    ui->label_13->setText("Calender Upper Limit (95%) :");
    ui->label_14->setText("Calender Lower Limit (95%) :");
    edit=new AMSEdit(this,amsdata,0);
    attd=new attDialog(this,"","");

    //modelData = new QStandardItemModel(0,0,this);

    setupTable(0);

    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(selected(QModelIndex)));// entry selected
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(set_CalibData()));// Change Calib-File
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(reservoirSingle()));// Suggest Reservoir Age
    connect(ui->pushButton_11,SIGNAL(clicked()),this,SLOT(reservoirAll()));// Suggest Reservoir Age All
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(save_AMS()));// save AMS to File
    connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(applyAM()));// apply AMS and write to File
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(reload()));// Reload AMS
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(calibrate()));// Calibrate selected entry
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(calibrateAll()));// Calibrate All
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(NewEntry()));// Add new AMS Entry
    connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(DeleteEntry()));// Delete Entry
    connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(EditEntry()));// Edit Entry
    //connect(ui->pushButton_11,SIGNAL(clicked()),this,SLOT(RunBacon()));
    connect(amsplot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(calib,SIGNAL(graphChanged()),this,SLOT(updateGraph()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));

    update();
    changes=false;
}

AMS::~AMS()
{

    delete ui;
    delete resources;
    delete amsdata;
    delete amsplot;
    //delete calibplot;
    //delete modelData;
    delete amsplot_data;
    //delete[] amsplot_data;

    delete[] data_use;
    delete[] data_error;
    delete[] data_error2;
    delete[] col;
    delete[] mark;
    delete[] comment;
    delete[] plot_data;
    delete[] plot_col;
    delete[] plot_style;
    delete[] plot_width;
    delete edit;
    delete attd;
}

void AMS::paintEvent(QPaintEvent *)
{
    amsplot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    calib->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());

}

void AMS::setupTable(int column){
    // create the model for AMSData
    //delete modelData;
    modelData = new QStandardItemModel(amsdata->get_Length(),14,this);
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



    for (int i=0;i<amsdata->get_Length();i++){
        QStandardItem *var_Index=new QStandardItem(QString::number(i));
        modelData->setItem(i,0,var_Index);

        QStandardItem *var_Depth=new QStandardItem(QString::number(amsdata->get_Depth(i)));
        modelData->setItem(i,1,var_Depth);

        QStandardItem *var_Sample=new QStandardItem(QString::number(amsdata->get_Sample_Thickness(i)));
        modelData->setItem(i,2,var_Sample);

        QStandardItem *var_Label=new QStandardItem(amsdata->get_LabID(i));
        modelData->setItem(i,3,var_Label);

        QStandardItem *var_Type=new QStandardItem(amsdata->get_Type(i));
        modelData->setItem(i,4,var_Type);

        QStandardItem *var_Age_dated=new QStandardItem(QString::number(amsdata->get_Data(0,i)));
        modelData->setItem(i,5,var_Age_dated);

        QStandardItem *var_Age_UCL=new QStandardItem(QString::number(amsdata->get_Data(1,i)));
        modelData->setItem(i,6,var_Age_UCL);

        QStandardItem *var_Age_LCL=new QStandardItem(QString::number(amsdata->get_Data(2,i)));
        modelData->setItem(i,7,var_Age_LCL);

        QStandardItem *var_Age_Res=new QStandardItem(QString::number(amsdata->get_Data(3,i)));
        modelData->setItem(i,8,var_Age_Res);

        QStandardItem *var_Age_Res_Err=new QStandardItem(QString::number(amsdata->get_Reservoir_Error(i)));
        modelData->setItem(i,9,var_Age_Res_Err);

        QStandardItem *var_Cal=new QStandardItem(QString::number(amsdata->get_Data(4,i)));
        modelData->setItem(i,10,var_Cal);

        QStandardItem *var_Cal_Min=new QStandardItem(QString::number(amsdata->get_Data(5,i)));
        modelData->setItem(i,11,var_Cal_Min);

        QStandardItem *var_Cal_Max=new QStandardItem(QString::number(amsdata->get_Data(6,i)));
        modelData->setItem(i,12,var_Cal_Max);

        QStandardItem *var_Use_Flag=new QStandardItem(QString::number(amsdata->get_Data(7,i)));
        var_Use_Flag->setCheckable(true);
        if (amsdata->get_Data(7,i)) var_Use_Flag->setCheckState(Qt::Checked);
        modelData->setItem(i,13,var_Use_Flag);
        modelData->setData(modelData->index(i, 13), Qt::AlignCenter,Qt::TextAlignmentRole);

        QStandardItem *var_Comment=new QStandardItem(amsdata->get_Age_Comment(i));
        modelData->setItem(i,14,var_Comment);




        // coloring
        if (amsdata->get_Data(7,i)){
            modelData->setData(modelData->index(i, 13), QColor(Qt::green), Qt::BackgroundRole);
        } else {
            modelData->setData(modelData->index(i, 13), QColor(Qt::red), Qt::BackgroundRole);
        }


    }
    //ui->tableView->setSortingEnabled(0);
    //ui->tableView->horizontalHeader()->setSortIndicator(1,Qt::AscendingOrder);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->scrollTo(modelData->index(amsdata->getSelect(),column));
    ui->tableView->repaint();
}

void AMS::selected(QModelIndex mi){
        int sel=mi.row();
        //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        QString text = model->item(sel,mi.column())->text();
        //qDebug() << text;
        int index=model->item(sel,0)->text().toInt();

        if (mi.column()==13){
            if (amsdata->get_Data(7,sel)==1){
                amsdata->set_Data(7,sel,0);
            } else {
                if (!isnan(amsdata->get_Data(4,sel))) amsdata->set_Data(7,sel,1); else {
                    QMessageBox msgBox;
                    msgBox.setText("Core :"+inv->get_att_Core()+"\nSpecies :"+inv->get_att_Species()+"\nDepth :"+QString::number(amsdata->get_Depth(sel))+"\nNon-calibrated -> Use flags deactivated");
                    msgBox.setInformativeText("Please check the Agemodel data");
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    int ret = msgBox.exec();
                }
                checkReversals();
            }
            select=index;

            amsdata->setSelect(index);
            setupTable(13);
        }
        if (mi.column()==14){
            delete attd;
            attd=new attDialog(this,amsdata->get_Age_Comment(index),QString::number(index));
            attd->setModal(true);
            attd->show();
            select=index;

            amsdata->setSelect(index);
            setupTable(14);
        }
        if (mi.column()<13) {
            select=index;

            amsdata->setSelect(index);
            setupTable(0);
        }


        for (int i=0;i<15;i++)if (i!=13)modelData->setData(modelData->index(index,i), QColor(Qt::gray), Qt::BackgroundRole);
        select=index;
        // recreate Age model plot
        createAMSPlot();
        // show calib

        // if AMS do calculations for label and plot
        if (select>=0 && select<amsdata->get_Length() && amsdata->get_Type(select)=="AMS"){
            int index=select;
            value_off=0;
            float value=amsdata->get_Data(0,index)*1000-amsdata->get_Data(3,index)*1000;
            float error1=sqrt(amsdata->get_Data(1,index)*amsdata->get_Data(1,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
            float error2=sqrt(amsdata->get_Data(2,index)*amsdata->get_Data(2,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
            float valuep=error1*1000.0/1.0;
            float valuem=error2*1000.0/1.0;

            float pdf_max=amsdata->get_DPDF(value,valuep,value);// for scaling
            //float v=amsdata->get_DPDF(value,valuep,i)/pdf_max*(x_max-x_min)*0.2;// dated pdf values

            amsdata->calc_CalPDF(value,valuep);
            //float v=amsdata->get_CalPDF(i)/amsdata->get_CalPDF_max()*(y_max-y_min)*0.2;// cal pdf values

            amsdata->calc_intPDF();
            amsdata->sortInt();

            // Calender Age
            int c=0;
            for (c=0;c<amsdata->get_Calib_Length();c++) if (amsdata->get_dage(c)>value) break;
            float cvalue=amsdata->get_cage(c-1)+((amsdata->get_cage(c)-amsdata->get_cage(c-1))/(amsdata->get_dage(c)-amsdata->get_dage(c-1)))*(value-amsdata->get_dage(c-1));
            int cp=amsdata->get_Calib_Length()-1;
            for (cp=amsdata->get_Calib_Length()-1;cp>=0;cp--) if (amsdata->get_dage(cp)<(value+valuep*3)) break;
            float cvaluep=amsdata->get_cage(cp)+((amsdata->get_cage(cp+1)-amsdata->get_cage(cp))/(amsdata->get_dage(cp+1)-amsdata->get_dage(cp)))*((value+valuep*3)-amsdata->get_dage(cp));
            int cm=0;
            for (cm=0;cm<amsdata->get_Calib_Length();cm++) if (amsdata->get_dage(cm)>(value-valuem*3)) break;
            float cvaluem=amsdata->get_cage(cm-1)+((amsdata->get_cage(cm)-amsdata->get_cage(cm-1))/(amsdata->get_dage(cm)-amsdata->get_dage(cm-1)))*((value-valuem*3)-amsdata->get_dage(cm-1));

            ui->label_2->setText("Dated Age : "+QString::number(value));
            ui->label_3->setText("UCL : "+QString::number(valuep*3));
            ui->label_4->setText("LCL : "+QString::number(valuem*3));
            ui->label_6->setText("Reservoir Age : "+QString::number(amsdata->get_Data(3,index))+"+/-"+QString::number(amsdata->get_Reservoir_Error(index)));
            ui->label_7->setText("Calender Age (youngest mean): "+QString::number(cvalue));
            ui->label_8->setText("Calender UCL : "+QString::number(cvaluep));
            ui->label_9->setText("Calender LCL : "+QString::number(cvaluem));
            ui->label_10->setText("Calender Age (median) :"+QString::number(amsdata->get_CalMedian()));
            ui->label_11->setText("Calender Age (MidAge) :"+QString::number(amsdata->get_CalMidAge()));
            ui->label_12->setText("Calender Age (weighted mean) :"+QString::number(amsdata->get_CalWMean()));
            ui->label_13->setText("Calender Upper Limit (95%) :"+QString::number(amsdata->get_Cal95U()));
            ui->label_14->setText("Calender Lower Limit (95%) :"+QString::number(amsdata->get_Cal95L()));

            createPlot();
        }
        update();

}

void AMS::set_CalibData(){
    QString file = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources->get_path_target(),
                                             tr("C14 Calibration Data (*.14C)"));


        //qDebug() << "Selected :"+file;
        amsdata->setCalibFile(file);
        ui->label->setText(file);
        amsdata->ReadCalib();
        update();

}


void AMS::attChanged(QString text,QString origin){
        int sel=origin.toInt(0,10);
        //qDebug() << "Text:"+text;
        amsdata->set_Age_Comment(text,sel);
        //inv->saveData();
        setupTable(14);
        changes=true;
}

void AMS::refresh(){

    setupTable(0);
    if(select>=0){
    for (int i=0;i<15;i++)if (i!=13)modelData->setData(modelData->index(select,i), QColor(Qt::gray), Qt::BackgroundRole);

    // recreate Age model plot
    createAMSPlot();
    // show calib
    // if AMS do calculations for label and plot
    if (select>=0 && select<amsdata->get_Length() && amsdata->get_Type(select)=="AMS"){
        int index=select;
        value_off=0;
        float value=amsdata->get_Data(0,index)*1000-amsdata->get_Data(3,index)*1000;
        float error1=sqrt(amsdata->get_Data(1,index)*amsdata->get_Data(1,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
        float error2=sqrt(amsdata->get_Data(2,index)*amsdata->get_Data(2,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
        float valuep=error1*1000.0/1.0;
        float valuem=error2*1000.0/1.0;

        float pdf_max=amsdata->get_DPDF(value,valuep,value);// for scaling
        //float v=amsdata->get_DPDF(value,valuep,i)/pdf_max*(x_max-x_min)*0.2;// dated pdf values

        amsdata->calc_CalPDF(value,valuep);
        //float v=amsdata->get_CalPDF(i)/amsdata->get_CalPDF_max()*(y_max-y_min)*0.2;// cal pdf values

        amsdata->calc_intPDF();
        amsdata->sortInt();

        // Calender Age
        int c=0;
        for (c=0;c<amsdata->get_Calib_Length();c++) if (amsdata->get_dage(c)>value) break;
        float cvalue=amsdata->get_cage(c-1)+((amsdata->get_cage(c)-amsdata->get_cage(c-1))/(amsdata->get_dage(c)-amsdata->get_dage(c-1)))*(value-amsdata->get_dage(c-1));
        int cp=amsdata->get_Calib_Length()-1;
        for (cp=amsdata->get_Calib_Length()-1;cp>=0;cp--) if (amsdata->get_dage(cp)<(value+valuep*3)) break;
        float cvaluep=amsdata->get_cage(cp)+((amsdata->get_cage(cp+1)-amsdata->get_cage(cp))/(amsdata->get_dage(cp+1)-amsdata->get_dage(cp)))*((value+valuep*3)-amsdata->get_dage(cp));
        int cm=0;
        for (cm=0;cm<amsdata->get_Calib_Length();cm++) if (amsdata->get_dage(cm)>(value-valuem*3)) break;
        float cvaluem=amsdata->get_cage(cm-1)+((amsdata->get_cage(cm)-amsdata->get_cage(cm-1))/(amsdata->get_dage(cm)-amsdata->get_dage(cm-1)))*((value-valuem*3)-amsdata->get_dage(cm-1));

        ui->label_2->setText("Dated Age : "+QString::number(value));
        ui->label_3->setText("UCL : "+QString::number(valuep*3));
        ui->label_4->setText("LCL : "+QString::number(valuem*3));
        ui->label_6->setText("Reservoir Age : "+QString::number(amsdata->get_Data(3,index))+"+/-"+QString::number(amsdata->get_Reservoir_Error(index)));
        ui->label_7->setText("Calender Age (youngest mean): "+QString::number(cvalue));
        ui->label_8->setText("Calender UCL : "+QString::number(cvaluep));
        ui->label_9->setText("Calender LCL : "+QString::number(cvaluem));
        ui->label_10->setText("Calender Age (median) :"+QString::number(amsdata->get_CalMedian()));
        ui->label_11->setText("Calender Age (MidAge) :"+QString::number(amsdata->get_CalMidAge()));
        ui->label_12->setText("Calender Age (weighted mean) :"+QString::number(amsdata->get_CalWMean()));
        ui->label_13->setText("Calender Upper Limit (95%) :"+QString::number(amsdata->get_Cal95U()));
        ui->label_14->setText("Calender Lower Limit (95%) :"+QString::number(amsdata->get_Cal95L()));

        createPlot();
    }
    update();}
}

void AMS::save_AMS(){
    if (amsdata->get_Length()>0) amsdata->AMSSave();
    amsplot->autoSize();
    update();
    changes=false;
    QMessageBox msgBox;
            msgBox.setText("Agedata succesfully saved.");

            msgBox.exec();
}

void AMS::applyAM(){
    //qDebug()<<"Apply Model";
    //amdata->apply_Age_Model("linear");
    for (int i=0;i<inv->get_Length();i++){
        double v=amsdata->get_Int_Value_Age(inv->get_data_Isotopes(0,i),"linear");
        inv->set_data_Isotopes(1,i,v);
    }
    inv->saveData();
    QMessageBox msgBox;
            msgBox.setText("Agedata added to proxy.");
            msgBox.setInformativeText("This will enable plotting vs. age in some tools.");


            msgBox.exec();
}


void AMS::reload(){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "PaleoDataView",
                                                                tr("Do you really want to reload from the proxy file? All changes will be lost!"),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {


    } else {
    delete amsdata;
    amsdata=new AMSData(inv);
    amsdata->AMSRead();
    setupTable(0);


    // create Age model plot
    createAMSPlot();

    // Read Calibration Data
    ui->label->setText(resources->get_filename_C14_Cal());
    amsdata->ReadCalib();
    // if AMS do calculations for label and plot
    if (select>=0 && select<amsdata->get_Length() && amsdata->get_Type(select)=="AMS"){
        int index=select;

        value_off=0;
        float value=amsdata->get_Data(0,index)*1000-amsdata->get_Data(3,index)*1000;
        float error1=sqrt(amsdata->get_Data(1,index)*amsdata->get_Data(1,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
        float error2=sqrt(amsdata->get_Data(2,index)*amsdata->get_Data(2,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
        float valuep=error1*1000.0/1.0;
        float valuem=error2*1000.0/1.0;

        float pdf_max=amsdata->get_DPDF(value,valuep,value);// for scaling
        //float v=amsdata->get_DPDF(value,valuep,i)/pdf_max*(x_max-x_min)*0.2;// dated pdf values

        amsdata->calc_CalPDF(value,valuep);
        //float v=amsdata->get_CalPDF(i)/amsdata->get_CalPDF_max()*(y_max-y_min)*0.2;// cal pdf values

        amsdata->calc_intPDF();
        amsdata->sortInt();

        // Calender Age
        int c=0;
        for (c=0;c<amsdata->get_Calib_Length();c++) if (amsdata->get_dage(c)>value) break;
        float cvalue=amsdata->get_cage(c-1)+((amsdata->get_cage(c)-amsdata->get_cage(c-1))/(amsdata->get_dage(c)-amsdata->get_dage(c-1)))*(value-amsdata->get_dage(c-1));
        int cp=amsdata->get_Calib_Length()-1;
        for (cp=amsdata->get_Calib_Length()-1;cp>=0;cp--) if (amsdata->get_dage(cp)<(value+valuep*3)) break;
        float cvaluep=amsdata->get_cage(cp)+((amsdata->get_cage(cp+1)-amsdata->get_cage(cp))/(amsdata->get_dage(cp+1)-amsdata->get_dage(cp)))*((value+valuep*3)-amsdata->get_dage(cp));
        int cm=0;
        for (cm=0;cm<amsdata->get_Calib_Length();cm++) if (amsdata->get_dage(cm)>(value-valuem*3)) break;
        float cvaluem=amsdata->get_cage(cm-1)+((amsdata->get_cage(cm)-amsdata->get_cage(cm-1))/(amsdata->get_dage(cm)-amsdata->get_dage(cm-1)))*((value-valuem*3)-amsdata->get_dage(cm-1));

        ui->label_2->setText("Dated Age : "+QString::number(value));
        ui->label_3->setText("UCL : "+QString::number(valuep*3));
        ui->label_4->setText("LCL : "+QString::number(valuem*3));
        ui->label_6->setText("Reservoir Age : "+QString::number(amsdata->get_Data(3,index))+"+/-"+QString::number(amsdata->get_Reservoir_Error(index)));
        ui->label_7->setText("Calender Age (youngest mean): "+QString::number(cvalue));
        ui->label_8->setText("Calender UCL : "+QString::number(cvaluep));
        ui->label_9->setText("Calender LCL : "+QString::number(cvaluem));
        ui->label_10->setText("Calender Age (median) :"+QString::number(amsdata->get_CalMedian()));
        ui->label_11->setText("Calender Age (MidAge) :"+QString::number(amsdata->get_CalMidAge()));
        ui->label_12->setText("Calender Age (weighted mean) :"+QString::number(amsdata->get_CalWMean()));
        ui->label_13->setText("Calender Upper Limit (95%) :"+QString::number(amsdata->get_Cal95U()));
        ui->label_14->setText("Calender Lower Limit (95%) :"+QString::number(amsdata->get_Cal95L()));

        createPlot();
    }
    update();
    changes=false;
    }
}

void AMS::calibrate(){
    if (select!=-1 &&  amsdata->get_Type(select)=="AMS") {
        if (amsdata->get_Data(0,select)>0 && amsdata->get_Data(1,select)>0 && amsdata->get_Data(2,select)>0){
            if (amsdata->get_Data(3,select)>=0 && amsdata->get_Reservoir_Error(select)>=0){
                amsdata->set_Data(4,select,amsdata->get_CalWMean()/1000.0);
                amsdata->set_Data(5,select,amsdata->get_Cal95L()/1000.0);
                amsdata->set_Data(6,select,amsdata->get_Cal95U()/1000.0);
                amsdata->set_Data(7,select,1);
                checkReversals();
                setupTable(0);
                changes=true;
                // create Age model plot
                createAMSPlot();
                update();
            }else {
                QMessageBox msgBox;
                        msgBox.setText("The Reservoir Age is unknown.");
                        msgBox.setInformativeText("This operation requires the reservoir age. Simply press 'Reservoir Age'/'Edit' Button to generate/enter a valid value.");
                        msgBox.setIcon(QMessageBox::Warning);

                        msgBox.exec();
            }
        } else {
            QMessageBox msgBox;
                    msgBox.setText("The dated values are incomplete.");
                    msgBox.setInformativeText("This operation requires the dated mean and the dated error range.");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
        }
    }

}

void AMS::calibrateAll(){
    int error=0;
    for (int i=0;i<amsdata->get_Length();i++){
        if ( amsdata->get_Type(i)=="AMS") {
            if (amsdata->get_Data(0,select)>0 && amsdata->get_Data(1,select)>0 && amsdata->get_Data(2,select)>0){
                if (amsdata->get_Data(3,select)>=0 && amsdata->get_Reservoir_Error(select)>=0){
                    float error=sqrt(amsdata->get_Data(1,i)*amsdata->get_Data(1,i)+amsdata->get_Reservoir_Error(i)*amsdata->get_Reservoir_Error(i));
                    amsdata->calc_CalPDF(amsdata->get_Data(0,i)*1000-amsdata->get_Data(3,i)*1000,error*1000.0/1.0);
                    amsdata->calc_intPDF();
                    amsdata->sortInt();
                    amsdata->set_Data(4,i,amsdata->get_CalWMean()/1000.0);
                    amsdata->set_Data(5,i,amsdata->get_Cal95L()/1000.0);
                    amsdata->set_Data(6,i,amsdata->get_Cal95U()/1000.0);
                    amsdata->set_Data(7,i,1);
                    checkReversals();
                    setupTable(0);
                    changes=true;
                    // create Age model plot
                    createAMSPlot();
                    update();
                } else {
                    error=2;
                }
            }else {
                error=1;
            }
        }
    }

    if(error==1){
        QMessageBox msgBox;
            msgBox.setText("The dated values are incomplete.");
            msgBox.setInformativeText("This operation requires the dated mean and the dated error range.");
            msgBox.setIcon(QMessageBox::Warning);

            msgBox.exec();
    }
    if(error==2){
        QMessageBox msgBox;
                msgBox.setText("The Reservoir Age is unknown in some cases.");
                msgBox.setInformativeText("This operation requires the reservoir age. Simply press 'Reservoir Age'/'Edit' Button to generate/enter a valid value.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }

    select=-1;

}

void AMS::NewEntry(){
    amsdata->addEntry();
    delete edit;
    edit=new AMSEdit(this,amsdata,0);
    edit->setModal(true);
    edit->show();
    changes=true;
}


void AMS::EditEntry(){
    if (select>=0 && amsdata->get_Length()>select){
        delete edit;
        edit=new AMSEdit(this,amsdata,select);
        edit->setModal(true);
        edit->show();
    }
    changes=true;
}

void AMS::DeleteEntry(){
    if (amsdata->get_Length()>0){
        if (amsdata->get_Length()==1) {

            amsdata->set_Depth(select,NAN);
            amsdata->set_Sample_Thickness(select,NAN);
            amsdata->set_LabID(select,"Unknown");
            amsdata->set_Type(select,"Unknown");
            amsdata->set_Age_Comment("default Entry: Please Edit",select);
            for (int i=0;i<7;i++) amsdata->set_Data(i,select,NAN);
            amsdata->set_Reservoir_Error(select,NAN);
            amsdata->set_Data(7,select,0);
        }
        if (amsdata->get_Length()>1) amsdata->deleteEntry(select);

        if (amsdata->get_Length()>0){
            select=0;
        } else {
            select=-1;
        }

        refresh();
    }
    changes=true;
}

void AMS::createAMSPlot(){
    delete[] amsplot_data;
    delete[] data_use;
    delete[] data_error;
    delete[] data_error2;
    delete[] col;
    delete[] mark;
    delete[] comment;
    amsplot_data=new float[amsdata->get_Length()*4];
    data_use=new bool[amsdata->get_Length()*4];
    data_error=new float[amsdata->get_Length()*4];
    data_error2=new float[amsdata->get_Length()*4];
    col=new QColor[amsdata->get_Length()*4];
    mark=new int[amsdata->get_Length()*4];
    comment=new QString[amsdata->get_Length()*4];
    for (int i=0;i<amsdata->get_Length();i++){
        amsplot_data[i+0*amsdata->get_Length()]=amsdata->get_Depth(i);
        amsplot_data[i+1*amsdata->get_Length()]=amsdata->get_Data(0,i);
        amsplot_data[i+2*amsdata->get_Length()]=amsdata->get_Depth(i);
        amsplot_data[i+3*amsdata->get_Length()]=amsdata->get_Data(4,i);
        data_use[i+0*amsdata->get_Length()]=amsdata->get_Data(7,i);
        data_use[i+1*amsdata->get_Length()]=amsdata->get_Data(7,i);
        data_use[i+2*amsdata->get_Length()]=amsdata->get_Data(7,i);
        data_use[i+3*amsdata->get_Length()]=amsdata->get_Data(7,i);
        data_error[i+0*amsdata->get_Length()]=NAN;
        data_error[i+1*amsdata->get_Length()]=amsdata->get_Data(2,i);
        data_error[i+2*amsdata->get_Length()]=NAN;
        //data_error[i+3*amsdata->get_Length()]=abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i));
        if (amsdata->get_Data(5,i)!=0) data_error[i+3*amsdata->get_Length()]=abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)); else data_error[i+3*amsdata->get_Length()]=NAN;

        data_error2[i+0*amsdata->get_Length()]=NAN;
        data_error2[i+1*amsdata->get_Length()]=amsdata->get_Data(1,i);
        data_error2[i+2*amsdata->get_Length()]=NAN;
        //data_error2[i+3*amsdata->get_Length()]=abs(amsdata->get_Data(4,i)-amsdata->get_Data(6,i));
        if (amsdata->get_Data(6,i)!=0) data_error2[i+3*amsdata->get_Length()]=abs(amsdata->get_Data(4,i)-amsdata->get_Data(6,i)); else data_error2[i+3*amsdata->get_Length()]=NAN;

        col[i+0*amsdata->get_Length()]=QColor(0,0,0);
        col[i+1*amsdata->get_Length()]=QColor(255,255,0);
        col[i+2*amsdata->get_Length()]=QColor(0,0,0);
        col[i+3*amsdata->get_Length()]=QColor(0,0,255);

        //if (amdata->get_Age_Comment(i).simplified()!="NaN") comment[i+2*amdata->get_Length()]=amdata->get_Age_Comment(i);
        mark[i+0*amsdata->get_Length()]=0;
        mark[i+1*amsdata->get_Length()]=0;
        mark[i+2*amsdata->get_Length()]=0;
        mark[i+3*amsdata->get_Length()]=0;
    }
    mark[select+0*amsdata->get_Length()]=1;
    mark[select+1*amsdata->get_Length()]=1;
    mark[select+2*amsdata->get_Length()]=1;
    mark[select+3*amsdata->get_Length()]=1;
    amsplot->setData(amsplot_data,4,amsdata->get_Length());
    amsplot->setUse(data_use,1);
    amsplot->setError(data_error,2);
    amsplot->setError2(data_error2,2);
    amsplot->setColor(col,1);
    amsplot->setMark(mark,1);
    //am_plot->setComment(comment,1);
    amsplot->setTitel("","Depth [m]","Age [ka BP]");
    amsplot->setMultiplicator(1,1);
    amsplot->setTextSize(resources->get_Pointsize(),1,12);
    amsplot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    amsplot->setSymbol(3);
    amsplot->autoSize();
    amsplot->setSettings(resources->get_path_PaleoDataView()+"/Resources/Resources/Plot/AMS_AgeModel.txt",0);
    ui->graphicsView->setScene(amsplot);
    ui->graphicsView->repaint();
}



bool AMS::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (obj==ui->graphicsView||obj==ui->tableView||obj==ui->graphicsView_3){

            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);
                ui->splitter_2->restoreState(sp_2);
                ui->splitter_3->restoreState(sp_3);
                return true;
            }
        }

        if (keyEvent->key()==Qt::Key_Up){

            value_off=value_off+0.01;

            createPlot();
            return true;
        }
        if (keyEvent->key()==Qt::Key_Down){

            value_off=value_off-0.01;

            createPlot();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void AMS::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    if (changes) {
        resBtn = QMessageBox::question( this,"PaleoDataView - AMS",
                                        tr("There are unsaved changes.\nAre you sure?\n"),
                                        QMessageBox::Cancel | QMessageBox::Yes,
                                        QMessageBox::Yes);
    }
    if (resBtn == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void AMS::checkReversals(){
    for(int i=0;i<amsdata->get_Length();i++){
        int ok=1;
        if(amsdata->get_Data(7,i)==1){
            // check previous
            if(i>0){
                for (int j=i-1;j>=0;j--){
                    if (amsdata->get_Data(7,j)==1){
                        if (amsdata->get_Data(4,i)<amsdata->get_Data(4,j)) ok=0;
                    }
                }
            }
            // check followers
            if(i<amsdata->get_Length()-1){
                for(int j=i+1;j<amsdata->get_Length();j++){
                    if (amsdata->get_Data(7,j)==1){
                        if (amsdata->get_Data(4,i)>amsdata->get_Data(4,j)) ok=0;
                    }
                }

            }
            // check previous ages
            if(i>0){
                for (int j=i-1;j>=0;j--){
                    if (amsdata->get_Data(7,j)==1){
                        if (amsdata->get_Depth(i)<=amsdata->get_Depth(j)) ok=0;
                    }
                }
            }
            // check followers ages
            if(i<amsdata->get_Length()-1){
                for(int j=i+1;j<amsdata->get_Length();j++){
                    if (amsdata->get_Data(7,j)==1){
                        if (amsdata->get_Depth(i)>=amsdata->get_Depth(j)) ok=0;
                    }
                }

            }
            if (ok==0){
                QMessageBox msgBox;
                msgBox.setText("Core :"+inv->get_att_Core()+"\nSpecies :"+inv->get_att_Species()+"\nDepth :"+QString::number(amsdata->get_Depth(i))+"\nReversal found -> Use flags deactivated");
                msgBox.setInformativeText("Please check the Agemodel data");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                int ret = msgBox.exec();
                amsdata->set_Data(7,i,0);
            }
        }
    }
}

void AMS::createPlot(){
    delete[] plot_data;
    delete[] plot_col;
    delete[] plot_style;
    delete[] plot_width;


    plot_col=new QColor[22];
    for (int i=0;i<22;i++) plot_col[i]=QColor(0,0,0);
    plot_style=new Qt::PenStyle[22];
    for (int i=0;i<22;i++) plot_style[i]=Qt::SolidLine;
    plot_width=new int[22];
    for (int i=0;i<22;i++) plot_width[i]=1;


    plot_data=new float[amsdata->get_Calib_Length()*22];
    for (int i=0;i<amsdata->get_Calib_Length()*22;i++) plot_data[i]=NAN;

    for (int i=0;i<amsdata->get_Calib_Length();i++){
        plot_data[i+0*amsdata->get_Calib_Length()]=amsdata->get_cage(i);
        plot_data[i+1*amsdata->get_Calib_Length()]=amsdata->get_dage(i);
        plot_data[i+2*amsdata->get_Calib_Length()]=amsdata->get_cage(i);
        plot_data[i+3*amsdata->get_Calib_Length()]=amsdata->get_dage(i)+amsdata->get_uage(i);
        plot_data[i+4*amsdata->get_Calib_Length()]=amsdata->get_cage(i);
        plot_data[i+5*amsdata->get_Calib_Length()]=amsdata->get_dage(i)-amsdata->get_uage(i);

    }

    calib->setData(plot_data,22,amsdata->get_Calib_Length());

    plot_col[0]=QColor(0,0,0);
    plot_col[1]=QColor(0,0,0);
    plot_col[2]=QColor(128,128,128);
    plot_col[3]=QColor(128,128,128);
    plot_col[4]=QColor(128,128,128);
    plot_col[5]=QColor(128,128,128);
    plot_col[6]=QColor(0,0,255);
    plot_col[7]=QColor(0,0,255);
    plot_col[8]=QColor(0,0,255);
    plot_col[9]=QColor(0,0,255);
    plot_col[10]=QColor(255,0,0);
    plot_col[11]=QColor(255,0,0);
    plot_col[12]=QColor(255,0,0);
    plot_col[13]=QColor(255,0,0);
    plot_col[14]=QColor(255,0,0);
    plot_col[15]=QColor(0,255,0);
    plot_col[16]=QColor(0,255,0);
    plot_col[17]=QColor(0,255,0);
    plot_col[18]=QColor(0,255,0);
    plot_col[19]=QColor(0,255,0);
    plot_col[20]=QColor(0,255,0);
    plot_col[21]=QColor(0,255,0);


    calib->setSetLineColor(plot_col,1);

    plot_width[0]=3;
    plot_width[1]=3;
    plot_width[2]=2;
    plot_width[3]=2;
    plot_width[4]=2;
    plot_width[5]=2;
    plot_width[6]=2;
    plot_width[7]=2;
    plot_width[8]=2;
    plot_width[9]=2;
    plot_width[10]=3;
    plot_width[11]=3;
    plot_width[12]=3;
    plot_width[13]=3;
    plot_width[14]=3;
    plot_width[15]=3;
    plot_width[16]=3;
    plot_width[17]=3;
    plot_width[18]=3;
    plot_width[19]=3;
    plot_width[20]=3;
    plot_width[21]=3;


    calib->setSetLineWidth(plot_width,1);

    plot_style[0]=Qt::SolidLine;
    plot_style[1]=Qt::SolidLine;
    plot_style[2]=Qt::DotLine;
    plot_style[3]=Qt::DotLine;
    plot_style[4]=Qt::DotLine;
    plot_style[5]=Qt::DotLine;
    plot_style[6]=Qt::SolidLine;
    plot_style[7]=Qt::SolidLine;
    plot_style[8]=Qt::SolidLine;
    plot_style[9]=Qt::SolidLine;
    plot_style[10]=Qt::DashLine;
    plot_style[11]=Qt::DashLine;
    plot_style[12]=Qt::DashLine;
    plot_style[13]=Qt::DashLine;
    plot_style[14]=Qt::DashLine;
    plot_style[15]=Qt::DashLine;
    plot_style[16]=Qt::DashLine;
    plot_style[17]=Qt::DashLine;
    plot_style[18]=Qt::DashLine;
    plot_style[19]=Qt::DashLine;
    plot_style[20]=Qt::DashLine;
    plot_style[21]=Qt::DashLine;



    calib->setSetLineStyle(plot_style,1);

    calib->setTitel("","Calender Age [a]","Dated Age [a]");
    calib->setMultiplicator(1,1);
    calib->setTextSize(resources->get_Pointsize(),1,12);

    calib->setSymbol(0);
    //calib->autoSize();
    // set View for currently selected line
    // if AMS do calculations for label and plot
    if (select>=0 && select<amsdata->get_Length() && amsdata->get_Type(select)=="AMS"){

        int index=select;
        float value=(amsdata->get_Data(0,index)+value_off)*1000-amsdata->get_Data(3,index)*1000;
        float error1=sqrt(amsdata->get_Data(1,index)*amsdata->get_Data(1,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
        float error2=sqrt(amsdata->get_Data(2,index)*amsdata->get_Data(2,index)+amsdata->get_Reservoir_Error(index)*amsdata->get_Reservoir_Error(index));
        float valuep=error1*1000.0/1.0;
        float valuem=error2*1000.0/1.0;
        // Check for valid values
        if (value-valuem*5>0 && value+valuep*5<amsdata->get_dage(amsdata->get_Calib_Length()-1)) {
            float pdf_max=amsdata->get_DPDF(value,valuep,value);// for scaling


            amsdata->calc_CalPDF(value,valuep);


            amsdata->calc_intPDF();
            amsdata->sortInt();




            float y_min=value-valuem*5;
            float y_max=value+valuep*5;
            // estimate x_min for vm
            int count=0;

            float x_min=99999999999999.0;
            float x_max=0.0;
            for (int i=0;i<amsdata->get_Calib_Length();i++){
                if (amsdata->get_dage(i)+valuep>=y_min && amsdata->get_dage(i)-valuem<=y_max){
                    if (amsdata->get_cage(i)<x_min) x_min=amsdata->get_cage(i);//amsdata->get_cage(i-1)+((amsdata->get_cage(i)-amsdata->get_cage(i-1))/(amsdata->get_dage(i)-amsdata->get_dage(i-1)))*(y_min-amsdata->get_dage(i-1));
                }
                if (amsdata->get_dage(i)+valuep>=y_min && amsdata->get_dage(i)-valuem<=y_max){
                    if (amsdata->get_cage(i)>x_max) x_max=amsdata->get_cage(i);//amsdata->get_cage(i-1)+((amsdata->get_cage(i)-amsdata->get_cage(i-1))/(amsdata->get_dage(i)-amsdata->get_dage(i-1)))*(y_max-amsdata->get_dage(i-1));
                }

            }

            // estimate x_max for vp
            x_min-=(x_max-x_min)*0.01;
            x_max+=(x_max-x_min)*0.01;
            y_min-=(y_max-y_min)*0.01;
            y_max+=(y_max-y_min)*0.01;
            if ((x_max-x_min)==0){
                x_max++;
                x_min--;
            }
            if ((y_max-y_min)==0){
                y_max++;
                y_min--;
            }
            //qDebug()<<"Range: "+QString::number(x_min)+":"+QString::number(x_max);
            calib->setView(x_min,x_max,y_min,y_max);
            calib->setLimit(x_min,x_max,y_min,y_max);

            // add DPDF
            for (int i=0;i<amsdata->get_Calib_Length();i++){
                float y=y_min+i*((y_max-y_min)/amsdata->get_Calib_Length());
                float v=amsdata->get_DPDF(value,valuep,y)/pdf_max*(x_max-x_min)*0.2;// dated pdf values
                plot_data[i+6*amsdata->get_Calib_Length()]=x_min+v;
                plot_data[i+7*amsdata->get_Calib_Length()]=y;
            }
            // add CPDF
            for (int i=0;i<amsdata->get_Calib_Length();i++){
                float x=amsdata->get_cage(i);
                float v=amsdata->get_CalPDF(i)/amsdata->get_CalPDF_max()*(y_max-y_min)*0.2;// cal pdf values
                plot_data[i+8*amsdata->get_Calib_Length()]=x;
                plot_data[i+9*amsdata->get_Calib_Length()]=y_min+v;
            }

            // Calender Age
            int c=0;
            for (c=0;c<amsdata->get_Calib_Length();c++) if (amsdata->get_dage(c)>value) break;
            float cvalue=amsdata->get_cage(c-1)+((amsdata->get_cage(c)-amsdata->get_cage(c-1))/(amsdata->get_dage(c)-amsdata->get_dage(c-1)))*(value-amsdata->get_dage(c-1));
            int cp=amsdata->get_Calib_Length()-1;
            for (cp=amsdata->get_Calib_Length()-1;cp>=0;cp--) if (amsdata->get_dage(cp)<(value+valuep*3)) break;
            float cvaluep=amsdata->get_cage(cp)+((amsdata->get_cage(cp+1)-amsdata->get_cage(cp))/(amsdata->get_dage(cp+1)-amsdata->get_dage(cp)))*((value+valuep*3)-amsdata->get_dage(cp));
            int cm=0;
            for (cm=0;cm<amsdata->get_Calib_Length();cm++) if (amsdata->get_dage(cm)>(value-valuem*3)) break;
            float cvaluem=amsdata->get_cage(cm-1)+((amsdata->get_cage(cm)-amsdata->get_cage(cm-1))/(amsdata->get_dage(cm)-amsdata->get_dage(cm-1)))*((value-valuem*3)-amsdata->get_dage(cm-1));

            // add lines
            plot_data[0+10*amsdata->get_Calib_Length()]=x_min;
            plot_data[0+11*amsdata->get_Calib_Length()]=value;
            plot_data[1+10*amsdata->get_Calib_Length()]=cvalue;
            plot_data[1+11*amsdata->get_Calib_Length()]=value;
            plot_data[2+10*amsdata->get_Calib_Length()]=cvalue;
            plot_data[2+11*amsdata->get_Calib_Length()]=y_min;

            plot_data[0+12*amsdata->get_Calib_Length()]=x_min;
            plot_data[0+13*amsdata->get_Calib_Length()]=value-valuem*3;
            plot_data[1+12*amsdata->get_Calib_Length()]=cvaluem;
            plot_data[1+13*amsdata->get_Calib_Length()]=value-valuem*3;
            plot_data[2+12*amsdata->get_Calib_Length()]=cvaluem;
            plot_data[2+13*amsdata->get_Calib_Length()]=y_min;

            plot_data[0+14*amsdata->get_Calib_Length()]=x_min;
            plot_data[0+15*amsdata->get_Calib_Length()]=value+valuep*3;
            plot_data[1+14*amsdata->get_Calib_Length()]=cvaluep;
            plot_data[1+15*amsdata->get_Calib_Length()]=value+valuep*3;
            plot_data[2+14*amsdata->get_Calib_Length()]=cvaluep;
            plot_data[2+15*amsdata->get_Calib_Length()]=y_min;

            plot_data[0+16*amsdata->get_Calib_Length()]=amsdata->get_CalWMean();
            plot_data[0+17*amsdata->get_Calib_Length()]=y_min;
            plot_data[1+16*amsdata->get_Calib_Length()]=amsdata->get_CalWMean();
            plot_data[1+17*amsdata->get_Calib_Length()]=y_min+(y_max-y_min)*0.2;

            plot_data[0+18*amsdata->get_Calib_Length()]=amsdata->get_Cal95L();
            plot_data[0+19*amsdata->get_Calib_Length()]=y_min;
            plot_data[1+18*amsdata->get_Calib_Length()]=amsdata->get_Cal95L();
            plot_data[1+19*amsdata->get_Calib_Length()]=y_min+(y_max-y_min)*0.2;

            plot_data[0+20*amsdata->get_Calib_Length()]=amsdata->get_Cal95U();
            plot_data[0+21*amsdata->get_Calib_Length()]=y_min;
            plot_data[1+20*amsdata->get_Calib_Length()]=amsdata->get_Cal95U();
            plot_data[1+21*amsdata->get_Calib_Length()]=y_min+(y_max-y_min)*0.2;

            calib->setData(plot_data,22,amsdata->get_Calib_Length());
        } else {
            calib->autoSize();
        }



    } else {
        calib->autoSize();
    }
    calib->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    calib->setSettings(resources->get_path_PaleoDataView()+"/Resources/Resources/Plot/Calib.txt",0);
    ui->graphicsView_3->setScene(calib);
    ui->graphicsView_3->repaint();
}

void AMS::updateGraph(){

    createAMSPlot();
    createPlot();
}

void AMS::copy(QPoint pos){

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

void AMS::reservoir(int mode){


    // read Data
    float *longitude;
    int longitude_length;
    float *latitude;
    int latitude_length;
    float *time;
    int time_length;
    short *MRA_mean;
    //short *MRA_min;
    short *MRA_max;
    int repeatMessage=0;// Flag for avoiding multiple Messages

    // get file name
    QString QFilename=resources->get_path_PaleoDataView()+"/Resources/Calibration/Reservoir_Age.nc";

    char* filename;
    std::string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;


    // Initialize ids and meta
    int ncid;
    int varid;
    size_t dimlen[3]={0};
    int retval;// for Errors

    // Open the file
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // ******** read longitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lon", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    longitude_length=dimlen[0];

    // read data
    longitude=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&longitude[0]))) ERR(retval);

    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    latitude_length=dimlen[0];

    // read data
    latitude=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&latitude[0]))) ERR(retval);

    // ******** read times
    // get varid
    if ((retval = nc_inq_varid(ncid, "time", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    time_length=dimlen[0];

    // read data
    time=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&time[0]))) ERR(retval);

    // ********  read mean MRA
    // get varid
    // Get dimensions
    dimlen[0]=time_length;
    dimlen[1]=longitude_length;
    dimlen[2]=latitude_length;

    if ((retval = nc_inq_varid(ncid, "MRA_avg", &varid)))
       ERR(retval);
    // read MRA_mean
    MRA_mean=new short[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_short(ncid,varid,&MRA_mean[0]))) ERR(retval);

    //if ((retval = nc_inq_varid(ncid, "MRA_min", &varid)))
    //   ERR(retval);
    // read MRA_min
    //MRA_min=new short[dimlen[0]*dimlen[1]*dimlen[2]];
    //if ((retval = nc_get_var_short(ncid,varid,&MRA_min[0]))) ERR(retval);

    if ((retval = nc_inq_varid(ncid, "MRA_max", &varid)))
       ERR(retval);
    // read MRA_max
    MRA_max=new short[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_short(ncid,varid,&MRA_max[0]))) ERR(retval);


    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
    //delete[] dimlen;

    // transfer calender time-vector to c14 times without correcting reservoir age
    amsdata->ReadCalib();
    float *time_c14=new float[time_length];
    float *time_c14_err=new float[time_length];
    for (int i=0;i<time_length;i++) {
        time_c14[i]=0;
        time_c14_err[i]=0;
        for (int j=1;j<amsdata->get_Calib_Length();j++){
            if (time[i]<=amsdata->get_cage(j)){
                //qDebug()<<QString::number(j);
                time_c14[i]=amsdata->get_dage(j-1)+((amsdata->get_dage(j)-amsdata->get_dage(j-1))/(amsdata->get_cage(j)-amsdata->get_cage(j-1)))*(time[i]-amsdata->get_cage(j-1));

                time_c14_err[i]=amsdata->get_uage(j-1)+((amsdata->get_uage(j)-amsdata->get_uage(j-1))/(amsdata->get_cage(j)-amsdata->get_cage(j-1)))*(time[i]-amsdata->get_cage(j-1));
                j=amsdata->get_Calib_Length();
            }
        }
    }
    time_c14[0]=0;// can not be interpolated because there is no data
    time_c14_err[0]=0;
    // initialize
    int s=select;
    int start=0;
    int end=0;
    if (mode==0) {
        start=select;
        end=select+1;
    }
    if (mode==1){
        start=0;
        end=amsdata->get_Length();
    }
    // start calibrating
    for (int q=start;q<end;q++){
        qDebug()<<"Calibrating : "+QString::number(q);
        select=q;
        if (amsdata->get_Type(select)=="AMS" &&
                amsdata->get_Data(0,select)>0 && amsdata->get_Data(0,select)<400 &&
                amsdata->get_Data(1,select)>0 && amsdata->get_Data(1,select)<400 &&
                amsdata->get_Data(2,select)>0 && amsdata->get_Data(2,select)<400
                ){

        // find entry time
        int index_lon=-1;
        int index_lat=-1;
        int index_time=-1;
        float l=inv->get_att_Longitude();
        if (l<-5-1.25) l=360+l;
        l=l+6.25;
        index_lon=(int)(l/2.5);
        if (index_lon>=longitude_length && index_lon>=0) {
            index_lon=-1;
            qDebug()<<"index longitude out of range";
        }

        for (int i=0;i<latitude_length;i++) {
            if (latitude[i]<inv->get_att_Latitude()+1.25){
                index_lat=i;
                break;
            }
        }
        if (index_lat>=latitude_length && index_lat>=0) {
            index_lat=-1;
            qDebug()<<"index latitude out of range";
        }

        if (index_lon>=0 && index_lat>=0){
            for (int i=0;i<time_length;i++) {

                if (time_c14[i]+MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0>amsdata->get_Data(0,select)*1000.0){
                    index_time=i;
                    break;
                }
            }
        }
        if (index_time>=time_length && index_time>=0) {
            index_time=-1;
            qDebug()<<"index time out of range";
        }
        if (index_lat>-1 && index_lon>-1 && index_time>-1){// only go further if indizes were found
            qDebug()<<QString::number(index_lon)+" : "+QString::number(index_lat)+" : "+QString::number(index_time)+" : "+QString::number(MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]);
            int basin=inv->get_Basin(inv->get_att_Longitude(),inv->get_att_Latitude());

            // check if on land -> select neighbor if so
            int corrected=0;
            if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]<0) {
                qDebug()<<"On Land";

                /*

                //First order cells (4)
                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-1;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon--;
                        if (index_lon<0) index_lon+=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using left neighbor";
                    }
                }

                if (index_lon<longitude_length-1&& corrected==0){
                    int new_index_lon=index_lon+1;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon++;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using reight neighbor";
                    }
                }

                if (index_lat>0&& corrected==0){
                    int new_index_lat=index_lat-1;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat--;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using lower neighbor";
                    }
                }

                if (index_lat<latitude_length-1&& corrected==0){
                    int new_index_lat=index_lat+1;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat++;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using upper neighbor";
                    }
                }

                // First order diagonal cells (4)
                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-1;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    int new_index_lat=index_lat-1;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon--;
                        index_lat--;
                        if (index_lon<0) index_lon+=longitude_length;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using left-lower neighbor";
                    }
                }

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-1;
                    if (new_index_lon<0) new_index_lon=+longitude_length;
                    int new_index_lat=index_lat+1;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon--;
                        index_lat++;
                        if (index_lon<0) index_lon=+longitude_length;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using left-upper neighbor";
                    }
                }

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon+1;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    int new_index_lat=index_lat-1;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon++;
                        index_lat--;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using right-lower neighbor";
                    }
                }

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon+1;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    int new_index_lat=index_lat+1;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon++;
                        index_lat++;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using right-upper neighbor";
                    }
                }

                // Second order cells (4)

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-2;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon--;
                        index_lon--;
                        if (index_lon<0) index_lon+=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using left*2 neighbor";
                    }
                }

                if (index_lon<longitude_length-1&& corrected==0){
                    int new_index_lon=index_lon+2;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon++;
                        index_lon++;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using reight*2 neighbor";
                    }
                }

                if (index_lat>0&& corrected==0){
                    int new_index_lat=index_lat-2;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat--;
                        index_lat--;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using lower*2 neighbor";
                    }
                }

                if (index_lat<latitude_length-1&& corrected==0){
                    int new_index_lat=index_lat+2;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat++;
                        index_lat++;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using upper*2 neighbor";
                    }
                }
                */

                // search algorithem
                for (int order=1;order<8;order++){

                    for (int i=0;i<=order;i++){
                        // check all vertical
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon+order;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat+i;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon+order;
                                index_lat=index_lat+i;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                            }
                        }
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon+order;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat-i;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon+order;
                                index_lat=index_lat-i;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using "+QString::number(order)+": -"+QString::number(i);
                            }
                        }
                        // check all vertical
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon-order;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat+i;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon-order;
                                index_lat=index_lat+i;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using -"+QString::number(order)+":"+QString::number(i);
                            }
                        }
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon-order;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat-i;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon-order;
                                index_lat=index_lat-i;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using -"+QString::number(order)+": -"+QString::number(i);
                            }
                        }
                        // check all horizontal
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon+i;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat+order;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon+i;
                                index_lat=index_lat+order;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                            }
                        }
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon-i;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat+order;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon-i;
                                index_lat=index_lat+order;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                            }
                        }
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon+i;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat-order;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon+i;
                                index_lat=index_lat-order;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                            }
                        }
                        if (index_lon>0 && corrected==0){
                            int new_index_lon=index_lon-i;
                            if (new_index_lon<0) new_index_lon+=longitude_length;
                            int new_index_lat=index_lat-order;
                            if (new_index_lat<0) new_index_lat=0;
                            if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                    && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                index_lon=index_lon-i;
                                index_lat=index_lat-order;
                                if (index_lon<0) index_lon+=longitude_length;
                                if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                corrected=1;
                                qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                            }
                        }

                    }


                }






                if (corrected==1 && repeatMessage==0){
                    QMessageBox msgBox;
                        msgBox.setText("There are no values for the location.");
                        float dlon=inv->get_Longitude(inv->get_currentCore())-(longitude[index_lon]-360);
                        float dlat=inv->get_Latitude(inv->get_currentCore())-(latitude[index_lat]);
                        msgBox.setInformativeText("Instead values for location "+QString::number(longitude[index_lon]-360)+"/"+QString::number(latitude[index_lat])+" are used. The distance to this new location is :"+QString::number(-dlon)+"/"+QString::number(-dlat)+" [°]");
                        msgBox.setIcon(QMessageBox::Warning);

                        msgBox.exec();
                        repeatMessage=1;
                }



            }

            //if(index_lon>-1 && index_lat>-1 && index_time>-1){
            //    amsdata->set_Data(3,select,MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10000.0);
            //}

            //qDebug()<<QString::number(index_lon)+":"+QString::number(index_lat)+":"+QString::number(index_time);
            //qDebug()<<QString::number(longitude[index_lon])+":"+QString::number(latitude[index_lat])+":"+QString::number(time[index_time]);

            if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]>0){
                // create pdf of dated values
                float *pdf_years=new float[1000];
                float *pdf_p=new float[1000];
                float *pdf_ra=new float[1000];
                //float *pdf_ra_min=new float[1000];
                float *pdf_ra_max=new float[1000];

                float min=(amsdata->get_Data(0,select)-amsdata->get_Data(1,select)*6.0)*1000.0;
                float max=(amsdata->get_Data(0,select)+amsdata->get_Data(2,select)*6.0)*1000.0;

                float value=amsdata->get_Data(0,select)*1000.0;


                for (int i=0;i<1000;i++){
                    pdf_years[i]=min+i*((max-min)/1000.0);


                    // find entry longitude latitude time
                    int index_lon=-1;
                    int index_lat=-1;
                    int index_time=-1;
                    float l=inv->get_att_Longitude();
                    if (l<-5.0-1.25) l=360.0+l;
                    l=l+6.25;
                    index_lon=(int)(l/2.5);

                    for (int j=0;j<latitude_length;j++) {
                        if (latitude[j]<inv->get_att_Latitude()){
                            index_lat=j;
                            break;
                        }
                    }



                    for (int j=0;j<time_length;j++) {
                        if (time_c14[j]>pdf_years[i]){
                            index_time=j;
                            break;
                        }
                    }

                    if (index_lat>-1 && index_lon>-1 && index_time>0 && index_lat<latitude_length && index_lon<longitude_length && index_time<time_length){

                        // check if on land -> select neighbor if so
                        int corrected=0;
                        if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]<0) {
                            qDebug()<<"On Land";
/*
                            //First order cells (4)
                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-1;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon--;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left neighbor";
                                }
                            }

                            if (index_lon<longitude_length-1&& corrected==0){
                                int new_index_lon=index_lon+1;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon++;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using reight neighbor";
                                }
                            }

                            if (index_lat>0&& corrected==0){
                                int new_index_lat=index_lat-1;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat--;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using lower neighbor";
                                }
                            }

                            if (index_lat<latitude_length-1&& corrected==0){
                                int new_index_lat=index_lat+1;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat++;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using upper neighbor";
                                }
                            }

                            // First order diagonal cells (4)
                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-1;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                int new_index_lat=index_lat-1;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon--;
                                    index_lat--;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left-lower neighbor";
                                }
                            }

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-1;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                int new_index_lat=index_lat+1;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon--;
                                    index_lat++;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left-upper neighbor";
                                }
                            }

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon+1;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                int new_index_lat=index_lat-1;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon++;
                                    index_lat--;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using right-lower neighbor";
                                }
                            }

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon+1;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                int new_index_lat=index_lat+1;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon++;
                                    index_lat++;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using right-upper neighbor";
                                }
                            }

                            // Second order cells (4)

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-2;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon--;
                                    index_lon--;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left*2 neighbor";
                                }
                            }

                            if (index_lon<longitude_length-1&& corrected==0){
                                int new_index_lon=index_lon+2;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon++;
                                    index_lon++;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using reight*2 neighbor";
                                }
                            }

                            if (index_lat>0&& corrected==0){
                                int new_index_lat=index_lat-2;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat--;
                                    index_lat--;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using lower*2 neighbor";
                                }
                            }

                            if (index_lat<latitude_length-1&& corrected==0){
                                int new_index_lat=index_lat+2;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat++;
                                    index_lat++;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using upper*2 neighbor";
                                }
                            }*/

                            // search algorithem
                            for (int order=1;order<8;order++){

                                for (int i=0;i<=order;i++){

                                    // check all vertical
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon+order;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat+i;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon+order;
                                            index_lat=index_lat+i;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon+order;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat-i;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon+order;
                                            index_lat=index_lat-i;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    // check all vertical
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon-order;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat+i;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon-order;
                                            index_lat=index_lat+i;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon-order;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat-i;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon-order;
                                            index_lat=index_lat-i;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    // check all horizontal
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon+i;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat+order;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon+i;
                                            index_lat=index_lat+order;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon-i;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat+order;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon-i;
                                            index_lat=index_lat+order;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon+i;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat-order;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon+i;
                                            index_lat=index_lat-order;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }
                                    if (index_lon>0 && corrected==0){
                                        int new_index_lon=index_lon-i;
                                        if (new_index_lon<0) new_index_lon+=longitude_length;
                                        int new_index_lat=index_lat-order;
                                        if (new_index_lat<0) new_index_lat=0;
                                        if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                                && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                            index_lon=index_lon-i;
                                            index_lat=index_lat-order;
                                            if (index_lon<0) index_lon+=longitude_length;
                                            if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                            corrected=1;
                                            qDebug()<<"On Land: Using "+QString::number(order)+":"+QString::number(i);
                                        }
                                    }

                                }


                            }



                        }



                    // get values
                    if(MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]>0){
                        float MRA1=MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        float MRA2=MRA_mean[(index_time-1)*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        float mra=MRA2+(MRA1-MRA2)/(time_c14[index_time]-time_c14[index_time-1])*(pdf_years[i]-time_c14[index_time-1]);
                        pdf_ra[i]=mra;

                        //MRA1=MRA_min[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        //MRA2=MRA_min[(index_time-1)*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        //mra=MRA2+(MRA1-MRA2)/(time_c14[index_time]-time_c14[index_time-1])*(pdf_years[i]-time_c14[index_time-1]);
                        //if (time_c14[index_time]>11500)pdf_ra_min[i]=mra;else pdf_ra_min[i]=0.0;

                        MRA1=MRA_max[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        MRA2=MRA_max[(index_time-1)*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        mra=MRA2+(MRA1-MRA2)/(time_c14[index_time]-time_c14[index_time-1])*(pdf_years[i]-time_c14[index_time-1]);
                        if (time_c14[index_time]>11500)pdf_ra_max[i]=mra;else pdf_ra_max[i]=0.0;

                    }
                    else {
                        pdf_ra[i]=0.0;
                        //pdf_ra_min[i]=0.0;
                        pdf_ra_max[i]=0.0;
                    }

                    // Error weights and time error
                    float error1=sqrt(amsdata->get_Data(1,select)*amsdata->get_Data(1,select)+(time_c14_err[index_time]/1000.0)*(time_c14_err[index_time]/1000.0));
                    float valuep=error1*1000.0/1.0;
                    pdf_p[i]=amsdata->get_DPDF(value,valuep,pdf_years[i]);

                    } else {
                       pdf_p[i]=0.0;
                    }
                }

                // Calculate mean
                float sum_mean=0;
                float sum_w=0;
                float sum_std=0;
                for (int i=0;i<1000;i++){
                    sum_mean+=pdf_p[i]*pdf_ra[i];
                    if(pdf_ra[i]>0)sum_w+=pdf_p[i];
                }
                float mean=sum_mean/sum_w;
                //********************************
                for (int i=0;i<1000;i++){
                    sum_std+=pdf_p[i]*(pdf_ra[i]-mean)*(pdf_ra[i]-mean);
                }
                float std=sqrt(sum_std/((999.0/1000.0)*sum_w));

                /* Calculate mean_min
                sum_mean=0;
                sum_w=0;
                sum_std=0;
                for (int i=0;i<1000;i++){
                    sum_mean+=pdf_p[i]*pdf_ra_min[i];
                    if (pdf_ra_min[i]>0)sum_w+=pdf_p[i];
                }
                float mean_min=sum_mean/sum_w;
                //***********************************
                for (int i=0;i<1000;i++){
                    sum_std+=pdf_p[i]*(pdf_ra_min[i]-mean)*(pdf_ra_min[i]-mean);
                }
                float std_min=sqrt(sum_std/((999.0/1000.0)*sum_w));*/

                // Calculate mean_max
                sum_mean=0;
                sum_w=0;
                sum_std=0;
                for (int i=0;i<1000;i++){
                    sum_mean+=pdf_p[i]*pdf_ra_max[i];
                    if (pdf_ra_max[i]>0)sum_w+=pdf_p[i];
                }
                float mean_max=sum_mean/sum_w;
                //************************************
                for (int i=0;i<1000;i++){
                    sum_std+=pdf_p[i]*(pdf_ra_max[i]-mean)*(pdf_ra_max[i]-mean);
                }
                float std_max=sqrt(sum_std/((999.0/1000.0)*sum_w));

                amsdata->set_Data(3,select,round((mean/1000.0)*10000.0)/10000.0);
                //qDebug()<<QString::number((mean_max-mean)*2.0);

                if (mean_max-mean>1) amsdata->set_Reservoir_Error(select,round((
                                                                                   sqrt((std/1000.0)*(std/1000.0)
                                                                                   +(((mean_max-mean)*2.0)/4000.0)*(((mean_max-mean)*2.0)/4000.0))
                                                                                   )*10000.0)/10000.0);
                else amsdata->set_Reservoir_Error(select,round((
                                                                sqrt((std/1000.0)*(std/1000.0)
                                                                +((200.0)/4000.0)*((200.0)/4000.0))
                                                                )*10000.0)/10000.0);


                amsdata->set_Data(3,select,MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10000.0);
    /*
                float *ra=new float[4000];
                for (int i=0;i<1000;i++){
                    ra[0*1000+i]=time_c14[i];
                    ra[1*1000+i]=MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0-(MRA_max[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0-MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0);
                    ra[2*1000+i]=MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                    ra[3*1000+i]=MRA_max[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                }
                Editor *c=new Editor(this,ra,ra,4,1000);
                c->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                c->show();*/
                delete[] pdf_years;
                delete[] pdf_p;
                delete[] pdf_ra;
                //delete[] pdf_ra_min;
                delete[] pdf_ra_max;
                }
            }
        }
    }
    select=s;
    delete[] MRA_mean;
    //delete[] MRA_min;
    delete[] MRA_max;
    delete[] longitude;
    delete[] latitude;
    delete[] time;
    delete[] time_c14;
    delete[] time_c14_err;

    setupTable(0);
    update();

}

void AMS::reservoirSingle(){
    if (select>=0) reservoir(0);
}
void AMS::reservoirAll(){
    reservoir(1);
}

