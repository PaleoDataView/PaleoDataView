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

#include "extract.h"
#include "ui_extract.h"

Extract::Extract(QMainWindow *mainWindow,Inventory *inventory,QString str,QImage* wmap) :
    mainW(mainWindow),inv_main(inventory),proxy(str),worldmap(wmap),
    ui(new Ui::Extract)
{
    ui->setupUi(this);
     this->setWindowTitle("Equidistant time series ensemble");
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    sp_4=ui->splitter_4->saveState();


    qApp->installEventFilter(this);
    ui->pushButton->setFocusPolicy(Qt::NoFocus);
    resources=new Resources();

    inv=new Inventory(proxy);
    inv->read();
    inv->read_Basin();
    for(int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
    for(int i=0;i<inv->get_Entries();i++) inv->set_AgeModel(i,inv_main->get_AgeModel(i));
    colorScheme=new ColorScheme();

    AMSSummery();

    data=new float[0];
    coll=new QColor[0];
    lstyle=new Qt::PenStyle[0];
    pl1=new Graph(this,data,0,0);
    pl2=new Graph(this,data,0,0);
    ui->graphicsView->setScene(pl1);
    ui->graphicsView_3->setScene(pl2);

    mapmode=0;
    mapdata=new float[0];
    com=new QString[0];
    lab=new QString[0];
    dotcol=new QColor[0];
    dotsize=new int[0];
    map=new Graph(this,data,0,0);
    map->setColorScheme(0,0);
    ui->graphicsView_2->setScene(map);








    size=0;
    entries=0;
    selected_entry=-1;
    start=ui->doubleSpinBox->value();
    inc=ui->doubleSpinBox_4->value();
    end=ui->doubleSpinBox_2->value();
    ui->horizontalSlider->setMinimum(0);

    ui->horizontalSlider->setMaximum(size-1);
    ui->doubleSpinBox_3->setMaximum(end-inc);
    ui->doubleSpinBox_3->setMinimum(start);
    ui->doubleSpinBox_3->setSingleStep(inc);
    ui->horizontalSlider->setValue(0);


    // prepare drop box for dataselection
    ui->comboBox_3->clear();
    for (int i=0;i<inv->access_data_length();i++){
        QString str=inv->access_data_name(i);
        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
        ui->comboBox_3->addItem(str);
        if (inv->access_data_name(i)=="Sample Thickness" || inv->access_data_name(i)=="Age"|| inv->access_data_name(i)=="Use Flag"||inv->access_data_name(i).contains("Error")||inv->access_data_name(i).contains("Comment")){
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


    result_core.clear();
    result_species.clear();
    result_lat.clear();
    result_long.clear();
    result_wd.clear();
    result_min.clear();
    result_mid.clear();
    result_max.clear();
    result_skew.clear();

    result_age.clear();

    save_OK=0;


    modelInventory = new QStandardItemModel(1,1,this);
    modelList = new QStandardItemModel(1,1,this);
    setupInventory();
    setupExtractTable();
    setButtons();

    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(invSelected(QModelIndex)));
    connect(ui->tableView_2,SIGNAL(clicked(QModelIndex)),this,SLOT(listSelected(QModelIndex)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));
    connect(ui->comboBox_3,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));

    connect(ui->doubleSpinBox,SIGNAL(editingFinished()),this,SLOT(setup()));
    connect(ui->doubleSpinBox_2,SIGNAL(editingFinished()),this,SLOT(setup()));
    connect(ui->doubleSpinBox_4,SIGNAL(editingFinished()),this,SLOT(setup()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(calc()));
    //connect(ui->radioButton,SIGNAL(clicked(bool)),this,SLOT(setup()));
    connect(this,SIGNAL(selectionChanged()),mainW,SLOT(redraw_score()));// refresh main window
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(read()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(pl1,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(pl2,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
     connect(map,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    ui->tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy2(QPoint)));
    connect(map,SIGNAL(selected(int,int,int)),this,SLOT(selected_changed(int,int,int)));
    connect(ui->horizontalSlider,SIGNAL(sliderMoved(int)),this,SLOT(sMoved(int)));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sPressed(int)));
    connect(ui->doubleSpinBox_3,SIGNAL(valueChanged(double)),this,SLOT(sValue(double)));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(sTab(int)));
    connect(ui->checkBox,SIGNAL(stateChanged(int)),this,SLOT(sChanged2(int)));
    connect(ui->checkBox_2,SIGNAL(stateChanged(int)),this,SLOT(sChanged(int)));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(deselectAll()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(MapToWorld()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(MapToATL()));
    connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(MapToPAC()));
    connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(MapToIND()));
    connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(MapToMED()));
    connect(ui->comboBox_4,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(sortVerify()));

}

Extract::~Extract()
{
    delete ui;
    delete resources;
    delete modelInventory;
    delete modelList;
    delete[] data;
    delete[] coll;
    delete[] lstyle;

}



void Extract::sMoved(int v){
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_3);
    if (size>0&&result_age.at(v)>=start&&result_age.at(v)<end) ui->doubleSpinBox_3->setValue(result_age.at(v));
    plot();
    createmap(0);
    }
}

void Extract::sPressed(int v){
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_3);
    if (size>0&&result_age.at(v)>=start&&result_age.at(v)<end) ui->doubleSpinBox_3->setValue(result_age.at(v));
    plot();
    createmap(0);
    }
}

void Extract::sValue(double d){
    {
    const QSignalBlocker blocker(ui->horizontalSlider);
    int v=(d-start)/inc;
    if (v>=0 && v<=ui->horizontalSlider->maximum()) ui->horizontalSlider->setValue(v);
    plot();
    createmap(0);
    }
}

void Extract::sortVerify(){
    qDebug()<<"sortVerify";
    // get current sorting position
    int current_Section=ui->tableView->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder current_Order=ui->tableView->horizontalHeader()->sortIndicatorOrder();

    {
    const QSignalBlocker blocker(ui->tableView->horizontalHeader());
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    ui->tableView->sortByColumn(current_Section, current_Order);
    }
}

void Extract::sTab(int i){
    if (i==1){
        plot();
        createmap(0);
        setupExtractTable();
        repaint();
    }
    if (i==1){
        plot();

       setupInventory();
        repaint();
    }

}

void Extract::sChanged(int state){
    setupInventory();
}

void Extract::sChanged2(int state){
    createmap(0);
}

void Extract::selectAll(){
    for (int i=0;i<inv->get_Entries();i++){
        if (checkFilter(i)==true) inv->set_Selected(i,1);
    }
    setupInventory();
}

void Extract::deselectAll(){
    for (int i=0;i<inv->get_Entries();i++)inv->set_Selected(i,0);
    setupInventory();
}

void Extract::setupInventory(){
    qDebug()<<"setiing up inventory";

        // get number of entries that pass filter
        bool* filter=new bool[inv->get_Entries()];
        int sum=0;
        for (int i=0;i<inv->get_Entries();i++){
            if (checkFilter(i)){
                filter[i]=true;
                sum++;
            } else {
                filter[i]=false;
            }
        }

        // create the model for Inventory
        delete modelInventory;
        modelInventory = new QStandardItemModel(sum,5+inv->access_inv_length(),this);
        modelInventory->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
        modelInventory->setHorizontalHeaderItem(1, new QStandardItem(QString("Selected")));
        for (int i=0;i<inv->access_inv_length();i++){
            modelInventory->setHorizontalHeaderItem(2+i, new QStandardItem(inv->access_inv_name(i)+"\n"+inv->access_inv_unit(i)));
        }
        modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length(), new QStandardItem(QString("Age Min\n[ka]")));
        modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length()+1, new QStandardItem(QString("Age Max\n[ka]")));
        modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length()+2, new QStandardItem(QString("Bacon\nIterations")));


        //...
        ui->tableView->setModel(modelInventory);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        QFont font;
        font.setPointSize(resources->get_Textsize());
        ui->tableView->setFont(font);
        ui->tableView->horizontalHeader()->setFont(font);
        ui->tableView->verticalHeader()->setFont(font);


        int pos=0;



        for (int i=0;i<inv->get_Entries();i++){
            if (filter[i]){






                QStandardItem *var_Index=new QStandardItem(QString::number(i));
                var_Index->setData(i,Qt::EditRole);
                modelInventory->setItem(pos,0,var_Index);


                QStandardItem *var_Selected=new QStandardItem("");
                var_Selected->setCheckable(true);
                if (inv->get_Selected(i)==1) {
                    var_Selected->setCheckState(Qt::Checked);
                }
                modelInventory->setItem(pos,1,var_Selected);
                modelInventory->setData(modelInventory->index(pos, 1), Qt::AlignCenter,Qt::TextAlignmentRole);

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
                            if (inv->get_Selected(i)){
                                //modelInventory->setData(modelInventory->index(pos, 2+j), QColor(Qt::lightGray), Qt::BackgroundRole);
                            }
                        }
                        if (inv->access_inv_type(j).contains("float") || inv->access_inv_type(j).contains("int")){
                            QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                            var->setData(inv->access_inv_data(j,i).toFloat(),Qt::EditRole);
                            modelInventory->setItem(pos,2+j,var);
                            if (inv->get_Selected(i)){
                               // modelInventory->setData(modelInventory->index(pos, 2+j), QColor(Qt::lightGray), Qt::BackgroundRole);
                            }
                        }
                    }
                }

                inv->set_currentCore(i);


                QStandardItem *var_AgeMin=new QStandardItem(QString::number(ams_min_age[i]));
                var_AgeMin->setData(ams_min_age[i],Qt::EditRole);
                modelInventory->setItem(pos,2+inv->access_inv_length(),var_AgeMin);
                QStandardItem *var_AgeMax=new QStandardItem(QString::number(ams_max_age[i]));
                var_AgeMax->setData(ams_max_age[i],Qt::EditRole);
                modelInventory->setItem(pos,2+inv->access_inv_length()+1,var_AgeMax);

                QStandardItem *var_Bacon=new QStandardItem(QString::number(ams_bacon_iter[i]));
                var_Bacon->setData(ams_bacon_iter[i],Qt::EditRole);
                modelInventory->setItem(pos,2+inv->access_inv_length()+2,var_Bacon);





                // coloring
                if (inv->get_Selected(i)){
                    for (int j=0;j<inv->access_inv_length()+2+3;j++) if (!inv->access_inv_name(j-2).contains("flag",Qt::CaseInsensitive)) modelInventory->setData(modelInventory->index(pos, j), QColor(Qt::lightGray), Qt::BackgroundRole);
                }
                if (inv->get_Selected(i)){
                    for (int j=1;j<2;j++) {
                        if (ui->doubleSpinBox->value()>=ams_min_age[i] && ui->doubleSpinBox_2->value()<=ams_max_age[i]){
                            modelInventory->setData(modelInventory->index(pos, j), QColor(Qt::green), Qt::BackgroundRole);
                        }
                        if (ui->doubleSpinBox_2->value()<ams_min_age[i] || ui->doubleSpinBox->value()>ams_max_age[i]){
                            modelInventory->setData(modelInventory->index(pos, j), QColor(Qt::red), Qt::BackgroundRole);
                        }
                        if ((ui->doubleSpinBox->value()<ams_min_age[i] && ui->doubleSpinBox_2->value()>ams_min_age[i]) || (ui->doubleSpinBox->value()<ams_max_age[i] && ui->doubleSpinBox_2->value()>ams_max_age[i]) ){
                            modelInventory->setData(modelInventory->index(pos, j), QColor(Qt::yellow), Qt::BackgroundRole);
                        }
                        if (isnan(ams_min_age[i]) || isnan(ams_max_age[i]) || inv->get_AgeModel(i)==0){
                            modelInventory->setData(modelInventory->index(pos, j), QColor(Qt::red), Qt::BackgroundRole);
                        }

                    }
                }

                pos++;

            }
        }



        delete[] filter;

        ui->tableView->setSortingEnabled(0);
        ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);





    ui->tableView->repaint();
    qDebug()<<"finshed";
}

