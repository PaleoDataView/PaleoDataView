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

#ifndef AMSDATA_H
#define AMSDATA_H

#include <stdlib.h>
#include "netCDF/include/netcdf.h"
#include <QString>
#include <stdlib.h>
#include "General/resources.h"
#include <QDir>

#include <cmath>
#include "General/inventory.h"

using namespace std;

#define ERR(e) {qDebug("Error: '%s' : %i\n", nc_strerror(e)),e; exit(1);}

class AMSData
{
public:
    AMSData(Inventory *i);
    ~AMSData();
    void set_Inventory(Inventory *i);
    void AMSRead();
    bool checkAMS(int mode);
    void newAMS(int length);
    void AMSSave();
    void AMSSaveData();
    void Save(QString name);
    double get_Depth(int n);
    double get_Sample_Thickness(int n);
    int get_Length();
    QString get_Age_Comment(int n);
    QString get_Type(int n);
    QString get_LabID(int n);
    double get_Data(int c,int n);
    double get_Reservoir_Error(int n);
    void set_Depth(int n,double v);
    void set_Sample_Thickness(int n,double v);
    void set_Age_Comment(QString str,int n);
    void set_Type(int n,QString str);
    void set_LabID(int n,QString str);
    void set_Data(int c,int n,double v);
    void set_Reservoir_Error(int n,double v);

    void ReadCalib();
    double get_cage(int n);
    double get_dage(int n);
    double get_dage_fmu(int n);
    double get_uage(int n);
    double get_uage_fmu(int n);

    int get_Calib_Length();
    void setCalibFile(QString file);

    double calc_CalPDF(double mean,double sigma);
    double get_CalPDF(int i);
    double get_CalPDF_max();
    double get_DPDF(double value,double error,double x);

    void calc_intPDF();

    double get_intYear(int n);
    double get_intPDF(int n);
    int get_int_Length();

    void normPDF();
    void cummulativePDF();
    void sortInt();
    double get_Cal95U();
    double get_Cal95L();
    double get_CalMedian();
    double get_CalMidAge();
    double get_CalWMean();
    void Sort_intPDF(int begin, int end);
    void Sort_intYear(int begin, int end);
    void Sort_hpd_year(int begin, int end);
    void Sort_hpd(int begin,int end);

    void sort(int left,int right);
    void deleteEntry(int n);
    void addEntry();

    void setSelect(int i);
    int getSelect();
    double get_Int_Value_Age(double depth,QString str);
    double get_Int_Value_Depth(double age,QString str);
    double get_Rnd(double rnd);



    //******** Bacon Routines
    // get-routines
    float get_bacon_out(int i);
    int get_bacon_out_nrow();
    int get_bacon_out_ncol();
    float get_bacon_age_depth(int i);
    float get_bacon_age_mean(int i);
    float get_bacon_age_median(int i);
    float get_bacon_age_Q5(int i);
    float get_bacon_age_Q95(int i);

    int get_bacon_age_nrow();
    int get_bacon_age_ncol();
    int get_bacon_K();
    float get_bacon_d_min();
    float get_bacon_d_max();
    float get_bacon_d_by();
    float get_bacon_acc_shape();
    float get_bacon_acc_mean();
    float get_bacon_mem_strength();
    float get_bacon_mem_mean();
    int get_bacon_cc();
    QString get_bacon_cc1();
    QString get_bacon_cc2();
    QString get_bacon_cc3();
    QString get_bacon_cc4();
    int get_bacon_postbomb();
    //float get_bacon_d_R();
    //float get_bacon_d_STD();
    float get_bacon_t_a();
    float get_bacon_t_b();
    int get_bacon_normal();
    int get_bacon_suggest();
    float get_bacon_th0();
    float get_bacon_th0p();
    int get_bacon_burnin();
    int get_bacon_ssize();
    float get_bacon_yr_min();
    float get_bacon_yr_max();

