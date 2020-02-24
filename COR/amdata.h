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

#ifndef AMDATA_H
#define AMDATA_H


#include <stdlib.h>
#include <QDebug.h>
#include "netCDF/include/netcdf.h"
#include <QString>
#include <stdlib.h>
#include "General/resources.h"
#include <QDir>

#include <cmath>
#include "General/inventory.h"

using namespace std;

#define ERR(e) {qDebug("Error: '%s' : %i\n", nc_strerror(e)),e; exit(1);}

class AMData
{
public:
    AMData(Inventory *i);
    ~AMData();

    void AMRead();
    void AMSave();
    double get_Depth(int n);
    int get_Length();
    QString get_Age_Comment(int n);
    QString get_Type(int n);
    QString get_LabID(int n);
    double get_Data(int c,int n);
    void set_Depth(int n,double v);
    void set_Age_Comment(QString str,int n);
    void set_Type(int n,QString str);
    void set_LabID(int n,QString str);
    void set_Data(int c,int n,double v);

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
private:
    Inventory *inv;
    Resources *resources;
    double *depth;
    int depth_length;
    int select=0;
    QString *comment;
    QString *labid;
    QString *type;
    double *age_data;
    double *cal_pdf;
    double cal_pdf_max;
    int ageind;
    int intPDF_Size;
    double *intPDF;
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



};


#endif // AMDATA_H
