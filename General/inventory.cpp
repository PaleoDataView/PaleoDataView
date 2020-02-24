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

#include "inventory.h"

Inventory::Inventory(QString str){
    proxy=str;
    error_text.clear();
    entries=0;
    length=0;
    current_Core=-1;

    inv.clear();
    inv.reserve(access_inv_length());
    for (int i=0;i<access_inv_length();i++) {
        inv.append(new QStringList());
    }

    selected=new unsigned int[entries];
    var_AgeModel=new int [entries];



}

Inventory::~Inventory(){


    delete[] selected;

    delete[] var_AgeModel;


}

void Inventory::setShowError(){
    show_error=1;
}

void Inventory::noData(){
    current_Core=-1;
    flag_Data_OK=0;
}

// **************** Inventory
void Inventory::setProxyType(QString str){
    proxy=str;

    inv.clear();
    inv.reserve(access_inv_length());
    for (int i=0;i<access_inv_length();i++) {
        inv.append(new QStringList());
    }
    delete[] selected;

    delete[] var_AgeModel;
    //if (get_Entries()>0) for (int i=0;i<data_Comment.size();i++) delete data_Comment[i];

    error_text.clear();
    entries=0;
    length=0;
    current_Core=-1;

    for (int i=0;i<access_inv_length();i++) {
        for (int j=0;j<entries;j++){
            inv[i]->append("");
        }
    }
    selected=new unsigned int[entries];
    var_AgeModel=new int [entries];

    set_currentCore(-1);
    for (int i=0;i<get_Entries();i++) set_Selected(i,0);
}

void Inventory::read(){
    QString QFilename=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/inventory.nc";
    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << "Inventory : "+QFilename;


    // Initialize ids and meta
    int ncid;
    int varid[10];
    int dimid[4]={0};
    size_t dimlen[4]={0};

    int retval;// for Errors

    inv.clear();
    inv.reserve(access_inv_length());

    // Open the file
    if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){
        // read all data
        for (int i=0;i<access_inv_length();i++){
            // get length of variable
            entries=0;
            int entriesid;
            nc_inq_dimid(ncid,"record",&entriesid);
            nc_inq_dimlen(ncid,entriesid,&entries);
            qDebug() << "entries "+QString::number(entries);
            // create Stringlist
            inv.append(new QStringList());
            for (int j=0;j<entries;j++){
                inv[i]->append("");
            }

            // read strings
            if (access_inv_type(i).contains("string") && access_inv_netcdf(i).contains("dim=")){

                // get dim of string
                size_t dimlen=0;
                QString dimname="";
                int dimid;
                // search for dimname
                if (access_inv_netcdf(i).contains("dim=")){
                    QStringList l1=access_inv_netcdf(i).split(";");
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
                   ERR(retval);
                if ((retval = nc_inq_dimlen(ncid, dimid, &dimlen)))
                   ERR(retval);
                //read string
                unsigned char text[dimlen*entries];
                int varid;
                QString varname=access_inv_netcdf(i).split(";").at(0);
                qDebug()<<varname;
                if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) ERR(retval);
                if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);

                for (unsigned int ii=0;ii<entries;ii++){
                    QString str="";
                     for (unsigned int jj=0;jj<dimlen;jj++){
                         str.append(text[ii+entries*jj]);
                     }
                     inv[i]->replace(ii,str.simplified().toLatin1());
                qDebug()<<inv[i]->at(ii);
                }


            // read numbers
            } else {
                int varid;
                QString varname=access_inv_netcdf(i).split(";").at(0);
                qDebug()<<varname;
                if ((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))) ERR(retval);
                double* var=new double[entries];
                if ((retval = nc_get_var_double(ncid,varid,&var[0]))) ERR(retval);
                for (unsigned int ii=0;ii<entries;ii++){

                inv[i]->replace(ii,QString::number(var[ii]));
                qDebug()<<inv[i]->at(ii);
                }
            }

        }

    // create selected-Flag
    selected=new unsigned int[entries];
    for (unsigned int i=0;i<entries;i++) selected[i]=0;

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    } else {

        if (nc_strerror(retval)=="No such file or directory") {
            //qDebug() << "Start generating ...";
            entries=0;
            inv.clear();
            inv.reserve(access_inv_length());
            for (int i=0;i<access_inv_length();i++){
            inv.append(new QStringList());
            for (int j=0;j<entries;j++){
                inv[j]->append("");
            }
            }
            selected=new unsigned int[entries];

        } else {
            //qDebug() << "Start generating ...";
            entries=0;
            inv.clear();
            inv.reserve(access_inv_length());
            for (int i=0;i<access_inv_length();i++){
            inv.append(new QStringList());
            for (int j=0;j<entries;j++){
                inv[i]->append("");
            }
            }
            selected=new unsigned int[entries];

           // ERR(retval);
        }
    }
    // Check if AgeModel available
    var_AgeModel=new int [entries];
    for (int i=0;i<entries;i++){
        // get file name of age file
        QString name=get_Core(i).replace('\0',' ');
        name.simplified();
        QFilename.clear();
        QFilename=resources.path_age+name+".age";



        fname=QFilename.toStdString();
        delete[] filename;

        filename=new char[fname.size()+1];
        strcpy(filename,fname.c_str());
        //qDebug() << name;
        // Open the file
        if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){

            var_AgeModel[i]=1;
            // Close the file, freeing all resources.
            if ((retval = nc_close(ncid)))
               ERR(retval);
        } else {
            var_AgeModel[i]=0;
            //qDebug()<<"Not found";
            if (nc_strerror(retval)=="No such file or directory") {
                //qDebug() << "Age model not found...";
            } else {

               // ERR(retval);
            }
        }

    }

    delete[] filename;
}





void Inventory::save(){
    qDebug()<<"Saving data";
    // get file name
    QString QFilename=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/inventory.nc";
    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << "Save Inventory to :"+QFilename;


    // Initialize ids and meta
    int ncid;
    int retval;// for Errors

    // Create the file
    if ((retval = nc_create(filename, NC_WRITE,&ncid)))
       ERR(retval);

    // length of inventory
    int drows=0;
    if ((retval = nc_def_dim(ncid,"record",entries,&drows)))
       ERR(retval);
    //qDebug() << "entries"+QString::number(drows);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // define
    for (int i=0;i<access_inv_length();i++){
        if (access_inv_type(i).contains("string") && access_inv_netcdf(i).contains("dim=")){
            // begin definition mode
            if ((retval = nc_redef(ncid)))
                ERR(retval);

            // define dimension
            QString dimname="";
            // search for dimname
            if (access_inv_netcdf(i).contains("dim=")){
                QStringList l1=access_inv_netcdf(i).split(";");
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
            for (int ii=0;ii<entries;ii++) if (var_length<inv[i]->at(ii).length()) var_length=inv[i]->at(ii).length();

            int var=0;
            if ((retval = nc_def_dim(ncid,dimname.toLatin1(),var_length,&var)))
               ERR(retval);
            qDebug() << "length"+QString::number(var_length);


            // define variable
            int dim[2];
            dim[0]=drows;
            dim[1]=var;
            int varid;
            QString varname=access_inv_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_CHAR,2,dim,&varid)))
                ERR(retval);

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // write data
           unsigned char text[var_length][entries];
           for (int ii=0;ii<var_length;ii++) for(int jj=0;jj<entries;jj++) text[ii][jj]=' ';
            for (int ii=0;ii<entries;ii++){
                for (int jj=0;jj<inv[i]->at(ii).length();jj++) text[jj][ii]=inv[i]->at(ii).at(jj).toLatin1();
            }

            if ((retval = nc_put_var(ncid,varid,&text[0][0])))
                ERR(retval);
            qDebug() << access_inv_name(i)+" written";

        } else {
            // begin definition mode
            if ((retval = nc_redef(ncid)))
                ERR(retval);

            // define
            int dimm[1];
            dimm[0]=drows;
            int varid;
            QString varname=access_inv_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
                ERR(retval);

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // write data
            double data[entries];
            for (int ii=0;ii<entries;ii++) {
                QString str=inv[i]->at(ii);
                data[ii]=str.toDouble();

            }

            if ((retval = nc_put_var(ncid,varid,&data)))
                ERR(retval);
            qDebug() << access_inv_name(i)+" written";

        }
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
        ERR(retval);
    delete[] filename;

}

void Inventory::generate(){
    show_error=1;
    // get Entry from Directory
    QDir dir=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data";
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.nc"));
    QFileInfoList list = dir.entryInfoList();
    //qDebug() << QString::number(list.size());
    for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            //qDebug() << fileInfo.fileName();

            // Check if Filename is already in inventory
            int found=-1;
            for (int j=0;j<entries;j++){
                // find filename
                int fname=-1;
                for (fname=0;fname<access_inv_length();fname++) if (access_inv_netcdf(fname).contains("Filename")) break;
                if (inv[fname]->at(j).toLatin1()==fileInfo.fileName()){
                    //qDebug() << "Found";
                    found=j;
                    break;
                }
            }

            if (found>-1){
                // check if still consistent
                // read data
                // readData(found);


            }
            if (found==-1 && fileInfo.fileName()!="inventory.nc"){
                // read Data
                readData(fileInfo.fileName(),1);//Read only meta
                // create new Entry in Inventory
                addEntry();
                // enter information into last inventory
                for (int ii=0;ii<access_inv_length();ii++){
                    inv[ii]->replace(inv[ii]->size()-1,access_inv_meta(ii));
                }
                current_Core=inv[0]->size()-1;
                set_Filename(fileInfo.fileName());


                selected[entries-1]=0;

                // Check if AgeModel available


                // get file name of age file
                QString name=get_Core(entries-1).replace('\0',' ');
                name.replace(" ","");
                QString QFilename;
                QFilename.clear();
                QFilename=resources.path_age+name+".age";


                string fname=QFilename.toStdString();
                char* filename;
                filename=new char[fname.size()+1];
                strcpy(filename,fname.c_str());
                //qDebug() << name;
                // Open the file
                int retval,ncid;
                if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){

                    var_AgeModel[entries-1]=1;
                    // Close the file, freeing all resources.
                    if ((retval = nc_close(ncid)))
                       ERR(retval);
                } else {
                    var_AgeModel[entries-1]=0;
                    //qDebug()<<"Not found";
                    if (nc_strerror(retval)=="No such file or directory") {
                        //qDebug() << "Age model not found...";
                    } else {

                       // ERR(retval);
                    }
                }
                delete[] filename;
            }
            current_Core=-1;
    }

    // check if Inventory entries still exist
    for (int i=0;i<entries;i++){
        // get file name
        QString QFilename=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+this->get_Filename(i);

        char* filename;
        string fname=QFilename.toStdString();
        filename=new char[fname.size()+1];
        strcpy(filename,fname.c_str());
        //qDebug() << QFilename;

        // Initialize ids and meta
        int ncid;
        int retval;// for Errors

        // Open the file
        if ((retval = nc_open(filename, NC_NOWRITE, &ncid))){
            // Problem with File so delete it from inventory
            //qDebug()<<"delete :"+QString::number(i)+" of "+QString::number(entries);
            this->deleteEntry(i);
            i=-1;// Reset counter because of changing index(must be -1 because of inc at end of loop)
        } else {
        if ((retval = nc_close(ncid))) ERR(retval);
        }
        delete[] filename;

    }


    // save inventory
    if (entries>0) save();
    noData();
    show_error=1;
}

// for this to work the data needs to be in memory(do not trust the inventory)
bool Inventory::checkAgeFile(){
    int age=false;

    // Check if AgeModel available


    // get file name of age file
    QString name=get_att_Core().replace('\0',' ');
    name.replace(" ","");
    QString QFilename;
    QFilename.clear();
    QFilename=resources.path_age+name+".age";


    string fname=QFilename.toStdString();
    char* filename;
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << name;
    // Open the file
    int retval,ncid;
    if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){

        age=true;
        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);
    } else {
        age=false;
        //qDebug()<<"Not found";
        if (nc_strerror(retval)=="No such file or directory") {
            //qDebug() << "Age model not found...";
        } else {

           // ERR(retval);
        }
    }
    delete[] filename;

    return age;
}


void Inventory::addEntry(){

    // add inventory line
    if (entries>0){
        // save inventory data temporarilly

        unsigned int *temp_selected=new unsigned int[entries];
        int *temp_AgeModel=new int[entries];
        for (int i=0;i<entries;i++){           
            temp_selected[i]=selected[i];
            temp_AgeModel[i]=var_AgeModel[i];
        }
        // delete old data
        delete[] selected;
        delete[] var_AgeModel;


        // create new Inventory with old data
        for (int i=0;i<access_inv_length();i++){
            inv[i]->append("");
        }
        entries++;
        selected=new unsigned int[entries];
        var_AgeModel=new int [entries];
        for (int i=0;i<entries-1;i++){
            selected[i]=temp_selected[i];
            var_AgeModel[i]=temp_AgeModel[i];
        }
        delete[] temp_selected;
        delete[] temp_AgeModel;

    } else {
        delete[] selected;
        delete[] var_AgeModel;
        // create inventory if empty

       entries=1;

       inv.clear();
       inv.reserve(access_inv_length());
       for (int i=0;i<access_inv_length();i++){
           inv.append(new QStringList());

               inv[i]->append("");

       }

       selected=new unsigned int[entries];
       var_AgeModel=new int [entries];
    }
    qDebug()<<"adding Entry";
}

void Inventory::deleteEntry(int n){

    // delete inventory line
    if (entries>0){
        // save inventory data temporarilly

        unsigned int *temp_selected=new unsigned int[entries];
        int *temp_AgeModel=new int[entries];
        for (int i=0;i<entries;i++){
            temp_selected[i]=selected[i];
            temp_AgeModel[i]=var_AgeModel[i];
        }
        // delete old data
        delete[] selected;
        delete[] var_AgeModel;
        // create new Inventory with old data
        for (int i=0;i<access_inv_length();i++){
            inv[i]->removeAt(n);
        }
        entries--;

        selected=new unsigned int[entries];
        var_AgeModel=new int [entries];

        int count=0;
        for (int i=0;i<=entries;i++){
            if (i!=n){
                selected[count]=temp_selected[i];
                var_AgeModel[count]=temp_AgeModel[i];
                count++;
            }
        }
        delete[] temp_selected;
        delete[] temp_AgeModel;

    } else {
        delete[] selected;
        delete[] var_AgeModel;
        // create inventory if empty

       entries=0;

       inv.clear();
       inv.reserve(access_inv_length());
       /*
       for (int i=0;i<access_inv_length();i++){
           inv.append(new QStringList());
           for (int j=0;j<entries;j++){
               inv[j]->append("");
           }
       }*/

       selected=new unsigned int[entries];
       var_AgeModel=new int [entries];
    }

}



QString Inventory::get_Core(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Core Name")) break;
    return inv[i]->at(n);
}

void Inventory::set_Core(QString str){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Core Name")) break;
    inv[i]->replace(current_Core,str);
}

double Inventory::get_Latitude(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Latitude")) break;
    return inv[i]->at(n).toFloat();
}

void Inventory::set_Latitude(double v){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Latitude")) break;
    inv[i]->replace(current_Core,QString::number(v));
}

double Inventory::get_Longitude(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Longitude")) break;
    return inv[i]->at(n).toFloat();
}

void Inventory::set_Longitude(double v){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Longitude")) break;
    inv[i]->replace(current_Core,QString::number(v));

}

QString Inventory::get_Filename(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Filename")) break;
    return inv[i]->at(n);
}

void Inventory::set_Filename(QString str){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Filename")) break;
    if(i!=access_inv_length())inv[i]->replace(current_Core,str);
}

QString Inventory::get_Species(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Species")||access_inv_name(i).contains("Parameter")||access_inv_name(i).contains("Component")) break;
    return inv[i]->at(n);
}

void Inventory::set_Species(QString str){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Species")||access_inv_name(i).contains("Parameter")||access_inv_name(i).contains("Component")) break;
    inv[i]->replace(current_Core,str);

}

int Inventory::get_Basin(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Basin")) break;
    return inv[i]->at(n).toInt();
}

void Inventory::set_Basin(int b){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Basin")) break;
    inv[i]->replace(current_Core,QString::number(b));
}

double Inventory::get_Water_Depth(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Water Depth")) break;
    return inv[i]->at(n).toFloat();
}

void Inventory::set_Water_Depth(double v){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Water Depth")) break;
    inv[i]->replace(current_Core,QString::number(v));

}

int Inventory::get_Oxygen_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Oxygen Use Flag")) break;
    return inv[i]->at(n).toInt();
}

void Inventory::set_O_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Oxygen Use Flag")) break;
    inv[i]->replace(current_Core,QString::number(n));
}

int Inventory::get_Carbon_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Carbon Use Flag")) break;
    return inv[i]->at(n).toInt();
}

void Inventory::set_C_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Carbon Use Flag")) break;
    inv[i]->replace(current_Core,QString::number(n));
}

int Inventory::get_SST_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("SST Use Flag")) break;
    return inv[i]->at(n).toInt();
}

void Inventory::set_SST_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("SST Use Flag")) break;
    inv[i]->replace(current_Core,QString::number(n));
}