void Extract::invSelected(QModelIndex mi){

    int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    QString text = model->item(sel,mi.column())->text();
    //qDebug() << text;
    int index=model->item(sel,0)->text().toInt();

    if (mi.column()==1){
        if (inv->get_Selected(index)==1){
            inv->set_Selected(index,0);
        } else {
            inv->set_Selected(index,1);
        }

    }
    inv->set_currentCore(index);
    selected_entry=-1;
    int found=0;
    qDebug()<<inv->get_Core(index).simplified()+" "+inv->get_Species(index).simplified();
    for (int j=0;j<entries;j++){
        qDebug()<<result_core.at(j).toLatin1().simplified()+" "+result_species.at(j).toLatin1().simplified();
        if (inv->get_Core(index).simplified()==result_core.at(j).toLatin1().simplified() && inv->get_Species(index).simplified()==result_species.at(j).toLatin1().simplified()){


            found=1;
            selected_entry=j;
            break;
        }
    }
    if (found==0&&entries>0){
        selected_entry=-1;
        QString Core=inv->get_Core(inv->get_currentCore()).toLatin1();
        QString Species=inv->get_Species(inv->get_currentCore()).toLatin1();
        ui->plainTextEdit->appendPlainText("Core not part of current file: "+Core+" : "+Species);
        repaint();
    }
    setupInventory();
    plot();
    //createmap(1);
    //setupExtractTable();



}

void Extract::listSelected(QModelIndex mi){

    int sel=mi.row();
    //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    QString text = model->item(sel,mi.column())->text();
    //qDebug() << text;
    int index=model->item(sel,0)->text().toInt();

    selected_entry=index;
    qDebug()<<"creating plot";
    plot();
    qDebug()<<"creating map";
    createmap(1);
    qDebug()<<"creating list";
    setupExtractTable();
    qDebug()<<"creating table";
    setupInventory();


}


bool Extract::checkFilter(int i){
    qDebug()<<"checkFilter";
    bool ok=true;

    // check Ocean
    if (inv->get_Basinname(inv->get_Longitude(i),inv->get_Latitude(i))!=ui->comboBox_2->currentText() && ui->comboBox_2->currentText()!="Global") ok=false;

    // Selected
    if (inv->get_Selected(i)==0 && ui->comboBox->currentText()=="Selected Only") ok=false;

    //Bacon
    if (ui->checkBox_2->isChecked()){
        inv->set_currentCore(i);

        if (ams_bacon_iter[i]==0 || isnan(ams_bacon_iter[i]))ok=false;

    }

    qDebug()<<"finished";

    return ok;
}

void Extract::setup(){
    // Erase everything(should be locked after calculation or reading from file)
    qDebug()<<"deleting results";
    data_index=ui->comboBox_3->currentIndex();
    result_core.clear();
    result_species.clear();
    result_lat.clear();
    result_long.clear();
    result_wd.clear();
    result_min.clear();
    result_mid.clear();
    result_max.clear();
    result_skew.clear();

    result_age.clear();
    size=0;
    entries=0;
    selected_entry=-1;
    {
    const QSignalBlocker blocker(ui->horizontalSlider);

    start=ui->doubleSpinBox->value();
    inc=ui->doubleSpinBox_4->value();
    end=ui->doubleSpinBox_2->value();
    int count=(end-start)/inc;
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(count-1);
    ui->horizontalSlider->setValue(0);
    }
    setupInventory();
    setupExtractTable();

}

