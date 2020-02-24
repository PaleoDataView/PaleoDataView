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

#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QDialog>
#include <QMenu>
#include <QFileDialog>
#include <QDebug>
#include <QMainWindow>
#include "General/inventory.h"
#include <QStandardItemModel>
#include "Dialog/attdialog.h"
#include <QDesktopServices>
#include <QApplication>
#include "AMS/amsdata.h"
#include <QClipboard>
#include <QKeyEvent>
#include <QVector>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QFormLayout>
#include "General/resources.h"

namespace Ui {
class DataView;
}

class DataView : public QDialog
{
    Q_OBJECT

public:
    explicit DataView(QWidget *mainWindow,Inventory *inventory,QString str);
    ~DataView();
    void reject();
protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void entrySelected(QModelIndex mi);
    void isotopeSelected(QModelIndex mi);
    void refresh();
    void browse();
    void open();
    void save();
    void select();
    void deselect();
    void invert();
    void apply();
    void reset();
    void tabSelect(int n);
    void applyIsotope(int n);
    void sortVerify();
    void attChanged(QString text,QString origin);
    void invertFlag(QModelIndex mi);
    void newBasin();

    void copy(QPoint p);
    void copy_2(QPoint p);
    void copy_5(QPoint p);
    void copy_4(QPoint p);
    void createFilter();
    void createMeta();
    void applyData(int i);
signals:
    void selectionChanged();


private:
    void setupInventory();
    void reInventory(int i,int pos);
    void selectInventory();
    void setupIsotope();
    void setupAge();
    bool checkFilter(int i);


    Ui::DataView *ui;
    QWidget *mainW;
    Inventory *inv;
    QByteArray sp,sp_2,sp_3,sp_4;
    bool changes;
    QStandardItemModel *modelInventory;
    QStandardItemModel *modelIsotopeList;
    QStandardItemModel *modelIsotope;
    QStandardItemModel *modelAge;
    attDialog *attd;
    int oldCore;
    AMSData *amsdata;
    QString proxy;

    // Filter UI
    QVector<QLabel*> lab;
    QVector<QLineEdit*> lineedit;
    QVector<QComboBox*> combobox;
    QVector<QDoubleSpinBox*> min_float;
    QVector<QDoubleSpinBox*> max_float;
    QVector<QSpinBox*> min_int;
    QVector<QSpinBox*> max_int;
    QVector<QCheckBox*> minmax_invert;
    QPushButton *button_apply;
    QPushButton *button_reset;

    // Meta UI
    QVector<QLabel*> meta_lab;
    QVector<QComboBox*> meta_combo;
    QVector<QPlainTextEdit*> meta_text;
    QVector<QDoubleSpinBox*> meta_float;
    QVector<QSpinBox*> meta_int;
    QVector<QLineEdit*> meta_line;
    QVector<QPushButton*> meta_browse;
    QVector<QPushButton*> meta_open;
    QVector<QCheckBox*> meta_flag;
    Resources *resources;
};

#endif // DATAVIEW_H






