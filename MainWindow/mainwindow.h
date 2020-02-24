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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QPushButton>
#include <QImage>
#include "ui_mainwindow.h"
#include "General/inventory.h"
#include "General/resources.h"
#include "Hydro/hydrodatabase.h"
#include "Dialog/inputdialog.h"
#include "ISO/dataview.h"
#include "COR/correlate.h"
#include "Hydro/hydro.h"
#include "AMS/ams.h"
#include "AMS/amsall.h"
#include "Dialog/about.h"
#include "Dialog/credits.h"
#include "Export/export.h"
#include "Import/import.h"
#include "Import/importall.h"
#include "Import/importpangaea.h"
#include "Compare/compare.h"
#include "Extract/extract.h"
#include "Extract/extract_single.h"
#include "Calibration/calib.h"
#include "Editor/graph.h"
#include <QApplication>
#include "Convert/convert.h"
#include <QCloseEvent>
#include "DTW/dtw.h"
#include <QScreen>
#include "COR/amdata.h"
#include <QTreeWidgetItem>
#include "Convert/convert_ra.h"
#include <QSignalMapper>
#include "COR/targetimport.h"
#include "COR/importstack.h"

#include <QProgressDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    /*******************************************************************************************************/
    //  Constructor for the Mainwindow. It is only called when starting the program. It also prepares
    //  the Inventory, Worldmap Image,imports icons for the taskbar, shows the greeting widget,
    //  initiallizes plots and connects signals.
    //  Parameters  :
    //  parent      : always nullptr, because this QWidget is basically the root
    /*******************************************************************************************************/
    explicit MainWindow(QWidget *parent = nullptr);

    /*******************************************************************************************************/
    //  Destructor of the Mainwindow. Currently empty, because it is only used when closing the Software,
    //  which will delete everything anyway.
    /*******************************************************************************************************/
    ~MainWindow();

    /*******************************************************************************************************/
    //  Called when clicking on the X. Opens a message box to confirm, if the software should be closed now
    //  Parameter   :
    //  event       : not used
    /*******************************************************************************************************/
    void closeEvent (QCloseEvent *event);

    /*******************************************************************************************************/
    //  Defines the two graphs for the main window showing the proxy data. It first deletes the old data.
    //  There is a version for each different proxy.
    /*******************************************************************************************************/
    void createPlots(int i);

    /*******************************************************************************************************/
    //  Creates empty plots, just in case if there is not data available. Old graphs are deleted.
    /*******************************************************************************************************/
    void createEmptyPlots();

    /*******************************************************************************************************/
    //  Checks if a proxy is selected by the user and adds them to the tree widget
    //  listing all selected proxies/core.
    /*******************************************************************************************************/
    void createTree();

    /*******************************************************************************************************/
    //  Defines the worldmap and other maps.
    //  Parameters  :
    //  first       : if >0 it will add the map to the graph and set a starting view to initiallize the graph.
    /*******************************************************************************************************/
    void createMap(int first);

    /*******************************************************************************************************/
    //  Sets color of the buttons under the map to indicate selections
    /*******************************************************************************************************/
    void setButtons();



