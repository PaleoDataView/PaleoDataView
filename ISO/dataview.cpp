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

#include "dataview.h"
#include "ui_dataview.h"

DataView::DataView(QWidget *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv(inventory),proxy(str),
    ui(new Ui::DataView)
{
    ui->setupUi(this);
    resources=new Resources();
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    sp_4=ui->splitter_4->saveState();
    qApp->installEventFilter(this);
    this->setWindowTitle("Edit");
    oldCore=inv->get_currentCore();
    if (oldCore<0) inv->set_currentCore(0);
    inv->readData(inv->get_currentCore());
    createFilter();
    createMeta();

    setupInventory();
    ui->tableView_4->sortByColumn(0, Qt::AscendingOrder);

    attd=new attDialog(this,inv->get_data_Comment(0),QString::number(0));




    connect(ui->tableView_4,SIGNAL(clicked(QModelIndex)),this,SLOT(entrySelected(QModelIndex)));// Isotope selected
    connect(this,SIGNAL(selectionChanged()),mainW,SLOT(redraw_score()));// refresh main window
    connect(button_apply,SIGNAL(clicked(bool)),this,SLOT(apply()));// apply filter

    connect(button_reset,SIGNAL(clicked(bool)),this,SLOT(reset()));// reset filter to default
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(select()));// select all filtered inventory
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(deselect()));// deselect all filtered inventory
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(invert()));// invert all filtered inventory
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabSelect(int)));// tabs clicked call setup
    connect(ui->comboBox_5,SIGNAL(currentIndexChanged(int)),this,SLOT(applyIsotope(int)));// show all/selected cores   
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(isotopeSelected(QModelIndex)));// entry selected

    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(refresh()));// reload attributes from file

    connect(ui->pushButton_8,SIGNAL(clicked(bool)),this,SLOT(save()));// save changes to isotope data

    connect(ui->tableView_5,SIGNAL(clicked(QModelIndex)),this,SLOT(invertFlag(QModelIndex)));// check for changes
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    ui->tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_2(QPoint)));
    ui->tableView_4->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_4,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_4(QPoint)));
    connect(ui->tableView_4->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(sortVerify()));
    ui->tableView_5->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_5,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_5(QPoint)));


    connect(ui->tabWidget_2,SIGNAL(currentChanged(int)),this,SLOT(applyData(int)));

    changes=false;


    amsdata=new AMSData(inv);

    setupIsotope();

}

DataView::~DataView(){
    delete ui;

    inv->set_currentCore(oldCore);
    inv->readData(oldCore);
}


void DataView::setupInventory(){




    qDebug()<<"model Inventory";
    // create the model for Inventory
    //delete modelInventory;
    modelInventory = new QStandardItemModel(inv->get_Entries(),2+inv->access_inv_length(),this);
    modelInventory->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelInventory->setHorizontalHeaderItem(1, new QStandardItem(QString("Selected")));
    for (int i=0;i<inv->access_inv_length();i++){
        modelInventory->setHorizontalHeaderItem(2+i, new QStandardItem(inv->access_inv_name(i)+"\n"+inv->access_inv_unit(i)));
    }
    modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length(), new QStandardItem(QString("Agemodel")));





    qDebug()<<"putting data";
    for (int i=0;i<inv->access_inv_entries();i++){

        QStandardItem *var_Index=new QStandardItem(QString::number(i));
        var_Index->setData(i,Qt::EditRole);
        modelInventory->setItem(i,0,var_Index);



            QStandardItem *var_Selected=new QStandardItem(QString::number(inv->get_Selected(i)));
            var_Selected->setCheckable(true);
            if (inv->get_Selected(i)) var_Selected->setCheckState(Qt::Checked);
            modelInventory->setItem(i,1,var_Selected);
            modelInventory->setData(modelInventory->index(i, 1), Qt::AlignCenter,Qt::TextAlignmentRole);
            if (inv->get_Selected(i)){
                modelInventory->setData(modelInventory->index(i, 1), QColor(Qt::lightGray), Qt::BackgroundRole);
            } else {
                modelInventory->setData(modelInventory->index(i, 1), QColor(Qt::white), Qt::BackgroundRole);
            }

            for (int j=0;j<inv->access_inv_length();j++){
                if (inv->access_inv_name(j).contains("flag",Qt::CaseInsensitive)){
                    QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                    var->setCheckable(true);
                    if (inv->access_inv_data(j,i).contains("1")){
                        var->setCheckState(Qt::Checked);
                        modelInventory->setItem(i,2+j,var);
                        modelInventory->setData(modelInventory->index(i, 2+j), Qt::AlignCenter,Qt::TextAlignmentRole);
                        modelInventory->setData(modelInventory->index(i, 2+j), QColor(Qt::green), Qt::BackgroundRole);
                    }else{
                        var->setCheckState(Qt::Unchecked);
                        modelInventory->setItem(i,2+j,var);
                        modelInventory->setData(modelInventory->index(i, 2+j), Qt::AlignCenter,Qt::TextAlignmentRole);
                        modelInventory->setData(modelInventory->index(i, 2+j), QColor(Qt::red), Qt::BackgroundRole);
                    }


                } else {
                    if (inv->access_inv_type(j).contains("string")){
                        QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                        modelInventory->setItem(i,2+j,var);

                    }
                    if (inv->access_inv_type(j).contains("float") || inv->access_inv_type(j).contains("int")){
                        QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                        var->setData(inv->access_inv_data(j,i).toFloat(),Qt::EditRole);
                        modelInventory->setItem(i,2+j,var);

                    }
                }
            }

            QStandardItem *var_AgeModel=new QStandardItem(QString::number(inv->get_AgeModel(i)));
            modelInventory->setItem(i,2+inv->access_inv_length(),var_AgeModel);






    }
    ui->tableView_4->setModel(modelInventory);
    ui->tableView_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView_4->setFont(font);
    ui->tableView_4->horizontalHeader()->setFont(font);
    ui->tableView_4->verticalHeader()->setFont(font);

    ui->tableView_4->setSortingEnabled(true);

    ui->tableView_4->verticalHeader()->setDefaultSectionSize(ui->tableView_4->verticalHeader()->minimumSectionSize());
    //ui->tableView_4->resizeColumnsToContents();
    ui->tableView_4->setHorizontalScrollMode(ui->tableView_4->ScrollPerPixel);


    ui->tableView_4->repaint();

    qDebug()<<"finished";
}

