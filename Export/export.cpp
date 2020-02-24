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

#include "export.h"
#include "ui_export.h"

Export::Export(QWidget *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv(inventory),proxy(str),
    ui(new Ui::Export)
{
    ui->setupUi(this);
    this->setWindowTitle("Export for Core : "+inv->get_Core(inv->get_currentCore())+" : "+inv->get_Species(inv->get_currentCore())+" at "+QString::number(inv->get_Longitude(inv->get_currentCore()))+"° :"+QString::number(inv->get_Latitude(inv->get_currentCore()))+"° in "+QString::number(inv->get_Water_Depth(inv->get_currentCore()))+"m");

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    //sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);
    amsdata=new AMSData(inv);
    amsdata->AMSRead();





        meta_Flag=new int[inv->access_meta_length()];
        for (int i=0;i<inv->access_meta_length();i++) meta_Flag[i]=1;


    // Bacon Meta Data
    bacon_meta_length=24;

    bacon_str1<<"K";
    bacon_str2<<QString::number(amsdata->get_bacon_K());
    bacon_str1<<"d_min";
    bacon_str2<<QString::number(amsdata->get_bacon_d_min());
    bacon_str1<<"d_max";
    bacon_str2<<QString::number(amsdata->get_bacon_d_max());
    bacon_str1<<"d.by";
    bacon_str2<<QString::number(amsdata->get_bacon_d_by());
    bacon_str1<<"acc.shape";
    bacon_str2<<QString::number(amsdata->get_bacon_acc_shape());
    bacon_str1<<"acc.mean";
    bacon_str2<<QString::number(amsdata->get_bacon_acc_mean());
    bacon_str1<<"mem.strength";
    bacon_str2<<QString::number(amsdata->get_bacon_mem_strength());
    bacon_str1<<"mem.mean";
    bacon_str2<<QString::number(amsdata->get_bacon_mem_mean());
    bacon_str1<<"cc";
    bacon_str2<<QString::number(amsdata->get_bacon_cc());
    bacon_str1<<"cc1";
    bacon_str2<<amsdata->get_bacon_cc1();
    bacon_str1<<"cc2";
    bacon_str2<<amsdata->get_bacon_cc2();
    bacon_str1<<"cc3";
    bacon_str2<<amsdata->get_bacon_cc3();
    bacon_str1<<"cc4";
    bacon_str2<<amsdata->get_bacon_cc4();
    bacon_str1<<"postbomb";
    bacon_str2<<QString::number(amsdata->get_bacon_postbomb());
    bacon_str1<<"t_a";
    bacon_str2<<QString::number(amsdata->get_bacon_t_a());
    bacon_str1<<"t_b";
    bacon_str2<<QString::number(amsdata->get_bacon_t_b());
    bacon_str1<<"normal";
    bacon_str2<<QString::number(amsdata->get_bacon_normal());
    bacon_str1<<"suggest";
    bacon_str2<<QString::number(amsdata->get_bacon_suggest());
    bacon_str1<<"th0";
    bacon_str2<<QString::number(amsdata->get_bacon_th0());
    bacon_str1<<"th0p";
    bacon_str2<<QString::number(amsdata->get_bacon_th0p());
    bacon_str1<<"burnin";
    bacon_str2<<QString::number(amsdata->get_bacon_burnin());
    bacon_str1<<"ssize";
    bacon_str2<<QString::number(amsdata->get_bacon_ssize());
    bacon_str1<<"yr_min";
    bacon_str2<<QString::number(amsdata->get_bacon_yr_min());
    bacon_str1<<"yr_max";
    bacon_str2<<QString::number(amsdata->get_bacon_yr_max());
    //... more Meta Data

    bacon_meta_Flag=new int[bacon_meta_length];
    for (int i=0;i<bacon_meta_length;i++) bacon_meta_Flag[i]=1;
    //metaData = new QStandardItemModel(0,0,this);
    //modelIsotope = new QStandardItemModel(0,0,this);
    //ageData = new QStandardItemModel(0,0,this);
    //baconAge = new QStandardItemModel(0,0,this);
    //baconOut = new QStandardItemModel(0,0,this);


    dlm="\t";
    setupTable();
    setupASCII();
    connect(ui->tableView_3,SIGNAL(clicked(QModelIndex)),this,SLOT(metaTableSelected(QModelIndex)));
    connect(ui->tableView_6,SIGNAL(clicked(QModelIndex)),this,SLOT(bacon_metaTableSelected(QModelIndex)));
    connect(ui->checkBox,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(delimiterChanged()));
    connect(ui->checkBox_2,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));
    connect(ui->checkBox_3,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));
    connect(ui->checkBox_4,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));
    connect(ui->checkBox_5,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));
    connect(ui->checkBox_6,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));
    connect(ui->checkBox_7,SIGNAL(clicked(bool)),this,SLOT(delimiterChanged()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    ui->tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_2(QPoint)));
    ui->tableView_3->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_3,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_3(QPoint)));
    ui->tableView_4->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_4,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_4(QPoint)));
    ui->tableView_5->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_5,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_5(QPoint)));
    ui->tableView_6->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_6,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_6(QPoint)));
}

Export::~Export()
{
    delete ui;
    //delete str1;
    //delete str2;
    delete amsdata;
    delete meta_Flag;
    delete bacon_meta_Flag;

}