private slots:
    /*******************************************************************************************************/
    //  Recreates all graphs by calling their responsibe subroutines.
    //  Only done if the graph was edited(graphChanged).
    //  Can be found in any gui that uses graphs.
    /*******************************************************************************************************/
    void updateGraph();

    /*******************************************************************************************************/
    //  Is not connected to a Signal anymore, but is called at various places to update
    //  proxy graphs with new content.
    /*******************************************************************************************************/
    void redraw();

    /*******************************************************************************************************/
    //  Recreates graphs, tree and text to adapt content of gui to new selected proxy.
    /*******************************************************************************************************/
    void redraw_score();

    /*******************************************************************************************************/
    //  Allows to edit the comment by choosing 'Edit' from the comment context menü
    /*******************************************************************************************************/
    void commentContextMenu(QPoint pos);

    /*******************************************************************************************************/
    //  Allows to edit the reference by choosing 'Edit' from the reference context menü
    /*******************************************************************************************************/
    void referenceContextMenu(QPoint pos);


    //void plot1ContextMenu(QPoint pos);
    //void plot2ContextMenu(QPoint pos);


    /*******************************************************************************************************/
    //  Applys the Edited text from the inputDialog Widget.
    //  Parameters  :
    //  text        : The text that was entered into the inputDialog
    //  origin      : Given by inputDialog and tells which text to replace(reference or comment)
    /*******************************************************************************************************/
    void textChanged(QString text,QString origin);

    /*******************************************************************************************************/
    //  Calls the ISO-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void DataViewCall();

    /*******************************************************************************************************/
    //  Calls the Correlate-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void CorrelateCall();

    /*******************************************************************************************************/
    //  Calls the Hydro-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void HydroCall();

    /*******************************************************************************************************/
    //  Calls the AMS-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void AMSCall();

    /*******************************************************************************************************/
    //  Calls the AMSALL-Tool when selecting the Menu-entry
    /*******************************************************************************************************/
    void AMSAllCall();

    /*******************************************************************************************************/
    //  Calls the DTW-Tool when clicking the Tool-Button or selecting the Menu-entry. Currently disabled
    /*******************************************************************************************************/
    void DTWCall();

    /*******************************************************************************************************/
    //  Opens a window showing the version, Contact information and logos of the Palmod-Project/MARUM
    /*******************************************************************************************************/
    void AboutCall();

    /*******************************************************************************************************/
    //  Shows the credits. We used a lot of work from other people. We are thankfull for the possibility.
    /*******************************************************************************************************/
    void credits();

    /*******************************************************************************************************/
    //  Calls the Export-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void Xport();

    /*******************************************************************************************************/
    //  Calls the Import-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void Iport();

    /*******************************************************************************************************/
    //  Calls the ImportAll-Tool when selecting the Menu-entry
    /*******************************************************************************************************/
    void IportAll();

    /*******************************************************************************************************/
    //  Calls the Compare-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void Comp();

    /*******************************************************************************************************/
    //  Calls the Extract-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void Ext();

    /*******************************************************************************************************/
    //  Calls the Extract_Single-Tool when clicking the Tool-Button or selecting the Menu-entry
    /*******************************************************************************************************/
    void Ext_S();



    /*******************************************************************************************************/
    //  Sets the given Proxy Type
    //  Parameter   :
    //  str         : String containig the Proxytype. The wording is cuxial because it is also used for pathes
    //  status      : 0=read Inventory if it exista 1=always generate Inventory
    /*******************************************************************************************************/
    void setProxyType(QString str);
    void setProxyType(QString str,int status);
    void RefreshDatabase();

    /*******************************************************************************************************/
    //  Called by setProxyType() to setup the mainwindow for the proxy
    /*******************************************************************************************************/
    void setProxyForaminiferalIsotope();
    void setProxyForaminiferalMgCa();

    /*******************************************************************************************************/
    //  Calls the Calibrate-Tool when selecting the Menu-entry. Only available for the appropriate proxy
    /*******************************************************************************************************/
    void Calibrate();

    /*******************************************************************************************************/
    //  Calls the Tool to convert the big reservoir files from PANGEA into small short files used for the RA
    /*******************************************************************************************************/
    void Convert_ResAge();

    /*******************************************************************************************************/
    //  Removes selected entry from current list of selected proxies.
    /*******************************************************************************************************/
    void remove();

    /*******************************************************************************************************/
    //  Removes all proxies from the list of selected proxies.
    /*******************************************************************************************************/
    void removeall();

    /*******************************************************************************************************/
    //  Allows to select a paper as reference and add the path to the proxy meta data
    /*******************************************************************************************************/
    void set_EPaper();

    /*******************************************************************************************************/
    //  Opens the reference-file with the appropriate tool
    /*******************************************************************************************************/
    void view_EPaper();

    /*******************************************************************************************************/
    //  Initiallizes the window with the new data if a proxy is selected by mouse-click
    /*******************************************************************************************************/
    void proxyClicked();

    /*******************************************************************************************************/
    //  Resets the Layout by pressing F1 to default values
    /*******************************************************************************************************/
    void ResetLayout();

    /*******************************************************************************************************/
    //  Start the Bacon tool
    /*******************************************************************************************************/
    void RunBacon();

    /*******************************************************************************************************/
    //  Start the Covert tool to transform ascii tables into netCDF files. Used for Stacks in Correlate tool
    /*******************************************************************************************************/
    void Conv();

    /*******************************************************************************************************/
    //  Catches a signal from the plots if dots were selected by double click or ALT-Frame
    //  Parameters   :
    //  plot         : Source of the signal within a graphicsView
    //  x,y          : Positions of the selected dots
    /*******************************************************************************************************/
    void selection_changed(int plot,QList<int> x,QList<int> y);

    /*******************************************************************************************************/
    //  Catches a signal from the plots if a dot was selected by single click. Shows also available meta-data
    //  of the proxy.
    //  Parameters   :
    //  plot         : Source of the signal within a graphicsView
    //  x,y          : Positions of the selected dot
    /*******************************************************************************************************/
    void selected_changed(int plot,int x,int y);

    /*******************************************************************************************************/
    //  Toggles labels in map on/off. Not used anymore.
    /*******************************************************************************************************/
    void invertLabel();

    /*******************************************************************************************************/
    //  Switches to ATL view in the map-window.
    /*******************************************************************************************************/
    void MapToATL();

    /*******************************************************************************************************/
    //  Switches to PAC view in the map-window.
    /*******************************************************************************************************/
    void MapToPAC();

    /*******************************************************************************************************/
    //  Switches to IND view in the map-window.
    /*******************************************************************************************************/
    void MapToIND();

    /*******************************************************************************************************/
    //  Switches to MED view in the map-window.
    /*******************************************************************************************************/
    void MapToMED();

    /*******************************************************************************************************/
    //  Switches to Worldmap view in the map-window.
    /*******************************************************************************************************/
    void MapToWorld();

    /*******************************************************************************************************/
    //  Allows to select a different PaleoDataView-Folder. Restarts the software after changing the config file.
    /*******************************************************************************************************/
    void Change_PV_Folder();

    /*******************************************************************************************************/
    //  Allows to create a new working directory. All necessary folders will be created under the
    //  selected name.
    /*******************************************************************************************************/
    void New_Working_Directory();

    /*******************************************************************************************************/
    //  Change to a different working directory. This will restart the software.
    /*******************************************************************************************************/
    void Change_Working_Directory();

    /*******************************************************************************************************/
    //  Allows to import isotope data from a PANGAEA *.tab file
    /*******************************************************************************************************/
    void ImportP();

    void Verify();

    void ImportTarget();

    void CallImportStack();

    void DeleteFile();

