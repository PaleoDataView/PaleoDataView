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

#include "bacon.h"
#include "ui_bacon.h"

Bacon::Bacon(QMainWindow *mainWindow,Inventory *i) :
    mainW(mainWindow),inv (i),
    ui(new Ui::Bacon)
{
    ui->setupUi(this);

    resources=new Resources();
    this->setWindowTitle("Bacon Interface");

    // Read AgeModel
    amsdata=new AMSData(inv);
    amsdata->ReadCalib();

    amsdata->AMSRead();
    baconflag=new int[amsdata->get_Length()];

    for (int i=0;i<amsdata->get_Length();i++) {
        QString com=amsdata->get_Age_Comment(i);
        if (amsdata->get_Data(7,i)==1 || com.contains("BACON_ON"))
            baconflag[i]=1;
        else baconflag[i]=0;

        if (com.contains("BACON_OFF")) baconflag[i]=0;
        if (com.contains("BACON_ON")) baconflag[i]=1;
    }

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    sp_4=ui->splitter_4->saveState();

    qApp->installEventFilter(this);

    hist=new float[0];
    hist_ncol=0;
    hist_nrow=0;

    agemodel=new float[0];
    agemodel_nrow=0;
    agemodel_ncol=0;

    acchist=new float[0];
    acchist_nrow=0;
    acchist_ncol=0;

    memhist=new float[0];
    memhist_nrow=0;
    memhist_ncol=0;


    str_par<<"K"<<"res"<<"d.min"<<"d.max"<<"d.by";
    str_par<<"acc.shape"<<"acc.mean"<<"mem.strength"<<"mem.mean";
    str_par<<"t.a"<<"t.b"<<"normal"<<"suggest";
    str_par<<"th0"<<"th0p"<<"burnin";
    str_par<<"ssize";
    str_par<<"MinYr"<<"MaxYr";
    str_par<<"cc"<<"cc1"<<"cc2"<<"cc3"<<"cc4"<<"postbomb";

    str_com<<"Number of bins for age-resolution"<<"Resolution in depth in cm"<<"Minimum depth of core in [cm]"<<"Maximum depth of core in [cm]"<<"Stepsize of depth in [cm]";
    str_com<<"Shape parameter for accumulation rate"<<"Mean value of accumulation rate"<<"Strength of memory"<<"Mean of memory";
    str_com<<"Student Error Distribution Parameter"<<"Student Error Distribution Parameter"<<"Normal Error Distribution"<<"Use Suggested Values";
    str_com<<"Automatically set"<<"Automatically set"<<"Skip n first Iterations";
    str_com<<"Number of Iterations";
    str_com<<"Minimum Age [yr]"<<"Maximum Age [yr]";
    str_com<<"Selected calibration function"<<"IntCal13"<<"Marine13"<<"SHCal13"<<"ConstCall"<<"Entry newer than 1945";


    float res=(amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K();

    str_val<<QString::number(amsdata->get_bacon_K())<<QString::number(res)<<QString::number(amsdata->get_bacon_d_min())<<QString::number(amsdata->get_bacon_d_max())<<QString::number(amsdata->get_bacon_d_by());
    str_val<<QString::number(amsdata->get_bacon_acc_shape())<<QString::number(amsdata->get_bacon_acc_mean())<<QString::number(amsdata->get_bacon_mem_strength())<<QString::number(amsdata->get_bacon_mem_mean());
    str_val<<QString::number(amsdata->get_bacon_t_a())<<QString::number(amsdata->get_bacon_t_b())<<QString::number(amsdata->get_bacon_normal())<<QString::number(amsdata->get_bacon_suggest());
    str_val<<QString::number(amsdata->get_bacon_th0())<<QString::number(amsdata->get_bacon_th0p())<<QString::number(amsdata->get_bacon_burnin());
    str_val<<QString::number(amsdata->get_bacon_ssize());
    str_val<<QString::number(amsdata->get_bacon_yr_min())<<QString::number(amsdata->get_bacon_yr_max());
    str_val<<QString::number(amsdata->get_bacon_cc())<<amsdata->get_bacon_cc1()<<amsdata->get_bacon_cc2()<<amsdata->get_bacon_cc3()<<amsdata->get_bacon_cc4()<<QString::number(amsdata->get_bacon_postbomb());


    out_nrow=amsdata->get_bacon_out_nrow();
    out_nrow_real=out_nrow;
    out_ncol=amsdata->get_bacon_out_ncol();
    out.clear();
    out.reserve(out_nrow*out_ncol);

    for (int i=0;i<out_nrow;i++){
        for (int j=0;j<out_ncol;j++){
            out[j*out_nrow+i]=amsdata->get_bacon_out(j*out_nrow+i);
        }
    }

    select=-1;
    select2=-1;
    //enData = new QStandardItemModel(0,0,this);
    setupTable();
    para = new QStandardItemModel(str_par.length()-6,3,this);
    // create the model for AMSData

    setupParameters();
    p_bacon=new QProcess(this);// This is Bacon



    // create Age model plot

    data1=new float[0];
    data_use=new bool[0];
    data_error=new float[0];
    data_error2=new float[0];
    col=new QColor[0];
    coll=new QColor[0];
    lstyle=new Qt::PenStyle[0];
    lwidth=new int[0];
    symbol=new int[0];
    mark=new int[0];
    poly=new int[0];
    area_x=new float[0];
    area_y=new float[0];
    amsplot=new Graph(this,data1,0,0);
    createAMSPlot();

    // create prior plot acc
    data2=new float[0];
    col2=new QColor[0];
    poly2=new int[0];
    accplot=new Graph(this,data2,0,0);
    createAccPlot();

    // create prior plot mem
    data3=new float[0];
    col3=new QColor[0];
    poly3=new int[0];
    memplot=new Graph(this,data3,0,0);
    createMemPlot();

    // create iteration plot it
    data4=new float[0];
    itplot=new Graph(this,data4,0,0);
    createItPlot();


    // automatically set dmin/max and min/maxyr
    if (getParameter("d.min")=="nan"||getParameter("d.min")=="0"||
            getParameter("d.max")=="nan"||getParameter("d.max")=="0"||
            getParameter("MinYr")=="nan"||getParameter("MinYr")=="0"||
            getParameter("MaxYr")=="nan"||getParameter("MaxYr")=="0") {
                    float dmin=getParameter("d.min").toFloat();
                    if (getParameter("d.min")=="nan"||getParameter("d.min")=="0") dmin=999999999;
                    float dmax=getParameter("d.max").toFloat();
                    if (getParameter("d.max")=="nan"||getParameter("d.max")=="0") dmax=-1000;
                    float minyr=getParameter("MinYr").toFloat();
                    if (getParameter("MinYr")=="nan"||getParameter("MinYr")=="0") minyr=999999999;
                    float maxyr=getParameter("MaxYr").toFloat();
                    if (getParameter("MaxYr")=="nan"||getParameter("MaxYr")=="0") maxyr=-1000;
                    int count=0;
                    for (int i=0;i<amsdata->get_Length();i++){
                        if (isnan(amsdata->get_Depth(i))==false && isnan(amsdata->get_Data(4,i))==false){
                        if (baconflag[i]){
                            if (amsdata->get_Type(i)=="AMS"){
                                // generate d.min and d.max
                                if (amsdata->get_Depth(i)<dmin) {
                                    dmin=amsdata->get_Depth(i);
                                }
                                if (amsdata->get_Depth(i)>dmax) {
                                    dmax=amsdata->get_Depth(i);
                                }


                                // generate minyr and maxyr
                                if ((amsdata->get_Data(0,i)-5*amsdata->get_Data(1,i))*1000<minyr) {
                                    minyr=(amsdata->get_Data(0,i)-5*amsdata->get_Data(1,i))*1000;
                                }
                                if ((amsdata->get_Data(0,i)+5*amsdata->get_Data(1,i))*1000>maxyr) {
                                    maxyr=(amsdata->get_Data(0,i)+5*amsdata->get_Data(1,i))*1000;
                                }




                                count++;
                            }
                            if (amsdata->get_Type(i)!="AMS"){

                                // generate d.min and d.max
                                if (amsdata->get_Depth(i)<dmin) {
                                    dmin=amsdata->get_Depth(i);
                                }
                                if (amsdata->get_Depth(i)>dmax) {
                                    dmax=amsdata->get_Depth(i);
                                }


                                // generate minyr and maxyr
                                if ((amsdata->get_Data(4,i)-5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000<minyr) {
                                    minyr=(amsdata->get_Data(4,i)-5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000;
                                }
                                if ((amsdata->get_Data(4,i)+5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i))*1000>maxyr)) {
                                    maxyr=(amsdata->get_Data(4,i)+5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000;
                                }




                                count++;

                            }
                            }
                        }else{
                            dmin=0;
                            dmax=1;
                            minyr=0;
                            maxyr=1;
                        }
                    }
                    //Put new values
                    if (getParameter("d.min").toFloat()!=dmin){
                        setParameter("d.min",QString::number(dmin*100));
                        ui->plainTextEdit->appendPlainText("Correcting d.min to "+QString::number(dmin)+"\n");
                    }
                    if (getParameter("d.max").toFloat()!=dmax){
                        setParameter("d.max",QString::number(dmax*100));
                        ui->plainTextEdit->appendPlainText("Correcting d.max to "+QString::number(dmax)+"\n");
                    }
                    if (getParameter("MinYr").toFloat()!=minyr){
                        setParameter("MinYr",QString::number(minyr));
                        ui->plainTextEdit->appendPlainText("Correcting MinYr to "+QString::number(minyr)+"\n");
                    }
                    if (getParameter("MaxYr").toFloat()!=maxyr){
                        setParameter("MaxYr",QString::number(maxyr));
                        ui->plainTextEdit->appendPlainText("Correcting MaxYr to "+QString::number(maxyr)+"\n");
                    }
                    // Put default res

                    int K=(getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/5.0;
                    setParameter("K",QString::number(K));
                    setParameter("res",QString::number((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/K));



    }

    showB=new ShowBacon(this,NULL,0,0,0,0,0);
    busy=new BusyBacon(this);

    connect(para,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(editData(QModelIndex,QModelIndex)));// Parameter changed
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(Bacon_Run()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(p_bacon,SIGNAL(readyRead()),this,SLOT(Bacon_Output()));
    connect(p_bacon,SIGNAL(finished(int)),this,SLOT(Bacon_Finish()));

    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(selected(QModelIndex)));// entry selected
    connect(ui->tableView_3,SIGNAL(clicked(QModelIndex)),this,SLOT(selected2(QModelIndex)));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(reload()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->checkBox,SIGNAL(clicked()),this,SLOT(replotAMS()));
    connect(ui->checkBox_2,SIGNAL(clicked()),this,SLOT(replotAMS()));
    connect(ui->checkBox_3,SIGNAL(clicked()),this,SLOT(replotAMS()));
    connect(ui->checkBox_4,SIGNAL(clicked()),this,SLOT(replotAMS()));
    connect(ui->checkBox_5,SIGNAL(clicked()),this,SLOT(replotAMS()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(reloadBacon()));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),this,SLOT(openManual()));
    connect(ui->pushButton_7,SIGNAL(clicked(bool)),this,SLOT(apply()));
    connect(ui->pushButton_9,SIGNAL(clicked(bool)),this,SLOT(target()));
    connect(itplot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(memplot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(accplot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(amsplot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));


    connect(ui->pushButton_8,SIGNAL(clicked(bool)),this,SLOT(OpenBacon()));
    ui->pushButton_5->setEnabled(false);

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    ui->tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_2(QPoint)));
    ui->tableView_3->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_3,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_3(QPoint)));

}

void Bacon::showEvent(QShowEvent *)
{
    QTimer::singleShot(50, this, SLOT(init()));
}

void Bacon::init(){
    {
    const QSignalBlocker blocker(para);
    if(out_nrow!=0 && out_ncol!=0) {
        QMessageBox msgBox;
        msgBox.setText("Already existing Bacon data found.");
        msgBox.setInformativeText("Do you want to use it?");
        QPushButton *changeButton = msgBox.addButton(tr("Do not use!"), QMessageBox::ActionRole);
        QPushButton *continueButton = msgBox.addButton(tr("Load"), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == changeButton) {
            ui->pushButton_4->setEnabled(false);
            ui->pushButton_3->setEnabled(true);
            ui->pushButton_7->setEnabled(false);
            ui->pushButton_9->setEnabled(false);
            // init
            amsdata->erase_bacon();
            float res=(amsdata->get_bacon_d_max()-amsdata->get_bacon_d_min())/amsdata->get_bacon_K();
            str_val.clear();
            str_val<<QString::number(amsdata->get_bacon_K())<<QString::number(res)<<QString::number(amsdata->get_bacon_d_min())<<QString::number(amsdata->get_bacon_d_max())<<QString::number(amsdata->get_bacon_d_by());
            str_val<<QString::number(amsdata->get_bacon_acc_shape())<<QString::number(amsdata->get_bacon_acc_mean())<<QString::number(amsdata->get_bacon_mem_strength())<<QString::number(amsdata->get_bacon_mem_mean());
            str_val<<QString::number(amsdata->get_bacon_t_a())<<QString::number(amsdata->get_bacon_t_b())<<QString::number(amsdata->get_bacon_normal())<<QString::number(amsdata->get_bacon_suggest());
            str_val<<QString::number(amsdata->get_bacon_th0())<<QString::number(amsdata->get_bacon_th0p())<<QString::number(amsdata->get_bacon_burnin());
            str_val<<QString::number(amsdata->get_bacon_ssize());
            str_val<<QString::number(amsdata->get_bacon_yr_min())<<QString::number(amsdata->get_bacon_yr_max());
            str_val<<QString::number(amsdata->get_bacon_cc())<<amsdata->get_bacon_cc1()<<amsdata->get_bacon_cc2()<<amsdata->get_bacon_cc3()<<amsdata->get_bacon_cc4()<<QString::number(amsdata->get_bacon_postbomb());


            out_nrow=amsdata->get_bacon_out_nrow();
            out_nrow_real=out_nrow;
            out_ncol=amsdata->get_bacon_out_ncol();
            out.clear();
            out.reserve(out_nrow*out_ncol);

            for (int i=0;i<out_nrow;i++){
                for (int j=0;j<out_ncol;j++){
                    out[j*out_nrow+i]=amsdata->get_bacon_out(j*out_nrow+i);
                }
            }
            float sugg=0;
            int c=0;
            // estimate mean for age*1.1 and depth
            float agemean=0;
            float depthmean=0;
            for (int i=0;i<amsdata->get_Length();i++){
                if (amsdata->get_Type(i)=="AMS" && baconflag[i]==1){
                    agemean+=amsdata->get_Data(0,i)*1.1;
                    depthmean+=amsdata->get_Depth(i);
                    c++;
                } else {
                    if (baconflag[i]==1){
                        agemean+=amsdata->get_Data(4,i)*1.1;
                        depthmean+=amsdata->get_Depth(i);
                        c++;
                    }
                }
            }
            agemean=agemean/c;
            depthmean=depthmean/c;
            float sum1=0;
            float sum2=0;
            for (int i=0;i<amsdata->get_Length();i++){
                if (amsdata->get_Type(i)=="AMS" && baconflag[i]==1){
                    sum1+=(amsdata->get_Data(0,i)-agemean)*(amsdata->get_Depth(i)-depthmean);
                    sum2+=(amsdata->get_Data(0,i)-agemean)*(amsdata->get_Data(0,i)-agemean);
                } else {
                    if (baconflag[i]==1){
                        sum1+=(amsdata->get_Data(4,i)-agemean)*(amsdata->get_Depth(i)-depthmean);
                        sum2+=(amsdata->get_Data(4,i)-agemean)*(amsdata->get_Data(4,i)-agemean);
                    }
                }
            }
            sugg=sum1/sum2;
            sugg=sugg*(100.0/1000.0);
            sugg=1.0/sugg;
            ui->plainTextEdit->appendPlainText("Correcting acc.mean to "+QString::number(sugg)+"cm/yr\n");
            setParameter("acc.mean",QString::number(sugg));
            // automatically set dmin/max and min/maxyr
            {
                            float dmin=getParameter("d.min").toFloat();
                            dmin=999999999;
                            float dmax=getParameter("d.max").toFloat();
                            dmax=-1000;
                            float minyr=getParameter("MinYr").toFloat();
                            minyr=999999999;
                            float maxyr=getParameter("MaxYr").toFloat();
                            maxyr=-1000;
                            int count=0;
                            for (int i=0;i<amsdata->get_Length();i++){
                                if (isnan(amsdata->get_Depth(i))==false && isnan(amsdata->get_Data(4,i))==false){
                                if (baconflag[i]){
                                    if (amsdata->get_Type(i)=="AMS"){
                                        // generate d.min and d.max
                                        if (amsdata->get_Depth(i)<dmin) {
                                            dmin=amsdata->get_Depth(i);
                                        }
                                        if (amsdata->get_Depth(i)>dmax) {
                                            dmax=amsdata->get_Depth(i);
                                        }


                                        // generate minyr and maxyr
                                        if ((amsdata->get_Data(0,i)-5*amsdata->get_Data(1,i))*1000<minyr) {
                                            minyr=(amsdata->get_Data(0,i)-5*amsdata->get_Data(1,i))*1000;
                                        }
                                        if ((amsdata->get_Data(0,i)+5*amsdata->get_Data(1,i))*1000>maxyr) {
                                            maxyr=(amsdata->get_Data(0,i)+5*amsdata->get_Data(1,i))*1000;
                                        }




                                        count++;
                                    }
                                    if (amsdata->get_Type(i)!="AMS"){

                                        // generate d.min and d.max
                                        if (amsdata->get_Depth(i)<dmin) {
                                            dmin=amsdata->get_Depth(i);
                                        }
                                        if (amsdata->get_Depth(i)>dmax) {
                                            dmax=amsdata->get_Depth(i);
                                        }


                                        // generate minyr and maxyr
                                        if ((amsdata->get_Data(4,i)-5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000<minyr) {
                                            minyr=(amsdata->get_Data(4,i)-5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000;
                                        }
                                        if ((amsdata->get_Data(4,i)+5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i))*1000>maxyr)) {
                                            maxyr=(amsdata->get_Data(4,i)+5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000;
                                        }




                                        count++;

                                    }
                                    }
                                }else{
                                    dmin=0;
                                    dmax=1;
                                    minyr=0;
                                    maxyr=1;
                                }
                            }
                            //Put new values
                            if (getParameter("d.min").toFloat()!=dmin){
                                setParameter("d.min",QString::number(dmin*100));
                                ui->plainTextEdit->appendPlainText("Correcting d.min to "+QString::number(dmin)+"\n");
                            }
                            if (getParameter("d.max").toFloat()!=dmax){
                                setParameter("d.max",QString::number(dmax*100));
                                ui->plainTextEdit->appendPlainText("Correcting d.max to "+QString::number(dmax)+"\n");
                            }
                            if (getParameter("MinYr").toFloat()!=minyr){
                                setParameter("MinYr",QString::number(minyr));
                                ui->plainTextEdit->appendPlainText("Correcting MinYr to "+QString::number(minyr)+"\n");
                            }
                            if (getParameter("MaxYr").toFloat()!=maxyr){
                                setParameter("MaxYr",QString::number(maxyr));
                                ui->plainTextEdit->appendPlainText("Correcting MaxYr to "+QString::number(maxyr)+"\n");
                            }
                            // Put default res

                            int K=(getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/5.0;
                            setParameter("K",QString::number(K));
                            setParameter("res",QString::number((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/K));

                            createAMSPlot();


                            createAccPlot();


                            createMemPlot();


                            createItPlot();



            }



        } else if (msgBox.clickedButton() == continueButton) {
            baconrestart=0;
            ui->pushButton_4->setEnabled(true);
            ui->pushButton_3->setEnabled(true);
            ui->pushButton_7->setEnabled(true);
            ui->pushButton_9->setEnabled(true);
            emit(refresh());
        }


    } else {
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_7->setEnabled(false);
        ui->pushButton_9->setEnabled(false);
    }
    changes=false;
    }
}

Bacon::~Bacon()
{
    if (ui!=NULL)delete ui;
    if (resources!=NULL)delete resources;
    //if (para!=NULL)delete para;
    if (p_bacon!=NULL)delete p_bacon;
    if (amsplot!=NULL)delete amsplot;
    if (accplot!=NULL)delete accplot;
    if (memplot!=NULL)delete memplot;
    if (itplot!=NULL)delete itplot;
    //if (modelData!=NULL)delete modelData;
    //if (enData!=NULL)delete enData;
    if (amsdata!=NULL)delete amsdata;
    if (hist!=NULL)delete[] hist;
    if (agemodel!=NULL)delete[] agemodel;
    //delete[] acchist; // Why does this not work????
    //delete[] memhist; // Why does this not work????

    if (data1!=NULL) delete[] data1;
    if (data_use!=NULL)delete[] data_use;
    if (data_error!=NULL)delete[] data_error;
    if (data_error2!=NULL)delete[] data_error2;
    if (col!=NULL)delete[] col;
    if (coll!=NULL)delete[] coll;
    if (lstyle!=NULL)delete[] lstyle;
    if (lwidth!=NULL)delete[] lwidth;
    if (symbol!=NULL)delete[] symbol;
    if (mark!=NULL)delete[] mark;
    if (poly!=NULL)delete[] poly;
    if (area_x!=NULL)delete[] area_x;
    if (area_y!=NULL)delete[] area_y;

    if (data2!=NULL)delete[] data2;
    if (col2!=NULL)delete[] col2;
    if (poly2!=NULL)delete[] poly2;

    if (data3!=NULL)delete[] data3;
    if (col3!=NULL)delete[] col3;
    if (poly3!=NULL)delete[] poly3;

    if (data4!=NULL)delete[] data4;

    if (showB!=NULL)delete showB;
    if (baconflag!=NULL)delete[] baconflag;
}

void Bacon::Bacon_Run(){
    checkParameters();
    {
    const QSignalBlocker blocker(para);
    if (bacon_invalid==0){
    ui->plainTextEdit->clear();
    // suggest acc.mean
    // make linear fit trough age*1.1 and depth and use b-coefficient
    if (getParameter("suggest")=="TRUE"||getParameter("suggest")=="1"){
        float sugg=0;
        int c=0;
        // estimate mean for age*1.1 and depth
        float agemean=0;
        float depthmean=0;
        for (int i=0;i<amsdata->get_Length();i++){
            if (amsdata->get_Type(i)=="AMS" && baconflag[i]==1){
                agemean+=amsdata->get_Data(0,i)*1.1;
                depthmean+=amsdata->get_Depth(i);
                c++;
            } else {
                if (baconflag[i]==1){
                    agemean+=amsdata->get_Data(4,i)*1.1;
                    depthmean+=amsdata->get_Depth(i);
                    c++;
                }
            }
        }
        agemean=agemean/c;
        depthmean=depthmean/c;
        float sum1=0;
        float sum2=0;
        for (int i=0;i<amsdata->get_Length();i++){
            if (amsdata->get_Type(i)=="AMS" && baconflag[i]==1){
                sum1+=(amsdata->get_Data(0,i)-agemean)*(amsdata->get_Depth(i)-depthmean);
                sum2+=(amsdata->get_Data(0,i)-agemean)*(amsdata->get_Data(0,i)-agemean);
            } else {
                if (baconflag[i]==1){
                    sum1+=(amsdata->get_Data(4,i)-agemean)*(amsdata->get_Depth(i)-depthmean);
                    sum2+=(amsdata->get_Data(4,i)-agemean)*(amsdata->get_Data(4,i)-agemean);
                }
            }
        }
        sugg=sum1/sum2;
        sugg=sugg*(100.0/1000.0);
        sugg=1.0/sugg;
        ui->plainTextEdit->appendPlainText("Correcting acc.mean to "+QString::number(sugg)+"cm/yr\n");
        setParameter("acc.mean",QString::number(sugg));

        // automatically set dmin/max and min/maxyr and set th0 and th0p
        float dmin=getParameter("d.min").toFloat();
        if (getParameter("d.min")=="nan"||getParameter("d.min")=="0") dmin=999999999;
        float dmax=getParameter("d.max").toFloat();
        if (getParameter("d.max")=="nan"||getParameter("d.max")=="0") dmax=-1000;
        float minyr=getParameter("MinYr").toFloat();
        if (getParameter("MinYr")=="nan"||getParameter("MinYr")=="0") minyr=999999999;
        float maxyr=getParameter("MaxYr").toFloat();
        if (getParameter("MaxYr")=="nan"||getParameter("MaxYr")=="0") maxyr=-1000;
        int count=0;
        for (int i=0;i<amsdata->get_Length();i++){
            if (baconflag[i]){
                if (amsdata->get_Type(i)=="AMS"){
                    // generate d.min and d.max
                    if (amsdata->get_Depth(i)<dmin) {
                        dmin=amsdata->get_Depth(i);
                    }
                    if (amsdata->get_Depth(i)>dmax) {
                        dmax=amsdata->get_Depth(i);
                    }


                    // generate minyr and maxyr
                    if ((amsdata->get_Data(0,i)-5*amsdata->get_Data(1,i))*1000<minyr) {
                        minyr=(amsdata->get_Data(0,i)-5*amsdata->get_Data(1,i))*1000;
                    }
                    if ((amsdata->get_Data(0,i)+5*amsdata->get_Data(1,i))*1000>maxyr) {
                        maxyr=(amsdata->get_Data(0,i)+5*amsdata->get_Data(1,i))*1000;
                    }



                    // Generating starting values for first entry
                    if (count==0){
                        float th0=gauss(amsdata->get_Data(0,i),amsdata->get_Data(1,i))-(getParameter("").toFloat()/1000);
                        if (th0<minyr) th0=minyr;
                        setParameter("th0",QString::number(th0*1000));
                        ui->plainTextEdit->appendPlainText("Setting th0 to "+QString::number(th0*1000)+"\n");
                        float th0p=gauss(amsdata->get_Data(0,i),amsdata->get_Data(1,i))-(getParameter("d.R").toFloat()/1000);
                        if (th0p<minyr) th0p=minyr;
                        setParameter("th0p",QString::number(th0p*1000));
                        ui->plainTextEdit->appendPlainText("Setting th0p to "+QString::number(th0p*1000)+"\n");
                    }
                    count++;
                }
                if (amsdata->get_Type(i)!="AMS"){
                    // generate d.min and d.max
                    if (amsdata->get_Depth(i)<dmin) {
                        dmin=amsdata->get_Depth(i);
                    }
                    if (amsdata->get_Depth(i)>dmax) {
                        dmax=amsdata->get_Depth(i);
                    }


                    // generate minyr and maxyr
                    if ((amsdata->get_Data(4,i)-5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000<minyr) {
                        minyr=(amsdata->get_Data(4,i)-5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000;
                    }
                    if ((amsdata->get_Data(4,i)+5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i))*1000>maxyr)) {
                        maxyr=(amsdata->get_Data(4,i)+5*abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)))*1000;
                    }



                    // Generating starting values for first entry
                    if (count==0){
                        float th0=gauss(amsdata->get_Data(4,i),abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i))/2);
                        if (th0<minyr) th0=minyr;
                        setParameter("th0",QString::number(th0*1000));
                        ui->plainTextEdit->appendPlainText("Setting th0 to "+QString::number(th0*1000)+"\n");
                        float th0p=gauss(amsdata->get_Data(4,i),abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i))/2);
                        if (th0p<minyr) th0p=minyr;
                        setParameter("th0p",QString::number(th0p*1000));
                        ui->plainTextEdit->appendPlainText("Setting th0p to "+QString::number(th0p*1000)+"\n");
                    }
                    count++;
                }
            }
        }
        //Put new values
        if (getParameter("d.min").toFloat()!=dmin){
            setParameter("d.min",QString::number(dmin*100));
            ui->plainTextEdit->appendPlainText("Correcting d.min to "+QString::number(dmin)+"\n");
        }
        if (getParameter("d.max").toFloat()!=dmax){
            setParameter("d.max",QString::number(dmax*100));
            ui->plainTextEdit->appendPlainText("Correcting d.max to "+QString::number(dmax)+"\n");
        }
        if (getParameter("MinYr").toFloat()!=minyr){
            setParameter("MinYr",QString::number(minyr));
            ui->plainTextEdit->appendPlainText("Correcting MinYr to "+QString::number(minyr)+"\n");
        }
        if (getParameter("MaxYr").toFloat()!=maxyr){
            setParameter("MaxYr",QString::number(maxyr));
            ui->plainTextEdit->appendPlainText("Correcting MaxYr to "+QString::number(maxyr)+"\n");
        }




    }

    // check for mistakes in settings
    int start=1;
    if (getParameter("acc.shape")==getParameter("acc.mean")){
        ui->plainTextEdit->appendPlainText("\nWarning! acc.shape cannot be equal to acc.mean");
        start=0;
    }
    if (getParameter("t.b").toFloat()-getParameter("t.a").toFloat()!=1){
        ui->plainTextEdit->appendPlainText("\nWarning! t.b - t.a should always be 1, check the manual");
    }

    if (start){
        // block user interference
        //delete busy;
        busy=new BusyBacon(this);
        busy->setModal(true);
        busy->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
        connect(busy,SIGNAL(abortBacon()),this,SLOT(abort()));
        quit=0;
        busy->show();

        //a->close();

        createBaconFile();
        ui->plainTextEdit->appendPlainText("Try to start Bacon...\n");

        QString file = resources->get_path_bacon()+"/bin/bacon";
        QStringList arg;

        arg<<"PaleoDataView.bacon"
          << "PaleoDataView.out"
          << getParameter("ssize")
          << "";
        p_bacon->setWorkingDirectory(resources->get_path_bacon());
        p_bacon->start(file,arg);

        if(p_bacon->waitForStarted(5000)){
            ui->plainTextEdit->appendPlainText("Bacon started ...\n");
        } else {
            ui->plainTextEdit->appendPlainText("Bacon not started ...\n");
            busy->close();
        }
    }
    }//bacon invalid
  }//signal blocker
}

