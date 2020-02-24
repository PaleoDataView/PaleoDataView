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

#include "import.h"
#include "ui_import.h"

Import::Import(QMainWindow *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv2(inventory),proxy(str),
    ui(new Ui::Import)
{

    ui->setupUi(this);
    this->setWindowTitle("Import");

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    qApp->installEventFilter(this);
    inv=new Inventory(proxy);
    inv->read();// create an inventory to initialize Variables


    amsdata=new AMSData(inv);

    exist_meta=0;
    exist_iso=0;
    exist_age=0;
    

    str1.clear();
    for (int i=0;i<inv->access_meta_length();i++) str1.append(inv->access_meta_name(i));



    // bring them into shape


    dlm="\t";
    save_OK=0;
    parseOK=0;
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(resources.get_Textsize());

    ui->plainTextEdit->setFont(font);

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(browse2()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(import()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(this,SIGNAL(refresh()),mainW,SLOT(redraw_score()));

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));
    ui->tableView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_2(QPoint)));
    ui->tableView_3->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_3,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy_3(QPoint)));


}

Import::~Import()
{
    delete ui;
    delete amsdata;

    delete inv;
}

void Import::browse(){
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Import",
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
            txt.clear();
            txt.append(file.errorString());

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
                for(int j=0;j<inv->access_data_length();j++){
                    char c=(char)65+j;
                    QString f="";
                    f.append(c);
                    f.append(QString::number(i+1));
                    qDebug()<<f;
                    QString s=xlsx.read(f).toString();
                    if (s=="") s.append("NaN");
                    txt.append(s);

                    if (j<inv->access_data_length()-1) txt.append("\t");
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
                for(int j=0;j<inv->access_data_length();j++){
                    char c=(char)65+j;
                    QString f="";
                    f.append(c);
                    f.append(QString::number(i+1));
                    qDebug()<<f;
                    QString s=xlsx.read(f).toString();
                    if (s=="") s.append("NaN");
                    txt.append(s);

                    if (j<inv->access_data_length()-1) txt.append("\t");
                }
                txt.append("\n");

                i++;
            }
            txt.append("\n");
        }





        if (xlsx.selectSheet("Age")){

            //qDebug() <<"reading Age Model";
            int i=0;
            txt.append("Age Model Data\n");
            while (xlsx.read("A"+QString::number(i+1)).toString()!=""){
                //qDebug() << QString::number(i);
                QString s1=xlsx.read("A"+QString::number(i+1)).toString();
                QString s2=xlsx.read("B"+QString::number(i+1)).toString();
                QString s3=xlsx.read("C"+QString::number(i+1)).toString();
                QString s4=xlsx.read("D"+QString::number(i+1)).toString();
                QString s5=xlsx.read("E"+QString::number(i+1)).toString();
                QString s6=xlsx.read("F"+QString::number(i+1)).toString();
                QString s7=xlsx.read("G"+QString::number(i+1)).toString();
                QString s8=xlsx.read("H"+QString::number(i+1)).toString();
                QString s9=xlsx.read("I"+QString::number(i+1)).toString();
                QString s10=xlsx.read("J"+QString::number(i+1)).toString();
                QString s11=xlsx.read("K"+QString::number(i+1)).toString();
                QString s12=xlsx.read("L"+QString::number(i+1)).toString();
                QString s13=xlsx.read("M"+QString::number(i+1)).toString();
                QString s14=xlsx.read("N"+QString::number(i+1)).toString();
                if (s1=="") s1="NaN";
                if (s2=="") s2="NaN";
                if (s3=="") s3="NaN";
                if (s4=="") s4="NaN";
                if (s5=="") s5="NaN";
                if (s6=="") s6="NaN";
                if (s7=="") s7="NaN";
                if (s8=="") s8="NaN";
                if (s9=="") s9="NaN";
                if (s10=="") s10="NaN";
                if (s11=="") s11="NaN";
                if (s12=="") s12="NaN";
                if (s13=="") s13="NaN";
                if (s14=="") s14="NaN";

                txt.append(s1+"\t"+s2+"\t"+s3+"\t"+s4+"\t"+s5+"\t"+s6+"\t"+s7+"\t"+s8+"\t"+s9+"\t"+s10+"\t"+s11+"\t"+s12+"\t"+s13+"\t"+s14+"\n");
                //if (i>0)txt.append(QString::number(s1.toFloat(0))+"\t"+s2+"\t"+s3+"\t"+QString::number(s4.toFloat(0))+"\t"+QString::number(s5.toFloat(0))+"\t"+QString::number(s6.toFloat(0))+"\t"+QString::number(s7.toFloat(0))+"\t"+QString::number(s8.toFloat(0))+"\t"+QString::number(s9.toFloat(0))+"\t"+QString::number(s10.toFloat(0))+"\t"+QString::number(s11.toFloat(0))+"\t"+s12+"\n");
                i++;
            }
            txt.append("\n");
        }
        if (xlsx.selectSheet("Age Model")){

            //qDebug() <<"reading Age Model";
            int i=0;
            txt.append("Age Model Data\n");
            while (xlsx.read("A"+QString::number(i+1)).toString()!=""){
                //qDebug() << QString::number(i);
                QString s1=xlsx.read("A"+QString::number(i+1)).toString();
                QString s2=xlsx.read("B"+QString::number(i+1)).toString();
                QString s3=xlsx.read("C"+QString::number(i+1)).toString();
                QString s4=xlsx.read("D"+QString::number(i+1)).toString();
                QString s5=xlsx.read("E"+QString::number(i+1)).toString();
                QString s6=xlsx.read("F"+QString::number(i+1)).toString();
                QString s7=xlsx.read("G"+QString::number(i+1)).toString();
                QString s8=xlsx.read("H"+QString::number(i+1)).toString();
                QString s9=xlsx.read("I"+QString::number(i+1)).toString();
                QString s10=xlsx.read("J"+QString::number(i+1)).toString();
                QString s11=xlsx.read("K"+QString::number(i+1)).toString();
                QString s12=xlsx.read("L"+QString::number(i+1)).toString();
                QString s13=xlsx.read("M"+QString::number(i+1)).toString();
                QString s14=xlsx.read("N"+QString::number(i+1)).toString();
                if (s1=="") s1="NaN";
                if (s2=="") s2="NaN";
                if (s3=="") s3="NaN";
                if (s4=="") s4="NaN";
                if (s5=="") s5="NaN";
                if (s6=="") s6="NaN";
                if (s7=="") s7="NaN";
                if (s8=="") s8="NaN";
                if (s9=="") s9="NaN";
                if (s10=="") s10="NaN";
                if (s11=="") s11="NaN";
                if (s12=="") s12="NaN";
                if (s13=="") s13="NaN";
                if (s14=="") s14="NaN";
                txt.append(s1+"\t"+s2+"\t"+s3+"\t"+s4+"\t"+s5+"\t"+s6+"\t"+s7+"\t"+s8+"\t"+s9+"\t"+s10+"\t"+s11+"\t"+s12+"\t"+s13+"\t"+s14+"\n");
                //if (i>0)txt.append(QString::number(s1.toFloat(0))+"\t"+s2+"\t"+s3+"\t"+QString::number(s4.toFloat(0))+"\t"+QString::number(s5.toFloat(0))+"\t"+QString::number(s6.toFloat(0))+"\t"+QString::number(s7.toFloat(0))+"\t"+QString::number(s8.toFloat(0))+"\t"+QString::number(s9.toFloat(0))+"\t"+QString::number(s10.toFloat(0))+"\t"+QString::number(s11.toFloat(0))+"\t"+s12+"\n");
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
    ui->lineEdit_2->setText(resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+f+".nc");

}

void Import::browse2(){
    QString file = QFileDialog::getSaveFileName(this, tr("Select Save Filename"),
                                             ui->lineEdit_2->text(),
                                             tr("Library File (*.nc)"));

    //qDebug() << file;
    ui->lineEdit_2->setText(file);
}


void Import::import(){//qDebug() <<"started parsing";

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
        delete amsdata;
        inv=new Inventory(proxy);
        inv->read_Basin();
        inv->addEntry();// create an inventory to initialize Variables
        inv->set_currentCore(0);

        inv->set_Filename(ui->lineEdit_2->text());

        amsdata=new AMSData(inv);
        // create new Isotope File in Inventory
        qDebug()<<"newIso";
        //amsdata->newAMS(1);//qDebug()<<"newams";
        exist_meta=0;
        exist_iso=0;
        exist_age=0;
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

                for (int header=0;header<fields.size();header++){
                    iso_header[header]=-1;
                    for (int j=0;j<inv->access_data_length();j++){

                        QString str=inv->access_data_name(j);
                        if (inv->access_data_unit(j)!="") str.append(" ["+inv->access_data_unit(j)+"]");
                        if (fields[header].simplified()==str){
                            iso_header[header]=j;
                        }
                    }
                }


                counter=0; // number of entry
                inv->newIsotope(1);
                //qDebug() << "Found IsoData";

            }

            // Parse Age Data
            if (age_data==1){
                QStringList fields=lines.at(i).split(dlm);
                qDebug() <<lines.at(i);
                if (fields.size()==age_header_size){
                    QString tmp=fields.at(0);
                    for (int j=0;j<fields.size();j++){
                        QString tmp=fields.at(j);
                        if (age_header[j]==0) amsdata->set_Depth(counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==1) if (tmp.replace(",",".").toDouble()!=0) amsdata->set_Sample_Thickness(counter,tmp.replace(",",".").toDouble()); else amsdata->set_Sample_Thickness(counter,NAN);
                        if (age_header[j]==2) amsdata->set_LabID(counter,tmp);
                        if (age_header[j]==3) amsdata->set_Type(counter,tmp);
                        if (age_header[j]==4) amsdata->set_Data(0,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==5) amsdata->set_Data(1,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==6) amsdata->set_Data(2,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==7) if (tmp.replace(",",".").toDouble()==NAN || tmp.replace(",",".")=="") amsdata->set_Data(3,counter,0); else amsdata->set_Data(3,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==8) if (tmp.replace(",",".").toDouble()==NAN || tmp.replace(",",".")=="") amsdata->set_Reservoir_Error(counter,NAN); else amsdata->set_Reservoir_Error(counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==9) amsdata->set_Data(4,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==10) amsdata->set_Data(5,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==11) amsdata->set_Data(6,counter,tmp.replace(",",".").toDouble());
                        if (age_header[j]==12) amsdata->set_Data(7,counter,tmp.replace(",",".").toInt());
                        if (age_header[j]==13) amsdata->set_Age_Comment(tmp,counter);
                    }
                    counter++;
                    exist_age=1;
                } else {
                    age_data=0;
                }
            }
            //Toggle age_data mode
            if (lines.at(i).left(14)=="Age Model Data" && age_data==0) {
                iso_data=0;
                age_data=1;

                // read Header
                i++;
                QStringList fields=lines.at(i).split(dlm);
                qDebug()<<"Header:"+lines.at(i);
                delete[] age_header;
                age_header=new int[fields.size()];
                age_header_size=fields.size();
                for (int header=0;header<fields.size();header++){
                    if (fields[header].simplified()=="Depth [m]") age_header[header]=0;
                    if (fields[header].simplified()=="Sample Thickness [m]") age_header[header]=1;
                    if (fields[header].simplified()=="Label") age_header[header]=2;
                    if (fields[header].simplified()=="Type") age_header[header]=3;
                    if (fields[header].simplified()=="Age dated [ka]") age_header[header]=4;
                    if (fields[header].simplified()=="Age UCL [ka +]") age_header[header]=5;
                    if (fields[header].simplified()=="Age LCL [ka -]") age_header[header]=6;
                    if (fields[header].simplified()=="Res. Age [ka]") age_header[header]=7;
                    if (fields[header].simplified()=="Res. Age Error [ka]") age_header[header]=8;
                    if (fields[header].simplified()=="Cal yrs [wm ka BP]") age_header[header]=9;
                    if (fields[header].simplified()=="Cal yrs min [95%]") age_header[header]=10;
                    if (fields[header].simplified()=="Cal yrs max [95%]") age_header[header]=11;
                    if (fields[header].simplified()=="Use Flag") age_header[header]=12;
                    if (fields[header].simplified()=="Dating Method/Comments") age_header[header]=13;
                }

                counter=0; // number of entry
                int length=0;
                while (i+length<lines.size()-1 && lines.at(i+length)!=""){
                    length++;
                }
                if (length==0){
                    age_data=0;
                }else{
                    amsdata->newAMS(length-1);
                }
                qDebug() << "Length of AMS: "+QString::number(length-1);

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
        QString QFilename=ui->lineEdit_2->text();
        if (QFilename=="") QFilename=inv->get_att_Core()+".nc";
        QFilename.replace("\\","/");
        QStringList QPath=QFilename.split("/");
        QFilename=QPath.at(QPath.size()-1);
        QFilename.replace(QString(".txt"),QString(".nc"));
        QFilename.replace(QString(".xlsx"),QString(".nc"));
        inv->set_Filename(QFilename);
        qDebug()<<"preparing tables";

        setupTable();
        update();
        delete[] iso_header;
        delete[] age_header;
        save_OK=1;
        qDebug()<<"finished";
    }
}

