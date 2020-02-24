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

#include "hydro.h"
#include "ui_hydro.h"

Hydro::Hydro(QMainWindow *mainWindow,Inventory *inventory,QImage *wmap) :
    mainW(mainWindow),inv(inventory),worldmap(wmap),
    ui(new Ui::Hydro)
{

    // estimate index in inventory
    resources=new Resources();
    ui->setupUi(this);
    this->setWindowTitle("Atlas Hydrography for Core : "+inv->get_Core(inv->get_currentCore())+" : "+inv->get_Species(inv->get_currentCore())+" at "+QString::number(inv->get_Longitude(inv->get_currentCore()))+"° :"+QString::number(inv->get_Latitude(inv->get_currentCore()))+"° in "+QString::number(inv->get_Water_Depth(inv->get_currentCore()))+"m");

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();

    qApp->installEventFilter(this);


    // Prepare hydro-database
    hydro=new HydroDatabase();

    colorScheme=new ColorScheme();
    inv->read_Basin();
    //qDebug()<<"Reading Data Complete!";
    float tp=hydro->getPTemperature(-20,0,1000);

    // set Water Depth
    ui->horizontalSlider->setValue(inv->get_Water_Depth(inv->get_currentCore()));
    ui->spinBox->setValue(inv->get_Water_Depth(inv->get_currentCore()));
    //qDebug()<<"All World Map Complete!";

    // create Worldmap
    mapdata=new float[0];
    mapcol=new QColor[0];
    maplab=new QString[0];
    mapcom=new QString[0];
    mapuse=new bool[inv->get_Entries()*2];
    for (int i=0;i<inv->get_Entries();i++){
        mapuse[i+0*inv->get_Entries()]=false;
        mapuse[i+1*inv->get_Entries()]=false;
        if (i==inv->get_currentCore()) {
            mapuse[i+0*inv->get_Entries()]=true;
            mapuse[i+1*inv->get_Entries()]=true;
        }
    }


    map=new Graph(this,mapdata,0,0);
    map->setColorScheme(0,0);
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    createMap(1);

    //plot
    plotdata=new float[0];
    plotcol=new QColor[0];
    plot=new Graph(this,plotdata,0,0);
    createPlot();

    tsdata=new float[0];
    tscom=new QString[0];
    tscol=new QColor[0];
    size=new int[0];
    autosize=new int[0];
    plotTS=new Graph(this,tsdata,0,0);
    createTSPlot();





    // set Label
    ui->label_3->setText("Used GridBox : "
            +QString::number(-180+hydro->getBox_Longitude(inv->get_att_Longitude()))+"° / "
            +QString::number(hydro->getBox_Latitude(inv->get_att_Latitude()))+"° / "
            +QString::number(hydro->getBox_Depth(inv->get_att_Water_Depth()))+"m");
    //float t=hydro->getdc(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore()));
    //ui->label_5->setText("Shackleton 1974, Uvigerina, "+QString::number(inv->get_att_Water_Depth())+"m : "+QString::number(t));
    //ui->label_4->setText("Shackleton 1974, Cibicides, "+QString::number(inv->get_att_Water_Depth())+"m : "+QString::number(t-0.64));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(depth_changed(int)));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(depth_changed(int)));

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(set_focus(bool)));
    connect(ui->checkBox,SIGNAL(clicked(bool)),this,SLOT(up_date()));
    connect(ui->checkBox_2,SIGNAL(clicked(bool)),this,SLOT(up_date()));
    //connect(ui->checkBox_3,SIGNAL(clicked(bool)),this,SLOT(up_date()));
    connect(map,SIGNAL(click(int,float,float)),this,SLOT(getCompare(int,float,float)));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),SLOT(up_date()));

    //qDebug()<<"Text Box and Connect Complete!";
    connect(ui->doubleSpinBox,SIGNAL(editingFinished()),SLOT(newCompare()));
    connect(ui->doubleSpinBox_2,SIGNAL(editingFinished()),SLOT(newCompare()));
    connect(ui->doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(newCompare()));
    connect(ui->doubleSpinBox_2,SIGNAL(valueChanged(double)),SLOT(newCompare()));
    connect(plot,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(map,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(plotTS,SIGNAL(graphChanged()),this,SLOT(updateGraph()));

    update();
}

Hydro::~Hydro()
{
    //qDebug()<<"Ceaning Resources....";
    if(plot!=NULL) delete plot;
    //qDebug()<<"plotTD erased";

    if(plotTS!=NULL) delete plotTS;
    //qDebug()<<"plotTS erased";



    if(map!=NULL)delete map;
    //qDebug()<<"World Map erased";
    if(mask!=NULL)delete mask;
    //qDebug()<<"mask";
    if(plotdata!=NULL)delete[] plotdata;
    //qDebug()<<"plotdata";
    if(mapdata!=NULL)delete[] mapdata;
    //qDebug()<<"mapdata";
    if(tsdata!=NULL)delete[] tsdata;
    //qDebug()<<"tsdata";
    if(tscom!=NULL)delete[] tscom;
    //qDebug()<<"tscom";
    if(mapcom!=NULL)delete[] mapcom;
    //qDebug()<<"mapcom";
    if(plotcol!=NULL)delete[] plotcol;
    //qDebug()<<"plotcol";
    if(mapcol!=NULL)delete[] mapcol;
    //qDebug()<<"mapcol";
    if(tscol!=NULL)delete[] tscol;
    //qDebug()<<"tscol";
    if(maplab!=NULL)delete[] maplab;
    if(mapuse!=NULL)delete[] mapuse;
    //qDebug()<<"maplab";
    if(hydro!=NULL)delete hydro;
    //qDebug()<<"hydro";
    if(resources!=NULL)delete resources;
    //qDebug()<<"resources";
    if(size!=NULL)delete[] size;
    if(autosize!=NULL)delete[] autosize;
    if(ui!=NULL)delete ui;
    //qDebug()<<"GUI erased";
    //qDebug()<<"Destructor Complete!";
}

void Hydro::paintEvent(QPaintEvent *)
{
    //qDebug()<<"Paint...";



    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());

    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());

    plotTS->setSize(ui->graphicsView_5->width(),ui->graphicsView_5->height());

    //qDebug()<<"Paint Finished";
    //if (comp_long<1000 && comp_long>-1000 && comp_latt>-1000 && comp_latt<1000)ui->label_6->setText("Longitude: "+QString::number(comp_long)+" Lattitude: "+QString::number(comp_latt)+" : "+inv->get_Basinname(inv->get_Basin(comp_long,comp_latt)));
}
void Hydro::up_date(){
    ui->label_5->setText("° "+inv->get_Basinname(comp_long,comp_latt));
    createPlot();
    createTSPlot();
    createMap(0);

    update();
}