void Bacon::Bacon_Output(){
    QByteArray out=p_bacon->readAllStandardOutput();
    QString out2(out);
    ui->plainTextEdit->appendPlainText(out2.toStdString().c_str());
    QByteArray errout=p_bacon->readAllStandardError();
    QString errout2(errout);
    ui->plainTextEdit->appendPlainText(errout2.toStdString().c_str());

}

void Bacon::Bacon_Finish(){
    if (quit){
        quit=0;
        ui->plainTextEdit->appendPlainText("Bacon was terminated: No results generated.");
        busy->close();
    } else {
        QByteArray out=p_bacon->readAllStandardOutput();
        QString out2(out);
        ui->plainTextEdit->appendPlainText(out2.toStdString().c_str());
        QByteArray errout=p_bacon->readAllStandardError();
        QString errout2(errout);
        ui->plainTextEdit->appendPlainText(errout2.toStdString().c_str());

        QString str="";
        str.append("Bacon exited ");

        if (p_bacon->exitStatus()==QProcess::NormalExit) {
            str.append("normaly ");
            str.append("with exit code "+QString::number(p_bacon->exitCode()));
            ui->plainTextEdit->appendPlainText(str);
            changes=true;
            showResults();
            busy->close();
            quit=0;
        }
        else {
            str.append("in an unusual way ");
            str.append("with exit code "+QString::number(p_bacon->exitCode()));
             str.append("NO RESULTS WHERE GENERATED!!!"+QString::number(p_bacon->exitCode()));
            ui->plainTextEdit->appendPlainText(str);
            changes=false;

            busy->close();
            quit=0;

        }

    }
}



