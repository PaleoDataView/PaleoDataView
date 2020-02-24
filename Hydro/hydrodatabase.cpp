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

#include "hydrodatabase.h"

HydroDatabase::HydroDatabase()
{
    readTemperature();
    readSalinity();
    readd18o();
    readCarbon();
}

HydroDatabase::~HydroDatabase(){
    //qDebug()<<"Ceaning Database...";
    if (t_mn!=NULL) delete[] t_mn;
    if (t_mn_dmax!=NULL) delete[] t_mn_dmax;
    if (t_mn_dmin!=NULL)delete[] t_mn_dmin;
    if (t_lon!=NULL)delete[] t_lon;
    if (t_lat!=NULL)delete[] t_lat;
    if (t_depth!=NULL)delete[] t_depth;
    //qDebug()<<"t_mn erased";

    if (s_mn!=NULL) delete[] s_mn;
    if (s_mn_dmax!=NULL) delete[] s_mn_dmax;
    if (s_mn_dmin!=NULL)delete[] s_mn_dmin;
    if (s_lon!=NULL)delete[] s_lon;
    if (s_lat!=NULL)delete[] s_lat;
    if (s_depth!=NULL)delete[] s_depth;
    //qDebug()<<"Salinity erased";

    if (d18o_mn!=NULL) delete[] d18o_mn;
    if (d18o_mn_dmax!=NULL) delete[] d18o_mn_dmax;
    if (d18o_mn_dmin!=NULL)delete[] d18o_mn_dmin;
    if (d18o_lon!=NULL)delete[] d18o_lon;
    if (d18o_lat!=NULL)delete[] d18o_lat;
    if (d18o_depth!=NULL)delete[] d18o_depth;
    //qDebug()<<"d18o erased";

    if (c_mn!=NULL) delete[] c_mn;
    if (c_mn_dmax!=NULL) delete[] c_mn_dmax;
    if (c_mn_dmin!=NULL)delete[] c_mn_dmin;
    if (c_lon!=NULL)delete[] c_lon;
    if (c_lat!=NULL)delete[] c_lat;
    if (c_depth!=NULL)delete[] c_depth;
    //qDebug()<<"c erased";

}

void HydroDatabase::readTemperature(){
    // get file name
    QString QFilename=resources.path_hydro+"temperature_annual_1deg.nc";

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
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // ******** read longitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lon", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    t_lon_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    t_lon=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&t_lon[0]))) ERR(retval);

   //qDebug()<<QString::number(t_lon[0]);
    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    t_lat_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    t_lat=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&t_lat[0]))) ERR(retval);
    //for (int i=0;i<t_lat_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_lat[i]);

    // ******** read depths
    // get varid
    if ((retval = nc_inq_varid(ncid, "depth", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    t_depth_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    t_depth=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&t_depth[0]))) ERR(retval);
    //for (int i=0;i<t_depth_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_depth[i]);


    // ********  read mean temperature
    // get varid
    if ((retval = nc_inq_varid(ncid, "t_an", &varid)))
       ERR(retval);
    // Get dimensions
    dimlen[0]=t_lon_length;
    dimlen[1]=t_lat_length;
    dimlen[2]=t_depth_length;
    //qDebug() << "rows: "+QString::number(dimlen[0])+" length:"+QString::number(dimlen[1])+" depth:"+QString::number(dimlen[2]);
    // read t_mn
    t_mn=new float[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_float(ncid,varid,&t_mn[0]))) ERR(retval);
    // estimate min and max
    t_mn_min=9999999999;
    t_mn_max=-9999999999;
    t_mn_dmin=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) t_mn_dmin[i]=9999999999;
    t_mn_dmax=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) t_mn_dmax[i]=-9999999999;
    for (int di=0;di<dimlen[2];di++){
        for (int longi=0;longi<dimlen[0];longi++){
            for (int lati=0;lati<dimlen[1];lati++){
                int index=longi+(lati*t_lon_length)+(di*t_lon_length*t_lat_length);
                // set global min/max
                if (t_mn[index]<t_mn_min && t_mn[index]>-1000) t_mn_min=t_mn[index];
                if (t_mn[index]>t_mn_max && t_mn[index]<1000) t_mn_max=t_mn[index];
                // set depth dependent min/max
                if (t_mn[index]<t_mn_dmin[di] && t_mn[index]>-1000) t_mn_dmin[di]=t_mn[index];
                if (t_mn[index]>t_mn_dmax[di] && t_mn[index]<1000) t_mn_dmax[di]=t_mn[index];
            }
        }
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
}

