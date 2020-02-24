/********************************************************************************/
/*    This file is part of PaleoView.                       					*/
/*                                                                      		*/
/*    PaleoView is free software: you can redistribute it and/or modify 		*/
/*    it under the terms of the GNU General Public License as published by  	*/
/*    the Free Software Foundation, either version 3 of the License, or     	*/
/*    (at your option) any later version.                                   	*/
/*                                                                          	*/
/*    PaleoView is distributed in the hope that it will be useful,          	*/
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of        	*/
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*/
/*    GNU General Public License for more details.                              */
/*                                                                              */
/*    You should have received a copy of the GNU General Public License         */
/*    along with PaleoView.  If not, see <http://www.gnu.org/licenses/>.        */
/********************************************************************************/

#include "AMData.h"
#include "cmath"
AMData::AMData(Inventory *i):
    inv(i)
{
    select=0;
    resources=new Resources();
}

AMData::~AMData(){
    //qDebug()<<"Cleaning Database...";


    delete depth;

    delete comment;
    delete labid;
    delete type;
    delete age_data;
    delete cal_pdf;

    delete intPDF;
    delete intYear;

    delete hpd_prop;
    delete hpd_year;

    delete cage;
    delete dage;
    delete uage;

    delete resources;
}

void AMData::AMRead(){
    // get file name
    //qDebug()<<"Reading Age Model";
    QString name=inv->get_Core(inv->get_currentCore());
    name.replace('\0',' ');
    name.replace(" ","");
    QString QFilename=resources->path_age+name+".age";
    QFilename.replace(QString(".nc"),QString(".age"));

    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;


    // Initialize ids and meta
    int ncid;
    int varid;
    size_t dimlen[3]={0};
    int retval;// for Errors

    // Open the file
    if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){

        // ******** read depth
        // get varid
        if ((retval = nc_inq_varid(ncid, "Depth", &varid)))
           ERR(retval);
        // get size
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        depth_length=dimlen[0];
        //qDebug() << "rows: "+QString::number(dimlen[0]);
        // read data
        depth=new double[dimlen[0]];
        if ((retval = nc_get_var_double(ncid,varid,&depth[0]))) ERR(retval);
        for (int i=0;i<depth_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(depth[i]);

        // ******* read comment
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "lcom", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        //qDebug() << "length of Comment "+QString::number(dimlen[0]);
        // get Comments
        varid=0;
        if ((retval = nc_inq_varid(ncid, "Age Comment", &varid)))
           ERR(retval);

        unsigned char text[(dimlen[0])*depth_length];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);

        comment=new QString [depth_length];

        for (unsigned int i=0;i<depth_length;i++){
            for (unsigned int j=0;j<dimlen[0];j++){
                comment[i].append(text[i+depth_length*j]);

            }
            //qDebug()<<comment[i];
        }
        }

        // ******* read Lab. ID
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "llabel", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        //qDebug() << "length of labid "+QString::number(dimlen[0]);
        // get ids
        varid=0;
        if ((retval = nc_inq_varid(ncid, "Lab. ID", &varid)))
           ERR(retval);

        unsigned char text[(dimlen[0])*depth_length];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);

        labid=new QString [depth_length];

        for (unsigned int i=0;i<depth_length;i++){
            for (unsigned int j=0;j<dimlen[0];j++){
                labid[i].append(text[i+depth_length*j]);

            }
            //qDebug()<<labid[i];
        }
        }

        // ******* read Type
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "ltype", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        //qDebug() << "length of type "+QString::number(dimlen[0]);
        // get ids
        varid=0;
        if ((retval = nc_inq_varid(ncid, "Type", &varid)))
           ERR(retval);

        unsigned char text[(dimlen[0])*depth_length];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);

        type=new QString [depth_length];

        for (unsigned int i=0;i<depth_length;i++){
            for (unsigned int j=0;j<dimlen[0];j++){
                type[i].append(text[i+depth_length*j]);

            }
            //qDebug()<<type[i];
        }
        }

        // ******** read data
        // get varid
        if ((retval = nc_inq_dimid(ncid, "ageind", &varid)))
           ERR(retval);
        // get size
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        ageind=dimlen[0];
        //qDebug() << "colums: "+QString::number(dimlen[0]);

        // read data
        if ((retval = nc_inq_varid(ncid, "Age data", &varid)))
           ERR(retval);
        age_data=new double[ageind*depth_length];
        if ((retval = nc_get_var_double(ncid,varid,&age_data[0]))) ERR(retval);





        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);
    } else {

        if (nc_strerror(retval)=="No such file or directory") {
            //qDebug() << "Start generating new File...";
        } else {
            //qDebug() << "File Error";
           // ERR(retval);
        }
        // Default Model
        depth=new double[1];
        depth_length=1;
        comment=new QString[1];
        type=new QString[1];
        labid=new QString[1];
        ageind=8;
        age_data=new double[ageind*depth_length];
        depth[0]=NAN;
        type[0]="Unknown";
        labid[0]="Unknown";
        comment[0]="Default entry of new age model : Please Edit";
        for (int i=0;i<ageind-1;i++) age_data[i]=NAN;
        age_data[ageind]=0;
    }
    select=0;
    delete filename;
}

