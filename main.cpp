/********************************************************************************/
/*    This file is part of PaleoDataView.                       				*/
/*                                                                      		*/
/*    PaleoDataView is free software: you can redistribute it and/or modify 	*/
/*    it under the terms of the GNU General Public License as published by  	*/
/*    the Free Software Foundation, either version 3 of the License, or     	*/
/*    (at your option) any later version.                                   	*/
/*                                                                          	*/
/*    PaleoDataView is distributed in the hope that it will be useful,          */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of        	*/
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*/
/*    GNU General Public License for more details.                              */
/*                                                                              */
/*    You should have received a copy of the GNU General Public License         */
/*    along with PaleoDataView.  If not, see <http://www.gnu.org/licenses/>.    */
/********************************************************************************/

#include "MainWindow/mainwindow.h"
#include <QApplication>
#include <QSharedMemory>




int main(int argc, char *argv[])
{




  QApplication a(argc, argv);
  QSharedMemory shared("75d65669-bb54-4a95-82bb-b924892a7e64");
  if( !true/*!shared.create( 512, QSharedMemory::ReadWrite)*/ )
  {
  qWarning() << "Can't start more than one instance of the application.";
  exit(0);
  }
  else {
      qDebug() << "Application started successfully.";
      MainWindow w;
      w.show();
      return a.exec();

  }





}
