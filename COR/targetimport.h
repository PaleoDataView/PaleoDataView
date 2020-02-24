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

#ifndef TARGETIMPORT_H
#define TARGETIMPORT_H

#include <QDialog>
#include <QStandardItemModel>
#include "General/inventory.h"
#include <QDebug>
#include <QMainWindow>

#include <QFileDialog>
#include "General/resources.h"
#include <QDesktopServices>
#include "QtXlsx/src/xlsx/xlsxdocument.h"
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
using namespace QXlsx;

namespace Ui {
class TargetImport;
}

class TargetImport : public QDialog
{
    Q_OBJECT

public:
    explicit TargetImport(QMainWindow *mainWindow,Inventory *inventory,QString str);
    ~TargetImport();

private:
    void setupTable();


private slots:
    void browse();
    void browse2();
    void import();

    void copy(QPoint p);
    void target();
    void copy_3(QPoint p);
    void NetCDF_Error(QString fname,QString action,int value);
    void NetCDF_Error_Message();


signals:
    void refresh();


protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::TargetImport *ui;
    Resources resources;
    Inventory *inv;
    Inventory *inv2;

    QString txt;
    QMainWindow *mainW;
    QString proxy;

    QStringList str1;
    QStringList str2;
    QString dlm;

    QStandardItemModel metaData;
    QStandardItemModel modelIsotope;


    int exist_meta;
    int exist_iso;

    int save_OK;
    QByteArray sp,sp_2,sp_3;

    QStringList target_age_mean;
    QStringList target_age_Q05;
    QStringList target_age_Q50;
    QStringList target_age_Q95;
    int target_age_mean_c;
    int target_age_Q05_c;
    int target_age_Q50_c;
    int target_age_Q95_c;

    QString error_text;
    int show_error=0;
};

#endif // TARGETIMPORT_H