void AMData::AMSave(){
    // get file name
    //qDebug()<<"Saving Age Model";
    QString name=inv->get_Core(inv->get_currentCore());
            name.replace(" ","");
    QString QFilename=resources->path_age+name+".age";
    QFilename.replace(QString(".nc"),QString(".age"));

    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;


    // Initialize ids and meta
    int ncid;
    int retval;// for Errors

    // Open the file
    if (!(retval = nc_create(filename, NC_WRITE, &ncid))){
        // Define variables
        //qDebug() << "DefineMode";

        // Depth
        int depth_id;
        int dimm[1];
        int depth_dim;
        if ((retval = nc_def_dim(ncid,"depth",get_Length(),&depth_dim)))
           ERR(retval);
        dimm[0]=depth_dim;
        if ((retval = nc_def_var(ncid,"Depth",NC_DOUBLE,1,dimm,&depth_id)))
            ERR(retval);
        //qDebug() << depth_id;

        // Comment
        int comment_length=0;
        for (int i=0;i<get_Length();i++) if (comment_length<comment[i].length()) comment_length=comment[i].length();
        int lcom_dim;
        if ((retval = nc_def_dim(ncid,"lcom",comment_length,&lcom_dim)))
           ERR(retval);
        int rcom_dim;
        if ((retval = nc_def_dim(ncid,"rcom",get_Length(),&rcom_dim)))
           ERR(retval);
        //qDebug() << "comment length"+QString::number(comment_length);
        int dim[2];
        dim[0]=lcom_dim;
        dim[1]=rcom_dim;
        int comment_id;
        if ((retval = nc_def_var(ncid,"Age Comment",NC_CHAR,2,dim,&comment_id)))
            ERR(retval);
        //qDebug() << comment_id;

        // Lab.ID
        int labid_length=0;
        for (int i=0;i<get_Length();i++) if (labid_length<labid[i].length()) labid_length=labid[i].length();
        int llabel_dim;
        if ((retval = nc_def_dim(ncid,"llabel",labid_length,&llabel_dim)))
           ERR(retval);
        int rlabel_dim;
        if ((retval = nc_def_dim(ncid,"rlabel",get_Length(),&rlabel_dim)))
           ERR(retval);
        //qDebug() << "labid length"+QString::number(labid_length);

        dim[1]=rlabel_dim;
        dim[0]=llabel_dim;
        int labid_id;
        if ((retval = nc_def_var(ncid,"Lab. ID",NC_CHAR,2,dim,&labid_id)))
            ERR(retval);
        //qDebug() << labid_id;

        // Type
        int type_length=0;
        for (int i=0;i<get_Length();i++) if (type_length<type[i].length()) type_length=type[i].length();
        int ltype_dim=0;
        if ((retval = nc_def_dim(ncid,"ltype",type_length,&ltype_dim)))
           ERR(retval);
        int rtype_dim=0;
        if ((retval = nc_def_dim(ncid,"rtype",get_Length(),&rtype_dim)))
           ERR(retval);
        //qDebug() << "type length"+QString::number(type_length);

        dim[1]=rtype_dim;
        dim[0]=ltype_dim;
        int type_id;
        if ((retval = nc_def_var(ncid,"Type",NC_CHAR,2,dim,&type_id)))
            ERR(retval);
        //qDebug() << type_id;

        // Data
        int data_length=ageind;
        int ageind_dim;
        if ((retval = nc_def_dim(ncid,"ageind",data_length,&ageind_dim)))
           ERR(retval);

        //qDebug() << "data length"+QString::number(data_length);
        dim[1]=depth_dim;
        dim[0]=ageind_dim;
        int data_id;
        if ((retval = nc_def_var(ncid,"Age data",NC_DOUBLE,2,dim,&data_id)))
            ERR(retval);
        //qDebug() << data_id;

        // end definition mode
        if ((retval = nc_enddef(ncid)))
            ERR(retval);

        // ******** save depth
        if ((retval = nc_put_var(ncid,depth_id,depth)))
            ERR(retval);
        //qDebug() << "depth written";

        // ******* write comment
        {
        unsigned char text[comment_length][get_Length()];
        for(int i=0;i<comment_length;i++) for(int ii=0;ii<get_Length();ii++) text[i][ii]=' ';

        for (int i=0;i<get_Length();i++){
            for (int j=0;j<comment[i].length();j++) text[j][i]=comment[i].at(j).toLatin1();
        }
        if ((retval = nc_put_var(ncid,comment_id,&text[0][0])))
            ERR(retval);
        //qDebug() << "Comments written";
        }

        // ******* write Lab. ID
        {
        unsigned char text[labid_length][get_Length()];
        for(int i=0;i<labid_length;i++) for(int ii=0;ii<get_Length();ii++) text[i][ii]=' ';

        for (int i=0;i<get_Length();i++){
            for (int j=0;j<labid[i].length();j++) text[j][i]=labid[i].at(j).toLatin1();
        }
        if ((retval = nc_put_var(ncid,labid_id,&text[0][0])))
            ERR(retval);
        //qDebug() << "LabIDs written";
        }

        // ******* write Type
        {
        unsigned char text[type_length][get_Length()];
        for(int i=0;i<type_length;i++) for(int ii=0;ii<get_Length();ii++) text[i][ii]=' ';

        for (int i=0;i<get_Length();i++){
            for (int j=0;j<type[i].length();j++) text[j][i]=type[i].at(j).toLatin1();
        }
        if ((retval = nc_put_var(ncid,type_id,&text[0][0])))
            ERR(retval);
        //qDebug() << "Type written";
        }

        // ******** write data
        if ((retval = nc_put_var(ncid,data_id,age_data)))
            ERR(retval);





        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);
        inv->set_AgeModel(inv->get_currentCore(),1);

    } else {

        if (nc_strerror(retval)=="No such file or directory") {
            //qDebug() << "Start generating new File...";
        } else {

           // ERR(retval);
        }
    }
    delete filename;
}