void Export::setupTable(){
    // setup Meta
    // create the model for Meta Data

    //delete metaData;
    metaData = new QStandardItemModel(inv->access_meta_length(),3,this);
    metaData->setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    metaData->setHorizontalHeaderItem(2, new QStandardItem(QString("Value")));
    metaData->setHorizontalHeaderItem(0, new QStandardItem(QString("Status")));

    ui->tableView_3->setModel(metaData);
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources.get_Textsize());
    ui->tableView_3->setFont(font);
    ui->tableView_3->horizontalHeader()->setFont(font);
    ui->tableView_3->verticalHeader()->setFont(font);
    // Add Meta
    // Status Flag
    for (int i=0;i<inv->access_meta_length();i++) {
        QStandardItem *meta_Name=new QStandardItem(inv->access_meta_name(i));
        metaData->setItem(i,1,meta_Name);

        QStandardItem *meta_Value= new QStandardItem(inv->access_meta_value(i));
        metaData->setItem(i,2,meta_Value);

        QStandardItem *meta_Stat=new QStandardItem("No Export");
        meta_Stat->setCheckable(true);

        if (meta_Flag[i]==1) {
            meta_Stat->setCheckState(Qt::Checked);
            meta_Stat->setText("Export");
        }
        metaData->setItem(i,0,meta_Stat);
    }

    ui->tableView_3->setSortingEnabled(0);
    ui->tableView_3->verticalHeader()->setDefaultSectionSize(ui->tableView_3->verticalHeader()->minimumSectionSize());
    //ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->setHorizontalScrollMode(ui->tableView_3->ScrollPerPixel);
    //ui->tableView_3->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_3->horizontalHeader()->setStretchLastSection(1);
    ui->tableView_3->repaint();
    // setup IsoData
    // create the model for Meta Data



        modelIsotope = new QStandardItemModel(inv->get_Length(),inv->access_data_length(),this);
        for (int i=0;i<inv->access_data_length();i++) {
            QString str=inv->access_data_name(i);
            if (inv->access_data_unit(i)!="") str.append("\n["+inv->access_data_unit(i)+"]");
            modelIsotope->setHorizontalHeaderItem(i, new QStandardItem(str));
        }

        ui->tableView->setModel(modelIsotope);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        ui->tableView->setFont(font);
        ui->tableView->horizontalHeader()->setFont(font);
        ui->tableView->verticalHeader()->setFont(font);
        for (int i=0;i<inv->get_Length();i++){
            for (int j=0;j<inv->access_data_length();j++){
            QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
            modelIsotope->setItem(i,j,var);
            }

        }
        ui->tableView->setSortingEnabled(0);
        //ui->tableView->resizeColumnsToContents();
        ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
        ui->tableView->horizontalHeader()->setStretchLastSection(0);
        //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
        ui->tableView->repaint();



    // setup AgeModel
    //delete ageData;
    ageData = new QStandardItemModel(amsdata->get_Length(),15,this);
    ageData->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    ageData->setHorizontalHeaderItem(1, new QStandardItem(QString("Depth")));
    ageData->setHorizontalHeaderItem(2, new QStandardItem(QString("Sample\nThickness")));
    ageData->setHorizontalHeaderItem(3, new QStandardItem(QString("Label")));
    ageData->setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    ageData->setHorizontalHeaderItem(5, new QStandardItem(QString("Age dated\n[ka]")));
    ageData->setHorizontalHeaderItem(6, new QStandardItem(QString("Age UCL\n[ka+]")));
    ageData->setHorizontalHeaderItem(7, new QStandardItem(QString("Age LCL\n[ka-]")));
    ageData->setHorizontalHeaderItem(8, new QStandardItem(QString("Res. Age\n[ka]")));
    ageData->setHorizontalHeaderItem(9, new QStandardItem(QString("Res. Age\nError\n[ka]")));
    ageData->setHorizontalHeaderItem(10, new QStandardItem(QString("Cal yrs\n[wm ka BP]")));
    ageData->setHorizontalHeaderItem(11, new QStandardItem(QString("Cal yrs min\n[95%]")));
    ageData->setHorizontalHeaderItem(12, new QStandardItem(QString("Cal yrs max\n[95%]")));
    ageData->setHorizontalHeaderItem(13, new QStandardItem(QString("Use Flag")));
    ageData->setHorizontalHeaderItem(14, new QStandardItem(QString("Comments")));

    ui->tableView_2->setModel(ageData);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView_2->setFont(font);
    ui->tableView_2->horizontalHeader()->setFont(font);
    ui->tableView_2->verticalHeader()->setFont(font);


    for (int i=0;i<amsdata->get_Length();i++){
            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            ageData->setItem(i,0,var_Index);

            QStandardItem *var_Depth=new QStandardItem(QString::number(amsdata->get_Depth(i)));
            ageData->setItem(i,1,var_Depth);

            QStandardItem *var_Sample_Thickness=new QStandardItem(QString::number(amsdata->get_Sample_Thickness(i)));
            ageData->setItem(i,2,var_Sample_Thickness);

            QStandardItem *var_Label=new QStandardItem(amsdata->get_LabID(i));
            ageData->setItem(i,3,var_Label);

            QStandardItem *var_Type=new QStandardItem(amsdata->get_Type(i));
            ageData->setItem(i,4,var_Type);

            QStandardItem *var_Age_dated=new QStandardItem(QString::number(amsdata->get_Data(0,i)));
            ageData->setItem(i,5,var_Age_dated);

            QStandardItem *var_Age_UCL=new QStandardItem(QString::number(amsdata->get_Data(1,i)));
            ageData->setItem(i,6,var_Age_UCL);

            QStandardItem *var_Age_LCL=new QStandardItem(QString::number(amsdata->get_Data(2,i)));
            ageData->setItem(i,7,var_Age_LCL);

            QStandardItem *var_Age_Res=new QStandardItem(QString::number(amsdata->get_Data(3,i)));
            ageData->setItem(i,8,var_Age_Res);

            QStandardItem *var_Age_Res_Error=new QStandardItem(QString::number(amsdata->get_Reservoir_Error(i)));
            ageData->setItem(i,9,var_Age_Res_Error);

            QStandardItem *var_Cal=new QStandardItem(QString::number(amsdata->get_Data(4,i)));
            ageData->setItem(i,10,var_Cal);

            QStandardItem *var_Cal_Min=new QStandardItem(QString::number(amsdata->get_Data(5,i)));
            ageData->setItem(i,11,var_Cal_Min);

            QStandardItem *var_Cal_Max=new QStandardItem(QString::number(amsdata->get_Data(6,i)));
            ageData->setItem(i,12,var_Cal_Max);

            QStandardItem *var_Use_Flag=new QStandardItem(QString::number(amsdata->get_Data(7,i)));
            ageData->setItem(i,13,var_Use_Flag);
            ageData->setData(ageData->index(i, 13), Qt::AlignCenter,Qt::TextAlignmentRole);

            QStandardItem *var_Comment=new QStandardItem(amsdata->get_Age_Comment(i));
            ageData->setItem(i,14,var_Comment);

    }
    ui->tableView_2->setSortingEnabled(0);
    ui->tableView_2->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
    //ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setHorizontalScrollMode(ui->tableView_2->ScrollPerPixel);
    //ui->tableView_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(0);
    ui->tableView_2->repaint();

    // setup Bacon Meta
    // create the model for Bacon Meta Data

    bacon_metaData = new QStandardItemModel(bacon_meta_length,3,this);
    bacon_metaData->setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    bacon_metaData->setHorizontalHeaderItem(2, new QStandardItem(QString("Value")));
    bacon_metaData->setHorizontalHeaderItem(0, new QStandardItem(QString("Status")));

    ui->tableView_6->setModel(bacon_metaData);
    ui->tableView_6->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView_6->setFont(font);
    ui->tableView_6->horizontalHeader()->setFont(font);
    ui->tableView_6->verticalHeader()->setFont(font);
    //QStandardItem *bacon_meta_Name = new QStandardItem[bacon_meta_length];
    //QStandardItem *bacon_meta_Value = new QStandardItem[bacon_meta_length];
    //QStandardItem *bacon_meta_Stat = new QStandardItem[bacon_meta_length];

    // Add Meta
    // Status Flag
    for (int i=0;i<bacon_meta_length;i++) {
        QStandardItem *bacon_meta_Name=new QStandardItem(bacon_str1.at(i));
        bacon_metaData->setItem(i,1,bacon_meta_Name);

        QStandardItem *bacon_meta_Value=new QStandardItem(bacon_str2.at(i));
        bacon_metaData->setItem(i,2,bacon_meta_Value);

        QStandardItem *bacon_meta_Stat=new QStandardItem("No Export");
        bacon_meta_Stat->setCheckable(true);

        if (bacon_meta_Flag[i]==1) {
            bacon_meta_Stat->setCheckState(Qt::Checked);
            bacon_meta_Stat->setText("Export");
        }
        bacon_metaData->setItem(i,0,bacon_meta_Stat);
    }

    ui->tableView_6->setSortingEnabled(0);
    ui->tableView_6->verticalHeader()->setDefaultSectionSize(ui->tableView_6->verticalHeader()->minimumSectionSize());
    //ui->tableView_6->resizeColumnsToContents();
    ui->tableView_6->setHorizontalScrollMode(ui->tableView_6->ScrollPerPixel);
    //ui->tableView_6->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_6->horizontalHeader()->setStretchLastSection(1);
    ui->tableView_6->repaint();

    //setup Bacon Age model
    //delete baconAge;
    baconAge = new QStandardItemModel(amsdata->get_bacon_age_nrow(),amsdata->get_bacon_age_ncol(),this);
    baconAge->setHorizontalHeaderItem(0, new QStandardItem(QString("Depth[m]")));
    baconAge->setHorizontalHeaderItem(1, new QStandardItem(QString("Age Average[kyr]")));
    baconAge->setHorizontalHeaderItem(2, new QStandardItem(QString("5% Range[kyr]")));
    baconAge->setHorizontalHeaderItem(3, new QStandardItem(QString("Median[kyr]")));
    baconAge->setHorizontalHeaderItem(4, new QStandardItem(QString("95% Range[kyr]")));


    ui->tableView_4->setModel(baconAge);
    ui->tableView_4->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView_4->setFont(font);
    ui->tableView_4->horizontalHeader()->setFont(font);
    ui->tableView_4->verticalHeader()->setFont(font);
    //QStandardItem *var_D = new QStandardItem[amsdata->get_bacon_age_nrow()];
    //QStandardItem *var_Average = new QStandardItem[amsdata->get_bacon_age_nrow()];
    //QStandardItem *var_5 = new QStandardItem[amsdata->get_bacon_age_nrow()];
    //QStandardItem *var_50 = new QStandardItem[amsdata->get_bacon_age_nrow()];
    //QStandardItem *var_95 = new QStandardItem[amsdata->get_bacon_age_nrow()];


    for (int i=0;i<amsdata->get_bacon_age_nrow();i++){
            QStandardItem *var_D=new QStandardItem(QString::number(amsdata->get_bacon_age_depth(i)/100));
            baconAge->setItem(i,0,var_D);
            QStandardItem *var_Average=new QStandardItem(QString::number(amsdata->get_bacon_age_mean(i)/1000));
            baconAge->setItem(i,1,var_Average);
            QStandardItem *var_5=new QStandardItem(QString::number(amsdata->get_bacon_age_Q5(i)/1000));
            baconAge->setItem(i,2,var_5);
            QStandardItem *var_50=new QStandardItem(QString::number(amsdata->get_bacon_age_median(i)/1000));
            baconAge->setItem(i,3,var_50);
            QStandardItem *var_95=new QStandardItem(QString::number(amsdata->get_bacon_age_Q95(i)/1000));
            baconAge->setItem(i,4,var_95);
    }
    ui->tableView_4->setSortingEnabled(0);
    ui->tableView_4->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView_4->verticalHeader()->setDefaultSectionSize(ui->tableView_4->verticalHeader()->minimumSectionSize());
    //ui->tableView_4->resizeColumnsToContents();
    ui->tableView_4->setHorizontalScrollMode(ui->tableView_4->ScrollPerPixel);
    //ui->tableView_4->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_4->horizontalHeader()->setStretchLastSection(0);
    ui->tableView_4->repaint();

    if (ui->checkBox_6->isChecked()){
        // setup Bacon out
        int outr=amsdata->get_bacon_out_nrow();
        int outc=amsdata->get_bacon_out_ncol();
        if (outr>100)outr=100;//Limit these resultview to first 100 lines
        if (outc>0){
            //delete baconOut;
            baconOut = new QStandardItemModel(outr,outc-1,this);

            baconOut->setHorizontalHeaderItem(0, new QStandardItem(QString("Starting Age")));
            for (int i=0;i<amsdata->get_bacon_out_ncol()-3;i++) {
                float d1=amsdata->get_bacon_d_min()+i*((amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K());
                float d2=amsdata->get_bacon_d_min()+(i+1)*((amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K());
                baconOut->setHorizontalHeaderItem(1+i, new QStandardItem(QString("Acc(d="+QString::number(d1)+"-"+QString::number(d2)+"cm)")));
            }
            baconOut->setHorizontalHeaderItem(amsdata->get_bacon_out_ncol()-2, new QStandardItem(QString("Memory")));
            baconOut->setHorizontalHeaderItem(amsdata->get_bacon_out_ncol()-1, new QStandardItem(QString("Iteration")));



            ui->tableView_5->setModel(baconOut);
            ui->tableView_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
            QFont font;
            font.setPointSize(resources.get_Textsize());
            ui->tableView_5->setFont(font);
            ui->tableView_5->horizontalHeader()->setFont(font);
            ui->tableView_5->verticalHeader()->setFont(font);

            int nrow=amsdata->get_bacon_out_nrow();
            if (nrow>100)nrow=100;


            for (int i=0;i<nrow;i++){
                    QStandardItem *var_Age=new QStandardItem(QString::number(amsdata->get_bacon_out(0*nrow+i)/1000.0));
                    baconOut->setItem(i,0,var_Age);
                    for (int j=1;j<amsdata->get_bacon_out_ncol()-2;j++){
                        QStandardItem *var_Acc=new QStandardItem(QString::number(amsdata->get_bacon_out(i+j*nrow)));
                        baconOut->setItem(i,j,var_Acc);
                    }

                    QStandardItem *var_Mem=new QStandardItem(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-2)*nrow)));
                    baconOut->setItem(i,amsdata->get_bacon_out_ncol()-2,var_Mem);

                    QStandardItem *var_It=new QStandardItem(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-1)*nrow)));
                    baconOut->setItem(i,amsdata->get_bacon_out_ncol()-1,var_It);



            }
            ui->tableView_5->setSortingEnabled(0);
            ui->tableView_5->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
            ui->tableView_5->verticalHeader()->setDefaultSectionSize(ui->tableView_5->verticalHeader()->minimumSectionSize());
            //ui->tableView_5->resizeColumnsToContents();
            ui->tableView_5->setHorizontalScrollMode(ui->tableView_5->ScrollPerPixel);
            //ui->tableView_5->setSelectionMode(QAbstractItemView::NoSelection);
            ui->tableView_5->repaint();
        }
    }

}

