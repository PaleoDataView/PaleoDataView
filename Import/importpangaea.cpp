#include "importpangaea.h"
#include "ui_importpangaea.h"

ImportPangaea::ImportPangaea(QMainWindow *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv2(inventory),proxy(str),
    ui(new Ui::ImportPangaea)

{
    ui->setupUi(this);

    inv=new Inventory(proxy);
    inv->read();// create an inventory to initialize Variables


    amsdata=new AMSData(inv);

    exist_meta=0;
    exist_iso=0;
    exist_age=0;
    meta_length=21;

    str1.clear();
    for (int i=0;i<inv->access_meta_length();i++) str1.append(inv->access_meta_name(i));

    dlm="\t";
    save_OK=0;
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);

    ui->plainTextEdit->setFont(font);

    // add label and comboboxes to layout
    QVBoxLayout *vbox = new QVBoxLayout;
    lab.clear();
    lab.reserve(inv->access_data_length());
    combobox.clear();
    combobox.reserve(inv->access_data_length());

    for (int i=0;i<inv->access_data_length();i++){
        QHBoxLayout *hbox=new QHBoxLayout;

        QString str=inv->access_data_name(i);
        if (inv->access_data_unit(i)!="") str.append(+" in ["+inv->access_data_unit(i)+"]");
        while(str.length()<25) str.append(".");
        str.append(":");
        lab.append(new QLabel(str));
        lab.at(i)->setFont(font);
        lab.at(i)->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        hbox->addWidget(lab.at(i));

        combobox.append(new QComboBox());
        combobox.at(i)->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
        hbox->addWidget(combobox.at(i));


        hbox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        vbox->addLayout(hbox);

    }

    vbox->addStretch(1);

    ui->groupBox->setLayout(vbox);

    update();

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(parse()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(this,SIGNAL(refresh()),mainW,SLOT(redraw_score()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(Xport()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    ui->tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_2(QPoint)));
}

ImportPangaea::~ImportPangaea()
{
    delete ui;
    delete amsdata;
    delete inv;
}

void ImportPangaea::browse(){
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Import",
                                             tr("Tab SV (*.tab)"));
    //qDebug() << filename;
    ui->lineEdit->setText(filename);
    // read File and put to TextBrowser
    // get file name
    QString QFilename=ui->lineEdit->text();
    //qDebug() << QFilename;
    if (QFilename.right(4)==".tab"){
        QFile file(QFilename);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            //qDebug() <<  file.errorString();
        } else {
            txt.clear();
            QTextStream in(&file);
            while(!in.atEnd()) {
                txt.append(in.readLine().toLocal8Bit()+"\n");
            }

        file.close();
        }
    }

    //qDebug() << "finished reading";

    ui->plainTextEdit->clear();

    ui->plainTextEdit->setPlainText(txt);

    // get Filename
    QStringList filelist=QFilename.split("/");
    QStringList file=filelist.at(filelist.size()-1).split(".");
    QString f=file.at(0);
    ui->lineEdit_2->setText(resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+f+".nc");

    // Put table header to comboboxes
    QString header;
    QStringList lines = txt.split("\n");
    int i=0;
    int eventmode=-1;
    for (i=0;i<lines.size()-1;i++){
        if (lines[i]=="*/") {
            QStringList h=lines[i+1].split("\t");
            for (int j=0;j<inv->access_data_length();j++) {
                combobox.at(j)->clear();
                combobox.at(j)->addItem("Not Selected");
            }



            for (int j=0;j<h.size();j++){
                for (int q=0;q<inv->access_data_length();q++) combobox.at(q)->addItem(h[j]);

                if (h[j].contains("Depth")) {
                    int index=-1;
                    for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii).contains("Depth")) {
                        index=ii;
                        break;
                    }
                    if (index>-1)combobox.at(index)->setCurrentIndex(j+1);
                }
                if (h[j].contains("Age")) {
                    int index=-1;
                    for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii).contains("Age")) {
                        index=ii;
                        break;
                    }
                    if (index>-1)combobox.at(index)->setCurrentIndex(j+1);
                }


                if (h[j].contains("d18O") && proxy=="Foraminiferal Isotopes") {
                    int index=-1;
                    for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii)=="d18O") {
                        index=ii;
                        break;
                    }
                    if (index>-1)combobox.at(index)->setCurrentIndex(j+1);
                }
                if (h[j].contains("d13C")&& proxy=="Foraminiferal Isotopes") {
                    int index=-1;
                    for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii)=="d13C") {
                        index=ii;
                        break;
                    }
                    if (index>-1)combobox.at(index)->setCurrentIndex(j+1);
                }
                if (h[j].contains("Event")) eventmode=j;
                if (h[j].contains("Mg/Ca")&& proxy=="Foraminiferal MgCa") {
                    int index=-1;
                    for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii)=="Mg/Ca") {
                        index=ii;
                        break;
                    }
                    if (index>-1)combobox.at(index)->setCurrentIndex(j+1);
                }
                if (h[j].contains("SST")&& proxy=="Foraminiferal MgCa"){
                    int index=-1;
                    for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii)=="SST") {
                        index=ii;
                        break;
                    }
                    if (index>-1)combobox.at(index)->setCurrentIndex(j+1);
                }
            }

            i++;//skip header
            i++;
        }
        // read events and add to combobox
        if (eventmode>-1){
            QStringList h=lines[i].split("\t");
            if (h.size()>=eventmode) {
                QString event=h[eventmode];
                int found=0;
                for (int k=0;k<ui->comboBox_12->count();k++){
                    if (event==ui->comboBox_12->itemText(k)) found=1;
                }
                if (found==0) ui->comboBox_12->addItem(event);
            }
        }
    }



}