void Hydro::getCompare(int plot,float x,float y){

    comp_latt=y;
    comp_long=x;
    map->addMarker(x,y,1);
    ui->doubleSpinBox->setValue(x);
    ui->doubleSpinBox_2->setValue(y);


    up_date();
}

void Hydro::newCompare(){
    comp_long=ui->doubleSpinBox->value();
    comp_latt=ui->doubleSpinBox_2->value();
    map->addMarker(comp_long,comp_latt,1);
    up_date();
}

void Hydro::depth_changed(int i){
    //qDebug() << QString::number(i);
    ui->spinBox->setValue(i);
    ui->horizontalSlider->setValue(i);

    createMap(0);

    update();
}



void Hydro::set_focus(bool b){
    //qDebug()<<"Focus set";
        // set Water Depth
        int i=(int) inv->get_Water_Depth(inv->get_currentCore());
        ui->spinBox->setValue(i);
        ui->horizontalSlider->setValue(i);

        map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);

        update();
}

void Hydro::createPlot(){

    if (ui->comboBox->currentText()=="Temperature (WOA, Locarnini et al. 2009)") createTDPlot();
    if (ui->comboBox->currentText()=="Salinity (WOA, Antonov et al. 2009)") createSDPlot();
    if (ui->comboBox->currentText()=="Delta-O-18 (seawater, Legrande & Schmidt, 2006)") create18oPlot();
    if (ui->comboBox->currentText()=="Delta-C-13 (DIC, Eide et al., 2017)") createCPlot();
    if (ui->comboBox->currentText()=="Delta-O-18 (carbonate, Shackleton, 1974)") createDCPlot();
    if (ui->comboBox->currentText()=="Delta-O-18 (carbonate, Marchitto et al., 2014)") createDCMPlot();
    ui->graphicsView_8->repaint();
}

void Hydro::createTDPlot(){
    //qDebug()<<QString::number(inv->get_Longitude(inv->get_currentCore()))+":"+QString::number(inv->get_Latitude(inv->get_currentCore()));
    //qDebug()<<QString::number(comp_long)+":"+QString::number(comp_latt);
    delete[] plotdata;
    plotdata=new float[hydro->get_t_Depth_Size()*4];
    for(int i=0;i<hydro->get_t_Depth_Size()*4;i++) plotdata[i]=NAN;
    delete[] plotcol;
    plotcol=new QColor[4];
    plotcol[0]=Qt::blue;
    plotcol[1]=Qt::black;
    plotcol[2]=Qt::gray;
    plotcol[3]=Qt::black;
    for (int i=0;i<hydro->get_t_Depth_Size();i++){
        float t=hydro->getTemperature(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));

        if (t>-1000 && t<1000) {
            plotdata[i+0*hydro->get_t_Depth_Size()]=t;
            plotdata[i+1*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
        } else {
            plotdata[i+0*hydro->get_t_Depth_Size()]=NAN;
            plotdata[i+1*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){
            t=hydro->getTemperature(comp_long,comp_latt,hydro->get_t_Depth(i));

            if (t>-1000 && t<1000 ) {
                plotdata[i+2*hydro->get_t_Depth_Size()]=t;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
            } else {
                plotdata[i+2*hydro->get_t_Depth_Size()]=NAN;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);;
            }
        }
    }
    plot->setData(plotdata,4,hydro->get_t_Depth_Size());
    plot->setTitel("","Temperature [°C]","Depth [m]");
    plot->setMultiplicator(1,-1);
    plot->setTextSize(resources->get_Pointsize(),1,0);
    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());
    plot->setSymbol(3);
    plot->setSetColor(plotcol,1);
    plot->addMarker(hydro->getTemperature(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),inv->get_Water_Depth(inv->get_currentCore()),1);
    plot->autoSize();
    plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_TD.txt",0);
    ui->graphicsView_8->setScene(plot);
}

