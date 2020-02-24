#ifndef IMPORTPANGAEA_H
#define IMPORTPANGAEA_H

#include <QDialog>
#include <QStandardItemModel>
#include "General/inventory.h"
#include <QDebug>
#include <QMainWindow>
#include "AMS/amsdata.h"
#include <QFileDialog>
#include "General/resources.h"
#include <QDesktopServices>
#include "QtXlsx/src/xlsx/xlsxdocument.h"
#include <QMessageBox>
#include <QApplication>
#include "Export/export.h"
#include <QMenu>
#include <QClipboard>
#include <QComboBox>

namespace Ui {
class ImportPangaea;
}

class ImportPangaea : public QDialog
{
    Q_OBJECT

public:
    explicit ImportPangaea(QMainWindow *mainWindow=nullptr,Inventory *inventory=nullptr,QString str="");
    ~ImportPangaea();
    void setupTable();
    void save(QString str);
    void savexlsx(QString str);

private slots:
    void browse();
    void parse();
    void save();
    void Xport();
    void copy(QPoint p);
    void copy_2(QPoint p);

signals:
    void refresh();

private:
    Ui::ImportPangaea *ui;

    Resources resources;
    Inventory *inv;
    Inventory *inv2;
    AMSData *amsdata;
    QString txt;
    QMainWindow *mainW;
    QString proxy;

    QStringList str1;
    QStringList str2;
    int *meta_Flag;
    QString dlm;

    QStandardItemModel metaData;
    QStandardItemModel modelIsotope;
    QStandardItemModel ageData;
    int meta_length;
    int exist_meta;
    int exist_iso;
    int exist_age;
    int save_OK;
    QByteArray sp,sp_2,sp_3;

    QVector<QLabel*> lab;
    QVector<QComboBox*> combobox;

};

#endif // IMPORTPANGAEA_H