float HydroDatabase::getTemperature(float longitude,float latitude,float depth){
    float t=0;
    float t1=0;
    float t2=0;
    if(longitude<0) longitude=360+longitude;
    int index_lon=(int)(longitude);
    int index_lat=(int)(latitude+90);
    int index_depth=0;


    for (index_depth=0;index_depth<t_depth_length;index_depth++) if(t_depth[index_depth]>=depth) break;
    if (index_depth>0){
        index_depth--;// select last smaller value
    } else {

    }

    int index=index_lon+(index_lat*t_lon_length)+(index_depth*t_lon_length*t_lat_length);
    if (index>=0 && index<t_depth_length*t_lon_length*t_lat_length) t1=t_mn[index];

    if (index_depth<t_depth_length-1) index_depth++;// select first bigger value
    index=index_lon+(index_lat*t_lon_length)+(index_depth*t_lon_length*t_lat_length);
    if (index>=0 && index<t_depth_length*t_lon_length*t_lat_length) t2=t_mn[index];

    // interpolate
    if (t1>-1000 && t1<1000 && t2>-1000 && t2<1000) {
        t=t1-((t2-t1)/(t_depth[index_depth-1]-t_depth[index_depth])*(depth-t_depth[index_depth-1]));
    }else {
        t=-9999999999;
    }


    return t;
}

float HydroDatabase::getPTemperature(float longitude,float latitude,float depth){


    float S=getSalinity(longitude,latitude,depth);
    float T0=getTemperature(longitude,latitude,depth);
    float P0=999.842594 + 6.793952e-2*T0 - 9.095290e-3*T0*T0 + 1.001685e-4*T0*T0*T0 - 1.120083e-6*T0*T0*T0*T0 + 6.536332e-9*T0*T0*T0*T0*T0;

    float PR=0;
    float P=P0*10;// to decibar
    float T=T0;

    float H = PR - P;
    float XK = H*adiabatt(S,T,P) ;
    T = T + 0.5*XK;
    float Q = XK;
    P = P + 0.5*H ;
    XK = H*adiabatt(S,T,P) ;
    T = T + 0.29289322*(XK-Q) ;
    Q = 0.58578644*XK + 0.121320344*Q ;
    XK = H*adiabatt(S,T,P) ;
    T = T + 1.707106781*(XK-Q);
    Q = 3.414213562*XK - 4.121320344*Q;
    P = P + 0.5*H ;
    XK = H*adiabatt(S,T,P) ;
    float THETA = T + (XK-2.0*Q)/6.0;
    //qDebug() << QString::number(THETA);
    return THETA;
}

float HydroDatabase::adiabatt(float S,float T,float P){
    float AGT=0;
    float DS = S - 35.0 ;
    AGT = (((-2.1687E-16*T+1.8676E-14)*T-4.6206E-13)*P+((2.7759E-12*T-1.1351E-10)*DS+((-5.4481E-14*T+8.733E-12)*T-6.7795E-10)*T+1.8741E-8))*P+(-4.2393E-8*T+1.8932E-6)*DS+((6.6228E-10*T-6.836E-8)*T+8.5258E-6)*T+3.5803E-5;
    return AGT;
}

float HydroDatabase::getBox_Longitude(float longitude){
    int index=(int)(longitude+180);
    return t_lon[index];
}
float HydroDatabase::getBox_Latitude(float latitude){
    int index=(int)(latitude+90);
    return t_lat[index];
}
float HydroDatabase::getBox_Depth(float depth){
    int index=0;
    for (index=0;index<t_depth_length;index++) if(t_depth[index]>=depth) break;
    if (index>0) if ((t_depth[index]-depth)>(depth-t_depth[index-1])) index--;
    return t_depth[index];
}

int HydroDatabase::get_t_Lon_Size(){
    return t_lon_length;
}

int HydroDatabase::get_t_Lat_Size(){
    return t_lat_length;
}

int HydroDatabase::get_t_Depth_Size(){
    return t_depth_length;
}

float HydroDatabase::get_t_Lon(int index){
    return t_lon[index];
}

float HydroDatabase::get_t_Lat(int index){
    return t_lat[index];
}

float HydroDatabase::get_t_Depth(int index){
    return t_depth[index];
}