void DataView::reInventory(int i,int pos){




            QStandardItem *var_Selected=new QStandardItem(QString::number(inv->get_Selected(i)));
            var_Selected->setCheckable(true);
            if (inv->get_Selected(i)) var_Selected->setCheckState(Qt::Checked);
            modelInventory->setItem(pos,1,var_Selected);
            modelInventory->setData(modelInventory->index(pos, 1), Qt::AlignCenter,Qt::TextAlignmentRole);
            if (inv->get_Selected(i)){
                modelInventory->setData(modelInventory->index(pos, 1), QColor(Qt::lightGray), Qt::BackgroundRole);
            } else {
                modelInventory->setData(modelInventory->index(pos, 1), QColor(Qt::white), Qt::BackgroundRole);
            }

            for (int j=0;j<inv->access_inv_length();j++){
                if (inv->access_inv_name(j).contains("flag",Qt::CaseInsensitive)){
                    QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                    var->setCheckable(true);
                    if (inv->access_inv_data(j,i).contains("1")){
                        var->setCheckState(Qt::Checked);
                        modelInventory->setItem(pos,2+j,var);
                        modelInventory->setData(modelInventory->index(pos, 2+j), Qt::AlignCenter,Qt::TextAlignmentRole);
                        modelInventory->setData(modelInventory->index(pos, 2+j), QColor(Qt::green), Qt::BackgroundRole);
                    }else{
                        var->setCheckState(Qt::Unchecked);
                        modelInventory->setItem(pos,2+j,var);
                        modelInventory->setData(modelInventory->index(pos, 2+j), Qt::AlignCenter,Qt::TextAlignmentRole);
                        modelInventory->setData(modelInventory->index(pos, 2+j), QColor(Qt::red), Qt::BackgroundRole);
                    }


                } else {
                    if (inv->access_inv_type(j).contains("string")){
                        QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                        modelInventory->setItem(pos,2+j,var);

                    }
                    if (inv->access_inv_type(j).contains("float") || inv->access_inv_type(j).contains("int")){
                        QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                        var->setData(inv->access_inv_data(j,i).toFloat(),Qt::EditRole);
                        modelInventory->setItem(pos,2+j,var);

                    }
                }
            }

            QStandardItem *var_AgeModel=new QStandardItem(QString::number(inv->get_AgeModel(i)));
            modelInventory->setItem(pos,2+inv->access_inv_length(),var_AgeModel);

            ui->tableView_4->setRowHidden(pos,!checkFilter(i));
    ui->tableView_4->repaint();

}

void DataView::selectInventory(){


    for (int i=0;i<inv->get_Entries();i++){
        int entry=modelInventory->item(i,0)->text().toInt();
        QStandardItem *var_Selected=new QStandardItem(QString::number(inv->get_Selected(entry)));
        var_Selected->setCheckable(true);
        if (inv->get_Selected(entry)) var_Selected->setCheckState(Qt::Checked);
        modelInventory->setItem(i,1,var_Selected);
        modelInventory->setData(modelInventory->index(i, 1), Qt::AlignCenter,Qt::TextAlignmentRole);
        if (inv->get_Selected(entry)){
            modelInventory->setData(modelInventory->index(i, 1), QColor(Qt::lightGray), Qt::BackgroundRole);

        } else {
            modelInventory->setData(modelInventory->index(i, 1), QColor(Qt::white), Qt::BackgroundRole);
        }
        ui->tableView_4->setRowHidden(i,!checkFilter(entry));

    }


    ui->tableView_4->repaint();
}