void Export::metaTableSelected(QModelIndex mi){
    int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_3->model());
    QString text = model->item(sel,mi.column())->text();
    //qDebug() << text;

    if (mi.column()==0){
        if (meta_Flag[sel]==1){
            meta_Flag[sel]=0;
        } else {
            meta_Flag[sel]=1;
        }
    }

    setupTable();
    setupASCII();
}
void Export::bacon_metaTableSelected(QModelIndex mi){
    int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_6->model());
    QString text = model->item(sel,mi.column())->text();
    //qDebug() << text;

    if (mi.column()==0){
        if (bacon_meta_Flag[sel]==1){
            bacon_meta_Flag[sel]=0;
        } else {
            bacon_meta_Flag[sel]=1;
        }
    }

    setupTable();
    setupASCII();
}

void Export::delimiterChanged(){
    dlm=ui->lineEdit_2->text();
    dlm.replace("\\t","\t");


    setupTable();
    setupASCII();
}

void Export::setupASCII(){
    // Font setup
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(resources.get_Textsize());

    ui->textBrowser->setFont(font);

    QFontMetrics metrics(font);
    ui->textBrowser->setTabStopWidth(ui->spinBox->value() * metrics.width(' '));

    // create the result shown on left
    txt.clear();
    if (ui->checkBox_2->checkState()==Qt::Checked){

            // Create Meta Data
            for (int i=0;i<inv->access_meta_length();i++){
                if (meta_Flag[i]){

                    QString tmp=inv->access_meta_name(i);
                    if (inv->access_meta_unit(i)!="") tmp.append(" ["+inv->access_meta_unit(i)+"]");

                    if (ui->checkBox->checkState()!=Qt::Checked){

                         txt.append(tmp+dlm+inv->access_meta_value(i)+"\n");

                    } else{
                        if (inv->access_meta_type(i).contains("float")){
                            QString tmp2=inv->access_meta_value(i);
                            txt.append(tmp+dlm+tmp2.replace(".",",")+"\n");
                        }else{


                                txt.append(tmp+dlm+inv->access_meta_value(i)+"\n");

                        }
                    }
                }
            }
            txt.append("\n");



    }


    // Create Isotope Data
    if (ui->checkBox_3->checkState()==Qt::Checked){
        txt.append("Proxy Data\n");

            // Header of Table
            for (int j=0;j<inv->access_data_length();j++) {

                txt.append(inv->access_data_name(j));
                        if (inv->access_data_unit(j)!="") txt.append(" ["+inv->access_data_unit(j)+"]");
                if (j<inv->access_data_length()-1) txt.append(dlm); else txt.append("\n");
            }
            // Data
            for(int i=0;i<inv->get_Length();i++){
                if (ui->checkBox->checkState()!=Qt::Checked){
                    for (int j=0;j<inv->access_data_length();j++) {
                        txt.append(inv->access_data_value(j,i));
                        if (j<inv->access_data_length()-1) txt.append(dlm); else txt.append("\n");
                    }
                }else{
                    for (int j=0;j<inv->access_data_length();j++) {
                        txt.append(inv->access_data_value(j,i).replace(".",","));
                        if (j<inv->access_data_length()-1) txt.append(dlm); else txt.append("\n");
                    }
                }
            }
            txt.append("\n");


    }

    // Create Age Model Data
    if (ui->checkBox_4->checkState()==Qt::Checked){
        txt.append("Age Model Data\n");
        // Header
        txt.append("Depth [m]"+dlm+"Sample Thickness [m]"+dlm+"Label"+dlm+"Type"+dlm+"Age dated [ka]"+dlm+"Age UCL [ka +]"+dlm+"Age LCL [ka -]"+dlm+"Res. Age [ka]"+dlm+"Res. Age Error [ka]"+dlm+"Cal yrs [wm ka BP]"+dlm+"Cal yrs min [95%]"+dlm+"Cal yrs max [95%]"+dlm+"Use Flag"+dlm+"Dating Method/Comments\n");
        // Data
        for(int i=0;i<amsdata->get_Length();i++){
            if (ui->checkBox->checkState()!=Qt::Checked){
                txt.append(QString::number(amsdata->get_Depth(i))+dlm+
                           QString::number(amsdata->get_Sample_Thickness(i))+dlm+
                           amsdata->get_LabID(i)+dlm+
                           amsdata->get_Type(i)+dlm+
                           QString::number(amsdata->get_Data(0,i))+dlm+
                           QString::number(amsdata->get_Data(1,i))+dlm+
                           QString::number(amsdata->get_Data(2,i))+dlm+
                           QString::number(amsdata->get_Data(3,i))+dlm+
                           QString::number(amsdata->get_Reservoir_Error(i))+dlm+
                           QString::number(amsdata->get_Data(4,i))+dlm+
                           QString::number(amsdata->get_Data(5,i))+dlm+
                           QString::number(amsdata->get_Data(6,i))+dlm+
                           QString::number(amsdata->get_Data(7,i))+dlm+
                           amsdata->get_Age_Comment(i)+"\n");
            }else{
                txt.append(QString::number(amsdata->get_Depth(i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Sample_Thickness(i)).replace(".",",")+dlm+
                           amsdata->get_LabID(i)+dlm+
                           amsdata->get_Type(i)+dlm+
                           QString::number(amsdata->get_Data(0,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(1,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(2,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(3,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Reservoir_Error(i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(4,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(5,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(6,i)).replace(".",",")+dlm+
                           QString::number(amsdata->get_Data(7,i))+dlm+
                           amsdata->get_Age_Comment(i)+"\n");
            }
        }
        txt.append("\n");
    }
    // Bacon Meta Data
    if (ui->checkBox_7->checkState()==Qt::Checked && amsdata->get_bacon_out_ncol()>0){
        txt.append("Bacon Parameters\n");
        for (int i=0;i<bacon_meta_length;i++){
            if (bacon_meta_Flag[i]){
                QString tmp=bacon_str1.at(i);
                while (tmp.size()<30) tmp.append(" ");
                tmp.append(dlm);
                if (ui->checkBox->checkState()!=Qt::Checked){
                    txt.append(tmp+dlm+bacon_str2.at(i)+"\n");
                } else{

                        QString tmp2=bacon_str2.at(i);
                        txt.append(tmp+dlm+tmp2.replace(".",",")+"\n");

                }
            }
        }
        txt.append("\n");
    }
    // Bacon Age Model
    if (ui->checkBox_5->checkState()==Qt::Checked&& amsdata->get_bacon_age_ncol()>0){
        txt.append("Bacon Age Model\n");
        // Header of Table
        txt.append("Depth in [m]"+dlm+"Average Age in [kyr]"+dlm+"5% Range in [kyr]"+dlm+"Median in [kyr]"+dlm+"5% Range in [kyr]\n");
        // Data
        for(int i=0;i<amsdata->get_bacon_age_nrow();i++){

            if (ui->checkBox->checkState()!=Qt::Checked){

                txt.append(QString::number(amsdata->get_bacon_age_depth(i)/100)+dlm+
                           QString::number(amsdata->get_bacon_age_mean(i)/1000)+dlm+
                           QString::number(amsdata->get_bacon_age_Q5(i)/1000)+dlm+
                           QString::number(amsdata->get_bacon_age_median(i)/1000)+dlm+
                           QString::number(amsdata->get_bacon_age_Q95(i)/1000)+"\n");
            }else{
                txt.append(QString::number(amsdata->get_bacon_age_depth(i)/100).replace(".",",")+dlm+
                           QString::number(amsdata->get_bacon_age_mean(i)/1000).replace(".",",")+dlm+
                           QString::number(amsdata->get_bacon_age_Q5(i)/1000).replace(".",",")+dlm+
                           QString::number(amsdata->get_bacon_age_median(i)/1000).replace(".",",")+dlm+
                           QString::number(amsdata->get_bacon_age_Q95(i)/1000).replace(".",",")+"\n");
            }
        }
        txt.append("\n");
    }
    // Bacon Output
    if (ui->checkBox_6->checkState()==Qt::Checked && amsdata->get_bacon_out_ncol()>0){
        txt.append("Bacon Output\n");
        // Header of Table
        txt.append("Starting Age in [kyr]"+dlm);
        for (int i=0;i<amsdata->get_bacon_out_ncol()-3;i++){
            float d1=amsdata->get_bacon_d_min()+i*((amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K());
            float d2=amsdata->get_bacon_d_min()+(i+1)*((amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K());
            txt.append(QString("Acc(d="+QString::number(d1)+"-"+QString::number(d2)+"cm)")+dlm);
        }
        txt.append("Memory"+dlm+"Iteration\n");
        // Data
        for(int i=0;i<amsdata->get_bacon_out_nrow();i++){

            if (ui->checkBox->checkState()!=Qt::Checked){
                txt.append(QString::number(amsdata->get_bacon_out(i+0*amsdata->get_bacon_out_nrow())/1000)+dlm);
                for (int j=1;j<amsdata->get_bacon_out_ncol()-2;j++) txt.append(QString::number(amsdata->get_bacon_out(i+j*amsdata->get_bacon_out_nrow()))+dlm);
                txt.append(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-2)*amsdata->get_bacon_out_nrow()))+dlm);
                txt.append(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-1)*amsdata->get_bacon_out_nrow()))+"\n");
            }else{
                txt.append(QString::number(amsdata->get_bacon_out(i+0*amsdata->get_bacon_out_nrow())/1000).replace(".",",")+dlm);
                for (int j=1;j<amsdata->get_bacon_out_ncol()-2;j++) txt.append(QString::number(amsdata->get_bacon_out(i+j*amsdata->get_bacon_out_nrow())).replace(".",",")+dlm);
                txt.append(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-2)*amsdata->get_bacon_out_nrow())).replace(".",",")+dlm);
                txt.append(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-1)*amsdata->get_bacon_out_nrow())).replace(".",",")+"\n");
            }
        }
        txt.append("\n");
    }
    // put to preview
    ui->textBrowser->clear();
    ui->textBrowser->setText(txt);
    update();
}

