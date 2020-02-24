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

#include "resources.h"

Resources::Resources()
{
    // Search for Windows PAleoDataView Folder
    if (QDir::setCurrent(QDir::homePath()+"/Documents/PaleoDataView")) {
    } else {
        // Search for Mac PaleoDataView Folder
        if (QDir::setCurrent(QDir::homePath()+"/Applications/PaleoDataView")) {
        } else {
            if (QDir::setCurrent(QDir::homePath()+"/Programme/PaleoDataView")) {
            } else {
                if (QDir::setCurrent(QDir::homePath()+"/PaleoDataView")) {
                } else {
                    if (QDir::setCurrent(QDir::rootPath()+"/Applications/PaleoDataView")) {
                    } else {
                        if (QDir::setCurrent(QDir::rootPath()+"/Programme/PaleoDataView")) {
                        } else {
                            if (QDir::setCurrent(QDir::rootPath()+"/PaleoDataView")) {
                            } else {
                                QMessageBox msgBox;
                                msgBox.setText("The program was not able to locate the 'PaleoDataView' folder.");
                                msgBox.setInformativeText("This folder must be located in the 'Applications' folder of your root or home directory. It was installed together with the program. Find it and copy it to the required folder.");
                                msgBox.setStandardButtons(QMessageBox::Ok);
                                msgBox.setDefaultButton(QMessageBox::Ok);
                                int ret = msgBox.exec();
                                exit(0);
                            }
                        }
                    }
                }
            }
        }
    }

    // create defaults
    workdir="Default";

    path_PaleoDataView=QDir::currentPath();
    qDebug()<<path_PaleoDataView;
    filename_global=path_PaleoDataView+"/Resources/Map/Global_map_2.jpg";
    filename_ATL=path_PaleoDataView+"/Resources/Map/Atlantic.jpg";
    filename_PAC=path_PaleoDataView+"/Resources/Map/Pacific.jpg";
    filename_IND=path_PaleoDataView+"/Resources/Map/Indic.jpg";
    filename_MED=path_PaleoDataView+"/Resources/Map/Med.jpg";
    // Application Database
    filename_inventory=path_PaleoDataView+"/"+workdir;
    path_data=path_PaleoDataView+"/"+workdir;
    path_target=path_PaleoDataView+"/Resources/Calibration/";
    path_age=path_PaleoDataView+"/"+workdir+"/Age/";
    path_hydro=path_PaleoDataView+"/Resources/Hydro/";
    filename_C14_Cal=path_PaleoDataView+"/Resources/Calibration/IntCal13.14c";
    path_Bacon=path_PaleoDataView+"/Resources/Bacon";
    // Read Pathdata from File
    QString txt;
    QFile file(QDir::currentPath()+"/PaleoDataView_Config.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        save();
    } else {
        txt.clear();
        QTextStream in(&file);
        while(!in.atEnd()) {
            txt.append(in.readLine().toLocal8Bit()+"\n");
        }
        file.close();
    }
    //qDebug()<< txt;
    // Assign QString Variables
    QStringList lines = txt.split("\n");
    for (int i=0;i<lines.size();i++){
        QStringList fields=lines.at(i).split("=");
        if (fields.at(0)=="path_PaleoDataView"){
            path_PaleoDataView=fields.at(1);
            // apply to all paths/files
            filename_global=path_PaleoDataView+"/Resources/Map/Global_map_2.jpg";
            filename_ATL=path_PaleoDataView+"/Resources/Map/Atlantic.jpg";
            filename_PAC=path_PaleoDataView+"/Resources/Map/Pacific.jpg";
            filename_IND=path_PaleoDataView+"/Resources/Map/Indic.jpg";
            filename_MED=path_PaleoDataView+"/Resources/Map/Med.jpg";
            // Application Database
            filename_inventory=path_PaleoDataView+"/"+workdir;
            path_data=path_PaleoDataView+"/"+workdir;
            path_target=path_PaleoDataView+"/Resources/Calibration/";
            path_age=path_PaleoDataView+"/"+workdir+"/Age/";
            path_hydro=path_PaleoDataView+"/Resources/Hydro/";
            filename_C14_Cal=path_PaleoDataView+"/Resources/Calibration/IntCal13.14c";
            path_Bacon=path_PaleoDataView+"/Resources/Bacon";
        }
        if (fields.at(0)=="filename_global"){
            filename_global=fields.at(1);
        }
        if (fields.at(0)=="filename_ATL"){
            filename_ATL=fields.at(1);
        }
        if (fields.at(0)=="filename_PAC"){
            filename_PAC=fields.at(1);
        }
        if (fields.at(0)=="filename_IND"){
            filename_IND=fields.at(1);
        }
        if (fields.at(0)=="filename_MED"){
            filename_MED=fields.at(1);
        }
        if (fields.at(0)=="filename_inventory"){
            filename_inventory=fields.at(1);
        }
        if (fields.at(0)=="path_data"){
            path_data=fields.at(1);
        }
        if (fields.at(0)=="path_target"){
            path_target=fields.at(1);
        }
        if (fields.at(0)=="path_age"){
            path_age=fields.at(1);
        }
        if (fields.at(0)=="path_hydro"){
            path_hydro=fields.at(1);
        }
        if (fields.at(0)=="filename_C14_Cal"){
            filename_C14_Cal=fields.at(1);
        }
        if (fields.at(0)=="filename_Bacon"){
            path_Bacon=fields.at(1);
        }
        if (fields.at(0)=="workdir"){
            workdir=fields.at(1);
            filename_inventory=path_PaleoDataView+"/"+workdir;
            path_data=path_PaleoDataView+"/"+workdir;
            path_age=path_PaleoDataView+"/"+workdir+"/Age/";
        }
        if (fields.at(0)=="textsize"){
            textsize=fields.at(1).toInt();
        }

    }
}