double AMData::get_Depth(int n){
    return depth[n];
}

int AMData::get_Length(){
    return depth_length;
}

QString AMData::get_Age_Comment(int n){
    return comment[n];
}

QString AMData::get_Type(int n){
    return type[n].simplified();
}

QString AMData::get_LabID(int n){
    return labid[n];
}

//************************************
// Reads data from Array
// c=0 Age Dated
// c=1 Age + error
// c=2 Age - error
// c=3 Age Res.
// c=4 Calender
// c=5 Cal min
// c=6 Cal max
// c=7 Use Flag
//*************************************
double AMData::get_Data(int c,int n){
    return age_data[n+depth_length*c];
}

void AMData::set_Depth(int n,double v){
    depth[n]=v;
}

void AMData::set_Age_Comment(QString str,int n){
    comment[n]=str;
}

void AMData::set_Type(int n,QString str){
    type[n]=str;
}

void AMData::set_LabID(int n,QString str){
    labid[n]=str;
}

//************************************
// Reads data from Array
// c=0 Age Dated
// c=1 Age + error
// c=2 Age - error
// c=3 Age Res.
// c=4 Cal
// c=5 Cal min
// c=6 Cal max
// c=7 Use Flag
//*************************************
void AMData::set_Data(int c,int n,double v){
    age_data[n+depth_length*c]=v;
}

