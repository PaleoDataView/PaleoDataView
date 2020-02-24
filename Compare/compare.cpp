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

#include "compare.h"
#include "ui_compare.h"

Compare::Compare(QMainWindow *mainWindow,Inventory *inventory,QImage *image,QString str) :
    mainW(mainWindow),inv_main(inventory),mapimage(image),proxy(str),
    ui(new Ui::Compare)
{

    resources=new Resources();
    oldcore=inv_main->get_currentCore();
    ui->setupUi(this);
    this->setWindowTitle("Compare");
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);

    inv=new Inventory(proxy);
    inv->read();
    inv->read_Basin();
    for(int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
    for(int i=0;i<inv->get_Entries();i++) inv->set_AgeModel(i,inv_main->get_AgeModel(i));

    //Preparing Menu
    ui->comboBox->clear();
    // Add Data columns of current proxy to dropdown menu
    // first find Depth and age column
    depth=-1;
    age=-1;
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_name(i).contains("Depth")) depth=i;
        if (inv->access_data_name(i).contains("Age")) age=i;
    }
    // now create list
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("Comp")){
            if (depth>=0) {
               ui->comboBox->addItem(inv->access_data_name(i)+" (vs. Depth)");
            }
            if (age>=0) {
               ui->comboBox->addItem(inv->access_data_name(i)+" (vs. Age)");
            }
        }

    }
    ui->comboBox->setCurrentIndex(0);
    // Add Hydro datasets
    ui->comboBox->addItem("Age Model");
    ui->comboBox->addItem("Temperature");
    ui->comboBox->addItem("Salinity");
    ui->comboBox->addItem("d18o Seawater");

    // Prepare hydro-database
    hydro=new HydroDatabase();

    amsdata=new AMSData(inv);



    colorScheme=new ColorScheme();
    // Plot data
    color_red=new int[inv->get_Entries()];
    color_green=new int[inv->get_Entries()];
    color_blue=new int[inv->get_Entries()];
    plot=new int[inv->get_Entries()];
    for(int i=0;i<inv->get_Entries();i++){
        QColor col=colorScheme->getColor(i,0,inv->get_Entries(),ui->comboBox_3->currentIndex());
        color_red[i]=col.red();
        color_blue[i]=col.blue();
        color_green[i]=col.green();
        plot[i]=0;

    }


    //qDebug()<<"Reading Hydro Data Complete!";

    //modelIsotopeList=new QStandardItemModel(0,0,this);
    setupIsotope();
    if (inv->get_Entries()>0) setInfo();

    data2=new float[0];
    col2=new QColor[0];
    com2=new QString[0];
    lab2=new QString[0];
    size=new int[0];
    map=new Graph(this,data2,0,0);
    createMap(1);

    data1=new float[0];
    data_use1=new bool[0];
    col1=new QColor[0];
    com1=new QString[0];
    multiplot=new Graph(this,data1,0,0);
    qDebug()<<"setting up plot";
    createMultiPlot();
    //setupPlot();

    ui->graphicsView_3->setScene(new QGraphicsScene);

    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(applyIsotope(int)));// show all/selected cores
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(isotopeSelected(QModelIndex)));// entry selected
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(selectPlot()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(colorSelect()));
    connect(ui->comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updatePlot()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(autoColor()));
    connect(map,SIGNAL(selection(int,QList<int>,QList<int>)),this,SLOT(selection_changed(int,QList<int>,QList<int>)));
    connect(map,SIGNAL(selected(int,int,int)),this,SLOT(selected_changed(int,int,int)));
    connect(map,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(multiplot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(ui->comboBox_3,SIGNAL(currentTextChanged(QString)),this,SLOT(autoColor()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));

}

Compare::~Compare()
{
    delete ui;
    delete hydro;
    delete resources;
    delete amsdata;

    delete[] color_red;
    delete[] color_green;
    delete[] color_blue;
    delete[] plot;
    delete map;
    delete multiplot;
    delete[] data1;
    delete[] com1;
    delete[] col1;
    delete[] data_use1;
    delete[] data2;
    delete[] col2;
    delete[] com2;
    delete[] lab2;


    inv->set_currentCore(oldcore);

}


void Compare::setupIsotope(){
    // List of Cores
    // get number of entries that pass filter
    int sum=0;
    if (ui->comboBox_2->currentText()=="All")sum=inv->get_Entries();
    if (ui->comboBox_2->currentText()=="Only Selected") sum=inv->get_Selected_Sum();
    if (ui->comboBox_2->currentText()=="Only Displayed") for (int i=0;i<inv->get_Entries();i++) if (plot[i]) sum++;
    // create the model for Isotope list
    //delete modelIsotopeList;
    modelIsotopeList = new QStandardItemModel(sum,3,this);
    modelIsotopeList->setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    modelIsotopeList->setHorizontalHeaderItem(1, new QStandardItem(QString("Name of Core")));
    modelIsotopeList->setHorizontalHeaderItem(2, new QStandardItem(QString("Proxy")));
    modelIsotopeList->setHorizontalHeaderItem(3, new QStandardItem(QString("Color")));
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
    //QStandardItem *var_Plot = new QStandardItem[sum];

    int pos=0;
    for (int i=0;i<inv->get_Entries();i++){
        if (ui->comboBox_2->currentText()=="All" || (ui->comboBox_2->currentText()=="Only Selected"  && inv->get_Selected(i)==1)  || (ui->comboBox_2->currentText()=="Only Displayed"  && plot[i]==1)){


            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            modelIsotopeList->setItem(pos,0,var_Index);

            QStandardItem *var_Core=new QStandardItem(inv->get_Core(i));
            modelIsotopeList->setItem(pos,1,var_Core);

            QStandardItem *var_Species=new QStandardItem(inv->get_Species(i));
            modelIsotopeList->setItem(pos,2,var_Species);

            QStandardItem *var_Plot=new QStandardItem("");
            modelIsotopeList->setItem(pos,3,var_Plot);

            for (int j=0;j<3;j++) if (plot[i]) modelIsotopeList->setData(modelIsotopeList->index(pos,j), QColor(Qt::lightGray), Qt::BackgroundRole);
            if (plot[i]) modelIsotopeList->setData(modelIsotopeList->index(pos,3), QColor(color_red[i],color_green[i],color_blue[i],255), Qt::BackgroundRole);
            for (int j=0;j<3;j++) if (i==selected) modelIsotopeList->setData(modelIsotopeList->index(pos,j), QColor(Qt::gray), Qt::BackgroundRole);
            pos++;
        }
    }
    ui->tableView->setSortingEnabled(1);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    ui->tableView->repaint();
}

void Compare::applyIsotope(int n){
    setupIsotope();
}

void Compare::isotopeSelected(QModelIndex mi){

        int sel=mi.row();
        //qDebug() << "Clicked :"+QString::number(sel);
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
        QString text = model->item(sel,0)->text();
        //qDebug() << text;
        selected=text.toInt(0,10);

        setupIsotope();
        inv->readData(text.toInt(0,10));
        setInfo();
}

void Compare::setInfo(){
    inv->readData(selected);
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(12);

    ui->textBrowser->setFont(font);
    QString str="";
    int n=selected;
    if (n>=0){
        if (inv->get_flag_Data_OK()){
            for (int i=0;i<inv->access_meta_length();i++){
                QString sub="";
                sub.append("<b>"+inv->access_meta_name(i));
                if (inv->access_meta_unit(i)!="") sub.append(" ["+inv->access_meta_unit(i)+"]");
                for (int j=sub.length();j<20;j++) sub.append(".");
                sub.append(": </b>"+inv->access_meta_value(i));
                if (i+1<inv->access_meta_length()) sub.append("<br>");
                str.append(sub);
            }
        } else {

            for (int i=0;i<inv->access_inv_length();i++){
                QString sub="";
                sub.append("<b>"+inv->access_inv_name(i));
                if (inv->access_inv_unit(i)!="") sub.append(" ["+inv->access_inv_unit(i)+"]");
                for (int j=sub.length();j<20;j++) sub.append(".");
                sub.append(": </b>"+inv->access_inv_data(i,inv->get_currentCore()));
                if (i+1<inv->access_inv_length()) sub.append("<br>");
                str.append(sub);
            }
        }
    } else {
        str.append("Select a Core");
    }

    ui->textBrowser->setText(str);



    ui->graphicsView_3->setBackgroundBrush(QColor(color_red[n],color_green[n],color_blue[n]));


}

void Compare::paintEvent(QPaintEvent *)
{

    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    multiplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    //setupIsotope();

}

void Compare::updateList(){
    setupIsotope();
    setInfo();
    createMultiPlot();

    ui->tableView->setFocus();
}

void Compare::selectPlot(){
    if (plot[selected]){
        plot[selected]=0;
    } else {
        plot[selected]=1;
    }

    updateList();


    createMap(0);
    createMultiPlot();

    update();
}

void Compare::colorSelect(){
    QColorDialog cdiag;
    QColor col(cdiag.getColor(QColor(color_red[selected],color_green[selected],color_blue[selected]),this));
    if (col.isValid()){
        color_green[selected]=col.green();
        color_red[selected]=col.red();
        color_blue[selected]=col.blue();
        ui->graphicsView_3->setBackgroundBrush(QColor(color_red[selected],color_green[selected],color_blue[selected]));
        ui->graphicsView_3->repaint();
    }

    update();
}

void Compare::updatePlot(){
    createMultiPlot();   
}



void Compare::autoColor(){
    int count_total=0;
    for (int i=0;i<inv->get_Entries();i++) if(plot[i]) count_total++;
    if (count_total){
        count_total=count_total+2;
        int count=1;
        for(int i=0;i<inv->get_Entries();i++){
            if (plot[i]){
                QColor col=colorScheme->getColor(count,0,count_total,ui->comboBox_3->currentIndex());


                color_red[i]=col.red();
                color_blue[i]=col.blue();
                color_green[i]=col.green();
                ui->graphicsView_3->setBackgroundBrush(QColor(color_red[i],color_green[i],color_blue[i]));
                count++;
            }

        }
    }
    updateList();

    createMap(0);
    createMultiPlot();
    //setupPlot();
    update();
}

void Compare::createMultiPlot(){
    QString name=ui->comboBox->currentText();
    name=name.split(" (").at(0);
    name.simplified();
    // search for name
    int x=-1;
    int f=-1;
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_name(i)==name) x=i;
        if (inv->access_data_type(i).contains("dFlag=")) {
            QStringList str=inv->access_data_type(i).split(";");
            for (int j=0;j<str.size();j++) {
                if (str.at(j).contains("dFlag=")){
                    f=str.at(j).split("=").at(1).toInt();
                }
            }

        }
    }
    qDebug()<<QString::number(x)+" : "+QString::number(f);
    if (x>=0&&f>=0){
        // get the max size of data
        int max_row=0;
        int max_col=0;
        for (int i=0;i<inv->get_Entries();i++) max_col=max_col+plot[i];
        for (int i=0;i<inv->get_Entries();i++) {
            if (plot[i]){
                inv->set_currentCore(i);
                inv->readData(i);
                if (max_row<inv->get_Length()) max_row=inv->get_Length();
            }
        }
        delete[] data1;
        data1=new float[max_row*max_col*2];
        delete[] data_use1;
        data_use1=new bool[max_row*max_col*2];
        delete[] col1;
        col1=new QColor[max_col*2];
        delete[] com1;
        com1=new QString[max_row*max_col*2];
        int current_column=0;
        for (int i=0;i<inv->get_Entries();i++){
            if (plot[i]){
                inv->set_currentCore(i);
                inv->readData(i);
                for (int j=0;j<inv->get_Length();j++){
                    if (ui->comboBox->currentText().contains("(vs. Depth)")) data1[j+current_column*max_row]=inv->access_data_value(depth,j).toFloat(); else data1[j+current_column*max_row]=inv->access_data_value(age,j).toFloat();
                    data1[j+(current_column+1)*max_row]=inv->access_data_value(x,j).toFloat();
                    data_use1[j+current_column*max_row]=inv->access_data_value(f,j).toFloat();
                    data_use1[j+(current_column+1)*max_row]=inv->access_data_value(f,j).toFloat();
                    com1[j+current_column*max_row]=inv->get_att_Core()+"\n"+inv->get_att_Species();
                    com1[j+(current_column+1)*max_row]="";
                }
                for (int j=inv->get_Length();j<max_row;j++){
                    data1[j+current_column*max_row]=NAN;
                    data1[j+(current_column+1)*max_row]=NAN;
                }
                col1[current_column]=QColor(color_red[i],color_green[i],color_blue[i]);
                current_column=current_column+2;

            }
        }
        multiplot->setData(data1,max_col*2,max_row);
        multiplot->setUse(data_use1,1);
        if (inv->access_data_unit(x)!="")name.append(" ["+inv->access_data_unit(x)+"]");
        if (ui->comboBox->currentText().contains("(vs. Depth)")) multiplot->setTitel("","Depth [m]",name);
        else multiplot->setTitel("","Age [kyr]",name+" ["+inv->access_data_unit(x)+"]");

        multiplot->setMultiplicator(1,1);
        multiplot->setTextSize(resources->get_Pointsize(),1,0);
        multiplot->setSetColor(col1,1);
        multiplot->setSetLineColor(col1,1);
        multiplot->setComment(com1,2);
        multiplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        multiplot->setSymbol(3);
        multiplot->autoSize();
        if (ui->comboBox->currentText().contains("(vs. Depth)")) multiplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Compare_"+name+"_Depth_"+inv->access_proxy_abbreviation()+".txt",0); else multiplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Compare_"+name+"_Age.txt",0);
        ui->graphicsView_2->setScene(multiplot);

    }

    if (ui->comboBox->currentText()=="Age Model"){
        // get the max size of data
        int max_row=0;
        int max_col=0;
        for (int i=0;i<inv->get_Entries();i++) max_col=max_col+plot[i];
        for (int i=0;i<inv->get_Entries();i++) {
            if (plot[i]){
                inv->set_currentCore(i);

                amsdata->AMSRead();
                if (max_row<amsdata->get_Length()) max_row=amsdata->get_Length();
            }
        }
        delete[] data1;
        data1=new float[max_row*max_col*2];
        delete[] data_use1;
        data_use1=new bool[max_row*max_col*2];
        delete[] col1;
        col1=new QColor[max_col*2];
        delete[] com1;
        com1=new QString[max_row*max_col*2];
        int current_column=0;
        for (int i=0;i<inv->get_Entries();i++){
            if (plot[i]){
                inv->set_currentCore(i);

                amsdata->AMSRead();
                for (int j=0;j<amsdata->get_Length();j++){
                    data1[j+current_column*max_row]=amsdata->get_Depth(j);
                    data1[j+(current_column+1)*max_row]=amsdata->get_Data(4,j);
                    data_use1[j+current_column*max_row]=amsdata->get_Data(7,j);
                    data_use1[j+(current_column+1)*max_row]=amsdata->get_Data(7,j);
                    com1[j+current_column*max_row]=inv->get_Core(inv->get_currentCore())+"\n"+inv->get_Species(inv->get_currentCore());
                    com1[j+(current_column+1)*max_row]="";
                }
                for (int j=amsdata->get_Length();j<max_row;j++){
                    data1[j+current_column*max_row]=NAN;
                    data1[j+(current_column+1)*max_row]=NAN;
                }
                col1[current_column]=QColor(color_red[i],color_green[i],color_blue[i]);
                current_column=current_column+2;

            }
        }
        multiplot->setData(data1,max_col*2,max_row);
        multiplot->setUse(data_use1,1);
        multiplot->setTitel("","Depth [m]","Age [ka]");
        multiplot->setMultiplicator(1,1);
        multiplot->setTextSize(resources->get_Pointsize(),1,0);
        multiplot->setSetColor(col1,1);
        multiplot->setSetLineColor(col1,1);
        multiplot->setComment(com1,2);
        multiplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        multiplot->setSymbol(3);
        multiplot->autoSize();
        multiplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Compare_AgeModel.txt",0);
        ui->graphicsView_2->setScene(multiplot);
    }

    if (ui->comboBox->currentText()=="Temperature"){
        // get the max size of data
        int max_row=hydro->get_t_Depth_Size();
        int max_col=0;
        for (int i=0;i<inv->get_Entries();i++) max_col=max_col+plot[i];
        delete[] data1;
        data1=new float[max_row*max_col*2];

        delete[] col1;
        col1=new QColor[max_col*2];
        delete[] com1;
        com1=new QString[max_row*max_col*2];
        int current_column=0;
        for (int i=0;i<inv->get_Entries();i++){
            if (plot[i]){
                inv->set_currentCore(i);

                for (int j=0;j<hydro->get_t_Depth_Size();j++){
                    data1[j+(current_column+1)*max_row]=hydro->get_t_Depth(j);
                    float t=hydro->getTemperature(inv->get_Longitude(i),inv->get_Latitude(i),hydro->get_t_Depth(j));
                    if (t>-1000 && t<1000){
                        data1[j+(current_column)*max_row]=t;
                    }else{
                        data1[j+(current_column)*max_row]=NAN;
                    }
                    com1[j+current_column*max_row]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                    com1[j+(current_column+1)*max_row]="";
                }

                col1[current_column]=QColor(color_red[i],color_green[i],color_blue[i]);
                current_column=current_column+2;

            }
        }
        multiplot->setData(data1,max_col*2,max_row);
        multiplot->setUse(data_use1,0);
        multiplot->setTitel("","Temperature [°C]","Depth [m]");
        multiplot->setMultiplicator(1,-1);
        multiplot->setTextSize(resources->get_Pointsize(),1,0);
        multiplot->setSetColor(col1,1);
        multiplot->setSetLineColor(col1,1);
        multiplot->setComment(com1,2);
        multiplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        multiplot->setSymbol(3);
        multiplot->autoSize();
        multiplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Compare_Temp.txt",0);
        ui->graphicsView_2->setScene(multiplot);
    }

    if (ui->comboBox->currentText()=="Salinity"){
        // get the max size of data
        int max_row=hydro->get_s_Depth_Size();
        int max_col=0;
        for (int i=0;i<inv->get_Entries();i++) max_col=max_col+plot[i];

        delete[] data1;
        data1=new float[max_row*max_col*2];

        delete[] col1;
        col1=new QColor[max_col*2];
        delete[] com1;
        com1=new QString[max_row*max_col*2];
        int current_column=0;
        for (int i=0;i<inv->get_Entries();i++){
            if (plot[i]){
                inv->set_currentCore(i);

                for (int j=0;j<hydro->get_s_Depth_Size();j++){
                    data1[j+(current_column+1)*max_row]=hydro->get_s_Depth(j);
                    float t=hydro->getSalinity(inv->get_Longitude(i),inv->get_Latitude(i),hydro->get_s_Depth(j));
                    if (t>-1000 && t<1000){
                        data1[j+(current_column)*max_row]=t;
                    }else{
                        data1[j+(current_column)*max_row]=NAN;
                    }
                    com1[j+current_column*max_row]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                    com1[j+(current_column+1)*max_row]="";
                }

                col1[current_column]=QColor(color_red[i],color_green[i],color_blue[i]);
                current_column=current_column+2;

            }
        }
        multiplot->setData(data1,max_col*2,max_row);
        multiplot->setUse(data_use1,0);
        multiplot->setTitel("","Salinity [psu]","Depth [m]");
        multiplot->setMultiplicator(1,-1);
        multiplot->setTextSize(resources->get_Pointsize(),1,0);
        multiplot->setSetColor(col1,1);
        multiplot->setSetLineColor(col1,1);
        multiplot->setComment(com1,2);
        multiplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        multiplot->setSymbol(3);
        multiplot->autoSize();
        multiplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Compare_Salt.txt",0);
        ui->graphicsView_2->setScene(multiplot);
    }

    if (ui->comboBox->currentText()=="d18o Seawater"){
        // get the max size of data
        int max_row=hydro->get_d18o_Depth_Size();
        int max_col=0;
        for (int i=0;i<inv->get_Entries();i++) max_col=max_col+plot[i];
        delete[] data1;
        data1=new float[max_row*max_col*2];

        delete[] col1;
        col1=new QColor[max_col*2];
        delete[] com1;
        com1=new QString[max_row*max_col*2];
        int current_column=0;
        for (int i=0;i<inv->get_Entries();i++){
            if (plot[i]){
                inv->set_currentCore(i);

                for (int j=0;j<hydro->get_d18o_Depth_Size();j++){
                    data1[j+(current_column+1)*max_row]=hydro->get_d18o_Depth(j);
                    float t=hydro->getd18o(inv->get_Longitude(i),inv->get_Latitude(i),hydro->get_d18o_Depth(j));
                    if (t>-1000 && t<1000){
                        data1[j+(current_column)*max_row]=t;
                    }else{
                        data1[j+(current_column)*max_row]=NAN;
                    }
                    com1[j+current_column*max_row]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                    com1[j+(current_column+1)*max_row]="";
                }

                col1[current_column]=QColor(color_red[i],color_green[i],color_blue[i]);
                current_column=current_column+2;

            }
        }
        multiplot->setData(data1,max_col*2,max_row);
        multiplot->setUse(data_use1,0);
        multiplot->setTitel("","d18O Seawater (o/oo SMOW)","Depth [m]");
        multiplot->setMultiplicator(1,-1);
        multiplot->setTextSize(resources->get_Pointsize(),1,0);
        multiplot->setSetColor(col1,1);
        multiplot->setSetLineColor(col1,1);
        multiplot->setComment(com1,2);
        multiplot->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
        multiplot->setSymbol(3);
        multiplot->autoSize();
        multiplot->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Compare_Seawater.txt",0);
        ui->graphicsView_2->setScene(multiplot);
    }
    ui->graphicsView_2->repaint();
}

