#ifndef IMPORTSTACK_H
#define IMPORTSTACK_H

#include <QDialog>
#include <QStandardItemModel>
#include "General/inventory.h"
#include <QDebug>
#include <QMainWindow>

#include <QFileDialog>
#include "General/resources.h"
#include <QDesktopServices>
#include "QtXlsx/src/xlsx/xlsxdocument.h"
#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
using namespace QXlsx;


namespace Ui {
class ImportStack;
}

class ImportStack : public QDialog
{
    Q_OBJECT

public:
    explicit ImportStack(QMainWindow *mainWindow,Inventory *inventory,QString str);
    ~ImportStack();
    void createTree();

private slots:
    void browse();
    void parse();
    void save();


protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ImportStack *ui;

    Resources resources;
    Inventory *inv2;


    QMainWindow *mainW;
    QString proxy;

    QByteArray sp;

    QString error_text;
    int show_error=0;

    int col,row;
    QStringList header;
    QStringList var_parent;
    QStringList var_child;
    QStringList data;
    std::vector<int> nrow;

    QStandardItemModel *modelData;
    int nvar;
    //QStringList att_name;
    //QStringList att_value;
    //QStringList att_type;
    //QStandardItemModel *modelAtt;
};

#endif // IMPORTSTACK_H
