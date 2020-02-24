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

#include "targetimport.h"
#include "ui_targetimport.h"

TargetImport::TargetImport(QMainWindow *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv2(inventory),proxy(str),
    ui(new Ui::TargetImport)
{

    ui->setupUi(this);
    this->setWindowTitle("Import Target Data");

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);
    inv=new Inventory(proxy);
    inv->read();// create an inventory to initialize Variables



    //metaData = new QStandardItemModel(0,0,this);
    //modelIsotope = new QStandardItemModel(0,0,this);
    //ageData= new QStandardItemModel(0,0,this);
    exist_meta=0;
    exist_iso=0;



    str1.clear();
    for (int i=0;i<inv->access_meta_length();i++) str1.append(inv->access_meta_name(i));



    // bring them into shape
    //for (int i=0;i<meta_length;i++) {
    //    while (str1[i].size()<30) str1[i].append(" ");
    //    str1[i].append(":");
    //}

    dlm="\t";
    save_OK=0;
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(resources.get_Textsize());

    ui->plainTextEdit->setFont(font);

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(browse2()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(import()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(target()));
    connect(this,SIGNAL(refresh()),mainW,SLOT(redraw_score()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));

    ui->tableView_3->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_3,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_3(QPoint)));


}

TargetImport::~TargetImport()
{
    delete ui;

    //delete str1;
    //delete str2;
    //delete metaData;
    //delete modelIsotope;
    //delete ageData;
    delete inv;
}

