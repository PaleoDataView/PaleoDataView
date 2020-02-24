#include "targetselect.h"
#include "ui_targetselect.h"

TargetSelect::TargetSelect(QDialog *parent,Inventory *i,QString pr,QImage *wmap) :
    diag(parent),inv2(i),proxy(pr),worldmap(wmap),
    ui(new Ui::TargetSelect)
{
    ui->setupUi(this);
    this->setWindowTitle("Please select a target");
    resources=new Resources();
    inv=new Inventory(proxy);


    // Init Plot Variables for TargetPlot
    data1.clear();
    data1.append(new float[0]);
    com1.clear();
    com1.append(new QString[0]);
    use1.clear();
    use1.append(new bool[0]);
    col1.clear();
    col1.append(new QColor[0]);
    style=new Qt::PenStyle[0];
    stype=new int[0];
    pl1=new Graph(this,data1[0],0,0);
    pl1->setComHide(0,1);

    // Init Plot Variables for AgePlot
    data2.clear();
    data2.append(new float[0]);
    com2.clear();
    com2.append(new QString[0]);
    use2.clear();
    use2.append(new bool[0]);
    col2.clear();
    col2.append(new QColor[0]);
    style2=new Qt::PenStyle[0];
    symbolsize=new int[1];
    pl2=new Graph(this,data2[0],0,0);
    pl2->setComHide(0,1);

    // Init Map Variables
    data=new float[0];
    com=new QString[0];
    lab=new QString[0];
    col=new QColor[0];

    map=new Graph(this,data,0,0);

    // find default values for combomenus
    plotdata_1=-1;

    for (int i=0;i<inv->access_data_length();i++) if (inv->access_data_type(i).contains("plot1")) {
        plotdata_1=i;
        break;
    }


    qDebug()<<"Combobox default"+QString::number(plotdata_1);

    // set plot comboBoxes
     {
     const QSignalBlocker blocker(ui->comboBox);
     ui->comboBox->clear();
    for (int i=0;i<inv->access_data_length();i++){
        QString str=inv->access_data_name(i);
        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
        ui->comboBox->addItem(str);
        if (inv->access_data_name(i)=="Sample Thickness" || inv->access_data_name(i)=="Age"|| inv->access_data_name(i)=="Use Flag"||inv->access_data_name(i).contains("Error")||inv->access_data_name(i).contains("Comment")||inv->access_data_name(i)=="Depth"){
            // Deactivate/Hide the entry
            QListView* view = qobject_cast<QListView *>(ui->comboBox->view());
            Q_ASSERT(view != nullptr);
            view->setRowHidden(i, true);
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->comboBox->model());
            Q_ASSERT(model != nullptr);
            QStandardItem* item = model->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            if (ui->comboBox->currentIndex()==i) ui->comboBox->setCurrentIndex(-1);
        } else {
            if (ui->comboBox->currentIndex()==-1) ui->comboBox->setCurrentIndex(i);
        }

    }

    if (plotdata_1>0) ui->comboBox->setCurrentIndex(plotdata_1);
    }
    createList();
    createMap(1);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(rejected()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateGraph()));
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(targetSelected()));
    connect(this,SIGNAL(targetS(QString)),diag,SLOT(targetS(QString)));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(updateGraph()));
    connect(pl1,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(pl2,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(map,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(map,SIGNAL(selected(int,int,int)),this,SLOT(selected_changed(int,int,int)));
    pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    pl2->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    if (entries>0) {
        {
        const QSignalBlocker blocker(ui->listWidget);
        ui->listWidget->setCurrentRow(0);
        }
        targetSelected();
    }
}

TargetSelect::~TargetSelect()
{
    delete ui;
    delete pl1;
    delete pl2;
    delete map;
}

void TargetSelect::paintEvent(QPaintEvent *)
{


    pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    pl2->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());
}

QString TargetSelect::getFilename(){
    return filename;
}

void TargetSelect::createList(){
    // get Entry from Directory
    QDir dir=resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+proxy+"/Derived Data/Targets";
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.tgt"));
    QFileInfoList list = dir.entryInfoList();

    //Fill List
    ui->listWidget->clear();
    for (int i=0;i<list.size();i++) ui->listWidget->addItem(list.at(i).fileName());


    // create Inventory
    entries=list.size();

    // create Stringlist
    for (int i=0;i<entries;i++){
        target_inv.append(new QStringList());
    }
    for (int q=0;q<entries;q++){
        // read meta data
        QString QFilename=resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+proxy+"/Derived Data/Targets/"+list.at(q).fileName();
        error_text.clear();
        QString action="Reading Proxy";
        // get file name
        char* file;
        string fname=QFilename.toStdString();
        file=new char[fname.size()+1];
        strcpy(file,fname.c_str());
        qDebug() << QFilename;

        // Initialize ids and meta
        int ncid;

        int retval;// for Errors

        // Open the file
        if ((retval = nc_open(file, NC_NOWRITE, &ncid)))
           NetCDF_Error(file,action,retval);

        target_inv[q]->clear();

        for (int i=0;i<inv->access_meta_length();i++){
            if (inv->access_meta_type(i).contains("string") || inv->access_meta_type(i).contains("combo") ) {
               if (!inv->access_meta_netcdf(i).contains("ignore")){
                    QString att_name=inv->access_meta_netcdf(i).split(";").at(0);
                    size_t attlen;
                     QString err=QFilename;
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
                    target_inv[q]->append(str);
                    qDebug() << str;
               } else {
                   target_inv[q]->append("");
               }
            } else {
                if (!inv->access_meta_netcdf(i).contains("ignore")){
                    QString att_name=inv->access_meta_netcdf(i).split(";").at(0);
                    double att=0;
                    QString err=QFilename;
                    if ((retval = nc_get_att_double(ncid,NC_GLOBAL,att_name.toLatin1(),&att)))
                       NetCDF_Error(err+" : "+att_name,action,retval);
                    target_inv[q]->append(QString::number(att));
                    qDebug() << QString::number(att);
                } else {
                    target_inv[q]->append("");
                }
            }
        }
        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           NetCDF_Error(QFilename,action,retval);

        delete[] file;
        if (error_text!=""&& show_error==1) NetCDF_Error_Message();
        qDebug()<<"Target succesfully read";


    }
    // get data columns
    corename=0;
    type=1;
    longitude=2;
    latitude=3;
    for (int i=0;i<inv->access_meta_length();i++){
        if (inv->access_meta_name(i).contains("Core")) corename=i;
        if (inv->access_meta_name(i).contains("Species")||inv->access_meta_name(i).contains("Parameter")||inv->access_meta_name(i).contains("Component")) type=i;
        if (inv->access_meta_name(i).contains("Longitude")) longitude=i;
        if (inv->access_meta_name(i).contains("Latitude")) latitude=i;
    }
    qDebug()<<"Corename:"+QString::number(corename)+" Type:"+QString::number(type)+" Longitude:"+QString::number(longitude)+" Latitude:"+QString::number(latitude);







}

void TargetSelect::targetSelected(){
    filename=resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+proxy+"/Derived Data/Targets/"+ui->listWidget->currentItem()->text();
    readTarget();
    createMeta();
    createTargetPlot();
    createAgeModelPlot();
    map->setSelected_X(0);
    int pos=ui->listWidget->currentRow();
    if (pos>-1){
        map->setSelected_Y(pos);
        createMap(0);
    }


}

void TargetSelect::accepted(){
    if (entries>0) {
        filename=resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+proxy+"/Derived Data/Targets/"+ui->listWidget->currentItem()->text();
        emit (targetS(filename));
    } else {
        filename="";
    }
}

void TargetSelect::rejected(){
    filename="";
}

void TargetSelect::createMeta(){
    QString str="";

    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(resources->get_Textsize());

    for (int i=0;i<inv->access_meta_length();i++){
        QString sub="";
        sub.append("<b>"+inv->access_meta_name(i));
        if (inv->access_meta_unit(i)!="") sub.append(" ["+inv->access_meta_unit(i)+"]");
        for (int j=sub.length();j<20;j++) sub.append(".");
        sub.append(": </b>"+target_meta.at(i));
        if (i+1<inv->access_meta_length()) sub.append("<br>");
        str.append(sub);
    }

    ui->textBrowser->clear();
    ui->textBrowser->setFont(font);
    ui->textBrowser->append(str);
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.setPosition(0);
    ui->textBrowser->setTextCursor(cursor);

}



void TargetSelect::readTarget(){
    QString QFilename=filename;
    error_text.clear();
    QString action="Reading Proxy";



    // get file name


    char* file;
    string fname=QFilename.toStdString();
    file=new char[fname.size()+1];
    strcpy(file,fname.c_str());
    qDebug() << QFilename;

    // Initialize ids and meta
    int ncid;

    int retval;// for Errors

    // Open the file
    if ((retval = nc_open(file, NC_NOWRITE, &ncid)))
       NetCDF_Error(file,action,retval);

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

    delete[] file;
    if (error_text!=""&& show_error==1) NetCDF_Error_Message();
    qDebug()<<"Target succesfully read";
}

void TargetSelect::NetCDF_Error(QString filename,QString action,int value){
    error_text.append(filename+" : "+action+" : "+nc_strerror(value)+" : "+QString::number(value)+"\n");

}

void TargetSelect::NetCDF_Error_Message(){
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

void TargetSelect::createTargetPlot(){
    qDebug()<<"Creating Target Plot";
    delete[] style;
    style=new Qt::PenStyle[2];
    style[0]=Qt::SolidLine;
    style[1]=Qt::SolidLine;


    delete[] stype;
    stype=new int[2];
    stype[0]=0;
    stype[1]=0;


    // Estimate number of plots

    int plot1=0;
    /*
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("plot1")){
            plot1++;
        }
    }*/
    plot1=1;// We now support only single plots
    QString titel1;
    pl1->setNumber(plot1);



    // delete old graph data
    for (int i=0;i<data1.length();i++) delete[] data1[i];
    data1.clear();
    for (int i=0;i<use1.length();i++) delete[] use1[i];
    use1.clear();
    for (int i=0;i<com1.length();i++) delete[] com1[i];
    com1.clear();

    for (int i=0;i<col1.length();i++) delete[] col1[i];
    col1.clear();


    // create plots
    for (int k=0;k<plot1;k++){
        qDebug()<<"search for Plot information";
        // Search for plot information
        int c=-1;
        int i=0;
        /*for (i=0;i<inv->access_data_length();i++){
            if (inv->access_data_type(i).contains("plot1")) c++;
            if (c==k) break;
        }*/
        int data=plotdata_1;
        int flag=-1;
        int comment=-1;
        float multx=1;
        float multy=1;
        int metaflag=-1;
        QString str=inv->access_data_type(data);
        QStringList list=str.split(";");
        for(int i=0;i<list.length();i++) {
            if (list[i].contains("comment")) {
                QStringList list2=list[i].split("=");
                comment=list2[1].toInt();
            }
            if (list[i].contains("dFlag")) {
                QStringList list2=list[i].split("=");
                flag=list2[1].toInt();
            }
            if (list[i].contains("multx")) {
                QStringList list2=list[i].split("=");
                multx=list2[1].toFloat();
            }
            if (list[i].contains("multy")) {
                QStringList list2=list[i].split("=");
                multy=list2[1].toFloat();
            }
            if (list[i].contains("mFlag")) {
                QStringList list2=list[i].split("=");
                metaflag=list2[1].toInt();
            }

        }



        //get age model



        // get length
        int length=target_length;




qDebug()<<"prepare memory";
        // fill first plot with data
        data1.append(new float[length*2]);
        for (int i=0;i<length*2;i++) data1[k][i]=NAN;

        use1.append(new bool[length*2]);
        for (int i=0;i<length*2;i++) use1[k][i]=0;

        com1.append(new QString[length*2]);
        for (int i=0;i<length*2;i++) com1[k][i]="";

        col1.append(new QColor[length*2]);
        for (int i=0;i<length*2;i++) col1[k][i]=Qt::blue;

        float low=INFINITY;
        float high=-INFINITY;
        int dataOK=0;
        qDebug()<<"put data";
        for (int i=0;i<target_length;i++){
            data1[k][i+0*length]=target_age[0]->at(i).toFloat();
            data1[k][i+1*length]=target_data[data]->at(i).toFloat();
            if (data1[k][i+0*length]<100000 && data1[k][i+0*length]>-100000 && data1[k][i+1*length]<100000 && data1[k][i+1*length]>-100000) dataOK=1;
            if (low>target_data[data]->at(i).toFloat()) low=target_data[data]->at(i).toFloat();
            if (high<target_data[data]->at(i).toFloat()) high=target_data[data]->at(i).toFloat();
            if (flag!=-1){
                use1[k][i+0*length]=target_data[flag]->at(i).toInt();
                use1[k][i+1*length]=target_data[flag]->at(i).toInt();
            }
            if (comment!=-1){
                if (target_data[comment]->at(i).simplified()!="NaN") com1[k][i+1*length]=target_data[comment]->at(i);
            }
            col1[k][i+0*length]=Qt::black;
        }





qDebug()<<"set plot preferences";
        pl1->setData(data1[k],2,length,k);
        pl1->setUse(use1[k],1,k);
        QString str1="";
        // search for optional
        int optional=0;
        for (int i=0;i<inv->access_meta_length();i++) if (inv->access_meta_name(i).contains("Optional Values")) optional=i;
        if (target_meta[optional].contains("%"+inv->access_data_name(data)+"=")) {
            QStringList list=target_meta[optional].split(";");
            for(int i=0;i<list.size();i++) if(list[i].contains("%"+inv->access_data_name(data)+"=")) if (list[i].split("=").size()>0) str1=list[i].split("=").at(1);
        } else {
            str1=inv->access_data_name(data);
        }
        if (inv->access_data_unit(data)!="") str1.append(" ["+inv->access_data_unit(data)+"]");
        pl1->setTitel("","Age in [ka]",str1,k);
        pl1->setMultiplicator(multx,multy,k);
        pl1->setTextSize(resources->get_Pointsize(),1,12,k);
        pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());


        pl1->setSetLineStyle(style,1,k);
        pl1->setColor(col1[k],1,k);
        pl1->setSetSymboltype(stype,1,k);
        pl1->setComment(com1[k],2,k);
        pl1->setComHide(k,1);

        pl1->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Target_plot1_"+inv->access_proxy_abbreviation()+"-"+QString::number(ui->comboBox->currentIndex())+".txt");
        pl1->autoSize(k);





    }

    //ui->groupBox_10->setTitle(titel1);
    ui->graphicsView_2->setScene(pl1);
    ui->graphicsView_2->repaint();
    qDebug()<<"Target Plot Finished";
}

void TargetSelect::createAgeModelPlot(){
    qDebug()<<"Creating Age Plot";
    delete[] style2;
    style2=new Qt::PenStyle[6];
    style2[0]=Qt::SolidLine;
    style2[1]=Qt::SolidLine;
    style2[2]=Qt::SolidLine;
    style2[3]=Qt::SolidLine;
    style2[4]=Qt::SolidLine;
    style2[5]=Qt::SolidLine;

    delete[] symbolsize;
    symbolsize=new int[6];
    symbolsize[0]=3;
    symbolsize[1]=3;
    symbolsize[2]=0;
    symbolsize[3]=0;
    symbolsize[4]=0;
    symbolsize[5]=0;


    // Estimate number of plots

    int plot1=0;
    /*
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("plot1")){
            plot1++;
        }
    }*/
    plot1=1;// We now support only single plots

    pl2->setNumber(plot1);



    // delete old graph data
    for (int i=0;i<data2.length();i++) delete[] data2[i];
    data2.clear();
    for (int i=0;i<use2.length();i++) delete[] use2[i];
    use2.clear();
    for (int i=0;i<com2.length();i++) delete[] com2[i];
    com2.clear();
    for (int i=0;i<col2.length();i++) delete[] col2[i];
    col2.clear();


    // create plots
    for (int k=0;k<plot1;k++){
        qDebug()<<"search for Plot information";
        // Search for plot information

        int data=plotdata_1;
        int flag=-1;
        int comment=-1;
        float multx=1;
        float multy=1;
        int metaflag=-1;
        QString str=inv->access_data_type(data);
        QStringList list=str.split(";");
        for(int i=0;i<list.length();i++) {
            if (list[i].contains("comment")) {
                QStringList list2=list[i].split("=");
                comment=list2[1].toInt();
            }
            if (list[i].contains("dFlag")) {
                QStringList list2=list[i].split("=");
                flag=list2[1].toInt();
            }
            if (list[i].contains("multx")) {
                QStringList list2=list[i].split("=");
                multx=list2[1].toFloat();
            }
            if (list[i].contains("multy")) {
                QStringList list2=list[i].split("=");
                multy=list2[1].toFloat();
            }
            if (list[i].contains("mFlag")) {
                QStringList list2=list[i].split("=");
                metaflag=list2[1].toInt();
            }

        }



        //get age model



        // get length
        int length=target_length;




        qDebug()<<"prepare memory";
        // fill first plot with data
        data2.append(new float[length*6]);
        for (int i=0;i<length*6;i++) data2[k][i]=NAN;

        use2.append(new bool[length*6]);
        for (int i=0;i<length*6;i++) use2[k][i]=0;

        com2.append(new QString[length*6]);
        for (int i=0;i<length*6;i++) com2[k][i]="";

        col2.append(new QColor[length*6]);
        for (int i=0;i<length*6;i++) col2[k][i]=Qt::blue;



        qDebug()<<"put data";
        for (int i=0;i<target_length;i++){
            data2[k][i+0*length]=target_data[0]->at(i).toFloat();
            data2[k][i+1*length]=target_age[0]->at(i).toFloat();
            data2[k][i+2*length]=target_data[0]->at(i).toFloat();
            data2[k][i+3*length]=target_age[2]->at(i).toFloat();
            data2[k][i+4*length]=target_data[0]->at(i).toFloat();
            data2[k][i+5*length]=target_age[3]->at(i).toFloat();

            if (flag!=-1){
                use2[k][i+0*length]=target_data[flag]->at(i).toInt();
                use2[k][i+1*length]=target_data[flag]->at(i).toInt();
                use2[k][i+2*length]=1;
                use2[k][i+3*length]=1;
                use2[k][i+4*length]=1;
                use2[k][i+5*length]=1;

            }
            if (comment!=-1){
                if (target_data[comment]->at(i).simplified()!="NaN") com2[k][i+1*length]=target_data[comment]->at(i);
            }
            col2[k][i+0*length]=Qt::black;
        }





        qDebug()<<"set plot preferences";
        pl2->setData(data2[k],6,length,k);
        qDebug()<<"setData complete";
        pl2->setUse(use2[k],1,k);

        pl2->setSetSymbolsize(symbolsize,1);

        pl2->setTitel("","Depth in [m]","Age in [ka]",k);
        pl2->setMultiplicator(1,1,k);
        pl2->setTextSize(resources->get_Pointsize(),1,12,k);
        pl2->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());


        pl2->setSetLineStyle(style2,1,k);
        pl2->setColor(col2[k],1,k);

        pl2->setComment(com2[k],2,k);
        pl2->setComHide(k,1);

        pl2->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Target_plot2_"+inv->access_proxy_abbreviation()+".txt");
        pl2->autoSize(k);





    }

    //ui->groupBox_10->setTitle(titel1);
    ui->graphicsView_3->setScene(pl2);
    ui->graphicsView_3->repaint();
    qDebug()<<"Age Plot Finished";

}

