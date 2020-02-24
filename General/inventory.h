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

#ifndef INVENTORY_H
#define INVENTORY_H

#include <math.h>
#include <stdlib.h>
#include <qdebug.h>
#include "netCDF/include/netcdf.h"
#include <QString>
#include <stdlib.h>
#include "General/resources.h"
#include <QDir>

#include <QPushButton>
#include <string.h>

using namespace std;

#define ERR(e) {qDebug("Error: '%s' : %i\n", nc_strerror(e)),e;exit(1);}


class Inventory
{

public:
    explicit Inventory(QString str);
    ~Inventory();

public:

    void setShowError();
    // ******* Inventory
    void read();
    void save();
    void generate();
    bool checkAgeFile();
    void addEntry();
    void deleteEntry(int n);
    void noData();
    void setProxyType(QString str);

    //******** Inventory Data
    QString get_Core(int n);
    void set_Core(QString str);

    double get_Latitude(int n);
    void set_Latitude(double v);

    double get_Longitude(int n);
    void set_Longitude(double v);

    QString get_Filename(int n);
    void set_Filename(QString str);

    QString get_Species(int n);
    void set_Species(QString str);

    int get_Basin(int n);
    void set_Basin(int b);

    double get_Water_Depth(int n);
    void set_Water_Depth(double v);

    int get_Oxygen_Use_Flag(int n);
    void invert_O_Flag(int n);
    void set_O_Use_Flag(int n);

    int get_Carbon_Use_Flag(int n);
    void invert_C_Flag(int n);
    void set_C_Use_Flag(int n);

    int get_SST_Use_Flag(int n);
    void invert_SST_Flag(int n);
    void set_SST_Use_Flag(int n);

    QString get_Record_Type(int n);
    void set_Record_Type(QString str);



    int get_Use_Flag(int n);
    void set_Use_Flag(int n);

    //********** Iventory Meta
    unsigned int get_Entries();
    unsigned int get_Selected(int n);
    unsigned int get_Selected_Sum();
    void invert_Selected(int n);
    void set_Selected(int n,int m);
    int get_AgeModel(int n);
    void set_AgeModel(int n,int i);

    //********** Data
    void readData(int n);
    void readData(QString Filename);
    void readData(QString Filename,int onlyMeta);

    bool checkData(int mode);
    void saveData();
    void saveData(QString filename);
    int get_currentCore();
    void set_currentCore(QString name,QString proxy);
    void set_currentCore(int i);
    void newIsotope(int isolength);
    void newIsotopeMeta();
    void addIsotope();

    //********** Data attributes
    int access_proxy_length();
    QString access_proxy_name(int type);
    QString access_proxy_name();
    QString access_proxy_abbreviation();
    QString access_proxy_type(int type);
    QString access_proxy_target();
    QString access_proxy_folder();
    double get_data_Isotopes(int i,int j);
    void set_data_Isotopes(int i,int j,double v);

    //generalization functions for index based data access

    // data
    QString access_data_value(int type,int row);
    void write_data_value(int i,int j,QString str);
    QString access_data_error(int type,int row);
    QString access_data_name(int type);
    QString access_data_unit(int type);
    QString access_data_type(int type);
    int access_data_length();  
    QString access_data_netcdf(int type);

    // Meta
    int access_meta_length();
    QString access_meta_name(int type);
    QString access_meta_value(int type);
    void write_meta_value(int type,QString str);
    QString access_meta_unit(int type);
    QString access_meta_type(int type);
    QString access_meta_netcdf(int type);

    // Inventory
    int access_inv_length();
    QString access_inv_name(int type);
    int access_inv_entries();
    QString access_inv_data(int type,int row);
    QString access_inv_meta(int type);
    int access_inv_meta_nr(int type);
    void write_inv_data(int type,int row,QString str);
    QString access_inv_type(int type);
    QString access_inv_unit(int type);
    QString access_inv_netcdf(int type);

    //*************************************************************
    double get_data_Depth(int i);
    double get_data_Sample_Thickness(int i);
    double get_data_Age(int i);

    double get_data_d13C(int i);
    double get_data_d18O(int i);
    double get_data_d13C_Corr(int i);
    double get_data_d18O_Corr(int i);
    double get_data_d13C_Err(int i);
    double get_data_d18O_Err(int i);

    double get_data_Mg(int i);
    double get_data_Mg_Err(int i);
    double get_data_Ca(int i);
    double get_data_Ca_Err(int i);
    double get_data_MgCa(int i);
    double get_data_MgCa_Err(int i);
    double get_data_SST(int i);
    double get_data_SST_Err(int i);

    double get_data_C37_2(int i);
    double get_data_C37_3(int i);
    double get_data_C37_2_Err(int i);
    double get_data_C37_3_Err(int i);
    double get_data_UK37(int i);