protected:
    /*******************************************************************************************************/
    //  If F1 is pressed call resetLayout()
    /*******************************************************************************************************/
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindow *ui;
    QVector <QAction*> proxyAction;

    /*******************************************************************************************************/
    //  Standart Mouse actions. Currently not used.
    /*******************************************************************************************************/
    void mousePressEvent(QMouseEvent* mouseEvent1);
    void mouseReleaseEvent(QMouseEvent* mouseEvent2);
    void mouseMoveEvent(QMouseEvent* mouseEvent3);

    /*******************************************************************************************************/
    //  Repaint graphic views.
    /*******************************************************************************************************/
    void paintEvent(QPaintEvent*);

    /*******************************************************************************************************/
    //  Inventory Object containing database meta data. It is created and filled with data in the Constructor.
    /*******************************************************************************************************/
    Inventory *inv;

    /*******************************************************************************************************/
    //  Objects and datastorage for the bottom proxy plot
    /*******************************************************************************************************/
    Graph* pl1;
    QVector<float*> data1;
    QVector<QString*> com1;
    QVector<bool*> use1;
    QVector <QColor*> col1;
    int plotdata_1;

    /*******************************************************************************************************/
    //  Objects and datastorage for the upper proxy plot
    /*******************************************************************************************************/
    Graph* pl2;
    QVector<float*> data2;
    QVector<QString*> com2;
    QVector<bool*> use2;
    QVector <QColor*> col2;
    int plotdata_2;

    /*******************************************************************************************************/
    //  General PenStyle and Symboltype for the Proxyplots. Could be overwritten by setting file
    /*******************************************************************************************************/
    Qt::PenStyle *style;
    int *stype;

    /*******************************************************************************************************/
    //  Where to find all the Resources. Paths for PaleoDataView Folder structure.
    /*******************************************************************************************************/
    Resources *resources;

    /*******************************************************************************************************/
    //  Image data storage used for background of the different map modes except for worldmap
    /*******************************************************************************************************/
    QImage *mapimage;

    /*******************************************************************************************************/
    //  Storage for default layout positions
    /*******************************************************************************************************/
    QByteArray sp,sp_2,sp_3,sp_4,sp_5;

    /*******************************************************************************************************/
    //  Age data object for accessing age data shown in the plots
    /*******************************************************************************************************/
    AMSData *amsdata;

    /*******************************************************************************************************/
    //  Map graph storage data.
    /*******************************************************************************************************/
    Graph *map;
    float *data;
    QString *com;
    QString *lab;
    QColor *col;
    int *size;

    /*******************************************************************************************************/
    //  Currently selected map mode
    /*******************************************************************************************************/
    int mapmode=0;

    /*******************************************************************************************************/
    //  Map label flag. Not used anymore.
    /*******************************************************************************************************/
    int label_flag=0;

    /*******************************************************************************************************/
    //  Image data of world map
    /*******************************************************************************************************/
    QImage *worldmap;

    /*******************************************************************************************************/
    //  Currently selected proxy type. Following terms are in use:
    //  'Foraminiferal Isotopes'
    //  'Foraminiferal MgCa'
    /*******************************************************************************************************/
    QString Proxy_Type;

    /*******************************************************************************************************/
    //  Counts calls to accepted close, to avoid multiple exit messages
    /*******************************************************************************************************/
    int numCall=0;



};

#endif // MAINWINDOW_H
