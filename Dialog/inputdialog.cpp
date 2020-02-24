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

#include "inputdialog.h"
#include "ui_inputdialog.h"

inputDialog::inputDialog(QMainWindow *mainWindow,QString str,QString o) :
    mainW(mainWindow),
    ui(new Ui::inputDialog)
{
    text=str;
    origin=o;
    ui->setupUi(this);
    //qDebug() << text;
    ui->plainTextEdit->insertPlainText(text);
    this->setWindowTitle("Please Edit "+origin);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(rejected()));
    connect(this,SIGNAL(textChanged(QString,QString)),mainW,SLOT(textChanged(QString,QString)));

}

inputDialog::~inputDialog()
{
    delete ui;
}

void inputDialog::accepted(){
    //qDebug() << "accepted";
    emit (textChanged(ui->plainTextEdit->toPlainText(),origin));
}

void inputDialog::rejected(){

}