    // set-routines
    void create_bacon_out(int nrow,int ncol);
    void set_bacon_out(float f,int r,int c);
    void create_bacon_age(int nrow);
    void set_bacon_age_depth(float f,int r);
    void set_bacon_age_mean(float f,int r);
    void set_bacon_age_median(float f,int r);
    void set_bacon_age_Q5(float f,int r);
    void set_bacon_age_Q95(float f,int r);

    void set_bacon_K(int i);
    void set_bacon_d_min(float f);
    void set_bacon_d_max(float f);
    void set_bacon_d_by(float f);
    void set_bacon_acc_shape(float f);
    void set_bacon_acc_mean(float f);
    void set_bacon_mem_strength(float f);
    void set_bacon_mem_mean(float f);
    void set_bacon_cc(int i);
    void set_bacon_cc1(QString s);
    void set_bacon_cc2(QString s);
    void set_bacon_cc3(QString s);
    void set_bacon_cc4(QString s);
    void set_bacon_postbomb(int i);
    //void set_bacon_d_R(float f);
    //void set_bacon_d_STD(float f);
    void set_bacon_t_a(float f);
    void set_bacon_t_b(float f);
    void set_bacon_normal(int i);
    void set_bacon_suggest(int i);
    void set_bacon_th0(float f);
    void set_bacon_th0p(float f);
    void set_bacon_burnin(int i);
    void set_bacon_ssize(int i);
    void set_bacon_yr_min(float f);
    void set_bacon_yr_max(float f);

    void erase_bacon();

private:
    Inventory *inv;
    Resources *resources;
    //double *depth;
    int length;
    int select=0;

    //QString *comment;
    //QString *labid;
    //QString *type;
    //double *age_data;

    // Age model variables
    std::vector<double> data_Depth;
    std::vector<double> data_Sample_Thickness;
    std::vector<QString*> data_Label;
    std::vector<QString*> data_Type;
    std::vector<double> data_Age_dated;
    std::vector<double> data_Age_pErr;
    std::vector<double> data_Age_mErr;
    std::vector<double> data_Reservoir;
    std::vector<double> data_Reservoir_Err;
    std::vector<double> data_Cal_Age;
    std::vector<double> data_Cal_min;
    std::vector<double> data_Cal_max;
    std::vector<int> data_Use_Flag;
    std::vector<QString*> data_Comment;

    double *cal_pdf;
    double cal_pdf_max;
    int ageind;
    int intPDF_Size;
    double *intPDF;
    double *cumPDF;
    double *intYear;
    int hpd_length;
    double *hpd_prop;
    double *hpd_year;
    double Cal95U;
    double Cal95L;
    double CalMedian;
    double CalMidAge;
    double CalWMean;
    // Calib Data
    double *cage;// Calender Age
    double *dage;// Data Age(messurement)
    double *uage;// error of dage
    int Calib_Length;

    // Bacon Ensamble Data
    float *bacon_out;
    int bacon_out_nrow;
    int bacon_out_ncol;
    // Bacon Age Data
    std::vector<double> bacon_Depth;
    std::vector<double> bacon_Mean;
    std::vector<double> bacon_Q5;
    std::vector<double> bacon_Median;
    std::vector<double> bacon_Q95;
   // float *bacon_age;
    int bacon_age_nrow;
    //int bacon_age_ncol;
    // Bacon Meta Data
    int bacon_K;
    float bacon_d_min;
    float bacon_d_max;
    float bacon_d_by;
    float bacon_acc_shape;
    float bacon_acc_mean;
    float bacon_mem_strength;
    float bacon_mem_mean;
    int bacon_cc;
    QString bacon_cc1;
    QString bacon_cc2;
    QString bacon_cc3;
    QString bacon_cc4;
    int bacon_postbomb;
    //float bacon_d_R;
    //float bacon_d_STD;
    float bacon_t_a;
    float bacon_t_b;
    int bacon_normal;
    int bacon_suggest;
    float bacon_th0;
    float bacon_th0p;
    int bacon_burnin;
    int bacon_ssize;
    float bacon_yr_min;
    float bacon_yr_max;
};

#endif // AMSDATA_H