void Bacon::paintEvent(QPaintEvent *)
{
    amsplot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    itplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    accplot->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    memplot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());
}

void Bacon::createAMSPlot(){

    // select right number of rows
    int length=1000;
    if (amsdata->get_Length()>length) length=amsdata->get_Length();
    if (hist_nrow>length) length=hist_nrow;
    if (agemodel_nrow>length) length=agemodel_nrow;
    int width=10;
    if (ui->checkBox_3->isChecked()) width=width+amsdata->get_Length()*2;
    if (ui->checkBox_4->isChecked()) width=width+hist_ncol;
    int histcol;// needs to be used instead of global hist_ncol
    if (ui->checkBox_4->isChecked()){
        histcol=hist_ncol;
    } else {
        histcol=0;
    }

    delete[] data1;
    data1=new float[length*(width)];
    for (int i=0;i<length*(width);i++) data1[i]=NAN;
    delete[] data_use;
    data_use=new bool[length*(width)];
    for (int i=0;i<length*(width);i++) data_use[i]=1;
    delete[] data_error;
    data_error=new float[length*(width)];
    for (int i=0;i<length*(width);i++) data_error[i]=NAN;
    delete[] data_error2;
    data_error2=new float[length*(width)];
    for (int i=0;i<length*(width);i++) data_error2[i]=NAN;
    delete[] col;
    col=new QColor[length*(width)];
    for (int i=0;i<length*(width);i++) col[i]=QColor(0,0,0);
    delete[] coll,
    coll=new QColor[(width)];
    for (int i=0;i<(width);i++) coll[i]=QColor(0,0,0);
    delete[] lstyle;
    lstyle=new Qt::PenStyle[(width)];
    for (int i=0;i<(width);i++) lstyle[i]=Qt::DashLine;
    delete[] lwidth;
    lwidth=new int[(width)];
    for (int i=0;i<(width);i++) lwidth[i]=1;
    delete[] symbol;
    symbol=new int[(width)];
    for (int i=0;i<(width);i++) symbol[i]=3;
    delete[] mark;
    mark=new int[length*(width)];
    for (int i=0;i<length*(width);i++) mark[i]=0;
    delete[] poly;
    poly=new int[(width)];
    for (int i=0;i<(width);i++) poly[i]=0;
    delete[] area_x;
    area_x=new float[(width)];
    for (int i=0;i<(width);i++) area_x[i]=NAN;
    delete[] area_y;
    area_y=new float[(width)];
    for (int i=0;i<(width);i++) area_y[i]=NAN;


    for (int i=0;i<amsdata->get_Length();i++){
        if (ui->checkBox->isChecked()){
            data1[i+(width-4)*length]=amsdata->get_Depth(i);
            data1[i+(width-3)*length]=amsdata->get_Data(0,i);
        }
        if (ui->checkBox_2->isChecked()){
            data1[i+(width-2)*length]=amsdata->get_Depth(i);
            data1[i+(width-1)*length]=amsdata->get_Data(4,i);
        }
        col[i+(width-4)*length]=QColor(0,0,0);
        col[i+(width-3)*length]=QColor(255,255,0);
        col[i+(width-2)*length]=QColor(0,0,0);
        col[i+(width-1)*length]=QColor(0,0,255);

        data_use[i+(width-4)*length]=baconflag[i];
        data_use[i+(width-3)*length]=baconflag[i];
        data_use[i+(width-2)*length]=baconflag[i];
        data_use[i+(width-1)*length]=baconflag[i];

        data_error[i+(width-4)*length]=NAN;
        data_error[i+(width-3)*length]=amsdata->get_Data(2,i);
        data_error[i+(width-2)*length]=NAN;
        if (amsdata->get_Data(5,i)!=0) data_error[i+(width-1)*length]=abs(amsdata->get_Data(4,i)-amsdata->get_Data(5,i)); else data_error[i+(width-1)*length]=NAN;

        data_error2[i+(width-4)*length]=NAN;
        data_error2[i+(width-3)*length]=amsdata->get_Data(1,i);
        data_error2[i+(width-2)*length]=NAN;
        if (amsdata->get_Data(6,i)!=0) data_error2[i+(width-1)*length]=abs(amsdata->get_Data(4,i)-amsdata->get_Data(6,i)); else data_error2[i+(width-1)*length]=NAN;

        mark[i+(width-10)*length]=0;
        mark[i+(width-9)*length]=0;
        mark[i+(width-8)*length]=0;
        mark[i+(width-7)*length]=0;
        mark[i+(width-6)*length]=0;
        mark[i+(width-5)*length]=0;
        mark[i+(width-4)*length]=0;
        mark[i+(width-3)*length]=0;
        mark[i+(width-2)*length]=0;
        mark[i+(width-1)*length]=0;


        if (ui->checkBox_3->isChecked()){


            // creating ghost for ams data
            // get pdf
            if (amsdata->get_Type(i)=="AMS"){
                amsdata->calc_CalPDF(amsdata->get_Data(0,i)*1000-amsdata->get_Data(3,i)*1000,amsdata->get_Data(1,i)*1000.0);
                int count=0;
                for (int j=0;j<amsdata->get_Calib_Length();j++){
                    if (amsdata->get_CalPDF(j)/amsdata->get_CalPDF_max()>0.01){
                        data1[count+(i*2+histcol)*length]=amsdata->get_Depth(i)+(amsdata->get_CalPDF(j)/amsdata->get_CalPDF_max())*0.1;
                        data1[count+(i*2+1+histcol)*length]=amsdata->get_cage(j)/1000;
                        data1[length-count+(i*2+histcol)*length-1]=amsdata->get_Depth(i)-(amsdata->get_CalPDF(j)/amsdata->get_CalPDF_max())*0.1;
                        data1[length-count+(i*2+1+histcol)*length-1]=amsdata->get_cage(j)/1000;
                        count++;
                    }
                    if (count==length/2) break;

                }
            }
        }
    }

    // copy data for hist
    if (ui->checkBox_4->isChecked()){

        for (int i=0;i<hist_ncol;i=i+2){
            float min=9999999999999;
            float max=-999999999999;

            for (int j=0;j<hist_nrow;j++){
                if (std::isnan(hist[(i+1)*hist_nrow+j])==false){
                    if (hist[(i+1)*hist_nrow+j]<min) min=hist[(i+1)*hist_nrow+j];
                    if (hist[(i+1)*hist_nrow+j]>max) max=hist[(i+1)*hist_nrow+j];
                }
            }

            for (int j=0;j<hist_nrow;j++){
                data1[i*length+j]=getParameter("d.min").toFloat()/100+(i/2)*getParameter("d.by").toFloat()/100+getParameter("d.by").toFloat()/200;
                data1[(i+1)*length+j]=hist[i*hist_nrow+j]/1000;
                if (std::isnan(hist[(i+1)*hist_nrow+j])==false){
                    int c=255-(int)(((hist[(i+1)*hist_nrow+j]-min)/(max-min))*255.0);
                    if (c<230){
                        col[i*length+j]=QColor(c,c,c);
                        col[(i+1)*length+j]=QColor(c,c,c);
                    } else {
                        col[i*length+j]=QColor(c,c,c,0);
                        col[(i+1)*length+j]=QColor(c,c,c,0);
                    }
                }
            }
        }
    }

    // copy data for median and 95%quantile
    if (ui->checkBox_5->isChecked()){
        for (int i=0;i<agemodel_nrow;i++){
            data1[i+(width-10)*length]=agemodel[i+0*agemodel_nrow]/100;
            data1[i+(width-9)*length]=agemodel[i+2*agemodel_nrow]/1000;
            data1[i+(width-8)*length]=agemodel[i+0*agemodel_nrow]/100;
            data1[i+(width-7)*length]=agemodel[i+3*agemodel_nrow]/1000;
            data1[i+(width-6)*length]=agemodel[i+0*agemodel_nrow]/100;
            data1[i+(width-5)*length]=agemodel[i+4*agemodel_nrow]/1000;
        }
    }
    coll[(width-10)]=QColor(0,0,0);
    coll[(width-9)]=QColor(0,0,0);
    coll[(width-8)]=QColor(255,0,0);
    coll[(width-7)]=QColor(255,0,0);
    coll[(width-6)]=QColor(0,0,0);
    coll[(width-5)]=QColor(0,0,0);
    coll[(width-4)]=QColor(0,0,0);
    coll[(width-3)]=QColor(0,0,0);
    coll[(width-2)]=QColor(0,0,0);
    coll[(width-1)]=QColor(0,0,0);
    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) coll[i]=QColor(128,128,128);
    if (ui->checkBox_3->isChecked()) for (int i=histcol;i<4+amsdata->get_Length()*2+histcol-10;i++) coll[i]=QColor(0,0,255);


    lstyle[(width-10)]=Qt::DotLine;
    lstyle[(width-9)]=Qt::DotLine;
    lstyle[(width-8)]=Qt::DotLine;
    lstyle[(width-7)]=Qt::DotLine;
    lstyle[(width-6)]=Qt::DotLine;
    lstyle[(width-5)]=Qt::DotLine;
    lstyle[(width-4)]=Qt::SolidLine;
    lstyle[(width-3)]=Qt::SolidLine;
    lstyle[(width-2)]=Qt::SolidLine;
    lstyle[(width-1)]=Qt::SolidLine;
    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) lstyle[i]=Qt::NoPen;
    if (ui->checkBox_3->isChecked()) for (int i=histcol;i<width-10;i++) lstyle[i]=Qt::SolidLine;

    symbol[(width-10)]=0;
    symbol[(width-9)]=0;
    symbol[(width-8)]=0;
    symbol[(width-7)]=0;
    symbol[(width-6)]=0;
    symbol[(width-5)]=0;
    symbol[(width-4)]=3;
    symbol[(width-3)]=3;
    symbol[(width-2)]=3;
    symbol[(width-1)]=3;
    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) symbol[i]=3;
    if (ui->checkBox_3->isChecked()) for (int i=histcol;i<width-10;i++) symbol[i]=0;


    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) area_x[i]=getParameter("d.by").toFloat()/200.0;
    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) area_y[i]=50.0/1000.0;


    lwidth[(width-10)]=3;
    lwidth[(width-9)]=3;
    lwidth[(width-8)]=3;
    lwidth[(width-7)]=3;
    lwidth[(width-6)]=3;
    lwidth[(width-5)]=3;
    lwidth[(width-4)]=1;
    lwidth[(width-3)]=1;
    lwidth[(width-2)]=1;
    lwidth[(width-1)]=1;
    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) lwidth[i]=0;
    if (ui->checkBox_3->isChecked()) for (int i=histcol;i<width-10;i++) lwidth[i]=1;

    poly[(width-10)]=0;
    poly[(width-9)]=0;
    poly[(width-8)]=0;
    poly[(width-7)]=0;
    poly[(width-6)]=0;
    poly[(width-5)]=0;
    poly[(width-4)]=0;
    poly[(width-3)]=0;
    poly[(width-2)]=0;
    poly[(width-1)]=0;
    if (ui->checkBox_4->isChecked()) for (int i=0;i<histcol;i++) poly[i]=0;
    if (ui->checkBox_3->isChecked()) for (int i=histcol;i<width-10;i++) poly[i]=1;

    if (select2>=0){
    mark[select2+(width-10)*length]=2;
    mark[select2+(width-9)*length]=2;
    mark[select2+(width-8)*length]=2;
    mark[select2+(width-7)*length]=2;
    mark[select2+(width-6)*length]=2;
    mark[select2+(width-5)*length]=2;
    }
    if (select>=0){
    mark[select+(width-4)*length]=1;
    mark[select+(width-3)*length]=1;
    mark[select+(width-2)*length]=1;
    mark[select+(width-1)*length]=1;
    }


    amsplot->setData(data1,width,length);
    amsplot->setUse(data_use,1);
    amsplot->setError(data_error,2);
    amsplot->setError2(data_error2,2);
    amsplot->setColor(col,1);

    amsplot->setSetLineColor(coll,1);
    amsplot->setSetLineStyle(lstyle,1);
    amsplot->setSetLineWidth(lwidth,1);
    amsplot->setSetSymbolsize(symbol,1);
    amsplot->setSetPoly(poly,1);
    amsplot->setMark(mark,1);
    amsplot->setSetArea(area_x,area_y,1);

    amsplot->setTitel("","Depth [m]","Age [ka BP]");
    amsplot->setMultiplicator(1,1);
    amsplot->setTextSize(resources->get_Pointsize(),1,12);
    amsplot->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    amsplot->setSymbol(3);
    amsplot->autoSize();
    amsplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Bacon_AgeModel.txt",0);
    ui->graphicsView->setScene(amsplot);
    ui->graphicsView->repaint();
}

