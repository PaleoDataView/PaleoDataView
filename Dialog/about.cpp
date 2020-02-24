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

#include "about.h"
#include "ui_about.h"

About::About(QMainWindow *mainWindow) :
    mainW(mainWindow),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setWindowTitle("About");
    this->setStyleSheet("background-color:white;");

    resources=new Resources();
    QImageReader reader(resources->get_path_PaleoDataView()+"/Resources/Icons/PDV_icon.png");
    reader.setAutoTransform(true);
    QSize size(50,50);
    reader.setScaledSize(size);
    PVLogo = reader.read();

    ui->label_4->setPixmap(QPixmap::fromImage(PVLogo));

    QImageReader reader_2(resources->get_path_PaleoDataView()+"/Resources/Icons/Marum.jpg");
    reader_2.setAutoTransform(true);
    QSize size2(225,125);
    size2.scale(225,125,Qt::KeepAspectRatio);
    reader_2.setScaledSize(size2);
    MARUM = reader_2.read();

    ui->label_6->setPixmap(QPixmap::fromImage(MARUM));

    QImageReader reader_3(resources->get_path_PaleoDataView()+"/Resources/Icons/Palmod.png");
    reader_3.setAutoTransform(true);
    QSize size3(310,100);
    size3.scale(310,100,Qt::KeepAspectRatio);
    reader_3.setScaledSize(size3);

    Palmod = reader_3.read();
    ui->label_8->setPixmap(QPixmap::fromImage(Palmod));



}

About::~About()
{
    delete ui;
    delete resources;
}

