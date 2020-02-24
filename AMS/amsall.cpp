#include "amsall.h"
#include "ui_amsall.h"

AMSAll::AMSAll(QMainWindow *mainWindow,Inventory *inventory,QString str) :
    mainW(mainWindow),inv_main(inventory),proxy(str),
    ui(new Ui::AMSAll)
{
    ui->setupUi(this);
    resources=new Resources();
    inv=new Inventory(proxy);
    inv->read();
    inv->read_Basin();
    for(int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
    for(int i=0;i<inv->get_Entries();i++) inv->set_AgeModel(i,inv_main->get_AgeModel(i));
    amsdata=new AMSData(inv);
    ui->lineEdit->setText(resources->get_filename_C14_Cal());
    amsdata->ReadCalib();

    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(set_CalibData()));// Change Calib-File
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(start()));
    connect(this,SIGNAL(refresh()),mainW,SLOT(redraw_score()));
}

AMSAll::~AMSAll()
{
    delete ui;
    delete resources;
    delete amsdata;
}

void AMSAll::set_CalibData(){
    QString file = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources->get_path_target(),
                                             tr("C14 Calibration Data (*.14C)"));


    //qDebug() << "Selected :"+file;
    amsdata->setCalibFile(file);
    ui->lineEdit->setText(file);
    amsdata->ReadCalib();
    update();

}

void AMSAll::start(){
    for (int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0); // deselect all
    for (int i=0;i<inv->get_Entries();i++){
        inv->set_currentCore(i);
        inv->readData(inv->get_currentCore());
        ui->plainTextEdit->appendPlainText("\n"+inv->get_Core(inv->get_currentCore())+" ("+QString::number(i+1)+" of "+QString::number(inv->get_Entries())+")");
        repaint();
        amsdata->AMSRead();
        reservoirAll();
        calibrateAll();

        amsdata->AMSSave();
    }
    emit(refresh());
    ui->plainTextEdit->appendPlainText("\nChecked and callibrated all agemodels. See list in mainwindow for changed proxy agemodels.");

}

void AMSAll::calibrateAll(){
    inv_main->set_Selected(inv->get_currentCore(),0);
    for (int i=0;i<amsdata->get_Length();i++){
        if ( amsdata->get_Type(i)=="AMS" ) {
            if (amsdata->get_Data(0,i)>0 && amsdata->get_Data(1,i)>0 && amsdata->get_Data(2,i)>0){
                if (amsdata->get_Data(4,i)==NAN && ui->checkBox->isChecked()){
                    float error=sqrt(amsdata->get_Data(1,i)*amsdata->get_Data(1,i)+amsdata->get_Reservoir_Error(i)*amsdata->get_Reservoir_Error(i));
                    amsdata->calc_CalPDF(amsdata->get_Data(0,i)*1000-amsdata->get_Data(3,i)*1000,error*1000.0/1.0);
                    amsdata->calc_intPDF();
                    amsdata->sortInt();
                    amsdata->set_Data(4,i,amsdata->get_CalWMean()/1000.0);
                    amsdata->set_Data(5,i,amsdata->get_Cal95L()/1000.0);
                    amsdata->set_Data(6,i,amsdata->get_Cal95U()/1000.0);
                    amsdata->set_Data(7,i,1);
                    inv_main->set_Selected(inv->get_currentCore(),1);
                    ui->plainTextEdit->appendPlainText(QString::number(i)+" of "+QString::number(amsdata->get_Length())+" : found uncalibrated AMS: Calibrated");
                }
                if (amsdata->get_Data(4,i)!=NAN && (!ui->checkBox->isChecked())){
                    float error=sqrt(amsdata->get_Data(1,i)*amsdata->get_Data(1,i)+amsdata->get_Reservoir_Error(i)*amsdata->get_Reservoir_Error(i));
                    amsdata->calc_CalPDF(amsdata->get_Data(0,i)*1000-amsdata->get_Data(3,i)*1000,error*1000.0/1.0);
                    amsdata->calc_intPDF();
                    amsdata->sortInt();
                    amsdata->set_Data(4,i,amsdata->get_CalWMean()/1000.0);
                    amsdata->set_Data(5,i,amsdata->get_Cal95L()/1000.0);
                    amsdata->set_Data(6,i,amsdata->get_Cal95U()/1000.0);
                    amsdata->set_Data(7,i,1);
                    inv_main->set_Selected(inv->get_currentCore(),1);
                    ui->plainTextEdit->appendPlainText(QString::number(i)+" of "+QString::number(amsdata->get_Length())+"found calibrated AMS: Re-Calibrated");
                }
            } else {
                ui->plainTextEdit->appendPlainText(QString::number(i)+" of "+QString::number(amsdata->get_Length())+"incomplete dated values!!!");


            }
        }
    }
    checkReversals();
}