void Hydro::createSDPlot(){
    delete[] plotdata;
    plotdata=new float[hydro->get_t_Depth_Size()*4];
    for(int i=0;i<hydro->get_t_Depth_Size()*4;i++) plotdata[i]=NAN;
    delete[] plotcol;
    plotcol=new QColor[4];
    plotcol[0]=Qt::blue;
    plotcol[1]=Qt::black;
    plotcol[2]=Qt::gray;
    plotcol[3]=Qt::black;
    for (int i=0;i<hydro->get_t_Depth_Size();i++){
        float t=hydro->getSalinity(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));
        plotdata[i+0*hydro->get_t_Depth_Size()]=t;
        if (t>-1000 && t<1000) {
            plotdata[i+1*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
        } else {
            plotdata[i+1*hydro->get_t_Depth_Size()]=NAN;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){
            t=hydro->getSalinity(comp_long,comp_latt,hydro->get_t_Depth(i));
            if (t>-1000 && t<1000) {
                plotdata[i+2*hydro->get_t_Depth_Size()]=t;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
            } else {
                plotdata[i+2*hydro->get_t_Depth_Size()]=NAN;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);;
            }
        }
    }
    plot->setData(plotdata,4,hydro->get_t_Depth_Size());
    plot->setTitel("","Salinity [psu]","Depth [m]");
    plot->setMultiplicator(1,-1);
    plot->setTextSize(resources->get_Pointsize(),1,0);
    plot->setSetColor(plotcol,1);
    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());
    plot->setSymbol(3);
    plot->addMarker(hydro->getSalinity(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),inv->get_Water_Depth(inv->get_currentCore()),1);
    plot->autoSize();
    plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_SD.txt",0);
    ui->graphicsView_8->setScene(plot);
}

void Hydro::create18oPlot(){
    delete[] plotdata;
    plotdata=new float[hydro->get_t_Depth_Size()*4];
    for(int i=0;i<hydro->get_t_Depth_Size()*4;i++) plotdata[i]=NAN;
    delete[] plotcol;
    plotcol=new QColor[4];
    plotcol[0]=Qt::blue;
    plotcol[1]=Qt::black;
    plotcol[2]=Qt::gray;
    plotcol[3]=Qt::black;
    for (int i=0;i<hydro->get_t_Depth_Size();i++){
        float t=hydro->getd18o(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));
        plotdata[i+0*hydro->get_t_Depth_Size()]=t;
        if (t>-1000 && t<1000) {
            plotdata[i+1*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
        } else {
            plotdata[i+1*hydro->get_t_Depth_Size()]=NAN;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){
            t=hydro->getd18o(comp_long,comp_latt,hydro->get_t_Depth(i));
            if (t>-1000 && t<1000) {
                plotdata[i+2*hydro->get_t_Depth_Size()]=t;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
            } else {
                plotdata[i+2*hydro->get_t_Depth_Size()]=NAN;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);;
            }
        }
    }
    plot->setData(plotdata,4,hydro->get_t_Depth_Size());
    plot->setTitel("","d18O Seawater [o/oo SMOW]","Depth [m]");
    plot->setMultiplicator(1,-1);
    plot->setTextSize(resources->get_Pointsize(),1,0);
    plot->setSetColor(plotcol,1);
    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());
    plot->setSymbol(3);
    plot->addMarker(hydro->getd18o(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),inv->get_Water_Depth(inv->get_currentCore()),1);
    plot->autoSize();
    plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_18o.txt",0);
    ui->graphicsView_8->setScene(plot);
}

void Hydro::createDCPlot(){
    delete[] plotdata;
    plotdata=new float[hydro->get_t_Depth_Size()*4];
    for(int i=0;i<hydro->get_t_Depth_Size()*4;i++) plotdata[i]=NAN;
    delete[] plotcol;
    plotcol=new QColor[4];
    plotcol[0]=Qt::blue;
    plotcol[1]=Qt::black;
    plotcol[2]=Qt::gray;
    plotcol[3]=Qt::black;
    for (int i=0;i<hydro->get_t_Depth_Size();i++){
        float t=hydro->getdc(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));
        plotdata[i+0*hydro->get_t_Depth_Size()]=t;
        if (t>-1000 && t<1000) {
            plotdata[i+1*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
        } else {
            plotdata[i+1*hydro->get_t_Depth_Size()]=NAN;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){
            t=hydro->getdc(comp_long,comp_latt,hydro->get_t_Depth(i));
            if (t>-1000 && t<1000) {
                plotdata[i+2*hydro->get_t_Depth_Size()]=t;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
            } else {
                plotdata[i+2*hydro->get_t_Depth_Size()]=NAN;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);;
            }
        }
    }
    plot->setData(plotdata,4,hydro->get_t_Depth_Size());
    plot->setTitel("","Delta-O-18 (carbonate) [o/oo]","Depth [m]");
    plot->setMultiplicator(1,-1);
    plot->setTextSize(resources->get_Pointsize(),1,0);
    plot->setSetColor(plotcol,1);
    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());
    plot->setSymbol(3);
    plot->addMarker(hydro->getdc(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),inv->get_Water_Depth(inv->get_currentCore()),1);
    plot->autoSize();
    plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_DC.txt",0);
    ui->graphicsView_8->setScene(plot);
}

