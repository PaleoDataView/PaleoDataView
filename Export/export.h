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

#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>
#include <QStandardItemModel>
#include "General/inventory.h"
#include <QDebug>
#include <QMainWindow>
#include "AMS/amsdata.h"
#include <QFileDialog>
#include <QMessageBox>
#include "General/resources.h"
#include <QDesktopServices>
#include "QtXlsx/src/xlsx/xlsxdocument.h"
#include <QApplication>
#include <QMenu>
#include <QClipboard>
using namespace QXlsx;

namespace Ui {
class Export;
}

class Export : public QDialog
{
    Q_OBJECT

public:
    explicit Export(QWidget *mainWindow,Inventory *inventory,QString str);
    ~Export();

    void setupTable();
    void setupASCII();
    void save(QString file);
    void savexls(QString file);
private slots:
    void metaTableSelected(QModelIndex mi);
    void bacon_metaTableSelected(QModelIndex mi);
    void delimiterChanged();
    void browse();
    void copy(QPoint p);
    void copy_2(QPoint p);
    void copy_3(QPoint p);
    void copy_4(QPoint p);
    void copy_5(QPoint p);
    void copy_6(QPoint p);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Export *ui;
    QWidget *mainW;
    Resources resources;


    QStandardItemModel *metaData;
    int *meta_Flag;
    int bacon_meta_length;
    QStandardItemModel *bacon_metaData;
    int *bacon_meta_Flag;
    QStandardItemModel *modelIsotope;
    Inventory *inv;
    QStandardItemModel *ageData;
    QStandardItemModel *baconAge;
    QStandardItemModel *baconOut;
    AMSData *amsdata;
    QString txt;


    QStringList bacon_str1;
    QStringList bacon_str2;
    QString dlm;
    QByteArray sp,sp_2;

    QString proxy;
};

#endif // EXPORT_H