QString Inventory::get_Record_Type(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Record Type")) break;
    return inv[i]->at(n);
}

void Inventory::set_Record_Type(QString str){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).contains("Record Type")) break;
    inv[i]->replace(current_Core,str);

}
int Inventory::get_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).toLatin1()=="Use Flag") break;
    return inv[i]->at(n).toInt();
}

void Inventory::set_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_inv_length();i++) if (access_inv_name(i).toLatin1()=="Use Flag") break;
    inv[i]->replace(current_Core,QString::number(n));
}


unsigned int Inventory::get_Entries(){
    return entries;
}

unsigned int Inventory::get_Selected(int n){
    return selected[n];
}

unsigned int Inventory::get_Selected_Sum(){
    int sum=0;
    for (int i=0;i<entries;i++) sum+=selected[i];
    return sum;
}

void Inventory::invert_Selected(int n){
    if (selected[n]){
        selected[n]=0;
    } else {
        selected[n]=1;
    }

}

void Inventory::set_Selected(int n,int m){
    selected[n]=m;

}

int Inventory::get_AgeModel(int n){
    return var_AgeModel[n];
}

void Inventory::set_AgeModel(int n,int i){
    var_AgeModel[n]=i;
}

//*************  Data
void Inventory::readData(int n){
    flag_Data_OK=0;
    if (n>=0){
    // get file name
    QString QFilename=this->get_Filename(n);
    current_Core=n;
    readData(QFilename);
    }




}

void Inventory::readData(QString Filename){
    readData(Filename,0);
}

void Inventory::readData(QString Filename,int onlyMeta){



    error_text.clear();
    QString action="Reading Proxy";

    flag_Data_OK=0;

    // get file name
    QString QFilename=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+Filename;

    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    qDebug() << QFilename;
    // first check if the file is still present
    QFile* file=new QFile(resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+Filename);
    if (file->exists()){


    // Initialize ids and meta
    int ncid;
    /*
    int n = 0;
    if (proxy=="Foraminiferal Isotopes") n=11;
    if (proxy=="Foraminiferal MgCa") n=13;
    int varid[n];
    int dimid[2]={0};
    size_t dimlen[2]={0};
    size_t attlen[22]={0};*/


    int retval;// for Errors

    // Open the file
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       NetCDF_Error(filename,action,retval);

    meta.clear();

    for (int i=0;i<access_meta_length();i++){
        if (access_meta_type(i).contains("string") || access_meta_type(i).contains("combo") ) {
           if (!access_meta_netcdf(i).contains("ignore")){
                QString att_name=access_meta_netcdf(i).split(";").at(0);
                size_t attlen;
                 QString err=filename;
                if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,att_name.toLatin1(),&attlen)))
                   NetCDF_Error(err+" : "+att_name,action,retval);
                qDebug()<<QString::number(attlen);
                if(attlen>99999) attlen=0;
               unsigned char text[attlen+1];
               for (int ii=0;ii<attlen+1;ii++) text[ii]=' ';
                if ((retval = nc_get_att(ncid,NC_GLOBAL,att_name.toLatin1(),&text[0])))
                   NetCDF_Error(err+" : "+att_name,action,retval);
                QString str="";
                for (unsigned int j=0;j<attlen;j++){
                        str.append(text[j]);
                }
                meta.append(str);
                qDebug() << str;
           } else {
               meta.append("");
           }
        } else {
            if (!access_meta_netcdf(i).contains("ignore")){
                QString att_name=access_meta_netcdf(i).split(";").at(0);
                double att=0;
                QString err=filename;
                if ((retval = nc_get_att_double(ncid,NC_GLOBAL,att_name.toLatin1(),&att)))
                   NetCDF_Error(err+" : "+att_name,action,retval);
                meta.append(QString::number(att));
                qDebug() << QString::number(att);
            } else {
                meta.append("");
            }
        }
    }


    // -------------------- READ ATTRIBUTES -------------------------------


    //----------------- read data ---------------------------------
    // get dims




    //qDebug()<<QString::number(dimlen[0]);
    //qDebug()<<QString::number(dimlen[1]);

    // read all data
    if (onlyMeta==0){
    for (int i=0;i<access_data_length();i++){
        // get length of variable
        length=0;
        size_t l=0;
        int lengthid;
        nc_inq_dimid(ncid,"Length",&lengthid);
        nc_inq_dimlen(ncid,lengthid,&l);
        length=l;
        qDebug() << "length "+QString::number(length);
        // create Stringlist
        data.append(new QStringList());
        for (int j=0;j<length;j++){
            data[i]->append("");
        }

        // read strings
        if (access_data_type(i).contains("string") && access_data_netcdf(i).contains("dim=")){

            // get dim of string
            size_t dimlen=0;
            QString dimname="";
            int dimid;
            // search for dimname
            if (access_data_netcdf(i).contains("dim=")){
                QStringList l1=access_data_netcdf(i).split(";");
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
            unsigned char text[dimlen*length];
            int varid;
            QString varname=access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            // try to read , try any netcdfname befor giving an error
            int count=0;
            while((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))){
                if (count==access_data_netcdf(i).split(";").length()){
                    NetCDF_Error(filename,action+":"+access_data_netcdf(i).split(";").at(0),retval);
                    break;
                }else{
                    count++;
                    varname=access_data_netcdf(i).split(";").at(count);
                }

            }




            if ((retval = nc_get_var(ncid,varid,&text[0]))) NetCDF_Error(filename,action+":"+access_data_netcdf(i).split(";").at(0),retval);

            for (unsigned int ii=0;ii<length;ii++){
                QString str="";
                 for (unsigned int jj=0;jj<dimlen;jj++){
                     str.append(text[ii+length*jj]);
                 }
                 data[i]->replace(ii,str.simplified().toLatin1());
            //qDebug()<<data[i]->at(ii).toLatin1();
            }


        // read numbers
        } else {
            int varid;
            QString varname=access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            // try to read , try any netcdfname befor giving an error
            int count=0;
            while((retval = nc_inq_varid(ncid,varname.toLatin1() , &varid))){
                if (count==access_data_netcdf(i).split(";").length()){
                    NetCDF_Error(filename,action+":"+access_data_netcdf(i).split(";").at(0),retval);
                    break;
                }else{
                    count++;
                    varname=access_data_netcdf(i).split(";").at(count);
                }

            }

            double* var=new double[length];
            if ((retval = nc_get_var_double(ncid,varid,&var[0]))) NetCDF_Error(filename,action+":"+access_data_netcdf(i).split(";").at(0),retval);
            for (unsigned int ii=0;ii<length;ii++){

            data[i]->replace(ii,QString::number(var[ii]));
            //qDebug()<<data[i]->at(ii).toLatin1();
            }
        }

    }
    }



    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       NetCDF_Error(filename,action,retval);
    flag_Data_OK=1;

    if (error_text!=""&& show_error==1) NetCDF_Error_Message();
    } else {
        QMessageBox msgBox;
                msgBox.setText("The file was not found. Please refresh database.");

                msgBox.setInformativeText(QFilename);
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
                flag_Data_OK=0;

    }
    delete[] filename;
}

void Inventory::NetCDF_Error(QString filename,QString action,int value){
    error_text.append(filename+" : "+action+" : "+nc_strerror(value)+" : "+QString::number(value)+"\n");

}