void Bacon::createAccPlot(){

    int length=(int) (getParameter("acc.mean").toFloat()*0.1)*1000;
    if (acchist_nrow+2>length && acchist_nrow>0) length=acchist_nrow+2;
    delete[] data2;
    data2=new float[length*4];
    for (int i=0;i<length*4;i++){
        data2[i]=NAN;
    }
    delete[] col2;
    col2=new QColor[length*4];
    for (int i=0;i<length*4;i++) col2[i].setRgb(0,0,0,255);
    delete[] poly2;
    poly2=new int[length*4];
    for (int i=0;i<length*4;i++) poly2[i]=0;

    // posterior
    if (acchist_nrow!=0){
        for (int i=0;i<acchist_nrow;i++){
            data2[i+1+0*length]=acchist[i+0*acchist_nrow]*0.1;
            data2[i+1+1*length]=acchist[i+1*acchist_nrow];
            col2[i+1+0*length].setRgb(64,64,64,128);
            col2[i+1+1*length].setRgb(64,64,64,128);

        }

        poly2[0]=1;
        poly2[1]=1;
        data2[0+0*length]=0;
        data2[0+1*length]=0;
        data2[acchist_nrow+1+0*length]=acchist[acchist_nrow-1+0*acchist_nrow]*0.1;
        data2[acchist_nrow+1+1*length]=0;
    }
    // prior
    float a=getParameter("acc.shape").toFloat();
    float s=(getParameter("acc.mean").toFloat()*0.1)/a;
    data2[0+2*length]=0;
    data2[0+3*length]=0;

    col2[0+2*length].setRgb(0,255,0,255);
    col2[0+3*length].setRgb(0,255,0,255);
    for (int i=1;i<length;i++){
        data2[i+2*length]=i*0.01;
        data2[i+3*length]=1.0/(pow(s,a)*tgamma(a))*pow((i*0.01),(a-1.0))*exp(-((i*0.01)/s))*0.1;

        col2[i+2*length].setRgb(0,255,0,255);
        col2[i+3*length].setRgb(0,255,0,255);
    }



    accplot->setData(data2,4,length);
    accplot->setColor(col2,1);
    accplot->setLineColor(Qt::darkGreen);
    accplot->setLineStyle(Qt::SolidLine);
    accplot->setLineWidth(2);
    accplot->setSetPoly(poly2,1);
    accplot->setTitel("","Acc rate (ka/m)","Density");
    accplot->setTextSize(resources->get_Pointsize(),1,12);
    accplot->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    accplot->setSymbol(0);

    accplot->autoSize();
    accplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Bacon_Acc.txt",0);
    ui->graphicsView_3->setScene(accplot);
    ui->graphicsView_3->repaint();

}

void Bacon::createMemPlot(){

    int length=1001;
    if (memhist_nrow+2>length && memhist_nrow>0) length=memhist_nrow+2;
    delete[] data3;
    data3=new float[length*4];
    for (int i=0;i<length*4;i++){
        data3[i]=NAN;
    }
    delete[] col3;
    col3=new QColor[length*4];
    for (int i=0;i<length*4;i++) col3[i].setRgb(0,0,0,255);
    delete[] poly3;
    poly3=new int[length*4];
    for (int i=0;i<length*4;i++) poly3[i]=0;
    // posterior
    if (memhist_nrow>0){
        for (int i=0;i<memhist_nrow;i++){
            data3[i+1+0*length]=memhist[i+0*memhist_nrow];
            data3[i+1+1*length]=memhist[i+1*memhist_nrow];
            col3[i+1+0*length].setRgb(64,64,64,128);
            col3[i+1+1*length].setRgb(64,64,64,128);

        }
        poly3[0]=1;
        poly3[1]=1;
        data3[0+0*length]=0;
        data3[0+1*length]=0;
        data3[memhist_nrow+1+0*length]=memhist[memhist_nrow-1+0*memhist_nrow];
        data3[memhist_nrow+1+1*length]=0;
    }
    // prior
    float a=getParameter("mem.strength").toFloat()*getParameter("mem.mean").toFloat();
    float b=getParameter("mem.strength").toFloat()*(1-getParameter("mem.mean").toFloat());
    for (int i=0;i<length;i++){
        float res=1.0/(length-1);
        data3[i+2*length]=i*res;
        data3[i+3*length]=tgamma(a+b)/(tgamma(a)*tgamma(b))*pow((i*res),(a-1.0))*pow((1.0-(i*res)),(b-1.0));
        if (i==length-1) data3[i+3*length]=0;
        col3[i+2*length].setRgb(0,255,0,255);
        col3[i+3*length].setRgb(0,255,0,255);
    }

    memplot->setData(data3,4,length);
    memplot->setColor(col3,1);
    memplot->setLineColor(Qt::darkGreen);
    memplot->setLineStyle(Qt::SolidLine);
    memplot->setLineWidth(2);
    memplot->setSetPoly(poly3,1);
    memplot->setTitel("","Memory (ratio)","Density");
    memplot->setTextSize(resources->get_Pointsize(),1,12);
    memplot->setSize(ui->graphicsView_4->width(),ui->graphicsView_4->height());
    memplot->setSymbol(0);

    memplot->autoSize();
    memplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Bacon_Memory.txt",0);
    ui->graphicsView_4->setScene(memplot);
    ui->graphicsView_4->repaint();
}

void Bacon::createItPlot(){

    if (out_ncol!=0){

        int length=out_nrow;
        delete[] data4;
        data4=new float[length*2];
        for (int i=0;i<length*2;i++){
            data4[i]=NAN;
        }


        for (int i=0;i<length;i++){
            data4[i+0*length]=i;
            data4[i+1*length]=out[i+1*(out_nrow)];
        }
        itplot->setLineStyle(Qt::SolidLine);
        itplot->setData(data4,2,length);
        itplot->setTitel("","Iteration","Log of Objective");
        itplot->setTextSize(resources->get_Pointsize(),1,12);
        itplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        itplot->setSymbol(0);

        itplot->autoSize();
        itplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Bacon_It.txt",0);
        ui->graphicsView_2->setScene(itplot);
    } else {
        delete[] data4;
        data4=new float[0];
        itplot->setData(data4,0,0);
        itplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Bacon_It.txt",0);
        ui->graphicsView_2->setScene(itplot);
    }
    ui->graphicsView_2->repaint();
}

void Bacon::setupTable(){

    // create the model for AMSData
    modelData = new QStandardItemModel(amsdata->get_Length(),15,this);
    modelData->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelData->setHorizontalHeaderItem(1, new QStandardItem(QString("Bacon\nFlag")));
    modelData->setHorizontalHeaderItem(2, new QStandardItem(QString("Depth")));
    modelData->setHorizontalHeaderItem(3, new QStandardItem(QString("Sample\nThickness")));
    modelData->setHorizontalHeaderItem(4, new QStandardItem(QString("Label")));
    modelData->setHorizontalHeaderItem(5, new QStandardItem(QString("Type")));
    modelData->setHorizontalHeaderItem(6, new QStandardItem(QString("Age dated\n[ka]")));
    modelData->setHorizontalHeaderItem(7, new QStandardItem(QString("Age std\n[ka+]")));
    modelData->setHorizontalHeaderItem(8, new QStandardItem(QString("Age std\n[ka-]")));
    modelData->setHorizontalHeaderItem(9, new QStandardItem(QString("Res. Age\n[ka]")));
    modelData->setHorizontalHeaderItem(10, new QStandardItem(QString("Res. Age\n Error\n[ka]")));
    modelData->setHorizontalHeaderItem(11, new QStandardItem(QString("Cal yrs\n[wm ka BP]")));
    modelData->setHorizontalHeaderItem(12, new QStandardItem(QString("Cal yrs min\n[95%]")));
    modelData->setHorizontalHeaderItem(13, new QStandardItem(QString("Cal yrs max\n[95%]")));
    modelData->setHorizontalHeaderItem(14, new QStandardItem(QString("Use Flag")));
    modelData->setHorizontalHeaderItem(15, new QStandardItem(QString("Comments")));
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

        QStandardItem *var_Bacon_Flag=new QStandardItem(QString::number(baconflag[i]));
        var_Bacon_Flag->setCheckable(true);
        if (baconflag[i]==1) var_Bacon_Flag->setCheckState(Qt::Checked); else var_Bacon_Flag->setCheckState(Qt::Unchecked);
        modelData->setItem(i,1,var_Bacon_Flag);
        modelData->setData(modelData->index(i, 1), Qt::AlignCenter,Qt::TextAlignmentRole);

        QStandardItem *var_Depth=new QStandardItem(QString::number(amsdata->get_Depth(i)));
        modelData->setItem(i,2,var_Depth);

        QStandardItem *var_Sample=new QStandardItem(QString::number(amsdata->get_Sample_Thickness(i)));
        modelData->setItem(i,3,var_Sample);

        QStandardItem *var_Label=new QStandardItem(amsdata->get_LabID(i));
        modelData->setItem(i,4,var_Label);

        QStandardItem *var_Type=new QStandardItem(amsdata->get_Type(i));
        modelData->setItem(i,5,var_Type);

        QStandardItem *var_Age_dated=new QStandardItem(QString::number(amsdata->get_Data(0,i)));
        modelData->setItem(i,6,var_Age_dated);

        QStandardItem *var_Age_UCL=new QStandardItem(QString::number(amsdata->get_Data(1,i)));
        modelData->setItem(i,7,var_Age_UCL);

        QStandardItem *var_Age_LCL=new QStandardItem(QString::number(amsdata->get_Data(2,i)));
        modelData->setItem(i,8,var_Age_LCL);

        QStandardItem *var_Age_Res=new QStandardItem(QString::number(amsdata->get_Data(3,i)));
        modelData->setItem(i,9,var_Age_Res);

        QStandardItem *var_Age_Res_Err=new QStandardItem(QString::number(amsdata->get_Reservoir_Error(i)));
        modelData->setItem(i,10,var_Age_Res_Err);

        QStandardItem *var_Cal=new QStandardItem(QString::number(amsdata->get_Data(4,i)));
        modelData->setItem(i,11,var_Cal);

        QStandardItem *var_Cal_Min=new QStandardItem(QString::number(amsdata->get_Data(5,i)));
        modelData->setItem(i,12,var_Cal_Min);

        QStandardItem *var_Cal_Max=new QStandardItem(QString::number(amsdata->get_Data(6,i)));
        modelData->setItem(i,13,var_Cal_Max);

        QStandardItem *var_Use_Flag=new QStandardItem(QString::number(amsdata->get_Data(7,i)));
        modelData->setItem(i,14,var_Use_Flag);
        modelData->setData(modelData->index(i, 14), Qt::AlignCenter,Qt::TextAlignmentRole);

        QStandardItem *var_Comment=new QStandardItem(amsdata->get_Age_Comment(i));
        modelData->setItem(i,15,var_Comment);




        // coloring
        if (amsdata->get_Data(7,i)){
            modelData->setData(modelData->index(i, 14), QColor(Qt::green), Qt::BackgroundRole);
        } else {
            modelData->setData(modelData->index(i, 14), QColor(Qt::red), Qt::BackgroundRole);
        }


    }
    ui->tableView->setSortingEnabled(0);
    //ui->tableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->scrollTo(modelData->index(amsdata->getSelect(),0));
    ui->tableView->repaint();
}




