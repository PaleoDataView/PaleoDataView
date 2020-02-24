#include "convert_ra.h"
#include "ui_convert_ra.h"

Convert_RA::Convert_RA(QMainWindow *mainWindow) :
    mainW(mainWindow),
    ui(new Ui::Convert_RA)
{
    ui->setupUi(this);
    this->setWindowTitle("Changing Reservoir data file into a more efficient format.");
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(start()));
}

Convert_RA::~Convert_RA()
{
    delete ui;
}


void Convert_RA::browse(){
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources.get_path_PaleoDataView()+"/Resources",
                                             tr("netCDF (*.nc)"));
    //qDebug() << filename;
    ui->lineEdit->setText(filename);
}

void Convert_RA::start(){
    {
    // get file name
    QString QFilename=ui->lineEdit->text();

    char* filename;
    std::string fname=QFilename.toStdString();
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
    longitude_length=dimlen[0];

    // read data
    longitude=new double[dimlen[0]];
    if ((retval = nc_get_var_double(ncid,varid,&longitude[0]))) ERR(retval);

    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    latitude_length=dimlen[0];

    // read data
    latitude=new double[dimlen[0]];
    if ((retval = nc_get_var_double(ncid,varid,&latitude[0]))) ERR(retval);

    // ******** read times
    // get varid
    if ((retval = nc_inq_varid(ncid, "time", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    time_length=dimlen[0];

    // read data
    time=new double[dimlen[0]];
    if ((retval = nc_get_var_double(ncid,varid,&time[0]))) ERR(retval);

    // ********  read mean MRA
    // get varid
    // Get dimensions
    dimlen[0]=longitude_length;
    dimlen[1]=latitude_length;
    dimlen[2]=time_length;

    if ((retval = nc_inq_varid(ncid, "MRA_avg", &varid)))
       ERR(retval);
    // read MRA_mean
    MRA_mean=new double[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_double(ncid,varid,&MRA_mean[0]))) ERR(retval);

    if ((retval = nc_inq_varid(ncid, "MRA_min", &varid)))
       ERR(retval);
    // read MRA_min
    MRA_min=new double[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_double(ncid,varid,&MRA_min[0]))) ERR(retval);

    if ((retval = nc_inq_varid(ncid, "MRA_max", &varid)))
       ERR(retval);
    // read MRA_max
    MRA_max=new double[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_double(ncid,varid,&MRA_max[0]))) ERR(retval);

    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;


    }

    // convert
    longitude_new=new float[longitude_length];
    for (int i=0;i<longitude_length;i++) longitude_new[i]=(float)(longitude[i]);
    latitude_new=new float[latitude_length];
    for (int i=0;i<latitude_length;i++) latitude_new[i]=(float)(latitude[i]);
    time_new=new float[time_length];
    for (int i=0;i<time_length;i++) time_new[i]=(float)(time[i]);
    MRA_mean_new=new short[longitude_length*latitude_length*time_length];
    for (int i=0;i<longitude_length*latitude_length*time_length;i++) MRA_mean_new[i]=(short)(MRA_mean[i]*10);
    MRA_min_new=new short[longitude_length*latitude_length*time_length];
    for (int i=0;i<longitude_length*latitude_length*time_length;i++) MRA_min_new[i]=(short)(MRA_min[i]*10);
    MRA_max_new=new short[longitude_length*latitude_length*time_length];
    for (int i=0;i<longitude_length*latitude_length*time_length;i++) MRA_max_new[i]=(short)(MRA_max[i]*10);


    // write to file
    {
    QString QFilename;
    QFilename.append(resources.get_path_PaleoDataView()+"/Resources/Calibration/Reservoir_Age.nc");

    char* filename;
    std::string fname=QFilename.toStdString();
    filename=new char[fname.size()+1];
    strcpy(filename,fname.c_str());

    // Initialize ids and meta
    int ncid;
    int retval;// for Errors

    // Open the file
    if (!(retval = nc_create(filename, NC_WRITE, &ncid))){
        // Define variables

        // Float list longitude
        int lon_dim;
        int dim[1];
        if ((retval = nc_def_dim(ncid,"lon_length",longitude_length,&lon_dim)))
           ERR(retval);
        dim[0]=lon_dim;
        int lon_id;
        if ((retval = nc_def_var(ncid,"lon",NC_FLOAT,1,dim,&lon_id)))
            ERR(retval);


        // Float list latitude
        int lat_dim;
        if ((retval = nc_def_dim(ncid,"lat_length",latitude_length,&lat_dim)))
           ERR(retval);
        dim[0]=lat_dim;
        int lat_id;
        if ((retval = nc_def_var(ncid,"lat",NC_FLOAT,1,dim,&lat_id)))
            ERR(retval);

        // Float list time
        int time_dim;

        if ((retval = nc_def_dim(ncid,"time_length",time_length,&time_dim)))
           ERR(retval);
        dim[0]=time_dim;
        int time_id;
        if ((retval = nc_def_var(ncid,"time",NC_FLOAT,1,dim,&time_id)))
            ERR(retval);




        // set size of Q matrix
        //int size_dim;
        int MRA_avg_id;
        int MRA_min_id;
        int MRA_max_id;
        //if ((retval = nc_def_dim(ncid,"MRV_avg_dim",longitude_length*latitude_length*time_length,&size_dim)))
        //   ERR(retval);
        int dimm[3];
        dimm[0]=time_dim;
        dimm[1]=lat_dim;
        dimm[2]=lon_dim;

        if ((retval = nc_def_var(ncid,"MRA_avg",NC_SHORT,3,dimm,&MRA_avg_id)))
            ERR(retval);
        //if ((retval = nc_def_var(ncid,"MRA_min",NC_SHORT,3,dimm,&MRA_min_id)))
        //    ERR(retval);
        if ((retval = nc_def_var(ncid,"MRA_max",NC_SHORT,3,dimm,&MRA_max_id)))
            ERR(retval);



        //**********************************************

        // end definition mode
        if ((retval = nc_enddef(ncid)))
            ERR(retval);



        // ******** save lat
        if ((retval = nc_put_var(ncid,lat_id,&latitude_new[0])))
            ERR(retval);
        // ******** save lon
        if ((retval = nc_put_var(ncid,lon_id,&longitude_new[0])))
            ERR(retval);
        // ******** save time
        if ((retval = nc_put_var(ncid,time_id,&time_new[0])))
            ERR(retval);

        // ******** save MRV_mean
        if ((retval = nc_put_var(ncid,MRA_avg_id,&MRA_mean_new[0])))
            ERR(retval);

        // ******** save MRV_min
        //if ((retval = nc_put_var(ncid,MRA_min_id,&MRA_min_new[0])))
        //    ERR(retval);

        // ******** save MRV_max
        if ((retval = nc_put_var(ncid,MRA_max_id,&MRA_max_new[0])))
            ERR(retval);





        // *******************************************************

       //qDebug()<<"data written";

        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
           ERR(retval);

    } else {
        if (nc_strerror(retval)=="No such file or directory") {
           //qDebug() << "Start generating new File...";
        } else {

            //ERR(retval);
        }
    }
    }
}
