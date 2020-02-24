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

#include "amedit.h"
#include "ui_amedit.h"

AMEdit::AMEdit(QWidget *parent,AMSData *ad,int sel,QString str) :
    diag(parent),amdata(ad),select(sel),proxy(str),
    ui(new Ui::amedit)
{
    ui->setupUi(this);
    // set Data to form
    ui->lineEdit->setText(amdata->get_LabID(select));
    ui->comboBox->setCurrentIndex(0);
    if (amdata->get_Type(select)=="INA") ui->comboBox->setCurrentIndex(1);
    if (amdata->get_Type(select)=="DNA") ui->comboBox->setCurrentIndex(2);
    if (amdata->get_Type(select)=="ISA") ui->comboBox->setCurrentIndex(3);
    if (amdata->get_Type(select)=="DSA") ui->comboBox->setCurrentIndex(4);
    if (amdata->get_Type(select)=="IP") ui->comboBox->setCurrentIndex(5);
    if (amdata->get_Type(select)=="DP") ui->comboBox->setCurrentIndex(6);
    if (amdata->get_Type(select)=="II") ui->comboBox->setCurrentIndex(7);
    if (amdata->get_Type(select)=="DI") ui->comboBox->setCurrentIndex(8);
    if (amdata->get_Type(select)=="LH04") ui->comboBox->setCurrentIndex(9);
    if (amdata->get_Type(select)=="EDML") ui->comboBox->setCurrentIndex(10);
    if (amdata->get_Type(select)=="NGRIP") ui->comboBox->setCurrentIndex(11);

    ui->lineEdit_2->setText(QString::number(amdata->get_Depth(select)));

    ui->lineEdit_3->setText(QString::number(amdata->get_Data(4,select)));
    ui->lineEdit_4->setText(QString::number(amdata->get_Data(5,select)));
    ui->lineEdit_5->setText(QString::number(amdata->get_Data(6,select)));
    ui->lineEdit_6->setText(QString::number(amdata->get_Sample_Thickness(select)));
    ui->lineEdit_7->setText(QString::number(amdata->get_Data(3,select)));
    ui->lineEdit_8->setText(QString::number(amdata->get_Reservoir_Error(select)));

    ui->checkBox->setCheckState(Qt::Checked);
    if (amdata->get_Data(7,select)!=1)  ui->checkBox->setCheckState(Qt::Unchecked);
    ui->plainTextEdit->setPlainText(amdata->get_Age_Comment(select));
    this->setWindowTitle("Please Edit Entry");
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(rejected()));
    connect(this,SIGNAL(attChanged()),diag,SLOT(redraw()));

}

AMEdit::~AMEdit()
{
    delete ui;
}

void AMEdit::accepted(){
    //qDebug() << "accepted";
    amdata->set_LabID(select,ui->lineEdit->text());
    if (ui->comboBox->currentText()=="Unknown") amdata->set_Type(0,"Unknown");
    if (ui->comboBox->currentText()=="Intermediate North Atlantic") amdata->set_Type(0,"INA");
    if (ui->comboBox->currentText()=="Deep North Atlantic") amdata->set_Type(0,"DNA");
    if (ui->comboBox->currentText()=="Intermediate South Atlantic") amdata->set_Type(0,"ISA");
    if (ui->comboBox->currentText()=="Deep South Atlantic") amdata->set_Type(0,"DSA");
    if (ui->comboBox->currentText()=="Intermediate Pacific") amdata->set_Type(0,"IP");
    if (ui->comboBox->currentText()=="Deep Pacific") amdata->set_Type(0,"DP");
    if (ui->comboBox->currentText()=="Intermediate Indian") amdata->set_Type(0,"II");
    if (ui->comboBox->currentText()=="Deep Indian") amdata->set_Type(0,"DI");
    if (ui->comboBox->currentText()=="LH04") amdata->set_Type(0,"LH04");
    if (ui->comboBox->currentText()=="EDML") amdata->set_Type(0,"EDML");
    if (ui->comboBox->currentText()=="NGRIP") amdata->set_Type(0,"NGRIP");
    amdata->set_Depth(select,ui->lineEdit_2->text().toFloat());
    amdata->set_Age_Comment(ui->plainTextEdit->toPlainText(),select);
    amdata->set_Data(4,select,ui->lineEdit_3->text().toFloat());
    amdata->set_Data(5,select,ui->lineEdit_4->text().toFloat());
    amdata->set_Data(6,select,ui->lineEdit_5->text().toFloat());
    amdata->set_Sample_Thickness(select,ui->lineEdit_6->text().toFloat());
    amdata->set_Data(3,select,ui->lineEdit_7->text().toFloat());
    amdata->set_Reservoir_Error(select,ui->lineEdit_8->text().toFloat());

    if (ui->checkBox->checkState()==Qt::Checked){
        amdata->set_Data(7,select,1);
    } else {
        amdata->set_Data(7,select,0);
    }
    // sort list
    amdata->sort(0,amdata->get_Length()-1);


    emit (attChanged());
}

void AMEdit::rejected(){
    //qDebug() << "Rejected";
}