float HydroDatabase::get_t_min(){
    return t_mn_min;
}

float HydroDatabase::get_t_max(){
    return t_mn_max;
}

float HydroDatabase::get_t_dmin(int i){
    return t_mn_dmin[i];
}

float HydroDatabase::get_t_dmax(int i){
    return t_mn_dmax[i];
}


void HydroDatabase::readSalinity(){
    // get file name
    QString QFilename=resources.path_hydro+"salinity_annual_1deg.nc";

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
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // ******** read longitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lon", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    s_lon_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    s_lon=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&s_lon[0]))) ERR(retval);

   //qDebug()<<QString::number(s_lon[0]);
    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    s_lat_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    s_lat=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&s_lat[0]))) ERR(retval);
    //for (int i=0;i<t_lat_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_lat[i]);

    // ******** read depths
    // get varid
    if ((retval = nc_inq_varid(ncid, "depth", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    s_depth_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    s_depth=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&s_depth[0]))) ERR(retval);
    //for (int i=0;i<t_depth_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_depth[i]);


    // ********  read mean temperature
    // get varid
    if ((retval = nc_inq_varid(ncid, "s_an", &varid)))
       ERR(retval);
    // Get dimensions
    dimlen[0]=s_lon_length;
    dimlen[1]=s_lat_length;
    dimlen[2]=s_depth_length;
    //qDebug() << "rows: "+QString::number(dimlen[0])+" length:"+QString::number(dimlen[1])+" depth:"+QString::number(dimlen[2]);
    // read s_mn
    s_mn=new float[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_float(ncid,varid,&s_mn[0]))) ERR(retval);
    // estimate min and max
    s_mn_min=9999999999;
    s_mn_max=-9999999999;
    s_mn_dmin=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) s_mn_dmin[i]=9999999999;
    s_mn_dmax=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) s_mn_dmax[i]=-9999999999;
    for (int di=0;di<dimlen[2];di++){
        for (int longi=0;longi<dimlen[0];longi++){
            for (int lati=0;lati<dimlen[1];lati++){
                int index=longi+(lati*s_lon_length)+(di*s_lon_length*s_lat_length);
                // set global min/max
                if (s_mn[index]<s_mn_min && s_mn[index]>-1000) s_mn_min=s_mn[index];
                if (s_mn[index]>s_mn_max && s_mn[index]<1000) s_mn_max=s_mn[index];
                // set depth dependent min/max
                if (s_mn[index]<s_mn_dmin[di] && s_mn[index]>-1000) s_mn_dmin[di]=s_mn[index];
                if (s_mn[index]>s_mn_dmax[di] && s_mn[index]<1000) s_mn_dmax[di]=s_mn[index];
            }
        }
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
}

float HydroDatabase::getSalinity(float longitude,float latitude,float depth){
    float s=0;
    float s1=0;
    float s2=0;
    if(longitude<0) longitude=360+longitude;
    int index_lon=(int)(longitude);
    int index_lat=(int)(latitude+90);
    int index_depth=0;

    for (index_depth=0;index_depth<s_depth_length;index_depth++) if(s_depth[index_depth]>=depth) break;
    if (index_depth>0) index_depth--;

    int index=index_lon+(index_lat*s_lon_length)+(index_depth*s_lon_length*s_lat_length);
    if (index>=0 && index<s_depth_length*s_lon_length*s_lat_length)s1=s_mn[index];

    if (index_depth<s_depth_length-1) index_depth++;
    index=index_lon+(index_lat*s_lon_length)+(index_depth*s_lon_length*s_lat_length);
    if (index>=0 && index<s_depth_length*s_lon_length*s_lat_length)s2=s_mn[index];

    // interpolate
    if (s1>-1000 && s1<1000 && s2>-1000 && s2<1000) {
        s=s1-((s2-s1)/(s_depth[index_depth-1]-s_depth[index_depth])*(depth-s_depth[index_depth-1]));
    }else {
        s=-9999999999;
    }
    if (s==0) s=-999999999;
    return s;
}

int HydroDatabase::get_s_Lon_Size(){
    return s_lon_length;
}

int HydroDatabase::get_s_Lat_Size(){
    return s_lat_length;
}

int HydroDatabase::get_s_Depth_Size(){
    return s_depth_length;
}

float HydroDatabase::get_s_Lon(int index){
    return s_lon[index];
}