void ImportPangaea::parse(){//qDebug() <<"started parsing";
    txt=ui->plainTextEdit->toPlainText();


    if (txt!=""){
        // delete all old data
        delete inv;
        delete amsdata;
        inv=new Inventory(proxy);
        inv->read();// create an inventory to initialize Variables
        inv->read_Basin();
        inv->addEntry();// create an inventory to initialize Variables
        inv->set_currentCore(0);

        inv->set_Filename(ui->lineEdit_2->text());
        inv->newIsotopeMeta();
        inv->newIsotope(0);
        amsdata=new AMSData(inv);
        // create new Isotope File in Inventory
        //qDebug()<<"newIso";
        amsdata->newAMS(1);//qDebug()<<"newams";
        exist_meta=1;
        exist_iso=1;
        exist_age=0;
        inv->set_att_Comment("");
        //qDebug()<<"init";
        // parse meta Data

        QStringList lines = txt.split("\n"); //Separate into lines
        // Add Meta Data
        QString mode="";
        int number=0;
        for (int i=0;i<lines.size();i++){
            int found=0;
            QStringList head=lines[i].split("\t");

            if (head[0].simplified()=="Citation:") {
               mode="Citation";
               found=1;
               number=0;
               if (head.size()>1) {
                   QString t="";
                   for (int j=1;j<head.size();j++) t.append(head[j]);
                   QStringList t1=t.split("PANGAEA,");
                   if (t1.size()>0){
                       QStringList t2=t1[1].split(",");
                       inv->set_att_Data_Source(t2[0].simplified());
                   }
                   if (head[1].contains("In supplement to:")) inv->set_att_Reference(inv->get_att_Comment().append(t.remove(0,18)+";"));
               }
            }



            if (head[0].simplified()=="Project(s):") {
               mode="Project(s)";
               found=1;
               number=0;
               if (head.size()>1) {
                   QString t="";
                   for (int j=1;j<head.size();j++) t.append(head[j]);
                   //inv->set_att_Comment(inv->get_att_Comment().append(t.simplified()+";"));
               }
            }

            if (head[0].simplified()=="Comment:") {
               mode="Comment";
               found=1;
               number=0;
               if (head.size()>1) {
                   QString t="";
                   for (int j=1;j<head.size();j++) t.append(head[j]);
                   inv->set_att_Comment(inv->get_att_Comment().append(t.simplified()+";"));
               }
            }

            if (head[0].simplified()=="Coverage:") {
               mode="Coverage";
               number=0;
               found=1;
            }

            if (head[0].simplified()=="Event(s):") {
               mode="Event(s)";
               found=1;
               number=0;
               QStringList t=lines[i].split("*");
               if (lines[i].contains(ui->comboBox_12->currentText())||ui->comboBox_12->currentText()=="Not Selected"){
                   for (int j=0;j<t.size();j++){
                       QStringList p=t[j].split(":");
                       if (j==0) {
                           QStringList pp=p[1].split(" ");
                           inv->set_att_Core(pp[0].simplified());
                       }
                       if (p[0].simplified()=="LATITUDE") inv->set_att_Latitude(p[1].toFloat());
                       if (p[0].simplified()=="LONGITUDE") inv->set_att_Longitude(p[1].toFloat());
                       if (p[0].simplified()=="ELEVATION") inv->set_att_Water_Depth(-p[1].replace('m',' ').toFloat());
                       if (p[0].simplified()=="DEVICE") inv->set_att_Device(p[1].simplified());
                   }
               }






            }

            if (head[0].simplified()=="Parameter(s):") {
               mode="Parameter(s)";
               found=1;
               number=0;
            }

            if (head[0].simplified()=="License:") {
               mode="License";
               found=1;
               number=0;
            }

            if (head[0].simplified()=="Size:") {
               mode="Size";
               found=1;
               number=0;
            }

            // End of Meta
            if (head[0].simplified()=="*/") {
               break;
            }

            // Analyse the rest of the lines
            if (mode=="Citation" && found==0) {
                number++;
                QString t=inv->get_att_Comment();
                if (t.length()>0) t.append("; ");
                t.append(lines[i].simplified());
                if (lines[i].contains("In supplement to:")) {
                    inv->set_att_Reference(t.remove(0,18));
                }

            }

            if (mode=="Project(s)" && found==0) {
                number++;
                QString t=inv->get_att_Comment();
                t.append("; ");
                t.append(lines[i].simplified());
                //inv->set_att_Comment(t);

            }

            if (mode=="Parameter(s)" && found==0){
                number++;
                int index=-1;
                for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii)=="d18O") {
                    index=ii;
                    break;
                }
                if (index>-1){
                    if (number==combobox.at(index)->currentIndex()-1 && inv->get_att_Species()==""){
                        QStringList t=lines[i].split(",");
                        inv->set_att_Species(t[0].simplified());
                    }
                }
                index=-1;
                for (int ii=0;ii<inv->access_data_length();ii++) if (inv->access_data_name(ii)=="d13C") {
                    index=ii;
                    break;
                }
                if (index>-1){
                    if (number==combobox.at(index)->currentIndex()-1 && inv->get_att_Species()==""){
                        QStringList t=lines[i].split(",");
                        inv->set_att_Species(t[0].simplified());
                    }
                }

            }
            if (mode=="Event(s)" && found==0){
                number++;
                QStringList t=lines[i].split("*");
                if (lines[i].contains(ui->comboBox_12->currentText())||ui->comboBox_12->currentText()=="Not Selected"){
                    for (int j=0;j<t.size();j++){
                        QStringList p=t[j].split(":");
                        if (j==0) inv->set_att_Core(ui->comboBox_12->currentText());
                        if (p[0].simplified()=="LATITUDE") inv->set_att_Latitude(p[1].toFloat());
                        if (p[0].simplified()=="LONGITUDE") inv->set_att_Longitude(p[1].toFloat());
                        if (p[0].simplified()=="ELEVATION") inv->set_att_Water_Depth(-p[1].replace('m',' ').toFloat());
                        if (p[0].simplified()=="DEVICE") inv->set_att_Device(p[1].simplified());
                    }
                }
            }
        }
        inv->set_att_Category(proxy);
        // Add Data
        int found=0;
        int count=0;




        for (int i=0;i<lines.size();i++){

            if (lines[i]=="*/"){
                found=1;
                i++;
                i++;
            }
            if (found==1){
                QStringList v=lines[i].split("\t");

                for (int i=0;i<v.size();i++) if (v[i]=="") v[i]="NAN";
                if (v.size()==combobox.at(0)->count()-1){
                //  Check if anything besides depth was selected and if there is actually a valid value to be imported
                int ok=0;
                for (int ii=1;ii<inv->access_data_length();ii++) if (combobox.at(ii)->currentIndex()>0 && v[combobox.at(ii)->currentIndex()-1]!="NAN" && (v[0]==ui->comboBox_12->currentText()||ui->comboBox_12->currentText()=="Not Selected") ) ok=1;


                    if (ok){
                        inv->addIsotope();
                        for (int ii=0;ii<inv->access_data_length();ii++){

                            if (combobox.at(ii)->currentIndex()>0) {
                                qDebug()<<QString::number(ii)+" :"+QString::number(count)+" :"+v[combobox.at(ii)->currentIndex()-1];
                                inv->write_data_value(ii,count,v[combobox.at(ii)->currentIndex()-1]);
                            }
                        }
                        count++;
                    }
                }
            }

        }



        //qDebug() << "parsen finished";
        if (inv->get_att_Comment()=="") inv->set_att_Comment("na");
        if (inv->get_att_Species()=="") inv->set_att_Species("na");
        // Create corrected isotopes
        if (proxy=="Foraminiferal Isotopes"){
            for (int i=0;i<inv->get_Length();i++) {
                if (inv->get_data_d13C_Corr(i)==0 ||isnan(inv->get_data_d13C_Corr(i))) inv->set_data_d13C_Corr(inv->get_data_d13C(i)+inv->get_att_C_Correction(),i);
                if (inv->get_data_d18O_Corr(i)==0 ||isnan(inv->get_data_d18O_Corr(i))) inv->set_data_d18O_Corr(inv->get_data_d18O(i)+inv->get_att_O_Correction(),i);
            }
        }
        for (int i=0;i<inv->get_Length();i++) inv->set_data_Use_Flag(1,i);
        if (proxy=="Alkenones") inv->set_att_Species("Alkenones");
        if (proxy=="Alkenones") inv->set_att_SST_Use_Flag(0);
        if (proxy=="Alkenones") qDebug()<<"Flag:"+QString::number(inv->get_att_SST_Use_Flag());
    for (int i=0;i<inv->get_Length();i++) if(inv->get_data_Comment(i)=="") inv->set_data_Comment("na",i);
        save_OK=1;
        setupTable();

    }
}