void AMData::ReadCalib(){
    // get file name
    //qDebug()<<"Reading Calibration Data";
    QString QFilename=resources->filename_C14_Cal;

    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());
    //qDebug() << QFilename;

    QFile file(QFilename);
    if(!file.open(QIODevice::ReadOnly)) {
        //qDebug() <<  file.errorString();
    }

    QTextStream in(&file);
    int count=0;
    while(!in.atEnd()) {
        QString line = in.readLine();
        count++;
    }
    file.close();


    Calib_Length=count;
    //qDebug() << "C14 Calib Entries found "+QString::number(count);
    dage=new double[count];
    cage=new double[count];
    uage=new double[count];
    count=0;
    if(!file.open(QIODevice::ReadOnly)) {
        //qDebug() <<  file.errorString();
    }
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split("\t");
        cage[count]=fields.at(0).toDouble();
        dage[count]=fields.at(1).toDouble();
        uage[count]=fields.at(2).toDouble();
        //qDebug()<<QString::number(dage[count])+":"+QString::number(cage[count]);
        count++;

    }

    file.close();
    delete filename;
    //qDebug() << "C14 Calib imported";
}

double AMData::get_dage(int n){
    return dage[n];
}

double AMData::get_cage(int n){
    return cage[n];
}

double AMData::get_dage_fmu(int n){
    return exp(-dage[n]/8033.0);
}

double AMData::get_uage(int n){
    return uage[n];
}

double AMData::get_uage_fmu(int n){
    return get_dage_fmu(n)-exp(-(dage[n]+uage[n])/8033.0);
}

int AMData::get_Calib_Length(){
    return Calib_Length;
}

void AMData::setCalibFile(QString file){
    resources->filename_C14_Cal=file;
}

double AMData::calc_CalPDF(double value,double error){
    cal_pdf=new double[Calib_Length];
    double pi=3.14159265;
    double value_fmu= exp(-(value/8033.0));
    double error_fmu= value_fmu-exp(-(value+error)/8033.0);
    cal_pdf_max=0;
    for (int i=0;i<Calib_Length;i++){
        double s=sqrt(pow(error_fmu,2.0)+pow(get_uage_fmu(i),2.0));
        cal_pdf[i]=exp(-(pow((get_dage_fmu(i)-value_fmu),2.0)/(2.0*pow(s,2.0))));
        cal_pdf[i]=cal_pdf[i]/(s*sqrt(2.0*pi));
        if (cal_pdf[i]>cal_pdf_max) cal_pdf_max=cal_pdf[i];

    }
}

double AMData::get_CalPDF(int i){
    return cal_pdf[i];
}

double AMData::get_DPDF(double value,double error,double dage){

    double pi=3.14159265;
    double value_fmu= exp(-(value/8033.0));
    double error_fmu= value_fmu-exp(-(value+error)/8033.0);
    double dage_fmu=exp(-(dage/8033.0));
    double uage_fmu=0;// is zero for dated Age


    double s=sqrt(pow(error_fmu,2.0)+pow(uage_fmu,2.0));
    double pdf=exp(-(pow((dage_fmu-value_fmu),2.0)/(2.0*pow(s,2.0))));
    pdf=pdf/(s*sqrt(2.0*pi));


    return pdf;
}

