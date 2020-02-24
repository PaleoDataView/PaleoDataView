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

#include "amsdata.h"

AMSData::AMSData(Inventory *i):
    inv(i)
{
    select=0;
    resources=new Resources();
    cal_pdf=new double[0];

    intPDF=new double[0];
    intYear=new double[0];

    hpd_prop=new double[0];
    hpd_year=new double[0];

    cage=new double[0];
    dage=new double[0];
    uage=new double[0];

    cumPDF=new double[0];

    //**** Init Bacon Data Defaults
    // Bacon Ensamble Data
    bacon_out=new float[0];
    bacon_out_nrow=0;
    bacon_out_ncol=0;
    // Bacon Agemodel Data

    bacon_age_nrow=0;

    // Bacon Meta Data
    bacon_K=50;
    bacon_d_min=NAN;
    bacon_d_max=NAN;
    bacon_d_by=1;
    bacon_acc_shape=1.5;
    bacon_acc_mean=20;
    bacon_mem_strength=4;
    bacon_mem_mean=0.7;
    bacon_cc=1;
    bacon_cc1="IntCal13";
    bacon_cc2="Marine13";
    bacon_cc3="SHCal13";
    bacon_cc4="ConstCal";
    bacon_postbomb=0;
    //bacon_d_R=0;
    //bacon_d_STD=0;
    bacon_t_a=3;
    bacon_t_b=4;
    bacon_normal=0;
    bacon_suggest=1;
    bacon_th0=0;
    bacon_th0p=0;
    bacon_burnin=0;
    bacon_ssize=2000;
    bacon_yr_min=-1000;
    bacon_yr_max=1000000;

    // init String data
    data_Label.reserve(1);
    data_Label[0]=new QString();
    data_Label[0]->append("");
    data_Type.reserve(1);
    data_Type[0]=new QString();
    data_Type[0]->append("");
    data_Comment.reserve(1);
    data_Comment[0]=new QString();
    data_Comment[0]->append("");

}

AMSData::~AMSData(){
    //qDebug()<<"Cleaning Database...";


    //delete depth;

    //delete comment;
    //delete labid;
    //delete type;

    delete[] cal_pdf;

    delete[] intPDF;
    delete[] intYear;

    delete[] hpd_prop;
    delete[] hpd_year;

    delete[] cumPDF;

    delete[] cage;
    delete[] dage;
    delete[] uage;

    delete resources;

    delete[] bacon_out;


}
void AMSData::newAMS(int dlength){
    length=dlength;

    // creating init variable data of length
    data_Depth.clear();
    data_Depth.reserve(length);
    for(int i=0;i<length;i++) data_Depth.push_back(0);

    data_Sample_Thickness.clear();
    data_Sample_Thickness.reserve(length);
    for(int i=0;i<length;i++) data_Sample_Thickness.push_back(0);

    data_Label.clear();
    data_Label.reserve(length);
    for (int i=0;i<length;i++) {
        data_Label.push_back(new QString("na"));

    }

    data_Type.clear();
    data_Type.reserve(length);
    for (int i=0;i<length;i++) {
        data_Type.push_back(new QString("na"));

    }

    data_Age_dated.clear();
    data_Age_dated.reserve(length);
    for(int i=0;i<length;i++) data_Age_dated.push_back(0);

    data_Age_pErr.clear();
    data_Age_pErr.reserve(length);
    for(int i=0;i<length;i++) data_Age_pErr.push_back(0);

    data_Age_mErr.clear();
    data_Age_mErr.reserve(length);
    for(int i=0;i<length;i++) data_Age_mErr.push_back(0);

    data_Reservoir.clear();
    data_Reservoir.reserve(length);
    for(int i=0;i<length;i++) data_Reservoir.push_back(0);

    data_Reservoir_Err.clear();
    data_Reservoir_Err.reserve(length);
    for(int i=0;i<length;i++) data_Reservoir_Err.push_back(0);

    data_Cal_Age.clear();
    data_Cal_Age.reserve(length);
    for(int i=0;i<length;i++) data_Cal_Age.push_back(0);

    data_Cal_min.clear();
    data_Cal_min.reserve(length);
    for(int i=0;i<length;i++) data_Cal_min.push_back(0);

    data_Cal_max.clear();
    data_Cal_max.reserve(length);
    for(int i=0;i<length;i++) data_Cal_max.push_back(0);

    data_Use_Flag.clear();
    data_Use_Flag.reserve(length);
    for(int i=0;i<length;i++) data_Use_Flag.push_back(0);

    data_Comment.clear();
    data_Comment.reserve(length);
    for (int i=0;i<length;i++) {
        data_Comment.push_back(new QString("na"));

    }



}

void AMSData::set_Inventory(Inventory *i){
    inv=i;
}