void Inventory::NetCDF_Error_Message(){
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

void Inventory::newIsotope(int isolength){

    length=isolength;

    data.clear();
    data.reserve(access_data_length());
    for (int i=0;i<access_data_length();i++) {
        data.append(new QStringList());
        for (int j=0;j<length;j++){
            data[i]->append("");
        }
    }



}

void Inventory::newIsotopeMeta(){
    flag_Data_OK=1;

    meta.clear();
    for (int i=0;i<access_meta_length();i++) {
        if (access_meta_type(i).contains("default=")){
            QStringList str=access_meta_type(i).split(";");
            for (int j=0;j<str.length();j++){
                if (str[j].contains("default=")){
                    QString value=str[j].remove(0,8);
                    meta.append(value);
                }
            }
        } else {
            if (access_meta_type(i).contains("string")||access_meta_type(i).contains("combo")) meta.append("na"); else meta.append("NAN");

        }
    }

}

void Inventory::addIsotope(){


    length++;
    for (int i=0;i<access_data_length();i++) {
        if (access_data_type(i).contains("string")) data[i]->append("na"); else data[i]->append("NAN");
    }
}

int Inventory::get_flag_Data_OK(){
    return flag_Data_OK;
}

void Inventory::set_flag_Data_OK(int n){
    flag_Data_OK=n;
}

// Old Style
// 0 : Depth
// 1 : Age
// 2 : C13
// 3 : O14
// 4 : C13 Corrected
// 5 : O14 Corrected
// 6 : Flag
double Inventory::get_data_Isotopes(int i,int j){
    double value=NAN;
    if (j<length){
        if (i==0) value=get_data_Depth(j);
        if (i==1) value=get_data_Age(j);
        if (i==2) value=get_data_d13C(j);
        if (i==3) value=get_data_d18O(j);
        if (i==4) value=get_data_d13C_Corr(j);
        if (i==5) value=get_data_d18O_Corr(j);
        if (i==6) value=get_data_Use_Flag(j);
    }
    return value;
}

void Inventory::set_data_Isotopes(int i,int j,double v){
    if (j<length){
        if (i==0) set_data_Depth(v,j);
        if (i==1) set_data_Age(v,j);
        if (i==2) set_data_d13C(v,j);
        if (i==3) set_data_d18O(v,j);
        if (i==4) set_data_d13C_Corr(v,j);
        if (i==5) set_data_d18O_Corr(v,j);
        if (i==6) set_data_Use_Flag((int)v,j);
    }
}

// Data strukture for different proxies +++++++++++++++++++++++++++++++++++++++++++++

int Inventory::access_proxy_length(){
    return 5;
}

QString Inventory::access_proxy_name(int type){
    QString str="";
    if (type>=0 && type<access_proxy_length()) {
        if (type==0)  str="Foraminiferal Isotopes";
        if (type==1)  str="Foraminiferal MgCa";
        if (type==2)  str="Alkenones";
        if (type==3)  str="XRF";
        if (type==4)  str="Miscellaneous";

        if (type==5)  str="Age Model";
    }
    return str;
}

QString Inventory::access_proxy_name(){
    return proxy;
}

QString Inventory::access_proxy_abbreviation(){
    QString str="";
        if (proxy=="Foraminiferal Isotopes")  str="FIs";
        if (proxy=="Foraminiferal MgCa")  str="FMC";
        if (proxy=="Alkenones")  str="Alk";
        if (proxy=="XRF")  str="XRF";
        if (proxy=="Miscellaneous") str="Misc";

        //if (proxy=="Age Model") str="Age";
    return str;
}




QString Inventory::access_proxy_type(int type){
    QString str="";
    if (type>=0 && type<access_proxy_length()) {
        if (type==0)  str="";
        if (type==1)  str="CalibrateSST";
        if (type==2)  str="CalibrateSST";
        if (type==3)  str="";
        if (type==4)  str="";

        //if (type==5)  str="";
    }
    return str;
}

QString Inventory::access_proxy_target(){
    QString QFilename="";
    QFilename=resources.path_target+"target_"+proxy+".nc";
    //if (proxy=="Foraminiferal Isotopes") QFilename=resources.path_target+"targets_Isotopes.nc";
    //if (proxy=="Foraminiferal MgCa") QFilename=resources.path_target+"targets_Isotopes.nc";
    //if (proxy=="Alkenones") QFilename=resources.path_target+"targets_Isotopes.nc";
    //if (proxy=="Miscellaneous") QFilename=resources.path_target+"targets_Isotopes.nc";
    return QFilename;
}

QString Inventory::access_proxy_folder(){
    QString str="";
    if (proxy=="Foraminiferal Isotopes")  str=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/";
    if (proxy=="Foraminiferal MgCa")  str=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/";
    if (proxy=="Alkenones")  str=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/";
    if (proxy=="XRF")  str=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/";
    if (proxy=="Miscellaneous") str=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/";

    //if (proxy=="Age Model") str=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/Age/";
    return str;
}


int Inventory::access_data_length(){
    int length=0;
    if (proxy=="Foraminiferal Isotopes") length=11;
    if (proxy=="Foraminiferal MgCa") length=9;
    if (proxy=="Alkenones") length=12;
    if (proxy=="XRF") length=21;
    if (proxy=="Miscellaneous") length=13;

    //if (proxy=="Age Model") length=0;
    return length;
}

QString Inventory::access_data_value(int type,int row){
    QString value="NAN";
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Foraminiferal Isotopes"){
        if (type==0) value=QString::number(get_data_Depth(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value=QString::number(get_data_Age(row));
        if (type==3) value=QString::number(get_data_d13C(row));
        if (type==4) value=QString::number(get_data_d18O(row));
        if (type==5) value=QString::number(get_data_d13C_Err(row));
        if (type==6) value=QString::number(get_data_d18O_Err(row));
        if (type==7) value=QString::number(get_data_d13C_Corr(row));
        if (type==8) value=QString::number(get_data_d18O_Corr(row));
        if (type==9) value=QString::number(get_data_Use_Flag(row));
        if (type==10) value=get_data_Comment(row).toLatin1();
    }
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Foraminiferal MgCa"){
        if (type==0) value=QString::number(get_data_Depth(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value=QString::number(get_data_Age(row));
        //if (type==3) value=QString::number(get_data_Mg(row));
        //if (type==4) value=QString::number(get_data_Mg_Err(row));
        //if (type==5) value=QString::number(get_data_Ca(row));
        //if (type==6) value=QString::number(get_data_Ca_Err(row));
        if (type==3) value=QString::number(get_data_MgCa(row));
        if (type==4) value=QString::number(get_data_MgCa_Err(row));
        if (type==5) value=QString::number(get_data_SST(row));
        if (type==6) value=QString::number(get_data_SST_Err(row));
        if (type==7) value=QString::number(get_data_Use_Flag(row));
        if (type==8) value=get_data_Comment(row).toLatin1();
    }
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Alkenones"){
        if (type==0) value=QString::number(get_data_Depth(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value=QString::number(get_data_Age(row));
        if (type==3) value=QString::number(get_data_C37_3(row));
        if (type==4) value=QString::number(get_data_C37_2(row));
        if (type==5) value=QString::number(get_data_C37_3_Err(row));
        if (type==6) value=QString::number(get_data_C37_2_Err(row));
        if (type==7) value=QString::number(get_data_UK37(row));
        if (type==8) value=QString::number(get_data_SST(row));
        if (type==9) value=QString::number(get_data_SST_Err(row));
        if (type==10) value=QString::number(get_data_Use_Flag(row));
        if (type==11) value=get_data_Comment(row).toLatin1();
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="XRF"){
        if (type==0) value=QString::number(get_data_Depth(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value=QString::number(get_data_Age(row));
        if (type==3) value=QString::number(get_data_XRF_Al(row));
        if (type==4) value=QString::number(get_data_XRF_Si(row));
        if (type==5) value=QString::number(get_data_XRF_P(row));
        if (type==6) value=QString::number(get_data_XRF_S(row));
        if (type==7) value=QString::number(get_data_XRF_Cl(row));
        if (type==8) value=QString::number(get_data_XRF_K(row));
        if (type==9) value=QString::number(get_data_XRF_Ca(row));
        if (type==10) value=QString::number(get_data_XRF_Ti(row));
        if (type==11) value=QString::number(get_data_XRF_Cr(row));
        if (type==12) value=QString::number(get_data_XRF_Mn(row));
        if (type==13) value=QString::number(get_data_XRF_Fe(row));
        if (type==14) value=QString::number(get_data_XRF_Rh(row));
        if (type==15) value=QString::number(get_data_XRF_Al_Si(row));
        if (type==16) value=QString::number(get_data_XRF_Fe_K(row));
        if (type==17) value=QString::number(get_data_XRF_Fe_Ca(row));
        if (type==18) value=QString::number(get_data_XRF_Al_Ca(row));
        if (type==19) value=QString::number(get_data_Use_Flag(row));
        if (type==20) value=get_data_Comment(row).toLatin1();
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Miscellaneous"){
        if (type==0) value=QString::number(get_data_Depth(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value=QString::number(get_data_Age(row));
        if (type==3) value=QString::number(get_data_Parameter1(row));
        if (type==4) value=QString::number(get_data_Parameter1_Err(row));
        if (type==5) value=QString::number(get_data_Parameter2(row));
        if (type==6) value=QString::number(get_data_Parameter2_Err(row));
        if (type==7) value=QString::number(get_data_Parameter1_Cal(row));
        if (type==8) value=QString::number(get_data_Parameter1_Cal_Err(row));
        if (type==9) value=QString::number(get_data_Parameter2_Cal(row));
        if (type==10) value=QString::number(get_data_Parameter2_Cal_Err(row));
        if (type==11) value=QString::number(get_data_Use_Flag(row));
        if (type==12) value=get_data_Comment(row).toLatin1();
    }
    return value;
}

void Inventory::write_data_value(int type,int row,QString str){
    for (int i=0;i<str.size();i++){
        if (str.at(i)>QChar(255) ) str.replace(i,1,"_");
    }
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Foraminiferal Isotopes"){
        if (type==0) set_data_Depth(str.toFloat(),row);
        if (type==1) set_data_Sample_Thickness(str.toFloat(),row);
        if (type==2) set_data_Age(str.toFloat(),row);
        if (type==3) set_data_d13C(str.toFloat(),row);
        if (type==4) set_data_d18O(str.toFloat(),row);
        if (type==5) set_data_d13C_Err(str.toFloat(),row);
        if (type==6) set_data_d18O_Err(str.toFloat(),row);
        if (type==7) set_data_d13C_Corr(str.toFloat(),row);
        if (type==8) set_data_d18O_Corr(str.toFloat(),row);
        if (type==9) set_data_Use_Flag(str.toInt(),row);
        if (type==10) {
            set_data_Comment(str,row);
        }
    }
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Foraminiferal MgCa"){
        if (type==0) set_data_Depth(str.toFloat(),row);
        if (type==1) set_data_Sample_Thickness(str.toFloat(),row);
        if (type==2) set_data_Age(str.toFloat(),row);
        //if (type==3) set_data_Mg(str.toFloat(),row);
        //if (type==4) set_data_Mg_Err(str.toFloat(),row);
        //if (type==5) set_data_Ca(str.toFloat(),row);
        //if (type==6) set_data_Ca_Err(str.toFloat(),row);
        if (type==3) set_data_MgCa(str.toFloat(),row);
        if (type==4) set_data_MgCa_Err(str.toFloat(),row);
        if (type==5) set_data_SST(str.toFloat(),row);
        if (type==6) set_data_SST_Err(str.toFloat(),row);
        if (type==7) set_data_Use_Flag(str.toInt(),row);
        if (type==8) {
            set_data_Comment(str,row);
        }
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Alkenones"){
        if (type==0) set_data_Depth(str.toFloat(),row);
        if (type==1) set_data_Sample_Thickness(str.toFloat(),row);
        if (type==2) set_data_Age(str.toFloat(),row);
        if (type==3) set_data_C37_3(str.toFloat(),row);
        if (type==4) set_data_C37_2(str.toFloat(),row);
        if (type==5) set_data_C37_3_Err(str.toFloat(),row);
        if (type==6) set_data_C37_2_Err(str.toFloat(),row);
        if (type==7) set_data_UK37(str.toFloat(),row);
        if (type==8) set_data_SST(str.toFloat(),row);
        if (type==9) set_data_SST_Err(str.toFloat(),row);
        if (type==10) set_data_Use_Flag(str.toInt(),row);
        if (type==11) {
            set_data_Comment(str,row);
        }
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="XRF"){
        if (type==0) set_data_Depth(str.toFloat(),row);
        if (type==1) set_data_Sample_Thickness(str.toFloat(),row);
        if (type==2) set_data_Age(str.toFloat(),row);
        if (type==3) set_data_XRF_Al(str.toInt(),row);
        if (type==4) set_data_XRF_Si(str.toInt(),row);
        if (type==5) set_data_XRF_P(str.toInt(),row);
        if (type==6) set_data_XRF_S(str.toInt(),row);
        if (type==7) set_data_XRF_Cl(str.toInt(),row);
        if (type==8) set_data_XRF_K(str.toInt(),row);
        if (type==9) set_data_XRF_Ca(str.toInt(),row);
        if (type==10) set_data_XRF_Ti(str.toInt(),row);
        if (type==11) set_data_XRF_Cr(str.toInt(),row);
        if (type==12) set_data_XRF_Mn(str.toInt(),row);
        if (type==13) set_data_XRF_Fe(str.toInt(),row);
        if (type==14) set_data_XRF_Rh(str.toInt(),row);
        if (type==15) set_data_XRF_Al_Si(str.toFloat(),row);
        if (type==16) set_data_XRF_Fe_K(str.toFloat(),row);
        if (type==17) set_data_XRF_Fe_Ca(str.toFloat(),row);
        if (type==18) set_data_XRF_Al_Ca(str.toFloat(),row);
        if (type==19) set_data_Use_Flag(str.toInt(),row);
        if (type==20) {
            set_data_Comment(str,row);
        }
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Miscellaneous"){
        if (type==0) set_data_Depth(str.toFloat(),row);
        if (type==1) set_data_Sample_Thickness(str.toFloat(),row);
        if (type==2) set_data_Age(str.toFloat(),row);
        if (type==3) set_data_Parameter1(str.toFloat(),row);
        if (type==4) set_data_Parameter1_Err(str.toFloat(),row);
        if (type==5) set_data_Parameter2(str.toFloat(),row);
        if (type==6) set_data_Parameter2_Err(str.toFloat(),row);
        if (type==7) set_data_Parameter1_Cal(str.toFloat(),row);
        if (type==8) set_data_Parameter1_Cal_Err(str.toFloat(),row);
        if (type==9) set_data_Parameter2_Cal(str.toFloat(),row);
        if (type==10) set_data_Parameter2_Cal_Err(str.toFloat(),row);
        if (type==11) set_data_Use_Flag(str.toInt(),row);
        if (type==12) {
            set_data_Comment(str,row);
        }
    }

}

QString Inventory::access_data_error(int type,int row){
    QString value="NAN";
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Foraminiferal Isotopes"){
        if (type==0) value=QString::number(get_data_Sample_Thickness(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value="0.1";
        if (type==3) value=QString::number(get_data_d13C_Err(row));
        if (type==4) value=QString::number(get_data_d18O_Err(row));
        if (type==5) value=QString::number(get_data_d13C_Err(row));
        if (type==6) value=QString::number(get_data_d18O_Err(row));
        if (type==7) value=QString::number(get_data_d13C_Err(row));
        if (type==8) value=QString::number(get_data_d18O_Err(row));
        if (type==9) value="0";
        if (type==10) value="0";
    }
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Foraminiferal MgCa"){
        if (type==0) value=QString::number(get_data_Sample_Thickness(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value="0.1";
        //if (type==3) value=QString::number(get_data_Mg_Err(row));
        //if (type==4) value=QString::number(get_data_Mg_Err(row));
        //if (type==5) value=QString::number(get_data_Ca_Err(row));
        //if (type==6) value=QString::number(get_data_Ca_Err(row));
        if (type==3) value=QString::number(get_data_MgCa_Err(row));
        if (type==4) value=QString::number(get_data_MgCa_Err(row));
        if (type==5) value=QString::number(get_data_SST_Err(row));
        if (type==6) value=QString::number(get_data_SST_Err(row));
        if (type==7) value="0";
        if (type==8) value="0";
    }
    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Alkenones"){
        if (type==0) value=QString::number(get_data_Sample_Thickness(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value="0.1";
        if (type==3) value=QString::number(get_data_C37_3_Err(row));
        if (type==4) value=QString::number(get_data_C37_2_Err(row));
        if (type==5) value=QString::number(get_data_C37_3_Err(row));
        if (type==6) value=QString::number(get_data_C37_2_Err(row));
        if (type==7) value="0";
        if (type==8) value=QString::number(get_data_SST_Err(row));
        if (type==9) value=QString::number(get_data_SST_Err(row));
        if (type==10) value="0";
        if (type==11) value="0";
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="XRF"){
        if (type==0) value=QString::number(get_data_Sample_Thickness(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value="0.1";
        if (type==3) value="0";
        if (type==4) value="0";
        if (type==5) value="0";
        if (type==6) value="0";
        if (type==7) value="0";
        if (type==8) value="0";
        if (type==9) value="0";
        if (type==10) value="0";
        if (type==11) value="0";
        if (type==12) value="0";
        if (type==13) value="0";
        if (type==14) value="0";
        if (type==15) value="0";
        if (type==16) value="0";
        if (type==17) value="0";
        if (type==18) value="0";
        if (type==19) value="0";
        if (type==20) value="0";
    }

    if (type>=0 && type<access_data_length() && row>=0 && row<length && proxy=="Miscellaneous"){
        if (type==0) value=QString::number(get_data_Sample_Thickness(row));
        if (type==1) value=QString::number(get_data_Sample_Thickness(row));
        if (type==2) value="0.1";
        if (type==3) value=QString::number(get_data_Parameter1_Err(row));
        if (type==4) value=QString::number(get_data_Parameter1_Err(row));
        if (type==5) value=QString::number(get_data_Parameter2_Err(row));
        if (type==6) value=QString::number(get_data_Parameter2_Err(row));
        if (type==7) value=QString::number(get_data_Parameter1_Cal_Err(row));
        if (type==8) value=QString::number(get_data_Parameter1_Cal_Err(row));
        if (type==9) value=QString::number(get_data_Parameter2_Cal_Err(row));
        if (type==10) value=QString::number(get_data_Parameter2_Cal_Err(row));
        if (type==11) value="0";
        if (type==12) value="0";
    }
    return value;
}

QString Inventory::access_data_name(int type){
    QString str="";
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="d13C";
        if (type==4) str="d18O";
        if (type==5) str="d13C Error";
        if (type==6) str="d18O Error";
        if (type==7) str="d13C Corr";
        if (type==8) str="d18O Corr";
        if (type==9) str="Use Flag";
        if (type==10) str="Comment";
    }
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        //if (type==3) str="Mg";
        //if (type==4) str="Mg Error";
        //if (type==5) str="Ca";
        //if (type==6) str="Ca Error";
        if (type==3) str="Mg/Ca";
        if (type==4) str="Mg/Ca Error";
        if (type==5) str="SST";
        if (type==6) str="SST Error";
        if (type==7) str="Use Flag";
        if (type==8) str="Comment";
    }
    if (type>=0 && type<access_data_length() && proxy=="Alkenones"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="C37_3";
        if (type==4) str="C37_2";
        if (type==5) str="C37_3 Error";
        if (type==6) str="C37_2 Error";
        if (type==7) str="UK'37";
        if (type==8) str="SST";
        if (type==9) str="SST Error";
        if (type==10) str="Use Flag";
        if (type==11) str="Comment";
    }

    if (type>=0 && type<access_data_length() && proxy=="XRF"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="Al";
        if (type==4) str="Si";
        if (type==5) str="P";
        if (type==6) str="S";
        if (type==7) str="Cl";
        if (type==8) str="K";
        if (type==9) str="Ca";
        if (type==10) str="Ti";
        if (type==11) str="Cr";
        if (type==12) str="Mn";
        if (type==13) str="Fe";
        if (type==14) str="Rh";
        if (type==15) str="Al/Si";
        if (type==16) str="Fe/K";
        if (type==17) str="Fe/Ca";
        if (type==18) str="Al/Ca";
        if (type==19) str="Use Flag";
        if (type==20) str="Comment";
    }

    if (type>=0 && type<access_data_length() && proxy=="Miscellaneous"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="Parameter 1";
        if (type==4) str="Parameter 1 Error";
        if (type==5) str="Parameter 2";
        if (type==6) str="Parameter 2 Error";
        if (type==7) str="Parameter 1 Corr";
        if (type==8) str="Parameter 1 Corr Error";
        if (type==9) str="Parameter 2 Corr";
        if (type==10) str="Parameter 2 Corr Error";
        if (type==11) str="Use Flag";
        if (type==12) str="Comment";
    }
    return str;
}

QString Inventory::access_data_unit(int type){
    QString str="";
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="m";
        if (type==1) str="m";
        if (type==2) str="ka BP";
        if (type==3) str="per mil PDB";
        if (type==4) str="per mil PDB";
        if (type==5) str="per mil PDB";
        if (type==6) str="per mil PDB";
        if (type==7) str="per mil PDB";
        if (type==8) str="per mil PDB";
        if (type==9) str="";
        if (type==10) str="";
    }
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="m";
        if (type==1) str="m";
        if (type==2) str="ka BP";
        if (type==3) str="mmol/mol";
        if (type==4) str="";
        if (type==5) str="°C";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
    }
    if (type>=0 && type<access_data_length() && proxy=="Alkenones"){
        if (type==0) str="m";
        if (type==1) str="m";
        if (type==2) str="ka BP";
        if (type==3) str="ng/g";
        if (type==4) str="ng/g";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="°C";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
    }
    if (type>=0 && type<access_data_length() && proxy=="XRF"){
        if (type==0) str="m";
        if (type==1) str="m";
        if (type==2) str="ka BP";
        if (type==3) str="counts";
        if (type==4) str="counts";
        if (type==5) str="counts";
        if (type==6) str="counts";
        if (type==7) str="counts";
        if (type==8) str="counts";
        if (type==9) str="counts";
        if (type==10) str="counts";
        if (type==11) str="counts";
        if (type==12) str="counts";
        if (type==13) str="counts";
        if (type==14) str="counts";
        if (type==15) str="counts/counts";
        if (type==16) str="counts/counts";
        if (type==17) str="counts/counts";
        if (type==18) str="counts/counts";
        if (type==19) str="";
        if (type==20) str="";
    }
    if (type>=0 && type<access_data_length() && proxy=="Miscellaneous"){
        if (type==0) str="m";
        if (type==1) str="m";
        if (type==2) str="ka BP";
        if (type==3) str="";
        if (type==4) str="";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
        if (type==12) str="";
    }
    return str;
}

QString Inventory::access_data_type(int type){
    QString str="";
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="float";
        if (type==1) str="float";
        if (type==2) str="float";
        if (type==3) str="float;comment=10;dFlag=9;mFlag=13;error=5";
        if (type==4) str="float;comment=10;dFlag=9;multy=-1;mFlag=14;error=6";
        if (type==5) str="float";
        if (type==6) str="float";
        if (type==7) str="float;plot2;comment=10;dFlag=9;mFlag=13;Comp;error=5";
        if (type==8) str="float;plot1;comment=10;dFlag=9;multy=-1;mFlag=14;Correlate;Comp;error=6";
        if (type==9) str="flag;edit";
        if (type==10) str="string;edit";
    }
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="float;check=asis";
        if (type==1) str="float;check=asis";
        if (type==2) str="float;check=mean";
        if (type==3) str="float;plot1;comment=8;dFlag=7;mFlag=13;Comp;SST=Source;error=4;check=mean";
        if (type==4) str="float;check=error=3";
        if (type==5) str="float;plot2;comment=8;dFlag=7;mFlag=13;Correlate;Comp;SST=Target;error=6;check=mean";
        if (type==6) str="float;check=error=5";
        if (type==7) str="flag;edit;check=asis";
        if (type==8) str="string;edit;check=asis";
    }
    if (type>=0 && type<access_data_length() && proxy=="Alkenones"){
        if (type==0) str="float;check=asis";
        if (type==1) str="float;check=asis";
        if (type==2) str="float;check=mean";
        if (type==3) str="float;check=mean;comment=11;dFlag=10;mFlag=13;error=5";
        if (type==4) str="float;check=mean;comment=11;dFlag=10;mFlag=13;error=6";
        if (type==5) str="float;check=error=3";
        if (type==6) str="float;check=error=4";
        if (type==7) str="float;plot1;comment=11;dFlag=10;mFlag=13;Comp;check=mean;SST=Source";
        if (type==8) str="float;plot2;comment=11;dFlag=10;mFlag=13;Correlate;Comp;check=mean;SST=Target;error=9";
        if (type==9) str="float;check=error=8";
        if (type==10) str="flag;edit;check=asis";
        if (type==11) str="string;edit;check=asis";
    }

    if (type>=0 && type<access_data_length() && proxy=="XRF"){
        if (type==0) str="float;check=asis";
        if (type==1) str="float;check=asis";
        if (type==2) str="float;check=mean";
        if (type==3) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==4) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==5) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==6) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==7) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==8) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==9) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==10) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==11) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==12) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==13) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==14) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==15) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==16) str="float;plot1;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==17) str="float;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==18) str="float;plot2;Correlate;check=mean;comment=20;dFlag=19;mFlag=13;Comp";
        if (type==19) str="flag;edit;check=asis";
        if (type==20) str="string;edit;check=asis";
    }

    if (type>=0 && type<access_data_length() && proxy=="Miscellaneous"){
        if (type==0) str="float";
        if (type==1) str="float";
        if (type==2) str="float";
        if (type==3) str="float;plot1;comment=12;dFlag=11;mFlag=12;Comp;SST=Source;error=4";
        if (type==4) str="float";
        if (type==5) str="float;plot2;comment=12;dFlag=11;mFlag=12;Comp;error=6";
        if (type==6) str="float";
        if (type==7) str="float;comment=12;dFlag=11;mFlag=12;Correlate;Comp;SST=Target;error=8";
        if (type==8) str="float";
        if (type==9) str="float;comment=12;dFlag=11;mFlag=12;Comp;error=10";
        if (type==10) str="float";
        if (type==11) str="flag;edit";
        if (type==12) str="string;edit";
    }
    return str;
}