float HydroDatabase::get_s_Lat(int index){
    return s_lat[index];
}

float HydroDatabase::get_s_Depth(int index){
    return s_depth[index];
}

float HydroDatabase::get_s_min(){
    return s_mn_min;
}

float HydroDatabase::get_s_max(){
    return s_mn_max;
}

float HydroDatabase::get_s_dmin(int i){
    return s_mn_dmin[i];
}

float HydroDatabase::get_s_dmax(int i){
    return s_mn_dmax[i];
}


void HydroDatabase::readd18o(){
    // get file name
    QString QFilename=resources.path_hydro+"modern_ocean_atlas_v1_1.nc";

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
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // ******** read longitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lon", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    d18o_lon_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    d18o_lon=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&d18o_lon[0]))) ERR(retval);

   //qDebug()<<QString::number(d18o_lon[0]);
    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    d18o_lat_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    d18o_lat=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&d18o_lat[0]))) ERR(retval);
    //for (int i=0;i<t_lat_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_lat[i]);

    // ******** read depths
    // get varid
    if ((retval = nc_inq_varid(ncid, "depth", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    d18o_depth_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    d18o_depth=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&d18o_depth[0]))) ERR(retval);
    //for (int i=0;i<t_depth_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_depth[i]);


    // ********  read d18o
    // get varid
    if ((retval = nc_inq_varid(ncid, "d18o", &varid)))
       ERR(retval);
    // Get dimensions
    dimlen[0]=d18o_lon_length;
    dimlen[1]=d18o_lat_length;
    dimlen[2]=d18o_depth_length;
    //qDebug() << "rows: "+QString::number(dimlen[0])+" length:"+QString::number(dimlen[1])+" depth:"+QString::number(dimlen[2]);
    // read s_mn
    d18o_mn=new float[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_float(ncid,varid,&d18o_mn[0]))) ERR(retval);
    // estimate min and max
    d18o_mn_min=9999999999;
    d18o_mn_max=-9999999999;
    d18o_mn_dmin=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) d18o_mn_dmin[i]=9999999999;
    d18o_mn_dmax=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) d18o_mn_dmax[i]=-9999999999;
    for (int di=0;di<dimlen[2];di++){
        for (int longi=0;longi<dimlen[0];longi++){
            for (int lati=0;lati<dimlen[1];lati++){
                int index=longi+(lati*d18o_lon_length)+(di*d18o_lon_length*d18o_lat_length);
                // set global min/max
                if (d18o_mn[index]<d18o_mn_min && d18o_mn[index]>-1000) d18o_mn_min=d18o_mn[index];
                if (d18o_mn[index]>d18o_mn_max && d18o_mn[index]<1000) d18o_mn_max=d18o_mn[index];
                // set depth dependent min/max
                if (d18o_mn[index]<d18o_mn_dmin[di] && d18o_mn[index]>-1000) d18o_mn_dmin[di]=d18o_mn[index];
                if (d18o_mn[index]>d18o_mn_dmax[di] && d18o_mn[index]<1000) d18o_mn_dmax[di]=d18o_mn[index];
            }
        }
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
}

float HydroDatabase::getd18o(float longitude,float latitude,float depth){
    float d18o=0;
    float d18o1=0;
    float d18o2=0;
    longitude=longitude-180;
    if(longitude<0) longitude=360+longitude;
    int index_lon=(int)(longitude);
    int index_lat=(int)(latitude+90);
    int index_depth=0;

    for (index_depth=0;index_depth<d18o_depth_length;index_depth++) if(d18o_depth[index_depth]>=depth) break;
    if (index_depth>0) index_depth--;

    int index=index_lon+(index_lat*d18o_lon_length)+(index_depth*d18o_lon_length*d18o_lat_length);
    if (index>=0 && index<d18o_depth_length*d18o_lon_length*d18o_lat_length)d18o1=d18o_mn[index];

    if (index_depth<d18o_depth_length-1) index_depth++;
    index=index_lon+(index_lat*d18o_lon_length)+(index_depth*d18o_lon_length*d18o_lat_length);
    if (index>=0 && index<d18o_depth_length*d18o_lon_length*d18o_lat_length)d18o2=d18o_mn[index];

    if (d18o1>-1000 && d18o1<1000 && d18o2>-1000 && d18o2<1000) {
        d18o=d18o1-((d18o2-d18o1)/(d18o_depth[index_depth-1]-d18o_depth[index_depth])*(depth-d18o_depth[index_depth-1]));
    }else {
        d18o=-9999999999;
    }
    return d18o;
}

