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

#include "extract_single.h"
#include "ui_extract_single.h"

Extract_Single::Extract_Single(QMainWindow *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv_main(inventory),proxy(str),
    ui(new Ui::Extract_Single)
{
    ui->setupUi(this);
    this->setWindowTitle("Extract a single ensemble");
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();

    qApp->installEventFilter(this);
    ui->pushButton->setFocusPolicy(Qt::NoFocus);
    resources=new Resources();

    inv=new Inventory(proxy);
    inv->read();
    inv->read_Basin();
    for(int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
    for(int i=0;i<inv->get_Entries();i++) inv->set_AgeModel(i,inv_main->get_AgeModel(i));


    data=new float[0];
    coll=new QColor[0];
    lstyle=new Qt::PenStyle[0];
    pl1=new Graph(this,data,0,0);




    this->setWindowTitle("Single time series ensemble");






    //start=ui->doubleSpinBox->value();
    //inc=ui->doubleSpinBox_4->value();
    //end=ui->doubleSpinBox_2->value();

    // prepare drop box for dataselection
    ui->comboBox_3->clear();
    for (int i=0;i<inv->access_data_length();i++){
        ui->comboBox_3->addItem(inv->access_data_name(i));
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

    result_Q05.clear();
    result_Q50.clear();
    result_Q95.clear();
    result_age.clear();
    result_age05.clear();
    result_age95.clear();
    result_depth.clear();

    save_OK=0;
    view_only=0;

    //modelInventory = new QStandardItemModel(1,1,this);
    setupInventory();

    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(invSelected(QModelIndex)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));
    connect(ui->comboBox_3,SIGNAL(currentIndexChanged(int)),this,SLOT(setup()));
    //connect(ui->doubleSpinBox,SIGNAL(editingFinished()),this,SLOT(setup()));
    //connect(ui->doubleSpinBox_2,SIGNAL(editingFinished()),this,SLOT(setup()));
    //connect(ui->doubleSpinBox_4,SIGNAL(editingFinished()),this,SLOT(setup()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(calc()));
    //connect(ui->radioButton,SIGNAL(clicked(bool)),this,SLOT(setup()));
    connect(this,SIGNAL(selectionChanged()),mainW,SLOT(redraw_score()));// refresh main window
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(read()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(pl1,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),this,SLOT(sortVerify()));
}

Extract_Single::~Extract_Single()
{
    delete ui;
    delete resources;
    //delete modelInventory;
    delete[] data;
    delete[] coll;
    delete[] lstyle;
}

void Extract_Single::setupInventory(){
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
    //delete modelInventory;
    modelInventory = new QStandardItemModel(sum,5+inv->access_inv_length(),this);
    modelInventory->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelInventory->setHorizontalHeaderItem(1, new QStandardItem(QString("Selected")));
    for (int i=0;i<inv->access_inv_length();i++){
        modelInventory->setHorizontalHeaderItem(2+i, new QStandardItem(inv->access_inv_name(i)+"\n"+inv->access_inv_unit(i)));
    }
    modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length(), new QStandardItem(QString("Age Min")));
    modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length()+1, new QStandardItem(QString("Age Max")));
    modelInventory->setHorizontalHeaderItem(2+inv->access_inv_length()+2, new QStandardItem(QString("Bacon")));


    //...
    ui->tableView->setModel(modelInventory);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView->setFont(font);
    ui->tableView->horizontalHeader()->setFont(font);
    ui->tableView->verticalHeader()->setFont(font);



    int pos=0;

    AMSData* ams=new AMSData(inv);

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
                            modelInventory->setData(modelInventory->index(pos, 2+j), QColor(Qt::lightGray), Qt::BackgroundRole);
                        }
                    }
                    if (inv->access_inv_type(j).contains("float") || inv->access_inv_type(j).contains("int")){
                        QStandardItem *var=new QStandardItem(inv->access_inv_data(j,i));
                        var->setData(inv->access_inv_data(j,i).toFloat(),Qt::EditRole);
                        modelInventory->setItem(pos,2+j,var);
                        if (inv->get_Selected(i)){
                            modelInventory->setData(modelInventory->index(pos, 2+j), QColor(Qt::lightGray), Qt::BackgroundRole);
                        }
                    }
                }
            }
            inv->set_currentCore(i);
            ams->AMSRead();
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



            QStandardItem *var_AgeMin=new QStandardItem(QString::number(min));
            var_AgeMin->setData(min,Qt::EditRole);
            modelInventory->setItem(pos,2+inv->access_inv_length(),var_AgeMin);
            QStandardItem *var_AgeMax=new QStandardItem(QString::number(max));
            var_AgeMax->setData(max,Qt::EditRole);
            modelInventory->setItem(pos,2+inv->access_inv_length()+1,var_AgeMax);

            QStandardItem *var_Bacon=new QStandardItem(QString::number(ams->get_bacon_out_nrow()-ams->get_bacon_burnin()));
            var_Bacon->setData(ams->get_bacon_out_nrow()-ams->get_bacon_burnin(),Qt::EditRole);
            modelInventory->setItem(pos,2+inv->access_inv_length()+2,var_Bacon);





            // coloring
            if (inv->get_Selected(i)){
                for (int j=0;j<15;j++) modelInventory->setData(modelInventory->index(pos, j), QColor(Qt::lightGray), Qt::BackgroundRole);
            }


            pos++;
        }
    }


    delete ams;
    delete[] filter;

    ui->tableView->setSortingEnabled(1);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);