void AMSData::AMSRead(){
    // get file name
   //qDebug()<<"Reading Age Model"+QString::number(inv->get_currentCore());
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
    size_t dimlen[2]={0};
    int retval;// for Errors

    // Open the file
    if (!(retval = nc_open(filename, NC_NOWRITE, &ncid))){


        // ******** read depth
        // get varid
        if ((retval = nc_inq_varid(ncid, "Depth", &varid)))
           ERR(retval);
        //Get Length of Age Model
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        length=dimlen[0];
        //qDebug() << "rows: "+QString::number(dimlen[0]);
        // read data

        data_Depth.clear();
        for (int i=0;i<length;i++) data_Depth.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Depth[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Depth[i]);

        // ************ Sample Thickness
        // get varid
        if ((retval = nc_inq_varid(ncid, "Sample Thickness", &varid)))
           ERR(retval);
        data_Sample_Thickness.clear();
        for (int i=0;i<length;i++) data_Sample_Thickness.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Sample_Thickness[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Sample_Thickness[i]);

        // ******* read Label
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "Label_Characters", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[1])))
           ERR(retval);

        if ((retval = nc_inq_varid(ncid, "Label", &varid)))
           ERR(retval);
        unsigned char text[dimlen[0]*dimlen[1]];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);
        data_Label.clear();

        for (int i=0;i<dimlen[0];i++){
            data_Label.push_back(new QString(""));
            for (int j=0;j<dimlen[1];j++){
                if (text[j*dimlen[0]+i]!=0){
                    data_Label[i]->append(text[j*dimlen[0]+i]);
                }
            }
          //qDebug() << data_Label[i]->toLatin1();
        }
        }

        // ******* read Type
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "Type_Characters", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[1])))
           ERR(retval);

        if ((retval = nc_inq_varid(ncid, "Type", &varid)))
           ERR(retval);
        unsigned char text[dimlen[0]*dimlen[1]];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);
        data_Type.clear();

        for (int i=0;i<dimlen[0];i++){
            data_Type.push_back(new QString(""));
            for (int j=0;j<dimlen[1];j++){
                if (text[j*dimlen[0]+i]!=0){
                    data_Type[i]->append(text[j*dimlen[0]+i]);
                }
            }
          //qDebug() << data_Type[i]->toLatin1();
        }
        }

        // ************ Age dated
        // get varid
        if ((retval = nc_inq_varid(ncid, "Age dated", &varid)))
           ERR(retval);
        data_Age_dated.clear();
        for (int i=0;i<length;i++) data_Age_dated.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Age_dated[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Age_dated[i]);

        // ************ Age pErr
        // get varid
        if ((retval = nc_inq_varid(ncid, "Age +Error", &varid)))
           ERR(retval);
        data_Age_pErr.clear();
        for (int i=0;i<length;i++) data_Age_pErr.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Age_pErr[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Age_pErr[i]);

        // ************ Age mErr
        // get varid
        if ((retval = nc_inq_varid(ncid, "Age -Error", &varid)))
           ERR(retval);
        data_Age_mErr.clear();
        for (int i=0;i<length;i++) data_Age_mErr.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Age_mErr[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Age_mErr[i]);

        // ************ Reservoir Age
        // get varid
        if ((retval = nc_inq_varid(ncid, "Reservoir Age", &varid)))
           ERR(retval);
        data_Reservoir.clear();
        for (int i=0;i<length;i++) data_Reservoir.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Reservoir[0]))) ERR(retval);
        //for (int i=0;i<length;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Reservoir[i]);

        // ************ Reservoir Age Error
        // get varid
        if ((retval = nc_inq_varid(ncid, "Reservoir Age Error", &varid)))
           ERR(retval);
        data_Reservoir_Err.clear();
        for (int i=0;i<length;i++) data_Reservoir_Err.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Reservoir_Err[0]))) ERR(retval);
        for (int i=0;i<length;i++)
           //qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Reservoir_Err[i]);

        // ************ Cal Age
        // get varid
        if ((retval = nc_inq_varid(ncid, "Cal Age", &varid)))
           ERR(retval);
        data_Cal_Age.clear();
        for (int i=0;i<length;i++) data_Cal_Age.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Cal_Age[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Cal_Age[i]);

        // ************ Cal min
        // get varid
        if ((retval = nc_inq_varid(ncid, "Cal Age Min", &varid)))
           ERR(retval);
        data_Cal_min.clear();
        for (int i=0;i<length;i++) data_Cal_min.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Cal_min[0]))) ERR(retval);
        //for (int i=0;i<length;i++)qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Cal_min[i]);

        // ************ Cal Max
        // get varid
        if ((retval = nc_inq_varid(ncid, "Cal Age Max", &varid)))
           ERR(retval);
        data_Cal_max.clear();
        for (int i=0;i<length;i++) data_Cal_max.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&data_Cal_max[0]))) ERR(retval);
        //for (int i=0;i<length;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Cal_max[i]);

        // ************ Use Flag
        // get varid
        if ((retval = nc_inq_varid(ncid, "Use Flag", &varid)))
           ERR(retval);
        data_Use_Flag.clear();
        for (int i=0;i<length;i++) data_Use_Flag.push_back(NAN);
        if ((retval = nc_get_var_int(ncid,varid,&data_Use_Flag[0]))) ERR(retval);
        //for (int i=0;i<length;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(data_Use_Flag[i]);

        // ******* read Comment
        {
        // get length
        if ((retval = nc_inq_dimid(ncid, "Comment_Characters", &varid)))
           ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[1])))
           ERR(retval);

        if ((retval = nc_inq_varid(ncid, "Comment", &varid)))
           ERR(retval);
        unsigned char text[dimlen[0]*dimlen[1]];
        if ((retval = nc_get_var(ncid,varid,&text[0]))) ERR(retval);
        data_Comment.clear();

        for (int i=0;i<dimlen[0];i++){
            data_Comment.push_back(new QString(""));
            for (int j=0;j<dimlen[1];j++){
                if (text[j*dimlen[0]+i]!=0){
                    data_Comment[i]->append(text[j*dimlen[0]+i]);
                }
            }
          //qDebug() << data_Comment[i]->toLatin1();
        }
        }

        // read bacon *******************************

        //**** Init Bacon Data Defaults
        // Bacon Ensamble Data
        delete[] bacon_out;
        bacon_out=new float[0];
        bacon_out_nrow=0;
        bacon_out_ncol=0;
        // Bacon Agemodel Data

        bacon_age_nrow=0;

        // Bacon Meta Data
        bacon_K=50;
        bacon_d_min=NAN;
        bacon_d_max=NAN;
        bacon_d_by=1;
        bacon_acc_shape=1.5;
        bacon_acc_mean=20;
        bacon_mem_strength=4;
        bacon_mem_mean=0.7;
        bacon_cc=1;
        bacon_cc1="IntCal13";
        bacon_cc2="Marine13";
        bacon_cc3="SHCal13";
        bacon_cc4="ConstCal";
        bacon_postbomb=0;
        //bacon_d_R=0;
        //bacon_d_STD=0;
        bacon_t_a=3;
        bacon_t_b=4;
        bacon_normal=0;
        bacon_suggest=1;
        bacon_th0=0;
        bacon_th0p=0;
        bacon_burnin=0;
        bacon_ssize=2000;
        bacon_yr_min=-1000;
        bacon_yr_max=100000;

        // check if bacon data is present
        if ((retval = nc_inq_dimid(ncid, "bacon_out_ncol", &varid))){

        }else{
        // read bacon out
        // get varid
        if ((retval = nc_inq_dimid(ncid, "bacon_out_ncol", &varid)))
           ERR(retval);
        // get size
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        bacon_out_ncol=dimlen[0];

        if ((retval = nc_inq_dimid(ncid, "bacon_out_nrow", &varid)))
           ERR(retval);
        // get size
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        bacon_out_nrow=dimlen[0];


        // read data
        if ((retval = nc_inq_varid(ncid, "bacon_data", &varid)))
           ERR(retval);
        delete[] bacon_out;
        bacon_out=new float[bacon_out_ncol*bacon_out_nrow];
        if ((retval = nc_get_var_float(ncid,varid,&bacon_out[0]))) ERR(retval);



        if ((retval = nc_inq_dimid(ncid, "Bacon Length", &varid)))
           ERR(retval);
        // get size
        if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
           ERR(retval);
        bacon_age_nrow=dimlen[0];


        // read age data
        // read Depth
        if ((retval = nc_inq_varid(ncid, "Bacon Depth", &varid)))
           ERR(retval);
        bacon_Depth.clear();
        for (int i=0;i<bacon_age_nrow;i++) bacon_Depth.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&bacon_Depth[0]))) ERR(retval);
        //for (int i=0;i<bacon_age_nrow;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(bacon_Depth[i]);

        // read Mean
        if ((retval = nc_inq_varid(ncid, "Bacon Mean Age", &varid)))
           ERR(retval);
        bacon_Mean.clear();
        for (int i=0;i<bacon_age_nrow;i++) bacon_Mean.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&bacon_Mean[0]))) ERR(retval);
        //for (int i=0;i<bacon_age_nrow;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(bacon_Mean[i]);

        // read Q5
        if ((retval = nc_inq_varid(ncid, "Bacon 5% Quantile", &varid)))
           ERR(retval);
        bacon_Q5.clear();
        for (int i=0;i<bacon_age_nrow;i++) bacon_Q5.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&bacon_Q5[0]))) ERR(retval);
        //for (int i=0;i<bacon_age_nrow;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(bacon_Q5[i]);

        // read Median
        if ((retval = nc_inq_varid(ncid, "Bacon Median", &varid)))
           ERR(retval);
        bacon_Median.clear();
        for (int i=0;i<bacon_age_nrow;i++) bacon_Median.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&bacon_Median[0]))) ERR(retval);
        //for (int i=0;i<bacon_age_nrow;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(bacon_Median[i]);

        // read Q95
        if ((retval = nc_inq_varid(ncid, "Bacon 95% Quantile", &varid)))
           ERR(retval);
        bacon_Q95.clear();
        for (int i=0;i<bacon_age_nrow;i++) bacon_Q95.push_back(NAN);
        if ((retval = nc_get_var_double(ncid,varid,&bacon_Q95[0]))) ERR(retval);
        //for (int i=0;i<bacon_age_nrow;i++) qDebug() << "index: "+QString::number(i)+" : "+QString::number(bacon_Q95[i]);

        /*
        if ((retval = nc_inq_varid(ncid, "bacon_age", &varid)))
           ERR(retval);
        bacon_age=new float[bacon_age_ncol*bacon_age_nrow];
        if ((retval = nc_get_var_float(ncid,varid,&bacon_age[0]))) ERR(retval);
        */
        // read bacon_attributes
        // 1:bacon_K
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_K",&bacon_K)))
            ERR(retval);
        // 2:bacon_d_min
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_d_min",&bacon_d_min)))
            ERR(retval);
        // 3:bacon_d_max
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_d_max",&bacon_d_max)))
            ERR(retval);
        // 4:bacon_d_by
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_d_by",&bacon_d_by)))
            ERR(retval);
        // 5:bacon_acc_shape
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_acc_shape",&bacon_acc_shape)))
            ERR(retval);
        // 6:bacon_acc_mean
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_acc_mean",&bacon_acc_mean)))
            ERR(retval);
        // 7:bacon_mem_strength
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_mem_strength",&bacon_mem_strength)))
            ERR(retval);
        // 8:bacon_mem_mean
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_mem_mean",&bacon_mem_mean)))
            ERR(retval);
        // 9:bacon_cc
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_cc",&bacon_cc)))
            ERR(retval);
        // 10:bacon_cc1
        size_t attlen[1]={0};
        if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,"bacon_cc1",&attlen[0])))
           ERR(retval);
        unsigned char text[attlen[0]+1];
        for (int ii=0;ii<attlen[0]+1;ii++) text[ii]=' ';
        if ((retval = nc_get_att(ncid,NC_GLOBAL,"bacon_cc1",&text[0])))
           ERR(retval);
        bacon_cc1="";
        for (unsigned int j=0;j<attlen[0];j++){
                bacon_cc1.append(text[j]);
        }
        // 11:bacon_cc2
        if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,"bacon_cc2",&attlen[0])))
           ERR(retval);
        unsigned char text2[attlen[0]+1];
        for (int ii=0;ii<attlen[0]+1;ii++) text2[ii]=' ';
        if ((retval = nc_get_att(ncid,NC_GLOBAL,"bacon_cc2",&text2[0])))
           ERR(retval);
        bacon_cc2="";
        for (unsigned int j=0;j<attlen[0];j++){
                bacon_cc2.append(text2[j]);
        }
        // 12:bacon_cc3
        if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,"bacon_cc3",&attlen[0])))
           ERR(retval);
        unsigned char text3[attlen[0]+1];
        for (int ii=0;ii<attlen[0]+1;ii++) text3[ii]=' ';
        if ((retval = nc_get_att(ncid,NC_GLOBAL,"bacon_cc3",&text3[0])))
           ERR(retval);
        bacon_cc3="";
        for (unsigned int j=0;j<attlen[0];j++){
                bacon_cc3.append(text3[j]);
        }
        // 13:bacon_cc4
        if ((retval = nc_inq_attlen(ncid,NC_GLOBAL,"bacon_cc4",&attlen[0])))
           ERR(retval);
        unsigned char text4[attlen[0]+1];
        for (int ii=0;ii<attlen[0]+1;ii++) text4[ii]=' ';
        if ((retval = nc_get_att(ncid,NC_GLOBAL,"bacon_cc4",&text4[0])))
           ERR(retval);
        bacon_cc4="";
        for (unsigned int j=0;j<attlen[0];j++){
                bacon_cc4.append(text4[j]);
        }
        // 14:bacon_postbomb
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_postbomb",&bacon_postbomb)))
            ERR(retval);
        /* 15:bacon_d_R
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_d_R",&bacon_d_R)))
            ERR(retval);
        // 16:bacon_d_STD
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_d_STD",&bacon_d_STD)))
            ERR(retval);*/
        // 17:bacon_t_a
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_t_a",&bacon_t_a)))
            ERR(retval);
        // 18:bacon_t_b
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_t_b",&bacon_t_b)))
            ERR(retval);
        // 19:bacon_normal
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_normal",&bacon_normal)))
            ERR(retval);
        // 20:bacon_suggest
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_suggest",&bacon_suggest)))
            ERR(retval);
        // 21:bacon_th0
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_th0",&bacon_th0)))
            ERR(retval);
        // 22:bacon_th0p
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_th0p",&bacon_th0p)))
            ERR(retval);
        // 23:bacon_burnin
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_burnin",&bacon_burnin)))
            ERR(retval);
        // 24:bacon_ssize
        if ((retval = nc_get_att_int(ncid,NC_GLOBAL,"bacon_ssize",&bacon_ssize)))
            ERR(retval);
        // 25:bacon_yr_min
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_yr_min",&bacon_yr_min)))
            ERR(retval);
        // 26:bacon_yr_max
        if ((retval = nc_get_att_float(ncid,NC_GLOBAL,"bacon_yr_max",&bacon_yr_max)))
            ERR(retval);
        }
        // *********************************************




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
        newAMS(1);
        data_Depth[0]=NAN;
        data_Sample_Thickness[0]=NAN;
        data_Label[0]=new QString("na");
        data_Type[0]=new QString("na");
        data_Age_dated[0]=NAN;
        data_Age_pErr[0]=NAN;
        data_Age_mErr[0]=NAN;
        data_Reservoir[0]=NAN;
        data_Reservoir_Err[0]=NAN;
        data_Cal_Age[0]=NAN;
        data_Cal_min[0]=NAN;
        data_Cal_max[0]=NAN;
        data_Use_Flag[0]=0;
        data_Comment[0]=new QString("Default entry of new age model : Please Edit");
    }
    select=0;
    delete filename;
}

