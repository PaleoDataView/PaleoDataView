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

#include "importall.h"
#include "ui_importall.h"

ImportAll::ImportAll(QMainWindow *mainWindow,Inventory *inventory,QString str) :
                     mainW(mainWindow),inv2(inventory),proxy(str),
                     ui(new Ui::ImportAll)
{
    ui->setupUi(this);

    inv=new Inventory(proxy);
    inv->read();// create an inventory to initialize Variables

    amsdata=new AMSData(inv2);

    exist_meta=0;
    exist_iso=0;
    exist_age=0;

    inv2->setShowError();



    // bring them into shape


    dlm="\t";
    save_OK=0;
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);

    ui->plainTextEdit->setFont(font);

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(start()));
    connect(this,SIGNAL(refresh()),mainW,SLOT(redraw_score()));
}

ImportAll::~ImportAll()
{
    delete ui;
    delete inv;

    delete amsdata;
}

void ImportAll::browse(){
    QString folder = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
                                             "",0);

    ui->lineEdit->setText(folder);

    QDir dir=folder;
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QStringList filters;
    filters<<"*.xlsx"<<"*.txt";
    dir.setNameFilters(filters);
    list = dir.entryInfoList();

    ui->plainTextEdit->appendPlainText("\n"+QString::number(list.size())+" files(EXCEL+ASCII) found at specified location :");
    for (int i=0;i<list.size();i++) ui->plainTextEdit->appendPlainText(list.at(i).fileName());

}

void ImportAll::start(){
    ui->plainTextEdit->appendPlainText("Starting to import...");
    for (int i=0;i<inv2->get_Entries();i++) inv2->set_Selected(i,0); // deselect all
    for (int filenr=0;filenr<list.size();filenr++){
        ui->plainTextEdit->appendPlainText(list.at(filenr).fileName()+"found.");
        update();
        // Init
        exist_meta=0;
        exist_iso=0;
        exist_age=0;

        //-----------------------------------------------------------
        //extract txt from file
        QString QFilename=ui->lineEdit->text()+"/"+list.at(filenr).fileName();
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
        ui->plainTextEdit->appendPlainText("...Reading complete...");
        update();
       //qDebug() << "finished reading";

        // get Filename

        QStringList filelist=QFilename.split("/");
        QStringList file=filelist.at(filelist.size()-1).split(".");
        QString f=file.at(0);
        // turn all special characters into '_'
        for (int i=0;i<f.size();i++){
            if (f.at(i)>QChar(127)) f.replace(i,1,"_");
        }
        QString saveToFileName=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+f+".nc";

        //---------------------------------------------------------------
        // Parse txt
        // turn all special characters into '_'
        for (int i=0;i<txt.size();i++){
            if (txt.at(i)>QChar(255)) {
                qDebug()<<"Replaced Character";
                txt.replace(i,1,"_");
            }
        }
        // delete all old data

        //delete inv;
        delete amsdata;
        //inv=new Inventory(proxy);

        inv->read_Basin();
        inv->addEntry();// create an inventory to initialize Variables
        inv->set_currentCore(0);
        inv->set_Filename(saveToFileName);
        amsdata=new AMSData(inv);
        // create new Isotope File in Inventory
       //qDebug()<<"newIso";
        //amsdata->newAMS(1);qDebug()<<"newams";
        exist_meta=0;
        exist_iso=0;
        exist_age=0;
        save_OK=0;
       //qDebug()<<"init";
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
                   //qDebug()<<"found:"+comp+"="+tmp;

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
               //qDebug() <<lines.at(i);
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
               //qDebug()<<"Header:"+lines.at(i);
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
               //qDebug() << "Length of AMS: "+QString::number(length-1);

            }


        }
        if (meta_length>0) exist_meta=1;
       //qDebug() << "parsen finished";
        // Create corrected isotopes
        if (proxy=="Foraminiferal Isotopes"){
            for (int i=0;i<inv->get_Length();i++) {
                if (inv->get_data_d13C_Corr(i)==0 ||isnan(inv->get_data_d13C_Corr(i))) inv->set_data_d13C_Corr(inv->get_data_d13C(i)+inv->get_att_C_Correction(),i);
                if (inv->get_data_d18O_Corr(i)==0 ||isnan(inv->get_data_d18O_Corr(i))) inv->set_data_d18O_Corr(inv->get_data_d18O(i)+inv->get_att_O_Correction(),i);
            }
        }

        delete[] iso_header;
        delete[] age_header;
        save_OK=1;
        ui->plainTextEdit->appendPlainText("...Parsing complete...");
        update();

        //---------------------------------------------------------------------
        // save

        QFilename=saveToFileName;
        if (QFilename=="") QFilename=inv->get_att_Core()+".nc";
        QFilename.replace("\\","/");
        QStringList QPath=QFilename.split("/");
        QFilename=QPath.at(QPath.size()-1);
        QFilename.replace(QString(".txt"),QString(".nc"));
        QFilename.replace(QString(".xlsx"),QString(".nc"));

        int save=save_OK;