void ImportPangaea::setupTable(){
    // setup Meta
    if (exist_meta){
        // create the model for Meta Data
        qDebug() << "setting up Meta Table";
        //delete metaData;

        metaData.clear();
        //metaData = new QStandardItemModel(meta_length,2,this);
        metaData.setColumnCount(2);
        metaData.setRowCount(inv->access_meta_length());
        metaData.setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
        metaData.setHorizontalHeaderItem(1, new QStandardItem(QString("Value")));


        ui->tableView->setModel(&metaData);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        QFont font;
        font.setPointSize(resources.get_Textsize());
        ui->tableView->setFont(font);
        ui->tableView->horizontalHeader()->setFont(font);
        ui->tableView->verticalHeader()->setFont(font);
        //QStandardItem *meta_Name = new QStandardItem[meta_length];
        //QStandardItem *meta_Value = new QStandardItem[meta_length];

        qDebug()<<"preparing values";
        // Add Meta
        str2.clear();
        for (int i=0;i<inv->access_meta_length();i++) {
            qDebug()<<"adding "+inv->access_meta_name(i);
            str2.append(inv->access_meta_value(i));
        }


        // and more...
        qDebug()<<"creating meta table";
        for (int i=0;i<inv->access_meta_length();i++) {
            QStandardItem *meta_Name=new QStandardItem(str1.at(i));
            metaData.setItem(i,0,meta_Name);

            QStandardItem *meta_Value=new QStandardItem(str2.at(i));
            metaData.setItem(i,1,meta_Value);
        }

        ui->tableView->setSortingEnabled(0);
        ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
        //ui->tableView_3->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView->horizontalHeader()->setStretchLastSection(1);
        ui->tableView->repaint();
    }
    // setup IsoData
    if (exist_iso){

        // create the model for Meta Data
        qDebug() << "setting up Iso Table";
        //delete modelIsotope;
        modelIsotope.clear();
        modelIsotope.setColumnCount(inv->access_data_length());
        modelIsotope.setRowCount(inv->get_Length());
        //modelIsotope = new QStandardItemModel(inv->get_Length(),10,this);
        for (int i=0;i<inv->access_data_length();i++){
            QString str=inv->access_data_name(i);
            if (inv->access_data_unit(i)!="") str.append("\n["+inv->access_data_unit(i)+"]");
            modelIsotope.setHorizontalHeaderItem(i, new QStandardItem(str));
        }


        ui->tableView_2->setModel(&modelIsotope);
        ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        QFont font;
        font.setPointSize(resources.get_Textsize());
        ui->tableView_2->setFont(font);
        ui->tableView_2->horizontalHeader()->setFont(font);
        ui->tableView->verticalHeader()->setFont(font);


        for (int i=0;i<inv->get_Length();i++){
            for (int j=0;j<inv->access_data_length();j++){

                if (inv->access_data_type(j).contains("flag")){
                    QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                    //var->setCheckable(true);
                    if (inv->access_data_value(j,i).contains("1")){
                        //var->setCheckState(Qt::Checked);
                        //var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                        modelIsotope.setItem(i,j,var);
                        modelIsotope.setData(modelIsotope.index(i, j), Qt::AlignCenter,Qt::TextAlignmentRole);
                        modelIsotope.setData(modelIsotope.index(i, j), QColor(Qt::green), Qt::BackgroundRole);
                    }else{
                        //var->setCheckState(Qt::Unchecked);
                        //var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                        modelIsotope.setItem(i,j,var);
                        modelIsotope.setData(modelIsotope.index(i, j), Qt::AlignCenter,Qt::TextAlignmentRole);
                        modelIsotope.setData(modelIsotope.index(i, j), QColor(Qt::red), Qt::BackgroundRole);
                    }


                } else {
                    if (inv->access_data_type(j).contains("string")){
                        QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));

                            var->setFlags(var->flags() & ~Qt::ItemIsEditable);

                        modelIsotope.setItem(i,j,var);

                    }
                    if (inv->access_data_type(j).contains("float") || inv->access_data_type(j).contains("int")){
                        QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                        var->setData(inv->access_data_value(j,i).toFloat(),Qt::EditRole);

                            var->setFlags(var->flags() & ~Qt::ItemIsEditable);

                        modelIsotope.setItem(i,j,var);

                    }
                }
            }
        }
        //qDebug() << "2";
        ui->tableView_2->setSortingEnabled(0);
        ui->tableView_2->resizeColumnsToContents();
        ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
        ui->tableView_2->horizontalHeader()->setStretchLastSection(1);
        //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView_2->setHorizontalScrollMode(ui->tableView_2->ScrollPerPixel);
        ui->tableView_2->repaint();



     }




}