double AMData::get_CalPDF_max(){
    return cal_pdf_max;
}

void AMData::calc_intPDF(){
    // estimate length of vector
    intPDF_Size=0;
    for (int i=0;i<Calib_Length-1;i++){
        intPDF_Size=intPDF_Size+(int)(get_cage(i+1)-get_cage(i));
    }
    //qDebug() << "PDF Size :"+QString::number(intPDF_Size);

    intPDF=new double[intPDF_Size];
    intYear=new double[intPDF_Size];
    int count=0;
    for (int i=0;i<Calib_Length-1;i++){
        for (int y=get_cage(i);y<get_cage(i+1);y++){// new PDF in 1 year intervall
            intYear[count]=(double)y;
            intPDF[count]=get_CalPDF(i)+(y-get_cage(i))*((get_CalPDF(i+1)-get_CalPDF(i))/(get_cage(i+1)-get_cage(i)));
            count++;
        }
    }

}

double AMData::get_intYear(int n){
    return intYear[n];
}

double AMData::get_intPDF(int n){
    return intPDF[n];
}

int AMData::get_int_Length(){
    return intPDF_Size;
}

void AMData::normPDF(){
    double sum=0;
    for(int i=0;i<intPDF_Size;i++) sum+=intPDF[i];
    for(int i=0;i<intPDF_Size;i++) intPDF[i]=intPDF[i]/sum;
}

void AMData::sortInt(){
    normPDF();
    // sort after intPDF;
    Sort_intPDF(0,intPDF_Size-1);
    // Estimate 95% Range
    // number of values lower<sum95%

    double cumprop=0;
    int count=0;
    for (int i=0;i<intPDF_Size-1;i++){
        cumprop+=intPDF[i];
        if(cumprop<0.95){

        } else {
            count=i;
            break;
        }
    }
    hpd_length=count;
    cumprop=0;
    hpd_prop=new double [hpd_length];
    hpd_year=new double [hpd_length];
    int index=0;
    count=0;
    for (int i=0;i<intPDF_Size-1;i++){
        cumprop+=intPDF[i];
        if(cumprop<0.95){
            index=i;
            hpd_prop[count]=intPDF[i];
            hpd_year[count]=intYear[i];
            count++;
        } else {
            break;
        }
    }
    //qDebug() << QString::number(hpd_length);
    Sort_hpd_year(0,hpd_length-1);
    Cal95U=hpd_year[0];
    Cal95L=hpd_year[hpd_length-1];
    CalMedian=0;
    if (index%2==0){
        CalMedian=(intYear[(hpd_length-1)/2]+intYear[(hpd_length-1)/2+1])/2;
    } else {
        CalMedian=(intYear[(hpd_length-1)/2]+1);
    }
    CalMedian=floor(CalMedian+0.5);
    CalMidAge=floor(((Cal95U+Cal95L)/2)+0.5);
    //qDebug()<<"95U :"+QString::number(Cal95U);
    //qDebug()<<"95L :"+QString::number(Cal95L);
    //qDebug()<<"Median :"+QString::number(CalMedian);
    //qDebug()<<"MidAge :"+QString::number(CalMidAge);
    // WeightedMean
    double Weigth=0, Age=0;
    for (int i=0;i<hpd_length;i++)
    {
    Age = Age + (hpd_year[i] * hpd_prop[i]);
    Weigth = Weigth + hpd_prop[i];
    }
    CalWMean = Age/Weigth;
    CalWMean = floor (CalWMean + 0.5);
    //qDebug()<<"WMean :"+QString::number(CalWMean);
}

double AMData::get_Cal95U(){
    return Cal95U;
}

double AMData::get_Cal95L(){
    return Cal95L;
}

double AMData::get_CalMedian(){
    return CalMedian;
}

double AMData::get_CalMidAge(){
    return CalMidAge;
}

double AMData::get_CalWMean(){
    return CalWMean;
}