void DataView::entrySelected(QModelIndex mi){




    int pos=mi.row();
    int i=modelInventory->item(pos,0)->text().toInt();


    if (inv->access_inv_name(mi.column()-2).contains("flag",Qt::CaseInsensitive)){




        qDebug() << "Clicked Flag:"+QString::number(mi.column()-2)+":"+QString::number(i);
        inv->readData(i);
        if (inv->access_inv_meta_nr(mi.column()-2)!=-1){
            if(inv->access_meta_value(inv->access_inv_meta_nr(mi.column()-2))=="0") {
                inv->write_meta_value(inv->access_inv_meta_nr(mi.column()-2),"1");
                inv->write_inv_data(mi.column()-2,i,"1");
                reInventory(i,pos);

            }else{
                inv->write_meta_value(inv->access_inv_meta_nr(mi.column()-2),"0");
                inv->write_inv_data(mi.column()-2,i,"0");
                reInventory(i,pos);
            }
            inv->save();
            inv->saveData();
            createMeta();
            emit(selectionChanged());
        }




    } else {
        if(mi.column()==1){
            //qDebug() << "Clicked Selected:"+QString::number(sel);



            inv->invert_Selected(i);

            reInventory(i,pos);

            emit(selectionChanged());
        } else {

        }

    }





}

void DataView::sortVerify(){
    qDebug()<<"sortVerify";
    // get current sorting position
    int current_Section=ui->tableView_4->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder current_Order=ui->tableView_4->horizontalHeader()->sortIndicatorOrder();

    {
    const QSignalBlocker blocker(ui->tableView_4->horizontalHeader());
    ui->tableView_4->sortByColumn(0, Qt::AscendingOrder);
    ui->tableView_4->sortByColumn(current_Section, current_Order);
    }
}

void DataView::setupIsotope(){
    // List of Cores
    // get number of entries that pass filter
    int sum=0;
    if (ui->comboBox_5->currentText()=="Show All Cores")sum=inv->get_Entries();
    if (ui->comboBox_5->currentText()=="Show only Selected Cores") sum=inv->get_Selected_Sum();

    // create the model for Inventory
    //delete modelIsotopeList;
    modelIsotopeList = new QStandardItemModel(sum,3,this);
    modelIsotopeList->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelIsotopeList->setHorizontalHeaderItem(1, new QStandardItem(QString("Name of Core")));
    modelIsotopeList->setHorizontalHeaderItem(2, new QStandardItem(QString("Proxy")));

    //...
    ui->tableView->setModel(modelIsotopeList);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView->setFont(font);
    ui->tableView->horizontalHeader()->setFont(font);
    ui->tableView->verticalHeader()->setFont(font);
    //QStandardItem *var_Index = new QStandardItem[sum];
    //QStandardItem *var_Core = new QStandardItem[sum];
    //QStandardItem *var_Species = new QStandardItem[sum];

    int pos=0;
    for (int i=0;i<inv->get_Entries();i++){
        if (ui->comboBox_5->currentText()=="Show All Cores" || (ui->comboBox_5->currentText()=="Show only Selected Cores"  && inv->get_Selected(i)==1)){


            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            modelIsotopeList->setItem(pos,0,var_Index);

            QStandardItem *var_Core=new QStandardItem(inv->access_inv_data(0,i));
            modelIsotopeList->setItem(pos,1,var_Core);

            QStandardItem *var_Species=new QStandardItem(inv->access_inv_data(1,i));
            modelIsotopeList->setItem(pos,2,var_Species);



            pos++;
        }
    }
    ui->tableView->setSortingEnabled(1);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    ui->tableView->repaint();

    // create the model for Isoptopes
    //delete modelIsotope;
    modelIsotope = new QStandardItemModel(inv->get_Length(),inv->access_data_length(),this);


    for (int i=0;i<inv->access_data_length();i++) {
        QString str1="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(i)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int j=0;j<list.size();j++) if(list[j].contains("%"+inv->access_data_name(i)+"=")) if (list[j].split("=").size()>0) str1=list[j].split("=").at(1);
        } else {
            str1=inv->access_data_name(i);
        }
        if (inv->access_data_unit(i)!="") str1.append("\n["+inv->access_data_unit(i)+"]");
        modelIsotope->setHorizontalHeaderItem(i, new QStandardItem(str1));
    }


    //ui->tableView_5->setEditTriggers(QAbstractItemView::NoEditTriggers);

    font.setPointSize(resources->get_Textsize());
    ui->tableView_5->setFont(font);
    ui->tableView_5->horizontalHeader()->setFont(font);
    ui->tableView_5->verticalHeader()->setFont(font);

    for (int i=0;i<inv->get_Length();i++){
        for (int j=0;j<inv->access_data_length();j++){

            if (inv->access_data_type(j).contains("flag")){
                QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                var->setCheckable(true);
                if (inv->access_data_value(j,i).contains("1")){
                    var->setCheckState(Qt::Checked);
                    var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    modelIsotope->setItem(i,j,var);
                    modelIsotope->setData(modelIsotope->index(i, j), Qt::AlignCenter,Qt::TextAlignmentRole);
                    modelIsotope->setData(modelIsotope->index(i, j), QColor(Qt::green), Qt::BackgroundRole);
                }else{
                    var->setCheckState(Qt::Unchecked);
                    var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    modelIsotope->setItem(i,j,var);
                    modelIsotope->setData(modelIsotope->index(i, j), Qt::AlignCenter,Qt::TextAlignmentRole);
                    modelIsotope->setData(modelIsotope->index(i, j), QColor(Qt::red), Qt::BackgroundRole);
                }


            } else {
                if (inv->access_data_type(j).contains("string")){
                    QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                    if (inv->access_data_type(j).contains("edit")){
                        var->setFlags(var->flags() | Qt::ItemIsEditable);

                    } else {
                        var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    }
                    modelIsotope->setItem(i,j,var);

                }
                if (inv->access_data_type(j).contains("float") || inv->access_data_type(j).contains("int")){
                    QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                    var->setData(inv->access_data_value(j,i).toFloat(),Qt::EditRole);
                    if (inv->access_data_type(j).contains("edit")){
                        var->setFlags(var->flags() | Qt::ItemIsEditable);
                    } else {
                        var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    }
                    modelIsotope->setItem(i,j,var);

                }
            }
        }
    }
    ui->tableView_5->setModel(modelIsotope);
    ui->tableView_5->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableView_5->resizeColumnsToContents();
    ui->tableView_5->verticalHeader()->setDefaultSectionSize(ui->tableView_5->verticalHeader()->minimumSectionSize());
    ui->tableView_5->horizontalHeader()->setStretchLastSection(1);
    ui->tableView_5->repaint();

    // Set Isotope Data Attributes
    for (int i=0;i<inv->access_meta_length();i++){
        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("lineedit")){
            meta_line.at(i)->setText(inv->access_meta_value(i));
        }

        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("field")){
            meta_text.at(i)->setPlainText(inv->access_meta_value(i));
        }

        if (inv->access_meta_type(i).contains("float")){
            meta_float.at(i)->setValue(inv->access_meta_value(i).toFloat());
        }

        if (inv->access_meta_type(i).contains("combo")){
            if (meta_combo.at(i)->findText(inv->access_meta_value(i))>0) {
                meta_combo.at(i)->setCurrentText(inv->access_meta_value(i));
            } else {
                meta_combo.at(i)->setCurrentIndex(0);
            }
        }

        if (inv->access_meta_type(i).contains("flag")){
            if (inv->access_meta_value(i)=="1") {
                meta_flag.at(i)->setCheckState(Qt::Checked);
            } else {
                meta_flag.at(i)->setCheckState(Qt::Unchecked);
            }
        }
    }




    setupAge();
}