int HydroDatabase::get_d18o_Lon_Size(){
    return d18o_lon_length;
}

int HydroDatabase::get_d18o_Lat_Size(){
    return d18o_lat_length;
}

int HydroDatabase::get_d18o_Depth_Size(){
    return d18o_depth_length;
}

float HydroDatabase::get_d18o_Lon(int index){
    return d18o_lon[index];
}

float HydroDatabase::get_d18o_Lat(int index){
    return d18o_lat[index];
}

float HydroDatabase::get_d18o_Depth(int index){
    return d18o_depth[index];
}

float HydroDatabase::get_d18o_min(){
    return d18o_mn_min;
}

float HydroDatabase::get_d18o_max(){
    return d18o_mn_max;
}

float HydroDatabase::get_d18o_dmin(int i){
    return d18o_mn_dmin[i];
}

float HydroDatabase::get_d18o_dmax(int i){
    return d18o_mn_dmax[i];
}


float HydroDatabase::getdc(float longitude,float latitude,float depth){
    float dc=0;
    float t=getTemperature(longitude,latitude,depth);
    float d18o=getd18o(longitude,latitude,depth);

    dc=d18o-0.27+21.9-sqrt(310.61+10*t);
    return dc;
}

float HydroDatabase::get_dc_min(){
    dc_mn_min=d18o_mn_min-0.27+21.9-sqrt(310.61+10*t_mn_max);
    return dc_mn_min;
}

float HydroDatabase::get_dc_max(){
    dc_mn_max=d18o_mn_max-0.27+21.9-sqrt(310.61+10*t_mn_min);
    return dc_mn_max;
}

float HydroDatabase::get_dc_dmin(int i){
    dc_mn_dmin=d18o_mn_dmin[i]-0.27+21.9-sqrt(310.61+10*t_mn_dmax[i]);
    return dc_mn_dmin;
}

float HydroDatabase::get_dc_dmax(int i){
    dc_mn_dmax=d18o_mn_dmax[i]-0.27+21.9-sqrt(310.61+10*t_mn_dmin[i]);
    return dc_mn_dmax;
}

float HydroDatabase::getdcm(float longitude,float latitude,float depth){
    float dc=0;
    float t=getTemperature(longitude,latitude,depth);
    float d18o=getd18o(longitude,latitude,depth);
    //dc=-0.245*T+0.0011*T^2+3.58+dw-0.27
    dc=-0.245*t+pow(0.0011*t,2)+3.58+d18o-0.27;
    return dc;
}

float HydroDatabase::get_dcm_min(){

    dcm_mn_min=-0.245*t_mn_max+pow(0.0011*t_mn_min,2)+3.58+d18o_mn_min-0.27;
    return dcm_mn_min;
}

float HydroDatabase::get_dcm_max(){
    dcm_mn_max=-0.245*t_mn_min+pow(0.0011*t_mn_max,2)+3.58+d18o_mn_max-0.27;
    return dcm_mn_max;
}

float HydroDatabase::get_dcm_dmin(int i){

    dcm_mn_dmin=-0.245*t_mn_dmax[i]+pow(0.0011*t_mn_dmin[i],2)+3.58+d18o_mn_dmin[i]-0.27;
    return dcm_mn_dmin;
}

float HydroDatabase::get_dcm_dmax(int i){
    dcm_mn_dmax=-0.245*t_mn_dmin[i]+pow(0.0011*t_mn_dmax[i],2)+3.58+d18o_mn_dmax[i]-0.27;
    return dcm_mn_dmax;
}