void Export::browse(){

    QString file = QFileDialog::getSaveFileName(this, tr("Select Save File"),
                                             resources.path_data+"//"+proxy+"//Import//"+inv->get_att_Core(),
                                             tr("Excel (*.xlsx);;Text File (*.txt)"));

    //qDebug() << file;

    if (file.right(4)==".txt") save(file);
    if (file.right(5)==".xlsx") savexls(file);



    update();
}

void Export::save(QString file){
    // get file name
    QString QFilename=file;
    QFilename.replace(".xlsx",".txt");
    //qDebug() << QFilename;

    QFile f(QFilename);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //qDebug() <<  f.errorString();
    } else {
    //const char *text=txt.toStdString().c_str();
    //f.write(text,qstrlen(text));
    QTextStream out(&f);
    out<<txt;
    f.close();
    }
    QDesktopServices::openUrl(QUrl("file:///"+QFilename, QUrl::TolerantMode));
    emit(this->close());
}

void Export::savexls(QString file){
    // get file name
    QString QFilename=file;
    QFilename.replace(".txt",".xlsx");

    //qDebug() << QFilename;


    QXlsx::Document xlsx;
    if (ui->checkBox_2->checkState()==Qt::Checked){


            xlsx.addSheet("Meta Data", AbstractSheet::ST_WorkSheet);
            xlsx.selectSheet("Meta Data");

                xlsx.setColumnWidth(1, 2, 40);
                // Create Meta Data
                int count=0;
                for (int i=0;i<inv->access_meta_length();i++){
                    if (meta_Flag[i]){
                        // write meta anme
                        QString str=inv->access_meta_name(i);
                        if (inv->access_meta_unit(i)!="") str.append(" ["+inv->access_meta_unit(i)+"]");
                        xlsx.write("A"+QString::number(count+1),str);
                        if (inv->access_meta_type(i).contains("string")||inv->access_meta_type(i).contains("combo")){
                            xlsx.write("B"+QString::number(count+1),inv->access_meta_value(i));
                        } else {
                            Format lAlign;
                            lAlign.setHorizontalAlignment(Format::AlignLeft);
                            xlsx.write("B"+QString::number(count+1),round(inv->access_meta_value(i).toFloat()*1000.0)/1000.0,lAlign);
                        }
                        count++;
                    }
                }
    }
    if (ui->checkBox_3->checkState()==Qt::Checked){
        xlsx.addSheet("Proxy", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Proxy");
            // Header
            xlsx.setColumnWidth(1, 9, 20);
            for (int i=0;i<inv->access_data_length();i++){
                QString str=inv->access_data_name(i);
                if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
                char c=(char)65+i;
                QString f="";
                f.append(c);
                f.append("1");
                qDebug()<<f;

                xlsx.write(f,str);
            }

            // data
            for(int i=0;i<inv->get_Length();i++){
                for (int j=0;j<inv->access_data_length();j++){
                    char c=(char)65+j;
                    QString f="";
                    f.append(c);
                    f.append(QString::number(i+2));
                    qDebug()<<f;
                    if (inv->access_data_type(j).contains("string")){
                        xlsx.write(f,inv->access_data_value(j,i));
                    } else {
                        if (isnan(inv->access_data_value(j,i).toFloat())==false){
                            xlsx.write(f,round(inv->access_data_value(j,i).toFloat()*1000.0)/1000.0);
                        }else{
                            xlsx.write(f,"NAN");
                        }
                    }
                }
            }
    }
    if (ui->checkBox_4->checkState()==Qt::Checked){
        xlsx.addSheet("Age", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Age");
        xlsx.setColumnWidth(1, 12, 20);
        xlsx.write("A1","Depth [m]");
        xlsx.write("B1","Sample Thickness [m]");
        xlsx.write("C1","Label");
        xlsx.write("D1","Type");
        xlsx.write("E1","Age dated [ka]");
        xlsx.write("F1","Age UCL [ka +]");
        xlsx.write("G1","Age LCL [ka -]");
        xlsx.write("H1","Res. Age [ka]");
        xlsx.write("I1","Res. Age Error [ka]");
        xlsx.write("J1","Cal yrs [wm ka BP]");
        xlsx.write("K1","Cal yrs min [95%]");
        xlsx.write("L1","Cal yrs max [95%]");
        xlsx.write("M1","Use Flag");
        xlsx.write("N1","Dating Method/Comments");
        for(int i=0;i<amsdata->get_Length();i++){
            if (isnan(amsdata->get_Depth(i))==false){xlsx.write("A"+QString::number(i+2),round(amsdata->get_Depth(i)*1000.0)/1000.0);}else{xlsx.write("A"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Sample_Thickness(i))==false){xlsx.write("B"+QString::number(i+2),round(amsdata->get_Sample_Thickness(i)*1000.0)/1000.0);}else{xlsx.write("B"+QString::number(i+2),"NAN");}
            xlsx.write("C"+QString::number(i+2),amsdata->get_LabID(i));
            xlsx.write("D"+QString::number(i+2),amsdata->get_Type(i));
            if (isnan(amsdata->get_Data(0,i))==false){xlsx.write("E"+QString::number(i+2),round(amsdata->get_Data(0,i)*1000.0)/1000.0);}else{xlsx.write("E"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(1,i))==false){xlsx.write("F"+QString::number(i+2),round(amsdata->get_Data(1,i)*1000.0)/1000.0);}else{xlsx.write("F"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(2,i))==false){xlsx.write("G"+QString::number(i+2),round(amsdata->get_Data(2,i)*1000.0)/1000.0);}else{xlsx.write("G"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(3,i))==false){xlsx.write("H"+QString::number(i+2),round(amsdata->get_Data(3,i)*1000.0)/1000.0);}else{xlsx.write("H"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Reservoir_Error(i))==false){xlsx.write("I"+QString::number(i+2),round(amsdata->get_Reservoir_Error(i)*1000.0)/1000.0);}else{xlsx.write("I"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(4,i))==false){xlsx.write("J"+QString::number(i+2),round(amsdata->get_Data(4,i)*1000.0)/1000.0);}else{xlsx.write("J"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(5,i))==false){xlsx.write("K"+QString::number(i+2),round(amsdata->get_Data(5,i)*1000.0)/1000.0);}else{xlsx.write("K"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(6,i))==false){xlsx.write("L"+QString::number(i+2),round(amsdata->get_Data(6,i)*1000.0)/1000.0);}else{xlsx.write("L"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_Data(7,i))==false){xlsx.write("M"+QString::number(i+2),round(amsdata->get_Data(7,i)*1000.0)/1000.0);}else{xlsx.write("M"+QString::number(i+2),"NAN");}
            xlsx.write("N"+QString::number(i+2),amsdata->get_Age_Comment(i));
        }
    }
    if (ui->checkBox_7->checkState()==Qt::Checked && amsdata->get_bacon_out_ncol()>0){
        xlsx.addSheet("Bacon Meta Data", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Bacon Meta Data");
        xlsx.setColumnWidth(1, 2, 40);
        // Create Meta Data
        int count=0;
        for (int i=0;i<bacon_meta_length;i++){
            if (bacon_meta_Flag[i]){
                xlsx.write("A"+QString::number(count+1),bacon_str1.at(i));
                if(i!=9&&i!=10&&i!=11&&i!=12){
                    Format lAlign;
                    lAlign.setHorizontalAlignment(Format::AlignLeft);
                    xlsx.write("B"+QString::number(count+1),round(bacon_str2.at(i).toFloat()*1000.0)/1000.0,lAlign);
                } else {
                    xlsx.write("B"+QString::number(count+1),bacon_str2.at(i));
                }
                count++;
            }
        }
    }

    if (ui->checkBox_5->checkState()==Qt::Checked&& amsdata->get_bacon_age_ncol()>0){
        xlsx.addSheet("Bacon Age", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Bacon Age");
        xlsx.setColumnWidth(1, 9, 20);
        xlsx.write("A1","Depth in [m]");
        xlsx.write("B1","Average Age in [kyr]");
        xlsx.write("C1","5% Range in [kyr]");
        xlsx.write("D1","Median in [kyr]");
        xlsx.write("E1","95% Range in [kyr]");

        for(int i=0;i<amsdata->get_bacon_age_nrow();i++){
            if (isnan(amsdata->get_bacon_age_depth(i))==false){xlsx.write("A"+QString::number(i+2),round(amsdata->get_bacon_age_depth(i)/100*1000.0)/1000.0);}else{xlsx.write("A"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_bacon_age_mean(i))==false){xlsx.write("B"+QString::number(i+2),round(amsdata->get_bacon_age_mean(i)/1000*1000.0)/1000.0);}else{xlsx.write("B"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_bacon_age_Q5(i))==false){xlsx.write("C"+QString::number(i+2),round(amsdata->get_bacon_age_Q5(i)/1000*1000.0)/1000.0);}else{xlsx.write("C"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_bacon_age_median(i))==false){xlsx.write("D"+QString::number(i+2),round(amsdata->get_bacon_age_median(i)/1000*1000.0)/1000.0);}else{xlsx.write("D"+QString::number(i+2),"NAN");}
            if (isnan(amsdata->get_bacon_age_Q95(i))==false){xlsx.write("E"+QString::number(i+2),round(amsdata->get_bacon_age_Q95(i)/1000*1000.0)/1000.0);}else{xlsx.write("E"+QString::number(i+2),"NAN");}


        }
    }

    if (ui->checkBox_6->checkState()==Qt::Checked && amsdata->get_bacon_out_ncol()>0){
        xlsx.addSheet("Bacon Output", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Bacon Output");
        xlsx.setColumnWidth(1, amsdata->get_bacon_out_ncol(), 20);
        xlsx.write(1,1,"Starting Age in [kyr]");
        for (int i=0;i<amsdata->get_bacon_out_ncol()-3;i++){
            float d1=amsdata->get_bacon_d_min()+i*((amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K());
            float d2=amsdata->get_bacon_d_min()+(i+1)*((amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K());
            xlsx.write(1,i+2,"Acc(d="+QString::number(d1)+"-"+QString::number(d2)+"cm)"+dlm);
        }
        xlsx.write(1,amsdata->get_bacon_out_ncol()-1,"Memory");
        xlsx.write(1,amsdata->get_bacon_out_ncol(),"Iteration");


        for(int i=0;i<amsdata->get_bacon_out_nrow();i++){
            if (isnan(amsdata->get_bacon_out(i+0*amsdata->get_bacon_out_nrow()))==false){xlsx.write(2+i,1,amsdata->get_bacon_out(i+0*amsdata->get_bacon_out_nrow())/1000);}else{xlsx.write(i+2,1,"NAN");}
            for (int j=1;j<amsdata->get_bacon_out_ncol()-2;j++) if (isnan(amsdata->get_bacon_out(i+0*amsdata->get_bacon_out_nrow()))==false){xlsx.write(2+i,j+1,amsdata->get_bacon_out(i+j*amsdata->get_bacon_out_nrow()));}else{xlsx.write(i+2,j+1,"NAN");}
            if (isnan(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-2)*amsdata->get_bacon_out_nrow()))==false){xlsx.write(2+i,amsdata->get_bacon_out_ncol()-1,amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-2)*amsdata->get_bacon_out_nrow()));}else{xlsx.write(i+2,amsdata->get_bacon_out_ncol()-1,"NAN");}
            if (isnan(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-1)*amsdata->get_bacon_out_nrow()))==false){xlsx.write(2+i,amsdata->get_bacon_out_ncol(),amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-1)*amsdata->get_bacon_out_nrow()));}else{xlsx.write(i+2,amsdata->get_bacon_out_ncol(),"NAN");}


            txt.append(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-2)*amsdata->get_bacon_out_nrow()))+dlm);
            txt.append(QString::number(amsdata->get_bacon_out(i+(amsdata->get_bacon_out_ncol()-1)*amsdata->get_bacon_out_nrow()))+"\n");

        }
    }


    xlsx.saveAs(QFilename);


    QDesktopServices::openUrl(QUrl("file:///"+QFilename, QUrl::TolerantMode));
    emit(this->close());
}