void AMSData::AMSSave(){
    // get file name
    //qDebug()<<"Saving Age Model";
    QString name=inv->get_Core(inv->get_currentCore());
            name.replace('\0',' ');
            name.replace(" ","");
    Save(name);
}


void AMSData::AMSSaveData(){
    // get file name
    //qDebug()<<"Saving Age Model";
    QString name=inv->get_att_Core();
    name.replace('\0',' ');
    name.replace(" ","");
    Save(name);
}

void AMSData::Save(QString name){
    QString QFilename=resources->path_age+name+".age";
    QFilename.replace(QString(".nc"),QString(".age"));
    //qDebug() << QFilename;
    char* filename;
    string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());

   //qDebug()<<"saving...";

    // Initialize ids and meta
    int ncid;
    int retval;// for Errors

    // Open the file
    if (!(retval = nc_create(filename, NC_WRITE, &ncid))){
        // Define variables
       //qDebug() << "DefineMode";

        // get length of age model
        int length_dim;
        int dim[1];
        if ((retval = nc_def_dim(ncid,"Length",get_Length(),&length_dim)))
           ERR(retval);
        dim[0]=length_dim;

        // Depth
        int depth_id;
        if ((retval = nc_def_var(ncid,"Depth",NC_DOUBLE,1,dim,&depth_id)))
            ERR(retval);
       //qDebug() << depth_id;

        // Sample Thickness
        int sample_id;
        if ((retval = nc_def_var(ncid,"Sample Thickness",NC_DOUBLE,1,dim,&sample_id)))
            ERR(retval);
       //qDebug() << sample_id;

        // Label
        int label_length=0;
        for (int i=0;i<get_Length();i++) if (label_length<data_Label[i]->length()) label_length=data_Label[i]->length();
        int llabel_dim;
        if ((retval = nc_def_dim(ncid,"Label_Characters",label_length,&llabel_dim)))
           ERR(retval);
        int dimm[2];
        dimm[0]=llabel_dim;
        dimm[1]=length_dim;
        int label_id;
        if ((retval = nc_def_var(ncid,"Label",NC_CHAR,2,dimm,&label_id)))
            ERR(retval);
       //qDebug() << label_id;

        // Type
        int type_length=0;
        for (int i=0;i<get_Length();i++) if (type_length<data_Type[i]->length()) type_length=data_Type[i]->length();
        int ltype_dim;
        if ((retval = nc_def_dim(ncid,"Type_Characters",type_length,&ltype_dim)))
           ERR(retval);

        dimm[0]=ltype_dim;
        dimm[1]=length_dim;
        int type_id;
        if ((retval = nc_def_var(ncid,"Type",NC_CHAR,2,dimm,&type_id)))
            ERR(retval);
       //qDebug() << type_id;

        // Age dated
        int age_dated_id;
        if ((retval = nc_def_var(ncid,"Age dated",NC_DOUBLE,1,dim,&age_dated_id)))
            ERR(retval);
       //qDebug() << age_dated_id;

        // Age pErr
        int age_pErr_id;
        if ((retval = nc_def_var(ncid,"Age +Error",NC_DOUBLE,1,dim,&age_pErr_id)))
            ERR(retval);
       //qDebug() << age_pErr_id;

        // Age mErr
        int age_mErr_id;
        if ((retval = nc_def_var(ncid,"Age -Error",NC_DOUBLE,1,dim,&age_mErr_id)))
            ERR(retval);
       //qDebug() << age_mErr_id;

        // Reservoir Age
        int reservoir_id;
        if ((retval = nc_def_var(ncid,"Reservoir Age",NC_DOUBLE,1,dim,&reservoir_id)))
            ERR(retval);
       //qDebug() << reservoir_id;

        // Reservoir Age Error
        int reservoir_err_id;
        if ((retval = nc_def_var(ncid,"Reservoir Age Error",NC_DOUBLE,1,dim,&reservoir_err_id)))
            ERR(retval);
       //qDebug() << reservoir_err_id;

        // Cal Age
        int cal_age_id;
        if ((retval = nc_def_var(ncid,"Cal Age",NC_DOUBLE,1,dim,&cal_age_id)))
            ERR(retval);
       //qDebug() << cal_age_id;

        // Cal Age min
        int cal_age_min_id;
        if ((retval = nc_def_var(ncid,"Cal Age Min",NC_DOUBLE,1,dim,&cal_age_min_id)))
            ERR(retval);
       //qDebug() << cal_age_min_id;

        // Cal Age max
        int cal_age_max_id;
        if ((retval = nc_def_var(ncid,"Cal Age Max",NC_DOUBLE,1,dim,&cal_age_max_id)))
            ERR(retval);
       //qDebug() << cal_age_max_id;

        // Use Flag
        int use_flag_id;
        if ((retval = nc_def_var(ncid,"Use Flag",NC_INT,1,dim,&use_flag_id)))
            ERR(retval);
       //qDebug() << use_flag_id;

        // Comment
        int comment_length=0;
        for (int i=0;i<get_Length();i++) if (comment_length<data_Comment[i]->length()) comment_length=data_Comment[i]->length();
        int lcom_dim;
        if ((retval = nc_def_dim(ncid,"Comment_Characters",comment_length,&lcom_dim)))
           ERR(retval); 

        dimm[0]=lcom_dim;
        dimm[1]=length_dim;
       //qDebug()<<"test";
        int comment_id;
        if ((retval = nc_def_var(ncid,"Comment",NC_CHAR,2,dimm,&comment_id)))
            ERR(retval);
       //qDebug() << comment_id;
       //qDebug() << "Age Model Data Defined";
        // *******************************************
        // define bacon out data
        // out data

        int out_nrow_dim;
        int out_ncol_dim;
        int out_id;
        if (bacon_out_nrow>0){

            if ((retval = nc_def_dim(ncid,"bacon_out_nrow",bacon_out_nrow,&out_nrow_dim)))
               ERR(retval);

            if ((retval = nc_def_dim(ncid,"bacon_out_ncol",bacon_out_ncol,&out_ncol_dim)))
               ERR(retval);

            dimm[0]=out_nrow_dim;
            dimm[1]=out_ncol_dim;
           //qDebug()<<dim[0];
           //qDebug()<<dim[1];
            if ((retval = nc_def_var(ncid,"bacon_data",NC_FLOAT,2,dimm,&out_id)))
                ERR(retval);
        }
        // define bacon age data
        // age data

        int bacon_length_dim;
        int bacon_dim[1];
        int bacon_depth_id;
        int bacon_mean_age_id;
        int bacon_Q5_id;
        int bacon_median_id;
        int bacon_Q95_id;
        if (bacon_age_nrow>0){
            // get length of bacon age model

            if ((retval = nc_def_dim(ncid,"Bacon Length",bacon_Depth.size(),&bacon_length_dim)))
               ERR(retval);
            bacon_dim[0]=bacon_length_dim;
            // Bacon Depth

            if ((retval = nc_def_var(ncid,"Bacon Depth",NC_DOUBLE,1,bacon_dim,&bacon_depth_id)))
                ERR(retval);
            //qDebug() << bacon_depth_id;

            // Bacon Mean Age

            if ((retval = nc_def_var(ncid,"Bacon Mean Age",NC_DOUBLE,1,bacon_dim,&bacon_mean_age_id)))
                ERR(retval);
            //qDebug() << bacon_mean_age_id;

            // Bacon 5%Quantile

            if ((retval = nc_def_var(ncid,"Bacon 5% Quantile",NC_DOUBLE,1,bacon_dim,&bacon_Q5_id)))
                ERR(retval);
            //qDebug() << bacon_Q5_id;

            // Bacon Median

            if ((retval = nc_def_var(ncid,"Bacon Median",NC_DOUBLE,1,bacon_dim,&bacon_median_id)))
                ERR(retval);
            //qDebug() << bacon_median_id;

            // Bacon 95%Quantile

            if ((retval = nc_def_var(ncid,"Bacon 95% Quantile",NC_DOUBLE,1,bacon_dim,&bacon_Q95_id)))
                ERR(retval);
            //qDebug() << bacon_Q95_id;

            /*
            if ((retval = nc_def_dim(ncid,"bacon_age_nrow",bacon_age_nrow,&age_nrow_dim)))
               ERR(retval);

            if ((retval = nc_def_dim(ncid,"bacon_age_ncol",bacon_age_ncol,&age_ncol_dim)))
               ERR(retval);

            dim[0]=age_nrow_dim;
            dim[1]=age_ncol_dim;

            if ((retval = nc_def_var(ncid,"bacon_age",NC_FLOAT,2,dim,&age_id)))
                ERR(retval);*/
        }
        //**********************************************

        // end definition mode
        if ((retval = nc_enddef(ncid)))
            ERR(retval);
       //qDebug()<<"defined all variables/attributes";
        // ******** save depth
        if ((retval = nc_put_var(ncid,depth_id,&data_Depth[0])))
            ERR(retval);
       //qDebug() << "Depth written";

        // ******** save sample_thickness
        if ((retval = nc_put_var(ncid,sample_id,&data_Sample_Thickness[0])))
            ERR(retval);
       //qDebug() << "Sample written";

        // ******* write Label
        {
        unsigned char text[label_length][get_Length()];
        for(int i=0;i<label_length;i++) for(int ii=0;ii<get_Length();ii++) text[i][ii]=' ';
        for (int i=0;i<get_Length();i++){
            for (int j=0;j<data_Label[i]->length();j++) text[j][i]=data_Label[i]->at(j).toLatin1();
        }
        if ((retval = nc_put_var(ncid,label_id,&text[0][0])))
            ERR(retval);
       //qDebug() << "Label written";
        }

        // ******* write Type
        {
        unsigned char text[type_length][get_Length()];
        for(int i=0;i<type_length;i++) for(int ii=0;ii<get_Length();ii++) text[i][ii]=' ';
        for (int i=0;i<get_Length();i++){
            for (int j=0;j<data_Type[i]->length();j++) text[j][i]=data_Type[i]->at(j).toLatin1();
        }
        if ((retval = nc_put_var(ncid,type_id,&text[0][0])))
            ERR(retval);
       //qDebug() << "Type written";
        }

        // ******** save age dated
        if ((retval = nc_put_var(ncid,age_dated_id,&data_Age_dated[0])))
            ERR(retval);
       //qDebug() << "Age dated written";

        // ******** save age mErr
        if ((retval = nc_put_var(ncid,age_mErr_id,&data_Age_mErr[0])))
            ERR(retval);
       //qDebug() << "Age mErr written";

        // ******** save age pErr
        if ((retval = nc_put_var(ncid,age_pErr_id,&data_Age_pErr[0])))
            ERR(retval);
       //qDebug() << "Age pErr written";

        // ******** save Reservoir
        if ((retval = nc_put_var(ncid,reservoir_id,&data_Reservoir[0])))
            ERR(retval);
       //qDebug() << "Reservoir written";

        // ******** save Reservoir Errror
        if ((retval = nc_put_var(ncid,reservoir_err_id,&data_Reservoir_Err[0])))
            ERR(retval);
       //qDebug() << "Reservoir Error written";

        // ******** save Cal Age
        if ((retval = nc_put_var(ncid,cal_age_id,&data_Cal_Age[0])))
            ERR(retval);
       //qDebug() << "Cal Age written";

        // ******** save Cal Age Min
        if ((retval = nc_put_var(ncid,cal_age_min_id,&data_Cal_min[0])))
            ERR(retval);
       //qDebug() << "Cal Min written";

        // ******** save Cal Age Max
        if ((retval = nc_put_var(ncid,cal_age_max_id,&data_Cal_max[0])))
            ERR(retval);
       //qDebug() << "Cal Max written";

        // ******** save Use Flag
        if ((retval = nc_put_var(ncid,use_flag_id,&data_Use_Flag[0])))
            ERR(retval);
       //qDebug() << "Use Flag written";

        // ******* write comment
        {
        unsigned char text[comment_length][get_Length()];
        for(int i=0;i<comment_length;i++) for(int ii=0;ii<get_Length();ii++) text[i][ii]=' ';

        for (int i=0;i<get_Length();i++){
            for (int j=0;j<data_Comment[i]->length();j++) text[j][i]=data_Comment[i]->at(j).toLatin1();
        }
        if ((retval = nc_put_var(ncid,comment_id,&text[0][0])))
            ERR(retval);
       //qDebug() << "Comments written";
        }

        // ******** saving bacon
        if (bacon_out_nrow>0){
            if ((retval = nc_put_var(ncid,out_id,bacon_out)))
                ERR(retval);
            // write bacon age model
            // ******** save bacon depth
            if ((retval = nc_put_var(ncid,bacon_depth_id,&bacon_Depth[0])))
                ERR(retval);
           //qDebug() << "Bacon Depth written";
            // ******** save bacon mean
            if ((retval = nc_put_var(ncid,bacon_mean_age_id,&bacon_Mean[0])))
                ERR(retval);
           //qDebug() << "Bacon Mean written";
            // ******** save bacon 5%
            if ((retval = nc_put_var(ncid,bacon_Q5_id,&bacon_Q5[0])))
                ERR(retval);
           //qDebug() << "Bacon q5 written";
            // ******** save bacon Median
            if ((retval = nc_put_var(ncid,bacon_median_id,&bacon_Median[0])))
                ERR(retval);
           //qDebug() << "Bacon median written";
            // ******** save bacon 95%
            if ((retval = nc_put_var(ncid,bacon_Q95_id,&bacon_Q95[0])))
                ERR(retval);
           //qDebug() << "Bacon q95 written";



            // activate defMode
            if ((retval = nc_redef(ncid)))
                ERR(retval);

            // write attributes
            // 1:bacon_K
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_K",NC_INT,1,&bacon_K)))
                ERR(retval);
            // 2:bacon_d_min
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_d_min",NC_FLOAT,1,&bacon_d_min)))
                ERR(retval);
            // 3:bacon_d_max
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_d_max",NC_FLOAT,1,&bacon_d_max)))
                ERR(retval);
            // 4:bacon_d_by
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_d_by",NC_FLOAT,1,&bacon_d_by)))
                ERR(retval);
            // 5:bacon_acc_shape
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_acc_shape",NC_FLOAT,1,&bacon_acc_shape)))
                ERR(retval);
            // 6:bacon_acc_mean
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_acc_mean",NC_FLOAT,1,&bacon_acc_mean)))
                ERR(retval);
            // 7:bacon_mem_strength
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_mem_strength",NC_FLOAT,1,&bacon_mem_strength)))
                ERR(retval);
            // 8:bacon_mem_mean
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_mem_mean",NC_FLOAT,1,&bacon_mem_mean)))
                ERR(retval);
            // 9:bacon_cc
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_cc",NC_INT,1,&bacon_cc)))
                ERR(retval);
            // 10:bacon_cc1
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"bacon_cc1",NC_CHAR,bacon_cc1.length(),bacon_cc1.toLatin1().data())))
                ERR(retval);
            // 11:bacon_cc2
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"bacon_cc2",NC_CHAR,bacon_cc2.length(),bacon_cc2.toLatin1().data())))
                ERR(retval);
            // 12:bacon_cc3
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"bacon_cc3",NC_CHAR,bacon_cc3.length(),bacon_cc3.toLatin1().data())))
                ERR(retval);
            // 13:bacon_cc4
            if ((retval = nc_put_att(ncid,NC_GLOBAL,"bacon_cc4",NC_CHAR,bacon_cc4.length(),bacon_cc4.toLatin1().data())))
                ERR(retval);
            // 14:bacon_postbomb
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_postbomb",NC_INT,1,&bacon_postbomb)))
                ERR(retval);
            /* 15:bacon_d_R
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_d_R",NC_FLOAT,1,&bacon_d_R)))
                ERR(retval);
            // 16:bacon_d_STD
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_d_STD",NC_FLOAT,1,&bacon_d_STD)))
                ERR(retval);*/
            // 17:bacon_t_a
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_t_a",NC_FLOAT,1,&bacon_t_a)))
                ERR(retval);
            // 18:bacon_t_b
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_t_b",NC_FLOAT,1,&bacon_t_b)))
                ERR(retval);
            // 19:bacon_normal
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_normal",NC_INT,1,&bacon_normal)))
                ERR(retval);
            // 20:bacon_suggest
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_suggest",NC_INT,1,&bacon_suggest)))
                ERR(retval);
            // 21:bacon_th0
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_th0",NC_FLOAT,1,&bacon_th0)))
                ERR(retval);
            // 22:bacon_th0p
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_th0p",NC_FLOAT,1,&bacon_th0p)))
                ERR(retval);
            // 23:bacon_burnin
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_burnin",NC_INT,1,&bacon_burnin)))
                ERR(retval);
            // 24:bacon_ssize
            if ((retval = nc_put_att_int(ncid,NC_GLOBAL,"bacon_ssize",NC_INT,1,&bacon_ssize)))
                ERR(retval);
            // 25:bacon_yr_min
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_yr_min",NC_FLOAT,1,&bacon_yr_min)))
                ERR(retval);
            // 26:bacon_yr_max
            if ((retval = nc_put_att_float(ncid,NC_GLOBAL,"bacon_yr_max",NC_FLOAT,1,&bacon_yr_max)))
                ERR(retval);

        }
        // *******************************************************

       //qDebug()<<"data written";

        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);
        // Set AgeModel_Flag
        inv->set_AgeModel(inv->get_currentCore(),1);

    } else {

        if (nc_strerror(retval)=="No such file or directory") {
           //qDebug() << "Start generating new File...";
        } else {

            //ERR(retval);
        }
    }
    delete filename;
   //qDebug()<<"saving finished";
}