bool Compare::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==this||obj==ui->graphicsView||obj==ui->tableView||obj==ui->graphicsView_2||obj==ui->graphicsView_3||obj==ui->textBrowser){
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

void Compare::createMap(int first){
    delete[] data2;
    data2=new float[inv->get_Entries()*4];
    delete[] com2;
    com2=new QString[inv->get_Entries()*4];
    delete[] lab2;
    lab2=new QString[inv->get_Entries()*4];
    delete[] col2;
    col2=new QColor[inv->get_Entries()*4];
    delete[] size;
    size=new int[4];
    size[0]=8;
    size[1]=8;
    size[2]=5;
    size[3]=5;


    for (int i=0;i<inv->get_Entries();i++){
        data2[i+2*inv->get_Entries()]=inv->get_Longitude(i);
        data2[i+3*inv->get_Entries()]=inv->get_Latitude(i);
        com2[i+2*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        lab2[i+2*inv->get_Entries()]=inv->get_Core(i);
        col2[i+2*inv->get_Entries()]=Qt::black;
        col2[i+3*inv->get_Entries()]=QColor(255,255,255,128);
        if(plot[i]) {
            data2[i+0*inv->get_Entries()]=inv->get_Longitude(i);
            data2[i+1*inv->get_Entries()]=inv->get_Latitude(i);
            com2[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
            lab2[i+0*inv->get_Entries()]=inv->get_Core(i);
            col2[i+0*inv->get_Entries()]=Qt::black;
            col2[i+1*inv->get_Entries()]=QColor(color_red[i],color_green[i],color_blue[i],128);
            data2[i+2*inv->get_Entries()]=NAN;
            data2[i+3*inv->get_Entries()]=NAN;
        } else {
            data2[i+0*inv->get_Entries()]=NAN;
            data2[i+1*inv->get_Entries()]=NAN;
        }

    }
    map->setData(data2,4,inv->get_Entries());
    map->setSetSymbolsize(size,1);
    map->setMultiplicator(1,1);
    map->setColor(col2,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(com2,2);
    map->setLabel(lab2,1);
    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(mapimage,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Resources/Plot/Comp_Map.txt");
    if (first) map->autoSize();
    ui->graphicsView->setScene(map);
    ui->graphicsView->repaint();
}
void Compare::selection_changed(int p,QList<int> x,QList<int> y){
    //qDebug()<<"Plot send:"+QString::number(plot);
    for (int i=0;i<x.length();i++){
        //qDebug()<<QString::number(x.at(i))+" : "+QString::number(y.at(i));
        if (plot[y.at(i)]) {
            plot[y.at(i)]=0;
        }else{
            plot[y.at(i)]=1;
        }
        selected=y.at(i);


    }
    //map->setSelected_X(-1);
    //map->setSelected_Y(-1);
    setInfo();
    createMap(0);

    updateList();
}

void Compare::selected_changed(int p,int x,int y){
    if(y>-1){
    selected=y;
    //map->setSelected_X(-1);
    //map->setSelected_Y(-1);
    setInfo();
    }

}

void Compare::updateGraph(){
    createMultiPlot();
    createMap(1);
}

void Compare::copy(QPoint pos){

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