ui->tableView->repaint();
}

void Extract_Single::sortVerify(){
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

void Extract_Single::invSelected(QModelIndex mi){
    if (!view_only){
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
            // deselect everything else
            for (int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
            inv->set_Selected(index,1);
        }
        //emit(selectionChanged());
    }
    inv->set_currentCore(index);
    }
    plot();
    setupInventory();

}

bool Extract_Single::checkFilter(int i){
    bool ok=true;

    // check Ocean
    if (inv->get_Basinname(inv->get_Longitude(i),inv->get_Latitude(i))!=ui->comboBox_2->currentText() && ui->comboBox_2->currentText()!="Global") ok=false;

    // Selected
    if (inv->get_Selected(i)==0 && ui->comboBox->currentText()=="Selected Only") ok=false;


    return ok;
}


void Extract_Single::setup(){
    data_index=ui->comboBox_3->currentIndex();
    result_Q05.clear();
    result_Q50.clear();
    result_Q95.clear();
    result_age.clear();
    result_age05.clear();
    result_age95.clear();
    result_depth.clear();


    setupInventory();
}

void Extract_Single::calc(){
    // setup results(again)

    //start=ui->doubleSpinBox->value();
    //inc=ui->doubleSpinBox_4->value();
    //end=ui->doubleSpinBox_2->value();
    int index=0;

    for (index=0;index<inv->get_Entries();index++) if (inv->get_Selected(index)) break;
    inv->set_currentCore(index);
    inv->readData(index);

    int count=inv->get_Length();

        result_Q05.clear();
        result_Q50.clear();
        result_Q95.clear();
        result_age.clear();
        result_age05.clear();
        result_age95.clear();
        result_depth.clear();

        for (int j=0;j<count;j++){
            result_Q05.push_back(NAN);
            result_Q50.push_back(NAN);
            result_Q95.push_back(NAN);
            result_age.push_back(NAN);
            result_age05.push_back(NAN);
            result_age95.push_back(NAN);
            result_depth.push_back(NAN);
        }







    // Go through all inventory
    for (int i=0;i<inv->get_Entries();i++){
        inv->set_currentCore(i);
        inv->readData(inv->get_currentCore());
        if (inv->get_Selected(i)){
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

                    ui->plainTextEdit->appendPlainText("    Adding some noise to proxy data...");
                    repaint();
                    float* noisy=new float[inv->get_Length()*out_nrow];
                    for (int iter=0;iter<out_nrow;iter++){
                        for (int j=0;j<inv->get_Length();j++) {

                            if ((isnan(source_error[j])==true || source_error[j]==0) || ui->radioButton->isChecked()==false) {
                               noisy[j+iter*inv->get_Length()]=source_data[j]+gauss(0,1)*ui->doubleSpinBox_6->value();
                            }else{
                               noisy[j+iter*inv->get_Length()]=source_data[j]+gauss(0,1)*source_error[j];
                            }
                        }
                    }
/*
                    Editor *c=new Editor(this,noisy,noisy,ui->spinBox->value(),inv->get_Length());// This takes a lot of time!!!!
                    c->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                    c->show();
*/
                    // (4) ++++++ Interpolate bacon agemodels(ages) to have same depths(inv->get_Depth()) as isodata ++++++++++++++++++++
                    ui->plainTextEdit->appendPlainText("    Interpolating...");
                    repaint();
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
                    // (5) Interpolation Fit noisy and newages to new ageintervalls and interpolate new values
                    ui->plainTextEdit->appendPlainText("    Aligning Bacon and proxy...");
                    repaint();

                    float* multi_data=new float[count*out_nrow];

                    for (int j=0;j<count*out_nrow;j++)multi_data[j]=NAN;
                    for (int j=0;j<count;j++){// fill for all new ages
                        AgeModel(ams);
                        float age=result_age[j]*1000;//ui->doubleSpinBox->value()+j*ui->doubleSpinBox_4->value();



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
                    }
                    delete[] newages;

/*
                        Editor *cc=new Editor(this,multi_d18O,multi_d18O,out_nrow,count);// This takes a lot of time!!!!
                        cc->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                        cc->show();
*/

                    // (6) calculate 5 50 95 Quantile for all ages
                    // sort values in Iteration and estimate Quantiles
                    // set nans to 999999 and count elements
                    ui->plainTextEdit->appendPlainText("    Estimating Quantiles...");
                    repaint();
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



                    for (int j=0;j<count;j++){
                        sort(multi_data,j,count,0, out_nrow-1);
                        // get Quantiles
                        float v=multi_data[j+count*((int)(c[j]*0.023))];
                        if (v!=9999999 && c[j]/count>0.5) result_Q05[j]=v;

                        v=multi_data[j+count*((int)(c[j]*0.5))];
                        if (v!=9999999 && c[j]/count>0.5) result_Q50[j]=v;

                        v=multi_data[j+count*((int)(c[j]*0.977))];
                        if (v!=9999999 && c[j]/count>0.5) result_Q95[j]=v;



                    }

                    delete[] c;
                    delete[] multi_data;



                    delete[] noisy;
                    delete[] source_depth;
                    delete[] source_data;
                    delete[] source_error;
                    delete[] source_flag;



                } else {
                    // What to do if no bacon data found?
                    ui->plainTextEdit->appendPlainText("    NO BACON AGE-MODEL FOUND!!!");
                    repaint();
                }
                delete ams;


            } else {
                // No Agemodel found

            }

        }

    }




    ui->plainTextEdit->appendPlainText("finished!!!");
    // select first choosen dataset
    for(int i=0;i<inv->get_Entries();i++) if(inv->get_Selected(i)==1){
        inv->set_currentCore(i);
        break;
    }
    plot();
    repaint();
    save_OK=1;



}

