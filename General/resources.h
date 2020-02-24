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

#ifndef RESOURCES_H
#define RESOURCES_H

#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopWidget>
class Resources
{
public:
    Resources();
    QString get_path_PaleoDataView();
    QString get_filename_global();
    QString get_filename_ATL();
    QString get_filename_PAC();
    QString get_filename_IND();
    QString get_filename_MED();
    // Application Database
    QString get_filename_inventory();
    QString get_path_data();
    QString get_path_target();
    QString get_path_age();
    QString get_filename_C14_Cal();
    QString get_path_hydro();
    QString get_path_bacon();
    int get_Textsize();
    int get_Pointsize();
    void set_filename_global(QString str);
    void set_filename_ATL(QString str);
    void set_filename_PAC(QString str);
    void set_filename_IND(QString str);
    void set_filename_MED(QString str);
    void set_filename_inventory(QString str);
    void set_path_data(QString str);
    void set_path_age(QString str);
    void set_path_target(QString str);
    void set_filename_C14_Cal(QString str);
    void set_path_hydro(QString str);
    void set_path_PaleoDataView(QString str);
    void set_path_bacon(QString str);
    void set_workdir(QString str);
    QString get_workdir();
    void set_Textsize(int n);
    void save();

public:
    // Programm Resources
    QString path_PaleoDataView;
    QString filename_global;
    QString filename_ATL;
    QString filename_PAC;
    QString filename_IND;
    QString filename_MED;
    // Application Database
    QString filename_inventory;
    QString path_data;
    QString path_target;
    QString path_age;
    QString path_hydro;
    QString filename_C14_Cal;
    QString path_Bacon;
    // Working Directory
    QString workdir;

    int textsize=12;
};

#endif // RESOURCES_H