void DataView::setupAge(){
    // Read AgeModel
    amsdata->AMSRead();
    // create the model for AMSData
    modelAge = new QStandardItemModel(amsdata->get_Length(),14,this);
    modelAge->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelAge->setHorizontalHeaderItem(1, new QStandardItem(QString("Depth")));
    modelAge->setHorizontalHeaderItem(2, new QStandardItem(QString("Sample\nThickness")));
    modelAge->setHorizontalHeaderItem(3, new QStandardItem(QString("Label")));
    modelAge->setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    modelAge->setHorizontalHeaderItem(5, new QStandardItem(QString("Age dated\n[ka]")));
    modelAge->setHorizontalHeaderItem(6, new QStandardItem(QString("Age UCL\n[ka+]")));
    modelAge->setHorizontalHeaderItem(7, new QStandardItem(QString("Age LCL\n[ka-]")));
    modelAge->setHorizontalHeaderItem(8, new QStandardItem(QString("Res. Age\n[ka]")));
    modelAge->setHorizontalHeaderItem(9, new QStandardItem(QString("Res. Age\n Error\n[ka]")));
    modelAge->setHorizontalHeaderItem(10, new QStandardItem(QString("Cal yrs\n[wm ka BP]")));
    modelAge->setHorizontalHeaderItem(11, new QStandardItem(QString("Cal yrs min\n[95%]")));
    modelAge->setHorizontalHeaderItem(12, new QStandardItem(QString("Cal yrs max\n[95%]")));
    modelAge->setHorizontalHeaderItem(13, new QStandardItem(QString("Use Flag")));
    modelAge->setHorizontalHeaderItem(14, new QStandardItem(QString("Comments")));
    ui->tableView_2->setModel(modelAge);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView_2->setFont(font);
    ui->tableView_2->horizontalHeader()->setFont(font);
    ui->tableView_2->verticalHeader()->setFont(font);


    for (int i=0;i<amsdata->get_Length();i++){
        QStandardItem *var_Index=new QStandardItem(QString::number(i));
        modelAge->setItem(i,0,var_Index);

        QStandardItem *var_Depth=new QStandardItem(QString::number(amsdata->get_Depth(i)));
        modelAge->setItem(i,1,var_Depth);

        QStandardItem *var_Sample=new QStandardItem(QString::number(amsdata->get_Sample_Thickness(i)));
        modelAge->setItem(i,2,var_Sample);

        QStandardItem *var_Label=new QStandardItem(amsdata->get_LabID(i));
        modelAge->setItem(i,3,var_Label);

        QStandardItem *var_Type=new QStandardItem(amsdata->get_Type(i));
        modelAge->setItem(i,4,var_Type);

        QStandardItem *var_Age_dated=new QStandardItem(QString::number(amsdata->get_Data(0,i)));
        modelAge->setItem(i,5,var_Age_dated);

        QStandardItem *var_Age_UCL=new QStandardItem(QString::number(amsdata->get_Data(1,i)));
        modelAge->setItem(i,6,var_Age_UCL);

        QStandardItem *var_Age_LCL=new QStandardItem(QString::number(amsdata->get_Data(2,i)));
        modelAge->setItem(i,7,var_Age_LCL);

        QStandardItem *var_Age_Res=new QStandardItem(QString::number(amsdata->get_Data(3,i)));
        modelAge->setItem(i,8,var_Age_Res);

        QStandardItem *var_Age_Res_Err=new QStandardItem(QString::number(amsdata->get_Reservoir_Error(i)));
        modelAge->setItem(i,9,var_Age_Res_Err);

        QStandardItem *var_Cal=new QStandardItem(QString::number(amsdata->get_Data(4,i)));
        modelAge->setItem(i,10,var_Cal);

        QStandardItem *var_Cal_Min=new QStandardItem(QString::number(amsdata->get_Data(5,i)));
        modelAge->setItem(i,11,var_Cal_Min);

        QStandardItem *var_Cal_Max=new QStandardItem(QString::number(amsdata->get_Data(6,i)));
        modelAge->setItem(i,12,var_Cal_Max);

        QStandardItem *var_Use_Flag=new QStandardItem(QString::number(amsdata->get_Data(7,i)));
        //var_Use_Flag->setCheckable(true);
        //if (amsdata->get_Data(7,i)) var_Use_Flag->setCheckState(Qt::Checked);
        modelAge->setItem(i,13,var_Use_Flag);
        modelAge->setData(modelAge->index(i, 13), Qt::AlignCenter,Qt::TextAlignmentRole);

        QStandardItem *var_Comment=new QStandardItem(amsdata->get_Age_Comment(i));
        modelAge->setItem(i,14,var_Comment);




        // coloring
        if (amsdata->get_Data(7,i)){
            modelAge->setData(modelAge->index(i, 13), QColor(Qt::green), Qt::BackgroundRole);
        } else {
            modelAge->setData(modelAge->index(i, 13), QColor(Qt::red), Qt::BackgroundRole);
        }


    }
    ui->tableView_2->setSortingEnabled(0);
    //ui->tableView_2->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setHorizontalScrollMode(ui->tableView_2->ScrollPerPixel);
    //ui->tableView_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_2->scrollTo(modelAge->index(amsdata->getSelect(),0));
    ui->tableView_2->repaint();
}