void Extract::calc(){
    // setup results(again)

    start=ui->doubleSpinBox->value();
    inc=ui->doubleSpinBox_4->value();
    end=ui->doubleSpinBox_2->value();

    int count=(end-start)/inc;

    result_core.clear();
    result_species.clear();
    result_lat.clear();
    result_long.clear();
    result_wd.clear();
    result_min.clear();
    result_mid.clear();
    result_max.clear();
    result_skew.clear();

    result_age.clear();
    for (int j=0;j<count;j++) result_age.push_back(ui->doubleSpinBox->value()+j*ui->doubleSpinBox_4->value());
    size=count;
    entries=0;
    selected_entry=-1;

    {
    const QSignalBlocker blocker(ui->horizontalSlider);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(size-1);
    ui->horizontalSlider->setValue(0);
    }
    {
    const QSignalBlocker blocker(ui->doubleSpinBox_3);
    ui->doubleSpinBox_3->setMaximum(end-inc);
    ui->doubleSpinBox_3->setMinimum(start);
    ui->doubleSpinBox_3->setSingleStep(inc);
    ui->doubleSpinBox_3->setValue(start);
    }



    QProgressDialog progress("Processing Cores...","Cancel",0,inv->get_Selected_Sum()+1,this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(0);
    progress.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setMaximumHeight(100);
    progress.setMaximumWidth(500);
    progress.setMinimumDuration(0);


    progress.setValue(0);
    progress.show();
    progress.update();
    progress.repaint();




    // Go through all inventory
    for (int i=0;i<inv->get_Entries();i++){

        inv->set_currentCore(i);
        inv->readData(inv->get_currentCore());
        if (inv->get_Selected(i)){
            progress.setValue(progress.value()+1);
            progress.update();
            progress.repaint();
            QCoreApplication::processEvents();

            ui->plainTextEdit->appendPlainText("Name :"+inv->get_Core(i)+" : "+inv->get_Species(i));
            if (inv->get_AgeModel(i)==1){

                // Read Agemodel
                ui->plainTextEdit->appendPlainText("    Reading age model data...");
                repaint();
                AMSData* ams=new AMSData(inv);
                ams->AMSRead();
                ui->plainTextEdit->appendPlainText("    OK. ");
                repaint();
                if (ams->get_bacon_out_ncol()!=0){
                    ui->plainTextEdit->appendPlainText("    Bacon data found("+QString::number(ams->get_bacon_out_nrow()-ams->get_bacon_burnin())+" iterations).");
                    // (1)+++++ Using Bacon Agedata ++++++

                    // Calculation age models
                    ui->plainTextEdit->appendPlainText("    Calculating results...");
                    repaint();
                    // get original out_nrow

                    int out_nrow=ams->get_bacon_out_nrow();


                    // analyse out and transform it to data
                    // Calc hist with own routine
                    int burnin=ams->get_bacon_burnin();
                    if (burnin>out_nrow) burnin=out_nrow;

                    // transfer out to array for analysis and apply burnin
                    out_nrow=out_nrow-burnin;
                    int out_ncol=ams->get_bacon_out_ncol();
                    float *outdata=new float [out_nrow*out_ncol];

                    for (int q=0;q<out_ncol;q++){
                        for (int j=burnin;j<out_nrow+burnin;j++){
                            outdata[q*out_nrow+(j-burnin)]=ams->get_bacon_out(q*(out_nrow+burnin)+j);
                        }
                    }

                    // export source data to arrays to speed up the process
                    float* source_depth=new float[inv->get_Length()];
                    float* source_data=new float[inv->get_Length()];
                    float* source_error=new float[inv->get_Length()];
                    int* source_flag=new int[inv->get_Length()];
                    for (int ii=0;ii<inv->get_Length();ii++){
                        source_depth[ii]=inv->access_data_value(0,ii).toFloat();
                        source_data[ii]=inv->access_data_value(data_index,ii).toFloat();
                        if (inv->access_data_error(data_index,ii)=="nan") source_error[ii]=0;
                        else source_error[ii]=inv->access_data_error(data_index,ii).toFloat();
                        source_flag[ii]=inv->get_data_Use_Flag(ii);
                        //qDebug()<<QString::number(source_depth[ii])+" : "+QString::number(source_data[ii])+" : "+QString::number(source_error[ii])+" : "+QString::number(source_flag[ii]);
                    }



                    // calculate ages
                    float section_size=(ams->get_bacon_d_max()-ams->get_bacon_d_min())/ams->get_bacon_K();
                    for (int j=1;j<out_ncol-2;j++){
                        for (int i=0;i<out_nrow;i++) outdata[j*out_nrow+i]=outdata[(j-1)*out_nrow+i]+outdata[(j)*out_nrow+i]*section_size;
                    }

                    // Now agemodels are ready in ages
/*
                    Editor *cc=new Editor(this,ages,ages,depth_n,out_nrow);// This takes a lot of time!!!!
                    cc->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                    cc->show();
*/


                    // (2) ++++++ define age array ++++++++
                    // already done. see result_age array

                    // (3) ++++++ prepare isotope data +++++++
                    // create array with noisy isotope data

                    ui->plainTextEdit->appendPlainText("    Adding some noise to proxy-data...");
                    repaint();

                    float* noisy=new float[inv->get_Length()*out_nrow];
                    for (int iter=0;iter<out_nrow;iter++){
                        for (int j=0;j<inv->get_Length();j++) {

                            if ((isnan(source_data[j])==true || source_error[j]==0) || ui->radioButton->isChecked()==false) {
                               noisy[j+iter*inv->get_Length()]=source_data[j]+gauss(0,1)*ui->doubleSpinBox_6->value();
                            }else{
                               noisy[j+iter*inv->get_Length()]=source_data[j]+gauss(0,1)*source_error[j];
                            }
                        }

                        //qDebug()<<QString::number(iter)+" of "+QString::number(out_nrow);
                    }
/*
                    Editor *c=new Editor(this,noisy,noisy,ui->spinBox->value(),inv->get_Length());// This takes a lot of time!!!!
                    c->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                    c->show();
*/
                    qDebug()<<"finished";
                    // (4) ++++++ Interpolate bacon agemodels(ages) to have same depths(inv->get_Depth()) as isodata ++++++++++++++++++++
                    ui->plainTextEdit->appendPlainText("    Interpolating...");
                    repaint();
                    qDebug()<<"Interpolating";
                    float* newages=new float[inv->get_Length()*out_nrow];
                    for (int j=0;j<inv->get_Length()*out_nrow;j++) newages[j]=NAN;
                    for (int j=0;j<inv->get_Length();j++){
                        // get value for depth
                        float depth=source_depth[j]*100;// turn m to cm
                        //qDebug()<<QString::number(depth);
                        // find neihbors in ages(analytically because its equidistant)
                        float res=(ams->get_bacon_d_max()-ams->get_bacon_d_min())/ams->get_bacon_K();
                        int row=(depth-ams->get_bacon_d_min())/res;
                        double depth0=ams->get_bacon_d_min()+(row)*res;// get depth for r
                        double depth1=ams->get_bacon_d_min()+(row+1)*res;// get depth for r+1
                        //qDebug()<<QString::number(depth0)+"-"+QString::number(depth1);

                        if (depth>=ams->get_bacon_d_min() && depth<ams->get_bacon_d_max() && row<ams->get_bacon_K()-1){

                            for (int q=0;q<out_nrow;q++){
                                // interpolate for all agemodels of this depth

                                newages[j+q*inv->get_Length()]=outdata[q+row*out_nrow]
                                        +(outdata[q+(row+1)*out_nrow]
                                        -outdata[q+row*out_nrow])
                                        /(depth1-depth0)*(depth-depth0);
                            }
                        }
                    }
                    delete[] outdata;
/*
                    Editor *cc=new Editor(this,newages,newages,out_nrow,inv->get_Length());// This takes a lot of time!!!!
                    cc->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                    cc->show();
*/
                    qDebug()<<"finished";
                    // (5) Interpolation Fit noisy and newages to new ageintervalls and interpolate new values
                    ui->plainTextEdit->appendPlainText("    Aligning Bacon and proxy...");
                    repaint();
                    qDebug()<<"Interpolating source";
                    float* multi_data=new float[count*out_nrow];
                    for (int j=0;j<count*out_nrow;j++)multi_data[j]=NAN;
                    for (int j=0;j<count;j++){// fill for all new ages

                        float age=ui->doubleSpinBox->value()+j*ui->doubleSpinBox_4->value();
                        age=age*1000;
                        QCoreApplication::processEvents();

                        for (int q=0;q<out_nrow;q++){// do it for all iterations
                            // since not equidistant search necessary
                            // search for first valid bigger age
                            int age1=-1;
                            for (int r=0;r<inv->get_Length();r++){//search ages iteration q
                                if (source_flag[r]==1 && isnan(newages[r+q*inv->get_Length()])==false){ // accept only if use flag is set. This time we interpolate also the data, thats why it matters.
                                    if (newages[r+q*inv->get_Length()]>=age){
                                        age1=r;
                                        break;
                                    }
                                }
                            }

                            // search for biggest smaler age
                            int age0=-1;
                            if (age1!=-1){

                                for (int r=age1;r>=0;r--){//search ages iteration q
                                    if (source_flag[r]==1 && isnan(newages[r+q*inv->get_Length()])==false){ // accept only if use flag is set. This time we interpolate also the data, thats why it matters.
                                        if (newages[r+q*inv->get_Length()]<=age){
                                            age0=r;
                                            break;
                                        }
                                    }
                                }
                            }
                            //qDebug()<<QString::number(age0)+":"+QString::number(age1);
                            // if 2 points were found->start interpolation for d18O in iteration q
                            if (age0!=-1 && age1!=-1 ){
                                float lage=newages[age0+q*inv->get_Length()];
                                float uage=newages[age1+q*inv->get_Length()];
                                float ldat=noisy[age0+q*inv->get_Length()];
                                float udat=noisy[age1+q*inv->get_Length()];
                                multi_data[j+count*q]=ldat+(udat-ldat)/(uage-lage)*(age-lage);
                            }
                        }
                        QCoreApplication::processEvents();

                        //qDebug()<<QString::number(j)+" of "+QString::number(count);
                    }

                    delete[] newages;

/*
                        Editor *cc=new Editor(this,multi_d18O,multi_d18O,out_nrow,count);// This takes a lot of time!!!!
                        cc->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                        cc->show();
*/
                    qDebug()<<"finished";



                    // (6) calculate 5 50 95 Quantile for all ages
                    // sort values in Iteration and estimate Quantiles
                    // set nans to 999999 and count elements
                    ui->plainTextEdit->appendPlainText("    Estimating quantiles...");
                    repaint();
                    qDebug()<<"Statistics";






                    int* c=new int[count];
                    for (int j=0;j<count;j++) c[j]=0;
                    for (int j=0;j<count;j++){
                        for (int q=0;q<out_nrow;q++){
                            if (multi_data[j+count*q]>-5000000 && multi_data[j+count*q]<5000000) {
                                c[j]=c[j]+1;
                            } else {
                                multi_data[j+count*q]=9999999;
                            }
                        }
                    }

                    if (ui->comboBox_4->currentIndex()==0){
                        // estimate mean and stds
                        float* mean=new float[count];
                        for (int j=0;j<count;j++){
                            float sum=0;
                            int cn=0;
                            for (int q=0;q<out_nrow;q++){
                                if (multi_data[j+count*q]!=9999999){
                                    sum=sum+multi_data[j+count*q];
                                    cn++;
                                }
                            }
                            if (cn>1)result_mid.push_back(sum/cn); else result_mid.push_back(NAN);
                            if (cn>1)mean[j]=(sum/cn); else mean[j]=(NAN);

                        }
                        // std
                        for (int j=0;j<count;j++){
                            float sum=0;
                            int cn=0;
                            for (int q=0;q<out_nrow;q++){
                                if (multi_data[j+count*q]!=9999999){
                                    sum=sum+(multi_data[j+count*q]-mean[j])*(multi_data[j+count*q]-mean[j]);
                                    cn++;
                                }
                            }
                            if(cn>1)result_min.push_back(mean[j]-sqrt(sum/(cn-1)));else result_min.push_back(NAN);
                            if(cn>1)result_max.push_back(mean[j]+sqrt(sum/(cn-1)));else result_max.push_back(NAN);
                        }
                        // skewness
                        for (int j=0;j<count;j++){
                            float sum1=0;
                            float sum2=0;
                            int cn=0;
                            for (int q=0;q<out_nrow;q++){
                                if (multi_data[j+count*q]!=9999999){
                                    sum1=sum1+(multi_data[j+count*q]-mean[j])*(multi_data[j+count*q]-mean[j])*(multi_data[j+count*q]-mean[j]); cn++;
                                    sum2=sum2+(multi_data[j+count*q]-mean[j])*(multi_data[j+count*q]-mean[j]);
                                }
                            }
                            if (cn>1)result_skew.push_back((sum1/cn)/pow(sum2/(cn-1),3.0/2.0)); else result_skew.push_back(NAN);
                        }
                        delete[] mean;
                    }

                    if (ui->comboBox_4->currentIndex()==1){
                        // estimate mean and stds
                        float* mean=new float[count];
                        for (int j=0;j<count;j++){
                            float sum=0;
                            int cn=0;
                            for (int q=0;q<out_nrow;q++){
                                if (multi_data[j+count*q]!=9999999){
                                    sum=sum+multi_data[j+count*q];
                                    cn++;
                                }
                            }
                            if (cn>1)result_mid.push_back(sum/cn); else result_mid.push_back(NAN);
                            if (cn>1)mean[j]=(sum/cn); else mean[j]=(NAN);

                        }
                        for (int j=0;j<count;j++){
                            float sum=0;
                            int cn=0;
                            for (int q=0;q<out_nrow;q++){
                                if (multi_data[j+count*q]!=9999999&&multi_data[j+count*q]<=mean[j]){
                                    sum=sum+(multi_data[j+count*q]-mean[j])*(multi_data[j+count*q]-mean[j]);
                                    cn++;
                                }
                            }
                            if(cn>1)result_min.push_back(mean[j]-sqrt(sum/(cn-1)));else result_min.push_back(NAN);
                        }
                        for (int j=0;j<count;j++){
                            float sum=0;
                            int cn=0;
                            for (int q=0;q<out_nrow;q++){
                                if (multi_data[j+count*q]!=9999999&&multi_data[j+count*q]>=mean[j]){
                                    sum=sum+(multi_data[j+count*q]-mean[j])*(multi_data[j+count*q]-mean[j]); cn++;
                                }
                            }
                            if (cn>1)result_max.push_back(mean[j]+sqrt(sum/(cn-1))); else result_max.push_back(NAN);
                            if (cn>1)result_skew.push_back(NAN);
                        }
                        delete[] mean;
                    }

                    if (ui->comboBox_4->currentIndex()==2){
                        for (int j=0;j<count;j++){
                            sort(multi_data,j,count,0, out_nrow-1);
                            // get Quantiles
                            float v=multi_data[j+count*((int)(c[j]*0.023))];
                            if (v!=9999999 && c[j]/count>0.5) result_min.push_back(v); else result_min.push_back(NAN);

                            v=multi_data[j+count*((int)(c[j]*0.5))];
                            if (v!=9999999 && c[j]/count>0.5) result_mid.push_back(v); else result_mid.push_back(NAN);

                            v=multi_data[j+count*((int)(c[j]*0.977))];
                            if (v!=9999999 && c[j]/count>0.5) result_max.push_back(v); else result_max.push_back(NAN);
                            result_skew.push_back(NAN);

                        }
                    }




                    // add meta data to datastructure
                    result_core.push_back(inv->get_Core(inv->get_currentCore()));
                    result_species.push_back(inv->get_Species(inv->get_currentCore()));
                    result_lat.push_back(inv->get_Latitude(inv->get_currentCore()));
                    result_long.push_back(inv->get_Longitude(inv->get_currentCore()));
                    result_wd.push_back(inv->get_Water_Depth(inv->get_currentCore()));
                    entries++;


                    delete[] c;
                    delete[] multi_data;


                    delete[] noisy;
                    delete[] source_depth;
                    delete[] source_data;
                    delete[] source_error;
                    delete[] source_flag;


                    qDebug()<<"finished";
                    qDebug()<<"After Calculation: saveOK="+QString::number(save_OK)+" size="+QString::number(size)+" entries="+QString::number(entries);

                    // Lock Parameters
                    {
                    const QSignalBlocker blocker(ui->comboBox_3);
                    ui->comboBox_3->setEnabled(false);
                    }
                    {
                    const QSignalBlocker blocker(ui->doubleSpinBox);
                    ui->doubleSpinBox->setEnabled(false);
                    }
                    {
                    const QSignalBlocker blocker(ui->doubleSpinBox_2);
                    ui->doubleSpinBox_2->setEnabled(false);
                    }
                    {
                    const QSignalBlocker blocker(ui->doubleSpinBox_6);
                    ui->doubleSpinBox_6->setEnabled(false);
                    }
                    {
                    const QSignalBlocker blocker(ui->doubleSpinBox_4);
                    ui->doubleSpinBox_4->setEnabled(false);
                    }
                    {
                    const QSignalBlocker blocker(ui->comboBox_4);
                    ui->comboBox_4->setEnabled(false);
                    }
                    qDebug()<<"Right after disable: saveOK="+QString::number(save_OK)+" size="+QString::number(size)+" entries="+QString::number(entries);


                        // estimate global limits
                        global_min=999999999999;
                        global_max=-999999999999;
                        for (int i=0;i<result_mid.size();i++){
                            if (result_mid.at(i)<global_min) global_min=result_mid.at(i);
                            if (result_mid.at(i)>global_max) global_max=result_mid.at(i);
                        }












                } else {
                    // What to do if no bacon data found?
                    ui->plainTextEdit->appendPlainText("    NO BACON AGE-MODEL FOUND!!!");
                    repaint();
                }
                qDebug()<<"deleting agemodel data";
                delete ams;


            } else {

                // No Agemodel found
                ui->plainTextEdit->appendPlainText("    NO AGE-FILE FOUND!!!");
                repaint();
            }

        }
    }

     progress.close();


    size=count;
    ui->plainTextEdit->appendPlainText("finished!!!");
    // select first choosen dataset
    for(int i=0;i<inv->get_Entries();i++) if(inv->get_Selected(i)==1){
        inv->set_currentCore(i);
        break;
    }
    qDebug()<<"Right before updating everything: saveOK="+QString::number(save_OK)+" size="+QString::number(size)+" entries="+QString::number(entries);

    plot();
    createmap(1);
    setupExtractTable();
    repaint();
    save_OK=1;

    if (entries>0 && size>0) selected_entry=0;
    qDebug()<<"After Finshing Calc: saveOK="+QString::number(save_OK)+" size="+QString::number(size)+" entries="+QString::number(entries);

}

void Extract::sort(float* data,int j,int count,int left,int right){
    // sort values using QuickSort
    int ii = left, jj = right;
    double tmp;

    double pivot = data[j+count*((left + right) / 2)];
    while (ii <= jj) {
        while (data[j+count*ii] < pivot) ii++;
        while (data[j+count*jj] > pivot) jj--;
        if (ii <= jj) {
            // exchange everything
            tmp = data[j+count*ii];
            data[j+count*ii] = data[j+count*jj];
            data[j+count*jj] = tmp;

            ii++;
            jj--;
        }
    };
    if (left < jj) sort(data,j,count,left, jj);
    if (ii < right) sort(data,j,count,ii, right);
}

// Generate Gauss according Box-Mueller method
double Extract::gauss(double mu, double sigma)
{
    const double epsilon = std::numeric_limits<double>::min();
    const double two_pi = 2.0*3.14159265358979323846;

    static double z0, z1;
    static bool generate;
    generate = !generate;

    if (!generate)
       return z1 * sigma + mu;

    double u1, u2;
    do
     {
       u1 = qrand() * (1.0 / RAND_MAX);
       u2 = qrand() * (1.0 / RAND_MAX);

     }
    while ( u1 <= epsilon );

    z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
    return z0 * sigma + mu;
}

void Extract::plot(){
    {
    int index=selected_entry;
    if (index>=0&&index<entries){

        delete[] data;
        data=new float[6*size];
        for(int i=0;i<6*size;i++) data[i]=NAN;
        delete[] coll;
        coll=new QColor[6];
        for (int i=0;i<6;i++) coll[i]=QColor(0,0,0);
        delete[] lstyle;
        lstyle=new Qt::PenStyle[6];
        for (int i=0;i<6;i++) lstyle[i]=Qt::DashLine;
        for(int i=0;i<size;i++){



                data[0*size+i]=result_age[i];
                data[1*size+i]=result_min[i+size*index];

                data[2*size+i]=result_age[i];
                data[3*size+i]=result_mid[i+size*index];

                data[4*size+i]=result_age[i];
                data[5*size+i]=result_max[i+size*index];




        }



        coll[0]=QColor(128,128,128);
        coll[1]=QColor(128,128,128);
        coll[2]=QColor(255,0,0);
        coll[3]=QColor(255,0,0);
        coll[4]=QColor(128,128,128);
        coll[5]=QColor(128,128,128);
        lstyle[0]=Qt::DotLine;
        lstyle[1]=Qt::DotLine;
        lstyle[2]=Qt::SolidLine;
        lstyle[3]=Qt::SolidLine;
        lstyle[4]=Qt::DotLine;
        lstyle[5]=Qt::DotLine;


        pl1->setData(data,6,size);
        pl1->setLineWidth(3);
        pl1->setTitel(QString::number(index)+" : "+result_core.at(index)+" : "+result_species.at(index),"Age [ka BP]",ui->comboBox_3->currentText());
        pl1->setMultiplicator(1,1);
        pl1->setTextSize(resources->get_Pointsize(),12,12);
        pl1->setSetLineColor(coll,1);
        pl1->setSetLineStyle(lstyle,1);
        pl1->setSize(ui->graphicsView->width(),ui->graphicsView->height());
        pl1->setSymbol(0);
        pl1->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract1_"+inv->access_proxy_abbreviation()+".txt");
        pl1->autoSize();
        ui->graphicsView->setScene(pl1);

        pl2->setData(data,6,size);
        pl2->setLineWidth(3);
        pl2->setTitel(QString::number(index)+" : "+result_core.at(index)+" : "+result_species.at(index),"Age [ka BP]",ui->comboBox_3->currentText());
        pl2->setMultiplicator(1,1);
        pl2->setTextSize(resources->get_Pointsize(),12,12);
        pl2->addMarker(ui->doubleSpinBox_3->value(),result_mid.at((int)((ui->doubleSpinBox_3->value()-start)/inc)+size*index),1);
        pl2->setSetLineColor(coll,1);
        pl2->setSetLineStyle(lstyle,1);
        pl2->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
        pl2->setSymbol(0);
        pl2->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract2_"+inv->access_proxy_abbreviation()+".txt");
        pl2->autoSize();
        ui->graphicsView_3->setScene(pl2);



    }
    ui->graphicsView_3->repaint();
    }



}

void Extract::paintEvent(QPaintEvent *)
{
    pl1->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    pl2->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    map->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
}



void Extract::save(){
    qDebug()<<"saveOK="+QString::number(save_OK)+" size="+QString::number(size)+" entries="+QString::number(entries);
    if (save_OK==1 && size>0 && entries>0){
    QString Qfilename = QFileDialog::getSaveFileName(this, tr("Select Save File"),
                                             resources->get_path_data()+"/"+proxy+"/Derived Data/data "+ui->comboBox_3->currentText().replace("°",""),
                                             tr("Data Compilation (*.dc);;ASCII (*.txt);;Excel (*.xlsx)"));




    // get number of datasets

    if (entries!=0){
    ui->plainTextEdit->appendPlainText("Saving to File : "+Qfilename);
    repaint();
    // +++++++++ Preparing data
    // Meta attributes
    // -starting age
    // -increment
    // -ending age
    // Core Meta
    QString* core_name=new QString[entries];
    QString* species_name=new QString[entries];
    float* core_lat=new float[entries];
    float* core_long=new float[entries];
    float* core_depth=new float[entries];
    float* Q05=new float[entries*size];
    float* Q50=new float[entries*size];
    float* Q95=new float[entries*size];

    float* skew=new float[entries*size];
    int count=0;
    for (int i=0;i<entries;i++){

            core_name[count]=result_core.at(i);
            species_name[count]=result_species.at(i);
            core_lat[count]=round(result_lat.at(i)*1000.0)/1000.0;
            core_long[count]=round(result_long.at(i)*1000.0)/1000.0;
            core_depth[count]=round(result_wd.at(i)*1000.0)/1000.0;
            for (int j=0;j<size;j++){
                Q05[j+count*size]=round(result_min[j+size*i]*1000.0)/1000.0;
                Q50[j+count*size]=round(result_mid[j+size*i]*1000.0)/1000.0;
                Q95[j+count*size]=round(result_max[j+size*i]*1000.0)/1000.0;
                skew[j+count*size]=round(result_skew[j+size*i]*1000.0)/1000.0;
            }
            count++;

    }


    if (Qfilename.right(3)==".dc"){
        char* filename;// need character for nc file
        string fname=Qfilename.toStdString();
        filename=new char[fname.size()+1];
        strcpy(filename,fname.c_str());

       qDebug()<<"saving...";

        // Initialize ids and meta
        int ncid;
        int retval;// for Errors

        // Open the file
        if (!(retval = nc_create(filename, NC_WRITE, &ncid))){
            // Define variables

            // Float list length
            int length_dim;
            int dim[1];
            if ((retval = nc_def_dim(ncid,"Length",entries,&length_dim)))
               ERR(retval);
            dim[0]=length_dim;

            // Character Strings for identification
            // Corename
            int corename_length=0;
            for (int i=0;i<entries;i++) if (corename_length<core_name[i].length()) corename_length=core_name[i].length();
            int corename_label_dim;
            if ((retval = nc_def_dim(ncid,"Core_Length",corename_length,&corename_label_dim)))
               ERR(retval);
            int dimm[2];
            dimm[0]=corename_label_dim;
            dimm[1]=length_dim;
            int corename_id;
            if ((retval = nc_def_var(ncid,"Core",NC_CHAR,2,dimm,&corename_id)))
                ERR(retval);

            // Species
            int species_length=0;
            for (int i=0;i<entries;i++) if (species_length<species_name[i].length()) species_length=species_name[i].length();
            int species_label_dim;
            if ((retval = nc_def_dim(ncid,"Species_Length",species_length,&species_label_dim)))
               ERR(retval);
            int dimm2[2];
            dimm2[0]=species_label_dim;
            dimm2[1]=length_dim;
            int species_id;
            if ((retval = nc_def_var(ncid,"Species",NC_CHAR,2,dimm2,&species_id)))
                ERR(retval);

            // Float lists for location
            // long
            int long_id;
            if ((retval = nc_def_var(ncid,"Longitude",NC_FLOAT,1,dim,&long_id)))
                ERR(retval);
            // latt
            int latt_id;
            if ((retval = nc_def_var(ncid,"Latitude",NC_FLOAT,1,dim,&latt_id)))
                ERR(retval);
            // depth
            int depth_id;
            if ((retval = nc_def_var(ncid,"Depth",NC_FLOAT,1,dim,&depth_id)))
                ERR(retval);



            // set size of Q matrix
            int size_dim;
            int Q05_id;
            int Q50_id;
            int Q95_id;
            int skew_id;

            if ((retval = nc_def_dim(ncid,"Size",size,&size_dim)))
               ERR(retval);
            dimm[0]=length_dim;
            dimm[1]=size_dim;
           //qDebug()<<dim[0];
           //qDebug()<<dim[1];
           if (ui->comboBox_4->currentIndex()==2){
                if ((retval = nc_def_var(ncid,"Quantile_023",NC_FLOAT,2,dimm,&Q05_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Quantile_50",NC_FLOAT,2,dimm,&Q50_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Quantile_977",NC_FLOAT,2,dimm,&Q95_id)))
                    ERR(retval);
            }
           if (ui->comboBox_4->currentIndex()==1){
                if ((retval = nc_def_var(ncid,"Std-",NC_FLOAT,2,dimm,&Q05_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Mean",NC_FLOAT,2,dimm,&Q50_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Std+",NC_FLOAT,2,dimm,&Q95_id)))
                    ERR(retval);
            }

           if (ui->comboBox_4->currentIndex()==0){
                if ((retval = nc_def_var(ncid,"Std_min",NC_FLOAT,2,dimm,&Q05_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Mean",NC_FLOAT,2,dimm,&Q50_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Std_max",NC_FLOAT,2,dimm,&Q95_id)))
                    ERR(retval);
                if ((retval = nc_def_var(ncid,"Skewness",NC_FLOAT,2,dimm,&skew_id)))
                    ERR(retval);
            }

            //**********************************************

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // ******* write Core
            {
            unsigned char text[corename_length][entries];
            for(int i=0;i<corename_length;i++) for(int ii=0;ii<entries;ii++) text[i][ii]=' ';
            for (int i=0;i<entries;i++){
                for (int j=0;j<core_name[i].length();j++) text[j][i]=core_name[i].at(j).toLatin1();
            }
            if ((retval = nc_put_var(ncid,corename_id,&text[0][0])))
                ERR(retval);
            }

            // ******* write Species
            {
            unsigned char text[species_length][entries];
            for(int i=0;i<species_length;i++) for(int ii=0;ii<entries;ii++) text[i][ii]=' ';
            for (int i=0;i<entries;i++){
                for (int j=0;j<species_name[i].length();j++) text[j][i]=species_name[i].at(j).toLatin1();
            }
            if ((retval = nc_put_var(ncid,species_id,&text[0][0])))
                ERR(retval);
            }

            // ******** save latt
            if ((retval = nc_put_var(ncid,latt_id,&core_lat[0])))
                ERR(retval);
            // ******** save long
            if ((retval = nc_put_var(ncid,long_id,&core_long[0])))
                ERR(retval);
            // ******** save depth
            if ((retval = nc_put_var(ncid,depth_id,&core_depth[0])))
                ERR(retval);

            // ******** save Q05
            if ((retval = nc_put_var(ncid,Q05_id,&Q05[0])))
                ERR(retval);
            // ******** save Q50
            if ((retval = nc_put_var(ncid,Q50_id,&Q50[0])))
                ERR(retval);
            // ******** save Q95
            if ((retval = nc_put_var(ncid,Q95_id,&Q95[0])))
                ERR(retval);

            if (ui->comboBox_4->currentIndex()==0){
                // ******** save skewness
                if ((retval = nc_put_var(ncid,skew_id,&skew[0])))
                    ERR(retval);
            }


            // activate defMode
            if ((retval = nc_redef(ncid)))
                ERR(retval);

            // write attributes
            // 1:start
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"Start_Age",NC_FLOAT,1,&start)))
                ERR(retval);
            // 2:increment
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"Increment_Age",NC_FLOAT,1,&inc)))
                ERR(retval);
            // 3:end
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"End_Age",NC_FLOAT,1,&end)))
                ERR(retval);

            // 3:noise
            float noise=(float)ui->doubleSpinBox_6->value();
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"Noise",NC_FLOAT,1,&noise)))
                ERR(retval);

            // 4:save parameter
            QString para=ui->comboBox_3->currentText();
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"Parameter",NC_CHAR,para.length(),para.toLatin1().data())))
                ERR(retval);
            // 5:save statistic
            QString para2=ui->comboBox_4->currentText();
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"Statistic",NC_CHAR,para2.length(),para2.toLatin1().data())))
                ERR(retval);
           //qDebug()<< para;



            // *******************************************************

           //qDebug()<<"data written";

            // Close the file, freeing all resources.
            if ((retval = nc_close(ncid)))
               ERR(retval);




        } else {
            if (nc_strerror(retval)=="No such file or directory") {
               //qDebug() << "Start generating new File...";
            } else {

                //ERR(retval);
            }
        }
        delete[] filename;

    }

    if (Qfilename.right(4)==".txt"){
        qDebug()<<"saving ASCII";
        QString txt="";
        // write metatable
        txt.append("List of Cores\n");
        txt.append("Corename\tProxy\tLatitude\tLongitude\tDepth\n");
        for (int i=0;i<entries;i++){
            txt.append(core_name[i]+"\t"+species_name[i]+"\t"+QString::number(core_lat[i])+"\t"+QString::number(core_long[i])+"\t"+QString::number(core_depth[i])+"\n");
        }

        // write data
        txt.append("\nParameter:"+ui->comboBox_3->currentText());
        for (int i=0;i<entries;i++) if(ui->comboBox_4->currentIndex()==0) txt.append("\t"+core_name[i]+"\t"+species_name[i]+"\t\t"); else  txt.append("\t"+core_name[i]+"\t"+species_name[i]+"\t");
        txt.append("\n");
        txt.append("Age in kyr");
        if(ui->comboBox_4->currentIndex()==2) for (int i=0;i<entries;i++) txt.append("\tQ023\tQ50\tQ977");
        if(ui->comboBox_4->currentIndex()==1) for (int i=0;i<entries;i++) txt.append("\tStd-\tMean\tStd+");
        if(ui->comboBox_4->currentIndex()==0) for (int i=0;i<entries;i++) txt.append("\tStd_min\tMean\tStd_max\tSkewness");
        txt.append("\n");
        for (int i=0;i<size;i++) {
            txt.append(QString::number(start+inc*i));
            for (int j=0;j<entries;j++){
                if(ui->comboBox_4->currentIndex()==0)txt.append("\t"+QString::number(Q05[j*size+i])+"\t"+QString::number(Q50[j*size+i])+"\t"+QString::number(Q95[j*size+i])+"\t"+QString::number(skew[j*size+i]));
               if(ui->comboBox_4->currentIndex()==1||ui->comboBox_4->currentIndex()==2) txt.append("\t"+QString::number(Q05[j*size+i])+"\t"+QString::number(Q50[j*size+i])+"\t"+QString::number(Q95[j*size+i]));
            }
            txt.append("\n");
        }



        QFile f(Qfilename);
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            //qDebug() <<  f.errorString();
        } else {
        //const char *text=txt.toStdString().c_str();
        //f.write(text,qstrlen(text));
        QTextStream out(&f);
        out<<txt;
        f.close();
        }
        QDesktopServices::openUrl(QUrl("file:///"+Qfilename, QUrl::TolerantMode));
    }

    if (Qfilename.right(5)==".xlsx"){
        QXlsx::Document xlsx;
        // Corelist
        xlsx.addSheet("Corelist", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Corelist");
        xlsx.setColumnWidth(1, 5, 20);

        xlsx.write("A1","Corename");
        xlsx.write("B1","Species");
        xlsx.write("C1","Latitude");
        xlsx.write("D1","Longitude");
        xlsx.write("E1","Depth");

        for(int i=0;i<entries;i++){
            xlsx.write("A"+QString::number(i+2),core_name[i]);
            xlsx.write("B"+QString::number(i+2),species_name[i]);
            xlsx.write("C"+QString::number(i+2),round(core_lat[i]*1000.0)/1000.0);
            xlsx.write("D"+QString::number(i+2),round(core_long[i]*1000.0)/1000.0);
            xlsx.write("E"+QString::number(i+2),round(core_depth[i]*1000.0)/1000.0);
        }
        int format=3;
        if (ui->comboBox_4->currentIndex()==0) format=4;
        xlsx.addSheet("Proxy Data", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Proxy Data");
        xlsx.setColumnWidth(1,entries+1, 20);
        xlsx.write("A1","\nParameter:"+ui->comboBox_3->currentText());
        xlsx.write("A2","Age in [kyr]");
        for (int i=0;i<size;i++) xlsx.write("A"+QString::number(i+format),round((start+i*inc)*1000.0)/1000.0);

        for (int j=0;j<entries;j++){
            // turn number into position
            int n=j*format+1;

            int hi=(int)(n/(26*26));
            int mid=(int)(n-hi*26*26)/26;
            int low=(int)(n-hi*26*26-mid*26);
            QString str="";
            if (hi>0)str.append(QChar(hi+64));
             if (mid>0)str.append(QChar(mid+64));
              str.append(QChar(low+65));
              //qDebug()<<str;
            xlsx.write(str+"1",core_name[j]);
            if(ui->comboBox_4->currentIndex()==2) xlsx.write(str+"2","Q023");
            if(ui->comboBox_4->currentIndex()==1) xlsx.write(str+"2","std-");
            if(ui->comboBox_4->currentIndex()==0) xlsx.write(str+"2","std_min");
            for (int i=0;i<size;i++) if (isnan(Q05[j*size+i])) xlsx.write(str+""+QString::number(i+format),"nan"); else xlsx.write(str+""+QString::number(i+format),round(Q05[j*size+i]*1000.0)/1000.0);


            n++;
            hi=(int)(n/(26*26));
            mid=(int)(n-hi*26*26)/26;
            low=(int)(n-hi*26*26-mid*26);
            str.clear();
            if (hi>0)str.append(QChar(hi+64));
             if (mid>0)str.append(QChar(mid+64));
              str.append(QChar(low+65));
              //qDebug()<<str;
            xlsx.write(str+"1",species_name[j]);
            if(ui->comboBox_4->currentIndex()==2) xlsx.write(str+"2","Q50");
            if(ui->comboBox_4->currentIndex()==1) xlsx.write(str+"2","mean");
            if(ui->comboBox_4->currentIndex()==0) xlsx.write(str+"2","mean");
            for (int i=0;i<size;i++) if (isnan(Q50[j*size+i])) xlsx.write(str+""+QString::number(i+format),"nan"); else xlsx.write(str+""+QString::number(i+format),round(Q50[j*size+i]*1000.0)/1000.0);

            n++;
            hi=(int)(n/(26*26));
            mid=(int)(n-hi*26*26)/26;
            low=(int)(n-hi*26*26-mid*26);
            str.clear();
            if (hi>0)str.append(QChar(hi+64));
             if (mid>0)str.append(QChar(mid+64));
              str.append(QChar(low+65));
              //qDebug()<<str;

              if(ui->comboBox_4->currentIndex()==2) xlsx.write(str+"2","Q977");
              if(ui->comboBox_4->currentIndex()==1) xlsx.write(str+"2","std+");
              if(ui->comboBox_4->currentIndex()==0) xlsx.write(str+"2","std_max");
            for (int i=0;i<size;i++) if (isnan(Q95[j*size+i])) xlsx.write(str+""+QString::number(i+format),"nan"); else xlsx.write(str+""+QString::number(i+format),round(Q95[j*size+i]*1000.0)/1000.0);

            if (format==4){
                n++;
                hi=(int)(n/(26*26));
                mid=(int)(n-hi*26*26)/26;
                low=(int)(n-hi*26*26-mid*26);
                str.clear();
                if (hi>0)str.append(QChar(hi+64));
                 if (mid>0)str.append(QChar(mid+64));
                  str.append(QChar(low+65));
                  //qDebug()<<str;

                  if(ui->comboBox_4->currentIndex()==0) xlsx.write(str+"2","Skewness");

                for (int i=0;i<size;i++) if (isnan(skew[j*size+i])) xlsx.write(str+""+QString::number(i+format),"nan"); else xlsx.write(str+""+QString::number(i+format),round(skew[j*size+i]*1000.0)/1000.0);
            }

        }





        // save
        xlsx.saveAs(Qfilename);


        QDesktopServices::openUrl(QUrl("file:///"+Qfilename, QUrl::TolerantMode));

    }


    delete[] core_name;
    delete[] species_name;
    delete[] core_lat;
    delete[] core_long;
    delete[] core_depth;
    delete[] Q05;
    delete[] Q50;
    delete[] Q95;
    delete[] skew;

    } else {
        ui->plainTextEdit->appendPlainText("There is nothing to save!!!");
        repaint();
    }
    } else {
        ui->plainTextEdit->appendPlainText("First generate some data or read some from a file!!!");
        repaint();
    }
}