void TargetImport::browse(){
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources.path_PaleoDataView+"/"+resources.workdir+"/"+inv->access_proxy_name()+"/Derived Data/Targets/",
                                             tr("Excel (*.xlsx);;Text File (*.txt)"));
    //qDebug() << filename;
    ui->lineEdit->setText(filename);
    // read File and put to TextBrowser
    // get file name
    QString QFilename=ui->lineEdit->text();
    //qDebug() << QFilename;
    if (QFilename.right(4)==".txt"){
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
    if (QFilename.right(5)==".xlsx"){
        //qDebug() <<"reading Excel";
        QXlsx::Document xlsx(QFilename);
        txt.clear();
        if (xlsx.selectSheet("Meta Data")){
            //qDebug() <<"reading Meta";
            int i=0;
            while (xlsx.read("A"+QString::number(i+1)).toString()!=""){
                //qDebug() << QString::number(i);
                QString s1=xlsx.read("A"+QString::number(i+1)).toString();
                //while (s1.size()<30) s1.append(" ");
                //s1.append(":");
                s1.replace('\n',' ');
                s1.replace('\t',' ');

                 s1.replace('\r',' ');
                QString s2=xlsx.read("B"+QString::number(i+1)).toString();
                s2.replace('\n',' ');
                s2.replace('\t',' ');

                s2.replace('\r',' ');
                if (s2=="") s2="NaN";
                txt.append(s1+dlm+s2+"\n");
                i++;
            }
            txt.append("\n");
        }

        if (xlsx.selectSheet("Proxy") ){

            int i=0;
            txt.append("Proxy Data\n");
            while (xlsx.read("A"+QString::number(i+1)).toString()!=""){
                for(int j=0;j<inv->access_data_length()+4;j++){
                    char c=(char)65+j;
                    QString f="";
                    f.append(c);
                    f.append(QString::number(i+1));
                    qDebug()<<f;
                    QString s=xlsx.read(f).toString();
                    if (s=="") s.append("NaN");
                    txt.append(s);

                    if (j<inv->access_data_length()-1+4) txt.append("\t");
                }
                txt.append("\n");

                i++;
            }
            txt.append("\n");
        }
        if (xlsx.selectSheet("Isotopes") ){

            int i=0;
            txt.append("Proxy Data\n");
            while (xlsx.read("A"+QString::number(i+1)).toString()!=""){
                for(int j=0;j<inv->access_data_length()+4;j++){
                    char c=(char)65+j;
                    QString f="";
                    f.append(c);
                    f.append(QString::number(i+1));
                    qDebug()<<f;
                    QString s=xlsx.read(f).toString();
                    if (s=="") s.append("NaN");
                    txt.append(s);

                    if (j<inv->access_data_length()-1+4) txt.append("\t");
                }
                txt.append("\n");

                i++;
            }
            txt.append("\n");
        }









    }
    //qDebug() << "finished reading";

    ui->plainTextEdit->clear();
    ui->plainTextEdit->setTabStopWidth(120);
    ui->plainTextEdit->setPlainText(txt);

    // check if special characters are present
    int special=0;
    for (int i=0;i<txt.size();i++){
        if (txt.at(i)>QChar(255)) {
            special=1;
            break;
        }
    }
    if (special) {
        QMessageBox msgBox;
                msgBox.setText("Non-ASCII Characters found.");
                msgBox.setInformativeText("Please edit the text to replace them with ASCII characters. Any non-ASCII character that is not changed will be replaced by '_'.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }

    // get Filename
    QStringList filelist=QFilename.split("/");
    QStringList file=filelist.at(filelist.size()-1).split(".");
    QString f=file.at(0);
    // turn all special characters into '_'
    for (int i=0;i<f.size();i++){
        if (f.at(i)>QChar(127)) f.replace(i,1,"_");
    }
    ui->lineEdit_2->setText(resources.path_PaleoDataView+"/"+resources.workdir+"/"+inv->access_proxy_name()+"/Derived Data/Targets/"+f+".tgt");

}

void TargetImport::browse2(){
    QString file = QFileDialog::getSaveFileName(this, tr("Select Save Filename"),
                                             resources.path_PaleoDataView+"/"+resources.workdir+"/"+inv->access_proxy_name()+"/Derived Data/Targets/"+inv->get_att_Core()+".tgt",
                                             tr("Library File (*.tgt)"));

    //qDebug() << file;
    ui->lineEdit_2->setText(file);
}


void TargetImport::import(){//qDebug() <<"started parsing";

    txt=ui->plainTextEdit->toPlainText();
    // turn all special characters into '_'
    for (int i=0;i<txt.size();i++){
        if (txt.at(i)>QChar(255)) {
            qDebug()<<"Replaced Character";
            txt.replace(i,1,"_");
        }
    }



    if (txt!=""){
        // delete all old data
        delete inv;

        inv=new Inventory(proxy);
        inv->read_Basin();
        inv->addEntry();// create an inventory to initialize Variables
        inv->set_currentCore(0);

        inv->set_Filename(ui->lineEdit_2->text());


        // create new Isotope File in Inventory
        qDebug()<<"newIso";
        //amsdata->newAMS(1);//qDebug()<<"newams";
        exist_meta=0;
        exist_iso=0;

        qDebug()<<"init";
        // parse meta Data

        QStringList lines = txt.split("\n"); //Separate into lines
        int iso_data=0;
        int age_data=0;
        int counter=0;
        // clear optional attribute
        inv->newIsotopeMeta();
        inv->set_att_Optional("");
        inv->set_att_Species("na");

        int* iso_header=new int[0];
        int iso_header_size=0;
        int* age_header=new int[0];
        int age_header_size=0;

        for (int i=0;i<lines.size();i++){
            qDebug()<<"parsing line:"+QString::number(i);
            if (iso_data==0 && age_data==0 && lines.at(i).left(12)!="Proxy Data" && lines.at(i).left(14)!="Age Model Data"){ // if not in iso or age mode search for meta
                QStringList fields=lines.at(i).split(dlm);
                qDebug()<<lines.at(i);
                if (fields.size()==2){
                    QString comp=fields.at(0);
                    comp.remove(dlm);

                    comp=comp.simplified();
                    int found=0;

                    for (int j=0;j<inv->access_meta_length();j++){
                        QString str=inv->access_meta_name(j);
                        if (inv->access_meta_unit(j)!="") str.append(" ["+inv->access_meta_unit(j)+"]");
                        if (comp==str && fields.size()==2) {

                            inv->write_meta_value(j,fields.at(1).simplified());
                            qDebug()<<inv->access_meta_value(j);
                            found=1;
                            break;
                        }
                    }

                    // if nothing was found...
                    if (found==0 && comp!=""){
                        // add as optional attribute
                        QString tmp=fields.at(1).simplified();
                        QString tmp2=inv->get_att_Optional();
                        if (tmp2!="") tmp2=tmp2+";";
                        inv->set_att_Optional(tmp2+comp+"="+tmp);
                        qDebug()<<"found:"+comp+"="+tmp;

                        found=1;
                    }

                }



            }
            // Parse Proxy Data
            if (iso_data==1){
                QStringList fields=lines.at(i).split(dlm);
                qDebug() <<lines.at(i);
                if (fields.at(0)!=""){
                    if (fields.size()==iso_header_size){
                        if (counter!=0) inv->addIsotope();
                        for (int j=0;j<fields.size();j++){
                            QString tmp=fields.at(j);
                            if (iso_header[j]>=0) inv->write_data_value(iso_header[j],counter,tmp);
                            if (j==target_age_mean_c) target_age_mean.push_back(tmp);
                            if (j==target_age_Q05_c) target_age_Q05.push_back(tmp);
                            if (j==target_age_Q50_c) target_age_Q50.push_back(tmp);
                            if (j==target_age_Q95_c) target_age_Q95.push_back(tmp);
                            qDebug()<<QString::number(iso_header[j])+":"+QString::number(counter)+":"+tmp;
                        }



                        counter++;
                        //qDebug() << "parsed";
                        exist_iso=1;
                    } else {
                        // not enough data error
                    }
                } else {
                    iso_data=0;
                }
            }
            //Toggle iso_data mode
            if (lines.at(i).left(12)=="Proxy Data" && iso_data==0) {
                iso_data=1;
                age_data=0;
                // read Header
                i++;
                QStringList fields=lines.at(i).split(dlm);
                qDebug()<<"Header:"+lines.at(i);
                delete[] iso_header;
                iso_header=new int[fields.size()];
                iso_header_size=fields.size();
                target_age_mean_c=-1;
                target_age_Q05_c=-1;
                target_age_Q50_c=-1;
                target_age_Q95_c=-1;

                for (int header=0;header<fields.size();header++){
                    iso_header[header]=-1;

                    for (int j=0;j<inv->access_data_length();j++){

                        QString str=inv->access_data_name(j);
                        if (inv->access_data_unit(j)!="") str.append(" ["+inv->access_data_unit(j)+"]");
                        if (fields[header].simplified()==str){
                            iso_header[header]=j;
                        }
                        if (fields[header].simplified()=="Age Mean [ka BP]"){
                            target_age_mean_c=header;
                        }
                        if (fields[header].simplified()=="Age Median [ka BP]"){
                            target_age_Q50_c=header;
                        }
                        if (fields[header].simplified()=="Age Q05 [ka BP]"){
                            target_age_Q05_c=header;
                        }
                        if (fields[header].simplified()=="Age Q95 [ka BP]"){
                            target_age_Q95_c=header;
                        }

                    }
                }


                counter=0; // number of entry
                inv->newIsotope(1);
                target_age_mean.clear();
                target_age_Q05.clear();
                target_age_Q50.clear();
                target_age_Q95.clear();
                //qDebug() << "Found IsoData";

            }




        }
        if (inv->access_meta_length()>0) exist_meta=1;
        qDebug() << "parsen finished";
        // Create corrected isotopes
        if (proxy=="Foraminiferal Isotopes"){
            for (int i=0;i<inv->get_Length();i++) {
                if (inv->get_data_d13C_Corr(i)==0 ||isnan(inv->get_data_d13C_Corr(i))) inv->set_data_d13C_Corr(inv->get_data_d13C(i)+inv->get_att_C_Correction(),i);
                if (inv->get_data_d18O_Corr(i)==0 ||isnan(inv->get_data_d18O_Corr(i))) inv->set_data_d18O_Corr(inv->get_data_d18O(i)+inv->get_att_O_Correction(),i);
            }
        }
        qDebug()<<"preparing tables";
        setupTable();
        update();
        delete[] iso_header;
        delete[] age_header;
        save_OK=1;
        qDebug()<<"finished";
    }
}

void TargetImport::setupTable(){
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


        ui->tableView_3->setModel(&metaData);
        ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
        QFont font;
        font.setPointSize(resources.get_Textsize());
        ui->tableView_3->setFont(font);
        ui->tableView_3->horizontalHeader()->setFont(font);
        ui->tableView_3->verticalHeader()->setFont(font);
        //QStandardItem *meta_Name = new QStandardItem[meta_length];
        //QStandardItem *meta_Value = new QStandardItem[meta_length];

        qDebug()<<"preparing values";
        // Add Meta
        str2.clear();
        for (int i=0;i<inv->access_meta_length();i++) {
            qDebug()<<"adding "+inv->access_meta_name(i);
            str2.append(inv->access_meta_value(i));
        }
        /*
        str2<<inv->get_att_Core()<<inv->get_att_Species()<<QString::number(inv->get_att_Longitude())<<
        QString::number(inv->get_att_Latitude())<<QString::number(inv->get_att_Water_Depth())<<inv->get_att_Device()<<
        inv->get_att_Record_Type()<<inv->get_att_Laboratory()<<inv->get_att_EPaper()<<inv->get_att_Comment()<<inv->get_att_Data_Source()<<
        inv->get_att_Reference()<<QString::number(inv->get_att_Oxygen_Use_Flag())<<QString::number(inv->get_att_Carbon_Use_Flag())<<QString::number(inv->get_att_O_Correction())<<
        QString::number(inv->get_att_C_Correction())<<inv->get_att_O_Justification()<<inv->get_att_C_Justification()<<inv->get_att_Category()<<inv->get_att_Importer()<<
        inv->get_att_Optional();*/

        // and more...
        qDebug()<<"creating meta table";
        for (int i=0;i<inv->access_meta_length();i++) {
            QStandardItem *meta_Name=new QStandardItem(str1.at(i));
            metaData.setItem(i,0,meta_Name);

            QStandardItem *meta_Value=new QStandardItem(str2.at(i));
            metaData.setItem(i,1,meta_Value);
        }

        ui->tableView_3->setSortingEnabled(0);
        ui->tableView_3->verticalHeader()->setDefaultSectionSize(ui->tableView_3->verticalHeader()->minimumSectionSize());
        ui->tableView_3->resizeColumnsToContents();
        ui->tableView_3->setHorizontalScrollMode(ui->tableView_3->ScrollPerPixel);
        //ui->tableView_3->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView_3->horizontalHeader()->setStretchLastSection(1);
        ui->tableView_3->repaint();
    }
    // setup IsoData
    if (exist_iso){

        // create the model for Iso Data
        qDebug() << "setting up Iso Table";
        //delete modelIsotope;
        modelIsotope.clear();
        modelIsotope.setColumnCount(inv->access_data_length()+4);
        modelIsotope.setRowCount(inv->get_Length());
        //modelIsotope = new QStandardItemModel(inv->get_Length(),10,this);
        for (int i=0;i<inv->access_data_length();i++){
            QString str=inv->access_data_name(i);
            if (inv->access_data_unit(i)!="") str.append("\n["+inv->access_data_unit(i)+"]");
            modelIsotope.setHorizontalHeaderItem(i, new QStandardItem(str));
        }
        modelIsotope.setHorizontalHeaderItem(inv->access_data_length()+0,new QStandardItem( "Age Mean [ka BP]"));
        modelIsotope.setHorizontalHeaderItem(inv->access_data_length()+1, new QStandardItem("Age Q05 [ka BP]"));
        modelIsotope.setHorizontalHeaderItem(inv->access_data_length()+2, new QStandardItem("Age Median [ka BP]"));
        modelIsotope.setHorizontalHeaderItem(inv->access_data_length()+3, new QStandardItem("Age Q95 [ka BP]"));

        ui->tableView->setModel(&modelIsotope);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        QFont font;
        font.setPointSize(resources.get_Textsize());
        ui->tableView->setFont(font);
        ui->tableView->horizontalHeader()->setFont(font);
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
            {
            QStandardItem *var=new QStandardItem(target_age_mean[i].toFloat());
            var->setData(target_age_mean[i].toFloat(),Qt::EditRole);
            var->setFlags(var->flags() & ~Qt::ItemIsEditable);
            modelIsotope.setItem(i,inv->access_data_length()+0,var);
            }
            {
            QStandardItem *var=new QStandardItem(target_age_Q50[i].toFloat());
            var->setData(target_age_Q50[i].toFloat(),Qt::EditRole);
            var->setFlags(var->flags() & ~Qt::ItemIsEditable);
            modelIsotope.setItem(i,inv->access_data_length()+1,var);
            }
            {
            QStandardItem *var=new QStandardItem(target_age_Q05[i].toFloat());
            var->setData(target_age_Q05[i].toFloat(),Qt::EditRole);
            var->setFlags(var->flags() & ~Qt::ItemIsEditable);
            modelIsotope.setItem(i,inv->access_data_length()+2,var);
            }
            {
            QStandardItem *var=new QStandardItem(target_age_Q95[i].toFloat());
            var->setData(target_age_Q95[i].toFloat(),Qt::EditRole);
            var->setFlags(var->flags() & ~Qt::ItemIsEditable);
            modelIsotope.setItem(i,inv->access_data_length()+3,var);
            }

        }
        //qDebug() << "2";
        ui->tableView->setSortingEnabled(0);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
        ui->tableView->horizontalHeader()->setStretchLastSection(1);
        //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
        ui->tableView->repaint();



     }


    //qDebug() << "Tables finished";*/
}