QString Inventory::access_data_netcdf(int type){
    QString str="";
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="d13C";
        if (type==4) str="d18O";
        if (type==5) str="d13C Err";
        if (type==6) str="d18O Err";
        if (type==7) str="d13C Corr";
        if (type==8) str="d18O Corr";
        if (type==9) str="Use Flag";
        if (type==10) str="Comment;dim=Character";
    }
    if (type>=0 && type<access_data_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="MgCa";
        if (type==4) str="MgCa Error";
        if (type==5) str="Temperature;SST";
        if (type==6) str="Temperature Error;SST Error";
        if (type==7) str="Use Flag";
        if (type==8) str="Comment;dim=Character";
    }
    if (type>=0 && type<access_data_length() && proxy=="Alkenones"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="C37_3";
        if (type==4) str="C37_2";
        if (type==5) str="C37_3 Error";
        if (type==6) str="C37_2 Error";
        if (type==7) str="UK'37";
        if (type==8) str="Temperature;SST";
        if (type==9) str="Temperature Error;SST Error";
        if (type==10) str="Use Flag";
        if (type==11) str="Comment;dim=Character";
    }
    if (type>=0 && type<access_data_length() && proxy=="XRF"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="Al";
        if (type==4) str="Si";
        if (type==5) str="P";
        if (type==6) str="S";
        if (type==7) str="Cl";
        if (type==8) str="K";
        if (type==9) str="Ca";
        if (type==10) str="Ti";
        if (type==11) str="Cr";
        if (type==12) str="Mn";
        if (type==13) str="Fe";
        if (type==14) str="Rh";
        if (type==15) str="AlSi";
        if (type==16) str="FeK";
        if (type==17) str="FeCa";
        if (type==18) str="AlCa";
        if (type==19) str="Use Flag";
        if (type==20) str="Comment;dim=Character";
    }
    if (type>=0 && type<access_data_length() && proxy=="Miscellaneous"){
        if (type==0) str="Depth";
        if (type==1) str="Sample Thickness";
        if (type==2) str="Age";
        if (type==3) str="Parameter 1";
        if (type==4) str="Parameter 1 Error";
        if (type==5) str="Parameter 2";
        if (type==6) str="Parameter 2 Error";
        if (type==7) str="Parameter 1 Corr";
        if (type==8) str="Parameter 1 Corr Error";
        if (type==9) str="Parameter 2 Corr";
        if (type==10) str="Parameter 2 Corr Error";
        if (type==11) str="Use Flag";
        if (type==12) str="Comment;dim=Character";
    }
    return str;
}




int Inventory::access_meta_length(){
    int length=0;
    if (proxy=="Foraminiferal Isotopes") length=23;
    if (proxy=="Foraminiferal MgCa") length=18;
    if (proxy=="Alkenones") length=18;
    if (proxy=="XRF") length=18;
    if (proxy=="Miscellaneous") length=17;
    return length;
}

QString Inventory::access_meta_name(int type){
    QString str="";
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="Core";
        if (type==1) str="Species";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Category";
        if (type==6) str="Device";
        if (type==7) str="Type";
        if (type==8) str="Laboratory";
        if (type==9) str="Electronic Paper";
        if (type==10) str="Comments";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="O-Use Flag";
        if (type==14) str="C-Use Flag";
        if (type==15) str="O-Correction";
        if (type==16) str="C-Correction";
        if (type==17) str="O-Justification";
        if (type==18) str="C-Justification";
        if (type==19) str="Importer";
        if (type==20) str="Basin";
        if (type==21) str="Filename";
        if (type==22) str="Optional Values";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="Core";
        if (type==1) str="Species";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Category";
        if (type==6) str="Device";
        if (type==7) str="Type";
        if (type==8) str="Laboratory";
        if (type==9) str="Electronic Paper";
        if (type==10) str="Comments";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="SST Use Flag";
        if (type==14) str="Importer";
        if (type==15) str="Basin";
        if (type==16) str="Filename";
        if (type==17) str="Optional Values";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Alkenones"){
        if (type==0) str="Core";
        if (type==1) str="Component";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Category";
        if (type==6) str="Device";
        if (type==7) str="Type";
        if (type==8) str="Laboratory";
        if (type==9) str="Electronic Paper";
        if (type==10) str="Comments";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="SST Use Flag";
        if (type==14) str="Importer";
        if (type==15) str="Basin";
        if (type==16) str="Filename";
        if (type==17) str="Optional Values";
    }

    if (type>=0 && type<access_meta_length() && proxy=="XRF"){
        if (type==0) str="Core";
        if (type==1) str="Component";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Category";
        if (type==6) str="Device";
        if (type==7) str="Type";
        if (type==8) str="Laboratory";
        if (type==9) str="Electronic Paper";
        if (type==10) str="Comments";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="Use Flag";
        if (type==14) str="Importer";
        if (type==15) str="Basin";
        if (type==16) str="Filename";
        if (type==17) str="Optional Values";
    }

    if (type>=0 && type<access_meta_length() && proxy=="Miscellaneous"){
        if (type==0) str="Core";
        if (type==1) str="Parameter";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Category";
        if (type==6) str="Device";
        if (type==7) str="Laboratory";
        if (type==8) str="Electronic Paper";
        if (type==9) str="Comments";
        if (type==10) str="Data Source";
        if (type==11) str="Reference";
        if (type==12) str="Use Flag";
        if (type==13) str="Importer";
        if (type==14) str="Basin";
        if (type==15) str="Filename";
        if (type==16) str="Optional Values";
     }
    return str;
}

QString Inventory::access_meta_unit(int type){
    QString str="";
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dez";
        if (type==3) str="dez";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
        if (type==12) str="";
        if (type==13) str="";
        if (type==14) str="";
        if (type==15) str="";
        if (type==16) str="";
        if (type==17) str="";
        if (type==18) str="";
        if (type==19) str="";
        if (type==20) str="";
        if (type==21) str="";
        if (type==22) str="";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dez";
        if (type==3) str="dez";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
        if (type==12) str="";
        if (type==13) str="";
        if (type==14) str="";
        if (type==15) str="";
        if (type==16) str="";
        if (type==17) str="";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Alkenones"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dez";
        if (type==3) str="dez";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
        if (type==12) str="";
        if (type==13) str="";
        if (type==14) str="";
        if (type==15) str="";
        if (type==16) str="";
        if (type==17) str="";
    }
    if (type>=0 && type<access_meta_length() && proxy=="XRF"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dez";
        if (type==3) str="dez";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
        if (type==12) str="";
        if (type==13) str="";
        if (type==14) str="";
        if (type==15) str="";
        if (type==16) str="";
        if (type==17) str="";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Miscellaneous"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dez";
        if (type==3) str="dez";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";
        if (type==10) str="";
        if (type==11) str="";
        if (type==12) str="";
        if (type==13) str="";
        if (type==14) str="";
        if (type==15) str="";
        if (type==16) str="";

    }
    return str;
}

QString Inventory::access_meta_value(int type){
    QString value=QString::number(NAN);
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) value=get_att_Core();
        if (type==1) value=get_att_Species();
        if (type==2) value=QString::number(get_att_Longitude());
        if (type==3) value=QString::number(get_att_Latitude());
        if (type==4) value=QString::number(get_att_Water_Depth());
        if (type==5) value=get_att_Category();
        if (type==6) value=get_att_Device();
        if (type==7) value=get_att_Record_Type();
        if (type==8) value=get_att_Laboratory();
        if (type==9) value=get_att_EPaper();
        if (type==10) value=get_att_Comment();
        if (type==11) value=get_att_Data_Source();
        if (type==12) value=get_att_Reference();
        if (type==13) value=QString::number(get_att_Oxygen_Use_Flag());
        if (type==14) value=QString::number(get_att_Carbon_Use_Flag());
        if (type==15) value=QString::number(get_att_O_Correction());
        if (type==16) value=QString::number(get_att_C_Correction());
        if (type==17) value=get_att_O_Justification();
        if (type==18) value=get_att_C_Justification();
        if (type==19) value=get_att_Importer();
        if (type==20) value=get_Basinname(get_att_Longitude(),get_att_Latitude());
        if (type==21) value=get_Filename(current_Core);
        if (type==22) value=get_att_Optional();
    }
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) value=get_att_Core();
        if (type==1) value=get_att_Species();
        if (type==2) value=QString::number(get_att_Longitude());
        if (type==3) value=QString::number(get_att_Latitude());
        if (type==4) value=QString::number(get_att_Water_Depth());
        if (type==5) value=get_att_Category();
        if (type==6) value=get_att_Device();
        if (type==7) value=get_att_Record_Type();
        if (type==8) value=get_att_Laboratory();
        if (type==9) value=get_att_EPaper();
        if (type==10) value=get_att_Comment();
        if (type==11) value=get_att_Data_Source();
        if (type==12) value=get_att_Reference();
        if (type==13) value=QString::number(get_att_SST_Use_Flag());
        if (type==14) value=get_att_Importer();
        if (type==15) value=get_Basinname(get_att_Longitude(),get_att_Latitude());
        if (type==16) value=get_Filename(current_Core);
        if (type==17) value=get_att_Optional();
    }
    if (type>=0 && type<access_meta_length() && proxy=="Alkenones"){
        if (type==0) value=get_att_Core();
        if (type==1) value=get_att_Species();
        if (type==2) value=QString::number(get_att_Longitude());
        if (type==3) value=QString::number(get_att_Latitude());
        if (type==4) value=QString::number(get_att_Water_Depth());
        if (type==5) value=get_att_Category();
        if (type==6) value=get_att_Device();
        if (type==7) value=get_att_Record_Type();
        if (type==8) value=get_att_Laboratory();
        if (type==9) value=get_att_EPaper();
        if (type==10) value=get_att_Comment();
        if (type==11) value=get_att_Data_Source();
        if (type==12) value=get_att_Reference();
        if (type==13) value=QString::number(get_att_SST_Use_Flag());
        if (type==14) value=get_att_Importer();
        if (type==15) value=get_Basinname(get_att_Longitude(),get_att_Latitude());
        if (type==16) value=get_Filename(current_Core);
        if (type==17) value=get_att_Optional();
    }
    if (type>=0 && type<access_meta_length() && proxy=="XRF"){
        if (type==0) value=get_att_Core();
        if (type==1) value=get_att_Species();
        if (type==2) value=QString::number(get_att_Longitude());
        if (type==3) value=QString::number(get_att_Latitude());
        if (type==4) value=QString::number(get_att_Water_Depth());
        if (type==5) value=get_att_Category();
        if (type==6) value=get_att_Device();
        if (type==7) value=get_att_Record_Type();
        if (type==8) value=get_att_Laboratory();
        if (type==9) value=get_att_EPaper();
        if (type==10) value=get_att_Comment();
        if (type==11) value=get_att_Data_Source();
        if (type==12) value=get_att_Reference();
        if (type==13) value=QString::number(get_att_Use_Flag());
        if (type==14) value=get_att_Importer();
        if (type==15) value=get_Basinname(get_att_Longitude(),get_att_Latitude());
        if (type==16) value=get_Filename(current_Core);
        if (type==17) value=get_att_Optional();
    }
    if (type>=0 && type<access_meta_length() && proxy=="Miscellaneous"){
        if (type==0) value=get_att_Core();
        if (type==1) value=get_att_Species();
        if (type==2) value=QString::number(get_att_Longitude());
        if (type==3) value=QString::number(get_att_Latitude());
        if (type==4) value=QString::number(get_att_Water_Depth());
        if (type==5) value=get_att_Category();
        if (type==6) value=get_att_Device();   
        if (type==7) value=get_att_Laboratory();
        if (type==8) value=get_att_EPaper();
        if (type==9) value=get_att_Comment();
        if (type==10) value=get_att_Data_Source();
        if (type==11) value=get_att_Reference();
        if (type==12) value=QString::number(get_att_Use_Flag());
        if (type==13) value=get_att_Importer();
        if (type==14) value=get_Basinname(get_att_Longitude(),get_att_Latitude());
        if (type==15) value=get_Filename(current_Core);
        if (type==16) value=get_att_Optional();

    }
    return value;
}

void Inventory::write_meta_value(int type,QString str){
    for (int i=0;i<str.size();i++){
        if (str.at(i)>QChar(255)|| (str.at(i)=="/"&&type==0)) str.replace(i,1,"_");
    }
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) set_att_Core(str);
        if (type==1) set_att_Species(str);
        if (type==2) set_att_Longitude(str.toFloat());
        if (type==3) set_att_Latitude(str.toFloat());
        if (type==4) set_att_Water_Depth(str.toFloat());
        if (type==5) set_att_Category(str);
        if (type==6) set_att_Device(str);
        if (type==7) set_att_Record_Type(str);
        if (type==8) set_att_Laboratory(str);
        if (type==9) set_att_EPaper(str);
        if (type==10) set_att_Comment(str);
        if (type==11) set_att_Data_Source(str);
        if (type==12) set_att_Reference(str);
        if (type==13) set_att_O_Use_Flag(str.toInt());
        if (type==14) set_att_C_Use_Flag(str.toInt());
        if (type==15) set_att_O_Correction(str.toFloat());
        if (type==16) set_att_C_Correction(str.toFloat());
        if (type==17) set_att_O_Justification(str);
        if (type==18) set_att_C_Justification(str);
        if (type==19) set_att_Importer(str);
        if (type==20) {}//set_Basin(str.toInt());
        if (type==21) set_Filename(str);
        if (type==22) set_att_Optional(str);
    }
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) set_att_Core(str);
        if (type==1) set_att_Species(str);
        if (type==2) set_att_Longitude(str.toFloat());
        if (type==3) set_att_Latitude(str.toFloat());
        if (type==4) set_att_Water_Depth(str.toFloat());
        if (type==5) set_att_Category(str);
        if (type==6) set_att_Device(str);
        if (type==7) set_att_Record_Type(str);
        if (type==8) set_att_Laboratory(str);
        if (type==9) set_att_EPaper(str);
        if (type==10) set_att_Comment(str);
        if (type==11) set_att_Data_Source(str);
        if (type==12) set_att_Reference(str);
        if (type==13) set_att_SST_Use_Flag(str.toInt());
        if (type==14) set_att_Importer(str);
        if (type==15) {}//set_Basin(str.toInt());
        if (type==16) set_Filename(str);
        if (type==17) set_att_Optional(str);
    }
    if (type>=0 && type<access_meta_length() && proxy=="Alkenones"){
        if (type==0) set_att_Core(str);
        if (type==1) set_att_Species(str);
        if (type==2) set_att_Longitude(str.toFloat());
        if (type==3) set_att_Latitude(str.toFloat());
        if (type==4) set_att_Water_Depth(str.toFloat());
        if (type==5) set_att_Category(str);
        if (type==6) set_att_Device(str);
        if (type==7) set_att_Record_Type(str);
        if (type==8) set_att_Laboratory(str);
        if (type==9) set_att_EPaper(str);
        if (type==10) set_att_Comment(str);
        if (type==11) set_att_Data_Source(str);
        if (type==12) set_att_Reference(str);
        if (type==13) set_att_SST_Use_Flag(str.toInt());
        if (type==14) set_att_Importer(str);
        if (type==15) {}//set_Basin(str.toInt());
        if (type==16) set_Filename(str);
        if (type==17) set_att_Optional(str);
    }
    if (type>=0 && type<access_meta_length() && proxy=="XRF"){
        if (type==0) set_att_Core(str);
        if (type==1) set_att_Species(str);
        if (type==2) set_att_Longitude(str.toFloat());
        if (type==3) set_att_Latitude(str.toFloat());
        if (type==4) set_att_Water_Depth(str.toFloat());
        if (type==5) set_att_Category(str);
        if (type==6) set_att_Device(str);
        if (type==7) set_att_Record_Type(str);
        if (type==8) set_att_Laboratory(str);
        if (type==9) set_att_EPaper(str);
        if (type==10) set_att_Comment(str);
        if (type==11) set_att_Data_Source(str);
        if (type==12) set_att_Reference(str);
        if (type==13) set_att_Use_Flag(str.toInt());
        if (type==14) set_att_Importer(str);
        if (type==15) {}//set_Basin(str.toInt());
        if (type==16) set_Filename(str);
        if (type==17) set_att_Optional(str);
    }
    if (type>=0 && type<access_meta_length() && proxy=="Miscellaneous"){
        if (type==0) set_att_Core(str);
        if (type==1) set_att_Species(str);
        if (type==2) set_att_Longitude(str.toFloat());
        if (type==3) set_att_Latitude(str.toFloat());
        if (type==4) set_att_Water_Depth(str.toFloat());
        if (type==5) set_att_Category(str);
        if (type==6) set_att_Device(str);
        if (type==7) set_att_Laboratory(str);
        if (type==8) set_att_EPaper(str);
        if (type==9) set_att_Comment(str);
        if (type==10) set_att_Data_Source(str);
        if (type==11) set_att_Reference(str);
        if (type==12) set_att_Use_Flag(str.toInt());
        if (type==13) set_att_Importer(str);
        if (type==14) {}//set_Basin(str.toInt());
        if (type==15) set_Filename(str);
        if (type==16) set_att_Optional(str);
    }
}