void Extract::read(){
    QString Qfilename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources->get_path_data()+"/"+proxy+"/Derived Data/",
                                             tr("Data Compilation (*.dc)"));
    ui->plainTextEdit->appendPlainText("Reading from file : "+Qfilename);
    repaint();

    char* filename;
    string fname=Qfilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;


    // Initialize ids and meta
    int ncid;
    int varid;
    size_t dimlen[2]={0};
    int retval;// for Errors

    // Open the file
    if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){




        // ******* read Corename
        std::vector<QString*> core;
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "Length", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);

        // number of cores and character length
        if ((retval = nc_inq_dimid(ncid, "Core_Length", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[1])))
           ERR(retval);




        if ((retval = nc_inq_varid(ncid, "Core", &varid)))
           ERR(retval);
        unsigned char text[dimlen[0]*dimlen[1]];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);
        core.clear();
        core.reserve(dimlen[0]);
        for (int i=0;i<dimlen[0];i++){
            core.push_back(new QString(""));
            for (int j=0;j<dimlen[1];j++){
                if (text[j*dimlen[0]+i]!=0){
                    core[i]->append(text[j*dimlen[0]+i]);
                }
            }
        }
        }

        // ******* read Species
        std::vector<QString*> species;
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "Species_Length", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[1])))
           ERR(retval);

        if ((retval = nc_inq_varid(ncid, "Species", &varid)))
           ERR(retval);
        unsigned char text[dimlen[0]*dimlen[1]];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);
        species.clear();

        for (int i=0;i<dimlen[0];i++){
            species.push_back(new QString(""));
            for (int j=0;j<dimlen[1];j++){
                if (text[j*dimlen[0]+i]!=0){
                    species[i]->append(text[j*dimlen[0]+i]);
                }
            }
        }
        }



        // read Q

        if ((retval = nc_inq_dimid(ncid, "Size", &varid)))
           ERR(retval);
        // get size
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[1])))
           ERR(retval);

        int nrow=dimlen[1];
        int ncol=dimlen[0];
        size=nrow;
        entries=ncol;
        qDebug()<<QString::number(nrow);
        qDebug()<<QString::number(ncol);
        qDebug()<<"reading Latitude...";
        // read latitude
        float* lat=new float[dimlen[0]];
        if ((retval = nc_inq_varid(ncid, "Latitude", &varid)))
           ERR(retval);
        qDebug()<<"...";
        if ((retval = nc_get_var_float(ncid,varid,&lat[0]))) ERR(retval);
        // read longitude
        qDebug()<<"reading Longitude...";
        float* lon=new float[dimlen[0]];
        if ((retval = nc_inq_varid(ncid, "Longitude", &varid)))
           ERR(retval);
        if ((retval = nc_get_var_float(ncid,varid,&lon[0]))) ERR(retval);
        // read water depth
        qDebug()<<"reading Water Depth...";
        float* wd=new float[dimlen[0]];
        if ((retval = nc_inq_varid(ncid, "Depth", &varid)))
           ERR(retval);
        if ((retval = nc_get_var_float(ncid,varid,&wd[0]))) ERR(retval);

        // 1:start
        float start_age;
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"Start_Age",&start_age)))
            ERR(retval);
        // 2:increment
        float inc_age;
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"Increment_Age",&inc_age)))
            ERR(retval);

        // 3:noise
        float noise;
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"Noise",&noise)))
            ERR(retval);


        // 4:parameter
        size_t length=0;
        if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,"Parameter",&length)))
           ERR(retval);
        unsigned char text3[length+1];
        if ((retval = nc_get_att(ncid,NC_GLOBAL,"Parameter",&text3[0])))
           ERR(retval);
        QString para;
        para.clear();
        for (unsigned int j=0;j<length;j++){
                para.append(text3[j]);
        }
        //qDebug() << att_Species;

        // read statistic variable to know for w´hich variables i have to look
        // 5:statistic
        size_t length2=0;
        if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,"Statistic",&length2)))
           ERR(retval);
        unsigned char text4[length+1];
        if ((retval = nc_get_att(ncid,NC_GLOBAL,"Statistic",&text4[0])))
           ERR(retval);
        QString para2;
        para2.clear();
        for (unsigned int j=0;j<length2;j++){
                para2.append(text4[j]);
        }
        //qDebug() << att_Species;



            // read Q023
            if ((retval = nc_inq_varid(ncid, "Quantile_023", &varid))) if ((retval = nc_inq_varid(ncid, "Quantile_05", &varid))) if ((retval = nc_inq_varid(ncid, "Std-", &varid))) if ((retval = nc_inq_varid(ncid, "Std_min", &varid)))
               ERR(retval);
            float* Q05=new float[ncol*nrow];
            if ((retval = nc_get_var_float(ncid,varid,&Q05[0]))) ERR(retval);

            // read Q50
            if ((retval = nc_inq_varid(ncid, "Quantile_50", &varid))) if ((retval = nc_inq_varid(ncid, "Mean", &varid)))
               ERR(retval);
            float* Q50=new float[ncol*nrow];
            if ((retval = nc_get_var_float(ncid,varid,&Q50[0])))
                ERR(retval);

            // read Q95
            if ((retval = nc_inq_varid(ncid, "Quantile_977", &varid))) if ((retval = nc_inq_varid(ncid, "Quantile_95", &varid))) if ((retval = nc_inq_varid(ncid, "Std+", &varid))) if ((retval = nc_inq_varid(ncid, "Std_max", &varid)))
               ERR(retval);
            float* Q95=new float[ncol*nrow];
            if ((retval = nc_get_var_float(ncid,varid,&Q95[0]))) ERR(retval);

            float* skew=new float[ncol*nrow];
            for (int j=0;j<ncol*nrow;j++) skew[j]=0;
            if (para2.contains("Mean, std and skewness")){

                // read skewness
                if ((retval = nc_inq_varid(ncid, "Skewness", &varid)))
                   ERR(retval);

                if ((retval = nc_get_var_float(ncid,varid,&skew[0]))) ERR(retval);
                ui->plainTextEdit->appendPlainText("Skewness");
                repaint();
            }






        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);

        // put data into shape

        ui->doubleSpinBox_6->setValue(noise);
        start=start_age;
        ui->doubleSpinBox->setValue(start);
        inc=inc_age;
        ui->doubleSpinBox_4->setValue(inc);
        end=start_age+inc_age*dimlen[1];
        ui->doubleSpinBox_2->setValue(end);
        {
        const QSignalBlocker blocker(ui->horizontalSlider);
        ui->horizontalSlider->setMinimum(0);
        ui->horizontalSlider->setMaximum(size-1);
        ui->horizontalSlider->setValue(0);
        }
        {
        const QSignalBlocker blocker(ui->doubleSpinBox_3);
        ui->doubleSpinBox_3->setMaximum(end-inc);
        ui->doubleSpinBox_3->setMinimum(start);
        ui->doubleSpinBox_3->setSingleStep(inc);
        ui->doubleSpinBox_3->setValue(start);
        }



        ui->comboBox_3->clear();
        ui->comboBox_3->addItem(para);

        ui->comboBox_4->setCurrentText(para2);

        result_core.clear();
        result_species.clear();
        result_lat.clear();
        result_long.clear();
        result_wd.clear();
        result_min.clear();
        result_mid.clear();
        result_max.clear();
        result_skew.clear();

        result_age.clear();
        size=nrow;
        entries=ncol;
        int count=(int)((ui->doubleSpinBox_2->value()-ui->doubleSpinBox->value())/(ui->doubleSpinBox_4->value()));

        for (int j=0;j<count;j++) result_age.push_back(ui->doubleSpinBox->value()+j*ui->doubleSpinBox_4->value());
        // copy data to results
        for (int i=0;i<dimlen[0];i++) for (int q=0;q<dimlen[1];q++) result_min.push_back(Q05[i*dimlen[1]+q]);
        for (int i=0;i<dimlen[0];i++) for (int q=0;q<dimlen[1];q++) result_mid.push_back(Q50[i*dimlen[1]+q]);
        for (int i=0;i<dimlen[0];i++) for (int q=0;q<dimlen[1];q++) result_max.push_back(Q95[i*dimlen[1]+q]);
        for (int i=0;i<dimlen[0];i++) for (int q=0;q<dimlen[1];q++) result_skew.push_back(skew[i*dimlen[1]+q]);

        for (int i=0;i<entries;i++) result_core.append(core[i]->toLatin1().simplified());
        for (int i=0;i<entries;i++) result_species.append(species[i]->toLatin1().simplified());


        for (int i=0;i<entries;i++) result_lat.push_back(lat[i]);
        for (int i=0;i<entries;i++) result_long.push_back(lon[i]);
        for (int i=0;i<entries;i++) result_wd.push_back(wd[i]);


        // find index in inventory and select it
        for (int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
        for (int i=0;i<dimlen[0];i++){

            int found=0;
            for (int j=0;j<inv->get_Entries();j++){
                if (inv->get_Core(j).simplified()==core[i]->toLatin1().simplified() && inv->get_Species(j).simplified()==species[i]->toLatin1().simplified()){
                    inv->set_Selected(j,1);

                    found=1;
                    break;
                }
            }
            if (found==0){
                QString Core=core[i]->toLatin1();
                QString Species=species[i]->toLatin1();
                ui->plainTextEdit->appendPlainText("Core not found : "+Core+" : "+Species);
                repaint();
            }
        }

        qDebug()<<"Entries:"+QString::number(entries);
        qDebug()<<"Size:"+QString::number(size);


        delete[] Q05;
        delete[] Q50;
        delete[] Q95;
        delete[] skew;

        delete[] lat;
        delete[] lon;
        delete[] wd;


            // estimate global limits
            global_min=999999999999;
            global_max=-999999999999;
            for (int i=0;i<result_mid.size();i++){
                if (result_mid.at(i)<global_min) global_min=result_mid.at(i);
                if (result_mid.at(i)>global_max) global_max=result_mid.at(i);
            }




        setupInventory();
        plot();
        createmap(1);
        setupExtractTable();
        save_OK=1;
        // Lock Parameters

        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->setEnabled(false);
        ui->doubleSpinBox->setEnabled(false);
        ui->doubleSpinBox_2->setEnabled(false);
        ui->doubleSpinBox_6->setEnabled(false);
        ui->doubleSpinBox_4->setEnabled(false);


    } else {

        if (nc_strerror(retval)=="No such file or directory") {
            //qDebug() << "Start generating new File...";
        } else {
            //qDebug() << "File Error";
           // ERR(retval);
        }


    }

    delete filename;




}