void DataView::select(){


    for (int i=0;i<inv->get_Entries();i++){
        if (ui->tableView_4->isRowHidden(i)==false) inv->set_Selected(modelInventory->item(i,0)->text().toInt(),1);

    }
selectInventory();
    emit(selectionChanged());
}

void DataView::deselect(){


    for (int i=0;i<inv->get_Entries();i++){
        if (ui->tableView_4->isRowHidden(i)==false) inv->set_Selected(modelInventory->item(i,0)->text().toInt(),0);

    }
selectInventory();
    emit(selectionChanged());
}

void DataView::invert(){


    for (int i=0;i<inv->get_Entries();i++){
        if (ui->tableView_4->isRowHidden(i)==false) inv->invert_Selected(modelInventory->item(i,0)->text().toInt());

    }
selectInventory();

    emit(selectionChanged());
}

void DataView::apply(){

    for (int i=0;i<inv->get_Entries();i++) ui->tableView_4->setRowHidden(i,!checkFilter(modelInventory->item(i,0)->text().toInt()));

}

void DataView::reset(){

    for (int i=0;i<inv->access_inv_length();i++){

        if (inv->access_inv_type(i).contains("search")){
            lineedit.at(i)->setText("");
        }


        if (inv->access_inv_type(i).contains("combo")){
           combobox.at(i)->setCurrentIndex(0);
        }

        if (inv->access_inv_type(i).contains("float") && inv->access_inv_type(i).contains("minmax")){

            QStringList list=inv->access_inv_type(i).split(";");
            for (int j=0;j<list.size();j++){
                if (list[j].contains("min=")) {
                    QStringList sublist=list[j].split("=");
                    min_float.at(i)->setMinimum(sublist[1].toFloat());
                    max_float.at(i)->setMinimum(sublist[1].toFloat());
                    min_float.at(i)->setValue(sublist[1].toFloat());
                }
                if (list[j].contains("max=")) {
                    QStringList sublist=list[j].split("=");
                    min_float.at(i)->setMaximum(sublist[1].toFloat());
                    max_float.at(i)->setMaximum(sublist[1].toFloat());
                    max_float.at(i)->setValue(sublist[1].toFloat());
                }
            }


            if (inv->access_inv_type(i).contains("invert")){
                minmax_invert.at(i)->setCheckState(Qt::Unchecked);
            }
        }
    }
    update();

    apply();

}

