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

#ifndef CONVERT_H
#define CONVERT_H

#include <QStandardItemModel>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QApplication>
#include <QMainWindow>
#include <stdlib.h>
#include "netCDF/include/netcdf.h"
#include <stdlib.h>
#include <QDir>
#include <string.h>
#include <QInputDialog>

using namespace std;

#define ERR(e) {qDebug("Error: '%s' : %i\n", nc_strerror(e)),e; exit(1);}

namespace Ui {
class Convert;
}

class Convert : public QDialog
{
    Q_OBJECT

public:
    explicit Convert(QMainWindow *mainWindow);
    ~Convert();
    void createTree();
private slots:
    void optionsChanged();
    void parse();
    void save();
    void bundleChanged();
    void editNames();

    void addAtt();
    void delAtt();
    void editAtt();
    void createAttTable();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::Convert *ui;  
    QByteArray sp,sp_2;
    int col,row;
    QStringList header;
    QStringList var_parent;
    QStringList var_child;
    QStringList data;
    std::vector<int> nrow;

    QStandardItemModel *modelData;
    QMainWindow *mainW;
    int nvar;
    QStringList att_name;
    QStringList att_value;
    QStringList att_type;
    QStandardItemModel *modelAtt;
};

#endif // CONVERT_H