void AMData::Sort_intPDF(int left,int right){
    // Using QuickSort
    int i = left, j = right;
    double tmp;
    double pivot = intPDF[(left + right) / 2];
    while (i <= j) {
        while (intPDF[i] > pivot) i++;
        while (intPDF[j] < pivot) j--;
        if (i <= j) {
            tmp = intPDF[i];
            intPDF[i] = intPDF[j];
            intPDF[j] = tmp;
            tmp = intYear[i];
            intYear[i] = intYear[j];
            intYear[j] = tmp;
            i++;
            j--;
        }
    };
    if (left < j) Sort_intPDF(left, j);
    if (i < right) Sort_intPDF(i, right);
}

void AMData::Sort_intYear(int left,int right){
    // Using QuickSort
    int i = left, j = right;
    double tmp;
    double pivot = intYear[(left + right) / 2];
    while (i <= j) {
        while (intYear[i] > pivot) i++;
        while (intYear[j] < pivot) j--;
        if (i <= j) {
            tmp = intPDF[i];
            intPDF[i] = intPDF[j];
            intPDF[j] = tmp;
            tmp = intYear[i];
            intYear[i] = intYear[j];
            intYear[j] = tmp;
            i++;
            j--;
        }
    };
    if (left < j) Sort_intYear(left, j);
    if (i < right) Sort_intYear(i, right);
}

void AMData::Sort_hpd_year(int left,int right){
    // Using QuickSort
    int i = left, j = right;
    double tmp;
    double pivot = hpd_year[(left + right) / 2];
    while (i <= j) {
        while (hpd_year[i] > pivot) i++;
        while (hpd_year[j] < pivot) j--;
        if (i <= j) {
            tmp = hpd_prop[i];
            hpd_prop[i] = hpd_prop[j];
            hpd_prop[j] = tmp;
            tmp = hpd_year[i];
            hpd_year[i] = hpd_year[j];
            hpd_year[j] = tmp;
            i++;
            j--;
        }
    };
    if (left < j) Sort_hpd_year(left, j);
    if (i < right) Sort_hpd_year(i, right);
}

void AMData::sort(int left,int right){// sort for Depth(NOT Age)
    // Using QuickSort
    int i = left, j = right;
    double tmp;
    QString tmp_str;
    double pivot = depth[(left + right) / 2];
    while (i <= j) {
        while (depth[i] < pivot) i++;
        while (depth[j] > pivot) j--;
        if (i <= j) {
            // exchange everything
            tmp = depth[i];
            depth[i] = depth[j];
            depth[j] = tmp;
            tmp_str = labid[i];
            labid[i] = labid[j];
            labid[j] = tmp_str;
            tmp_str = type[i];
            type[i] = type[j];
            type[j] = tmp_str;
            tmp_str = comment[i];
            comment[i] = comment[j];
            comment[j] = tmp_str;
            for (int ii=0;ii<=7;ii++){
                tmp = get_Data(ii,i);
                set_Data(ii,i,get_Data(ii,j));
                set_Data(ii,j,tmp);
            }
            i++;
            j--;
        }
    };
    if (left < j) sort(left, j);
    if (i < right) sort(i, right);
}

void AMData::deleteEntry(int n){
    //temp
    double *depth_tmp=new double[get_Length()];
    QString *type_tmp=new QString[get_Length()];
    QString *labid_tmp=new QString[get_Length()];
    QString *comment_tmp=new QString[get_Length()];
    double *data_tmp=new double[get_Length()*ageind];
    for (int i=0;i<get_Length();i++){
        depth_tmp[i]=depth[i];
        type_tmp[i]=type[i];
        labid_tmp[i]=labid[i];
        comment_tmp[i]=comment[i];
        for (int j=0;j<ageind;j++){
            data_tmp[i+depth_length*j]=age_data[i+depth_length*j];
        }
    }
    depth_length--;
    depth=new double[depth_length];
    type=new QString[depth_length];
    labid=new QString[depth_length];
    comment=new QString[depth_length];
    int count=0;
    for (int i=0;i<depth_length+1;i++){
        if (i!=n){
            depth[count]=depth_tmp[i];
            type[count]=type_tmp[i];
            labid[count]=labid_tmp[i];
            comment[count]=comment_tmp[i];
            for (int j=0;j<ageind;j++){
                age_data[count+(depth_length)*j]=data_tmp[i+(depth_length+1)*j];
            }
            count++;
        }

    }
    delete depth_tmp;
    delete type_tmp;
    delete labid_tmp;
    delete comment_tmp;
    delete data_tmp;
    sort(0,depth_length-1);
}