QString Inventory::access_meta_type(int type){
    QString str="";
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="string;lineedit";
        if (type==1) str="string;lineedit";
        if (type==2) str="float;min=-180;max=180";
        if (type==3) str="float;min=-90;max=90";
        if (type==4) str="float;min=0;max=12000";
        if (type==5) str="string;lineedit;read-only";
        if (type==6) str="string;lineedit";
        if (type==7) str="combo;item=unknown;item=sb;item=sp;item=db;item=dp";
        if (type==8) str="string;lineedit";
        if (type==9) str="string;lineedit;browse;open";
        if (type==10) str="string;field";
        if (type==11) str="string;lineedit";
        if (type==12) str="string;lineedit";
        if (type==13) str="flag";
        if (type==14) str="flag";
        if (type==15) str="float;default=0";
        if (type==16) str="float;default=0";
        if (type==17) str="string;field";
        if (type==18) str="string;field";
        if (type==19) str="string;lineedit";
        if (type==20) str="string;lineedit;read-only";
        if (type==21) str="string;lineedit;read-only";
        if (type==22) str="string;field";
    }

    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="string;lineedit";
        if (type==1) str="string;lineedit";
        if (type==2) str="float;min=-180;max=180";
        if (type==3) str="float;min=-90;max=90";
        if (type==4) str="float;min=0;max=12000";
        if (type==5) str="string;lineedit;read-only";
        if (type==6) str="string;lineedit";
        if (type==7) str="combo;item=unknown;item=sb;item=sp;item=db;item=dp";
        if (type==8) str="string;lineedit";
        if (type==9) str="string;lineedit;browse;open";
        if (type==10) str="string;field";
        if (type==11) str="string;lineedit";
        if (type==12) str="string;lineedit";
        if (type==13) str="flag";
        if (type==14) str="string;lineedit";
        if (type==15) str="string;lineedit;read-only";
        if (type==16) str="string;lineedit;read-only";
        if (type==17) str="string;field";

    }
    if (type>=0 && type<access_meta_length() && proxy=="Alkenones"){
        if (type==0) str="string;lineedit";
        if (type==1) str="string;lineedit";
        if (type==2) str="float;min=-180;max=180";
        if (type==3) str="float;min=-90;max=90";
        if (type==4) str="float;min=0;max=12000";
        if (type==5) str="string;lineedit;read-only";
        if (type==6) str="string;lineedit";
        if (type==7) str="combo;item=unknown;item=sb;item=sp;item=db;item=dp";
        if (type==8) str="string;lineedit";
        if (type==9) str="string;lineedit;browse;open";
        if (type==10) str="string;field";
        if (type==11) str="string;lineedit";
        if (type==12) str="string;lineedit";
        if (type==13) str="flag";
        if (type==14) str="string;lineedit";
        if (type==15) str="string;lineedit;read-only";
        if (type==16) str="string;lineedit;read-only";
        if (type==17) str="string;field";


    }
    if (type>=0 && type<access_meta_length() && proxy=="XRF"){
        if (type==0) str="string;lineedit";
        if (type==1) str="string;lineedit";
        if (type==2) str="float;min=-180;max=180";
        if (type==3) str="float;min=-90;max=90";
        if (type==4) str="float;min=0;max=12000";
        if (type==5) str="string;lineedit;read-only";
        if (type==6) str="string;lineedit";
        if (type==7) str="combo;item=unknown;item=sb;item=sp;item=db;item=dp";
        if (type==8) str="string;lineedit";
        if (type==9) str="string;lineedit;browse;open";
        if (type==10) str="string;field";
        if (type==11) str="string;lineedit";
        if (type==12) str="string;lineedit";
        if (type==13) str="flag";
        if (type==14) str="string;lineedit";
        if (type==15) str="string;lineedit;read-only";
        if (type==16) str="string;lineedit;read-only";
        if (type==17) str="string;field";


    }
    if (type>=0 && type<access_meta_length() && proxy=="Miscellaneous"){
        if (type==0) str="string;lineedit";
        if (type==1) str="string;lineedit";
        if (type==2) str="float;min=-180;max=180";
        if (type==3) str="float;min=-90;max=90";
        if (type==4) str="float;min=0;max=12000";
        if (type==5) str="string;lineedit;read-only";
        if (type==6) str="string;lineedit";
        if (type==7) str="string;lineedit";
        if (type==8) str="string;lineedit;browse;open";
        if (type==9) str="string;field";
        if (type==10) str="string;lineedit";
        if (type==11) str="string;lineedit";
        if (type==12) str="flag";
        if (type==13) str="string;lineedit";
        if (type==14) str="string;lineedit;read-only";
        if (type==15) str="string;lineedit;read-only";
        if (type==16) str="string;field";


    }
    return str;
}

QString Inventory::access_meta_netcdf(int type){
    QString str="";
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="Core";
        if (type==1) str="Species";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Proxy Category";
        if (type==6) str="Device";
        if (type==7) str="Record Type";
        if (type==8) str="Laboratory";
        if (type==9) str="E-Paper";
        if (type==10) str="Comment";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="Oxygen Use Flag";
        if (type==14) str="Carbon Use Flag";
        if (type==15) str="O-Correction";
        if (type==16) str="C-Correction";
        if (type==17) str="O-Correction Justification";
        if (type==18) str="C-Correction Justification";
        if (type==19) str="Importer";
        if (type==20) str="Basin;ignore";
        if (type==21) str="Filename;ignore";
        if (type==22) str="Optional";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="Core";
        if (type==1) str="Species";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Proxy Category";
        if (type==6) str="Device";
        if (type==7) str="Record Type";
        if (type==8) str="Laboratory";
        if (type==9) str="E-Paper";
        if (type==10) str="Comment";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="SST Use Flag";
        if (type==14) str="Importer";
        if (type==15) str="Basin;ignore";
        if (type==16) str="Filename;ignore";
        if (type==17) str="Optional";
    }
    if (type>=0 && type<access_meta_length() && proxy=="Alkenones"){
        if (type==0) str="Core";
        if (type==1) str="Component";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Proxy Category";
        if (type==6) str="Device";
        if (type==7) str="Record Type";
        if (type==8) str="Laboratory";
        if (type==9) str="E-Paper";
        if (type==10) str="Comment";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="SST Use Flag";
        if (type==14) str="Importer";
        if (type==15) str="Basin;ignore";
        if (type==16) str="Filename;ignore";
        if (type==17) str="Optional";
    }
    if (type>=0 && type<access_meta_length() && proxy=="XRF"){
        if (type==0) str="Core";
        if (type==1) str="Component";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Proxy Category";
        if (type==6) str="Device";
        if (type==7) str="Record Type";
        if (type==8) str="Laboratory";
        if (type==9) str="E-Paper";
        if (type==10) str="Comment";
        if (type==11) str="Data Source";
        if (type==12) str="Reference";
        if (type==13) str="Use Flag";
        if (type==14) str="Importer";
        if (type==15) str="Basin;ignore";
        if (type==16) str="Filename;ignore";
        if (type==17) str="Optional";
    }

    if (type>=0 && type<access_meta_length() && proxy=="Miscellaneous"){
        if (type==0) str="Core";
        if (type==1) str="Parameter";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Proxy Category";
        if (type==6) str="Device";
        if (type==7) str="Laboratory";
        if (type==8) str="E-Paper";
        if (type==9) str="Comment";
        if (type==10) str="Data Source";
        if (type==11) str="Reference";
        if (type==12) str="Use Flag";
        if (type==13) str="Importer";
        if (type==14) str="Basin;ignore";
        if (type==15) str="Filename;ignore";
        if (type==16) str="Optional";
    }
    return str;
}



int Inventory::access_inv_length(){
    int length=0;
    if (proxy=="Foraminiferal Isotopes") length=10;
    if (proxy=="Foraminiferal MgCa") length=9;
    if (proxy=="Alkenones") length=9;
    if (proxy=="XRF") length=9;
    if (proxy=="Miscellaneous") length=8;
    return length;
}

QString Inventory::access_inv_name(int type){
    QString str="";
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="Core Name";
        if (type==1) str="Species";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type";
        if (type==7) str="Filename";
        if (type==8) str="Carbon Use Flag";
        if (type==9) str="Oxygen Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="Core Name";
        if (type==1) str="Species";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type";
        if (type==7) str="Filename";
        if (type==8) str="SST Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Alkenones"){
        if (type==0) str="Core Name";
        if (type==1) str="Component";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type";
        if (type==7) str="Filename";
        if (type==8) str="SST Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="XRF"){
        if (type==0) str="Core Name";
        if (type==1) str="Component";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type";
        if (type==7) str="Filename";
        if (type==8) str="Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Miscellaneous"){
        if (type==0) str="Core Name";
        if (type==1) str="Parameter";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Filename";
        if (type==7) str="Use Flag";
    }
    return str;
}

QString Inventory::access_inv_unit(int type){
    QString str="";
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dec";
        if (type==3) str="dec";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
        if (type==9) str="";


    }
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dec";
        if (type==3) str="dec";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Alkenones"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dec";
        if (type==3) str="dec";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
    }
    if (type>=0 && type<access_inv_length() && proxy=="XRF"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dec";
        if (type==3) str="dec";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";
        if (type==8) str="";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Miscellaneous"){
        if (type==0) str="";
        if (type==1) str="";
        if (type==2) str="dec";
        if (type==3) str="dec";
        if (type==4) str="m";
        if (type==5) str="";
        if (type==6) str="";
        if (type==7) str="";

    }
    return str;
}

QString Inventory::access_inv_data(int type,int row){
    QString value=QString::number(NAN);
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Foraminiferal Isotopes"){
        if (type==0) value=get_Core(row);
        if (type==1) value=get_Species(row);
        if (type==2) value=QString::number(get_Longitude(row));
        if (type==3) value=QString::number(get_Latitude(row));
        if (type==4) value=QString::number(get_Water_Depth(row));
        if (type==5) value=get_Basinname(get_Longitude(row),get_Latitude(row));
        if (type==6) value=get_Record_Type(row);
        if (type==7) value=get_Filename(row);
        if (type==8) value=QString::number(get_Carbon_Use_Flag(row));
        if (type==9) value=QString::number(get_Oxygen_Use_Flag(row));

    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Foraminiferal MgCa"){
        if (type==0) value=get_Core(row);
        if (type==1) value=get_Species(row);
        if (type==2) value=QString::number(get_Longitude(row));
        if (type==3) value=QString::number(get_Latitude(row));
        if (type==4) value=QString::number(get_Water_Depth(row));
        if (type==5) value=get_Basinname(get_Longitude(row),get_Latitude(row));
        if (type==6) value=get_Record_Type(row);
        if (type==7) value=get_Filename(row);
        if (type==8) value=QString::number(get_SST_Use_Flag(row));
    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Alkenones"){
        if (type==0) value=get_Core(row);
        if (type==1) value=get_Species(row);
        if (type==2) value=QString::number(get_Longitude(row));
        if (type==3) value=QString::number(get_Latitude(row));
        if (type==4) value=QString::number(get_Water_Depth(row));
        if (type==5) value=get_Basinname(get_Longitude(row),get_Latitude(row));
        if (type==6) value=get_Record_Type(row);
        if (type==7) value=get_Filename(row);
        if (type==8) value=QString::number(get_SST_Use_Flag(row));
    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="XRF"){
        if (type==0) value=get_Core(row);
        if (type==1) value=get_Species(row);
        if (type==2) value=QString::number(get_Longitude(row));
        if (type==3) value=QString::number(get_Latitude(row));
        if (type==4) value=QString::number(get_Water_Depth(row));
        if (type==5) value=get_Basinname(get_Longitude(row),get_Latitude(row));
        if (type==6) value=get_Record_Type(row);
        if (type==7) value=get_Filename(row);
        if (type==8) value=QString::number(get_Use_Flag(row));
    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Miscellaneous"){
        if (type==0) value=get_Core(row);
        if (type==1) value=get_Species(row);
        if (type==2) value=QString::number(get_Longitude(row));
        if (type==3) value=QString::number(get_Latitude(row));
        if (type==4) value=QString::number(get_Water_Depth(row));
        if (type==5) value=get_Basinname(get_Longitude(row),get_Latitude(row));

        if (type==6) value=get_Filename(row);
        if (type==7) value=QString::number(get_Use_Flag(row));
    }
    return value;
}

QString Inventory::access_inv_meta(int type){
    QString value=QString::number(NAN);
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) value=access_meta_value(0);
        if (type==1) value=access_meta_value(1);
        if (type==2) value=access_meta_value(2);
        if (type==3) value=access_meta_value(3);
        if (type==4) value=access_meta_value(4);
        if (type==5) value=get_Basinname(access_meta_value(2).toFloat(),access_meta_value(3).toFloat());
        if (type==6) value=access_meta_value(7);
        if (type==7) value="";
        if (type==8) value=access_meta_value(14);
        if (type==9) value=access_meta_value(13);

    }
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) value=access_meta_value(0);
        if (type==1) value=access_meta_value(1);
        if (type==2) value=access_meta_value(2);
        if (type==3) value=access_meta_value(3);
        if (type==4) value=access_meta_value(4);
        if (type==5) value=get_Basinname(access_meta_value(2).toFloat(),access_meta_value(3).toFloat());
        if (type==6) value=access_meta_value(7);
        if (type==7) value="";
        if (type==8) value=access_meta_value(13);
    }
    if (type>=0 && type<access_inv_length() && proxy=="Alkenones"){
        if (type==0) value=access_meta_value(0);
        if (type==1) value=access_meta_value(1);
        if (type==2) value=access_meta_value(2);
        if (type==3) value=access_meta_value(3);
        if (type==4) value=access_meta_value(4);
        if (type==5) value=get_Basinname(access_meta_value(2).toFloat(),access_meta_value(3).toFloat());
        if (type==6) value=access_meta_value(7);
        if (type==7) value="";
        if (type==8) value=access_meta_value(13);
    }

    if (type>=0 && type<access_inv_length() && proxy=="XRF"){
        if (type==0) value=access_meta_value(0);
        if (type==1) value=access_meta_value(1);
        if (type==2) value=access_meta_value(2);
        if (type==3) value=access_meta_value(3);
        if (type==4) value=access_meta_value(4);
        if (type==5) value=get_Basinname(access_meta_value(2).toFloat(),access_meta_value(3).toFloat());
        if (type==6) value=access_meta_value(7);
        if (type==7) value="";
        if (type==8) value=access_meta_value(13);
    }
    if (type>=0 && type<access_inv_length() && proxy=="Miscellaneous"){
        if (type==0) value=access_meta_value(0);
        if (type==1) value=access_meta_value(1);
        if (type==2) value=access_meta_value(2);
        if (type==3) value=access_meta_value(3);
        if (type==4) value=access_meta_value(4);
        if (type==5) value=get_Basinname(access_meta_value(2).toFloat(),access_meta_value(3).toFloat());

        if (type==6) value="";
        if (type==7) value=access_meta_value(12);
    }
    return value;
}

int Inventory::access_inv_meta_nr(int type){
    int value=-1;
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) value=0;
        if (type==1) value=1;
        if (type==2) value=2;
        if (type==3) value=3;
        if (type==4) value=4;
        if (type==5) value=-1;
        if (type==6) value=7;
        if (type==7) value=21;
        if (type==8) value=14;
        if (type==9) value=13;

    }
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) value=0;
        if (type==1) value=1;
        if (type==2) value=2;
        if (type==3) value=3;
        if (type==4) value=4;
        if (type==5) value=-1;
        if (type==6) value=7;
        if (type==7) value=16;
        if (type==8) value=13;
    }
    if (type>=0 && type<access_inv_length() && proxy=="Alkenones"){
        if (type==0) value=0;
        if (type==1) value=1;
        if (type==2) value=2;
        if (type==3) value=3;
        if (type==4) value=4;
        if (type==5) value=-1;
        if (type==6) value=7;
        if (type==7) value=16;
        if (type==8) value=13;
    }

    if (type>=0 && type<access_inv_length() && proxy=="XRF"){
        if (type==0) value=0;
        if (type==1) value=1;
        if (type==2) value=2;
        if (type==3) value=3;
        if (type==4) value=4;
        if (type==5) value=-1;
        if (type==6) value=7;
        if (type==7) value=16;
        if (type==8) value=13;
    }
    if (type>=0 && type<access_inv_length() && proxy=="Miscellaneous"){
        if (type==0) value=0;
        if (type==1) value=1;
        if (type==2) value=2;
        if (type==3) value=3;
        if (type==4) value=4;
        if (type==5) value=-1;

        if (type==6) value=15;
        if (type==7) value=12;
    }
    return value;
}