void TargetSelect::updateGraph(){
    plotdata_1=ui->comboBox->currentIndex();
    if (ui->listWidget->currentRow()>=0) {
        targetSelected();
    }
    pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    pl2->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    map->setSize(ui->graphicsView->width(),ui->graphicsView->height());
}



void TargetSelect::createMap(int first){

    qDebug()<<"creating map";
    delete[] data;
    delete[] com;
    delete[] lab;
    delete[] col;

    data=new float[entries*2];
    com=new QString[entries*2];
    lab=new QString[entries*2];
    col=new QColor[entries*2];

qDebug()<<"putting data";

    for (int i=0;i<entries;i++){
        data[i+0*entries]=target_inv[i]->at(longitude).toFloat();
        data[i+1*entries]=target_inv[i]->at(latitude).toFloat();
        com[i+0*entries]=target_inv[i]->at(corename)+"\n"+target_inv[i]->at(type);

        lab[i+0*entries]=target_inv[i]->at(corename);

        col[i+0*entries]=Qt::black;
        col[i+1*entries]=QColor(0,0,255,128);


    }
    qDebug()<<"defining plot preferences";
    map->setData(data,2,entries);


    map->setMultiplicator(1,1);

    map->setColor(col,true);
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
        qDebug()<<"Putting Map Data";
        map->setBackground(worldmap,-180,180,-90,90,1);
        qDebug()<<"finished";
    }
    map->setFolding(1,0);
    if (first) map->setView((-(180.0/ui->graphicsView->size().height())*ui->graphicsView->size().width())/2.0,((180.0/ui->graphicsView->size().height())*ui->graphicsView->size().width())/2.0,-90,90);
    map->setLineStyle(Qt::NoPen);
    map->addMarker(inv2->get_att_Longitude(),inv2->get_att_Latitude(),2);
    map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Target_Map.txt");

    ui->graphicsView->setScene(map);
     qDebug()<<"map finished";
}

void TargetSelect::selected_changed(int plot,int x,int y){
    qDebug()<<QString::number(plot)+":"+QString::number(x)+" "+QString::number(y);
    {
    const QSignalBlocker blocker(ui->listWidget);
    ui->listWidget->setCurrentRow(y);
    }
    targetSelected();
}