bool DataView::checkFilter(int entry){
    bool ok=true;

    for (int i=0;i<inv->access_inv_length();i++){
        if (inv->access_inv_type(i).contains("string")){
            QString str=lineedit.at(i)->text();
            if (!inv->access_inv_data(i,entry).contains(str)) ok=false;
        }

        if (inv->access_inv_type(i).contains("combo")){
            QString str=combobox.at(i)->currentText();
            if (!inv->access_inv_data(i,entry).contains(str) && combobox.at(i)->currentIndex()!=0) ok=false;
        }

        if (inv->access_inv_type(i).contains("float") && inv->access_inv_type(i).contains("minmax")){
            float min=min_float.at(i)->value();
            float max=max_float.at(i)->value();
            bool invert=false;
            if (inv->access_inv_type(i).contains("invert")){
                if (minmax_invert.at(i)->isChecked()) {
                    invert=true;
                }
            }
            if ((inv->access_inv_data(i,entry).toFloat()<min || inv->access_inv_data(i,entry).toFloat()>max) && invert==false){
                ok=false;
            }
            if ((inv->access_inv_data(i,entry).toFloat()>min && inv->access_inv_data(i,entry).toFloat()<max) && invert==true){
                ok=false;
            }
        }

    }
    return ok;
}

void DataView::tabSelect(int n){
    //qDebug()<< "Tab Selected:"+QString::number(n);
    //if (n==0) setupInventory();
    if (n==1) setupIsotope();

}

void DataView::applyIsotope(int n){
    setupIsotope();
}


void DataView::isotopeSelected(QModelIndex mi){

        int sel=mi.row();
        //qDebug() << "Clicked :"+QString::number(sel);
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        QString text = model->item(sel,0)->text();
        inv->readData(text.toInt(0,10));
        setupIsotope();
}

void DataView::browse(){
    QString file = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             QDir::homePath(),
                                             tr("PDF (*.pdf)"));
    //qDebug() << file;
    inv->set_att_EPaper(file);
    setupIsotope();
}

void DataView::open(){
    //qDebug() << "Open: "+inv->get_att_EPaper();
    QDesktopServices::openUrl(QUrl("file:///"+inv->get_att_EPaper(), QUrl::TolerantMode));
}

void DataView::refresh(){
    setupIsotope();
    update();
}

void DataView::applyData(int i){
    qDebug()<<"Write to Memory";
    if (inv->get_currentCore()>=0) {

    inv->readData(inv->get_currentCore());
    for(int i=0;i<inv->access_meta_length();i++){
        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("lineedit")){
            inv->write_meta_value(i,meta_line.at(i)->text());
        }

        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("field")){
            inv->write_meta_value(i,meta_text.at(i)->toPlainText());
        }

        if (inv->access_meta_type(i).contains("float")){
            inv->write_meta_value(i,QString::number(meta_float.at(i)->value()));
        }

        if (inv->access_meta_type(i).contains("combo")){
            inv->write_meta_value(i,meta_combo.at(i)->currentText());
        }

        if (inv->access_meta_type(i).contains("flag")){
            if (meta_flag.at(i)->isChecked()) inv->write_meta_value(i,"1"); else inv->write_meta_value(i,"0");
        }
    }

/*
    for (int j=0;j<inv->get_Length();j++){
        for (int i=0;i<inv->access_data_length();i++){
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_5->model());
            QString text = model->item(j,i)->text();
            inv->write_data_value(i,j,text);
        }
    }*/
    setupIsotope();
    emit(selectionChanged());
    }
}

void DataView::save(){
    if (inv->get_currentCore()>=0) {

    inv->readData(inv->get_currentCore());
    for(int i=0;i<inv->access_meta_length();i++){
        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("lineedit")){
            inv->write_meta_value(i,meta_line.at(i)->text());
        }

        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("field")){
            inv->write_meta_value(i,meta_text.at(i)->toPlainText());
        }

        if (inv->access_meta_type(i).contains("float")){
            inv->write_meta_value(i,QString::number(meta_float.at(i)->value()));
        }

        if (inv->access_meta_type(i).contains("combo")){
            inv->write_meta_value(i,meta_combo.at(i)->currentText());
        }

        if (inv->access_meta_type(i).contains("flag")){
            if (meta_flag.at(i)->isChecked()) inv->write_meta_value(i,"1"); else inv->write_meta_value(i,"0");
        }
    }


    for (int j=0;j<inv->get_Length();j++){
        for (int i=0;i<inv->access_data_length();i++){
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_5->model());
            QString text = model->item(j,i)->text();
            inv->write_data_value(i,j,text);
        }
    }

    inv->save();
    inv->saveData();
    // correct inventory entry
    //find position in table
    int pos=-1;
    for(int i=0;i<inv->get_Entries();i++) {
        if (modelInventory->item(i,0)->text().toInt()==inv->get_currentCore()) {
            pos=i;
            break;
        }
    }
    if (pos>-1) reInventory(inv->get_currentCore(),pos);
    setupIsotope();
    emit(selectionChanged());
    QMessageBox msgBox;
    msgBox.setText("Changes where saved to proxy file.");
    msgBox.exec();
    }
}



