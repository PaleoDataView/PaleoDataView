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

#ifndef IMPORTALL_H
#define IMPORTALL_H

#include <QDialog>
#include <QStandardItemModel>
#include "General/inventory.h"
#include <QDebug>
#include <QMainWindow>
#include "AMS/amsdata.h"
#include <QFileDialog>
#include "General/resources.h"
#include <QDesktopServices>
#include "QtXlsx/src/xlsx/xlsxdocument.h"
#include <QMessageBox>
#include <QApplication>
using namespace QXlsx;

namespace Ui {
class ImportAll;
}

class ImportAll : public QDialog
{
    Q_OBJECT

public:
    explicit ImportAll(QMainWindow *mainWindow,Inventory *inventory,QString str);
    ~ImportAll();

private slots:
    void browse();
    void start();
signals:
    void refresh();
private:



    Ui::ImportAll *ui;
    Resources resources;
    Inventory *inv;
    Inventory *inv2;
    AMSData *amsdata;
    QString txt;
    QMainWindow *mainW;
    QString proxy;

    QString *str1;
    QString *str2;
    QString dlm;

    QStandardItemModel *metaData;
    QStandardItemModel *modelIsotope;
    QStandardItemModel *ageData;
    int meta_length;
    int exist_meta;
    int exist_iso;
    int exist_age;
    int save_OK;

    QFileInfoList list;
};

#endif // IMPORTALL_H