void Extract_Single::sort(float* data,int j,int count,int left,int right){
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
double Extract_Single::gauss(double mu, double sigma)
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

void Extract_Single::plot(){
    int index=0;

    for (index=0;index<inv->get_Entries();index++) if (inv->get_Selected(index)) break;
    inv->set_currentCore(index);
    inv->readData(index);
    if (index>=0&&index<=inv->get_Entries()&&result_age.size()>0){

        delete[] data;
        data=new float[6*inv->get_Length()];
        for(int i=0;i<6*inv->get_Length();i++) data[i]=NAN;
        delete[] coll;
        coll=new QColor[6];
        for (int i=0;i<6;i++) coll[i]=QColor(0,0,0);
        delete[] lstyle;
        lstyle=new Qt::PenStyle[6];
        for (int i=0;i<6;i++) lstyle[i]=Qt::DashLine;
        for(int i=0;i<inv->get_Length();i++){


                data[0*inv->get_Length()+i]=result_age[i];
                data[1*inv->get_Length()+i]=result_Q05[i];

                data[2*inv->get_Length()+i]=result_age[i];
                data[3*inv->get_Length()+i]=result_Q50[i];

                data[4*inv->get_Length()+i]=result_age[i];
                data[5*inv->get_Length()+i]=result_Q95[i];


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
        pl1->setLineWidth(3);

            pl1->setData(data,6,inv->get_Length());
            pl1->setTitel(QString::number(index)+" : "+inv->get_Core(index)+" : "+inv->get_Species(index),"Age [ka BP]",inv->access_data_name(data_index)+" ["+inv->access_data_unit(data_index)+"]");
            pl1->setMultiplicator(1,1);
            pl1->setTextSize(resources->get_Pointsize(),12,12);

            pl1->setSetLineColor(coll,1);
            pl1->setSetLineStyle(lstyle,1);
            pl1->setSize(ui->graphicsView->width(),ui->graphicsView->height());
            pl1->setSymbol(0);
            pl1->setSettings(resources->path_PaleoDataView+"/Resources/Plot/ExtractS_"+inv->access_proxy_abbreviation()+".txt");
            pl1->autoSize();
            ui->graphicsView->setScene(pl1);


    }
    ui->graphicsView->repaint();
}

void Extract_Single::paintEvent(QPaintEvent *)
{
    pl1->setSize(ui->graphicsView->width(),ui->graphicsView->height());
}



void Extract_Single::save(){
    int size=inv->get_Length();
    int index=0;
    for (index=0;index<inv->get_Entries();index++) if (inv->get_Selected(index)) break;

    if (save_OK==1 && size>0 && result_Q50.size()>0){
    QString Qfilename = QFileDialog::getSaveFileName(this, tr("Select Save File"),
                                             resources->get_path_data()+"/"+proxy+"/Derived Data/data "+ui->comboBox_3->currentText(),
                                             tr("Data Compilation (*.dc);;ASCII (*.txt);;Excel (*.xlsx)"));




    // get number of datasets
    int length=0;
    //for (int i=0;i<inv->get_Entries();i++){
        int ok=0;
        for (int j=0;j<size;j++) {
            if (isnan(result_Q50[j])==false) {
                ok=1;
                break;
            }
        }
        if (ok==1) length++;
    //}
    if (length!=0){
    ui->plainTextEdit->appendPlainText("Saving to File : "+Qfilename);
    repaint();
    // +++++++++ Preparing data
    // Meta attributes
    // -starting age
    // -increment
    // -ending age
    // Core Meta
    QString* core_name=new QString[length];
    QString* species_name=new QString[length];
    float* core_latt=new float[length];
    float* core_long=new float[length];
    float* core_depth=new float[length];
    float* Q05=new float[size];
    float* Q50=new float[size];
    float* Q95=new float[size];
    float* age=new float[size];
    float* depth=new float[size];
    float* age_min=new float[size];
    float* age_max=new float[size];

    int count=0;
    //for (int i=0;i<inv->get_Entries();i++){
        int ok=0;
        for (int j=0;j<size;j++) {
            if (isnan(result_Q50[j])==false) {
                ok=1;
                break;
            }
        }
        if (ok==1) {
            core_name[count]=inv->get_Core(index);
            species_name[count]=inv->get_Species(index);
            core_latt[count]=round(inv->get_Latitude(index)*1000.0)/1000.0;
            core_long[count]=round(inv->get_Longitude(index)*1000.0)/1000.0;
            core_depth[count]=round(inv->get_Water_Depth(index)*1000.0)/1000.0;
            for (int j=0;j<size;j++){
                Q05[j+count*size]=round(result_Q05[j]*1000.0)/1000.0;
                Q50[j+count*size]=round(result_Q50[j]*1000.0)/1000.0;
                Q95[j+count*size]=round(result_Q95[j]*1000.0)/1000.0;
                age[j+count*size]=round(result_age[j]*1000.0)/1000.0;
                age_min[j+count*size]=round(result_age05[j]*1000.0)/1000.0;
                age_max[j+count*size]=round(result_age95[j]*1000.0)/1000.0;
                depth[j+count*size]=round(result_depth[j]*1000.0)/1000.0;
            }
            count++;
        }
    //}


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
            if ((retval = nc_def_dim(ncid,"Length",length,&length_dim)))
               ERR(retval);
            dim[0]=length_dim;

            // Character Strings for identification
            // Corename
            int corename_length=0;
            for (int i=0;i<length;i++) if (corename_length<core_name[i].length()) corename_length=core_name[i].length();
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
            for (int i=0;i<length;i++) if (species_length<species_name[i].length()) species_length=species_name[i].length();
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
            if ((retval = nc_def_var(ncid,"Lattitude",NC_FLOAT,1,dim,&latt_id)))
                ERR(retval);
            // depth
            int wdepth_id;
            if ((retval = nc_def_var(ncid,"Water Depth",NC_FLOAT,1,dim,&wdepth_id)))
                ERR(retval);



            // set size of Q matrix
            int size_dim;
            int Q05_id;
            int Q50_id;
            int Q95_id;
            int age_id;
            int age_min_id;
            int age_max_id;
            int depth_id;
            if ((retval = nc_def_dim(ncid,"Size",size,&size_dim)))
               ERR(retval);
            dimm[0]=length_dim;
            dimm[1]=size_dim;
           //qDebug()<<dim[0];
           //qDebug()<<dim[1];
            if ((retval = nc_def_var(ncid,"Depth",NC_FLOAT,2,dimm,&depth_id)))
                ERR(retval);
            if ((retval = nc_def_var(ncid,"Age",NC_FLOAT,2,dimm,&age_id)))
                ERR(retval);
            if ((retval = nc_def_var(ncid,"Age_5%",NC_FLOAT,2,dimm,&age_min_id)))
                ERR(retval);
            if ((retval = nc_def_var(ncid,"Age_95%",NC_FLOAT,2,dimm,&age_max_id)))
                ERR(retval);
            if ((retval = nc_def_var(ncid,"Quantile_023",NC_FLOAT,2,dimm,&Q05_id)))
                ERR(retval);
            if ((retval = nc_def_var(ncid,"Quantile_50",NC_FLOAT,2,dimm,&Q50_id)))
                ERR(retval);
            if ((retval = nc_def_var(ncid,"Quantile_977",NC_FLOAT,2,dimm,&Q95_id)))
                ERR(retval);

            //**********************************************

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // ******* write Core
            {
            unsigned char text[corename_length][length];
            for(int i=0;i<corename_length;i++) for(int ii=0;ii<length;ii++) text[i][ii]=' ';
            for (int i=0;i<length;i++){
                for (int j=0;j<core_name[i].length();j++) text[j][i]=core_name[i].at(j).toLatin1();
            }
            if ((retval = nc_put_var(ncid,corename_id,&text[0][0])))
                ERR(retval);
            }

            // ******* write Species
            {
            unsigned char text[species_length][length];
            for(int i=0;i<species_length;i++) for(int ii=0;ii<length;ii++) text[i][ii]=' ';
            for (int i=0;i<length;i++){
                for (int j=0;j<species_name[i].length();j++) text[j][i]=species_name[i].at(j).toLatin1();
            }
            if ((retval = nc_put_var(ncid,species_id,&text[0][0])))
                ERR(retval);
            }

            // ******** save latt
            if ((retval = nc_put_var(ncid,latt_id,&core_latt[0])))
                ERR(retval);
            // ******** save long
            if ((retval = nc_put_var(ncid,long_id,&core_long[0])))
                ERR(retval);
            // ******** save depth
            if ((retval = nc_put_var(ncid,wdepth_id,&core_depth[0])))
                ERR(retval);

            // ******** save depth
            if ((retval = nc_put_var(ncid,depth_id,&depth[0])))
                ERR(retval);

            // ******** save age
            if ((retval = nc_put_var(ncid,age_id,&age[0])))
                ERR(retval);

            // ******** save age_min
            if ((retval = nc_put_var(ncid,age_min_id,&age_min[0])))
                ERR(retval);

            // ******** save age_max
            if ((retval = nc_put_var(ncid,age_max_id,&age_max[0])))
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

            // activate defMode
            if ((retval = nc_redef(ncid)))
                ERR(retval);

            // write attributes
            // 1:start
            //if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"Start_Age",NC_FLOAT,1,&start)))
            //    ERR(retval);
            // 2:increment
            //if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"Increment_Age",NC_FLOAT,1,&inc)))
            //    ERR(retval);
            // 3:end
            //if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"End_Age",NC_FLOAT,1,&end)))
            //    ERR(retval);

            // 3:noise
            float noise=(float)ui->doubleSpinBox_6->value();
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"Noise",NC_FLOAT,1,&noise)))
                ERR(retval);

            // 4:save parameter
            QString para=ui->comboBox_3->currentText();
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"Parameter",NC_CHAR,para.length(),para.toLatin1().data())))
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
        txt.append("Corename\tSpecies\tLatitude\tLongitude\tDepth\n");
        for (int i=0;i<length;i++){
            txt.append(core_name[i]+"\t"+species_name[i]+"\t"+QString::number(core_latt[i])+"\t"+QString::number(core_long[i])+"\t"+QString::number(core_depth[i])+"\n");
        }

        // write data
        txt.append("\n"+ui->comboBox_3->currentText());
        for (int i=0;i<length;i++) txt.append("\t"+core_name[i]+"\t"+species_name[i]+"\t");
        txt.append("\n");

        for (int i=0;i<length;i++) txt.append("Depth\tAge\tAge 5%\t Age \95%\tQ023\tQ50\tQ977");
        txt.append("\n");
        for (int i=0;i<size;i++) {
            //txt.append(QString::number(start+inc*i));
            for (int j=0;j<length;j++){
                txt.append(QString::number(depth[j*size+i])+"\t"+QString::number(age[j*size+i])+"\t"+QString::number(age_min[j*size+i])+"\t"+QString::number(age_max[j*size+i])+"\t"+QString::number(Q05[j*size+i])+"\t"+QString::number(Q50[j*size+i])+"\t"+QString::number(Q95[j*size+i]));
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

        for(int i=0;i<length;i++){
            xlsx.write("A"+QString::number(i+2),core_name[i]);
            xlsx.write("B"+QString::number(i+2),species_name[i]);
            xlsx.write("C"+QString::number(i+2),round(core_latt[i]*1000.0)/1000.0);
            xlsx.write("D"+QString::number(i+2),round(core_long[i]*1000.0)/1000.0);
            xlsx.write("E"+QString::number(i+2),round(core_depth[i]*1000.0)/1000.0);
        }

        xlsx.addSheet("Proxy Data", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Proxy Data");
        xlsx.setColumnWidth(1, length+1, 20);

        //xlsx.write("A2","Age in [kyr]");
        //for (int i=0;i<size;i++) xlsx.write("A"+QString::number(i+3),round((start+i*inc)*1000.0)/1000.0);

        for (int j=0;j<length;j++){
            // turn number into position
            int n=j*7;
            int hi=(int)(n/(26*26));
            int mid=(int)(n-hi*26*26)/26;
            int low=(int)(n-hi*26*26-mid*26);
            QString str="";
            if (hi>0)str.append(QChar(hi+64));
             if (mid>0)str.append(QChar(mid+64));
              str.append(QChar(low+65));
              //qDebug()<<str;
            xlsx.write(str+"1",core_name[j]);
            xlsx.write(str+"2","Depth");
            for (int i=0;i<size;i++) if (isnan(depth[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(depth[j*size+i]*1000.0)/1000.0);

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
            xlsx.write(str+"2","Age");
            for (int i=0;i<size;i++) if (isnan(age[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(age[j*size+i]*1000.0)/1000.0);

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
            xlsx.write(str+"2","Age Q023");
            for (int i=0;i<size;i++) if (isnan(age_min[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(age_min[j*size+i]*1000.0)/1000.0);

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
            xlsx.write(str+"2","Age Q977");
            for (int i=0;i<size;i++) if (isnan(age_max[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(age_max[j*size+i]*1000.0)/1000.0);

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
            xlsx.write(str+"2","Q023");
            for (int i=0;i<size;i++) if (isnan(Q05[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(Q05[j*size+i]*1000.0)/1000.0);


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
            xlsx.write(str+"2","Q50");
            for (int i=0;i<size;i++) if (isnan(Q50[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(Q50[j*size+i]*1000.0)/1000.0);

            n++;
            hi=(int)(n/(26*26));
            mid=(int)(n-hi*26*26)/26;
            low=(int)(n-hi*26*26-mid*26);
            str.clear();
            if (hi>0)str.append(QChar(hi+64));
             if (mid>0)str.append(QChar(mid+64));
              str.append(QChar(low+65));
              //qDebug()<<str;

            xlsx.write(str+"2","Q977");
            for (int i=0;i<size;i++) if (isnan(Q95[j*size+i])) xlsx.write(str+""+QString::number(i+3),"nan"); else xlsx.write(str+""+QString::number(i+3),round(Q95[j*size+i]*1000.0)/1000.0);
        }





        // save
        xlsx.saveAs(Qfilename);


        QDesktopServices::openUrl(QUrl("file:///"+Qfilename, QUrl::TolerantMode));

    }


    delete[] core_name;
    delete[] species_name;
    delete[] core_latt;
    delete[] core_long;
    delete[] core_depth;
    delete[] Q05;
    delete[] Q50;
    delete[] Q95;
    delete[] depth;
    delete[] age;
    delete[] age_min;
    delete[] age_max;
    } else {
        ui->plainTextEdit->appendPlainText("There is nothing to save!!!");
        repaint();
    }
    } else {
        ui->plainTextEdit->appendPlainText("First generate some data or read some from a file!!!");
        repaint();
    }

}

void Extract_Single::read(){

    QString Qfilename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources->get_path_data()+"/"+proxy+"/Derived Data/",
                                             tr("Data Compilation (*.dc)"));
    ui->plainTextEdit->appendPlainText("Reading from File : "+Qfilename);
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
        if ((retval = nc_inq_varid(ncid, "Quantile_023", &varid)))
           ERR(retval);
        float* Q05=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&Q05[0]))) ERR(retval);

        // read Q50
        if ((retval = nc_inq_varid(ncid, "Quantile_50", &varid)))
           ERR(retval);
        float* Q50=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&Q50[0]))) ERR(retval);

        // read Q95
        if ((retval = nc_inq_varid(ncid, "Quantile_977", &varid)))
           ERR(retval);
        float* Q95=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&Q95[0]))) ERR(retval);

        // read depth
        if ((retval = nc_inq_varid(ncid, "Depth", &varid)))
           ERR(retval);
        float* depth=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&depth[0]))) ERR(retval);

        // read age
        if ((retval = nc_inq_varid(ncid, "Age", &varid)))
           ERR(retval);
        float* age=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&age[0]))) ERR(retval);

        // read age_min
        if ((retval = nc_inq_varid(ncid, "Age_5%", &varid)))
           ERR(retval);
        float* age_min=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&age_min[0]))) ERR(retval);

        // read age_max
        if ((retval = nc_inq_varid(ncid, "Age_95%", &varid)))
           ERR(retval);
        float* age_max=new float[ncol*nrow];
        if ((retval = nc_get_var_float(ncid,varid,&age_max[0]))) ERR(retval);

        /*
        // 1:start
        float start_age;
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"Start_Age",&start_age)))
            ERR(retval);
        // 2:increment
        float inc_age;
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"Increment_Age",&inc_age)))
            ERR(retval);
*/
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

        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);

        // put data into shape
        ui->doubleSpinBox_6->setValue(noise);
        //start=start_age;
        //ui->doubleSpinBox->setValue(start);
        //inc=inc_age;
        //ui->doubleSpinBox_4->setValue(inc);
        //end=start_age+inc_age*dimlen[1];
        //ui->doubleSpinBox_2->setValue(end);
        //size=dimlen[1];
        ui->comboBox_3->clear();
        ui->comboBox_3->addItem(para);
        result_Q05.clear();
        result_Q50.clear();
        result_Q95.clear();
        result_age.clear();
        result_age05.clear();
        result_age95.clear();
        result_depth.clear();
        int count=0;

        for (int j=0;j<dimlen[1];j++){
            result_Q05.push_back(NAN);
            result_Q50.push_back(NAN);
            result_Q95.push_back(NAN);
            result_age.push_back(NAN);
            result_age05.push_back(NAN);
            result_age95.push_back(NAN);
            result_depth.push_back(NAN);

        }

        // copy to results
        for (int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
        for (int i=0;i<dimlen[0];i++){
            // find index
            int found=0;
            for (int j=0;j<inv->get_Entries();j++){
                if (inv->get_Core(j).simplified()==core[i]->toLatin1().simplified() && inv->get_Species(j).simplified()==species[i]->toLatin1().simplified()){
                    inv->set_Selected(j,1);
                    for (int q=0;q<dimlen[1];q++) result_Q05[q]=Q05[i*dimlen[1]+q];
                    for (int q=0;q<dimlen[1];q++) result_Q50[q]=Q50[i*dimlen[1]+q];
                    for (int q=0;q<dimlen[1];q++) result_Q95[q]=Q95[i*dimlen[1]+q];
                    for (int q=0;q<dimlen[1];q++) result_depth[q]=depth[i*dimlen[1]+q];
                    for (int q=0;q<dimlen[1];q++) result_age[q]=age[i*dimlen[1]+q];
                    for (int q=0;q<dimlen[1];q++) result_age05[q]=age_min[i*dimlen[1]+q];
                    for (int q=0;q<dimlen[1];q++) result_age95[q]=age_max[i*dimlen[1]+q];
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



        delete[] Q05;
        delete[] Q50;
        delete[] Q95;
        delete[] depth;
        delete[] age;
        delete[] age_min;
        delete[] age_max;

        setupInventory();
        plot();
        save_OK=1;
        view_only=1;
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



bool Extract_Single::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->tableView){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);
                ui->splitter_2->restoreState(sp_2);

                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void Extract_Single::updateGraph(){
    plot();
}

void Extract_Single::copy(QPoint pos){

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

void Extract_Single::AgeModel(AMSData *ams){
   for (int i=0;i<inv->get_Length();i++){
       float depth=inv->get_data_Isotopes(0,i)*100;
       result_depth[i]=depth;
       // calc value for age
       float value=NAN;
       int j=1;

       int nrow=ams->get_bacon_age_nrow();
       for (j=1;j<nrow;j++){
           if(ams->get_bacon_age_depth(j)>=depth && ams->get_bacon_age_depth(j-1)<depth){
               value=ams->get_bacon_age_median(j-1)+
                       ((ams->get_bacon_age_median(j)-ams->get_bacon_age_median(j-1))/(ams->get_bacon_age_depth(j)-ams->get_bacon_age_depth(j-1)))*
                       (depth-ams->get_bacon_age_depth(j-1));

               break;
           }
       }
       result_age[i]=value/1000;

       // calc value for age min
       value=NAN;
       j=1;
       for (j=1;j<nrow;j++){
           if(ams->get_bacon_age_depth(j)>=depth && ams->get_bacon_age_depth(j-1)<depth){
               value=ams->get_bacon_age_Q5(j-1)+
                       ((ams->get_bacon_age_Q5(j)-ams->get_bacon_age_Q5(j-1))/(ams->get_bacon_age_depth(j)-ams->get_bacon_age_depth(j-1)))*
                       (depth-ams->get_bacon_age_depth(j-1));

               break;
           }
       }
       result_age05[i]=value/1000;

       // calc value for age max
       value=NAN;
       j=1;
       for (j=1;j<nrow;j++){
           if(ams->get_bacon_age_depth(j)>=depth && ams->get_bacon_age_depth(j-1)<depth){
               value=ams->get_bacon_age_Q95(j-1)+
                       ((ams->get_bacon_age_Q95(j)-ams->get_bacon_age_Q95(j-1))/(ams->get_bacon_age_depth(j)-ams->get_bacon_age_depth(j-1)))*
                       (depth-ams->get_bacon_age_depth(j-1));

               break;
           }
       }
       result_age95[i]=value/1000;

   }

}