void DataView::attChanged(QString text,QString origin){
        int sel=origin.toInt(0,10);
        //qDebug() << "Text:"+text;
        inv->set_data_Comment(text,sel);
        inv->saveData();
        setupIsotope();
}

void DataView::invertFlag(QModelIndex mi){

        int sel=mi.row();
        if (inv->access_data_type(mi.column()).contains("flag")){
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_5->model());

            if (model->item(mi.row(),mi.column())->checkState()==Qt::Checked){
                qDebug()<<"click";
                QStandardItem *var=new QStandardItem("1");
                var->setCheckable(true);
                var->setCheckState(Qt::Checked);
                var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                modelIsotope->setItem(mi.row(),mi.column(),var);
                modelIsotope->setData(modelIsotope->index(mi.row(), mi.column()), Qt::AlignCenter,Qt::TextAlignmentRole);
                modelIsotope->setData(modelIsotope->index(mi.row(), mi.column()), QColor(Qt::green), Qt::BackgroundRole);
                //ui->tableView_5->scrollTo(mi.model()->index(mi.row(),mi.column()),QAbstractItemView::PositionAtCenter);

            }else {
                QStandardItem *var=new QStandardItem("0");
                var->setCheckable(true);
                var->setCheckState(Qt::Unchecked);
                var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                modelIsotope->setItem(mi.row(),mi.column(),var);
                modelIsotope->setData(modelIsotope->index(mi.row(), mi.column()), Qt::AlignCenter,Qt::TextAlignmentRole);
                modelIsotope->setData(modelIsotope->index(mi.row(), mi.column()), QColor(Qt::red), Qt::BackgroundRole);

                //ui->tableView_5->scrollTo(mi.model()->index(mi.row(),mi.column()),QAbstractItemView::PositionAtCenter);
            }


        }


}


void DataView::newBasin(){
    //ui->label_26->setText(inv->get_Basinname(inv->get_Basin(ui->doubleSpinBox_5->value(),ui->doubleSpinBox_6->value())));

}

bool DataView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->tableView||obj==ui->tableView_4||obj==ui->tableView_5||obj==ui->tabWidget||obj==ui->tab_3||obj==ui->tab_4){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);
                ui->splitter_2->restoreState(sp_2);
                ui->splitter_3->restoreState(sp_3);
                ui->splitter_4->restoreState(sp_4);
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}



void DataView::reject()
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

