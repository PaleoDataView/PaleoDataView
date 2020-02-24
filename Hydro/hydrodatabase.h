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

#ifndef HYDRODATABASE_H
#define HYDRODATABASE_H

#include <stdlib.h>
#include <QDebug.h>
#include "netCDF/include/netcdf.h"
#include <QString>
#include <stdlib.h>
#include "General/resources.h"
#include <QDir>

#include <cmath>

using namespace std;

#define ERR(e) {qDebug("Error: '%s' : %i\n", nc_strerror(e)),e; exit(1);}



class HydroDatabase
{
public:
    HydroDatabase();
    ~HydroDatabase();

    void readTemperature();
    float getTemperature(float longitude,float latitude,float depth);
    float getPTemperature(float longitude,float latitude,float depth);
    float adiabatt(float S,float T,float P);
    int get_t_Lon_Size();
    int get_t_Lat_Size();
    int get_t_Depth_Size();
    float get_t_Lon(int index);
    float get_t_Lat(int index);
    float get_t_Depth(int index);
    float get_t_min();
    float get_t_max();
    float get_t_dmin(int index);
    float get_t_dmax(int index);
    void readSalinity();
    float getSalinity(float longitude,float latitude,float depth);
    int get_s_Lon_Size();
    int get_s_Lat_Size();
    int get_s_Depth_Size();
    float get_s_Lon(int index);
    float get_s_Lat(int index);
    float get_s_Depth(int index);
    float get_s_min();
    float get_s_max();
    float get_s_dmin(int index);
    float get_s_dmax(int index);
    void readd18o();
    float getd18o(float longitude,float latitude,float depth);
    int get_d18o_Lon_Size();
    int get_d18o_Lat_Size();
    int get_d18o_Depth_Size();
    float get_d18o_Lon(int index);
    float get_d18o_Lat(int index);
    float get_d18o_Depth(int index);
    float get_d18o_min();
    float get_d18o_max();
    float get_d18o_dmin(int index);
    float get_d18o_dmax(int index);

    float getdc(float longitude,float latitude,float depth);
    float get_dc_min();
    float get_dc_max();
    float get_dc_dmin(int index);
    float get_dc_dmax(int index);

    float getdcm(float longitude,float latitude,float depth);
    float get_dcm_min();
    float get_dcm_max();
    float get_dcm_dmin(int index);
    float get_dcm_dmax(int index);

    float getp(float longitude,float latitude,float depth,float press, float temp);
    float get_press_min(float t_min,float s_min);
    float get_press_max(float t_max,float s_max);
    float getBox_Longitude(float longitude);
    float getBox_Latitude(float latitude);
    float getBox_Depth(float depth);
    void readCarbon();
    float getCarbon(float longitude,float latitude,float depth);
    int get_c_Lon_Size();
    int get_c_Lat_Size();
    int get_c_Depth_Size();
    float get_c_Lon(int index);
    float get_c_Lat(int index);
    float get_c_Depth(int index);
    float get_c_min();
    float get_c_max();
    float get_c_dmin(int index);
    float get_c_dmax(int index);


private:
    // Temperature
    float *t_mn;
    float t_mn_min=0;
    float t_mn_max=0;
    float *t_mn_dmin;
    float *t_mn_dmax;
    float *t_lon;
    float *t_lat;
    float *t_depth;
    size_t t_lon_length=0;
    size_t t_lat_length=0;
    size_t t_depth_length=0;
    // Salinity
    float *s_mn;
    float s_mn_min=0;
    float s_mn_max=0;
    float *s_mn_dmin;
    float *s_mn_dmax;
    float *s_lon;
    float *s_lat;
    float *s_depth;
    size_t s_lon_length=0;
    size_t s_lat_length=0;
    size_t s_depth_length=0;
    // d18o
    float *d18o_mn;
    float d18o_mn_min=0;
    float d18o_mn_max=0;
    float *d18o_mn_dmin;
    float *d18o_mn_dmax;
    float *d18o_lon;
    float *d18o_lat;
    float *d18o_depth;
    size_t d18o_lon_length=0;
    size_t d18o_lat_length=0;
    size_t d18o_depth_length=0;
    // Carbon
    float *c_mn;
    float c_mn_min=0;
    float c_mn_max=0;
    float *c_mn_dmin;
    float *c_mn_dmax;
    float *c_lon;
    float *c_lat;
    float *c_depth;
    size_t c_lon_length=0;
    size_t c_lat_length=0;
    size_t c_depth_length=0;

    float dc_mn_min=0;
    float dc_mn_max=0;
    float dc_mn_dmin;
    float dc_mn_dmax;

    float dcm_mn_min=0;
    float dcm_mn_max=0;
    float dcm_mn_dmin;
    float dcm_mn_dmax;
    Resources resources;
};

#endif // HYDRODATABASE_H