void Hydro::createDCMPlot(){
    delete[] plotdata;
    plotdata=new float[hydro->get_t_Depth_Size()*4];
    for(int i=0;i<hydro->get_t_Depth_Size()*4;i++) plotdata[i]=NAN;
    delete[] plotcol;
    plotcol=new QColor[4];
    plotcol[0]=Qt::blue;
    plotcol[1]=Qt::black;
    plotcol[2]=Qt::gray;
    plotcol[3]=Qt::black;
    for (int i=0;i<hydro->get_t_Depth_Size();i++){
        float t=hydro->getdcm(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));
        plotdata[i+0*hydro->get_t_Depth_Size()]=t;
        if (t>-1000 && t<1000) {
            plotdata[i+1*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
        } else {
            plotdata[i+1*hydro->get_t_Depth_Size()]=NAN;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){
            t=hydro->getdcm(comp_long,comp_latt,hydro->get_t_Depth(i));
            if (t>-1000 && t<1000) {
                plotdata[i+2*hydro->get_t_Depth_Size()]=t;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);
            } else {
                plotdata[i+2*hydro->get_t_Depth_Size()]=NAN;
                plotdata[i+3*hydro->get_t_Depth_Size()]=hydro->get_t_Depth(i);;
            }
        }
    }
    plot->setData(plotdata,4,hydro->get_t_Depth_Size());
    plot->setTitel("","Delta-O-18 (carbonate) [o/oo]","Depth [m]");
    plot->setMultiplicator(1,-1);
    plot->setTextSize(resources->get_Pointsize(),1,0);
    plot->setSetColor(plotcol,1);
    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());
    plot->setSymbol(3);
    plot->addMarker(hydro->getdcm(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),inv->get_Water_Depth(inv->get_currentCore()),1);
    plot->autoSize();
    plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_DCM.txt",0);
    ui->graphicsView_8->setScene(plot);
}

void Hydro::createCPlot(){
    delete[] plotdata;
    plotdata=new float[hydro->get_c_Depth_Size()*4];
    for(int i=0;i<hydro->get_c_Depth_Size()*4;i++) plotdata[i]=NAN;
    delete[] plotcol;
    plotcol=new QColor[4];
    plotcol[0]=Qt::blue;
    plotcol[1]=Qt::black;
    plotcol[2]=Qt::gray;
    plotcol[3]=Qt::black;
    for (int i=0;i<hydro->get_c_Depth_Size();i++){
        float c=hydro->getCarbon(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_c_Depth(i));
        plotdata[i+0*hydro->get_c_Depth_Size()]=c;
        if (c>-1000 && c<1000) {
            plotdata[i+1*hydro->get_c_Depth_Size()]=hydro->get_c_Depth(i);
        } else {
            plotdata[i+1*hydro->get_t_Depth_Size()]=NAN;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){
            c=hydro->getCarbon(comp_long,comp_latt,hydro->get_c_Depth(i));
            if (c>-1000 && c<1000) {
                plotdata[i+2*hydro->get_c_Depth_Size()]=c;
                plotdata[i+3*hydro->get_c_Depth_Size()]=hydro->get_c_Depth(i);
            } else {
                plotdata[i+2*hydro->get_c_Depth_Size()]=NAN;
                plotdata[i+3*hydro->get_c_Depth_Size()]=hydro->get_c_Depth(i);;
            }
        }
    }
    plot->setData(plotdata,4,hydro->get_c_Depth_Size());
    plot->setTitel("","Delta-C-13 (DIC) [o/oo]","Depth [m]");
    plot->setMultiplicator(1,-1);
    plot->setTextSize(resources->get_Pointsize(),1,0);
    plot->setSetColor(plotcol,1);
    plot->setSize(ui->graphicsView_8->width(),ui->graphicsView_8->height());
    plot->setSymbol(3);
    plot->addMarker(hydro->getCarbon(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),inv->get_Water_Depth(inv->get_currentCore()),1);
    plot->autoSize();
    plot->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_C.txt",0);
    ui->graphicsView_8->setScene(plot);
}