double AMSData::get_Depth(int n){
    return data_Depth[n];
}

double AMSData::get_Sample_Thickness(int n){
    return data_Sample_Thickness[n];
}

int AMSData::get_Length(){
    return length;
}

QString AMSData::get_Age_Comment(int n){
    return data_Comment[n]->toLatin1();
}

QString AMSData::get_Type(int n){
    return data_Type[n]->simplified().toLatin1();
}

QString AMSData::get_LabID(int n){
    return data_Label[n]->toLatin1();
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
double AMSData::get_Data(int c,int n){
    double v=NAN;
    if (n<length){
        if (c==0) v=data_Age_dated[n];
        if (c==1) v=data_Age_pErr[n];
        if (c==2) v=data_Age_mErr[n];
        if (c==3) v=data_Reservoir[n];
        if (c==4) v=data_Cal_Age[n];
        if (c==5) v=data_Cal_min[n];
        if (c==6) v=data_Cal_max[n];
        if (c==7) v=data_Use_Flag[n];
    }
    return v;
}

double AMSData::get_Reservoir_Error(int n){
    return data_Reservoir_Err[n];
}

void AMSData::set_Depth(int n,double v){
    data_Depth[n]=v;

}

void AMSData::set_Sample_Thickness(int n, double v){
    data_Sample_Thickness[n]=v;
}

void AMSData::set_Age_Comment(QString str,int n){
    data_Comment[n]->clear();
    data_Comment[n]=new QString(str);
}

void AMSData::set_Type(int n,QString str){
    data_Type[n]->clear();
    data_Type[n]=new QString(str);
}

void AMSData::set_LabID(int n,QString str){
    data_Label[n]->clear();
    data_Label[n]=new QString(str);
}

//************************************
// Writes data to Array
// c=0 Age Dated
// c=1 Age + error
// c=2 Age - error
// c=3 Age Res.
// c=4 Cal
// c=5 Cal min
// c=6 Cal max
// c=7 Use Flag
//*************************************
void AMSData::set_Data(int c,int n,double v){
    if (n<length){
        if (c==0) data_Age_dated[n]=v;
        if (c==1) data_Age_pErr[n]=v;
        if (c==2) data_Age_mErr[n]=v;
        if (c==3) data_Reservoir[n]=v;
        if (c==4) data_Cal_Age[n]=v;
        if (c==5) data_Cal_min[n]=v;
        if (c==6) data_Cal_max[n]=v;
        if (c==7) data_Use_Flag[n]=(int) v;
    }


}
void AMSData::set_Reservoir_Error(int n, double v){
    data_Reservoir_Err[n]=v;
}

void AMSData::ReadCalib(){
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
    delete[] dage;
    delete[] cage;
    delete[] uage;
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

double AMSData::get_dage(int n){
    return dage[n];
}

double AMSData::get_cage(int n){
    return cage[n];
}

double AMSData::get_dage_fmu(int n){
    return exp(-dage[n]/8033.0);
}

double AMSData::get_uage(int n){
    return uage[n];
}

double AMSData::get_uage_fmu(int n){
    return get_dage_fmu(n)-exp(-(dage[n]+uage[n])/8033.0);
}

int AMSData::get_Calib_Length(){
    return Calib_Length;
}

void AMSData::setCalibFile(QString file){
    resources->filename_C14_Cal=file;
}

double AMSData::calc_CalPDF(double value,double error){
    delete[] cal_pdf;

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

double AMSData::get_CalPDF(int i){
    return cal_pdf[i];
}

double AMSData::get_DPDF(double value,double error,double dage){

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

double AMSData::get_CalPDF_max(){
    return cal_pdf_max;
}

void AMSData::calc_intPDF(){
    // estimate length of vector
    intPDF_Size=0;
    for (int i=0;i<Calib_Length-1;i++){
        intPDF_Size=intPDF_Size+(int)(get_cage(i+1)-get_cage(i));
    }
    //qDebug() << "PDF Size :"+QString::number(intPDF_Size);
    delete[] intPDF;
    delete[] intYear;
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

double AMSData::get_intYear(int n){
    return intYear[n];
}

double AMSData::get_intPDF(int n){
    return intPDF[n];
}

int AMSData::get_int_Length(){
    return intPDF_Size;
}

void AMSData::normPDF(){
    double sum=0;
    for(int i=0;i<intPDF_Size;i++) sum+=intPDF[i];
    for(int i=0;i<intPDF_Size;i++) intPDF[i]=intPDF[i]/sum; 
}
void AMSData::cummulativePDF(){
    //Calculate Cumulative PDF from intPDF
    delete[] cumPDF;
    cumPDF=new double[intPDF_Size];
    double cum=0;
    for(int i=0;i<intPDF_Size;i++){
        cum+=intPDF[i];
        cumPDF[i]=cum;

    }
   //qDebug()<<QString::number(cumPDF[intPDF_Size-1]);
}

void AMSData::sortInt(){
    normPDF();
    cummulativePDF();
    // sort after intPDF;
    Sort_intPDF(0,intPDF_Size-1);

    // Estimate 95% Range
    // number of values lower<sum95%

    double cumprop=0;
    int count=0;
    for (int i=0;i<intPDF_Size-1;i++){
        cumprop+=intPDF[i];
        if(cumprop<0.954){

        } else {
            count=i;
            break;
        }
    }
    hpd_length=count;
    cumprop=0;
    delete[] hpd_prop;
    delete[] hpd_year;
    hpd_prop=new double [hpd_length];
    hpd_year=new double [hpd_length];
    int index=0;
    count=0;
    for (int i=0;i<intPDF_Size-1;i++){
        cumprop+=intPDF[i];
        if(cumprop<0.954){
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

double AMSData::get_Cal95U(){
    return Cal95U;
}

double AMSData::get_Cal95L(){
    return Cal95L;
}

double AMSData::get_CalMedian(){
    return CalMedian;
}

double AMSData::get_CalMidAge(){
    return CalMidAge;
}

double AMSData::get_CalWMean(){
    return CalWMean;
}

void AMSData::Sort_intPDF(int left,int right){
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

void AMSData::Sort_intYear(int left,int right){
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

void AMSData::Sort_hpd_year(int left,int right){
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

void AMSData::sort(int left,int right){// sort for Depth(NOT Age)
    // Using QuickSort
    int i = left, j = right;
    double tmp;
    QString *tmp_str;
    double pivot = data_Depth[(left + right) / 2];
    while (i <= j) {
        while (data_Depth[i] < pivot) i++;
        while (data_Depth[j] > pivot) j--;
        if (i <= j) {
            // exchange everything
            tmp = data_Depth[i];
            data_Depth[i] = data_Depth[j];
            data_Depth[j] = tmp;

            tmp = data_Sample_Thickness[i];
            data_Sample_Thickness[i] = data_Sample_Thickness[j];
            data_Sample_Thickness[j] = tmp;

            tmp = data_Reservoir_Err[i];
            data_Reservoir_Err[i] = data_Reservoir_Err[j];
            data_Reservoir_Err[j] = tmp;

            tmp_str = data_Label[i];
            data_Label[i] = data_Label[j];
            data_Label[j] = tmp_str;

            tmp_str = data_Type[i];
            data_Type[i] = data_Type[j];
            data_Type[j] = tmp_str;

            tmp_str = data_Comment[i];
            data_Comment[i] = data_Comment[j];
            data_Comment[j] = tmp_str;

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

void AMSData::deleteEntry(int n){
    data_Depth.erase(data_Depth.begin()+n);
    data_Sample_Thickness.erase(data_Sample_Thickness.begin()+n);
    data_Label.erase(data_Label.begin()+n);
    data_Type.erase(data_Type.begin()+n);
    data_Age_dated.erase(data_Age_dated.begin()+n);
    data_Age_pErr.erase(data_Age_pErr.begin()+n);
    data_Age_mErr.erase(data_Age_mErr.begin()+n);
    data_Reservoir.erase(data_Reservoir.begin()+n);
    data_Reservoir_Err.erase(data_Reservoir_Err.begin()+n);
    data_Cal_Age.erase(data_Cal_Age.begin()+n);
    data_Cal_min.erase(data_Cal_min.begin()+n);
    data_Cal_max.erase(data_Cal_max.begin()+n);
    data_Use_Flag.erase(data_Use_Flag.begin()+n);
    data_Comment.erase(data_Comment.begin()+n);
    length--;
    sort(0,length-1);
}

void AMSData::addEntry(){
    data_Depth.push_back(0);
    data_Sample_Thickness.push_back(NAN);
    data_Label.push_back(new QString("na"));
    data_Type.push_back(new QString("na"));
    data_Age_dated.push_back(NAN);
    data_Age_pErr.push_back(NAN);
    data_Age_mErr.push_back(NAN);
    data_Reservoir.push_back(NAN);
    data_Reservoir_Err.push_back(NAN);
    data_Cal_Age.push_back(NAN);
    data_Cal_min.push_back(NAN);
    data_Cal_max.push_back(NAN);
    data_Use_Flag.push_back(0);
    data_Comment.push_back(new QString("Default entry of new age model : Please Edit"));
    length++;
    sort(0,length-1);
}
void AMSData::setSelect(int i){
    select=i;
}

int AMSData::getSelect(){
    return select;
}

double AMSData::get_Int_Value_Age(double depth,QString str){

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
            for (i=1;i<active_count;i++) if (temp_depth[i]>=depth) break;
            yy=temp_age[i-1]+(temp_age[i]-temp_age[i-1])/(temp_depth[i]-temp_depth[i-1])*(depth-temp_depth[i-1]);

        }
    } else {
        yy=NAN;
    }
    delete[] temp_age;
    delete[] temp_depth;

    return yy;
}

double AMSData::get_Int_Value_Depth(double age,QString str){

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
    if (age>=temp_age[0] && age<=temp_age[active_count-1]){

    if (str=="linear"){
        int i=0;
        for (i=0;i<active_count;i++) if (temp_age[i]>age) break;
        if (i>0) yy=temp_depth[i-1]+(temp_depth[i]-temp_depth[i-1])/(temp_age[i]-temp_age[i-1])*(age-temp_age[i-1]);

    }
    } else {
        yy=NAN;
    }
    delete temp_age;
    delete temp_depth;

    return yy;
}

double AMSData::get_Rnd(double rnd){
    // rnd(0-1) needs to be given to this function, otherwise not normal distributed because of multiple seed induction
    // Transform rnd into random by using cumPDF
    // find rnd in cumPDF

    int index=(int) intPDF_Size/2;
    if (rnd!=0 && rnd!=1){
        for (int order=2;!(cumPDF[index]>=rnd && cumPDF[index-1]<rnd);order++){
            int step=intPDF_Size/pow(2,order);
            if (step==0) step=1;
            if (cumPDF[index]<rnd){
                index+=step;
            } else {
                index-=step;
            }
        }
    }
    if (rnd<cumPDF[1]) index=1;
    if (rnd>cumPDF[intPDF_Size-1]) index=intPDF_Size-1;
    /*if (cumPDF[index]>=rnd && cumPDF[index-1]<rnd) {
        //qDebug()<<QString::number(rnd)+":"+QString::number(cumPDF[index-1])+":"+QString::number(cumPDF[index]);
    } else {
        //qDebug()<<"Value not Found!!!!";
    }*/
    //return interpolated Value
    return index-1+(1/(cumPDF[index]-cumPDF[index-1])*(rnd-cumPDF[index-1]));
}



//******* Bacon Storage Area
float AMSData::get_bacon_out(int i){
    return bacon_out[i];
}

int AMSData::get_bacon_out_nrow(){
    return bacon_out_nrow;
}

int AMSData::get_bacon_out_ncol(){
    return bacon_out_ncol;
}

float AMSData::get_bacon_age_depth(int i){
    return bacon_Depth[i];
}
float AMSData::get_bacon_age_mean(int i){
    return bacon_Mean[i];
}
float AMSData::get_bacon_age_median(int i){
    return bacon_Median[i];
}
float AMSData::get_bacon_age_Q5(int i){
    return bacon_Q5[i];
}
float AMSData::get_bacon_age_Q95(int i){
    return bacon_Q95[i];
}

int AMSData::get_bacon_age_nrow(){
    return bacon_Depth.size();
}

int AMSData::get_bacon_age_ncol(){
    return 5;
}

int AMSData::get_bacon_K(){
    return bacon_K;
}

float AMSData::get_bacon_d_min(){
    return bacon_d_min;
}

float AMSData::get_bacon_d_max(){
    return bacon_d_max;
}

float AMSData::get_bacon_d_by(){
    return bacon_d_by;
}

float AMSData::get_bacon_acc_shape(){
    return bacon_acc_shape;
}

float AMSData::get_bacon_acc_mean(){
    return bacon_acc_mean;
}

float AMSData::get_bacon_mem_strength(){
    return bacon_mem_strength;
}

float AMSData::get_bacon_mem_mean(){
    return bacon_mem_mean;
}

int AMSData::get_bacon_cc(){
   return bacon_cc;
}

QString AMSData::get_bacon_cc1(){
    return bacon_cc1;
}

QString AMSData::get_bacon_cc2(){
    return bacon_cc2;
}

QString AMSData::get_bacon_cc3(){
    return bacon_cc3;
}

QString AMSData::get_bacon_cc4(){
    return bacon_cc4;
}

int AMSData::get_bacon_postbomb(){
    return bacon_postbomb;
}
/*
float AMSData::get_bacon_d_R(){
    return bacon_d_R;
}

float AMSData::get_bacon_d_STD(){
    return bacon_d_STD;
}
*/
float AMSData::get_bacon_t_a(){
    return bacon_t_a;
}

float AMSData::get_bacon_t_b(){
    return bacon_t_b;
}

int AMSData::get_bacon_normal(){
    return bacon_normal;
}

int AMSData::get_bacon_suggest(){
    return bacon_suggest;
}

float AMSData::get_bacon_th0(){
    return bacon_th0;
}

float AMSData::get_bacon_th0p(){
    return bacon_th0p;
}

int AMSData::get_bacon_burnin(){
    return bacon_burnin;
}

int AMSData::get_bacon_ssize(){
    return bacon_ssize;
}

float AMSData::get_bacon_yr_min(){
    return bacon_yr_min;
}

float AMSData::get_bacon_yr_max(){
    return bacon_yr_max;
}


void AMSData::create_bacon_out(int nrow,int ncol){
    bacon_out_nrow=nrow;
    bacon_out_ncol=ncol;
    delete[] bacon_out;
    bacon_out=new float[bacon_out_nrow*bacon_out_ncol];
}
void AMSData::set_bacon_out(float f,int r,int c){
    bacon_out[r+c*bacon_out_nrow]=f;
}

void AMSData::create_bacon_age(int nrow){
    bacon_age_nrow=nrow;
    bacon_Depth.clear();
    for (int i=0;i<bacon_age_nrow;i++) bacon_Depth.push_back(NAN);
    bacon_Mean.clear();
    for (int i=0;i<bacon_age_nrow;i++) bacon_Mean.push_back(NAN);
    bacon_Q5.clear();
    for (int i=0;i<bacon_age_nrow;i++) bacon_Q5.push_back(NAN);
    bacon_Median.clear();
    for (int i=0;i<bacon_age_nrow;i++) bacon_Median.push_back(NAN);
    bacon_Q95.clear();
    for (int i=0;i<bacon_age_nrow;i++) bacon_Q95.push_back(NAN);
}

void AMSData::set_bacon_age_depth(float f,int r){
     bacon_Depth.at(r)=f;
}
void AMSData::set_bacon_age_mean(float f,int r){
     bacon_Mean.at(r)=f;
}
void AMSData::set_bacon_age_median(float f,int r){
     bacon_Median.at(r)=f;
}
void AMSData::set_bacon_age_Q5(float f,int r){
     bacon_Q5.at(r)=f;
}
void AMSData::set_bacon_age_Q95(float f,int r){
     bacon_Q95.at(r)=f;
}



void AMSData::set_bacon_K(int i){
    bacon_K=i;
}

void AMSData::set_bacon_d_min(float f){
    bacon_d_min=f;
}

void AMSData::set_bacon_d_max(float f){
    bacon_d_max=f;
}

void AMSData::set_bacon_d_by(float f){
    bacon_d_by=f;
}

void AMSData::set_bacon_acc_shape(float f){
    bacon_acc_shape=f;
}

void AMSData::set_bacon_acc_mean(float f){
    bacon_acc_mean=f;
}

void AMSData::set_bacon_mem_strength(float f){
    bacon_mem_strength=f;
}

void AMSData::set_bacon_mem_mean(float f){
    bacon_mem_mean=f;
}

void AMSData::set_bacon_cc(int i){
    bacon_cc=i;
}

void AMSData::set_bacon_cc1(QString s){
    bacon_cc1=s;
}

void AMSData::set_bacon_cc2(QString s){
    bacon_cc2=s;
}

void AMSData::set_bacon_cc3(QString s){
    bacon_cc3=s;
}

void AMSData::set_bacon_cc4(QString s){
    bacon_cc4=s;
}

void AMSData::set_bacon_postbomb(int i){
    bacon_postbomb=i;
}
/*
void AMSData::set_bacon_d_R(float f){
    bacon_d_R=f;
}

void AMSData::set_bacon_d_STD(float f){
    bacon_d_STD=f;
}
*/
void AMSData::set_bacon_t_a(float f){
    bacon_t_a=f;
}

void AMSData::set_bacon_t_b(float f){
    bacon_t_b=f;
}

void AMSData::set_bacon_normal(int i){
    bacon_normal=i;
}

void AMSData::set_bacon_suggest(int i){
    bacon_suggest=i;
}

void AMSData::set_bacon_th0(float f){
    bacon_th0=f;
}

void AMSData::set_bacon_th0p(float f){
    bacon_th0p=f;
}

void AMSData::set_bacon_burnin(int i){
    bacon_burnin=i;
}

void AMSData::set_bacon_ssize(int i){
    bacon_ssize=i;
}

void AMSData::set_bacon_yr_min(float f){
    bacon_yr_min=f;
}

void AMSData::set_bacon_yr_max(float f){
    bacon_yr_max=f;
}

bool AMSData::checkAMS(int mode){
    qDebug()<<"Checking Age Data";
    sort(0,length-1);
    for(int i=0;i<get_Length();i++){
        int ok=1;
        if(get_Data(7,i)==1){
            // check previous ages
            if(i>0){
                for (int j=i-1;j>=0;j--){
                    if (get_Data(7,j)==1){
                        if (get_Data(4,i)<=get_Data(4,j)) ok=0;
                    }
                }
            }
            // check followers ages
            if(i<get_Length()-1){
                for(int j=i+1;j<get_Length();j++){
                    if (get_Data(7,j)==1){
                        if (get_Data(4,i)>=get_Data(4,j)) ok=0;
                    }
                }

            }
            // check previous depth
            if(i>0){
                for (int j=i-1;j>=0;j--){
                    if (get_Data(7,j)==1){
                        if (get_Depth(i)<=get_Depth(j)) ok=0;
                    }
                }
            }
            // check followers depth
            if(i<get_Length()-1){
                for(int j=i+1;j<get_Length();j++){
                    if (get_Data(7,j)==1){
                        if (get_Depth(i)>=get_Depth(j)) ok=0;
                    }
                }

            }
            if (ok==0&&mode==0){
                QMessageBox msgBox;
                msgBox.setText("Core :"+inv->get_att_Core()+"\nSpecies :"+inv->get_att_Species()+"\nDepth :"+QString::number(get_Depth(i))+"\nReversal found -> Use flags deactivated");
                msgBox.setInformativeText("Please check the Agemodel data");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                int ret = msgBox.exec();
                set_Data(7,i,0);
            } else {
                if (ok==0){
                    set_Data(7,i,0);
                }
            }
            if (isnan(get_Data(4,i))) set_Data(7,i,0);// Deactivate uncalibrated age data
        }
    }
    qDebug()<<"complete!";
    return true;
}

void AMSData::erase_bacon(){
    //**** Init Bacon Data Defaults
    // Bacon Ensamble Data
    delete[] bacon_out;
    bacon_out=new float[0];
    bacon_out_nrow=0;
    bacon_out_ncol=0;
    // Bacon Agemodel Data

    bacon_age_nrow=0;

    // Bacon Meta Data
    bacon_K=50;
    bacon_d_min=NAN;
    bacon_d_max=NAN;
    bacon_d_by=1;
    bacon_acc_shape=1.5;
    bacon_acc_mean=20;
    bacon_mem_strength=4;
    bacon_mem_mean=0.7;
    bacon_cc=1;
    bacon_cc1="IntCal13";
    bacon_cc2="Marine13";
    bacon_cc3="SHCal13";
    bacon_cc4="ConstCal";
    bacon_postbomb=0;
    //bacon_d_R=0;
    //bacon_d_STD=0;
    bacon_t_a=3;
    bacon_t_b=4;
    bacon_normal=0;
    bacon_suggest=1;
    bacon_th0=0;
    bacon_th0p=0;
    bacon_burnin=0;
    bacon_ssize=2000;
    bacon_yr_min=-1000;
    bacon_yr_max=100000;
}