float HydroDatabase::getp(float longitude,float latitude,float depth,float press,float temp){

    float p0=999.842594 + 6.793952e-2*temp - 9.095290e-3*temp*temp + 1.001685e-4*temp*temp*temp - 1.120083e-6*temp*temp*temp*temp + 6.536332e-9*temp*temp*temp*temp*temp;

    float p=press-p0;
    float A=(8.24493e-1) - (4.0899e-3)*temp + (7.6438e-5)*temp*temp - (8.2467e-7)*temp*temp*temp + (5.3875e-9)*temp*temp*temp*temp;
    float B=(-5.72466e-3) + (1.0227e-4)*temp -(1.6546e-6)*temp*temp;
    float C=4.8314e-4;
    float e=(p*p)/(B*B);
    float d=(-2.0*p*A)/(B*B);
    float c=(A*A-2.0*p*C)/(B*B);
    float b=(2.0*A*C-B*B)/(B*B);
    float a=(C*C)/(B*B);
    // Solve quatric equation
    float alpha=-(3.0*b*b)/(8.0*a*a)+(c)/(a);
    float beta=(b*b*b)/(8.0*a*a*a)-(b*c)/(2.0*a*a)+(d)/(a);
    float gamma=-(3*b*b*b*b)/(256.0*a*a*a*a)+(b*b*c)/(16.0*a*a*a)-(b*d)/(4.0*a*a)+(e)/(a);
    float P=-(alpha*alpha)/(12.0)-gamma;
    float Q=-(alpha*alpha*alpha)/(108.0)+(alpha*gamma)/(3.0)-(beta*beta)/(8.0);
    float U=pow(-(Q)/(2.0)+sqrt((Q*Q)/(4.0)+(P*P*P)/(27.0)),1.0/3.0);
    float y=-(5.0*alpha)/(6.0);
    if (P==0) y+=-pow(Q,1.0/3.0);
        else y+=U-(P)/(3.0*U);
    float w=sqrt(alpha+2.0*y);
    float z=(beta)/(2.0*w);
    // results
    float s1=-(b)/(4.0*a)+0.5*(1.0*w+1.0*sqrt(-(alpha+2.0*y)-2.0*(alpha+1*(beta)/(w))));
    float s2=-(b)/(4.0*a)+0.5*(1.0*w-1.0*sqrt(-(alpha+2.0*y)-2.0*(alpha+1*(beta)/(w))));
    float s3=-(b)/(4.0*a)+0.5*(-1.0*w+1.0*sqrt(-(alpha+2.0*y)-2.0*(alpha-1*(beta)/(w))));
    float s4=-(b)/(4.0*a)+0.5*(-1.0*w-1.0*sqrt(-(alpha+2.0*y)-2.0*(alpha-1*(beta)/(w))));
    float v=9999;
    if (s1<v) v=s1;
    if (s2<v) v=s2;
    if (s3<v) v=s3;
    if (s4<v) v=s4;
    return v;
}

float HydroDatabase::get_press_min(float t_min,float s_min){
    float temp=t_min;
    float p0=999.842594 + 6.793952e-2*temp - 9.095290e-3*temp*temp + 1.001685e-4*temp*temp*temp - 1.120083e-6*temp*temp*temp*temp + 6.536332e-9*temp*temp*temp*temp*temp;

    float A=8.24493e-1 - 4.0899e-3*temp + 7.6438e-5*temp*temp - 8.2467e-7*temp*temp*temp + 5.3875e-9*temp*temp*temp*temp;
    float B=-5.72466e-3 + 1.0227e-4*temp - 1.6546e-6*temp*temp;
    float C=4.8314e-4;
    return A*s_min+B*pow(s_min,3.0/2.0)+C*s_min*s_min+p0;
}

float HydroDatabase::get_press_max(float t_max,float s_max){
    float temp=t_max;
    float p0=999.842594 + 6.793952e-2*temp - 9.095290e-3*temp*temp + 1.001685e-4*temp*temp*temp - 1.120083e-6*temp*temp*temp*temp + 6.536332e-9*temp*temp*temp*temp*temp;

    float A=8.24493e-1 - 4.0899e-3*temp + 7.6438e-5*temp*temp - 8.2467e-7*temp*temp*temp + 5.3875e-9*temp*temp*temp*temp;
    float B=-5.72466e-3 + 1.0227e-4*temp - 1.6546e-6*temp*temp;
    float C=4.8314e-4;
    return A*s_max+B*pow(s_max,3.0/2.0)+C*s_max*s_max+p0;
}