bool Extract::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->tableView||obj==ui->graphicsView_2||obj==ui->graphicsView_3||obj==ui->tableView_2){
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

void Extract::updateGraph(){

    plot();
    createmap(0);
    setupExtractTable();
}

void Extract::copy(QPoint pos){

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

void Extract::copy2(QPoint pos){

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

void Extract::MapToWorld(){
    mapmode=0;
    setButtons();
    createmap(1);
    update();
}

void Extract::MapToATL(){
    mapmode=1;
    setButtons();
    createmap(1);
    update();
}

void Extract::MapToPAC(){
    mapmode=2;
    setButtons();
    createmap(1);
    update();
}

void Extract::MapToIND(){
    mapmode=3;
    setButtons();
    createmap(1);
    update();
}

void Extract::MapToMED(){
    mapmode=4;
    setButtons();
    createmap(1);
    update();
}

void Extract::setButtons(){
    //if (label_flag==0) ui->pushButton_10->setStyleSheet("background-color: lightGray"); else ui->pushButton_10->setStyleSheet("background-color: darkGray");
    if (mapmode==0) ui->pushButton_6->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;");
    else ui->pushButton_6->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==1) ui->pushButton_7->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_7->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==2) ui->pushButton_8->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_8->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==3) ui->pushButton_9->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_9->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==4) ui->pushButton_10->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_10->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
}