void Inventory::write_inv_data(int type,int row,QString str){
    for (int i=0;i<str.size();i++){
        if (str.at(i)>QChar(255)) str.replace(i,1,"_");
    }

    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Foraminiferal Isotopes"){
        if (type==0) set_Core(str);
        if (type==1) set_Species(str);
        if (type==2) set_Longitude(str.toFloat());
        if (type==3) set_Latitude(str.toFloat());
        if (type==4) set_Water_Depth(str.toFloat());
        if (type==5) set_Basin(str.toInt());
        if (type==6) set_Record_Type(str);
        if (type==7) set_Filename(str);
        if (type==8) set_C_Use_Flag(str.toInt());
        if (type==9) set_O_Use_Flag(str.toInt());

    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Foraminiferal MgCa"){
        if (type==0) set_Core(str);
        if (type==1) set_Species(str);
        if (type==2) set_Longitude(str.toFloat());
        if (type==3) set_Latitude(str.toFloat());
        if (type==4) set_Water_Depth(str.toFloat());
        if (type==5) set_Basin(str.toInt());
        if (type==6) set_Record_Type(str);
        if (type==7) set_Filename(str);
        if (type==8) set_SST_Use_Flag(str.toInt());
    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Alkenones"){
        if (type==0) set_Core(str);
        if (type==1) set_Species(str);
        if (type==2) set_Longitude(str.toFloat());
        if (type==3) set_Latitude(str.toFloat());
        if (type==4) set_Water_Depth(str.toFloat());
        if (type==5) set_Basin(str.toInt());
        if (type==6) set_Record_Type(str);
        if (type==7) set_Filename(str);
        if (type==8) set_SST_Use_Flag(str.toInt());
    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="XRF"){
        if (type==0) set_Core(str);
        if (type==1) set_Species(str);
        if (type==2) set_Longitude(str.toFloat());
        if (type==3) set_Latitude(str.toFloat());
        if (type==4) set_Water_Depth(str.toFloat());
        if (type==5) set_Basin(str.toInt());
        if (type==6) set_Record_Type(str);
        if (type==7) set_Filename(str);
        if (type==8) set_Use_Flag(str.toInt());
    }
    if (type>=0 && type<access_inv_length() && row>=0 && row<get_Entries() && proxy=="Miscellaneous"){
        if (type==0) set_Core(str);
        if (type==1) set_Species(str);
        if (type==2) set_Longitude(str.toFloat());
        if (type==3) set_Latitude(str.toFloat());
        if (type==4) set_Water_Depth(str.toFloat());
        if (type==5) set_Basin(str.toInt());

        if (type==6) set_Filename(str);
        if (type==7) set_Use_Flag(str.toInt());
    }

}

QString Inventory::access_inv_type(int type){
    QString str="";
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="string;search";
        if (type==1) str="string;search";
        if (type==2) str="float;minmax;min=-180;max=+180;default=0;invert";
        if (type==3) str="float;minmax;min=-90;max=90;default=0";
        if (type==4) str="float;minmax;min=0;max=12000;default=0";
        if (type==5) str="string;combo;item=World Ocean;item=Atlantic Ocean;item=Pacific Ocean;item=Indian Ocean;item=Mediterranean Sea;item=Arctic Ocean;item=Antarctic Ocean";
        if (type==6) str="string;combo;item=All;item=sb;item=sp;item=db;item=dp";
        if (type==7) str="string;search";
        if (type==8) str="int;combo;item=Both;item=0;item=1";
        if (type==9) str="int;combo;item=Both;item=0;item=1";


    }
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="string;search";
        if (type==1) str="string;search";
        if (type==2) str="float;minmax;min=-180;max=180;invert";
        if (type==3) str="float;minmax;min=-90;max=90";
        if (type==4) str="float;minmax;min=0;max=12000";
        if (type==5) str="string;combo;item=World Ocean;item=Atlantic Ocean;item=Pacific Ocean;item=Indian Ocean;item=Mediterranean Sea;item=Arctic Ocean;item=Antarctic Ocean";
        if (type==6) str="string;combo;item=All;item=sb;item=sp;item=db;item=dp";
        if (type==7) str="string;search";
        if (type==8) str="int;combo;item=Both;item=0;item=1";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Alkenones"){
        if (type==0) str="string;search";
        if (type==1) str="string;search";
        if (type==2) str="float;minmax;min=-180;max=180;invert";
        if (type==3) str="float;minmax;min=-90;max=90";
        if (type==4) str="float;minmax;min=0;max=12000";
        if (type==5) str="string;combo;item=World Ocean;item=Atlantic Ocean;item=Pacific Ocean;item=Indian Ocean;item=Mediterranean Sea;item=Arctic Ocean;item=Antarctic Ocean";
        if (type==6) str="string;combo;item=All;item=sb;item=sp;item=db;item=dp";
        if (type==7) str="string;search";
        if (type==8) str="int;combo;item=Both;item=0;item=1";
    }
    if (type>=0 && type<access_inv_length() && proxy=="XRF"){
        if (type==0) str="string;search";
        if (type==1) str="string;search";
        if (type==2) str="float;minmax;min=-180;max=180;invert";
        if (type==3) str="float;minmax;min=-90;max=90";
        if (type==4) str="float;minmax;min=0;max=12000";
        if (type==5) str="string;combo;item=World Ocean;item=Atlantic Ocean;item=Pacific Ocean;item=Indian Ocean;item=Mediterranean Sea;item=Arctic Ocean;item=Antarctic Ocean";
        if (type==6) str="string;combo;item=All;item=sb;item=sp;item=db;item=dp";
        if (type==7) str="string;search";
        if (type==8) str="int;combo;item=Both;item=0;item=1";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Miscellaneous"){
        if (type==0) str="string;search";
        if (type==1) str="string;search";
        if (type==2) str="float;minmax;min=-180;max=180;invert";
        if (type==3) str="float;minmax;min=-90;max=90";
        if (type==4) str="float;minmax;min=0;max=12000";
        if (type==5) str="string;combo;item=World Ocean;item=Atlantic Ocean;item=Pacific Ocean;item=Indian Ocean;item=Mediterranean Sea;item=Arctic Ocean;item=Antarctic Ocean";

        if (type==6) str="string;search";
        if (type==7) str="int;combo;item=Both;item=0;item=1";
    }
    return str;
}

QString Inventory::access_inv_netcdf(int type){
    QString str="";
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal Isotopes"){
        if (type==0) str="Core;dim=cname";
        if (type==1) str="Species;dim=sname";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type;dim=typeid";
        if (type==7) str="Filename;dim=iname";
        if (type==8) str="Carbon Use Flag";
        if (type==9) str="Oxygen Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Foraminiferal MgCa"){
        if (type==0) str="Core;dim=cname";
        if (type==1) str="Species;dim=sname";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type;dim=typeid";
        if (type==7) str="Filename;dim=iname";
        if (type==8) str="SST Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Alkenones"){
        if (type==0) str="Core;dim=cname";
        if (type==1) str="Species;dim=sname";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type;dim=typeid";
        if (type==7) str="Filename;dim=iname";
        if (type==8) str="SST Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="XRF"){
        if (type==0) str="Core;dim=cname";
        if (type==1) str="Species;dim=sname";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Record Type;dim=typeid";
        if (type==7) str="Filename;dim=iname";
        if (type==8) str="Use Flag";
    }
    if (type>=0 && type<access_inv_length() && proxy=="Miscellaneous"){
        if (type==0) str="Core;dim=cname";
        if (type==1) str="Species;dim=sname";
        if (type==2) str="Longitude";
        if (type==3) str="Latitude";
        if (type==4) str="Water Depth";
        if (type==5) str="Basin";
        if (type==6) str="Filename;dim=iname";
        if (type==7) str="Use Flag";
    }
    return str;
}

int Inventory::access_inv_entries(){
    return get_Entries();
}

//********************************************************************************

double Inventory::get_data_Depth(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Depth")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Sample_Thickness(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Sample Thickness")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Age(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Age")) break;
    return data[i]->at(n).toFloat();
}

// Foraminiferal Isotopes
double Inventory::get_data_d13C(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("d13C")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_d18O(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("d18O")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_d13C_Corr(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d13C Corr")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_d18O_Corr(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d18O Corr")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_d13C_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d13C Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_d18O_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d18O Error")) break;
    return data[i]->at(n).toFloat();
}

// Foraminiferal MgCa
double Inventory::get_data_Mg(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Mg") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Mg_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Mg Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Ca(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Ca") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Ca_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Ca Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_MgCa(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Mg/Ca") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_MgCa_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Mg/Ca Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_SST(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="SST") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_SST_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("SST Error")) break;
    return data[i]->at(n).toFloat();
}

// Alkenons
double Inventory::get_data_C37_2(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="C37_2") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_C37_3(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="C37_3") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_C37_2_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="C37_2 Error") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_C37_3_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="C37_3 Error") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_UK37(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="UK'37") break;
    return data[i]->at(n).toFloat();
}

// XRF
int Inventory::get_data_XRF_Al(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Al") break;
    return data[i]->at(n).toInt();
}

int Inventory::get_data_XRF_Si(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Si") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_P(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="P") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_S(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="S") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_Cl(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Cl") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_K(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="K") break;
    return data[i]->at(n).toInt();

}
int Inventory::get_data_XRF_Ca(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Ca") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_Ti(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Ti") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_Cr(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Cr") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_Mn(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Mn") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_Fe(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Fe") break;
    return data[i]->at(n).toInt();
}
int Inventory::get_data_XRF_Rh(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Rh") break;
    return data[i]->at(n).toInt();
}
double Inventory::get_data_XRF_Al_Si(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Al/Si") break;
    return data[i]->at(n).toFloat();
}
double Inventory::get_data_XRF_Fe_K(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Fe/K") break;
    return data[i]->at(n).toFloat();
}
double Inventory::get_data_XRF_Fe_Ca(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Fe/Ca") break;
    return data[i]->at(n).toFloat();
}
double Inventory::get_data_XRF_Al_Ca(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Al/Ca") break;
    return data[i]->at(n).toFloat();
}

// Misc
double Inventory::get_data_Parameter1(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Parameter 1") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter1_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 1 Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter2(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Parameter 2") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter2_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 2 Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter1_Cal(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Parameter 1 Corr") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter1_Cal_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 1 Corr Error")) break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter2_Cal(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="Parameter 2 Corr") break;
    return data[i]->at(n).toFloat();
}

double Inventory::get_data_Parameter2_Cal_Err(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 2 Corr")) break;
    return data[i]->at(n).toFloat();
}

int Inventory::get_data_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Use Flag")) break;
    return data[i]->at(n).toInt();
}

QString Inventory::get_data_Comment(int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Comment")) break;
    return data[i]->at(n).toLatin1();
}

void Inventory::set_data_Depth(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Depth")) break;
    data[i]->replace(n,QString::number(v));

}

void Inventory::set_data_Sample_Thickness(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Sample Thickness")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Age(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Age")) break;
    data[i]->replace(n,QString::number(v));
}

// Foraminiferal Isotopes
void Inventory::set_data_d13C(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)=="d13C") break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_d18O(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("d18O")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_d13C_Corr(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d13C Corr")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_d18O_Corr(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d18O Corr")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_d13C_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d13C Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_d18O_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("d18O Error")) break;
    data[i]->replace(n,QString::number(v));
}

// Foraminiferal MgCa
void Inventory::set_data_Mg(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Mg")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Mg_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Mg Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Ca(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Ca")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Ca_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Ca Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_MgCa(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Mg/Ca")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_MgCa_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Mg/Ca Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_SST(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("SST")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_SST_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("SST Error")) break;
    data[i]->replace(n,QString::number(v));
}
// Alkenones
void Inventory::set_data_C37_2(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("C37_2")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_C37_3(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("C37_3")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_C37_2_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("C37_2 Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_C37_3_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("C37_3 Error")) break;
    data[i]->replace(n,QString::number(v));
}
void Inventory::set_data_UK37(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("UK'37")) break;
    data[i]->replace(n,QString::number(v));
}
// XRF
void Inventory::set_data_XRF_Al(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Al")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Si(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Si")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_P(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("P")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_S(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("S")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Cl(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Cl")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_K(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("K")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Ca(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Ca")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Ti(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Ti")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Cr(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Cr")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Mn(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Mn")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Fe(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Fe")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Rh(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Rh")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Al_Si(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Al/Si")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Fe_K(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Fe/K")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Fe_Ca(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Fe/Ca")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_XRF_Al_Ca(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Al/Ca")) break;
    data[i]->replace(n,QString::number(v));
}

// Misc
void Inventory::set_data_Parameter1(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Parameter 1")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter1_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 1 Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter2(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Parameter 2")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter2_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 2 Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter1_Cal(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Parameter 1 Corr")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter1_Cal_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 1 Corr Error")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter2_Cal(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i)==("Parameter 2 Corr")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Parameter2_Cal_Err(double v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Parameter 2 Corr Error")) break;
    data[i]->replace(n,QString::number(v));
}


void Inventory::set_data_Use_Flag(int v,int n){
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Use Flag")) break;
    data[i]->replace(n,QString::number(v));
}

void Inventory::set_data_Comment(QString str,int n){
    if (str=="") str="na";
    int i=0;
    for (i=0;i<access_data_length();i++) if (access_data_name(i).contains("Comment")) break;
    data[i]->replace(n,str);
}

double Inventory::get_Int_Value_18O(double depth, QString str){
    double yy=NAN;
    // Create Sublist of isotope data
    // create subset of active Flag only
    // get length
    int active_count=0;
    for (int i=0;i<get_Length();i++) if (get_data_Use_Flag(i)==1) active_count++;

    double *temp_18O;
    double *temp_depth;
    temp_18O=new double[active_count];
    temp_depth=new double[active_count];
    int n=0;
    for (int i=0;i<get_Length();i++){
        if (get_data_Use_Flag(i)==1){
            temp_18O[n]=get_data_d18O_Corr(i);
            temp_depth[n]=get_data_Depth(i);
            n++;
        }
    }
    // Interpolation
    if (depth>=temp_depth[0] && depth<=temp_depth[active_count-1]){

    if (str=="linear"){
        int i=0;
        for (i=0;i<active_count;i++) if (temp_depth[i]>depth) break;
        if (i>0) yy=temp_18O[i-1]+(temp_18O[i]-temp_18O[i-1])/(temp_depth[i]-temp_depth[i-1])*(depth-temp_depth[i-1]);

    }
    } else {
        yy=NAN;
    }
    delete[] temp_18O;
    delete[] temp_depth;

    return yy;
}

//QString Inventory::get_data_Comments(int j){
//    return get_data_Comment(j).toLatin1();
//}

QString Inventory::get_att_Core(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Core")) break;
    return meta.at(i);
}

void Inventory::set_att_Core(QString str){

    QString f=str;
    // turn all special characters into '_'
    for (int i=0;i<f.size();i++){
        if (f.at(i)>QChar(127)) f.replace(i,1,"_");
    }
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Core")) break;
    meta.replace(i,f);
}

double Inventory::get_att_Latitude(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Latitude")) break;
    return meta.at(i).toFloat();
}

void Inventory::set_att_Latitude(double v){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Latitude")) break;
    if (v>=-90 && v<=90) meta.replace(i,QString::number(v)); else meta.replace(i,QString::number(0));

}

double Inventory::get_att_Longitude(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Longitude")) break;
    return meta.at(i).toFloat();
}

void Inventory::set_att_Longitude(double v){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Longitude")) break;
    if (v>=-180 && v<=180) meta.replace(i,QString::number(v)); else meta.replace(i,QString::number(0));

}

QString Inventory::get_att_Device(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Device")) break;
    return meta.at(i);
}

void Inventory::set_att_Device(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Device")) break;
    meta.replace(i,str);

}

double Inventory::get_att_Water_Depth(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Water Depth")) break;
    return meta.at(i).toFloat();
}

void Inventory::set_att_Water_Depth(double v){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Water Depth")) break;
    if (v>=0)meta.replace(i,QString::number(v)); else meta.replace(i,QString::number(0));

}

QString Inventory::get_att_Species(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Species")||access_meta_name(i).contains("Parameter")||access_meta_name(i).contains("Component")) break;
    return meta.at(i);
}

void Inventory::set_att_Species(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Species")||access_meta_name(i).contains("Parameter")||access_meta_name(i).contains("Component")) break;

    meta.replace(i,str);

}

QString Inventory::get_att_Laboratory(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Laboratory")) break;
    return meta.at(i);
}

void Inventory::set_att_Laboratory(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Laboratory")) break;
    meta.replace(i,str);

}

QString Inventory::get_att_Reference(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Reference")) break;
    return meta.at(i);
}

void Inventory::set_att_Reference(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Reference")) break;
    meta.replace(i,str);


}

QString Inventory::get_att_Comment(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Comment")) break;
    return meta.at(i);
}

void Inventory::set_att_Comment(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Comment")) break;
    meta.replace(i,str);

}

QString Inventory::get_att_EPaper(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Electronic Paper")) break;
    return meta.at(i);
}

void Inventory::set_att_EPaper(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Electronic Paper")) break;
    meta.replace(i,str);
}

int Inventory::get_att_Oxygen_Use_Flag(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("O-Use Flag")) break;
    return meta.at(i).toInt();
}

void Inventory::set_att_O_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("O-Use Flag")) break;
    meta.replace(i,QString::number(n));
}

int Inventory::get_att_Carbon_Use_Flag(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("C-Use Flag")) break;
    return meta.at(i).toInt();
}

void Inventory::set_att_C_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("C-Use Flag")) break;
    meta.replace(i,QString::number(n));
}

double Inventory::get_att_O_Correction(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("O-Correction")) break;
    return meta.at(i).toFloat();
}

void Inventory::set_att_O_Correction(double v){
    if (isnan(v)) v=0;
        int i=0;
        for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("O-Correction")) break;
        meta.replace(i,QString::number(v));
        for (int i=0;i<get_Length();i++){
            set_data_d18O_Corr(get_data_d18O(i)+get_att_O_Correction(),i);
        }

}