void HydroDatabase::readCarbon(){
    // get file name
    QString QFilename=resources.path_hydro+"C13_Climatology.nc";

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
    if ((retval = nc_open(filename, NC_NOWRITE, &ncid)))
       ERR(retval);

    // ******** read longitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lon", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    c_lon_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    c_lon=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&c_lon[0]))) ERR(retval);

   //qDebug()<<QString::number(s_lon[0]);
    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    c_lat_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    c_lat=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&c_lat[0]))) ERR(retval);
    //for (int i=0;i<t_lat_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_lat[i]);

    // ******** read depths
    // get varid
    if ((retval = nc_inq_varid(ncid, "depth", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    c_depth_length=dimlen[0];
    //qDebug() << "rows: "+QString::number(dimlen[0]);
    // read data
    c_depth=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&c_depth[0]))) ERR(retval);
    //for (int i=0;i<t_depth_length;i++)//qDebug() << "index: "+QString::number(i)+" : "+QString::number(t_depth[i]);


    // ********  read carbon
    // get varid
    if ((retval = nc_inq_varid(ncid, "c13_pi", &varid)))
       ERR(retval);
    // Get dimensions
    dimlen[0]=c_lon_length;
    dimlen[1]=c_lat_length;
    dimlen[2]=c_depth_length;
    //qDebug() << "rows: "+QString::number(dimlen[0])+" length:"+QString::number(dimlen[1])+" depth:"+QString::number(dimlen[2]);
    // read s_mn
    c_mn=new float[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_float(ncid,varid,&c_mn[0]))) ERR(retval);
    // estimate min and max
    c_mn_min=9999999999;
    c_mn_max=-9999999999;
    c_mn_dmin=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) c_mn_dmin[i]=9999999999;
    c_mn_dmax=new float[dimlen[2]];
    for (int i=0;i<dimlen[2];i++) c_mn_dmax[i]=-9999999999;
    for (int di=0;di<dimlen[2];di++){
        for (int longi=0;longi<dimlen[0];longi++){
            for (int lati=0;lati<dimlen[1];lati++){
                int index=longi+(lati*c_lon_length)+(di*c_lon_length*c_lat_length);
                // set global min/max
                if (c_mn[index]<c_mn_min && c_mn[index]>-1000) c_mn_min=c_mn[index];
                if (c_mn[index]>c_mn_max && c_mn[index]<1000) c_mn_max=c_mn[index];
                // set depth dependent min/max
                if (c_mn[index]<c_mn_dmin[di] && c_mn[index]>-1000) c_mn_dmin[di]=c_mn[index];
                if (c_mn[index]>c_mn_dmax[di] && c_mn[index]<1000) c_mn_dmax[di]=c_mn[index];
            }
        }
    }

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);

}

float HydroDatabase::getCarbon(float longitude,float latitude,float depth){
    float c=0;
    float c1=0;
    float c2=0;
    if(longitude<0) longitude=360+longitude;
    int index_lon=(int)(longitude);
    int index_lat=(int)(latitude+90);
    int index_depth=0;

    for (index_depth=0;index_depth<c_depth_length;index_depth++) if(c_depth[index_depth]>=depth) break;
    if (index_depth>0) index_depth--;

    int index=index_lon+(index_lat*c_lon_length)+(index_depth*c_lon_length*c_lat_length);
    if (index>=0 && index<c_depth_length*c_lon_length*c_lat_length)c1=c_mn[index];

    if (index_depth<c_depth_length-1) index_depth++;
    index=index_lon+(index_lat*c_lon_length)+(index_depth*c_lon_length*c_lat_length);
    if (index>=0 && index<c_depth_length*c_lon_length*c_lat_length)c2=c_mn[index];

    // interpolate
    if (c1>-1000 && c1<1000 && c2>-1000 && c2<1000) {
        c=c1-((c2-c1)/(c_depth[index_depth-1]-c_depth[index_depth])*(depth-c_depth[index_depth-1]));
    }else {
        c=-9999999999;
    }

    return c;
}

int HydroDatabase::get_c_Lon_Size(){
    return c_lon_length;
}

int HydroDatabase::get_c_Lat_Size(){
    return c_lat_length;
}

int HydroDatabase::get_c_Depth_Size(){
    return c_depth_length;
}

float HydroDatabase::get_c_Lon(int index){
    return c_lon[index];
}

float HydroDatabase::get_c_Lat(int index){
    return c_lat[index];
}

float HydroDatabase::get_c_Depth(int index){
    return c_depth[index];
}

float HydroDatabase::get_c_min(){
    return c_mn_min;
}

float HydroDatabase::get_c_max(){
    return c_mn_max;
}

float HydroDatabase::get_c_dmin(int i){
    return c_mn_dmin[i];
}

float HydroDatabase::get_c_dmax(int i){
    return c_mn_dmax[i];
}