    int get_data_XRF_Al(int i);
    int get_data_XRF_Si(int i);
    int get_data_XRF_P(int i);
    int get_data_XRF_S(int i);
    int get_data_XRF_Cl(int i);
    int get_data_XRF_K(int i);
    int get_data_XRF_Ca(int i);
    int get_data_XRF_Ti(int i);
    int get_data_XRF_Cr(int i);
    int get_data_XRF_Mn(int i);
    int get_data_XRF_Fe(int i);
    int get_data_XRF_Rh(int i);
    double get_data_XRF_Al_Si(int i);
    double get_data_XRF_Fe_K(int i);
    double get_data_XRF_Fe_Ca(int i);
    double get_data_XRF_Al_Ca(int i);

    double get_data_Parameter1(int i);
    double get_data_Parameter1_Err(int i);
    double get_data_Parameter1_Cal(int i);
    double get_data_Parameter1_Cal_Err(int i);
    double get_data_Parameter2(int i);
    double get_data_Parameter2_Err(int i);
    double get_data_Parameter2_Cal(int i);
    double get_data_Parameter2_Cal_Err(int i);

    int get_data_Use_Flag(int i);
    QString get_data_Comment(int i);
    //QString get_data_Comments(int j);
    void set_data_Depth(double v,int i);
    void set_data_Sample_Thickness(double v,int i);
    void set_data_Age(double v,int i);

    void set_data_d13C(double v,int i);
    void set_data_d18O(double v,int i);
    void set_data_d13C_Corr(double v,int i);
    void set_data_d18O_Corr(double v,int i);
    void set_data_d13C_Err(double v,int i);
    void set_data_d18O_Err(double v,int i);

    void set_data_Mg(double v,int i);
    void set_data_Mg_Err(double v,int i);
    void set_data_Ca(double v,int i);
    void set_data_Ca_Err(double v,int i);
    void set_data_MgCa(double v,int i);
    void set_data_MgCa_Err(double v,int i);
    void set_data_SST(double v,int i);
    void set_data_SST_Err(double v,int i);

    void set_data_C37_2(double v,int i);
    void set_data_C37_3(double v,int i);
    void set_data_C37_2_Err(double v,int i);
    void set_data_C37_3_Err(double v,int i);
    void set_data_UK37(double v,int i);

    void set_data_XRF_Al(int v,int i);
    void set_data_XRF_Si(int v,int i);
    void set_data_XRF_P(int v,int i);
    void set_data_XRF_S(int v,int i);
    void set_data_XRF_Cl(int v,int i);
    void set_data_XRF_K(int v,int i);
    void set_data_XRF_Ca(int v,int i);
    void set_data_XRF_Ti(int v,int i);
    void set_data_XRF_Cr(int v,int i);
    void set_data_XRF_Mn(int v,int i);
    void set_data_XRF_Fe(int v,int i);
    void set_data_XRF_Rh(int v,int i);
    void set_data_XRF_Al_Si(double v,int i);
    void set_data_XRF_Fe_K(double v,int i);
    void set_data_XRF_Fe_Ca(double v,int i);
    void set_data_XRF_Al_Ca(double v,int i);

    void set_data_Parameter1(double v,int i);
    void set_data_Parameter1_Err(double v,int i);
    void set_data_Parameter1_Cal(double v,int i);
    void set_data_Parameter1_Cal_Err(double v,int i);
    void set_data_Parameter2(double v,int i);
    void set_data_Parameter2_Err(double v,int i);
    void set_data_Parameter2_Cal(double v,int i);
    void set_data_Parameter2_Cal_Err(double v,int i);

    void set_data_Use_Flag(int v,int i);
    void set_data_Comment(QString str,int i);

    double get_Int_Value_18O(double depth,QString str);

    QString get_att_Core();
    void set_att_Core(QString str);

    double get_att_Latitude();
    void set_att_Latitude(double v);

    double get_att_Longitude();
    void set_att_Longitude(double v);

    QString get_att_Device();
    void set_att_Device(QString str);

    double get_att_Water_Depth();
    void set_att_Water_Depth(double v);

    QString get_att_Species();
    void set_att_Species(QString str);

    QString get_att_Laboratory();
    void set_att_Laboratory(QString str);

    QString get_att_Reference();
    void set_att_Reference(QString str);

    QString get_att_Comment();
    void set_att_Comment(QString str);

    QString get_att_EPaper();
    void set_att_EPaper(QString s);

    int get_att_Oxygen_Use_Flag();
    void set_att_O_Use_Flag(int n);

    int get_att_Carbon_Use_Flag();
    void set_att_C_Use_Flag(int n);

    double get_att_O_Correction();
    void set_att_O_Correction(double v);

    QString get_att_O_Justification();
    void set_att_O_Justification(QString str);

    double get_att_C_Correction();
    void set_att_C_Correction(double v);

    QString get_att_C_Justification();
    void set_att_C_Justification(QString str);

    int get_att_SST_Use_Flag();
    void set_att_SST_Use_Flag(int n);

    QString get_att_Data_Source();
    void set_att_Data_Source(QString str);