QString Inventory::get_att_O_Justification(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("O-Justification")) break;
    return meta.at(i);
}

void Inventory::set_att_O_Justification(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("O-Justification")) break;
    meta.replace(i,str);
}

double Inventory::get_att_C_Correction(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("C-Correction")) break;
    return meta.at(i).toFloat();
}

void Inventory::set_att_C_Correction(double v){
    if (isnan(v)) v=0;
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("C-Correction")) break;
    meta.replace(i,QString::number(v));
    for (int i=0;i<get_Length();i++){
        set_data_d13C_Corr(get_data_d13C(i)+get_att_C_Correction(),i);
    }
}

QString Inventory::get_att_C_Justification(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("C-Justification")) break;
    return meta.at(i);
}

void Inventory::set_att_C_Justification(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("C-Justification")) break;
    meta.replace(i,str);
}

int Inventory::get_att_SST_Use_Flag(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("SST Use Flag")) break;
    return meta.at(i).toInt();
}

void Inventory::set_att_SST_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("SST Use Flag")) break;
    if (n!=1) n=0;
    meta.replace(i,QString::number(n));
}

int Inventory::get_att_Use_Flag(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).toLatin1()=="Use Flag") break;
    return meta.at(i).toInt();
}

void Inventory::set_att_Use_Flag(int n){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).toLatin1()=="Use Flag") break;
    if (n!=1) n=0;
    meta.replace(i,QString::number(n));
}

QString Inventory::get_att_Data_Source(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Data Source")) break;
    return meta.at(i);
}

void Inventory::set_att_Data_Source(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Data Source")) break;
    if (str=="") str="na";
    meta.replace(i,str);

}

QString Inventory::get_att_Record_Type(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Type")) break;
    return meta.at(i);
}

void Inventory::set_att_Record_Type(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Type")) break;
    if (str=="") str="na";
    meta.replace(i,str);

}



unsigned int Inventory::get_Length(){
    return length;
}

void Inventory::invert_O_Flag(int n){
    readData(n);

    if (get_att_Oxygen_Use_Flag()==1){
        set_att_O_Use_Flag(0);
        int c=current_Core;
        current_Core=n;
        set_O_Use_Flag(0);
        current_Core=c;
        //var_Oxygen_Use_Flag[n]=0;
    } else {
        set_att_O_Use_Flag(1);
        int c=current_Core;
        current_Core=n;
        set_O_Use_Flag(1);
        current_Core=c;
        //var_Oxygen_Use_Flag[n]=1;
    }
    saveData();
}

void Inventory::invert_C_Flag(int n){
    readData(n);
    if (get_att_Carbon_Use_Flag()==1){
        set_att_C_Use_Flag(0);
        int c=current_Core;
        current_Core=n;
        set_C_Use_Flag(0);
        current_Core=c;
    } else {
        set_att_C_Use_Flag(1);
        int c=current_Core;
        current_Core=n;
        set_C_Use_Flag(1);
        current_Core=c;
    }
    saveData();

}

void Inventory::invert_SST_Flag(int n){
    readData(n);

    if (get_att_SST_Use_Flag()==1){
        set_att_SST_Use_Flag(0);
        int c=current_Core;
        current_Core=n;
        set_SST_Use_Flag(0);
        current_Core=c;

    } else {
        set_att_SST_Use_Flag(1);
        int c=current_Core;
        current_Core=n;
        set_SST_Use_Flag(1);
        current_Core=c;

    }
    saveData();
}


QString Inventory::get_att_Importer(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Importer")) break;
    return meta.at(i);
}

void Inventory::set_att_Importer(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Importer")) break;
    meta.replace(i,str);
}

QString Inventory::get_att_Optional(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Optional Values")) break;
    return meta.at(i);
}

void Inventory::set_att_Optional(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Optional Values")) break;
    meta.replace(i,str);
}

QString Inventory::get_att_Category(){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Category")) break;
    return meta.at(i);
}

void Inventory::set_att_Category(QString str){
    int i=0;
    for (i=0;i<access_meta_length();i++) if (access_meta_name(i).contains("Category")) break;
    meta.replace(i,str);
}


void Inventory::saveData(){
    int n=current_Core;
    if (n>=0){
        // get file name
        QString QFilename=this->get_Filename(n);
        saveData(QFilename);
        // save meta to inventory
        for (int i=0;i<access_inv_length();i++) write_inv_data(i,n,access_meta_value(access_inv_meta_nr(i)));
        save();
    }
}

void Inventory::saveData(QString Filename){
    error_text.clear();
    QString action="Save Proxy";
    // get file name
    QString QFilename=resources.get_path_PaleoDataView()+"/"+resources.get_workdir()+"/"+proxy+"/Data/"+Filename;
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



    for (int i=0;i<access_meta_length();i++){
        if (access_meta_type(i).contains("string") || access_meta_type(i).contains("combo")) {
            if (!access_meta_netcdf(i).contains("ignore")){
                QString att_name=access_meta_netcdf(i).split(";").at(0);
                if ((retval = nc_put_att(ncid,NC_GLOBAL,att_name.toLatin1(),NC_CHAR,access_meta_value(i).length(),access_meta_value(i).toLatin1().data())))
                    NetCDF_Error(filename,action,retval);
                qDebug()<< access_meta_value(i).toLatin1().data();
            }
        } else {
            if (!access_meta_netcdf(i).contains("ignore")){
                QString att_name=access_meta_netcdf(i).split(";").at(0);
                double v=access_meta_value(i).toDouble();
                if ((retval = nc_put_att_double(ncid,NC_GLOBAL,att_name.toLatin1(),NC_DOUBLE,1,&v)))
                    NetCDF_Error(filename,action,retval);
               qDebug()<< QString::number(access_meta_value(i).toDouble());
            }
        }
    }

    //-------------- Write Data ---------------------------

    // length of inventory
    int drows=0;
    if ((retval = nc_def_dim(ncid,"Length",length,&drows)))
       NetCDF_Error(filename,action,retval);
    qDebug() << "Length "+QString::number(drows);

    // end definition mode
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    // define
    for (int i=0;i<access_data_length();i++){
        if (access_data_type(i).contains("string") && access_data_netcdf(i).contains("dim=")){
            // begin definition mode
            if ((retval = nc_redef(ncid)))
                NetCDF_Error(filename,action,retval);

            // define dimension
            QString dimname="";
            // search for dimname
            if (access_data_netcdf(i).contains("dim=")){
                QStringList l1=access_data_netcdf(i).split(";");
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
            for (int ii=0;ii<length;ii++) if (var_length<data[i]->at(ii).length()) var_length=data[i]->at(ii).length();
            int var=0;
            if ((retval = nc_def_dim(ncid,dimname.toLatin1(),var_length,&var)))
               NetCDF_Error(filename,action,retval);
            qDebug() << "length :"+QString::number(var_length);

            // define variable
            int dim[2];
            dim[0]=drows;
            dim[1]=var;
            int varid;
            QString varname=access_data_netcdf(i).split(";").at(0);
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
                for (int jj=0;jj<data[i]->at(ii).length();jj++) text[jj][ii]=data[i]->at(ii).at(jj).toLatin1();
            }
            if ((retval = nc_put_var(ncid,varid,&text[0][0])))
                NetCDF_Error(filename,action,retval);
            qDebug() << access_data_name(i)+" written";

        } else {
            // begin definition mode
            if ((retval = nc_redef(ncid)))
                NetCDF_Error(filename,action,retval);

            // define
            int dimm[1];
            dimm[0]=drows;
            int varid;
            QString varname=access_data_netcdf(i).split(";").at(0);
            qDebug()<<varname;
            if ((retval = nc_def_var(ncid,varname.toLatin1(),NC_DOUBLE,1,dimm,&varid)))
                NetCDF_Error(filename,action,retval);

            // end definition mode
            if ((retval = nc_enddef(ncid)))
                ERR(retval);

            // write data
            double dat[length];
            for (int ii=0;ii<length;ii++) {
                QString str=data[i]->at(ii);
                dat[ii]=str.toDouble();

            }

            if ((retval = nc_put_var(ncid,varid,&dat)))
                NetCDF_Error(filename,action,retval);
            qDebug() << access_data_name(i)+" written";

        }
    }



    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
        NetCDF_Error(filename,action,retval);
   //qDebug()<<"OK";
    flag_Data_OK=1;


    delete[] filename;
    if (error_text!="") NetCDF_Error_Message();
}

int Inventory::get_currentCore(){
    return current_Core;
}

void Inventory::set_currentCore(QString name,QString proxy){
    int found=0;
    for (int i=0;i<get_Entries();i++){
        if (name==get_Core(i) && proxy==get_Species(i)){
            current_Core=i;
            found=1;
            break;
        }
    }
    if (found==0) {
        //qDebug() << "Entry not found";
        current_Core=-1;
    }

}

void Inventory::set_currentCore(int i){
    if (i<get_Entries() && i>=0){
        current_Core=i;
        //qDebug()<< "Current Core:"+QString::number(current_Core);
    }else{
        current_Core=-1;//qDebug()<<"entry not existent";
    }
}


//***************** Target Data
// Data Format Quartets of: Age(kyr):Age Error:Value:Error
void Inventory::readTarget(){
    // get file name

    QString QFilename="";
    QFilename=access_proxy_target();


    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;

    // Initialize ids and meta
    int ncid;
    int varid[2];
    int dimid[2]={0};
    size_t dimlen[2]={0};
    int retval;// for Errors

    // Open the file
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // Get Number of Variables
    int ndimsp,nvarsp,nattsp,unlimdimidp;
    if ((retval = nc_inq(ncid,&ndimsp,&nvarsp,&nattsp,&unlimdimidp)))
       ERR(retval);
    //qDebug()<<QString::number(ndimsp)+":"+QString::number(nvarsp)+":"+QString::number(nattsp)+":"+QString::number(unlimdimidp);
    // read header and data
    header.clear();
    target.clear();
    header.reserve(nvarsp/2);
    target.reserve((nvarsp/2)*4);
    for (int i=0;i<nvarsp;i++){
        // read header
        // get length of header
        size_t len[2];
        if ((retval =nc_inq_dimlen(ncid,i*2+0,&len[0])))
            ERR(retval);
        if ((retval =nc_inq_dimlen(ncid,i*2+1,&len[1])))
            ERR(retval);
        //qDebug()<<QString::number(len[0])+":"+QString::number(len[1]);
        // read header
        unsigned char text[len[0]*len[1]];
        if ((retval = nc_get_var(ncid,i,&text[0]))) ERR(retval);
        for (unsigned int j=0;j<len[0];j++){
            QString str="";
            for (unsigned int k=0;k<len[1];k++){
                str.append(text[j+len[0]*k]);
            }

            header<<str.simplified();
            //qDebug()<<str.simplified();
        }
        // finished reading header
        i++;
        // read data
        // get length of data
        if ((retval =nc_inq_dimlen(ncid,i*2+0,&len[0])))
            ERR(retval);
        if ((retval =nc_inq_dimlen(ncid,i*2+1,&len[1])))
            ERR(retval);

        //qDebug()<<QString::number(len[0])+":"+QString::number(len[1]);
        // read data
        double d[len[0]*len[1]];
        if ((retval = nc_get_var(ncid,i,&d))) ERR(retval);
        QList<double> x1;
        x1.clear();
        x1.reserve(4*len[0]);
        for (int j=0;j<len[0];j++){
            x1.append(d[j*4+0]);
            x1.append(d[j*4+1]);
            x1.append(d[j*4+2]);
            x1.append(d[j*4+3]);
        }
        target.append(x1);

        //qDebug()<<QString::number(target[0][0])+":"+QString::number(target[0][1])+":"+QString::number(target[0][2]);
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
}

int Inventory::get_Target_Length(int i){
    return target[i].size()/4;
}

int Inventory::get_Target_Vari(){
    return target.size();
}

QString Inventory::get_Target_Name(int i){
    return header[i*4+2];
}

double Inventory::get_Target_Age(int n,int i){
    return target[i][n*4];
}

double Inventory::get_Target_Value(int n,int i){
    return target[i][n*4+2];
}

double Inventory::get_Target_Error(int n,int i){
    return target[i][n*4+3];
}

double Inventory::get_Target_Age_Error(int n,int i){
    return target[i][n*4+1];
}

void Inventory::read_Basin(){

    // get file name
    QString QFilename=resources.path_hydro+"basin.nc";

    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;


    // Initialize ids and meta
    int ncid;
    int varid;
    size_t dimlen[2]={0};
    int dimid[2]={0};
    int retval;// for Errors

    // Open the file
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // ********  read basin dimension
    if ((retval = nc_inq_varid(ncid, "Ocean Basin", &varid)))
       ERR(retval);
    // get info for sizes
    if ((retval = nc_inq_dimid(ncid, "Latitude", &dimid[0])))
       ERR(retval);
    if ((retval = nc_inq_dimlen(ncid, dimid[0], &dimlen[0])))
       ERR(retval);
    if ((retval = nc_inq_dimid(ncid, "Longitude", &dimid[1])))
       ERR(retval);
    if ((retval = nc_inq_dimlen(ncid, dimid[1], &dimlen[1])))
       ERR(retval);
    basin_lat=dimlen[0];
    basin_lon=dimlen[1];
    // read basin
    basin_data=new double[(basin_lon)*(basin_lat)];
    if ((retval = nc_get_var_double(ncid,varid,&basin_data[0]))) ERR(retval);


    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
}

int Inventory::get_Basin(float lon,float lat){
    int basin=0;
    if (lon<0) lon=360+lon;

    lat=lat+90;
    int x=(int)lon;
    int y=(int)lat;
    if (x>0 && x<360 && y>0 && y<180){
        basin=basin_data[x+y*basin_lon];

        // search algorithem
        for (int order=1;order<8;order++){
            for (int i=0;i<=order;i++){
                if (basin==-100){
                    int new_x=x+order;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y+i;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x+order;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y-i;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x-order;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y+i;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x-order;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y-i;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x+i;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y+order;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x+i;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y-order;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x-i;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y+order;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                    basin=basin_data[new_x+new_y*basin_lon];
                }
                if (basin==-100){
                    int new_x=x-i;
                    if (new_x>=360) new_x-=360;
                    if (new_x<0) new_x+=360;
                    int new_y=y-order;
                    if (new_y>=180) new_x-=180;
                    if (new_x<0) new_x+=180;
                }
                if (basin!=-100) break;
            }
            if (basin!=-100) break;
        }

/*
        // if on land look for sourondings
        if (basin==-100){
            int new_x=x+1;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+0;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x-1;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+0;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+0;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+1;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+0;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y-1;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+1;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+1;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x-1;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+1;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+1;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y-1;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x-1;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y-1;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+2;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+0;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x-2;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+0;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+0;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y+2;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }
        if (basin==-100){
            int new_x=x+0;
            if (new_x>=360) new_x-=360;
            if (new_x<0) new_x+=360;
            int new_y=y-2;
            if (new_y>=180) new_x-=180;
            if (new_x<0) new_x+=180;
            basin=basin_data[new_x+new_y*basin_lon];
        }*/

    } else basin=NAN;

    return basin;
}

QString Inventory::get_Basinname(float lon,float lat){

    int basin=get_Basin(lon,lat);
    QString basinname="NA";
    if (basin==1) basinname="Atlantic Ocean";
    if (basin==2) basinname="Pacific Ocean";
    if (basin==3) basinname="Indian Ocean";
    if (basin==4) basinname="Mediterranean Sea";
    if (basin==5) basinname="Baltic Sea";
    if (basin==6) basinname="Black Sea";
    if (basin==7) basinname="Red Sea";
    if (basin==8) basinname="Persian Gulf";
    if (basin==9) basinname="Hudson Bay";
    if (basin==10) basinname="Antarctic Ocean";
    if (basin==11) basinname="Arctic Ocean";
    if (basin==53) basinname="Caspian Sea";
    if (basin==56) basinname="Bay of Bengal";
    if (basin==-100) basinname="On Land!";
    qDebug()<<QString::number(lon)+" : "+QString::number(lat)+" : "+basinname;
    return basinname;
}

QString Inventory::get_Basinname(int basin){
    QString basinname="NA";
    if (basin==1) basinname="Atlantic Ocean";
    if (basin==2) basinname="Pacific Ocean";
    if (basin==3) basinname="Indian Ocean";
    if (basin==4) basinname="Mediterranean Sea";
    if (basin==5) basinname="Baltic Sea";
    if (basin==6) basinname="Black Sea";
    if (basin==7) basinname="Red Sea";
    if (basin==8) basinname="Persian Gulf";
    if (basin==9) basinname="Hudson Bay";
    if (basin==10) basinname="Antarctic Ocean";
    if (basin==11) basinname="Arctic Ocean";
    if (basin==53) basinname="Caspian Sea";
    if (basin==56) basinname="Bay of Bengal";
    if (basin==-100) basinname="On Land!";
    return basinname;
}


bool Inventory::checkData(int mode){
    int corrected=0;
    if (proxy=="Foraminiferal Isotopes"){
        corrected=1;
    // check for double entries
    for (int i=0;i<get_Length();i++){
        float sum_18O=0;
        float sum_13C=0;
        int countO=0;
        int countC=0;
        for(int j=0;j<get_Length();j++){
            if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                if (!isnan(get_data_d18O(j))) {
                    sum_18O=sum_18O+get_data_d18O(j);
                    countO++;
                }
                if (!isnan(get_data_d13C(j))) {
                    sum_13C=sum_13C+get_data_d13C(j);
                    countC++;
                }
            }
        }
        if((countO>1 || countC>1) && mode==0){

            QMessageBox msgBox;
            msgBox.setText("Core :"+get_att_Core()+"\nSpecies :"+get_att_Species()+"\nDepth :"+QString::number(get_data_Depth(i))+"\nMultiple("+QString::number(countO+countC)+") values found -> Use flags deactivated");
            msgBox.setInformativeText("Do you want to create a mean?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret==QMessageBox::No){
                // just deactivate
                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }
            } else {
                // create new entry
                addIsotope();// is now last entry at pos [length]


                set_data_Depth(get_data_Depth(i),length-1);

                set_data_Sample_Thickness(get_data_Sample_Thickness(i),length-1);

                set_data_Age(get_data_Age(i),length-1);

                if (countC>0)set_data_d13C(sum_13C/countC,length-1);

                if (countO>0)set_data_d18O(sum_18O/countO,length-1);

                if (isnan(get_att_C_Correction())&&countC>0) set_data_d13C_Corr(sum_13C/countC,length-1); else set_data_d13C_Corr(sum_13C/countC+get_att_C_Correction(),length-1);

                if (isnan(get_att_O_Correction())&&countO>0) set_data_d18O_Corr(sum_18O/countO,length-1); else set_data_d18O_Corr(sum_18O/countO+get_att_O_Correction(),length-1);

                //bestimme fehler der mittelwerte
                float sum_18O_err=0;

                float sum_13C_err=0;

                for(int j=0;j<get_Length();j++){
                    if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                        if (countO>0)sum_18O_err=sum_18O_err+(get_data_d18O(i)-sum_18O/countO)*(get_data_d18O(i)-sum_18O/countO);
                        if (countC>0)sum_13C_err=sum_13C_err+(get_data_d13C(i)-sum_13C/countC)*(get_data_d13C(i)-sum_13C/countC);
                    }
                }

                if (countO>1)set_data_d13C_Err(sqrt(sum_13C_err/(countC-1))/sqrt(countC),length-1);

                if (countC>1)set_data_d18O_Err(sqrt(sum_18O_err/(countO-1))/sqrt(countO),length-1);

                set_data_Use_Flag(1,length-1);

                set_data_Comment("Mean of multiple measurements",length-1);

                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }

                // sort all data according depth
                sort(0,length-1);
            }
        }

        if((countO>1 ||countC>1) && mode>0){
            if (mode==2){
                // just deactivate
                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }
            }
            if (mode==1){
                // create new entry
                addIsotope();// is now last entry at pos [length]

                set_data_Depth(get_data_Depth(i),length-1);

                set_data_Sample_Thickness(get_data_Sample_Thickness(i),length-1);

                set_data_Age(get_data_Age(i),length-1);

                if (countC>1)set_data_d13C(sum_13C/countC,length-1);

                if (countO>1)set_data_d18O(sum_18O/countO,length-1);

                if (isnan(get_att_C_Correction())&&countC>1) set_data_d13C_Corr(sum_13C/countC,length-1); else set_data_d13C_Corr(sum_13C/countC+get_att_C_Correction(),length-1);

                if (isnan(get_att_O_Correction())&&countO>1) set_data_d18O_Corr(sum_18O/countO,length-1); else set_data_d18O_Corr(sum_18O/countO+get_att_O_Correction(),length-1);

                //bestimme fehler der mittelwerte
                float sum_18O_err=0;

                float sum_13C_err=0;

                for(int j=0;j<get_Length();j++){
                    if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                        if (countO>1)sum_18O_err=sum_18O_err+(get_data_d18O(i)-sum_18O/countO)*(get_data_d18O(i)-sum_18O/countO);
                        if (countC>1)sum_13C_err=sum_13C_err+(get_data_d13C(i)-sum_13C/countC)*(get_data_d13C(i)-sum_13C/countC);
                    }
                }

                if (countO>1)set_data_d13C_Err(sqrt(sum_13C_err/(countC-1))/sqrt(countC),length-1);

                if (countC>1)set_data_d18O_Err(sqrt(sum_18O_err/(countO-1))/sqrt(countO),length-1);


                set_data_Use_Flag(1,length-1);

                set_data_Comment("Mean of multiple measurements",length-1);

                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }

                // sort all data according depth
                sort(0,length-1);
            }
        }
    }
    }