void DataView::copy_4(QPoint pos){

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

void DataView::copy(QPoint pos){

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

void DataView::copy_5(QPoint pos){

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

void DataView::copy_2(QPoint pos){

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

void DataView::createFilter(){
    QVBoxLayout *vbox = new QVBoxLayout;
    lab.clear();
    lab.reserve(inv->access_inv_length());
    lineedit.clear();
    lineedit.reserve(inv->access_inv_length());
    min_float.clear();
    min_float.reserve(inv->access_inv_length());

    for (int i=0;i<inv->access_inv_length();i++){
        QString str=inv->access_inv_name(i);
        if (inv->access_inv_unit(i)!="") str.append(+" in ["+inv->access_inv_unit(i)+"]");
        str.append(" :");
        lab.append(new QLabel(str));
        vbox->addWidget(lab.at(i));

        lineedit.append(new QLineEdit());
        if (inv->access_inv_type(i).contains("search")){
            vbox->addWidget(lineedit.at(i));
        }

        combobox.append(new QComboBox());
        if (inv->access_inv_type(i).contains("combo")){
            QStringList list=inv->access_inv_type(i).split(";");
            for (int j=0;j<list.size();j++){
                if (list[j].contains("item=")) {
                    QStringList sublist=list[j].split("=");
                    combobox.at(i)->addItem(sublist[1]);
                }
            }
            vbox->addWidget(combobox.at(i));
        }

        min_float.append(new QDoubleSpinBox());
        max_float.append(new QDoubleSpinBox());
        minmax_invert.append(new QCheckBox("invert"));

        if (inv->access_inv_type(i).contains("float") && inv->access_inv_type(i).contains("minmax")){
            QHBoxLayout *hbox=new QHBoxLayout;
            QStringList list=inv->access_inv_type(i).split(";");
            for (int j=0;j<list.size();j++){
                if (list[j].contains("min=")) {
                    QStringList sublist=list[j].split("=");
                    min_float.at(i)->setMinimum(sublist[1].toFloat());
                    max_float.at(i)->setMinimum(sublist[1].toFloat());
                    min_float.at(i)->setValue(sublist[1].toFloat());
                }
                if (list[j].contains("max=")) {
                    QStringList sublist=list[j].split("=");
                    min_float.at(i)->setMaximum(sublist[1].toFloat());
                    max_float.at(i)->setMaximum(sublist[1].toFloat());
                    max_float.at(i)->setValue(sublist[1].toFloat());
                }
            }

            hbox->addWidget(min_float.at(i));
            hbox->addWidget(max_float.at(i));
            if (inv->access_inv_type(i).contains("invert")){

                hbox->addWidget(minmax_invert.at(i));

            }
            hbox->addStretch(1);
            vbox->addLayout(hbox);
        }
    }

    button_apply=new QPushButton("Apply Filter");
    vbox->addWidget(button_apply);
    button_reset=new QPushButton("Reset Filter");
    vbox->addWidget(button_reset);

    vbox->addStretch(1);
    ui->groupBox_3->setLayout(vbox);
    update();
}

void DataView::createMeta(){
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(12);
    QVBoxLayout *vbox = new QVBoxLayout;
    meta_lab.clear();
    meta_lab.reserve(inv->access_meta_length());

    for (int i=0;i<inv->access_meta_length();i++){
        QHBoxLayout *hbox=new QHBoxLayout;

        QString str=inv->access_meta_name(i);
        if (inv->access_meta_unit(i)!="") str.append(+" in ["+inv->access_meta_unit(i)+"]");
        while(str.length()<25) str.append(".");
        str.append(":");
        meta_lab.append(new QLabel(str));
        meta_lab.at(i)->setFont(font);
        meta_lab.at(i)->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        hbox->addWidget(meta_lab.at(i));

        meta_line.append(new QLineEdit());
        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("lineedit")){
            if (inv->access_meta_type(i).contains("read-only")) {
                meta_line.at(i)->setReadOnly(true);
                meta_line.at(i)->setEnabled(false);
            }
            meta_line.at(i)->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
            hbox->addWidget(meta_line.at(i));
        }

        meta_text.append(new QPlainTextEdit());
        if (inv->access_meta_type(i).contains("string") && inv->access_meta_type(i).contains("field")){
            if (inv->access_meta_type(i).contains("read-only")) {
                meta_text.at(i)->setReadOnly(true);
                meta_text.at(i)->setEnabled(false);
            }
            meta_text.at(i)->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
            hbox->addWidget(meta_text.at(i));
        }

        meta_float.append(new QDoubleSpinBox());
        if (inv->access_meta_type(i).contains("float")){
            if (inv->access_meta_type(i).contains("read-only")){
                meta_float.at(i)->setReadOnly(true);
                meta_float.at(i)->setEnabled(false);
            }
            if (inv->access_meta_type(i).contains("min") || inv->access_meta_type(i).contains("max")){
                QStringList list=inv->access_meta_type(i).split(";");
                for (int j=0;j<list.size();j++){
                    if (list[j].contains("min=")) {
                        QStringList sublist=list[j].split("=");
                        meta_float.at(i)->setMinimum(sublist[1].toFloat());
                    }
                    if (list[j].contains("max=")) {
                        QStringList sublist=list[j].split("=");
                        meta_float.at(i)->setMaximum(sublist[1].toFloat());
                    }
                }
            } else {
                meta_float.at(i)->setMinimum(-9e99);
            }   meta_float.at(i)->setMaximum(9e99);
            meta_float.at(i)->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
            hbox->addWidget(meta_float.at(i));
        }

        meta_combo.append(new QComboBox());
        if (inv->access_meta_type(i).contains("combo")){
            QStringList list=inv->access_meta_type(i).split(";");
            for (int j=0;j<list.size();j++){
                if (list[j].contains("item=")) {
                    QStringList sublist=list[j].split("=");
                    meta_combo.at(i)->addItem(sublist[1]);
                }
            }
            meta_combo.at(i)->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
            hbox->addWidget(meta_combo.at(i));
        }

        meta_flag.append(new QCheckBox());
        if (inv->access_meta_type(i).contains("flag")){
            if (inv->access_meta_type(i).contains("read-only")) {
                meta_flag.at(i)->setEnabled(false);
            }
            meta_flag.at(i)->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
            hbox->addWidget(meta_flag.at(i));
        }


        meta_browse.append(new QPushButton("Browse"));
        if (inv->access_meta_type(i).contains("browse")){
             hbox->addWidget(meta_browse.at(i));
             connect(meta_browse.at(i),SIGNAL(clicked(bool)),this,SLOT(browse()));
        }

        meta_open.append(new QPushButton("Open"));
        if (inv->access_meta_type(i).contains("open")){
             hbox->addWidget(meta_open.at(i));
             connect(meta_open.at(i),SIGNAL(clicked(bool)),this,SLOT(open()));
        }


        hbox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        vbox->addLayout(hbox);

    }

    vbox->addStretch(1);

    ui->groupBox->setLayout(vbox);

    update();
}