    QString get_att_Record_Type();
    void set_att_Record_Type(QString str);

    QString get_att_Importer();
    void set_att_Importer(QString str);

    QString get_att_Optional();
    void set_att_Optional(QString str);

    QString get_att_Category();
    void set_att_Category(QString str);



    int get_att_Use_Flag();
    void set_att_Use_Flag(int n);

    // ******** Data Meta
    unsigned int get_Length();
    int get_flag_Data_OK();
    void set_flag_Data_OK(int n);
    //void set_data_Comment(QString str,int j);
/*
    // ********* Age Model
    void new_Age_Model();
    void read_Age_Model();
    void save_Age_Model();
    void add_Point(double depth,double age);
    void remove_Point(int n);
    double get_Point_Depth(int n);
    double get_Point_Age(int n);
    int get_Age_Model_Size();
    double get_Int_Value_Age(double depth,QString str);
    double get_Int_Value_Depth(double age,QString str);
    void apply_Age_Model(QString str);
*/
    // ********* TargetDaten
    void readTarget();
    int get_Target_Length(int i);
    double get_Target_Age(int n,int i);
    double get_Target_Value(int n,int i);
    double get_Target_Error(int n,int i);
    double get_Target_Age_Error(int n,int i);
    int get_Target_Vari();
    QString get_Target_Name(int i);
    // ******** Basin
    void read_Basin();
    int get_Basin(float lon, float lat);
    QString get_Basinname(float lon,float lat);
    QString get_Basinname(int i);

    void NetCDF_Error(QString fname,QString action,int value);
    void NetCDF_Error_Message();

    void sort(int i,int j);

private:
    QString proxy;
    // Inventory data
    /*
    QString* var_Core;
    double* var_Latitude;
    double* var_Longitude;
    QString* var_Filename;
    QString* var_Species;
    double* var_Basin;
    double* var_Water_Depth;
    double* var_Oxygen_Use_Flag;
    double* var_Carbon_Use_Flag;
    QString* var_Record_Type;
    */

    QVector<QStringList*> inv;
    size_t entries=0;
    unsigned int* selected;
    int *var_AgeModel;
    int current_Core;





    // Core Data

    unsigned int length=0; // Length of Data Variable
    // no use
    //double* data_Isotopes;
    int data_Isotopes_row=0;
    int data_Isotopes_col=0;
    //QString* data_Comments;

    // DATA VARIABLES
    /*
    std::vector<double> data_Depth;
    std::vector<double> data_Sample_Thickness;
    std::vector<double> data_Age;
    // Foraminiferal Isotopes
    std::vector<double> data_d13C;
    std::vector<double> data_d18O;
    std::vector<double> data_d13C_Corr;
    std::vector<double> data_d18O_Corr;
    std::vector<double> data_d13C_Err;
    std::vector<double> data_d18O_Err;
    // Foraminiferal MgCa
    std::vector<double> data_Mg;
    std::vector<double> data_Mg_Err;
    std::vector<double> data_Ca;
    std::vector<double> data_Ca_Err;
    std::vector<double> data_MgCa;
    std::vector<double> data_MgCa_Err;
    std::vector<double> data_SST;
    std::vector<double> data_SST_Err;

    std::vector<int> data_Use_Flag;
    std::vector<QString*> data_Comment;
    */
    QVector <QStringList*> data;

    // ATTRIBUTES
    /*
    // Mandatory
    QString     att_Core="No Core Selected";
    double      att_Latitude=0;
    double      att_Longitude=0;
    double      att_Water_Depth=0;
    QString     att_Species="na";
    QString     att_Record_Type="na";
    // Proxy specific
    QString     att_Device="na";
    QString     att_Laboratory="na";
    QString     att_Instrumentation="na";
    QString     att_Data_Source="na";
    QString     att_Comment="na";
    QString     att_Category="na";
    QString     att_Reference="na";
    QString     att_Importer="na";
    // Optional/Proxy specific
    QString     att_EPaper="na";
    double      att_Oxygen_Use_Flag=0;
    double      att_Carbon_Use_Flag=0;
    double      att_O_Correction=0;
    QString     att_O_Justification="na";
    double      att_C_Correction=0;
    QString     att_C_Justification="na";
    // Dump/User specified
    QString     att_Optional="";// dump of all additional parameters
    */
    QStringList meta;


    int         flag_Data_OK=0;
/*
    // ********* Age Model Data
    double* age_Model_Depth;
    double* age_Model_Age;
    unsigned int age_Model_Length;
*/
    // ********* Target Data
    QStringList header;
    QList<QList<double> > target;




/*
    double* target_Govin;
    unsigned int length_Govin;
    double* target_LH4;
    unsigned int length_LH4;
    */
    Resources resources;

    // ********* Basin
    double* basin_data;
    int basin_lon;
    int basin_lat;

    // Errorhandling
    QString error_text;
    int show_error=1;

};

#endif // INVENTORY_H