void Extract::createmap(int first){
    if (mapmode==0){
        delete[] mapdata;
        delete[] com;
        delete[] lab;
        delete[] dotcol;
        delete[] dotsize;

        mapdata=new float[entries*4];
        com=new QString[entries*4];
        lab=new QString[entries*4];
        dotcol=new QColor[entries*4];
        dotsize=new int[4];
        dotsize[0]=8;
        dotsize[1]=8;
        dotsize[2]=5;
        dotsize[3]=5;

        // estimate min and max of datavalues
        float map_min=99999999;
        float map_max=-99999999;
        if (ui->checkBox->isChecked()){
            map_min=global_min;
            map_max=global_max;
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        } else {
            for (int i=0;i<entries;i++){

                    if (!isnan(result_mid.at(ui->horizontalSlider->value()+size*i))){
                        if (map_min>result_mid.at(ui->horizontalSlider->value()+size*i)) map_min=result_mid.at(ui->horizontalSlider->value()+size*i);
                        if (map_max<result_mid.at(ui->horizontalSlider->value()+size*i)) map_max=result_mid.at(ui->horizontalSlider->value()+size*i);
                    }


            }
            qDebug()<<"Limits:"+QString::number(map_min)+":"+QString::number(map_max);
            if (map_min==map_max) {

                map_max=map_min+1;
            }


            if (map_min>map_max) {
                map_min=0;
                map_max=map_min+1;
            }
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        }


        for (int i=0;i<entries;i++){
            mapdata[i+2*entries]=result_long.at(i);
            mapdata[i+3*entries]=result_lat.at(i);
            com[i+2*entries]=result_core.at(i)+"\n"+result_species.at(i);
            //com[i+3*entries]="Water Depth [m]: "+QString::number(result_wd.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
            com[i+0*entries]=result_core.at(i)+"\n"+result_species.at(i);
            //com[i+1*entries]="Water Depth [m]: "+QString::number(result_wd.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
            lab[i+2*entries]=result_core.at(i);
            lab[i+0*entries]=result_core.at(i);
            dotcol[i+2*entries]=Qt::black;
            dotcol[i+3*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
            if (i==selected_entry) {
                mapdata[i+0*entries]=result_long.at(i);
                mapdata[i+1*entries]=result_lat.at(i);
                mapdata[i+2*entries]=NAN;
                mapdata[i+3*entries]=NAN;
                dotcol[i+0*entries]=Qt::red;
                dotcol[i+1*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
            } else {
                mapdata[i+0*entries]=NAN;
                mapdata[i+1*entries]=NAN;
                dotcol[i+0*entries]=Qt::black;
                dotcol[i+1*entries]=Qt::black;
            }



        }
        map->setData(mapdata,4,entries);
        map->setSetSymbolsize(dotsize,1);

        map->setMultiplicator(1,1);

        map->setColor(dotcol,true);
        map->setTitel("","Longitude","Latitude");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);


        map->setComment(com,2);

        map->setLabel(lab,1);
        map->setLockAspect(1);

        map->setRepeat(2,-180,180,0,-90,90);
        map->setLimit(-900,900,-90,90);

        if (first) {
            map->setComHide(0,1);
            map->setBackground(worldmap,-180,180,-90,90,1);
        }
        map->setFolding(1,0);
        if (first) map->setView((-(180.0/ui->graphicsView_2->size().height())*ui->graphicsView_2->size().width())/2.0,((180.0/ui->graphicsView_2->size().height())*ui->graphicsView_2->size().width())/2.0,-90,90);
        map->setLineStyle(Qt::NoPen);
        map->setColorLegend(1,ui->comboBox_3->currentText(),map_min,map_max);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract_Map.txt");

        ui->graphicsView_2->setScene(map);
    }
    // data for basins
    double ATL_depth_min=-5500;
    double ATL_depth_max=0;
    double ATL_latt_min=-90;
    double ATL_latt_max=90;
    double PAC_depth_min=-5500;
    double PAC_depth_max=0;
    double PAC_latt_min=-90;
    double PAC_latt_max=60;
    double IND_depth_min=-5500;
    double IND_depth_max=0;
    double IND_latt_min=-90;
    double IND_latt_max=30;
    double MED_depth_min=-5500;
    double MED_depth_max=0;
    double MED_latt_min=-5.6;
    double MED_latt_max=36.4;

    if (mapmode==1){

        delete[] mapdata;
        delete[] com;
        delete[] lab;
        delete[] dotcol;
        delete[] dotsize;

        mapdata=new float[entries*4];
        com=new QString[entries*4];
        lab=new QString[entries*4];
        dotcol=new QColor[entries*4];
        dotsize=new int[4];
        dotsize[0]=8;
        dotsize[1]=8;
        dotsize[2]=5;
        dotsize[3]=5;

        for (int i=0;i<entries*4;i++){
           mapdata[i]=NAN;
            com[i]="";
            lab[i]="";
            dotcol[i]=Qt::black;
        }

        // estimate min and max of datavalues
        float map_min=99999999;
        float map_max=-99999999;
        if (ui->checkBox->isChecked()){
            map_min=global_min;
            map_max=global_max;
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        } else {
            for (int i=0;i<entries;i++){
                    if (!isnan(result_mid.at(ui->horizontalSlider->value()+size*i))){
                        if (map_min>result_mid.at(ui->horizontalSlider->value()+size*i)) map_min=result_mid.at(ui->horizontalSlider->value()+size*i);
                        if (map_max<result_mid.at(ui->horizontalSlider->value()+size*i)) map_max=result_mid.at(ui->horizontalSlider->value()+size*i);
                    }


            }
            qDebug()<<"Limits:"+QString::number(map_min)+":"+QString::number(map_max);
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
            if (map_min==map_max) {

                map_max=map_min+1;
            }
            if (map_min>map_max) {
                map_min=0;
                map_max=map_min+1;
            }
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        }

        for (int i=0;i<entries;i++){
            if (inv->get_Basin(result_long.at(i),result_lat.at(i))==1){
               mapdata[i+2*entries]=result_lat.at(i);
               mapdata[i+3*entries]=-result_wd.at(i);
                com[i+2*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+3*entries]="Longitude [°]: "+QString::number(result_long.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                com[i+0*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+1*entries]="Longitude [°]: "+QString::number(result_long.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                lab[i+0*entries]=result_core.at(i);
                lab[i+2*entries]=result_core.at(i);
                dotcol[i+2*entries]=Qt::black;
                dotcol[i+3*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();

                if (inv->get_Selected(i)==1) {
                   mapdata[i+0*entries]=result_lat.at(i);
                   mapdata[i+1*entries]=-result_wd.at(i);
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                    dotcol[i+0*entries]=Qt::red;
                    dotcol[i+1*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                    dotcol[i+0*entries]=Qt::black;
                    dotcol[i+1*entries]=Qt::black;
                }
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                }

        }
        map->setData(mapdata,4,entries);
        map->setSetSymbolsize(dotsize,1);
        map->setMultiplicator(1,1);
        map->setColor(dotcol,true);
        map->setTitel("","Latitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,1);
        map->setLockAspect(0);

        map->setRepeat(0,ATL_latt_min,ATL_latt_max,0,ATL_depth_min,ATL_depth_max);
        map->setLimit(ATL_latt_min,ATL_latt_max,ATL_depth_min,ATL_depth_max);
        if (first) {
            // map->setComHide(0,1);
            map->setBackground(resources->filename_ATL,ATL_latt_min,ATL_latt_max,ATL_depth_min,ATL_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setColorLegend(1,ui->comboBox_3->currentText(),map_min,map_max);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract_Map1.txt");
        if (first) map->setView(ATL_latt_min,ATL_latt_max,ATL_depth_min,ATL_depth_max);
        ui->graphicsView_2->setScene(map);
    }
    if (mapmode==2){

        delete[]mapdata;
        delete[] com;
        delete[] lab;
        delete[] dotcol;
        delete[] dotsize;

       mapdata=new float[entries*4];
        com=new QString[entries*4];
        lab=new QString[entries*4];
        dotcol=new QColor[entries*4];
        dotsize=new int[4];
        dotsize[0]=8;
        dotsize[1]=8;
        dotsize[2]=5;
        dotsize[3]=5;
        for (int i=0;i<entries*4;i++){
           mapdata[i]=NAN;
            com[i]="";
            lab[i]="";
            dotcol[i]=Qt::black;
        }

        // estimate min and max of datavalues
        float map_min=99999999;
        float map_max=-99999999;
        if (ui->checkBox->isChecked()){
            map_min=global_min;
            map_max=global_max;
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        } else {
            for (int i=0;i<entries;i++){

                    if (!isnan(result_mid.at(ui->horizontalSlider->value()+size*i))){
                        if (map_min>result_mid.at(ui->horizontalSlider->value()+size*i)) map_min=result_mid.at(ui->horizontalSlider->value()+size*i);
                        if (map_max<result_mid.at(ui->horizontalSlider->value()+size*i)) map_max=result_mid.at(ui->horizontalSlider->value()+size*i);
                    }


            }
            qDebug()<<"Limits:"+QString::number(map_min)+":"+QString::number(map_max);

            if (map_min==map_max) {

                map_max=map_min+1;
            }
            if (map_min>map_max) {
                map_min=0;
                map_max=map_min+1;
            }
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        }

        for (int i=0;i<entries;i++){
            if (inv->get_Basin(result_long.at(i),result_lat.at(i))==2){
               mapdata[i+2*entries]=result_lat.at(i);
               mapdata[i+3*entries]=-result_wd.at(i);
                com[i+2*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+3*entries]="Longitude [°]: "+QString::number(result_long.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                com[i+0*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+1*entries]="Longitude [°]: "+QString::number(result_long.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                lab[i+0*entries]=result_core.at(i);
                lab[i+2*entries]=result_core.at(i);
                dotcol[i+2*entries]=Qt::black;
                dotcol[i+3*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();

                if (inv->get_Selected(i)==1) {
                   mapdata[i+0*entries]=result_lat.at(i);
                   mapdata[i+1*entries]=-result_wd.at(i);
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                    dotcol[i+0*entries]=Qt::red;
                    dotcol[i+1*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                    dotcol[i+0*entries]=Qt::black;
                    dotcol[i+1*entries]=Qt::black;
                }
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                }

        }
        map->setData(mapdata,4,entries);
        map->setSetSymbolsize(dotsize,1);
        map->setMultiplicator(1,1);
        map->setColor(dotcol,true);
        map->setTitel("","Latitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,1);
        map->setLockAspect(0);

        map->setRepeat(0,PAC_latt_min,PAC_latt_max,0,PAC_depth_min,PAC_depth_max);
        map->setLimit(PAC_latt_min,PAC_latt_max,PAC_depth_min,PAC_depth_max);

        if (first) {
           //  map->setComHide(0,1);
            map->setBackground(resources->filename_PAC,PAC_latt_min,PAC_latt_max,PAC_depth_min,PAC_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setColorLegend(1,ui->comboBox_3->currentText(),map_min,map_max);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract_Map2.txt");
        if (first) map->setView(PAC_latt_min,PAC_latt_max,PAC_depth_min,PAC_depth_max);
        ui->graphicsView_2->setScene(map);
    }
    if (mapmode==3){
        delete[]mapdata;
        delete[] com;
        delete[] lab;
        delete[] dotcol;
        delete[] dotsize;

       mapdata=new float[entries*4];
        com=new QString[entries*4];
        lab=new QString[entries*4];
        dotcol=new QColor[entries*4];
        dotsize=new int[4];
        dotsize[0]=8;
        dotsize[1]=8;
        dotsize[2]=5;
        dotsize[3]=5;
        for (int i=0;i<entries*4;i++){
           mapdata[i]=NAN;
            com[i]="";
            lab[i]="";
            dotcol[i]=Qt::black;
        }

        // estimate min and max of datavalues
        float map_min=99999999;
        float map_max=-99999999;
        if (ui->checkBox->isChecked()){
            map_min=global_min;
            map_max=global_max;
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        } else {
            for (int i=0;i<entries;i++){
                    if (!isnan(result_mid.at(ui->horizontalSlider->value()+size*i))){
                        if (map_min>result_mid.at(ui->horizontalSlider->value()+size*i)) map_min=result_mid.at(ui->horizontalSlider->value()+size*i);
                        if (map_max<result_mid.at(ui->horizontalSlider->value()+size*i)) map_max=result_mid.at(ui->horizontalSlider->value()+size*i);
                    }


            }
            qDebug()<<"Limits:"+QString::number(map_min)+":"+QString::number(map_max);
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
            if (map_min==map_max) {

                map_max=map_min+1;
            }
            if (map_min>map_max) {
                map_min=0;
                map_max=map_min+1;
            }
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        }

        for (int i=0;i<entries;i++){
            if (inv->get_Basin(result_long.at(i),result_lat.at(i))==3){
               mapdata[i+2*entries]=result_lat.at(i);
               mapdata[i+3*entries]=-result_wd.at(i);
                com[i+2*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+3*entries]="Longitude [°]: "+QString::number(result_long.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                com[i+0*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+1*entries]="Longitude [°]: "+QString::number(result_long.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                lab[i+0*entries]=result_core.at(i);
                lab[i+2*entries]=result_core.at(i);
                dotcol[i+2*entries]=Qt::black;
                dotcol[i+3*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();

                if (inv->get_Selected(i)==1) {
                   mapdata[i+0*entries]=result_lat.at(i);
                   mapdata[i+1*entries]=-result_wd.at(i);
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                    dotcol[i+0*entries]=Qt::red;
                    dotcol[i+1*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                    dotcol[i+0*entries]=Qt::black;
                    dotcol[i+1*entries]=Qt::black;
                }
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                }

        }
        map->setData(mapdata,4,entries);
        map->setSetSymbolsize(dotsize,1);
        map->setMultiplicator(1,1);
        map->setColor(dotcol,true);
        map->setTitel("","Latitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,1);
        map->setLockAspect(0);

        map->setRepeat(0,IND_latt_min,IND_latt_max,0,IND_depth_min,IND_depth_max);
        map->setLimit(IND_latt_min,IND_latt_max,IND_depth_min,IND_depth_max);
        if (first) {
            // map->setComHide(0,1);
            map->setBackground(resources->filename_IND,IND_latt_min,IND_latt_max,IND_depth_min,IND_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setColorLegend(1,ui->comboBox_3->currentText(),map_min,map_max);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract_Map3.txt");
        if (first) map->setView(IND_latt_min,IND_latt_max,IND_depth_min,IND_depth_max);
        ui->graphicsView_2->setScene(map);
    }
    if (mapmode==4){
        delete[]mapdata;
        delete[] com;
        delete[] lab;
        delete[] dotcol;
        delete[] dotsize;

       mapdata=new float[entries*4];
        com=new QString[entries*4];
        lab=new QString[entries*4];
        dotcol=new QColor[entries*4];
        dotsize=new int[4];
        dotsize[0]=8;
        dotsize[1]=8;
        dotsize[2]=5;
        dotsize[3]=5;
        for (int i=0;i<entries*4;i++){
           mapdata[i]=NAN;
            com[i]="";
            lab[i]="";
            dotcol[i]=Qt::black;
        }

        // estimate min and max of datavalues
        float map_min=99999999;
        float map_max=-99999999;
        if (ui->checkBox->isChecked()){
            map_min=global_min;
            map_max=global_max;
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
        } else {
            for (int i=0;i<entries;i++){

                    if (!isnan(result_mid.at(ui->horizontalSlider->value()+size*i))){
                        if (map_min>result_mid.at(ui->horizontalSlider->value()+size*i)) map_min=result_mid.at(ui->horizontalSlider->value()+size*i);
                        if (map_max<result_mid.at(ui->horizontalSlider->value()+size*i)) map_max=result_mid.at(ui->horizontalSlider->value()+size*i);
                    }


            }
            qDebug()<<"Limits:"+QString::number(map_min)+":"+QString::number(map_max);
            map_min-=((map_max-map_min)*0.2);
            map_max+=((map_max-map_min)*0.2);
            if (map_min==map_max) {

                map_max=map_min+1;
            }
            if (map_min>map_max) {
                map_min=0;
                map_max=map_min+1;
            }
        }

        for (int i=0;i<entries;i++){
            if (inv->get_Basin(result_long.at(i),result_lat.at(i))==4){
               mapdata[i+2*entries]=result_long.at(i);
               mapdata[i+3*entries]=-result_wd.at(i);
                com[i+2*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+3*entries]="Latitude [°]: "+QString::number(result_lat.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                com[i+0*entries]=result_core.at(i)+"\n"+result_species.at(i);
                //com[i+1*entries]="Latitude [°]: "+QString::number(result_lat.at(i))+"\n"+ui->comboBox_3->currentText()+": "+QString::number(result_Q50.at(ui->horizontalSlider->value()+size*i));
                lab[i+0*entries]=result_core.at(i);
                lab[i+2*entries]=result_core.at(i);
                dotcol[i+2*entries]=Qt::black;
                dotcol[i+3*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
                if (inv->get_Selected(i)==1) {
                   mapdata[i+0*entries]=result_long.at(i);
                   mapdata[i+1*entries]=-result_wd.at(i);
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                    dotcol[i+0*entries]=Qt::red;
                   dotcol[i+1*entries]=eval_Color(result_mid.at(ui->horizontalSlider->value()+size*i),map_min,map_max).rgba();
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                    dotcol[i+0*entries]=Qt::black;
                    dotcol[i+1*entries]=Qt::black;
                }
                } else {
                   mapdata[i+0*entries]=NAN;
                   mapdata[i+1*entries]=NAN;
                   mapdata[i+2*entries]=NAN;
                   mapdata[i+3*entries]=NAN;
                }

        }
        map->setData(mapdata,4,entries);
        map->setSetSymbolsize(dotsize,1);
        map->setMultiplicator(1,1);
        map->setColor(dotcol,true);
        map->setTitel("","Longitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,1);
        map->setLockAspect(0);

        map->setRepeat(0,MED_latt_min,MED_latt_max,0,MED_depth_min,MED_depth_max);
        map->setLimit(MED_latt_min,MED_latt_max,MED_depth_min,MED_depth_max);
        if (first) {
            // map->setComHide(0,1);
            map->setBackground(resources->filename_MED,MED_latt_min,MED_latt_max,MED_depth_min,MED_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setColorLegend(1,ui->comboBox_3->currentText(),map_min,map_max);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Extract_Map4.txt");
        if (first) map->setView(MED_latt_min,MED_latt_max,MED_depth_min,MED_depth_max);
        ui->graphicsView_2->setScene(map);
    }
    ui->graphicsView_3->repaint();
}
QColor Extract::eval_Color(float v,float min,float max){
    QColor col;
    if (!isnan(v) && v>=min && v<=max){

        col=colorScheme->getColor(v,min,max,map->getColorScheme(0));
    } else {
        col=QColor(128,128,128,255);
    }
    return col;

}

void Extract::selected_changed(int p, int x, int y){
    selected_entry=y;
    plot();
    createmap(0);
    setupExtractTable();
    setupInventory();
}

void Extract::setupExtractTable(){



    // create the model for Inventory
    delete modelList;
    modelList = new QStandardItemModel(entries,6,this);
    modelList->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelList->setHorizontalHeaderItem(1, new QStandardItem(inv->access_inv_name(0)));
    modelList->setHorizontalHeaderItem(2, new QStandardItem(inv->access_inv_name(1)));
    modelList->setHorizontalHeaderItem(3, new QStandardItem(inv->access_inv_name(2)+"\n["+inv->access_inv_unit(2)+"]"));
    modelList->setHorizontalHeaderItem(4, new QStandardItem(inv->access_inv_name(3)+"\n["+inv->access_inv_unit(3)+"]"));
    modelList->setHorizontalHeaderItem(5, new QStandardItem(inv->access_inv_name(4)+"\n["+inv->access_inv_unit(4)+"]"));


    //...
    ui->tableView_2->setModel(modelList);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView_2->setFont(font);
    ui->tableView_2->horizontalHeader()->setFont(font);
    ui->tableView_2->verticalHeader()->setFont(font);

    int pos=0;

    for (int i=0;i<entries;i++){



            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            var_Index->setData(i,Qt::EditRole);
            modelList->setItem(pos,0,var_Index);
            if (i==selected_entry){
                modelList->setData(modelList->index(pos, 0), QColor(Qt::lightGray), Qt::BackgroundRole);
            }

            QStandardItem *var1=new QStandardItem(result_core.at(i));
            modelList->setItem(pos,1,var1);
            if (i==selected_entry){
                modelList->setData(modelList->index(pos, 1), QColor(Qt::lightGray), Qt::BackgroundRole);
            }

            QStandardItem *var2=new QStandardItem(result_species.at(i));
            modelList->setItem(pos,2,var2);
            if (i==selected_entry){
                modelList->setData(modelList->index(pos, 2), QColor(Qt::lightGray), Qt::BackgroundRole);
            }

            QStandardItem *var3=new QStandardItem(QString::number(result_long.at(i)));
            modelList->setItem(pos,3,var3);
            if (i==selected_entry){
                modelList->setData(modelList->index(pos, 3), QColor(Qt::lightGray), Qt::BackgroundRole);
            }

            QStandardItem *var4=new QStandardItem(QString::number(result_lat.at(i)));
            modelList->setItem(pos,4,var4);
            if (i==selected_entry){
                modelList->setData(modelList->index(pos, 4), QColor(Qt::lightGray), Qt::BackgroundRole);
            }

            QStandardItem *var5=new QStandardItem(QString::number(result_wd.at(i)));
            modelList->setItem(pos,5,var5);
            if (i==selected_entry){
                modelList->setData(modelList->index(pos, 5), QColor(Qt::lightGray), Qt::BackgroundRole);
            }

            pos++;

    }




    ui->tableView_2->setSortingEnabled(1);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setHorizontalScrollMode(ui->tableView_2->ScrollPerPixel);





ui->tableView_2->repaint();
}

void Extract::AMSSummery(){
    qDebug()<<"creating ams inventory";
    ams_min_age.clear();
    ams_max_age.clear();
    ams_bacon_iter.clear();
    ams_min_age.reserve(inv->get_Entries());
    ams_max_age.reserve(inv->get_Entries());
    ams_bacon_iter.reserve(inv->get_Entries());
    for (int i=0;i<inv->get_Entries();i++){
        inv->set_currentCore(i);
        if (inv->get_AgeModel(i)==1){
            AMSData* ams=new AMSData(inv);

            ams->AMSRead();
            ams_bacon_iter.push_back(ams->get_bacon_out_nrow()-ams->get_bacon_burnin());
            double min=INFINITY;
            double max=-INFINITY;
            for (int ii=0;ii<ams->get_Length();ii++) {
                if (ams->get_Data(7,ii)==1){
                    if (ams->get_Data(4,ii)<min) min=ams->get_Data(4,ii);
                    if (ams->get_Data(4,ii)>max) max=ams->get_Data(4,ii);
                }
            }
            if (min==INFINITY) min=NAN;
            if (max==-INFINITY) max=NAN;
            ams_min_age.push_back(min);
            ams_max_age.push_back(max);
            delete ams;
        } else {
            ams_min_age.push_back(NAN);
            ams_max_age.push_back(NAN);
            ams_bacon_iter.push_back(NAN);
        }
    }
    qDebug()<<"finished";
}