void Bacon::selected(QModelIndex mi){
        int sel=mi.row();
        //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        QString text = model->item(sel,mi.column())->text();
        //qDebug() << text;
        int index=model->item(sel,0)->text().toInt();

        if (mi.column()==1){
            if (baconflag[sel]==1){
                baconflag[sel]=0;
                QString com=amsdata->get_Age_Comment(sel);
                com.remove("BACON_OFF;",Qt::CaseSensitive);
                com.remove("BACON_ON;",Qt::CaseSensitive);
                if (com.right(1)==";") com.append("BACON_OFF;"); else com.append(";BACON_OFF;");
                amsdata->set_Age_Comment(com,sel);
            } else {
                baconflag[sel]=1;
                QString com=amsdata->get_Age_Comment(sel);
                com.remove("BACON_OFF;",Qt::CaseSensitive);
                com.remove("BACON_ON;",Qt::CaseSensitive);
                if (com.right(1)==";") com.append("BACON_ON;"); else com.append(";BACON_ON;");
                amsdata->set_Age_Comment(com,sel);
            }

        }

        amsdata->setSelect(index);
        setupTable();

        if (index!=select){ select=index;} else{select=-1;}
        // recreate Age model plot
        createAMSPlot();

        update();

}

void Bacon::setupParameters(){
    para->setHorizontalHeaderItem(0, new QStandardItem(QString("Parameter")));
    para->setHorizontalHeaderItem(1, new QStandardItem(QString("Value")));
    para->setHorizontalHeaderItem(2, new QStandardItem(QString("Comment")));
    ui->tableView_2->setModel(para);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView_2->setFont(font);
    ui->tableView_2->horizontalHeader()->setFont(font);
    ui->tableView_2->verticalHeader()->setFont(font);
    //QStandardItem *var_para = new QStandardItem[str_par.length()];
    //var_value = new QStandardItem[str_par.length()];
    //QStandardItem *var_com = new QStandardItem[str_par.length()];

    for (int i=0;i<str_par.length()-6;i++){
        QStandardItem *var_para=new QStandardItem(str_par[i]);
        para->setItem(i,0,var_para);
        QStandardItem *var_value=new QStandardItem(str_val[i]);
        para->setItem(i,1,var_value);
        QStandardItem *var_com=new QStandardItem(str_com[i]);
        para->setItem(i,2,var_com);
    }

    ui->tableView_2->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableView_2->setSortingEnabled(0);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    //ui->tableView_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_2->repaint();
}

void Bacon::editData(QModelIndex r,QModelIndex c){
    if((str_par[r.row()]=="K"||str_par[r.row()]=="res") && str_val[r.row()]!=para->item(r.row(),1)->text()){
        QMessageBox msgBox;
        msgBox.setText("Changing this value will require a new run of Bacon");
        msgBox.setInformativeText("Are you sure?");
        QPushButton *changeButton = msgBox.addButton(tr("Change"), QMessageBox::ActionRole);
        QPushButton *keepButton = msgBox.addButton(tr("Keep it"), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == changeButton) {
            str_val[r.row()]=para->item(r.row(),1)->text();
            ui->plainTextEdit->appendPlainText("Changed : "+str_par[r.row()]+" to "+str_val[r.row()]);
            if (str_par[r.row()]=="K"){
                para->item(r.row()+1,1)->setText(QString::number((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/para->item(r.row(),1)->text().toInt()));
                str_val[r.row()+1]=QString::number((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/para->item(r.row(),1)->text().toInt());
                ui->plainTextEdit->appendPlainText("Changed : "+str_par[r.row()+1]+" to "+str_val[r.row()+1]);
            }
            if (str_par[r.row()]=="res"){
                int v=(int)((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/para->item(r.row(),1)->text().toFloat()+0.5);
                para->item(r.row()-1,1)->setText(QString::number(v));
                str_val[r.row()-1]=QString::number(v);

                ui->plainTextEdit->appendPlainText("Changed : "+str_par[r.row()-1]+" to "+str_val[r.row()-1]);
            }
            baconrestart=1;
            ui->pushButton_4->setEnabled(false);
            ui->pushButton_3->setEnabled(false);

        } else if (msgBox.clickedButton() == keepButton) {
            QStandardItem *var_value=new QStandardItem(str_val[r.row()]);
            para->setItem(r.row(),1,var_value);
        }
    } else{

        str_val[r.row()]=para->item(r.row(),1)->text();
        ui->plainTextEdit->appendPlainText("Changed : "+str_par[r.row()]+" to "+str_val[r.row()]);
        if (str_par[r.row()]=="t.a") {
            float v=para->item(r.row(),1)->text().toFloat()+1;
            para->item(r.row()+1,1)->setText(QString::number(v));
            str_val[r.row()+1]=QString::number(v);


        }
        if (str_par[r.row()]=="t.b") {
            float v=para->item(r.row(),1)->text().toFloat()-1;
            para->item(r.row()-1,1)->setText(QString::number(v));
            str_val[r.row()-1]=QString::number(v);


        }
        checkParameters();
    }

}

QString Bacon::getParameter(QString name){
    QString str="";
    for (int i=0;i<str_par.length();i++){
        if (str_par[i]==name) {
            str=str_val[i];
            break;
        }
    }

    return str;
}

void Bacon::setParameter(QString name,QString value){

    int i;
    for (i=0;i<str_par.length();i++) if (str_par[i]==name) break;
    if (i==str_par.length()) {
        ui->plainTextEdit->appendPlainText("Parameter not found : "+name+" : "+value);
    }else{

                str_val[i]=value;
                if (i<str_par.length()-6){
                    QStandardItem *var_value=new QStandardItem(str_val[i]);
                    para->setItem(i,1,var_value);
                }



    }
}

void Bacon::setupEnsamble(){


    enData->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    enData->setHorizontalHeaderItem(1, new QStandardItem(QString("Depth")));
    enData->setHorizontalHeaderItem(2, new QStandardItem(QString("Mean Age")));
    enData->setHorizontalHeaderItem(3, new QStandardItem(QString("5% Quantile")));
    enData->setHorizontalHeaderItem(4, new QStandardItem(QString("Median")));
    enData->setHorizontalHeaderItem(5, new QStandardItem(QString("95% Quantile")));


    ui->tableView_3->setModel(enData);
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView_3->setFont(font);
    ui->tableView_3->horizontalHeader()->setFont(font);
    ui->tableView_3->verticalHeader()->setFont(font);



    for (int i=0;i<agemodel_nrow;i++){
            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            enData->setItem(i,0,var_Index);

            QStandardItem *var_Depth=new QStandardItem(QString::number(agemodel[i+0*agemodel_nrow]/100.0));
            enData->setItem(i,1,var_Depth);

            QStandardItem *var_Mean=new QStandardItem(QString::number(agemodel[i+1*agemodel_nrow]/1000.0));
            enData->setItem(i,2,var_Mean);

            QStandardItem *var_Median=new QStandardItem(QString::number(agemodel[i+2*agemodel_nrow]/1000.0));
            enData->setItem(i,3,var_Median);

            QStandardItem *var_Q5=new QStandardItem(QString::number(agemodel[i+3*agemodel_nrow]/1000.0));
            enData->setItem(i,4,var_Q5);

            QStandardItem *var_Q95=new QStandardItem(QString::number(agemodel[i+4*agemodel_nrow]/1000.0));
            enData->setItem(i,5,var_Q95);

    }
    ui->tableView_3->setSortingEnabled(0);
    //ui->tableView_3->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView_3->verticalHeader()->setDefaultSectionSize(ui->tableView_3->verticalHeader()->minimumSectionSize());
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->setHorizontalScrollMode(ui->tableView_3->ScrollPerPixel);
    //ui->tableView_3->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_3->repaint();
}



void Bacon::selected2(QModelIndex mi){
        int sel=mi.row();
        //qDebug() << "Clicked :"+QString::number(sel)+":"+QString::number(mi.column());
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_3->model());
        QString text = model->item(sel,mi.column())->text();
        //qDebug() << text;
        int index=model->item(sel,0)->text().toInt();



        setupEnsamble();
        for (int i=0;i<6;i++)enData->setData(enData->index(index,i), QColor(Qt::gray), Qt::BackgroundRole);
        if (select2!=index){select2=index;}else{select2=-1;}
        // recreate Age model plot
        createAMSPlot();

        update();

}


void Bacon::createBaconFile(){
    QString txt="";
    QTime time;
    time=QTime::currentTime();
    QDate date;
    date=QDate::currentDate();

    txt.append("## Ran on "+QDate::shortDayName(date.dayOfWeek())+" "+QDate::shortMonthName(date.month())+" "+QString::number(date.day())+" "+
               QString::number(time.hour())+":"+QString::number(time.minute())+":"+QString::number(time.second())+" "+QString::number(date.year())+"\n\n");

    txt.append("Cal 0 : ConstCal"
               ";\nCal 1 : "+getParameter("cc1")+", "+getParameter("postbomb")+
               ";\nCal 2 : "+getParameter("cc2")+
               ";\nCal 3 : "+getParameter("cc3")+", "+getParameter("postbomb")+";");
    if (getParameter("cc4")!="ConstCal") txt.append("\nCal 4 : GenericCal, Curves/"+getParameter("cc4")+";");

    txt.append("\n\n##   id.   yr   std   depth   d.R  d.STD     t.a   t.b   cc");
    int count=0;
    for (int i=0;i<amsdata->get_Length();i++){
        if (baconflag[i]){
            if (amsdata->get_Type(i)=="AMS"){

                // add entry
                txt.append("\nDet "+QString::number(count)+" : "+
                           amsdata->get_LabID(i).replace(","," ").replace(";"," ")+" , "+
                           QString::number(amsdata->get_Data(0,i)*1000)+", "+
                           QString::number(amsdata->get_Data(1,i)*1000)+", "+
                           QString::number(amsdata->get_Depth(i)*100));

                    txt.append(", "+(QString::number(amsdata->get_Data(3,i)*1000))+", "+
                               (QString::number(amsdata->get_Reservoir_Error(i)*1000))+", "+
                               getParameter("t.a")+", "+
                               getParameter("t.b")+", "+
                               getParameter("cc")+";");


                count++;
            }

            if (amsdata->get_Type(i)!="AMS" ){

                // add entry
                txt.append("\nDet "+QString::number(count)+" : "+
                           amsdata->get_LabID(i).replace(","," ").replace(";"," ")+" , "+
                           QString::number(amsdata->get_Data(4,i)*1000)+", "+
                           QString::number(abs((amsdata->get_Data(4,i)-amsdata->get_Data(5,i))*1000.0)/2.0)+", "+
                           QString::number(amsdata->get_Depth(i)*100));
                float resage=amsdata->get_Data(3,i);
                if (isnan(resage)) resage=0.0;
                txt.append(", "+(QString::number(resage*1000))+", "+
                               "0"+", "+
                               getParameter("t.a")+", "+
                               getParameter("t.b")+", 0;");


                count++;
            }
        }
    }


    txt.append("\n\n##\t\t K   MinYr   MaxYr   th0   th0p   w.a   w.b   alpha  beta  dmin  dmax");
    txt.append("\nBacon 0: ");


    if (getParameter("normal")=="1") {txt.append("FixNor");} else {txt.append("FixT");}
    txt.append(", "+QString::number(getParameter("K").toInt())+", "+
               QString::number(getParameter("MinYr").toFloat())+", "+
               QString::number(getParameter("MaxYr").toFloat())+", "+
               QString::number(getParameter("th0").toFloat())+", "+
               QString::number(getParameter("th0p").toFloat())+", "+
               QString::number(getParameter("mem.strength").toFloat()*getParameter("mem.mean").toFloat())+", "+
               QString::number(getParameter("mem.strength").toFloat()*(1-getParameter("mem.mean").toFloat()))+", "+
               getParameter("acc.shape")+", "+
               QString::number(getParameter("acc.shape").toFloat()/getParameter("acc.mean").toFloat())+", "+
               QString::number(getParameter("d.min").toFloat())+", "+
               QString::number(getParameter("d.max").toFloat())+";\n");

    // get file name
    QString QFilename=resources->get_path_bacon()+"/PaleoDataView.bacon";
    //qDebug()<<QFilename;
    QFile f(QFilename);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //qDebug() <<  f.errorString();
    } else {
    QTextStream out(&f);
    out<<txt;
    f.close();
    }
}

// Generate Gauss according Box-Mueller method
double Bacon::gauss(double mu, double sigma)
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

void Bacon::showResults(){
    loadResults();
    calcResults();
}

void Bacon::loadResults(){
    {
    const QSignalBlocker blocker(para);
    ui->plainTextEdit->appendPlainText("Reading output file...");
    QString txt;
    txt.clear();
    QString QFilename=resources->get_path_bacon()+"/PaleoDataView.out";
    QFile file(QFilename);
    int count=0;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->plainTextEdit->appendPlainText(file.errorString());
    } else {

        QTextStream in(&file);
        while(!in.atEnd()) {
            txt.append(in.readLine().toLocal8Bit()+"\n");
            count++;
        }

    file.close();
    }

    // Remove first iterations from file if possible
    if (ui->checkBox_6->isChecked()){
        int ssize=getParameter("ssize").toInt();
        if (ssize<count){
            int ncount=0;
            for (int i=0;i<txt.length();i++){
                if (txt.at(i)=="\n"){
                    ncount++;
                }
                if (ncount+ssize==count) {
                    txt.remove(0,i);
                    ui->plainTextEdit->appendPlainText("Limited to "+QString::number(ssize)+" Iterations.");
                    break;
                }
            }
        }

    }

    // Reread Meta from bacon file
    QString meta;
    meta.clear();
    QString QFilename2=resources->get_path_bacon()+"/PaleoDataView.bacon";
    QFile file2(QFilename2);
    if(!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->plainTextEdit->appendPlainText(file2.errorString());
    } else {
        QTextStream in(&file2);
        while(!in.atEnd()) {
            meta.append(in.readLine().toLocal8Bit()+"\n");
        }

    file.close();
    }
    meta.replace(";","");
    QStringList metalist=meta.split("\n");
    //qDebug()<<meta;
    for (int i=0;i<metalist.size();i++){
        QStringList metaline=metalist.at(i).split(",");

        if (metaline.at(0)=="##   id.   yr   std   depth   d.R  d.STD     t.a   t.b   cc"){
            QStringList metaline2=metalist.at(i+1).split(",");
            //setParameter("d.R",QString::number(metaline2.at(4).toFloat()));
            //setParameter("d.STD",QString::number(metaline2.at(5).toFloat()));
            setParameter("t.a",QString::number(metaline2.at(6).toFloat()));
            setParameter("t.b",QString::number(metaline2.at(7).toFloat()));
            int cc=metaline2.at(8).toInt();
            if (cc==0) cc=4;
            setParameter("cc",QString::number(cc));
        }
        if (metaline.at(0).startsWith("Bacon 0:")){
            setParameter("K",QString::number(metaline.at(1).toInt()));
            setParameter("MinYr",QString::number(metaline.at(2).toFloat()));
            setParameter("MaxYr",QString::number(metaline.at(3).toFloat()));
            setParameter("th0",QString::number(metaline.at(4).toFloat()));
            setParameter("th0p",QString::number(metaline.at(5).toFloat()));
            float memm=1.0/(metaline.at(7).toFloat()/metaline.at(6).toFloat()+1.0);
            float mems=metaline.at(6).toFloat()/memm;
            setParameter("mem.strength",QString::number(mems));
            setParameter("mem.mean",QString::number(memm));
            setParameter("acc.shape",QString::number(metaline.at(8).toFloat()));
            setParameter("acc.mean",QString::number(metaline.at(8).toFloat()/metaline.at(9).toFloat()));
            setParameter("d.min",QString::number(metaline.at(10).toFloat()));
            setParameter("d.max",QString::number(metaline.at(11).toFloat()));
        }
        setParameter("postbomb","0");
        setParameter("cc1","IntCal13");
        setParameter("cc2","Marine13");
        setParameter("cc3","SHCal13");
        ui->pushButton_5->setEnabled(true);
        reloadB=1;
    }

    ui->plainTextEdit->appendPlainText("Done!");

    QStringList txt_line=txt.split("\n");
    out_nrow=txt_line.size()-2;
    out_nrow_real=out_nrow;
    float val=0;
    QStringList txt_value=txt_line.at(1).split("\t");
    out_ncol=txt_value.size()-1;

    out.clear();
    out.reserve(out_nrow*out_ncol);

    for (int i=0;i<out_nrow;i++){
        QStringList txt_value=txt_line.at(i+1).split("\t");
        for (int j=0;j<out_ncol;j++){
            val=txt_value.at(j+1).toFloat();
            out[j*out_nrow+i]=val;
        }
    }
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_9->setEnabled(false);
    baconrestart=0;
    }
    checkParameters();
}

