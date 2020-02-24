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

#ifndef IMPORT_H
#define IMPORT_H

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
#include <QMenu>
#include <QClipboard>
using namespace QXlsx;

namespace Ui {
class Import;
}

class Import : public QDialog
{
    Q_OBJECT

public:
    explicit Import(QMainWindow *mainWindow,Inventory *inventory,QString str);
    ~Import();

private:
    void setupTable();

private slots:
    void browse();
    void browse2();
    void import();
    void save();
    void copy(QPoint p);
    void copy_2(QPoint p);
    void copy_3(QPoint p);

signals:
    void refresh();


protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Import *ui;
    Resources resources;
    Inventory *inv;
    Inventory *inv2;
    AMSData *amsdata;
    QString txt;
    QMainWindow *mainW;
    QString proxy;

    QStringList str1;
    QStringList str2;
    QString dlm;

    QStandardItemModel metaData;
    QStandardItemModel modelIsotope;
    QStandardItemModel ageData;

    int exist_meta;
    int exist_iso;
    int exist_age;
    int save_OK;
    int parseOK;
    QByteArray sp,sp_2,sp_3;
};

#endif // IMPORT_H