void Import::setupTable(){
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

    // setup AgeModel
    if (exist_age){
    qDebug() << "setting up Age Table";
    //delete ageData;
    ageData.clear();
    ageData.setColumnCount(15);
    ageData.setRowCount(amsdata->get_Length());
    //ageData = new QStandardItemModel(amsdata->get_Length(),15,this);
    ageData.setHorizontalHeaderItem(0, new QStandardItem(QString("Index")));
    ageData.setHorizontalHeaderItem(1, new QStandardItem(QString("Depth")));
    ageData.setHorizontalHeaderItem(2, new QStandardItem(QString("Sample\nThickness")));
    ageData.setHorizontalHeaderItem(3, new QStandardItem(QString("Label")));
    ageData.setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    ageData.setHorizontalHeaderItem(5, new QStandardItem(QString("Age dated\n[ka]")));
    ageData.setHorizontalHeaderItem(6, new QStandardItem(QString("Age UCL\n[ka+]")));
    ageData.setHorizontalHeaderItem(7, new QStandardItem(QString("Age LCL\n[ka-]")));
    ageData.setHorizontalHeaderItem(8, new QStandardItem(QString("Res. Age\n[ka]")));
    ageData.setHorizontalHeaderItem(9, new QStandardItem(QString("Res. Age Error\n[ka]")));
    ageData.setHorizontalHeaderItem(10, new QStandardItem(QString("Cal yrs\n[wm ka BP]")));
    ageData.setHorizontalHeaderItem(11, new QStandardItem(QString("Cal yrs min\n[95%]")));
    ageData.setHorizontalHeaderItem(12, new QStandardItem(QString("Cal yrs max\n[95%]")));
    ageData.setHorizontalHeaderItem(13, new QStandardItem(QString("Use Flag")));
    ageData.setHorizontalHeaderItem(14, new QStandardItem(QString("Comments")));
    //qDebug() << "1";
    ui->tableView_2->setModel(&ageData);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFont font;
    font.setPointSize(resources.get_Textsize());
    ui->tableView_2->setFont(font);
    ui->tableView_2->horizontalHeader()->setFont(font);
    ui->tableView_2->verticalHeader()->setFont(font);

    for (int i=0;i<amsdata->get_Length();i++){
            QStandardItem *var_Index=new QStandardItem(QString::number(i));
            ageData.setItem(i,0,var_Index);

            QStandardItem *var_Depth=new QStandardItem(QString::number(amsdata->get_Depth(i)));
            ageData.setItem(i,1,var_Depth);

            QStandardItem *var_Sample_Thickness=new QStandardItem(QString::number(amsdata->get_Sample_Thickness(i)));
            ageData.setItem(i,2,var_Sample_Thickness);

            QStandardItem *var_Label=new QStandardItem(amsdata->get_LabID(i));
            ageData.setItem(i,3,var_Label);

            QStandardItem *var_Type=new QStandardItem(amsdata->get_Type(i));
            ageData.setItem(i,4,var_Type);

            QStandardItem *var_Age_dated=new QStandardItem(QString::number(amsdata->get_Data(0,i)));
            ageData.setItem(i,5,var_Age_dated);

            QStandardItem *var_Age_UCL=new QStandardItem(QString::number(amsdata->get_Data(1,i)));
            ageData.setItem(i,6,var_Age_UCL);

            QStandardItem *var_Age_LCL=new QStandardItem(QString::number(amsdata->get_Data(2,i)));
            ageData.setItem(i,7,var_Age_LCL);

            QStandardItem *var_Age_Res=new QStandardItem(QString::number(amsdata->get_Data(3,i)));
            ageData.setItem(i,8,var_Age_Res);

            QStandardItem *var_Age_Res_Error=new QStandardItem(QString::number(amsdata->get_Reservoir_Error(i)));
            ageData.setItem(i,9,var_Age_Res_Error);

            QStandardItem *var_Cal=new QStandardItem(QString::number(amsdata->get_Data(4,i)));
            ageData.setItem(i,10,var_Cal);

            QStandardItem *var_Cal_Min=new QStandardItem(QString::number(amsdata->get_Data(5,i)));
            ageData.setItem(i,11,var_Cal_Min);

            QStandardItem *var_Cal_Max=new QStandardItem(QString::number(amsdata->get_Data(6,i)));
            ageData.setItem(i,12,var_Cal_Max);

            QStandardItem *var_Use_Flag=new QStandardItem(QString::number(amsdata->get_Data(7,i)));
            ageData.setItem(i,13,var_Use_Flag);
            ageData.setData(ageData.index(i, 12), Qt::AlignCenter,Qt::TextAlignmentRole);

            QStandardItem *var_Comment=new QStandardItem(amsdata->get_Age_Comment(i));
            ageData.setItem(i,14,var_Comment);

    }
    //qDebug()<<"3";
    ui->tableView_2->setSortingEnabled(0);
    ui->tableView_2->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setHorizontalScrollMode(ui->tableView_2->ScrollPerPixel);
    //ui->tableView_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(1);
    ui->tableView_2->repaint();
    }
    parseOK=1;
    //qDebug() << "Tables finished";*/
}

