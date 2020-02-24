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

#include "credits.h"
#include "ui_credits.h"

Credits::Credits(QMainWindow *mainWindow) :
    mainW(mainWindow),
    ui(new Ui::Credits)
{
    ui->setupUi(this);
    this->setWindowTitle("Credits");
    this->setStyleSheet("background-color:white;");

    ui->textBrowser->setOpenLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);
}

Credits::~Credits()
{
    delete ui;
}

