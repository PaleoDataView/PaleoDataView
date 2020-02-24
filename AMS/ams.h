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

#ifndef AMS_H
#define AMS_H

#include <QDialog>
#include <QDebug>
#include <QMainWindow>
#include <QStandardItemModel>
#include "General/inventory.h"
#include "amsdata.h"
#include "General/resources.h"
#include <QFileDialog>
#include "Dialog/attdialog.h"
#include "amsedit.h"
#include "Editor/graph.h"
#include "Bacon/bacon.h"
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QFileInfo>

namespace Ui {
class AMS;
}

class AMS : public QDialog
{
    Q_OBJECT

public:
    /*******************************************************************************************************/
    //  Constructor for the AMS tool. It initializes graphs and prepares the GUI.
    //  Parameters  :
    //  mainWindow  : refers to main window, because it is a child of it
    //  inventory   : inventory from main window. it gives also the currently selected proxy to use
    /*******************************************************************************************************/
    explicit AMS(QMainWindow *mainWindow,Inventory *inventory);

    /*******************************************************************************************************/
    //  Destructor for the AMS tool.
    /*******************************************************************************************************/
    ~AMS();

    /*******************************************************************************************************/
    //  Called when exiting the tool. Gives a message if unsaved changes present.
    /*******************************************************************************************************/
    void reject();

private:
    /*******************************************************************************************************/
    //  Repaint all graphics and GUI
    /*******************************************************************************************************/
    void paintEvent(QPaintEvent*);

    /*******************************************************************************************************/
    //  Creates the table showing the agedata.
    // Parameters  :
    // column      : Column in the table to focus on
    /*******************************************************************************************************/
    void setupTable(int column);

    /*******************************************************************************************************/
    //  Creates the age data plot
    /*******************************************************************************************************/
    void createAMSPlot();

    /*******************************************************************************************************/
    //  creates the plot showing the transferfunction for the currently selected entry in the table
    /*******************************************************************************************************/
    void createPlot();

    /*******************************************************************************************************/
    //  checks for reversals and unchecks flags of problematic entries
    /*******************************************************************************************************/
    void checkReversals();

    /*******************************************************************************************************/
    //  Suggests a reservoir age based on simulations. Based on file Reservoir_Age.nc
    //  Parameters  :
    //  mode        : 0=only for selected entry
    //              : 1=for all entries
    /*******************************************************************************************************/
    void reservoir(int mode);

protected:
    /*******************************************************************************************************/
    //  Treats Key events. F1 for layout reset and up/down for scrolling throgh the calibration plot
    //  Parameters  :
    //  obj         : The signal source.
    //  event       : the emited event. contains which key was pressed
    /*******************************************************************************************************/
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    /*******************************************************************************************************/
    //  If entry clicked in table, do calculations, set useflag ,prepare calibration plot and show results
    // This will not add the result to the data, its just a preview
    // Parameters   :
    // mi           : itemmodel index that was clicked
    /*******************************************************************************************************/
    void selected(QModelIndex mi);

    /*******************************************************************************************************/
    // sets the data that is used for calibration
    /*******************************************************************************************************/
    void set_CalibData();

    /*******************************************************************************************************/
    // put comment in age data. This is called by a input dialog to put the text into the right entry
    // Parameters   :
    // s1           : comment text
    // s2           : origin of the signal, which entry gets the comment
    /*******************************************************************************************************/
    void attChanged(QString s1, QString s2);

    /*******************************************************************************************************/
    // refresh table and recalculate everything show in the gui
    /*******************************************************************************************************/
    void refresh();

    /*******************************************************************************************************/
    // save the age data to the age file. this makes all changes permanent.
    /*******************************************************************************************************/
    void save_AMS();

    /*******************************************************************************************************/
    // reload the age data file. Undo all unsafed changes
    /*******************************************************************************************************/
    void reload();

    /*******************************************************************************************************/
    // calibrate selected entry with given calibration data that are labeled 'AMS'
    /*******************************************************************************************************/
    void calibrate();

    /*******************************************************************************************************/
    // scalibrate all age data entries that are labeled 'AMS'
    /*******************************************************************************************************/
    void calibrateAll();

    /*******************************************************************************************************/
    // opens an gui to change manually the data of the selected entry
    /*******************************************************************************************************/
    void EditEntry();

    /*******************************************************************************************************/
    // creates a new entry for age data. It is generated with default values and needs editing.
    /*******************************************************************************************************/
    void NewEntry();

    /*******************************************************************************************************/
    // Deletes entry from list. Remember that changes are only permanent if saved.
    /*******************************************************************************************************/
    void DeleteEntry();

    /*******************************************************************************************************/
    // adds the corresponding age to the age column in the proxy file. This enables vs. age plotting
    /*******************************************************************************************************/
    void applyAM();

    /*******************************************************************************************************/
    // creates all graphs
    /*******************************************************************************************************/
    void updateGraph();

    /*******************************************************************************************************/
    // allows to create a copy of the table for the clipboard
    // Parameters   :
    // p            : Cooridinates for context menu
    /*******************************************************************************************************/
    void copy(QPoint p);

    /*******************************************************************************************************/
    // creates reservoirage for selected entry
    /*******************************************************************************************************/
    void reservoirSingle();

    /*******************************************************************************************************/
    // creates reservoirage for all entries
    /*******************************************************************************************************/
    void reservoirAll();


private:
    /*******************************************************************************************************/
    // GUI variable
    /*******************************************************************************************************/
    Ui::AMS *ui;

    /*******************************************************************************************************/
    // This is a child of the MainWindow.
    /*******************************************************************************************************/
    QMainWindow *mainW;

    /*******************************************************************************************************/
    // inventory for the database, taken from the main window, because the current selection is needed
    /*******************************************************************************************************/
    Inventory *inv;

    /*******************************************************************************************************/
    // Age data loaded for the selected inventory entry
    /*******************************************************************************************************/
    AMSData *amsdata;

    /*******************************************************************************************************/
    // item model for the age data table
    /*******************************************************************************************************/
    QStandardItemModel *modelData;

    /*******************************************************************************************************/
    // Data for the Age data plot
    /*******************************************************************************************************/
    Graph *amsplot;
    float *amsplot_data;
    QString *comment;
    bool *data_use;
    int *mark;
    float *data_error;
    float *data_error2;
    QColor *col;

    /*******************************************************************************************************/
    // Data for the calibration plot
    /*******************************************************************************************************/
    Graph *calib;
    float *plot_data;
    QColor *plot_col;
    Qt::PenStyle *plot_style;
    int *plot_width;

    /*******************************************************************************************************/
    //  Where to find all the Resources. Paths for PaleoDataView Folder structure.
    /*******************************************************************************************************/
    Resources *resources;

    /*******************************************************************************************************/
    //  Input GUI for edit function
    /*******************************************************************************************************/
    AMSEdit *edit;

    /*******************************************************************************************************/
    // Currently selected entry in table
    /*******************************************************************************************************/
    int select;

    /*******************************************************************************************************/
    //  Storage for default layout positions
    /*******************************************************************************************************/
    QByteArray sp,sp_2,sp_3;

    /*******************************************************************************************************/
    //  unsaved changes to the agemodel?
    /*******************************************************************************************************/
    bool changes;

    /*******************************************************************************************************/
    //  Opens dialog for comment editing
    /*******************************************************************************************************/
    attDialog *attd;

    /*******************************************************************************************************/
    //  Value offset for the current calibration plot. It is changed with up/down.
    //  Allows to scroll through the callibration function
    /*******************************************************************************************************/
    float value_off;

};

#endif // AMS_H
