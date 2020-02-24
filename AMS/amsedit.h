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

#ifndef AMSEDIT_H
#define AMSEDIT_H

#include <QDialog>
#include "amsdata.h"


namespace Ui {
class AMSEdit;
}

class AMSEdit : public QDialog
{
    Q_OBJECT

public:

    explicit AMSEdit(QWidget *parent = 0,AMSData *ad=nullptr,int sel=-1);
    ~AMSEdit();

private slots:
    void rejected();
    void accepted();

signals:
    void attChanged();


private:

    Ui::AMSEdit *ui;
    int select;
    AMSData *amsdata;
    QWidget *diag;
};

#endif // AMSEDIT_H