bool Hydro::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->graphicsView_5||obj==ui->graphicsView_8){
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

void Hydro::createMap(int first){
    if (ui->comboBox->currentText()=="Temperature (WOA, Locarnini et al. 2009)") createMapT(first);
    if (ui->comboBox->currentText()=="Salinity (WOA, Antonov et al. 2009)") createMapS(first);
    if (ui->comboBox->currentText()=="Delta-O-18 (seawater, Legrande & Schmidt, 2006)") createMap18o(first);
    if (ui->comboBox->currentText()=="Delta-C-13 (DIC, Eide et al., 2017)") createMapC(first);
    if (ui->comboBox->currentText()=="Delta-O-18 (carbonate, Shackleton, 1974)") createMapDC(first);
    if (ui->comboBox->currentText()=="Delta-O-18 (carbonate, Marchitto et al., 2014)") createMapDCM(first);
    update();
    ui->graphicsView->repaint();
}

void Hydro::createMapT(int first){
    // locations of database
    if (first){
        delete[] mapdata;
        delete[] mapcom;
        delete[] maplab;
        delete[] mapcol;

        mapdata=new float[inv->get_Entries()*2];
        mapcom=new QString[inv->get_Entries()*2];
        maplab=new QString[inv->get_Entries()*2];
        mapcol=new QColor[inv->get_Entries()*2];


    for (int i=0;i<inv->get_Entries();i++){
        mapdata[i+0*inv->get_Entries()]=inv->get_Longitude(i);
        mapdata[i+1*inv->get_Entries()]=inv->get_Latitude(i);
        mapcom[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        maplab[i+0*inv->get_Entries()]=inv->get_Core(i);
        mapcol[i+0*inv->get_Entries()]=Qt::black;
        mapcol[i+1*inv->get_Entries()]=QColor(0,0,255,128);

        if (i==inv->get_currentCore()) mapcol[i+1*inv->get_Entries()]=Qt::green;
    }



    map->setData(mapdata,2,inv->get_Entries());
    map->setMultiplicator(1,1);
    map->setColor(mapcol,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(mapcom,2);

    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(worldmap,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    if (first) map->setView(-180,180,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_MapT.txt");
    map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);
    }
    //if (ui->checkBox_3->isChecked()==true)map->setLabel(maplab,1); else map->setLabel(maplab,0);
    if (ui->checkBox_2->isChecked()==true)map->setUse(mapuse,2); else map->setUse(mapuse,0);

    // create new Mask data
    delete mask;
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    int index=0;
    for (index=0;index<hydro->get_t_Depth_Size();index++) if(hydro->get_t_Depth(index)>ui->horizontalSlider->value()) break;
    index--;
    map_min=hydro->get_t_dmin(index);
    map_max=hydro->get_t_dmax(index);
    for (int i=-180;i<180;i=i+1){
        for (int j=-90;j<90;j=j+1){
            float v=hydro->getTemperature(i,j,ui->horizontalSlider->value());
            if (v>-100 && v<100) {
               mask->setPixel(i+180,179-(j+90),colorScheme->getColor(v,map_min,map_max,map->getColorScheme(0)).rgba());
            } else {
               mask->setPixel(i+180,179-(j+90),QColor(0,0,0,0).rgba());
            }
        }
    }

    map->setBackground2(mask,-180,180,-90,90,3);
    map->setColorLegend(1,"Temperature [°C]",map_min,map_max);
    ui->graphicsView->setScene(map);
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());



}
void Hydro::createMapS(int first){
    // locations of database
    if (first){
        delete[] mapdata;
        delete[] mapcom;
        delete[] maplab;
        delete[] mapcol;
        plotdata=new float[inv->get_Entries()*2];
        mapcom=new QString[inv->get_Entries()*2];
        maplab=new QString[inv->get_Entries()*2];
        mapcol=new QColor[inv->get_Entries()*2];
    for (int i=0;i<inv->get_Entries();i++){
        mapdata[i+0*inv->get_Entries()]=inv->get_Longitude(i);
        mapdata[i+1*inv->get_Entries()]=inv->get_Latitude(i);
        mapcom[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        maplab[i+0*inv->get_Entries()]=inv->get_Core(i);
        mapcol[i+0*inv->get_Entries()]=Qt::black;
        mapcol[i+1*inv->get_Entries()]=QColor(0,0,255,128);
        if (i==inv->get_currentCore()) mapcol[i+1*inv->get_Entries()]=Qt::green;
    }



    map->setData(mapdata,2,inv->get_Entries());
    map->setMultiplicator(1,1);
    map->setColor(mapcol,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(mapcom,2);

    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(worldmap,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    if (first) map->setView(-180,180,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_MapS.txt");
    map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);
    }
    //if (ui->checkBox_3->isChecked()==true)map->setLabel(maplab,1); else map->setLabel(maplab,0);
    if (ui->checkBox_2->isChecked()==true)map->setUse(mapuse,2); else map->setUse(mapuse,0);
    // create new Mask data
    delete mask;
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    int index=0;
    for (index=0;index<hydro->get_s_Depth_Size();index++) if(hydro->get_s_Depth(index)>ui->horizontalSlider->value()) break;
    index--;
    map_min=hydro->get_s_dmin(index);
    map_max=hydro->get_s_dmax(index);
    for (int i=-180;i<180;i=i+1){
        for (int j=-90;j<90;j=j+1){
            float v=hydro->getSalinity(i,j,ui->horizontalSlider->value());
            if (v>-100 && v<100) {
               mask->setPixel(i+180,179-(j+90),colorScheme->getColor(v,map_min,map_max,map->getColorScheme(0)).rgba());
            } else {
               mask->setPixel(i+180,179-(j+90),QColor(0,0,0,0).rgba());
            }
        }
    }

    map->setBackground2(mask,-180,180,-90,90,3);
    map->setColorLegend(1,"Salinity [psu]",map_min,map_max);
    ui->graphicsView->setScene(map);
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());

}


void Hydro::createMap18o(int first){
    // locations of database
    if (first){
        delete[] mapdata;
        delete[] mapcom;
        delete[] maplab;
        delete[] mapcol;
        mapdata=new float[inv->get_Entries()*2];
        mapcom=new QString[inv->get_Entries()*2];
        maplab=new QString[inv->get_Entries()*2];
        mapcol=new QColor[inv->get_Entries()*2];
    for (int i=0;i<inv->get_Entries();i++){
        mapdata[i+0*inv->get_Entries()]=inv->get_Longitude(i);
        mapdata[i+1*inv->get_Entries()]=inv->get_Latitude(i);
        mapcom[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        maplab[i+0*inv->get_Entries()]=inv->get_Core(i);
        mapcol[i+0*inv->get_Entries()]=Qt::black;
        mapcol[i+1*inv->get_Entries()]=QColor(0,0,255,128);
        if (i==inv->get_currentCore()) mapcol[i+1*inv->get_Entries()]=Qt::green;
    }



    map->setData(mapdata,2,inv->get_Entries());
    map->setMultiplicator(1,1);
    map->setColor(mapcol,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(mapcom,2);

    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(worldmap,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    if (first) map->setView(-180,180,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_Map18o.txt");
    map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);
    }
    //if (ui->checkBox_3->isChecked()==true)map->setLabel(maplab,1); else map->setLabel(maplab,0);
    if (ui->checkBox_2->isChecked()==true)map->setUse(mapuse,2); else map->setUse(mapuse,0);
    // create new Mask data
    delete mask;
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    int index=0;
    for (index=0;index<hydro->get_d18o_Depth_Size();index++) if(hydro->get_d18o_Depth(index)>ui->horizontalSlider->value()) break;
    index--;
    map_min=hydro->get_d18o_dmin(index);
    map_max=hydro->get_d18o_dmax(index);
    for (int i=-180;i<180;i=i+1){
        for (int j=-90;j<90;j=j+1){
            float v=hydro->getd18o(i,j,ui->horizontalSlider->value());
            if (v>-100 && v<100) {
               mask->setPixel(i+180,179-(j+90),colorScheme->getColor(v,map_min,map_max,map->getColorScheme(0)).rgba());
            } else {
               mask->setPixel(i+180,179-(j+90),QColor(0,0,0,0).rgba());
            }
        }
    }

    map->setBackground2(mask,-180,180,-90,90,3);
    map->setColorLegend(1,"d18O Seawater [o/oo SMOW]",map_min,map_max);
    ui->graphicsView->setScene(map);
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());

}
void Hydro::createMapDC(int first){
    // locations of database
    if (first){
        delete[] mapdata;
        delete[] mapcom;
        delete[] maplab;
        delete[] mapcol;
        mapdata=new float[inv->get_Entries()*2];
        mapcom=new QString[inv->get_Entries()*2];
        maplab=new QString[inv->get_Entries()*2];
        mapcol=new QColor[inv->get_Entries()*2];
    for (int i=0;i<inv->get_Entries();i++){
        mapdata[i+0*inv->get_Entries()]=inv->get_Longitude(i);
        mapdata[i+1*inv->get_Entries()]=inv->get_Latitude(i);
        mapcom[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        maplab[i+0*inv->get_Entries()]=inv->get_Core(i);
        mapcol[i+0*inv->get_Entries()]=Qt::black;
        mapcol[i+1*inv->get_Entries()]=QColor(0,0,255,128);
        if (i==inv->get_currentCore()) mapcol[i+1*inv->get_Entries()]=Qt::green;
    }



    map->setData(mapdata,2,inv->get_Entries());
    map->setMultiplicator(1,1);
    map->setColor(mapcol,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(mapcom,2);

    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(worldmap,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    if (first) map->setView(-180,180,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_MapDC.txt");
    map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);
    }
    //if (ui->checkBox_3->isChecked()==true)map->setLabel(maplab,1); else map->setLabel(maplab,0);
    if (ui->checkBox_2->isChecked()==true)map->setUse(mapuse,2); else map->setUse(mapuse,0);
    // create new Mask data
    delete mask;
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    int index=0;
    for (index=0;index<hydro->get_t_Depth_Size();index++) if(hydro->get_t_Depth(index)>ui->horizontalSlider->value()) break;
    index--;
    map_min=hydro->get_dc_dmin(index);
    map_max=hydro->get_dc_dmax(index);
    for (int i=-180;i<180;i=i+1){
        for (int j=-90;j<90;j=j+1){
            float v=hydro->getdc(i,j,ui->horizontalSlider->value());
            if (v>-100 && v<100) {
               mask->setPixel(i+180,179-(j+90),colorScheme->getColor(v,map_min,map_max,map->getColorScheme(0)).rgba());
            } else {
               mask->setPixel(i+180,179-(j+90),QColor(0,0,0,0).rgba());
            }
        }
    }

    map->setBackground2(mask,-180,180,-90,90,3);
    map->setColorLegend(1,"Delta-O-18 (carbonate) [o/oo]",map_min,map_max);
    ui->graphicsView->setScene(map);
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());

}

void Hydro::createMapDCM(int first){
    // locations of database
    if (first){
        delete[] mapdata;
        delete[] mapcom;
        delete[] maplab;
        delete[] mapcol;
        mapdata=new float[inv->get_Entries()*2];
        mapcom=new QString[inv->get_Entries()*2];
        maplab=new QString[inv->get_Entries()*2];
        mapcol=new QColor[inv->get_Entries()*2];
    for (int i=0;i<inv->get_Entries();i++){
        mapdata[i+0*inv->get_Entries()]=inv->get_Longitude(i);
        mapdata[i+1*inv->get_Entries()]=inv->get_Latitude(i);
        mapcom[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        maplab[i+0*inv->get_Entries()]=inv->get_Core(i);
        mapcol[i+0*inv->get_Entries()]=Qt::black;
        mapcol[i+1*inv->get_Entries()]=QColor(0,0,255,128);
        if (i==inv->get_currentCore()) mapcol[i+1*inv->get_Entries()]=Qt::green;
    }



    map->setData(mapdata,2,inv->get_Entries());
    map->setMultiplicator(1,1);
    map->setColor(mapcol,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(mapcom,2);

    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(worldmap,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    if (first) map->setView(-180,180,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_MapDCM.txt");
    map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);
    }
    //if (ui->checkBox_3->isChecked()==true)map->setLabel(maplab,1); else map->setLabel(maplab,0);
    if (ui->checkBox_2->isChecked()==true)map->setUse(mapuse,2); else map->setUse(mapuse,0);
    // create new Mask data
    delete mask;
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    int index=0;
    for (index=0;index<hydro->get_t_Depth_Size();index++) if(hydro->get_t_Depth(index)>ui->horizontalSlider->value()) break;
    index--;
    map_min=hydro->get_dcm_dmin(index);
    map_max=hydro->get_dcm_dmax(index);
    for (int i=-180;i<180;i=i+1){
        for (int j=-90;j<90;j=j+1){
            float v=hydro->getdcm(i,j,ui->horizontalSlider->value());
            if (v>-100 && v<100) {
               mask->setPixel(i+180,179-(j+90),colorScheme->getColor(v,map_min,map_max,map->getColorScheme(0)).rgba());
            } else {
               mask->setPixel(i+180,179-(j+90),QColor(0,0,0,0).rgba());
            }
        }
    }

    map->setBackground2(mask,-180,180,-90,90,3);
    map->setColorLegend(1,"Delta-O-18 (carbonate) [o/oo]",map_min,map_max);
    ui->graphicsView->setScene(map);
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());

}

void Hydro::createMapC(int first){
    // locations of database
    if (first){
        delete[] mapdata;
        delete[] mapcom;
        delete[] maplab;
        delete[] mapcol;
        mapdata=new float[inv->get_Entries()*2];
        mapcom=new QString[inv->get_Entries()*2];
        maplab=new QString[inv->get_Entries()*2];
        mapcol=new QColor[inv->get_Entries()*2];
    for (int i=0;i<inv->get_Entries();i++){
        mapdata[i+0*inv->get_Entries()]=inv->get_Longitude(i);
        mapdata[i+1*inv->get_Entries()]=inv->get_Latitude(i);
        mapcom[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
        maplab[i+0*inv->get_Entries()]=inv->get_Core(i);
        mapcol[i+0*inv->get_Entries()]=Qt::black;
        mapcol[i+1*inv->get_Entries()]=QColor(0,0,255,128);
        if (i==inv->get_currentCore()) mapcol[i+1*inv->get_Entries()]=Qt::green;
    }



    map->setData(mapdata,2,inv->get_Entries());
    map->setMultiplicator(1,1);
    map->setColor(mapcol,true);
    map->setTitel("","Longitude","Latitude");
    map->setAxisType(3,3);
    map->setTextSize(resources->get_Pointsize(),1,0);
    map->setComment(mapcom,2);

    map->setLockAspect(1);

    map->setRepeat(2,-180,180,0,-90,90);
    map->setLimit(-900,900,-90,90);
    if (first) map->setBackground(worldmap,-180,180,-90,90,1);
    if (first) map->setComHide(0,1);
    map->setFolding(1,0);
    if (first) map->setView(-180,180,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_MapS.txt");
    map->setView(inv->get_Longitude(inv->get_currentCore())-50,inv->get_Longitude(inv->get_currentCore())+50,inv->get_Latitude(inv->get_currentCore())-50,inv->get_Latitude(inv->get_currentCore())+50);
    }
    //if (ui->checkBox_3->isChecked()==true)map->setLabel(maplab,1); else map->setLabel(maplab,0);
    if (ui->checkBox_2->isChecked()==true)map->setUse(mapuse,2); else map->setUse(mapuse,0);
    // create new Mask data
    delete mask;
    mask=new QImage(360,180,QImage::Format_ARGB32_Premultiplied);
    int index=0;
    for (index=0;index<hydro->get_c_Depth_Size();index++) if(hydro->get_c_Depth(index)>ui->horizontalSlider->value()) break;
    index--;
    map_min=hydro->get_c_dmin(index);
    map_max=hydro->get_c_dmax(index);
    for (int i=-180;i<180;i=i+1){
        for (int j=-90;j<90;j=j+1){
            float v=hydro->getCarbon(i,j,ui->horizontalSlider->value());
            if (v>-100 && v<100) {
               mask->setPixel(i+180,179-(j+90),colorScheme->getColor(v,map_min,map_max,map->getColorScheme(0)).rgba());
            } else {
               mask->setPixel(i+180,179-(j+90),QColor(0,0,0,0).rgba());
            }
        }
    }

    map->setBackground2(mask,-180,180,-90,90,3);
    map->setColorLegend(1,"Delta-C-13 (DIC) [o/oo]",map_min,map_max);
    ui->graphicsView->setScene(map);
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());

}


void Hydro::createTSPlot(){

    delete[] tsdata;
    tsdata=new float[hydro->get_t_Depth_Size()*44];
    for(int i=0;i<hydro->get_t_Depth_Size()*44;i++) tsdata[i]=NAN;
    delete[] tscol;
    tscol=new QColor[44];
    tscol[0]=Qt::blue;
    tscol[1]=Qt::black;
    tscol[2]=Qt::gray;
    tscol[3]=Qt::black;
    for(int i=4;i<44;i++) tscol[i]=Qt::black;
    delete[] size;
    size=new int[44];
    for (int i=0;i<4;i++) size[i]=3;
    for (int i=4;i<44;i++) size[i]=0;
    delete[] autosize;
    autosize=new int[44];
    for (int i=0;i<4;i++) autosize[i]=1;
    for (int i=4;i<44;i++) autosize[i]=0;

    delete[] tscom;
    tscom=new QString[hydro->get_t_Depth_Size()*44];
    for (int i=0;i<hydro->get_t_Depth_Size()*44;i++) tscom[i]="";

    for (int i=0;i<hydro->get_t_Depth_Size();i++){

        float t=hydro->getPTemperature(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));
        float s=hydro->getSalinity(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_s_Depth(i));

        if (t>-1000 && t<1000 && s>-1000 && s<1000) {
            tsdata[i+0*hydro->get_t_Depth_Size()]=s;
            tsdata[i+1*hydro->get_t_Depth_Size()]=t;
        } else {
            tsdata[i+0*hydro->get_t_Depth_Size()]=NAN;
            tsdata[i+1*hydro->get_t_Depth_Size()]=NAN;
        }
        if (ui->checkBox->isChecked() && !isnan(comp_latt) && !isnan(comp_long)){


            t=hydro->getPTemperature(comp_long,comp_latt,hydro->get_t_Depth(i));
            s=hydro->getSalinity(comp_long,comp_latt,hydro->get_s_Depth(i));

            if (t>-1000 && t<1000) {
                tsdata[i+2*hydro->get_t_Depth_Size()]=s;
                tsdata[i+3*hydro->get_t_Depth_Size()]=t;
            } else {
                tsdata[i+2*hydro->get_t_Depth_Size()]=NAN;
                tsdata[i+3*hydro->get_t_Depth_Size()]=NAN;
            }
        }
        // generate isovalues
        for (int j=0;j<20;j++){
            float p=1015+j;
            float t=i*1.5-4;//hydro->getPTemperature(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),hydro->get_t_Depth(i));
            
            float s=hydro->getp(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore()),p,t);
            if (t>-1000 && t<1000) {
                tsdata[i+(4+j*2)*hydro->get_t_Depth_Size()]=s;
                tsdata[i+(4+j*2+1)*hydro->get_t_Depth_Size()]=t;
            } else {
                tsdata[i+(4+j*2)*hydro->get_t_Depth_Size()]=NAN;
                tsdata[i+(4+j*2+1)*hydro->get_t_Depth_Size()]=NAN;
            }
            if (i==i/3*3) tscom[i+(4+j*2)*hydro->get_t_Depth_Size()]=QString::number(p);
            if (i==i/3*3) tscom[i+(4+j*2+1)*hydro->get_t_Depth_Size()]=QString::number(p);
        }

    }




    plotTS->setData(tsdata,44,hydro->get_t_Depth_Size());
    plotTS->setTitel("","Salinity in [psu]","Temperature in [°C]");
    plotTS->setMultiplicator(1,-1);
    plotTS->setTextSize(resources->get_Pointsize(),1,0);
    plotTS->setSize(ui->graphicsView_5->width(),ui->graphicsView_5->height());
    plotTS->setSetSymbolsize(size,1);
    plotTS->setLabel(tscom,2);

    plotTS->setSetColor(tscol,1);
    plotTS->addMarker(hydro->getSalinity(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),
                      hydro->getPTemperature(inv->get_Longitude(inv->get_currentCore()),inv->get_Latitude(inv->get_currentCore()),inv->get_Water_Depth(inv->get_currentCore())),1);
    plotTS->setAutosize(autosize,1);
    plotTS->autoSize();
    plotTS->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Hydro_TS.txt",0);
    ui->graphicsView_5->setScene(plotTS);
ui->graphicsView_5->repaint();

}

void Hydro::updateGraph(){

    createPlot();
    createMap(1);
    createTSPlot();
}