QString Resources::get_path_PaleoDataView(){
    return path_PaleoDataView;
}
QString Resources::get_filename_global(){
    return filename_global;
}
QString Resources::get_filename_ATL(){
    return filename_ATL;
}
QString Resources::get_filename_PAC(){
    return filename_PAC;
}
QString Resources::get_filename_IND(){
    return filename_IND;
}
QString Resources::get_filename_MED(){
    return filename_MED;
}
QString Resources::get_filename_inventory(){
    return filename_inventory;
}
QString Resources::get_path_data(){
    return path_data;
}
QString Resources::get_path_target(){
    return path_target;
}
QString Resources::get_path_age(){
    return path_age;
}
QString Resources::get_filename_C14_Cal(){
    return filename_C14_Cal;
}
QString Resources::get_path_hydro(){
    return path_hydro;
}
QString Resources::get_path_bacon(){
    //qDebug()<<path_Bacon;
    return path_Bacon;
}

QString Resources::get_workdir(){
    return workdir;
}

void Resources::set_filename_global(QString str){
    filename_global=str;
}

void Resources::set_filename_ATL(QString str){
    filename_ATL=str;
}

void Resources::set_filename_PAC(QString str){
    filename_PAC=str;
}

void Resources::set_filename_IND(QString str){
    filename_IND=str;
}

void Resources::set_filename_MED(QString str){
    filename_MED=str;
}

void Resources::set_filename_inventory(QString str){
    filename_inventory=str;
}

void Resources::set_path_data(QString str){
    path_data=str;
}

void Resources::set_path_age(QString str){
    path_age=str;
}

void Resources::set_path_target(QString str){
    path_target=str;
}

void Resources::set_filename_C14_Cal(QString str){
    filename_C14_Cal=str;
}

void Resources::set_path_hydro(QString str){
    path_hydro=str;
}

void Resources::set_workdir(QString str){
    workdir=str;
}

void Resources::set_path_PaleoDataView(QString str){
    path_PaleoDataView=str;
    // apply to all paths/files
    filename_global=path_PaleoDataView+"/Resources/Map/Global_map_2.jpg";
    filename_ATL=path_PaleoDataView+"/Resources/Map/Atlantic.jpg";
    filename_PAC=path_PaleoDataView+"/Resources/Map/Pacific.jpg";
    filename_IND=path_PaleoDataView+"/Resources/Map/Indic.jpg";
    filename_MED=path_PaleoDataView+"/Resources/Map/Med.jpg";
    // Application Database
    filename_inventory=path_PaleoDataView+"/"+workdir;
    path_data=path_PaleoDataView+"/"+workdir;
    path_target=path_PaleoDataView+"/Resources/Calibration/";
    path_age=path_PaleoDataView+"/"+workdir+"/Age/";
    path_hydro=path_PaleoDataView+"/Resources/Hydro/";
    filename_C14_Cal=path_PaleoDataView+"/Resources/Calibration/IntCal13.14c";
    path_Bacon=path_PaleoDataView+"/Resources/Bacon";
}

void Resources::set_path_bacon(QString str){
    path_Bacon=str;
}

void Resources::save(){
    QString txt="path_PaleoDataView="+path_PaleoDataView+"\n";
    txt.append("workdir="+workdir+"\n");
    txt.append("textsize="+QString::number(textsize)+"\n");

    QFile f(QDir::currentPath()+"/PaleoDataView_Config.txt");

    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //qDebug() <<  f.errorString();
    } else {

    QTextStream out(&f);
    out<<txt;
    f.close();


    }

}

int Resources::get_Textsize(){
    return textsize;
}

int Resources::get_Pointsize(){
    QDesktopWidget *d=new QDesktopWidget();
    int h=d->screenGeometry().height();
    float f=(float)h/800.0;
    return (int)((float)(textsize*f));
}

void Resources::set_Textsize(int n){
    textsize=n;
}