void ImportPangaea::save(){
    parse();
    if(inv->get_att_Core()!="Not Selected"){
    QString QFilename=ui->lineEdit_2->text();
    if (QFilename=="") QFilename=inv->get_att_Core()+".nc";
    QFilename.replace("\\","/");
    QStringList QPath=QFilename.split("/");
    QFilename=QPath.at(QPath.size()-1);
    QFilename.replace(QString(".txt"),QString(".nc"));
    QFilename.replace(QString(".xlsx"),QString(".nc"));

    int save=save_OK;
/*
    // first check if already in database or if deviating meta for core present
    int double_Entry=0;
    for (int i=0;i<inv2->get_Entries();i++){
        if (inv->get_att_Core()==inv2->get_Core(i)){
            // Core found: check if species already present under different filename
            QString str1=inv2->get_Filename(i).replace('\0',' ');
            QString species1=inv->get_att_Species();
            if (species1.contains("#")){
                species1=species1.split("#").at(0);
            }
            QString species2=inv2->get_Species(i);
            if (species2.contains("#")){
                species2=species2.split("#").at(0);
            }
            if(species1==species2 && QFilename.simplified()!=str1.simplified()){
                // Not Possible because it would result in multiple entry
                double_Entry++;
            }
        }
    }
    if (double_Entry>0){
        QMessageBox msgBox;
        msgBox.setText("The same core/proxy combination already exists.");
        msgBox.setInformativeText("Do you want to mark the species with a #number#?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if (ret==QMessageBox::Discard){
            save=0;
        } else {
           inv->set_att_Species(inv->get_att_Species()+"#"+QString::number(double_Entry)+"#");

        }
    }
*/
    if (exist_iso){
    bool ok_data=true;
    bool ok_ams=true;
    if (exist_iso) {
        if (ui->comboBox_15->currentIndex()==0) ok_data=inv->checkData(0);
        if (ui->comboBox_15->currentIndex()==1) ok_data=inv->checkData(1);
        if (ui->comboBox_15->currentIndex()==2) ok_data=inv->checkData(2);
    } else ok_data=false;
    if (exist_age) {
        ok_ams=amsdata->checkAMS(0);
    }
    else ok_ams=false;

    int exist_file=0;
    int exist_entry=0;
    /***************/


    if (inv2->get_Entries()>0){
        // Check if Entry already exists in inventory
        int count_entry=-1;
        for (int i=0;i<inv2->get_Entries();i++){

            QString str1=inv->get_att_Core().replace('\0',' ');
            QString str2=inv2->get_Core(i).replace('\0',' ');
            QString str3=inv->get_att_Species().replace('\0',' ');
            QString str4=inv2->get_Species(i).replace('\0',' ');
            if (str1.simplified()==str2.simplified() ){

                if(str3.simplified()==str4.simplified()){
                exist_entry=1;
                count_entry=i;
                break;
                }
            }
        }

        // Check if Filename is already in Library
        int count_file=-1;
        for (int i=0;i<inv2->get_Entries();i++){


            QString str1=inv2->get_Filename(i).replace('\0',' ');

            if (QFilename.simplified()==str1.simplified()){
                count_file=i;
                exist_file=1;
                break;
            }
        }
        // Give out a warning if core/species combi already exists
        if (exist_entry!=0 && save==1 && exist_file!=0) {
            QMessageBox msgBox;
            msgBox.setText("The same filename and entry already exists.");
            msgBox.setInformativeText("Do you want to replace it?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            if (ret==QMessageBox::Discard){
                save=0;
            } else {
            // Correct Inventory entry count
                inv2->set_currentCore(count_entry);
                // enter information into last inventory
                qDebug()<<"putting data to inventory";
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }

                inv2->set_Filename(QFilename);

                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    // Check if already linked with an Age Model
                    if (inv2->get_AgeModel(inv2->get_currentCore())) {
                        QMessageBox msgBox;
                        msgBox.setText("The Core is already linked with an existing age model!!!");
                        msgBox.setInformativeText("Do you want to save anyway?(existing age data will be lost!!!)");
                        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
                        msgBox.setDefaultButton(QMessageBox::Save);
                        int ret = msgBox.exec();
                        if (ret==QMessageBox::Discard){

                        } else {
                            amsdata->AMSSave();

                        }
                    } else {
                        amsdata->AMSSave();
                        inv2->set_AgeModel(inv2->get_currentCore(),1);
                    }


                } else {
                    inv2->set_AgeModel(inv2->get_currentCore(),0);
                }
                qDebug()<<"saving data";
                inv2->save();
                inv2->set_Selected(inv2->get_currentCore(),1);
            }

        }



        // Give out a warning if core/species combi already exists
        if (exist_entry!=0 && save==1 && exist_file==0) {
            // find a fitting species name including prefix
            int number=1;
            int exist=1;
            while (exist){

                int i=0;
                for (i=0;i<inv2->get_Entries();i++){

                    QString str1=inv->get_att_Core().replace('\0',' ');
                    QString str2=inv2->get_Core(i).replace('\0',' ');
                    QString str3=inv->get_att_Species().replace('\0',' ')+" #"+QString::number(number);
                    QString str4=inv2->get_Species(i).replace('\0',' ');
                    if (str1.simplified()==str2.simplified() ){

                        if(str3.simplified()==str4.simplified()){
                        exist=1;
                        number++;
                        break;
                        }
                    }
                }
                if (i==inv2->get_Entries()) {
                    exist=0;
                    inv->set_att_Species(inv->get_att_Species()+" #"+QString::number(number));
                }
            }

            QMessageBox msgBox;
            msgBox.setText("An entry with the same "+inv2->access_meta_name(0)+"/"+inv2->access_meta_name(1)+" is already present in the database.");
            msgBox.setInformativeText("Do you want to save anyway?\nThis is NOT recomended: "+inv2->access_meta_name(0)+"/"+inv2->access_meta_name(1)+" needs to be unique!!!\nA Prefix was added to the proxy name.");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            if (ret==QMessageBox::Discard){
                save=0;
            } else {

                // new Entry in inventory
                inv2->addEntry();

                inv2->set_currentCore(inv2->get_Entries()-1);
                // enter information into last inventory
                qDebug()<<"putting data to inventory";
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }

                inv2->set_Filename(QFilename);

                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    // Check if already linked with an Age Model
                    if (inv2->get_AgeModel(inv2->get_currentCore())) {
                        QMessageBox msgBox;
                        msgBox.setText("The Core is already linked with an existing age model!!!");
                        msgBox.setInformativeText("Do you want to save anyway?(existing age data will be lost!!!)");
                        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
                        msgBox.setDefaultButton(QMessageBox::Save);
                        int ret = msgBox.exec();
                        if (ret==QMessageBox::Discard){

                        } else {
                            amsdata->AMSSave();

                        }
                    } else {
                        amsdata->AMSSave();
                        inv2->set_AgeModel(inv2->get_currentCore(),1);
                    }


                } else {
                    inv2->set_AgeModel(inv2->get_currentCore(),0);
                }
                qDebug()<<"saving data";
                inv2->save();
                inv2->set_Selected(inv2->get_currentCore(),1);
            }

        }


        // Give out a warning if filename already exists
        if (exist_file!=0 && save==1 && exist_entry==0) {
            QMessageBox msgBox;
            msgBox.setText("The same filename already exists.");
            msgBox.setInformativeText("Do you want to overwrite it?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            if (ret==QMessageBox::Discard){
                save=0;
            } else {
                // Correct Inventory entry count
                inv2->set_currentCore(count_file);
                // enter information into last inventory
                 qDebug()<<"putting data to inventory";
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }

                inv2->set_Filename(QFilename);


                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    // Check if already linked with an Age Model
                    if (inv2->get_AgeModel(inv2->get_currentCore())) {
                        QMessageBox msgBox;
                        msgBox.setText("The Core is already linked with an existing age model!!!");
                        msgBox.setInformativeText("Do you want to save anyway?(existing age data will be lost!!!)");
                        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
                        msgBox.setDefaultButton(QMessageBox::Save);
                        int ret = msgBox.exec();
                        if (ret==QMessageBox::Discard){

                        } else {
                            amsdata->AMSSave();

                        }
                    } else {
                        amsdata->AMSSave();
                        inv2->set_AgeModel(inv2->get_currentCore(),1);
                    }


                } else {
                    inv2->set_AgeModel(inv2->get_currentCore(),0);
                }
                inv2->save();
                inv2->set_Selected(inv2->get_currentCore(),1);
            }

        }

    }

    // save to library
    if (save_OK==1 && save==1){
        // save Data



        inv->saveData(QFilename);
        qDebug()<<"proxy saved";

        if (exist_file==0&&exist_entry==0){
            // new Entry
            inv2->addEntry();

            inv2->set_currentCore(inv2->get_Entries()-1);
            // enter information into last inventory
            qDebug()<<inv2->get_Core(inv2->get_currentCore());
            for (int i=0;i<inv2->access_inv_length();i++){
                qDebug()<<inv->access_inv_meta(i);
                inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
            }
            qDebug()<<inv2->get_Core(inv2->get_currentCore());
            inv2->set_Filename(QFilename);

            if (exist_age==1){
                amsdata->set_Inventory(inv2);
                qDebug()<<"saving age model";
                amsdata->AMSSave();
                inv2->set_AgeModel(inv2->get_currentCore(),1);
            } else {
                inv2->set_AgeModel(inv2->get_currentCore(),0);
            }
            qDebug()<<"saving inventory";
            inv2->save();
            inv2->set_Selected(inv2->get_currentCore(),1);
        }



        emit(refresh());

        close();
    }
    }
    }
}