/*
        // first check if already in database
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

               inv->set_att_Species(inv->get_att_Species()+"#"+QString::number(double_Entry)+"#");


        }*/




        bool ok_data=true;
        bool ok_ams=true;
        int mode=2;
        if (ui->checkBox_2->isChecked()==true) mode=1;

        if (exist_iso) ok_data=inv->checkData(mode); else ok_data=false;
        if (exist_age) ok_ams=amsdata->checkAMS(mode); else ok_ams=false;

        int exist_entry=0;
        int exist_file=0;
        // Check if Entry already exists
        int count_entry=0;
        if (inv2->get_Entries()>0){
        for (int i=0;i<inv2->get_Entries();i++){

            QString str1=inv->get_att_Core().replace('\0',' ');
            QString str2=inv2->get_Core(i).replace('\0',' ');
            QString str3=inv->get_att_Species().replace('\0',' ');
            QString str4=inv2->get_Species(i).replace('\0',' ');
            if (str1.simplified()==str2.simplified() && str3.simplified()==str4.simplified()){
                count_entry=i;
                exist_entry=1;
                break;
            }
        }
        // Check if Filename is already in Library
        int count_file=0;

        for (int i=0;i<inv2->get_Entries();i++){


            QString str1=inv2->get_Filename(i).replace('\0',' ');

            if (QFilename.simplified()==str1.simplified()){
                count_file=i;
                exist_file=1;
                break;
            }
        }

        // Give out a warning
        if (exist_entry==1 && save==1 &&exist_file==1) {

            if (!ui->checkBox->isChecked()){
                save=0;
                ui->plainTextEdit->appendPlainText("...Already Exists -> NOT Saved!\n");


            } else {
                ui->plainTextEdit->appendPlainText("...Already Exists -> Overwritten!\n");

            // Correct Inventory entry count
                inv2->set_currentCore(count_entry);
                // enter information into inventory
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }
                inv2->set_Filename(QFilename);
                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    if (ui->checkBox_3->isChecked()|| !inv->checkAgeFile()) amsdata->AMSSave(); else {
                        ui->plainTextEdit->appendPlainText("...Age file already exists -> NOT saved!\n");

                    }
                    inv2->set_AgeModel(inv2->get_currentCore(),1);
                } else {
                    inv2->set_AgeModel(inv2->get_currentCore(),0);
                }
            }
            inv2->save();
            inv2->set_Selected(inv2->get_currentCore(),1);
        }


        // Give out a warning
        if (exist_entry==1 && save==1 && exist_file==0) {


                ui->plainTextEdit->appendPlainText("...Already Exists -> Species prefix added\n");
                update();
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

                // new Entry in inventory
                inv2->addEntry();

                inv2->set_currentCore(inv2->get_Entries()-1);
                // enter information into last inventory
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }
                inv2->set_Filename(QFilename);

                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    if (ui->checkBox_3->isChecked()|| !inv->checkAgeFile()) amsdata->AMSSave(); else {
                        ui->plainTextEdit->appendPlainText("...Age file already exists -> NOT saved!\n");

                    }
                    inv2->set_AgeModel(inv2->get_currentCore(),1);
                } else {
                    inv2->set_AgeModel(inv2->get_currentCore(),0);
                }

            inv2->save();
            inv2->set_Selected(inv2->get_Entries()-1,1);

        }

        }

        // save to library
        if (save_OK==1 && save==1){
            // save Data



            inv->saveData(QFilename);
            ui->plainTextEdit->appendPlainText("...Saving Complete\n");


            if (exist_file==0&&exist_entry==0){
                // new Entry
                inv2->addEntry();
                inv2->set_currentCore(inv2->get_Entries()-1);
                // enter information into last inventory
                for (int i=0;i<inv2->access_inv_length();i++){
                    inv2->write_inv_data(i,inv2->get_currentCore(),inv->access_inv_meta(i));
                }
                inv2->set_Filename(QFilename);
                inv2->set_Selected(inv2->get_currentCore(),1);
                if (exist_age==1){
                    amsdata->set_Inventory(inv2);
                    amsdata->AMSSave();
                    inv2->set_AgeModel(inv2->get_currentCore(),1);
                } else {
                    inv2->set_AgeModel(inv2->get_currentCore(),0);
                }


            }
            inv2->save();
            inv2->set_Selected(inv2->get_currentCore(),1);
        }
        update();
        QApplication::processEvents();
    }
    inv2->setShowError();
    emit(refresh());
    ui->plainTextEdit->appendPlainText("Imported all files. Close this window to return to the MainWindow. The new Files are already selected.");

}