void Bacon::calcResults(){
    //try{
    ui->plainTextEdit->appendPlainText("\nCalculating results...");
    repaint();
    // get original out_nrow

    out_nrow=out_nrow_real;

    createItPlot();
    // analyse out and transform it to data
    // Calc hist with own routine
    int burnin=getParameter("burnin").toInt();
    if (burnin>out_nrow) burnin=out_nrow;
    ui->plainTextEdit->appendPlainText("Apply burnin ...");
    repaint();
    //qDebug()<<"Apply burnin...";
    // transfer out to array for analysis and apply burnin
    out_nrow=out_nrow-burnin;

    float *outdata=new float [out_nrow*out_ncol];

    for (int i=0;i<out_ncol;i++){
        for (int j=burnin;j<out_nrow+burnin;j++){
            outdata[i*out_nrow+(j-burnin)]=out[i*(out_nrow+burnin)+j];
        }
    }

    float dmin=getParameter("d.min").toFloat();
    float dby=(float)((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/getParameter("K").toInt());
    //******************    create hist for acc plot
    ui->plainTextEdit->appendPlainText("Create Accumulation Plot...");
    repaint();
    //qDebug()<<"It Plot";
    int bins=100;

    //delete[] acchist;
    acchist=new float[2*bins];
    acchist_ncol=2;
    acchist_nrow=bins;

    // get max and min
    float min=999999999;
    float max=-999999999;

    for (int i=1;i<out_ncol-2;i++){
        for (int j=0;j<out_nrow;j++){
            if (outdata[i*out_nrow+j]>max) max=outdata[i*out_nrow+j];
            if (outdata[i*out_nrow+j]<min) min=outdata[i*out_nrow+j];
        }
    }
    float res=(max-min)/bins;

    // Calc hist
    for (int j=0;j<bins*2;j++) acchist[j]=0.0;
    int hist_total=0;
    for (int i=1;i<out_ncol-2;i++){
        for (int j=0;j<out_nrow;j++){
            if (std::isnan(outdata[i*out_nrow+j])==false){
                int index=(int)((outdata[i*out_nrow+j]-min)/res);
                if (index>=0 && index<bins) {
                    acchist[1*bins+index]++;
                    hist_total++;
                }
            }
        }
    }

    // calc years for bins
    for (int j=0;j<bins;j++) acchist[j]=min+res*j+res/2.0;

    // calc density
    for (int j=0;j<bins;j++) acchist[1*bins+j]=(acchist[1*bins+j]/hist_total)/res;

    createAccPlot();

    //*****************   create hist for mem plot
    ui->plainTextEdit->appendPlainText("Create Memory Plot...");
    repaint();
    //qDebug()<<"Memmory Plot";
    bins=100;
    //delete[] memhist;
    memhist=new float[2*bins];
    memhist_ncol=2;
    memhist_nrow=bins;

    // get max and min
    min=0;
    max=1;
    float ddelta=(getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/getParameter("K").toFloat();

       /* for (int j=0;j<out_nrow;j++){
            if (pow(outdata[(out_ncol-2)*out_nrow+j],1.0/ddelta)>max) max=pow(outdata[(out_ncol-2)*out_nrow+j],1.0/ddelta);
            if (pow(outdata[(out_ncol-2)*out_nrow+j],1.0/ddelta)<min) min=pow(outdata[(out_ncol-2)*out_nrow+j],1.0/ddelta);
        }*/

    res=(max-min)/bins;

    // Calc hist
    for (int j=0;j<bins*2;j++) memhist[j]=0.0;
    hist_total=0;

        for (int j=0;j<out_nrow;j++){
            if (std::isnan(outdata[(out_ncol-2)*out_nrow+j])==false){
                int index=(int)((pow(outdata[(out_ncol-2)*out_nrow+j],1.0/ddelta)-min)/res);
                if (index>=0 && index<bins) {
                    memhist[1*bins+index]++;
                    hist_total++;
                }
            }
        }

    // calc segment-means for bins
    for (int j=0;j<bins;j++) memhist[j]=min+res*j+res/2.0;

    // calc density
    for (int j=0;j<bins;j++) memhist[1*bins+j]=(memhist[1*bins+j]/hist_total)/res;

    createMemPlot();


    //****************** Calculate agemodels by turning outdata accs into ages
    ui->plainTextEdit->appendPlainText("Transfering acc-rates to age-models...");
    repaint();
    //qDebug()<<"Calc acc rate";
    float section_size=(float)((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/getParameter("K").toInt());

    for (int j=1;j<out_ncol-2;j++){
        for (int i=0;i<out_nrow;i++) outdata[j*out_nrow+i]=outdata[(j-1)*out_nrow+i]+outdata[(j)*out_nrow+i]*section_size;
    }
/*
    Editor *c=new Editor(this,outdata,outdata,out_ncol,out_nrow);
    c->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    c->show();
*/
    // ************************* calc histogramms for any d
    ui->plainTextEdit->appendPlainText("Calculating Histogramms...");
    repaint();
    //qDebug()<<"Histogramms";
    // find the max length of a histogramm
    res=100;// resolution for age
    int bin_max=0;
    int depth_n=(getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/getParameter("d.by").toFloat();
    for (int i=0;i<depth_n;i++){
        // Calc hist of all column
        float min=99999999;
        float max=-99999999;
        float d=getParameter("d.min").toFloat()+i*getParameter("d.by").toFloat();
        for (int j=0;j<out_nrow;j++){
            int index=(int)((d-dmin)/dby);
            float v=outdata[index*out_nrow+j]+(outdata[(index+1)*out_nrow+j]-outdata[(index)*out_nrow+j])/(dby)*((d-dmin-index*dby));
            if (v>max) max=v;
            if (v<min) min=v;
        }
        int bins=(int)((max-min)/res)+1;
        if (bins>bin_max) bin_max=bins;
    }
    //initialize hist array
    delete[] hist;
    hist=new float[bin_max*depth_n*2];
    for (int i=0;i<bin_max*depth_n*2;i++) hist[i]=NAN;

    for (int i=0;i<depth_n;i++){
        float min=99999999;
        float max=-99999999;
        float d=getParameter("d.min").toFloat()+i*getParameter("d.by").toFloat();

        for (int j=0;j<out_nrow;j++){
            int index=(int)((d-dmin)/dby);
            float v=outdata[index*out_nrow+j]+(outdata[(index+1)*out_nrow+j]-outdata[(index)*out_nrow+j])/(dby)*((d-dmin-index*dby));
            if (v>max) max=v;
            if (v<min) min=v;
        }
        // min+n*res<max estimates bins
        int bins=(int)((max-min)/res)+1;
        // Calc hist
        for (int j=0;j<bins;j++) hist[(i*2+1)*bin_max+j]=0.0;
        int hist_total=0;
        for (int j=1;j<out_nrow;j++){
            int index=(int)((d-dmin)/dby);
            float v=outdata[index*out_nrow+j]+(outdata[(index+1)*out_nrow+j]-outdata[(index)*out_nrow+j])/(dby)*((d-dmin-index*dby));

            if (std::isnan(v)==false){
                int index=(int)((v-min)/res);
                hist[(i*2+1)*bin_max+index]++;
                hist_total++;
            }
        }
        // calc years for bins
        for(int j=0;j<bins;j++) hist[(i*2)*bin_max+j]=min+res*j+res/2.0;

        // calc probabillity
        for (int j=0;j<bins;j++) hist[(i*2+1)*bin_max+j]=hist[(i*2+1)*bin_max+j]/hist_total;
    }
    hist_ncol=depth_n*2;
    hist_nrow=bin_max;

/*
    Editor *c=new Editor(this,hist,hist,depth_n,bin_max);
    c->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    c->show();
*/

    // *************************** Calculate agemodell for any d
    ui->plainTextEdit->appendPlainText("Calculating Agemodel...");
    repaint();
    //qDebug()<<"Agemodel";
    delete[] agemodel;
    agemodel=new float[depth_n*5];
    for (int i=0;i<depth_n;i++){

        float d=getParameter("d.min").toFloat()+i*getParameter("d.by").toFloat();

        agemodel[0*depth_n+i]=getParameter("d.min").toFloat()+getParameter("d.by").toFloat()*i;
        // Calc mean directly from ages
        float sum=0;
        int c=0;
        for (int j=0;j<out_nrow;j++){
            int index=(int)((d-dmin)/dby);
            float v=outdata[index*out_nrow+j]+(outdata[(index+1)*out_nrow+j]-outdata[(index)*out_nrow+j])/(dby)*((d-dmin-index*dby));

            if (std::isnan(v)==false){
                sum=sum+v;
                c++;
            }
        }
        agemodel[1*depth_n+i]=sum/c;
        // Calc Lower 95% Quantile


        //estimate range of ages
        float min=99999999;
        float max=-99999999;
        for (int j=0;j<out_nrow;j++){
            int index=(int)((d-dmin)/dby);
            float v=outdata[index*out_nrow+j]+(outdata[(index+1)*out_nrow+j]-outdata[(index)*out_nrow+j])/(dby)*((d-dmin-index*dby));

            if (v>max) max=v;
            if (v<min) min=v;
        }
        // min+n*res<max estimates bins

        float cum=0;
        float inc=1;
        float p005=NAN;
        float p050=NAN;
        float p095=NAN;
        for (float age=min;age<max;age=age+inc){
            int index=(int)((age-min)/res);
            cum=cum+hist[(i*2+1)*bin_max+index]*(inc/res);
            if (cum>0.05 && isnan(p005)){
                p005=age-inc;
            }
            if (cum>0.50 && isnan(p050)){
                p050=age;
            }
            if (cum>0.95 && isnan(p095)){
                p095=age;
                break;
            }
        }
        agemodel[2*depth_n+i]=p005;
        agemodel[3*depth_n+i]=p050;
        agemodel[4*depth_n+i]=p095;
    }
    agemodel_ncol=5;
    agemodel_nrow=depth_n;
    ui->plainTextEdit->appendPlainText("Setting up Graphs and Tables...");
    repaint();
    //qDebug()<<"Graps & Tables";
    //delete enData;
    enData = new QStandardItemModel(agemodel_nrow,6,this);
    //qDebug()<<"Ensamble";
    setupEnsamble();
    //qDebug()<<"AMSPlot";
    createAMSPlot();



    //qDebug()<<"free resources";
    // free resources
    delete[] outdata;
    ui->plainTextEdit->appendPlainText("finished!!!\n");
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    //qDebug()<<"Finished";
    /*} catch (const std::exception& e){
        QMessageBox msgBox;
        msgBox.setText("An Error Occured!!!");
        QString error;
        error.clear();
        error=QString::fromUtf8(e.what());
        msgBox.setInformativeText("Error :"+error);
        QPushButton *changeButton = msgBox.addButton(tr("Quit Bacon"), QMessageBox::ActionRole);
        QPushButton *continueButton = msgBox.addButton(tr("Try Again"), QMessageBox::ActionRole);

        msgBox.exec();
    }*/
}


void Bacon::refresh(){
    if (baconrestart==0) calcResults();
}

void Bacon::reload(){
    ui->plainTextEdit->appendPlainText("\nReloading Data From Age File...");
    delete amsdata;
    amsdata=new AMSData(inv);
    amsdata->ReadCalib();

    amsdata->AMSRead();

    baconflag=new int[amsdata->get_Length()];

    for (int i=0;i<amsdata->get_Length();i++) {
        QString com=amsdata->get_Age_Comment(i);
        if (amsdata->get_Data(7,i)==1 || com.contains("BACON_ON"))
            baconflag[i]=1;
        else baconflag[i]=0;

        if (com.contains("BACON_OFF")) baconflag[i]=0;
        if (com.contains("BACON_ON")) baconflag[i]=1;
    }

    repaint();
    {
    const QSignalBlocker blocker(para);
    setParameter("K",QString::number(amsdata->get_bacon_K()));
    setParameter("d.min",QString::number(amsdata->get_bacon_d_min()));
    setParameter("d.max",QString::number(amsdata->get_bacon_d_max()));
    setParameter("d.by",QString::number(amsdata->get_bacon_d_by()));
    setParameter("acc.shape",QString::number(amsdata->get_bacon_acc_shape()));
    setParameter("acc.mean",QString::number(amsdata->get_bacon_acc_mean()));
    setParameter("mem.strength",QString::number(amsdata->get_bacon_mem_strength()));
    setParameter("mem.mean",QString::number(amsdata->get_bacon_mem_mean()));
    setParameter("cc",QString::number(amsdata->get_bacon_cc()));
    setParameter("cc1",amsdata->get_bacon_cc1());
    setParameter("cc2",amsdata->get_bacon_cc2());
    setParameter("cc3",amsdata->get_bacon_cc3());
    setParameter("cc4",amsdata->get_bacon_cc4());
    setParameter("postbomb",QString::number(amsdata->get_bacon_postbomb()));
    //setParameter("d.R",QString::number(amsdata->get_bacon_d_R()));
    //setParameter("d.STD",QString::number(amsdata->get_bacon_d_STD()));
    setParameter("t.a",QString::number(amsdata->get_bacon_t_a()));
    setParameter("t.b",QString::number(amsdata->get_bacon_t_b()));
    setParameter("normal",QString::number(amsdata->get_bacon_normal()));
    setParameter("suggest",QString::number(amsdata->get_bacon_suggest()));
    setParameter("th0",QString::number(amsdata->get_bacon_th0()));
    setParameter("th0p",QString::number(amsdata->get_bacon_th0p()));
    setParameter("burnin",QString::number(amsdata->get_bacon_burnin()));
    setParameter("ssize",QString::number(amsdata->get_bacon_ssize()));
    setParameter("MinYr",QString::number(amsdata->get_bacon_yr_min()));
    setParameter("MaxYr",QString::number(amsdata->get_bacon_yr_max()));


    out_nrow=amsdata->get_bacon_out_nrow();
    out_nrow_real=out_nrow;
    out_ncol=amsdata->get_bacon_out_ncol();
    out.clear();
    out.reserve(out_nrow*out_ncol);

    for (int i=0;i<out_nrow;i++){
        for (int j=0;j<out_ncol;j++){
            out[j*out_nrow+i]=amsdata->get_bacon_out(j*out_nrow+i);
        }
    }

    select=-1;
    select2=-1;

    setupParameters();
    delete[] hist;
    hist=new float[0];
    hist_ncol=0;
    hist_nrow=0;
    delete[] agemodel;
    agemodel=new float[0];
    agemodel_nrow=0;
    agemodel_ncol=0;
    //delete[] acchist;
    acchist=new float[0];
    acchist_nrow=0;
    acchist_ncol=0;
    //delete[] memhist;
    memhist=new float[0];
    memhist_nrow=0;
    memhist_ncol=0;

    // create Age model plot
    delete[] data1;
    data1=new float[0];
    amsplot=new Graph(this,data1,0,0);
    createAMSPlot();

    // create prior plot acc
    delete[] data2;
    data2=new float[0];
    accplot=new Graph(this,data2,0,0);
    createAccPlot();

    // create prior plot mem
    delete[] data3;
    data3=new float[0];
    memplot=new Graph(this,data3,0,0);
    createMemPlot();

    // create iteration plot it
    delete[] data4;
    data4=new float[0];
    itplot=new Graph(this,data4,0,0);
    createItPlot();
    modelData->clear();
    modelData=new QStandardItemModel(0,0,this);
    ui->tableView_3->setModel(modelData);

    ui->plainTextEdit->appendPlainText("Finished!!!\n");
    repaint();
    baconrestart=0;
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    if(out_nrow!=0 && out_ncol!=0) calcResults();
    }
    checkParameters();
}

void Bacon::save(){
    ui->plainTextEdit->appendPlainText("\nSaving Data In Age File...");
    repaint();
    int out_nrow_old=out_nrow_real;// get the whole field including burnin

    amsdata->create_bacon_out(out_nrow_old,out_ncol);
    for (int i=0;i<out_nrow_old;i++) for (int j=0;j<out_ncol;j++) amsdata->set_bacon_out(out[i+j*out_nrow_old],i,j);


    amsdata->set_bacon_K(getParameter("K").toInt());
    amsdata->set_bacon_d_min(getParameter("d.min").toFloat());
    amsdata->set_bacon_d_max(getParameter("d.max").toFloat());
    amsdata->set_bacon_d_by(getParameter("d.by").toFloat());
    amsdata->set_bacon_acc_shape(getParameter("acc.shape").toFloat());
    amsdata->set_bacon_acc_mean(getParameter("acc.mean").toFloat());
    amsdata->set_bacon_mem_strength(getParameter("mem.strength").toFloat());
    amsdata->set_bacon_mem_mean(getParameter("mem.mean").toFloat());
    amsdata->set_bacon_cc(getParameter("cc").toInt());
    amsdata->set_bacon_cc1(getParameter("cc1"));
    amsdata->set_bacon_cc2(getParameter("cc2"));
    amsdata->set_bacon_cc3(getParameter("cc3"));
    amsdata->set_bacon_cc4(getParameter("cc4"));
    amsdata->set_bacon_postbomb(getParameter("postbomb").toFloat());
    //amsdata->set_bacon_d_R(getParameter("d.R").toFloat());
    //amsdata->set_bacon_d_STD(getParameter("d.STD").toFloat());
    amsdata->set_bacon_t_a(getParameter("t.a").toFloat());
    amsdata->set_bacon_t_b(getParameter("t.b").toFloat());
    amsdata->set_bacon_normal(getParameter("normal").toInt());
    amsdata->set_bacon_suggest(getParameter("suggest").toInt());
    amsdata->set_bacon_th0(getParameter("th0").toFloat());
    amsdata->set_bacon_th0p(getParameter("th0p").toFloat());
    amsdata->set_bacon_burnin(getParameter("burnin").toInt());
    amsdata->set_bacon_ssize(getParameter("ssize").toInt());
    amsdata->set_bacon_yr_min(getParameter("MinYr").toFloat());
    amsdata->set_bacon_yr_max(getParameter("MaxYr").toFloat());



    amsdata->create_bacon_age(agemodel_nrow);
    for (int i=0;i<agemodel_nrow;i++){
        amsdata->set_bacon_age_depth(agemodel[i+0*agemodel_nrow],i);
        amsdata->set_bacon_age_mean(agemodel[i+1*agemodel_nrow],i);
        amsdata->set_bacon_age_median(agemodel[i+2*agemodel_nrow],i);
        amsdata->set_bacon_age_Q5(agemodel[i+3*agemodel_nrow],i);
        amsdata->set_bacon_age_Q95(agemodel[i+4*agemodel_nrow],i);
    }


    amsdata->AMSSave();
    ui->plainTextEdit->appendPlainText("Finished!!!\n");
    repaint();
    changes=false;


    QMessageBox msgBox;
            msgBox.setText("Agedata sucessfully saved.");
            msgBox.setInformativeText("This includes the results of a Bacon run.");


            msgBox.exec();

}

void Bacon::replotAMS(){
    createAMSPlot();
}

void Bacon::checkParameters(){
    bacon_invalid=0;

    if (getParameter("K").toInt()<2){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("K is invalid.");
        msgBox.setInformativeText("It must be 2 or larger.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("d.min").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("d.min is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("d.min").toFloat()>getParameter("d.max").toFloat()){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("d.min or d.max is invalid.");
        msgBox.setInformativeText("d.min must be smaller than d.max.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("d.max").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("d.max is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("d.by").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("d.by is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if ((getParameter("d.max").toFloat()-getParameter("d.min").toFloat())/getParameter("d.by").toFloat()<1){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("d.by is invalid.");
        msgBox.setInformativeText("Decrease its value or increase d range.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("acc.shape").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("acc.shape is invalid.");
        msgBox.setInformativeText("It must be positive. Also remember to set suggest to 0, or your value will be replaced with an estimate.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("acc.mean").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("acc.mean is invalid.");
        msgBox.setInformativeText("It must be positive. Also remember to set suggest to 0, or your value will be replaced with an estimate.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("acc.mean").toFloat()==getParameter("acc.shape").toFloat()){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("acc.mean or acc.shape is invalid.");
        msgBox.setInformativeText("It must not be equal. Also remember to set suggest to 0, or your value will be replaced with an estimate.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("mem.strength").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("mem.strength is invalid.");
        msgBox.setInformativeText("It must be positive. Also remember to set suggest to 0, or your value will be replaced with an estimate.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("mem.mean").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("mem.mean is invalid.");
        msgBox.setInformativeText("It must be positive. Also remember to set suggest to 0, or your value will be replaced with an estimate.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("acc.mean").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("acc.mean is invalid.");
        msgBox.setInformativeText("It must be positive. Also remember to set suggest to 0, or your value will be replaced with an estimate.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("cc").toInt()<1 || getParameter("cc").toInt()>4){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("cc is invalid.");
        msgBox.setInformativeText("It can only be 1, 2, 3, or 4.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("postbomb").toInt()!=1 && getParameter("postbomb").toInt()!=0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("postbomb is invalid.");
        msgBox.setInformativeText("It can only be 0 or 1.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    /*if (getParameter("d.STD").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("d.STD is invalid.");
        msgBox.setInformativeText("It can only be positive.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }*/
    if (getParameter("t.a").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("t.a is invalid.");
        msgBox.setInformativeText("It can only be positive.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("t.b").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("t.b is invalid.");
        msgBox.setInformativeText("It can only be positive.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("t.b").toFloat()-getParameter("t.a").toFloat()!=1){

        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("t.b-t.a is invalid.");
        msgBox.setInformativeText("It sould be 1.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}

    }
    if (getParameter("normal").toInt()!=1&&getParameter("normal").toInt()!=0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("This should be 1 or 0");
        msgBox.setInformativeText("I have no idea which other values are valid.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("suggest")!="1"&&getParameter("suggest")!="0"){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("This can only be 1(use it) or 0(do not use it)");
        msgBox.setInformativeText("This must be zero if you want to use your own values for d, acc, etc.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    /*
    if (getParameter("th0").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("th0 is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("th0p").toFloat()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("th0p is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }*/
    if (getParameter("burnin").toInt()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("burnin is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("burnin").toInt()>out_nrow_real){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("burnin is invalid.");
        msgBox.setInformativeText("It is bigger than the number of Iterations.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }
    if (getParameter("ssize").toInt()<0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("ssize is invalid.");
        msgBox.setInformativeText("It must be a positive value.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }

    if (getParameter("MinYr").toFloat()>getParameter("MaxYr").toFloat()){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("MinYr or MaxYr is invalid.");
        msgBox.setInformativeText("MinYr must be smaller than d.max.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }

    if (getParameter("suggest").toInt()==0){
        bacon_invalid=0;
        QMessageBox msgBox;
        msgBox.setText("WARNING: suggest is set to 0. ");
        msgBox.setInformativeText("Please set valid values for MinYr, MaxYr, d.min, d.max, th0, th0p and acc.mean manually.");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
        // check for valid values
        if (isnan(getParameter("MinYr").toFloat())==true ||
                isnan(getParameter("MaxYr").toFloat())==true ||
                isnan(getParameter("MinYr").toFloat())==true ||
                isnan(getParameter("th0").toFloat())==true ||
                isnan(getParameter("th0p").toFloat())==true ||
                isnan(getParameter("d.min").toFloat())==true ||
                isnan(getParameter("d.max").toFloat())==true ||
                isnan(getParameter("acc.mean").toFloat())==true){
            bacon_invalid=1;
            QMessageBox msgBox;
            msgBox.setText("One of the following values was not set");
            msgBox.setInformativeText("Please set valid values for MinYr, MaxYr, d.min, d.max, th0, th0p and acc.mean manually.");
            QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == changeButton) {}
        }
    }

    int sumflag=0;
    for (int i=0;i<amsdata->get_Length();i++) sumflag+=baconflag[i];
    if (sumflag==0){
        bacon_invalid=1;
        QMessageBox msgBox;
        msgBox.setText("There is no age data selected for this core!!!");
        msgBox.setInformativeText("Use the Correlation Tool to create an age model and/or set its flags");
        QPushButton *changeButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        msgBox.exec();
        if (msgBox.clickedButton() == changeButton) {}
    }





    if(bacon_invalid==0) {
        ui->pushButton->setEnabled(true);
    } else {
        ui->pushButton->setEnabled(false);
    }
}

void Bacon::reloadBacon(){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "PaleoDataView",
                                                                tr("This will reload data from the age file. Current changes will be lost."),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {


    } else {
        if (reloadB){
            showResults();
        }
    }
}

bool Bacon::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->graphicsView_2||obj==ui->graphicsView_3||obj==ui->graphicsView_4||obj==ui->tableView||obj==ui->tableView_2||obj==ui->tableView_3){
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

void Bacon::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    if (changes) {
        resBtn = QMessageBox::question( this,"PaleoDataView - Bacon",
                                        tr("There are unsaved changes.\nAre you sure?\n"),
                                        QMessageBox::Cancel | QMessageBox::Yes,
                                        QMessageBox::Yes);
    }
    if (resBtn == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void Bacon::openManual(){
    QDesktopServices::openUrl(QUrl("file:///"+resources->get_path_bacon()+"/manualBacon_2.2.pdf", QUrl::TolerantMode));
}

void Bacon::apply(){
   for (int i=0;i<inv->get_Length();i++){
       float depth=inv->get_data_Isotopes(0,i)*100;

       // calc value for depth
       float value=NAN;
       int j=1;
       for (j=1;j<agemodel_nrow;j++){
           if(agemodel[j+0*agemodel_nrow]>=depth && agemodel[(j-1)+0*agemodel_nrow]<depth){
               value=agemodel[(j-1)+3*agemodel_nrow]+
                       ((agemodel[(j)+3*agemodel_nrow]-agemodel[(j-1)+3*agemodel_nrow])/(agemodel[(j)+0*agemodel_nrow]-agemodel[(j-1)+0*agemodel_nrow]))*
                       (depth-agemodel[(j-1)+0*agemodel_nrow]);

               break;
           }
       }
       inv->set_data_Age(value/1000,i);
       ui->plainTextEdit->appendPlainText("Depth: "+QString::number(depth/100)+" Age :"+QString::number(value/1000)+" min :"+QString::number(agemodel[(j-1)+3*agemodel_nrow])+" max :"+QString::number(agemodel[(j)+3*agemodel_nrow]));
   }
   inv->saveData();
   ui->plainTextEdit->appendPlainText("Applied agemodel and results saved to proxy.\n");
   QMessageBox msgBox;
           msgBox.setText("Agedata added to proxy.");
           msgBox.setInformativeText("This will enable plotting vs. age in some tools.");


           msgBox.exec();
}

void Bacon::target(){

    QString QFilename = QFileDialog::getSaveFileName(this, tr("Select Save File"),
                                             resources->path_PaleoDataView+"/"+resources->workdir+"/"+inv->access_proxy_name()+"/Derived Data/Targets/"+inv->get_att_Core(),
                                             tr("Target File (*.tgt)"));
    if (QFilename!="" ){

    ui->plainTextEdit->appendPlainText("Creating Target File...:"+QFilename);

    // estimate median,mean und quantile via interpolation
    float* agedata=new float[(inv->access_data_length()+4)*inv->get_Length()];
    for (int i=0;i<inv->get_Length();i++){
        float depth=inv->get_data_Depth(i)*100;



        // calc value for depth
        float value_mean=NAN;
        float value_median=NAN;
        float value_min=NAN;
        float value_max=NAN;

        int j=1;
        for (j=1;j<agemodel_nrow;j++){
            if(agemodel[j+0*agemodel_nrow]>=depth && agemodel[(j-1)+0*agemodel_nrow]<depth){
                value_mean=agemodel[(j-1)+1*agemodel_nrow]+
                        ((agemodel[(j)+1*agemodel_nrow]-agemodel[(j-1)+1*agemodel_nrow])/(agemodel[(j)+0*agemodel_nrow]-agemodel[(j-1)+0*agemodel_nrow]))*
                        (depth-agemodel[(j-1)+0*agemodel_nrow]);
                value_median=agemodel[(j-1)+3*agemodel_nrow]+
                        ((agemodel[(j)+3*agemodel_nrow]-agemodel[(j-1)+3*agemodel_nrow])/(agemodel[(j)+0*agemodel_nrow]-agemodel[(j-1)+0*agemodel_nrow]))*
                        (depth-agemodel[(j-1)+0*agemodel_nrow]);
                value_min=agemodel[(j-1)+2*agemodel_nrow]+
                        ((agemodel[(j)+2*agemodel_nrow]-agemodel[(j-1)+2*agemodel_nrow])/(agemodel[(j)+0*agemodel_nrow]-agemodel[(j-1)+0*agemodel_nrow]))*
                        (depth-agemodel[(j-1)+0*agemodel_nrow]);
                value_max=agemodel[(j-1)+4*agemodel_nrow]+
                        ((agemodel[(j)+4*agemodel_nrow]-agemodel[(j-1)+4*agemodel_nrow])/(agemodel[(j)+0*agemodel_nrow]-agemodel[(j-1)+0*agemodel_nrow]))*
                        (depth-agemodel[(j-1)+0*agemodel_nrow]);


                break;
            }
        }

        for (int ii=0;ii<inv->access_data_length();ii++){
            agedata[i+ii*inv->get_Length()]=inv->access_data_value(ii,i).toFloat();
        }
        agedata[i+(inv->access_data_length()+0)*inv->get_Length()]=value_mean;
        agedata[i+(inv->access_data_length()+1)*inv->get_Length()]=value_median;
        agedata[i+(inv->access_data_length()+2)*inv->get_Length()]=value_min;
        agedata[i+(inv->access_data_length()+3)*inv->get_Length()]=value_max;
        ui->plainTextEdit->appendPlainText("Depth: "+QString::number(depth/100)+" Age Mean:"+QString::number(value_mean/1000)+" Median:"+QString::number(value_median/1000)+" 5%:"+QString::number(value_min/1000)+" 95%:"+QString::number(value_max/1000));
    }

    // Save the Data into netcdf
    error_text.clear();
    QString action="Save As Target";
    inv->readData(inv->get_currentCore());

    // get file name
    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
   //qDebug()<< "Save to :"+QFilename;


    // Initialize ids and meta
    int ncid;
    int retval;// for Errors

    // Create the file
    if ((retval = nc_create(filename, NC_WRITE,&ncid)))
       NetCDF_Error(filename,action,retval);

    //---------------- Define Attributes ---------------------



    for (int i=0;i<inv->access_meta_length();i++){
        if (inv->access_meta_type(i).contains("string") || inv->access_meta_type(i).contains("combo")) {
            if (!inv->access_meta_netcdf(i).contains("ignore")){
                QString att_name=inv->access_meta_netcdf(i).split(";").at(0);
                if ((retval = nc_put_att(ncid,NC_GLOBAL,att_name.toLatin1(),NC_CHAR,inv->access_meta_value(i).length(),inv->access_meta_value(i).toLatin1().data())))
                    NetCDF_Error(filename,action,retval);
                qDebug()<< inv->access_meta_value(i).toLatin1().data();
            }
        } else {
            if (!inv->access_meta_netcdf(i).contains("ignore")){
                QString att_name=inv->access_meta_netcdf(i).split(";").at(0);
                double v=inv->access_meta_value(i).toDouble();
                if ((retval = nc_put_att_double(ncid,NC_GLOBAL,att_name.toLatin1(),NC_DOUBLE,1,&v)))
                    NetCDF_Error(filename,action,retval);
               qDebug()<< QString::number(inv->access_meta_value(i).toDouble());
            }
        }
    }

    // Target Attributes
    // ->So far Nothing

    // length of inventory
    int drows=0;
    int length=inv->get_Length();
    if ((retval = nc_def_dim(ncid,"Length",length,&drows)))
       NetCDF_Error(filename,action,retval);
    qDebug() << "Length "+QString::number(drows);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // define
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("string") && inv->access_data_netcdf(i).contains("dim=")){
            // begin definition mode
            if ((retval = nc_redef(ncid)))
                NetCDF_Error(filename,action,retval);

            // define dimension
            QString dimname="";
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

            // length of unsigned character variables
            int var_length=0;
            for (int ii=0;ii<length;ii++) if (var_length<inv->access_data_value(i,ii).length()) var_length=inv->access_data_value(i,ii).length();
            int var=0;
            if ((retval = nc_def_dim(ncid,dimname.toLatin1(),var_length,&var)))
               NetCDF_Error(filename,action,retval);
            qDebug() << "length :"+QString::number(var_length);

            // define variable
            int dim[2];
            dim[0]=drows;
            dim[1]=var;
            int varid;
            QString varname=inv->access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_CHAR,2,dim,&varid)))
                NetCDF_Error(filename,action,retval);

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // write data
           unsigned char text[var_length][length];
           for (int ii=0;ii<var_length;ii++) for(int jj=0;jj<length;jj++) text[ii][jj]=' ';
            for (int ii=0;ii<length;ii++){
                for (int jj=0;jj<inv->access_data_value(i,ii).length();jj++) text[jj][ii]=inv->access_data_value(i,ii).at(jj).toLatin1();
            }
            if ((retval = nc_put_var(ncid,varid,&text[0][0])))
                NetCDF_Error(filename,action,retval);
            qDebug() << inv->access_data_name(i)+" written";

        } else {
            // begin definition mode
            if ((retval = nc_redef(ncid)))
                NetCDF_Error(filename,action,retval);

            // define
            int dimm[1];
            dimm[0]=drows;
            int varid;
            QString varname=inv->access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
                NetCDF_Error(filename,action,retval);

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // write data
            double dat[length];
            for (int ii=0;ii<length;ii++) {
                QString str=inv->access_data_value(i,ii);
                dat[ii]=str.toDouble();

            }

            if ((retval = nc_put_var(ncid,varid,&dat)))
                NetCDF_Error(filename,action,retval);
            qDebug() << inv->access_data_name(i)+" written";

        }


        }
    // put target data *******************
    {
    // age mean
    // begin definition mode
    if ((retval = nc_redef(ncid)))
        NetCDF_Error(filename,action,retval);

    // define
    int dimm[1];
    dimm[0]=drows;
    int varid;
    QString varname="Age Mean [ka]";
    qDebug()<<varname;
    if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
        NetCDF_Error(filename,action,retval);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // write data
    double dat[length];
    for (int ii=0;ii<length;ii++) {
        dat[ii]=agedata[ii+(inv->access_data_length()+0)*inv->get_Length()]/1000;
    }

    if ((retval = nc_put_var(ncid,varid,&dat)))
        NetCDF_Error(filename,action,retval);

    qDebug() <<"Age Mean written";
    }

    {
    // Age Median
    // begin definition mode
    if ((retval = nc_redef(ncid)))
        NetCDF_Error(filename,action,retval);

    // define
    int dimm[1];
    dimm[0]=drows;
    int varid;
    QString varname="Age Median [ka]";
    qDebug()<<varname;
    if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
        NetCDF_Error(filename,action,retval);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // write data
    double dat[length];
    for (int ii=0;ii<length;ii++) {
        dat[ii]=agedata[ii+(inv->access_data_length()+1)*inv->get_Length()]/1000;
    }

    if ((retval = nc_put_var(ncid,varid,&dat)))
        NetCDF_Error(filename,action,retval);
    qDebug() << "Age median written";
    }

    {
    // Age 05%
    // begin definition mode
    if ((retval = nc_redef(ncid)))
        NetCDF_Error(filename,action,retval);

    // define
    int dimm[1];
    dimm[0]=drows;
    int varid;
    QString varname="Age 05% [ka]";
    qDebug()<<varname;
    if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
        NetCDF_Error(filename,action,retval);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // write data
    double dat[length];
    for (int ii=0;ii<length;ii++) {
        dat[ii]=agedata[ii+(inv->access_data_length()+2)*inv->get_Length()]/1000;
    }

    if ((retval = nc_put_var(ncid,varid,&dat)))
        NetCDF_Error(filename,action,retval);
    qDebug() <<"Age 05 written";
    }

    {
    // Age 95%
    // begin definition mode
    if ((retval = nc_redef(ncid)))
        NetCDF_Error(filename,action,retval);

    // define
    int dimm[1];
    dimm[0]=drows;
    int varid;
    QString varname="Age 95% [ka]";
    qDebug()<<varname;
    if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
        NetCDF_Error(filename,action,retval);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // write data
    double dat[length];
    for (int ii=0;ii<length;ii++) {
        dat[ii]=agedata[ii+(inv->access_data_length()+3)*inv->get_Length()]/1000;
    }

    if ((retval = nc_put_var(ncid,varid,&dat)))
        NetCDF_Error(filename,action,retval);
    qDebug() << "Age 95 written";
    }

    if ((retval = nc_close(ncid)))
        NetCDF_Error(filename,action,retval);
    qDebug()<<"OK";
    delete[] filename;
    if (error_text!="") NetCDF_Error_Message();
    }
}

void Bacon::NetCDF_Error(QString filename,QString action,int value){
    error_text.append(filename+" : "+action+" : "+nc_strerror(value)+" : "+QString::number(value)+"\n");

}

void Bacon::NetCDF_Error_Message(){
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

void Bacon::OpenBacon(){
        ui->plainTextEdit->appendPlainText("\nPreparing Table (its big so it takes a while)...");
        repaint();
        delete showB;
        showB=new ShowBacon(this,out.data(),out_ncol+getParameter("burnin").toInt(),out_nrow,getParameter("d.min").toFloat(),getParameter("d.by").toFloat(),getParameter("d.max").toFloat(),getParameter("K").toInt());
        showB->setupOut();
        showB->setModal(true);
        showB->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        showB->show();

}

void Bacon::updateGraph(){
    createAMSPlot();
    createAccPlot();
    createMemPlot();
    createItPlot();
}

void Bacon::abort(){
    qDebug()<<"Abort";
    quit=1;
    p_bacon->kill();

}

void Bacon::copy(QPoint pos){

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

void Bacon::copy_2(QPoint pos){

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

void Bacon::copy_3(QPoint pos){

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
