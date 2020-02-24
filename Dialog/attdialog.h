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

#ifndef ATTDIALOG_H
#define ATTDIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class attDialog;
}

class attDialog : public QDialog
{
    Q_OBJECT

public:
    explicit attDialog(QDialog *mainDialog, QString str="",QString origin="");
    ~attDialog();
    QString getText();

private slots:
    void rejected();
    void accepted();

signals:
    void attChanged(QString,QString);

private:
    Ui::attDialog *ui;
    QString text;
    QDialog *mainD;
    QString origin;
};

#endif // ATTDIALOG_H
