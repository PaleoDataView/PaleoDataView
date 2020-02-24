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

#include "attdialog.h"
#include "ui_attdialog.h"

attDialog::attDialog(QDialog *mainDialog,QString str,QString o) :
    mainD(mainDialog),
    ui(new Ui::attDialog)
{
    text=str;
    origin=o;
    ui->setupUi(this);
    //qDebug() << text;
    ui->plainTextEdit->insertPlainText(text);
    this->setWindowTitle("Please Edit "+origin);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(rejected()));
    connect(this,SIGNAL(attChanged(QString,QString)),mainD,SLOT(attChanged(QString,QString)));

}

attDialog::~attDialog()
{
    delete ui;
}

void attDialog::accepted(){
    //qDebug() << "accepted";
    emit (attChanged(ui->plainTextEdit->toPlainText(),origin));
}

void attDialog::rejected(){

}

