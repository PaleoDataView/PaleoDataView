#include "showbacon.h"
#include "ui_showbacon.h"

ShowBacon::ShowBacon(QWidget *parent,float* out,int ncol,int nrow,float min,float by,float max,int k) :
    QDialog(parent),
    ui(new Ui::ShowBacon)
{
    ui->setupUi(this);
    this->setWindowTitle("Bacon output data");
    out_Data=out;
    out_ncol=ncol;
    out_nrow=nrow;
    dmin=min;
    dby=by;
    dmax=max;
    K=k;
    //outData = new QStandardItemModel(0,0,this);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(copy(QPoint)));

    setupOut();
}

ShowBacon::~ShowBacon()
{
    delete ui;
    //delete outData;
}

void ShowBacon::setupOut(){

    //delete outData;
    outData = new QStandardItemModel(out_nrow,out_ncol,this);
    outData->setHorizontalHeaderItem(0, new QStandardItem(QString("Starting Age")));
    for (int i=0;i<out_ncol-3;i++) {
        float d1=dmin+i*((dmax-dmin)/K);
        float d2=dmin+(i+1)*((dmax-dmin)/K);
        outData->setHorizontalHeaderItem(1+i, new QStandardItem(QString("Acc(d="+QString::number(d1)+"-"+QString::number(d2)+"cm)")));
    }
    outData->setHorizontalHeaderItem(out_ncol-2, new QStandardItem(QString("Memory")));
    outData->setHorizontalHeaderItem(out_ncol-1, new QStandardItem(QString("Iteration")));



    ui->tableView->setModel(outData);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    int nrow=out_nrow;



    //QStandardItem *var_Age = new QStandardItem[nrow];
    //QStandardItem *var_Acc = new QStandardItem[nrow*out_ncol-3];
    //QStandardItem *var_Mem = new QStandardItem[nrow];
    //QStandardItem *var_It = new QStandardItem[nrow];



    for (int i=0;i<nrow;i++){
            QStandardItem *var_Age=new QStandardItem(QString::number(out_Data[0*(nrow)+i]/1000.0));
            outData->setItem(i,0,var_Age);

            for (int j=1;j<out_ncol-2;j++){
                QStandardItem *var_Acc=new QStandardItem(QString::number(out_Data[i+j*nrow]));
                outData->setItem(i,j,var_Acc);
            }
            QStandardItem *var_Mem=new QStandardItem(QString::number(out_Data[i+(out_ncol-2)*nrow]));
            outData->setItem(i,out_ncol-2,var_Mem);

            QStandardItem *var_It=new QStandardItem(QString::number(out_Data[i+(out_ncol-1)*nrow]));
            outData->setItem(i,out_ncol-1,var_It);
    }
    ui->tableView->setSortingEnabled(0);

    ui->tableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    //ui->tableView->resizeColumnsToContents();
    ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
    //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

}

void ShowBacon::copy(QPoint pos){

    QPoint globalPos = ui->tableView->mapToGlobal(pos);

    QMenu menu;
    menu.addAction("Copy");


    QAction* selectedItem = menu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->iconText()=="Copy"){
            QString clipboardString;
            QAbstractItemModel* tableModel=ui->tableView->model();
            if (tableModel!=nullptr){
            int iRows = tableModel->rowCount();
            int iCols = tableModel->columnCount();
            for (int i=0;i<iCols;i++){
                QString txt=tableModel->headerData(i, Qt::Horizontal).toString();
                txt.replace("\n"," ");
                clipboardString.append(txt+"\t");
            }
            clipboardString.append("\n");

                QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
                qSort(selectedIndexes);
                for (int i = 0; i < selectedIndexes.count(); ++i){
                    QModelIndex current = selectedIndexes[i];
                    QString displayText = current.data(Qt::DisplayRole).toString();

                     if (i + 1 < selectedIndexes.count()){
                        QModelIndex next = selectedIndexes[i+1];
                        if (next.row() != current.row()){
                            displayText.append("\n");
                        }else{
                            displayText.append("\t");
                        }
                    }
                    clipboardString.append(displayText);
                }
                qApp->clipboard()->setText(clipboardString);
            }
        }
    }
}

