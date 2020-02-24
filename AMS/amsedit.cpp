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

#include "amsedit.h"
#include "ui_amsedit.h"

AMSEdit::AMSEdit(QWidget *parent,AMSData *ad,int sel) :
    diag(parent),amsdata(ad),select(sel),
    ui(new Ui::AMSEdit)
{
    ui->setupUi(this);
    // set Data to form
    ui->lineEdit->setText(amsdata->get_LabID(select));
    ui->comboBox->setCurrentIndex(0);
    if (amsdata->get_Type(select)=="AMS") ui->comboBox->setCurrentIndex(1);
    ui->lineEdit_2->setText(QString::number(amsdata->get_Depth(select)));
    ui->lineEdit_3->setText(QString::number(amsdata->get_Data(0,select)));
    ui->lineEdit_4->setText(QString::number(amsdata->get_Data(1,select)));
    ui->lineEdit_5->setText(QString::number(amsdata->get_Data(2,select)));
    ui->lineEdit_6->setText(QString::number(amsdata->get_Data(3,select)));
    ui->lineEdit_7->setText(QString::number(amsdata->get_Reservoir_Error(select)));
    ui->lineEdit_8->setText(QString::number(amsdata->get_Sample_Thickness(select)));
    ui->checkBox->setCheckState(Qt::Checked);
    if (amsdata->get_Data(7,select)!=1)  ui->checkBox->setCheckState(Qt::Unchecked);
    ui->plainTextEdit->setPlainText(amsdata->get_Age_Comment(select));
    this->setWindowTitle("Please Edit Entry");
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(rejected()));
    connect(this,SIGNAL(attChanged()),diag,SLOT(refresh()));

}

AMSEdit::~AMSEdit()
{
    delete ui;
}

void AMSEdit::accepted(){
    //qDebug() << "accepted";
    amsdata->set_LabID(select,ui->lineEdit->text());
    amsdata->set_Type(select,ui->comboBox->currentText());
    amsdata->set_Depth(select,ui->lineEdit_2->text().toFloat());
    amsdata->set_Age_Comment(ui->plainTextEdit->toPlainText(),select);
    amsdata->set_Data(0,select,ui->lineEdit_3->text().toFloat());
    amsdata->set_Data(1,select,ui->lineEdit_4->text().toFloat());
    amsdata->set_Data(2,select,ui->lineEdit_5->text().toFloat());
    amsdata->set_Data(3,select,ui->lineEdit_6->text().toFloat());
    amsdata->set_Reservoir_Error(select,ui->lineEdit_7->text().toFloat());
    amsdata->set_Sample_Thickness(select,ui->lineEdit_8->text().toFloat());
    if (ui->checkBox->checkState()==Qt::Checked){
        amsdata->set_Data(7,select,1);
    } else {
        amsdata->set_Data(7,select,0);
    }
    // sort list
    amsdata->sort(0,amsdata->get_Length()-1);


    emit (attChanged());
}

void AMSEdit::rejected(){
    //qDebug() << "Rejected";
}
