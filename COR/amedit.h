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

#ifndef AMEDIT_H
#define AMEDIT_H

#include <QDialog>
#include "AMS/amsdata.h"


namespace Ui {
class amedit;
}

class AMEdit : public QDialog
{
    Q_OBJECT

public:
    explicit AMEdit(QWidget *parent = 0,AMSData *ad=NULL,int sel=-1,QString str=nullptr);
    ~AMEdit();

private slots:
    void rejected();
    void accepted();

signals:
    void attChanged();


private:

    Ui::amedit *ui;
    int select;
    AMSData *amdata;
    QWidget *diag;
    QString proxy;
};

#endif // AMEDIT_H