void AMData::addEntry(){
    //qDebug()<<"Generating Entry....";
    //temp
    double *depth_tmp=new double[get_Length()];
    QString *type_tmp=new QString[get_Length()];
    QString *labid_tmp=new QString[get_Length()];
    QString *comment_tmp=new QString[get_Length()];
    double *data_tmp=new double[get_Length()*ageind];
    for (int i=0;i<get_Length();i++){
        depth_tmp[i]=depth[i];
        type_tmp[i]=type[i];
        labid_tmp[i]=labid[i];
        comment_tmp[i]=comment[i];
        for (int j=0;j<ageind;j++){
            data_tmp[i+depth_length*j]=age_data[i+depth_length*j];
        }
    }

    delete depth;
    delete type;
    delete labid;
    delete comment;
    delete age_data;
    depth_length++;


    depth=new double[depth_length];
    type=new QString[depth_length];
    labid=new QString[depth_length];
    comment=new QString[depth_length];
    age_data=new double[depth_length*ageind];

    for (int i=0;i<depth_length-1;i++){

            depth[i]=depth_tmp[i];
            type[i]=type_tmp[i];
            labid[i]=labid_tmp[i];
            comment[i]=comment_tmp[i];
            for (int j=0;j<ageind;j++){
                age_data[i+(depth_length)*j]=data_tmp[i+(depth_length-1)*j];
            }



    }

    delete depth_tmp;
    delete type_tmp;
    delete labid_tmp;
    delete comment_tmp;
    delete data_tmp;

    // initialize new entry
    depth[depth_length-1]=0;
    type[depth_length-1]="hallo";
    labid[depth_length-1]="";
    comment[depth_length-1]="";
    for (int j=0;j<ageind;j++){
        set_Data(j,depth_length-1,0);
    }

    sort(0,depth_length-1);
    // funktioniert nicht richtig!!!! Glaub ich!?! Abwarten...
    //qDebug() <<"... Done";
}

void AMData::setSelect(int i){
    select=i;
}

int AMData::getSelect(){
    return select;
}

double AMData::get_Int_Value_Age(double depth,QString str){
    double yy=NAN;
    // Create Sublist of Agemodel
    // create subset of active Flag only
    // get length
    int active_count=0;
    for (int i=0;i<get_Length();i++) if (get_Data(7,i)==1) active_count++;

    double *temp_age;
    double *temp_depth;
    temp_age=new double[active_count];
    temp_depth=new double[active_count];
    int n=0;
    for (int i=0;i<get_Length();i++){
        if (get_Data(7,i)==1){
            temp_age[n]=get_Data(4,i);
            temp_depth[n]=get_Depth(i);
            n++;
        }
    }
    // Interpolation
    if (depth>=temp_depth[0] && depth<=temp_depth[active_count-1]){

    if (str=="linear"){
        int i=0;
        for (i=0;i<active_count;i++) if (temp_depth[i]>depth) break;
        if (i>0) yy=temp_age[i-1]+(temp_age[i]-temp_age[i-1])/(temp_depth[i]-temp_depth[i-1])*(depth-temp_depth[i-1]);

    }
    } else {
        yy=NAN;// nan would be better
    }
    delete temp_age;
    delete temp_depth;

    return yy;
}