void ImportPangaea::Xport(){
    if (save_OK){


            QString file = QFileDialog::getSaveFileName(this, tr("Select Save File"),
                                                     resources.path_data+"//"+proxy+"//Import//"+inv->get_att_Core(),
                                                     tr("Excel (*.xlsx);;Text File (*.txt)"));





        if (file.right(4)==".txt") save(file);
        if (file.right(5)==".xlsx") savexlsx(file);


    }


}

void ImportPangaea::save(QString file){
    // create the result shown on left
    txt.clear();


            // Create Meta Data
            for (int i=0;i<inv->access_meta_length();i++){
                if (meta_Flag[i]){

                    QString tmp=inv->access_meta_name(i);
                    if (inv->access_meta_unit(i)!="") tmp.append(" ["+inv->access_meta_unit(i)+"]");



                         txt.append(tmp+dlm+inv->access_meta_value(i)+"\n");


                }
            }
            txt.append("\n");






    // Create Isotope Data

        txt.append("Proxy Data\n");

            // Header of Table
            for (int j=0;j<inv->access_data_length();j++) {

                txt.append(inv->access_data_name(j));
                        if (inv->access_data_unit(j)!="") txt.append(" ["+inv->access_data_unit(j)+"]");
                if (j<inv->access_data_length()-1) txt.append(dlm); else txt.append("\n");
            }
            // Data
            for(int i=0;i<inv->get_Length();i++){

                    for (int j=0;j<inv->access_data_length();j++) {
                        txt.append(inv->access_data_value(j,i));
                        if (j<inv->access_data_length()-1) txt.append(dlm); else txt.append("\n");
                    }

            }
            txt.append("\n");







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


}