void AMSAll::reservoirAll(){
    reservoir(1);
}


void AMSAll::reservoir(int mode){


    // read Data
    float *longitude;
    int longitude_length;
    float *latitude;
    int latitude_length;
    float *time;
    int time_length;
    short *MRA_mean;
    //short *MRA_min;
    short *MRA_max;

    // get file name
    QString QFilename=resources->get_path_PaleoDataView()+"/Resources/Calibration/Reservoir_Age.nc";

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
    longitude=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&longitude[0]))) ERR(retval);

    // ******** read latitudes
    // get varid
    if ((retval = nc_inq_varid(ncid, "lat", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    latitude_length=dimlen[0];

    // read data
    latitude=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&latitude[0]))) ERR(retval);

    // ******** read times
    // get varid
    if ((retval = nc_inq_varid(ncid, "time", &varid)))
       ERR(retval);
    // get size
    if ((retval = nc_inq_dimlen(ncid, varid, &dimlen[0])))
       ERR(retval);
    time_length=dimlen[0];

    // read data
    time=new float[dimlen[0]];
    if ((retval = nc_get_var_float(ncid,varid,&time[0]))) ERR(retval);

    // ********  read mean MRA
    // get varid
    // Get dimensions
    dimlen[0]=time_length;
    dimlen[1]=longitude_length;
    dimlen[2]=latitude_length;

    if ((retval = nc_inq_varid(ncid, "MRA_avg", &varid)))
       ERR(retval);
    // read MRA_mean
    MRA_mean=new short[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_short(ncid,varid,&MRA_mean[0]))) ERR(retval);

    //if ((retval = nc_inq_varid(ncid, "MRA_min", &varid)))
    //   ERR(retval);
    // read MRA_min
    //MRA_min=new short[dimlen[0]*dimlen[1]*dimlen[2]];
    //if ((retval = nc_get_var_short(ncid,varid,&MRA_min[0]))) ERR(retval);

    if ((retval = nc_inq_varid(ncid, "MRA_max", &varid)))
       ERR(retval);
    // read MRA_max
    MRA_max=new short[dimlen[0]*dimlen[1]*dimlen[2]];
    if ((retval = nc_get_var_short(ncid,varid,&MRA_max[0]))) ERR(retval);


    // Close the file, freeing all resources.
    if ((retval = nc_close(ncid)))
       ERR(retval);
    delete[] filename;
    //delete[] dimlen;

    // transfer calender time-vector to c14 times without correcting reservoir age
    amsdata->ReadCalib();
    float *time_c14=new float[time_length];
    float *time_c14_err=new float[time_length];
    for (int i=0;i<time_length;i++) {
        time_c14[i]=0;
        time_c14_err[i]=0;
        for (int j=1;j<amsdata->get_Calib_Length();j++){
            if (time[i]<=amsdata->get_cage(j)){
                //qDebug()<<QString::number(j);
                time_c14[i]=amsdata->get_dage(j-1)+((amsdata->get_dage(j)-amsdata->get_dage(j-1))/(amsdata->get_cage(j)-amsdata->get_cage(j-1)))*(time[i]-amsdata->get_cage(j-1));

                time_c14_err[i]=amsdata->get_uage(j-1)+((amsdata->get_uage(j)-amsdata->get_uage(j-1))/(amsdata->get_cage(j)-amsdata->get_cage(j-1)))*(time[i]-amsdata->get_cage(j-1));
                j=amsdata->get_Calib_Length();
            }
        }
    }
    time_c14[0]=0;// can not be interpolated because there is no data
    time_c14_err[0]=0;
    // initialize
    int s=select;
    int start=0;
    int end=0;
    if (mode==0) {
        start=select;
        end=select+1;
    }
    if (mode==1){
        start=0;
        end=amsdata->get_Length();
    }
    // start calibrating
    for (int q=start;q<end;q++){
        qDebug()<<"Calibrating : "+QString::number(q);
        select=q;
        if (amsdata->get_Type(select)=="AMS" &&
                amsdata->get_Data(0,select)>0 && amsdata->get_Data(0,select)<400 &&
                amsdata->get_Data(1,select)>0 && amsdata->get_Data(1,select)<400 &&
                amsdata->get_Data(2,select)>0 && amsdata->get_Data(2,select)<400 &&
                (amsdata->get_Data(3,select)==NAN || (!ui->checkBox_2->isChecked()))
                ){

        // find entry time
        int index_lon=-1;
        int index_lat=-1;
        int index_time=-1;
        float l=inv->get_att_Longitude();
        if (l<-5-1.25) l=360+l;
        l=l+6.25;
        index_lon=(int)(l/2.5);

        if (index_lon>=longitude_length && index_lon>=0) {
            index_lon=-1;
            qDebug()<<"index longitude out of range";
        }

        for (int i=0;i<latitude_length;i++) {
            if (latitude[i]<inv->get_att_Latitude()+1.25){
                index_lat=i;
                break;
            }
        }
        if (index_lat>=latitude_length && index_lat>=0) {
            index_lat=-1;
            qDebug()<<"index latitude out of range";
        }

        if (index_lon>=0 && index_lat>=0){
            for (int i=0;i<time_length;i++) {

                if (time_c14[i]+MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0>amsdata->get_Data(0,select)*1000.0){
                    index_time=i;
                    break;
                }
            }
        }
        if (index_time>=time_length && index_time>=0) {
            index_time=-1;
            qDebug()<<"index time out of range";
        }
        if (index_lat>-1 && index_lon>-1 && index_time>-1){// only go further if indizes were found
            qDebug()<<QString::number(index_lon)+" : "+QString::number(index_lat)+" : "+QString::number(index_time)+" : "+QString::number(MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]);
            int basin=inv->get_Basin(inv->get_att_Longitude(),inv->get_att_Latitude());

            // check if on land -> select neighbor if so
            int corrected=0;
            if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]<0) {
                qDebug()<<"On Land";

                //First order cells (4)
                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-1;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon--;
                        if (index_lon<0) index_lon=+longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using left neighbor";
                    } else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[index_lat]));
                    }
                }

                if (index_lon<longitude_length-1&& corrected==0){
                    int new_index_lon=index_lon+1;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon++;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using reight neighbor";
                    } else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[index_lat]));
                    }
                }

                if (index_lat>0&& corrected==0){
                    int new_index_lat=index_lat-1;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat--;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using lower neighbor";
                    } else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[index_lon],latitude[new_index_lat]));
                    }
                }

                if (index_lat<latitude_length-1&& corrected==0){
                    int new_index_lat=index_lat+1;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat++;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using upper neighbor";
                    } else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[index_lon],latitude[new_index_lat]));
                    }
                }

                // First order diagonal cells (4)
                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-1;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    int new_index_lat=index_lat-1;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon--;
                        index_lat--;
                        if (index_lon<0) index_lon=+longitude_length;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using left-lower neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat]));
                    }
                }

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-1;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    int new_index_lat=index_lat+1;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon--;
                        index_lat++;
                        if (index_lon<0) index_lon+=longitude_length;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using left-upper neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat]));
                    }
                }

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon+1;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    int new_index_lat=index_lat-1;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon++;
                        index_lat--;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using right-lower neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat]));
                    }
                }

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon+1;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    int new_index_lat=index_lat+1;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                        index_lon++;
                        index_lat++;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using right-upper neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat]));
                    }
                }

                // Second order cells (4)

                if (index_lon>0 && corrected==0){
                    int new_index_lon=index_lon-2;
                    if (new_index_lon<0) new_index_lon+=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon--;
                        index_lon--;
                        if (index_lon<0) index_lon+=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using left*2 neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[index_lat]));
                    }
                }

                if (index_lon<longitude_length-1&& corrected==0){
                    int new_index_lon=index_lon+2;
                    if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                            && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                        index_lon++;
                        index_lon++;
                        if (index_lon>=longitude_length) index_lon-=longitude_length;
                        corrected=1;
                        qDebug()<<"On Land: Using reight*2 neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[new_index_lon],latitude[index_lat]));
                    }
                }

                if (index_lat>0&& corrected==0){
                    int new_index_lat=index_lat-2;
                    if (new_index_lat<0) new_index_lat=0;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat--;
                        index_lat--;
                        if (index_lat<0) index_lat=0;
                        corrected=1;
                        qDebug()<<"On Land: Using lower*2 neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[index_lon],latitude[new_index_lat]));
                    }
                }

                if (index_lat<latitude_length-1&& corrected==0){
                    int new_index_lat=index_lat+2;
                    if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                    if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                            && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                        index_lat++;
                        index_lat++;
                        if (index_lat>=latitude_length) index_lat=latitude_length-1;
                        corrected=1;
                        qDebug()<<"On Land: Using upper*2 neighbor";
                    }else {
                        qDebug()<<QString::number(basin)+":"+QString::number(inv->get_Basin(longitude[index_lon],latitude[new_index_lat]));
                    }
                }



            }
            qDebug()<<"corrected"+QString::number(corrected);
            qDebug()<<QString::number(index_lon)+" : "+QString::number(index_lat)+" : "+QString::number(index_time)+" : "+QString::number(MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]);

            //if(index_lon>-1 && index_lat>-1 && index_time>-1){
            //    amsdata->set_Data(3,select,MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10000.0);
            //}

            //qDebug()<<QString::number(index_lon)+":"+QString::number(index_lat)+":"+QString::number(index_time);
            //qDebug()<<QString::number(longitude[index_lon])+":"+QString::number(latitude[index_lat])+":"+QString::number(time[index_time]);

            if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]>0){
                // create pdf of dated values
                float *pdf_years=new float[1000];
                float *pdf_p=new float[1000];
                float *pdf_ra=new float[1000];
                //float *pdf_ra_min=new float[1000];
                float *pdf_ra_max=new float[1000];

                float min=(amsdata->get_Data(0,select)-amsdata->get_Data(1,select)*6.0)*1000.0;
                float max=(amsdata->get_Data(0,select)+amsdata->get_Data(2,select)*6.0)*1000.0;

                float value=amsdata->get_Data(0,select)*1000.0;


                for (int i=0;i<1000;i++){
                    pdf_years[i]=min+i*((max-min)/1000.0);


                    // find entry longitude latitude time
                    int index_lon=-1;
                    int index_lat=-1;
                    int index_time=-1;
                    float l=inv->get_att_Longitude();
                    if (l<-5.0-1.25) l=360.0+l;
                    l=l+6.25;
                    index_lon=(int)(l/2.5);

                    for (int j=0;j<latitude_length;j++) {
                        if (latitude[j]<inv->get_att_Latitude()){
                            index_lat=j;
                            break;
                        }
                    }



                    for (int j=0;j<time_length;j++) {
                        if (time_c14[j]>pdf_years[i]){
                            index_time=j;
                            break;
                        }
                    }

                    if (index_lat>-1 && index_lon>-1 && index_time>0 && index_lat<latitude_length && index_lon<longitude_length && index_time<time_length){

                        // check if on land -> select neighbor if so
                        int corrected=0;
                        if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]<0) {
                            qDebug()<<"On Land";

                            //First order cells (4)
                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-1;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon--;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left neighbor";
                                }
                            }

                            if (index_lon<longitude_length-1&& corrected==0){
                                int new_index_lon=index_lon+1;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon++;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using reight neighbor";
                                }
                            }

                            if (index_lat>0&& corrected==0){
                                int new_index_lat=index_lat-1;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat--;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using lower neighbor";
                                }
                            }

                            if (index_lat<latitude_length-1&& corrected==0){
                                int new_index_lat=index_lat+1;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat++;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using upper neighbor";
                                }
                            }

                            // First order diagonal cells (4)
                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-1;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                int new_index_lat=index_lat-1;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon--;
                                    index_lat--;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left-lower neighbor";
                                }
                            }

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-1;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                int new_index_lat=index_lat+1;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon--;
                                    index_lat++;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left-upper neighbor";
                                }
                            }

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon+1;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                int new_index_lat=index_lat-1;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon++;
                                    index_lat--;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using right-lower neighbor";
                                }
                            }

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon+1;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                int new_index_lat=index_lat+1;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+new_index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[new_index_lat])){
                                    index_lon++;
                                    index_lat++;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using right-upper neighbor";
                                }
                            }

                            // Second order cells (4)

                            if (index_lon>0 && corrected==0){
                                int new_index_lon=index_lon-2;
                                if (new_index_lon<0) new_index_lon=+longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon--;
                                    index_lon--;
                                    if (index_lon<0) index_lon=+longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using left*2 neighbor";
                                }
                            }

                            if (index_lon<longitude_length-1&& corrected==0){
                                int new_index_lon=index_lon+2;
                                if (new_index_lon>=longitude_length) new_index_lon-=longitude_length;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+(new_index_lon)]>0
                                        && basin==inv->get_Basin(longitude[new_index_lon],latitude[index_lat])){
                                    index_lon++;
                                    index_lon++;
                                    if (index_lon>=longitude_length) index_lon-=longitude_length;
                                    corrected=1;
                                    qDebug()<<"On Land: Using reight*2 neighbor";
                                }
                            }

                            if (index_lat>0&& corrected==0){
                                int new_index_lat=index_lat-2;
                                if (new_index_lat<0) new_index_lat=0;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat--;
                                    index_lat--;
                                    if (index_lat<0) index_lat=0;
                                    corrected=1;
                                    qDebug()<<"On Land: Using lower*2 neighbor";
                                }
                            }

                            if (index_lat<latitude_length-1&& corrected==0){
                                int new_index_lat=index_lat+2;
                                if (new_index_lat>=latitude_length) new_index_lat=latitude_length-1;
                                if (MRA_mean[index_time*(longitude_length*latitude_length)+(new_index_lat)*longitude_length+(index_lon)]>0
                                        && basin==inv->get_Basin(longitude[index_lon],latitude[new_index_lat])){
                                    index_lat++;
                                    index_lat++;
                                    if (index_lat>=latitude_length) index_lat=latitude_length-1;
                                    corrected=1;
                                    qDebug()<<"On Land: Using upper*2 neighbor";
                                }
                            }



                        }


                    // get values
                    if(MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]>0){
                        float MRA1=MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        float MRA2=MRA_mean[(index_time-1)*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        float mra=MRA2+(MRA1-MRA2)/(time_c14[index_time]-time_c14[index_time-1])*(pdf_years[i]-time_c14[index_time-1]);
                        pdf_ra[i]=mra;

                        //MRA1=MRA_min[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        //MRA2=MRA_min[(index_time-1)*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        //mra=MRA2+(MRA1-MRA2)/(time_c14[index_time]-time_c14[index_time-1])*(pdf_years[i]-time_c14[index_time-1]);
                        //if (time_c14[index_time]>11500)pdf_ra_min[i]=mra;else pdf_ra_min[i]=0.0;

                        MRA1=MRA_max[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        MRA2=MRA_max[(index_time-1)*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                        mra=MRA2+(MRA1-MRA2)/(time_c14[index_time]-time_c14[index_time-1])*(pdf_years[i]-time_c14[index_time-1]);
                        if (time_c14[index_time]>11500)pdf_ra_max[i]=mra;else pdf_ra_max[i]=0.0;

                    }
                    else {
                        pdf_ra[i]=0.0;
                        //pdf_ra_min[i]=0.0;
                        pdf_ra_max[i]=0.0;
                    }

                    // Error weights and time error
                    float error1=sqrt(amsdata->get_Data(1,select)*amsdata->get_Data(1,select)+(time_c14_err[index_time]/1000.0)*(time_c14_err[index_time]/1000.0));
                    float valuep=error1*1000.0/1.0;
                    pdf_p[i]=amsdata->get_DPDF(value,valuep,pdf_years[i]);
                    } else {
                       pdf_p[i]=0.0;
                    }
                }

                // Calculate mean
                float sum_mean=0;
                float sum_w=0;
                float sum_std=0;
                for (int i=0;i<1000;i++){
                    sum_mean+=pdf_p[i]*pdf_ra[i];
                    if(pdf_ra[i]>0)sum_w+=pdf_p[i];
                }
                float mean=sum_mean/sum_w;
                //********************************
                for (int i=0;i<1000;i++){
                    sum_std+=pdf_p[i]*(pdf_ra[i]-mean)*(pdf_ra[i]-mean);
                }
                float std=sqrt(sum_std/((999.0/1000.0)*sum_w));

                /* Calculate mean_min
                sum_mean=0;
                sum_w=0;
                sum_std=0;
                for (int i=0;i<1000;i++){
                    sum_mean+=pdf_p[i]*pdf_ra_min[i];
                    if (pdf_ra_min[i]>0)sum_w+=pdf_p[i];
                }
                float mean_min=sum_mean/sum_w;
                //***********************************
                for (int i=0;i<1000;i++){
                    sum_std+=pdf_p[i]*(pdf_ra_min[i]-mean)*(pdf_ra_min[i]-mean);
                }
                float std_min=sqrt(sum_std/((999.0/1000.0)*sum_w));*/

                // Calculate mean_max
                sum_mean=0;
                sum_w=0;
                sum_std=0;
                for (int i=0;i<1000;i++){
                    sum_mean+=pdf_p[i]*pdf_ra_max[i];
                    if (pdf_ra_max[i]>0)sum_w+=pdf_p[i];
                }
                float mean_max=sum_mean/sum_w;
                //************************************
                for (int i=0;i<1000;i++){
                    sum_std+=pdf_p[i]*(pdf_ra_max[i]-mean)*(pdf_ra_max[i]-mean);
                }
                float std_max=sqrt(sum_std/((999.0/1000.0)*sum_w));

                amsdata->set_Data(3,select,round((mean/1000.0)*10000.0)/10000.0);
                //qDebug()<<QString::number((mean_max-mean)*2.0);

                if (mean_max-mean>1) amsdata->set_Reservoir_Error(select,round((
                                                                                   sqrt((std/1000.0)*(std/1000.0)
                                                                                   +(((mean_max-mean)*2.0)/4000.0)*(((mean_max-mean)*2.0)/4000.0))
                                                                                   )*10000.0)/10000.0);
                else amsdata->set_Reservoir_Error(select,round((
                                                                sqrt((std/1000.0)*(std/1000.0)
                                                                +((200.0)/4000.0)*((200.0)/4000.0))
                                                                )*10000.0)/10000.0);


                amsdata->set_Data(3,select,MRA_mean[index_time*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10000.0);
    /*
                float *ra=new float[4000];
                for (int i=0;i<1000;i++){
                    ra[0*1000+i]=time_c14[i];
                    ra[1*1000+i]=MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0-(MRA_max[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0-MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0);
                    ra[2*1000+i]=MRA_mean[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                    ra[3*1000+i]=MRA_max[i*(longitude_length*latitude_length)+index_lat*longitude_length+index_lon]/10.0;
                }
                Editor *c=new Editor(this,ra,ra,4,1000);
                c->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
                c->show();*/
                delete[] pdf_years;
                delete[] pdf_p;
                delete[] pdf_ra;
                //delete[] pdf_ra_min;
                delete[] pdf_ra_max;
                }
            }
            if (amsdata->get_Data(3,select)!=NAN) ui->plainTextEdit->appendPlainText("Reservoir Age for entry "+QString::number(q)+" taken from Dataset."); else ui->plainTextEdit->appendPlainText("Reservour Age for entry "+QString::number(q)+" could not be found!!!");

        } else {
            ui->plainTextEdit->appendPlainText("Reservoir Age for entry "+QString::number(q)+" was skipped.");
            if (amsdata->get_Data(3,select)!=NAN) ui->plainTextEdit->appendPlainText("->Reason : There was already a value.");
            if (!(amsdata->get_Data(0,select)>0 && amsdata->get_Data(0,select)<400 &&
                    amsdata->get_Data(1,select)>0 && amsdata->get_Data(1,select)<400 &&
                    amsdata->get_Data(2,select)>0 && amsdata->get_Data(2,select)<400 )) ui->plainTextEdit->appendPlainText("->Reason : Dated ages incomplete.");
        }
    }
    select=s;
    delete[] MRA_mean;
    //delete[] MRA_min;
    delete[] MRA_max;
    delete[] longitude;
    delete[] latitude;
    delete[] time;
    delete[] time_c14;
    delete[] time_c14_err;


    update();

}

void AMSAll::checkReversals(){
    for(int i=0;i<amsdata->get_Length();i++){
        int ok=1;
        if(amsdata->get_Data(7,i)==1){
            // check previous
            if(i>0){
                for (int j=i-1;j>=0;j--){
                    if (amsdata->get_Data(7,j)==1){
                        if (amsdata->get_Data(4,i)<amsdata->get_Data(4,j)) ok=0;
                    }
                }
            }
            // check followers
            if(i<amsdata->get_Length()-1){
                for(int j=i+1;j<amsdata->get_Length();j++){
                    if (amsdata->get_Data(7,j)==1){
                        if (amsdata->get_Data(4,i)>amsdata->get_Data(4,j)) ok=0;
                    }
                }

            }
            if (ok==0){
                amsdata->set_Data(7,i,0);
            }
        }
    }
}