/*
    if (proxy=="Foraminiferal MgCa"){
        corrected=1;
        // check for double entries
        for (int i=0;i<get_Length();i++){
            //float sum_Mg=0;
            //float sum_Ca=0;
            float sum_MgCa=0;
            float sum_SST=0;
            int countMgCa=0;
            int countSST=0;
            for(int j=0;j<get_Length();j++){
                if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                    //sum_Mg=sum_Mg+get_data_Mg(j);
                    //sum_Ca=sum_Ca+get_data_Ca(j);
                    if (!isnan())
                    sum_MgCa=sum_MgCa+get_data_MgCa(j);
                    sum_SST=sum_SST+get_data_SST(j);
                    count++;
                }
            }
            if(count>1 && mode==0){
                QMessageBox msgBox;
                msgBox.setText("Core :"+get_att_Core()+"\nSpecies :"+get_att_Species()+"\nDepth :"+QString::number(get_data_Depth(i))+"\nMultiple("+QString::number(count)+") values found -> Use flags deactivated");
                msgBox.setInformativeText("Do you want to create a mean?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                if (ret==QMessageBox::No){
                    // just deactivate
                    // deactivate all conflicting data
                    for(int j=0;j<get_Length()-1;j++){
                        if (get_data_Depth(j)==get_data_Depth(i)){
                            set_data_Use_Flag(0,j);
                        }
                    }
                } else {
                    // create new entry
                    addIsotope();// is now last entry at pos [length]

                    set_data_Depth(get_data_Depth(i),length-1);

                    set_data_Sample_Thickness(get_data_Sample_Thickness(i),length-1);

                    set_data_Age(get_data_Age(i),length-1);

                    //set_data_Mg(sum_Mg/count,length-1);

                    //set_data_Ca(sum_Ca/count,length-1);

                    set_data_MgCa(sum_MgCa/count,length-1);

                    set_data_SST(sum_SST/count,length-1);

                    //bestimme fehler der mittelwerte
                    //float sum_Mg_err=0;
                    //float sum_Ca_err=0;
                    float sum_MgCa_err=0;
                    float sum_SST_err=0;

                    for(int j=0;j<get_Length();j++){
                        if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                            //sum_Mg_err=sum_Mg_err+(get_data_Mg(i)-sum_Mg/count)*(get_data_Mg(i)-sum_Mg/count);
                            //sum_Ca_err=sum_Ca_err+(get_data_Ca(i)-sum_Ca/count)*(get_data_Ca(i)-sum_Ca/count);
                            sum_MgCa_err=sum_MgCa_err+(get_data_MgCa(i)-sum_MgCa/count)*(get_data_MgCa(i)-sum_MgCa/count);
                            sum_SST_err=sum_SST_err+(get_data_SST(i)-sum_SST/count)*(get_data_SST(i)-sum_SST/count);
                        }
                    }

                    //set_data_Mg_Err(sqrt(sum_Mg_err/(count-1))/sqrt(count),length-1);

                    //set_data_Ca_Err(sqrt(sum_Ca_err/(count-1))/sqrt(count),length-1);

                    set_data_MgCa_Err(sqrt(sum_MgCa_err/(count-1))/sqrt(count),length-1);

                    set_data_SST_Err(sqrt(sum_SST_err/(count-1))/sqrt(count),length-1);

                    set_data_Use_Flag(1,length-1);

                    set_data_Comment("Mean of multiple measurements",length-1);

                    // deactivate all conflicting data
                    for(int j=0;j<get_Length()-1;j++){
                        if (get_data_Depth(j)==get_data_Depth(i)){
                            set_data_Use_Flag(0,j);
                        }
                    }

                    // sort all data according depth
                    sort(0,length-1);
                }
            }

            if(count>1 && mode>0){

                if (mode==2){
                    // just deactivate
                    // deactivate all conflicting data
                    for(int j=0;j<get_Length()-1;j++){
                        if (get_data_Depth(j)==get_data_Depth(i)){
                            set_data_Use_Flag(0,j);
                        }
                    }
                }
                if (mode==1){
                    // create new entry
                    addIsotope();// is now last entry at pos [length]

                    set_data_Depth(get_data_Depth(i),length-1);

                    set_data_Sample_Thickness(get_data_Sample_Thickness(i),length-1);

                    set_data_Age(get_data_Age(i),length-1);

                    //set_data_Mg(sum_Mg/count,length-1);

                    //set_data_Ca(sum_Ca/count,length-1);

                    set_data_MgCa(sum_MgCa/count,length-1);

                    set_data_SST(sum_SST/count,length-1);

                    //bestimme fehler der mittelwerte
                    //float sum_Mg_err=0;
                    //float sum_Ca_err=0;
                    float sum_MgCa_err=0;
                    float sum_SST_err=0;

                    for(int j=0;j<get_Length();j++){
                        if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                            //sum_Mg_err=sum_Mg_err+(get_data_Mg(i)-sum_Mg/count)*(get_data_Mg(i)-sum_Mg/count);
                            //sum_Ca_err=sum_Ca_err+(get_data_Ca(i)-sum_Ca/count)*(get_data_Ca(i)-sum_Ca/count);
                            sum_MgCa_err=sum_MgCa_err+(get_data_MgCa(i)-sum_MgCa/count)*(get_data_MgCa(i)-sum_MgCa/count);
                            sum_SST_err=sum_SST_err+(get_data_SST(i)-sum_SST/count)*(get_data_SST(i)-sum_SST/count);
                        }
                    }

                    //set_data_Mg_Err(sqrt(sum_Mg_err/(count-1))/sqrt(count),length-1);

                    //set_data_Ca_Err(sqrt(sum_Ca_err/(count-1))/sqrt(count),length-1);

                    set_data_MgCa_Err(sqrt(sum_MgCa_err/(count-1))/sqrt(count),length-1);

                    set_data_SST_Err(sqrt(sum_SST_err/(count-1))/sqrt(count),length-1);

                    set_data_Use_Flag(1,length-1);

                    set_data_Comment("Mean of multiple measurements",length-1);

                    // deactivate all conflicting data
                    for(int j=0;j<get_Length()-1;j++){
                        if (get_data_Depth(j)==get_data_Depth(i)){
                            set_data_Use_Flag(0,j);
                        }
                    }

                    // sort all data according depth
                    sort(0,length-1);
                }
            }


        }
    }*/


    // generalized method according to access_data_type String
    if (corrected==0){
        corrected=1;
    // check for double entries
    for (int i=0;i<get_Length();i++){

        int count=0;
        for(int j=0;j<get_Length();j++){
            if (get_data_Depth(j)==get_data_Depth(i) && get_data_Use_Flag(j)==1){
                count++;
            }
        }

        if(count>1 && mode==0){
            QMessageBox msgBox;
            msgBox.setText("Core :"+get_att_Core()+"\nSpecies :"+get_att_Species()+"\nDepth :"+QString::number(get_data_Depth(i))+"\nMultiple("+QString::number(count)+") values found -> Use flags deactivated");
            msgBox.setInformativeText("Do you want to create a mean?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret==QMessageBox::No){
                // just deactivate
                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }
            } else {
                // create new entry
                addIsotope();// is now last entry at pos [length]
                // create mean entry
                for (int j=0;j<access_data_length();j++){
                    // get type
                    QStringList str=access_data_type(j).split(";");
                    int pos=-1;
                    for (int q=0;q<str.size();q++)if (str.at(q).contains("check=")) pos=q;

                    if (pos>=0) {// only if check was found
                        qDebug()<<"Check found";
                        QStringList type=str.at(pos).split("=");
                        if (type.at(1).contains("asis")) write_data_value(j,length-1,access_data_value(j,i));
                        if (type.at(1).contains("mean")) {
                            float sum=0;
                            int c=0;
                            for (int ii=0;ii<get_Length();ii++){
                                if (get_data_Depth(ii)==get_data_Depth(i) && get_data_Use_Flag(ii)==1){
                                    if(!isnan(access_data_value(j,ii).toFloat())){
                                        sum+=access_data_value(j,ii).toFloat();
                                        c++;
                                    }
                                }
                            }
                            if (c>0)write_data_value(j,length-1,QString::number(sum/c));
                        }
                        if (type.at(1).contains("error")) {
                            int data=type.at(2).toInt();

                            float sum=0;
                            int c=0;
                            for (int ii=0;ii<get_Length();ii++){
                                if (get_data_Depth(ii)==get_data_Depth(i) && get_data_Use_Flag(ii)==1){
                                    if (!isnan(access_data_value(data,ii).toFloat())){
                                        sum+=access_data_value(data,ii).toFloat();
                                        c++;
                                    }
                                }
                            }
                            float sum_err=0;
                            for(int ii=0;ii<get_Length();ii++){
                                if (get_data_Depth(ii)==get_data_Depth(i) && get_data_Use_Flag(ii)==1){
                                    if(!isnan(access_data_value(data,ii).toFloat())) sum_err=sum_err+(access_data_value(data,ii).toFloat()-sum/c)*(access_data_value(data,ii).toFloat()-sum/c);
                                }
                            }
                            if(c>1)write_data_value(j,length-1,QString::number(sqrt(sum_err/(c-1))/sqrt(c)));
                        }
                    }
                }

                set_data_Use_Flag(1,length-1);

                set_data_Comment("Mean of multiple measurements",length-1);

                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }

                // sort all data according depth
                sort(0,length-1);
            }
        }

        if(count>1 && mode>0){
            if (mode==2){
                // just deactivate
                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }
            }
            if (mode==1){
                // create new entry
                addIsotope();// is now last entry at pos [length]

                 // create mean entry
                for (int j=0;j<access_data_length();j++){
                    // get type
                    QStringList str=access_data_type(j).split(";");
                    int pos=-1;
                    for (int q=0;q<str.size();q++)if (str.at(q).contains("check=")) pos=q;

                    if (pos>=0) {// only if check was found
                        qDebug()<<"Check found";
                        QStringList type=str.at(pos).split("=");
                        if (type.at(1).contains("asis")) write_data_value(j,length-1,access_data_value(j,i));
                        if (type.at(1).contains("mean")) {
                            float sum=0;
                            int c=0;
                            for (int ii=0;ii<get_Length();ii++){
                                if (get_data_Depth(ii)==get_data_Depth(i) && get_data_Use_Flag(ii)==1){
                                    if(!isnan(access_data_value(j,ii).toFloat())){
                                        sum+=access_data_value(j,ii).toFloat();
                                        c++;
                                    }
                                }
                            }
                            if(c>0)write_data_value(j,length-1,QString::number(sum/c));
                        }
                        if (type.at(1).contains("error")) {
                            int data=type.at(2).toInt();

                            float sum=0;
                            int c=0;
                            for (int ii=0;ii<get_Length();ii++){
                                if (get_data_Depth(ii)==get_data_Depth(i) && get_data_Use_Flag(ii)==1){
                                    if (!isnan(access_data_value(data,ii).toFloat())){
                                        sum+=access_data_value(data,ii).toFloat();
                                        c++;
                                    }
                                }
                            }
                            float sum_err=0;
                            for(int ii=0;ii<get_Length();ii++){
                                if (get_data_Depth(ii)==get_data_Depth(i) && get_data_Use_Flag(ii)==1){
                                    if(!isnan(access_data_value(data,ii).toFloat())) sum_err=sum_err+(access_data_value(data,ii).toFloat()-sum/c)*(access_data_value(data,ii).toFloat()-sum/c);
                                }
                            }
                            if(c>1)write_data_value(j,length-1,QString::number(sqrt(sum_err/(c-1))/sqrt(c)));
                        }
                    }
                }

                set_data_Use_Flag(1,length-1);

                set_data_Comment("Mean of multiple measurements",length-1);

                // deactivate all conflicting data
                for(int j=0;j<get_Length()-1;j++){
                    if (get_data_Depth(j)==get_data_Depth(i)){
                        set_data_Use_Flag(0,j);
                    }
                }

                // sort all data according depth
                sort(0,length-1);
            }
        }
    }
    }
    return 0;
}

void Inventory::sort(int left,int right){// sort for Depth(NOT Age)
    // Using QuickSort
    int i = left, j = right;
    double tmp;
    QString *tmp_str;
    double pivot = get_data_Depth((left + right) / 2);
    while (i <= j) {
        while (get_data_Depth(i) < pivot) i++;
        while (get_data_Depth(j) > pivot) j--;
        if (i <= j) {
            // exchange everything
            for (int ii=0;ii<access_data_length();ii++){
                data[ii]->swap(i,j);
            }
            i++;
            j--;
        }
    };
    if (left < j) sort(left, j);
    if (i < right) sort(i, right);








}