bool TargetImport::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->tableView||obj==ui->tableView_3||obj==ui->plainTextEdit){
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



void TargetImport::copy(QPoint pos){

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


void TargetImport::copy_3(QPoint pos){

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

void TargetImport::target(){

    qDebug()<<"saving target";
    QString QFilename = ui->lineEdit_2->text();
    if (QFilename!=""){


    // estimate median,mean und quantile via interpolation
    float* agedata=new float[(inv->access_data_length()+4)*inv->get_Length()];
    for (int i=0;i<inv->get_Length();i++){








        for (int ii=0;ii<inv->access_data_length();ii++){
            agedata[i+ii*inv->get_Length()]=inv->access_data_value(ii,i).toFloat();
        }
        agedata[i+(inv->access_data_length()+0)*inv->get_Length()]=target_age_mean[i].toFloat();
        agedata[i+(inv->access_data_length()+1)*inv->get_Length()]=target_age_Q50[i].toFloat();
        agedata[i+(inv->access_data_length()+2)*inv->get_Length()]=target_age_Q05[i].toFloat();
        agedata[i+(inv->access_data_length()+3)*inv->get_Length()]=target_age_Q95[i].toFloat();
    }

    // Save the Data into netcdf
    error_text.clear();
    QString action="Save As Target";

    // get file name
    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
   qDebug()<< "Save to :"+QFilename;


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
        dat[ii]=agedata[ii+(inv->access_data_length()+0)*inv->get_Length()];
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
        dat[ii]=agedata[ii+(inv->access_data_length()+1)*inv->get_Length()];
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
        dat[ii]=agedata[ii+(inv->access_data_length()+2)*inv->get_Length()];
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
        dat[ii]=agedata[ii+(inv->access_data_length()+3)*inv->get_Length()];
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



    close();
}

void TargetImport::NetCDF_Error(QString filename,QString action,int value){
    error_text.append(filename+" : "+action+" : "+nc_strerror(value)+" : "+QString::number(value)+"\n");

}

void TargetImport::NetCDF_Error_Message(){
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