bool Export::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->tableView_2||obj==ui->tableView||obj==ui->tableView_3||obj==ui->textBrowser){
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

void Export::copy(QPoint pos){

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

void Export::copy_2(QPoint pos){

    QPoint globalPos = ui->tableView_2->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView_2->model();
            if (tableModel!=nullptr){
            int iRows = tableModel->rowCount();
            int iCols = tableModel->columnCount();
            for (int i=0;i<iCols;i++){
                QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                txt.replace("\n"," ");
                clipboardString.append(txt+"\t");
            }
            clipboardString.append("\n");

                QModelIndexList selectedIndexes = ui->tableView_2->selectionModel()->selectedIndexes();
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

void Export::copy_3(QPoint pos){

    QPoint globalPos = ui->tableView_3->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView_3->model();
            if (tableModel!=nullptr){
                int iRows = tableModel->rowCount();
                int iCols = tableModel->columnCount();
                for (int i=0;i<iCols;i++){
                    QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                    txt.replace("\n"," ");
                    clipboardString.append(txt+"\t");
                }
                clipboardString.append("\n");

                    QModelIndexList selectedIndexes = ui->tableView_3->selectionModel()->selectedIndexes();
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

void Export::copy_4(QPoint pos){

    QPoint globalPos = ui->tableView_4->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView_4->model();
            if (tableModel!=nullptr){
            int iRows = tableModel->rowCount();
            int iCols = tableModel->columnCount();
            for (int i=0;i<iCols;i++){
                QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                txt.replace("\n"," ");
                clipboardString.append(txt+"\t");
            }
            clipboardString.append("\n");

                QModelIndexList selectedIndexes = ui->tableView_4->selectionModel()->selectedIndexes();
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

void Export::copy_5(QPoint pos){

    QPoint globalPos = ui->tableView_5->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView_5->model();
            if (tableModel!=nullptr){
            int iRows = tableModel->rowCount();
            int iCols = tableModel->columnCount();
            for (int i=0;i<iCols;i++){
                QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                txt.replace("\n"," ");
                clipboardString.append(txt+"\t");
            }
            clipboardString.append("\n");

                QModelIndexList selectedIndexes = ui->tableView_5->selectionModel()->selectedIndexes();
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

void Export::copy_6(QPoint pos){

    QPoint globalPos = ui->tableView_6->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView_6->model();
            if (tableModel!=nullptr){
                int iRows = tableModel->rowCount();
                int iCols = tableModel->columnCount();
                for (int i=0;i<iCols;i++){
                    QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                    txt.replace("\n"," ");
                    clipboardString.append(txt+"\t");
                }
                clipboardString.append("\n");

                    QModelIndexList selectedIndexes = ui->tableView_6->selectionModel()->selectedIndexes();
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
