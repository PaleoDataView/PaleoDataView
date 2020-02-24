#ifndef CONVERT_RA_H
#define CONVERT_RA_H

#include <QDialog>
#include <QMainWindow>
#include <QFileDialog>
#include "General/resources.h"
#include <stdlib.h>
#include "netCDF/include/netcdf.h"
#define ERR(e) {qDebug("Error: '%s' : %i\n", nc_strerror(e)),e; exit(1);}
namespace Ui {
class Convert_RA;
}

class Convert_RA : public QDialog
{
    Q_OBJECT

public:
    explicit Convert_RA(QMainWindow *mainWindow);
    ~Convert_RA();

private slots:
    void browse();
    void start();

private:
    Ui::Convert_RA *ui;
    QMainWindow *mainW;
    Resources resources;

    // data as read from the file
    double *longitude;
    int longitude_length;
    double *latitude;
    int latitude_length;
    double *time;
    int time_length;
    double *MRA_mean;
    double *MRA_min;
    double *MRA_max;

    short *MRA_mean_new;
    short *MRA_min_new;
    short *MRA_max_new;
    float *longitude_new;
    float *latitude_new;
    float *time_new;
};

#endif // CONVERT_RA_H