void Import::save(){

    import();// just to be sure
    int save=save_OK;
    qDebug()<<"creating Filenames";
    QString QFilename=ui->lineEdit_2->text();
    if (QFilename=="") QFilename=inv->get_att_Core()+".nc";
    QFilename.replace("\\","/");
    QStringList QPath=QFilename.split("/");
    QFilename=QPath.at(QPath.size()-1);
    QFilename.replace(QString(".txt"),QString(".nc"));
    QFilename.replace(QString(".xlsx"),QString(".nc"));


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

    if (parseOK==1){
    qDebug()<<"checking data";
    bool ok_data=true;
    bool ok_ams=true;
    if (exist_iso) {
        if (ui->comboBox->currentIndex()==0) ok_data=inv->checkData(0);
        if (ui->comboBox->currentIndex()==1) ok_data=inv->checkData(1);
        if (ui->comboBox->currentIndex()==2) ok_data=inv->checkData(2);
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

        // Give out a warning if core/species combi already exists and file alread exists(overwrite existing entry)
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
                    if (inv->checkAgeFile()) {
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



        // Give out a warning if core/species combi already exists(create duplicate entry under new filename)
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
            msgBox.setText("The entry "+inv2->access_meta_name(0)+"/"+inv2->access_meta_name(1)+" is already present in the database.");
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
                    if (inv->checkAgeFile()) {
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


        // Give out a warning if filename already exists(save under same filename with different proxy)
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
                // enter information current inventory
                 qDebug()<<"putting data to inventory";
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }

                inv2->set_Filename(QFilename);


                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    // Check if already linked with an Age Model
                    if (inv->checkAgeFile()) {
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


        qDebug()<<"saving";
        inv->saveData(QFilename);
        qDebug()<<"proxy saved";

        if (exist_file==0 && exist_entry==0){
            // new Entry in inventory
            inv2->addEntry();

            inv2->set_currentCore(inv2->get_Entries()-1);
            // enter information into last inventory
             qDebug()<<"putting data to inventory";
            qDebug()<<inv2->get_Core(inv2->get_currentCore());
            for (int i=0;i<inv2->access_inv_length();i++){
                inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
            }
            qDebug()<<inv2->get_Core(inv2->get_currentCore());
            inv2->set_Filename(QFilename);

            if (exist_age==1){
                amsdata->set_Inventory(inv2);
                // Check if already linked with an Age Model
                if (inv->checkAgeFile()) {
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
            qDebug()<<"saving inventory";
            inv2->save();
            inv2->set_Selected(inv2->get_currentCore(),1);
        }



        emit(refresh());

        close();
    }
    } else {
        QMessageBox msgBox;
        msgBox.setText("First it is nesessary to parse the file content on the left(Press 'Parse' Button).");
        msgBox.exec();
    }

}

bool Import::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->tableView_2||obj==ui->tableView||obj==ui->tableView_3||obj==ui->plainTextEdit){
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



void Import::copy(QPoint pos){

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

void Import::copy_2(QPoint pos){

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

void Import::copy_3(QPoint pos){

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


