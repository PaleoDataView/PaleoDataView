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

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QMainWindow>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QApplication>
#include "General/resources.h"
namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QMainWindow *mainWindow);
    ~About();


private:
    Ui::About *ui;

    QImage PVLogo;
    QImage MARUM;
    QImage Palmod;
    QImage Bremen;

    QMainWindow *mainW;

    Resources *resources;
};

#endif // ABOUT_H