void ImportPangaea::savexlsx(QString file){
    // get file name
    QString QFilename=file;
    QFilename.replace(".txt",".xlsx");

    //qDebug() << QFilename;


    QXlsx::Document xlsx;



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
                        if (inv->access_meta_type(i).contains("string")){
                            xlsx.write("B"+QString::number(count+1),inv->access_meta_value(i));
                        } else {
                            Format lAlign;
                            lAlign.setHorizontalAlignment(Format::AlignLeft);
                            xlsx.write("B"+QString::number(count+1),round(inv->access_meta_value(i).toFloat()*1000.0)/1000.0,lAlign);
                        }
                        count++;
                    }
                }


        xlsx.addSheet("Proxy", AbstractSheet::ST_WorkSheet);
        xlsx.selectSheet("Proxy");
            // Header
            xlsx.setColumnWidth(1, 9, 20);
            for (int i=0;i<inv->access_data_length();i++){
                QString str=inv->access_data_name(i);
                if (inv->access_data_unit(i)!="") txt.append(" ["+inv->access_data_unit(i)+"]");
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






    xlsx.saveAs(QFilename);


    QDesktopServices::openUrl(QUrl("file:///"+QFilename, QUrl::TolerantMode));

}

void ImportPangaea::copy(QPoint pos){

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

void ImportPangaea::copy_2(QPoint pos){

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
