/********************************************************************************/
/*    This file is part of PaleoDataView.                       				*/
/*                                                                      		*/
/*    PaleoDataView is free software: you can redistribute it and/or modify 	*/
/*    it under the terms of the GNU General Public License as published by  	*/
/*    the Free Software Foundation, either version 3 of the License, or     	*/
/*    (at your option) any later version.                                   	*/
/*                                                                          	*/
/*    PaleoDataView is distributed in the hope that it will be useful,        */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of        	*/
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*/
/*    GNU General Public License for more details.                              */
/*                                                                              */
/*    You should have received a copy of the GNU General Public License         */
/*    along with PaleoDataView.  If not, see <http://www.gnu.org/licenses/>.  */
/********************************************************************************/

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    // genrate gui
    ui->setupUi(this);

    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    sp_3=ui->splitter_3->saveState();
    sp_4=ui->splitter_4->saveState();
    sp_5=ui->splitter_5->saveState();
    qApp->installEventFilter(this);

    //init resources
    resources=new Resources();
     this->setWindowTitle("PaleoDataView ("+resources->get_workdir()+")");




    // Search arguments for options
    int skip=0;
    for (int i=0;i<qApp->arguments().length();i++) if(qApp->arguments()[i].contains("-NoIntro")) skip=1;

    if (skip==0){
        ui->toolBar->setIconSize(QSize(32,32));
        ui->actionEdit->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/ISO.ico"));
        ui->actionCorrelation->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Cor.ico"));
        ui->actionAMS->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/AMS.ico"));
        ui->actionBacon->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Bacon.ico"));
        ui->actionDTW->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/DTW.ico"));
        ui->actionCompare->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Comp.ico"));
        ui->actionModern_Hydrography->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Hydro.ico"));
        ui->actionAbout->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/PDV_icon.ico"));
        ui->actionCredits->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/PDV_icon.ico"));
        ui->actionImport->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Import.ico"));
        ui->actionExport->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Export.ico"));
        ui->actionExtract_Time_Slice->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Extract.ico"));
        ui->actionExtract_Single_Time_Slice->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Extract.ico"));
        ui->actionExtract_Target->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Extract.ico"));
        // Show Logo
        About *a=new About(this);

        a->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint );
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->geometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        a->setGeometry(width/2-500,height/2-300,500,300);


        a->show();
        QApplication::processEvents();


        QTime dieTime= QTime::currentTime().addSecs(2);

        // Init Plot Variables
        data1.clear();
        data1.append(new float[0]);
        com1.clear();
        com1.append(new QString[0]);
        use1.clear();
        use1.append(new bool[0]);
        col1.clear();
        col1.append(new QColor[0]);
        style=new Qt::PenStyle[0];
        stype=new int[0];
        QApplication::processEvents();


        pl1=new Graph(this,data1[0],0,0);
        pl1->setComHide(0,1);

        data2.clear();
        data2.append(new float[0]);
        com2.clear();
        com2.append(new QString[0]);
        use2.clear();
        use2.append(new bool[0]);
        col2.clear();
        col2.append(new QColor[0]);


        pl2=new Graph(this,data2[0],0,0);
        pl2->setComHide(0,1);

        data=new float[0];
        com=new QString[0];
        lab=new QString[0];
        col=new QColor[0];
        size=new int[0];
        map=new Graph(this,data,0,0);


        label_flag=1;
        worldmap=new QImage();
        worldmap->load(resources->filename_global);
        qDebug()<<QString::number(worldmap->format());
        QApplication::processEvents();


        // get an inventory file
        Proxy_Type="Foraminiferal Isotopes";
        inv=new Inventory(Proxy_Type);


        amsdata=new AMSData(inv);


        while (QTime::currentTime() < dieTime) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        a->close();
        delete a;
    } else {

        QMessageBox msgBox;
        QProgressDialog progress("Restarting PaleoDataView ...","Cancel",0,1,this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setCancelButton(0);
        progress.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        progress.setMaximumHeight(100);
        progress.setMaximumWidth(500);
        progress.setMinimumDuration(0);
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->geometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        progress.setGeometry(width/2-250,height/2-50,500,100);


        progress.setValue(0);
        progress.update();
        QApplication::processEvents();

                ui->toolBar->setIconSize(QSize(32,32));
                ui->actionEdit->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/ISO.ico"));
                ui->actionCorrelation->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Cor.ico"));
                ui->actionAMS->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/AMS.ico"));
                ui->actionBacon->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Bacon.ico"));
                ui->actionDTW->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/DTW.ico"));
                ui->actionCompare->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Comp.ico"));
                ui->actionModern_Hydrography->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Hydro.ico"));
                ui->actionAbout->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/PDV_icon.ico"));
                ui->actionCredits->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/PDV_icon.ico"));
                ui->actionImport->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Import.ico"));
                ui->actionExport->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Export.ico"));
                ui->actionExtract_Time_Slice->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Extract.ico"));
                ui->actionExtract_Single_Time_Slice->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Extract.ico"));
                ui->actionExtract_Target->setIcon(QIcon(resources->get_path_PaleoDataView()+"/Resources/Icons/Extract.ico"));
        // Init Plot Variables
        data1.clear();
        data1.append(new float[0]);
        com1.clear();
        com1.append(new QString[0]);
        use1.clear();
        use1.append(new bool[0]);
        col1.clear();
        col1.append(new QColor[0]);
        style=new Qt::PenStyle[0];
        stype=new int[0];
        QApplication::processEvents();


        pl1=new Graph(this,data1[0],0,0);
        pl1->setComHide(0,1);

        data2.clear();
        data2.append(new float[0]);
        com2.clear();
        com2.append(new QString[0]);
        use2.clear();
        use2.append(new bool[0]);
        col2.clear();
        col2.append(new QColor[0]);


        pl2=new Graph(this,data2[0],0,0);
        pl2->setComHide(0,1);

        data=new float[0];
        com=new QString[0];
        lab=new QString[0];
        col=new QColor[0];
        size=new int[0];
        map=new Graph(this,data,0,0);


        label_flag=1;
        worldmap=new QImage();
        worldmap->load(resources->filename_global);
        qDebug()<<QString::number(worldmap->format());
        QApplication::processEvents();


        // get an inventory file
        Proxy_Type="Foraminiferal Isotopes";
        inv=new Inventory(Proxy_Type);


        amsdata=new AMSData(inv);
        progress.setValue(1);

        QApplication::processEvents();
        update();

        // Give control back

        progress.close();



    }

    // Add proxy menu content
    proxyAction.clear();
    QSignalMapper* signalMapper=new QSignalMapper(this);
    for (int i=0;i<inv->access_proxy_length();i++){
        proxyAction.append(new QAction(inv->access_proxy_name(i),this));
        QString str=inv->access_proxy_name(i);
        connect(proxyAction[i],SIGNAL(triggered()),signalMapper,SLOT(map()));
        signalMapper->setMapping(proxyAction[i],inv->access_proxy_name(i));
        proxyAction[i]->setCheckable(1);

        ui->menuProxy->addAction(proxyAction[i]);
    }
    connect (signalMapper, SIGNAL(mapped(QString)), this, SLOT(setProxyType(QString))) ;
    setProxyForaminiferalIsotope();
    setButtons();



    // all the buttons ...
    //connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(invertLabel()));
    connect(ui->pushButton_15,SIGNAL(clicked()),this,SLOT(MapToWorld()));
    connect(ui->pushButton_14,SIGNAL(clicked()),this,SLOT(MapToATL()));
    connect(ui->pushButton_13,SIGNAL(clicked()),this,SLOT(MapToPAC()));
    connect(ui->pushButton_12,SIGNAL(clicked()),this,SLOT(MapToIND()));
    connect(ui->pushButton_11,SIGNAL(clicked()),this,SLOT(MapToMED()));
    connect(ui->pushButton_17,SIGNAL(clicked()),this,SLOT(set_EPaper()));
    connect(ui->pushButton_16,SIGNAL(clicked()),this,SLOT(view_EPaper()));
    connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(remove()));
    ui->textBrowser_4->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textBrowser_4,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(commentContextMenu(QPoint)));
    ui->textBrowser_3->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->textBrowser_3,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(referenceContextMenu(QPoint)));
    connect(ui->pushButton_19,SIGNAL(clicked()),this,SLOT(removeall()));
    connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(proxyClicked()));
    connect(ui->actionAMS,SIGNAL(triggered(bool)),this,SLOT(AMSCall()));
    connect(ui->actionAMS_All,SIGNAL(triggered(bool)),this,SLOT(AMSAllCall()));
    connect(ui->actionDTW,SIGNAL(triggered(bool)),this,SLOT(DTWCall()));
    connect(ui->actionCorrelation,SIGNAL(triggered(bool)),this,SLOT(CorrelateCall()));
    connect(ui->actionCompare,SIGNAL(triggered(bool)),this,SLOT(Comp()));
    connect(ui->actionModern_Hydrography,SIGNAL(triggered(bool)),this,SLOT(HydroCall()));
    connect(ui->actionEdit,SIGNAL(triggered(bool)),this,SLOT(DataViewCall()));
    connect(ui->actionCalibrate,SIGNAL(triggered(bool)),this,SLOT(Calibrate()));
    connect(ui->actionBacon,SIGNAL(triggered(bool)),this,SLOT(RunBacon()));
    connect(ui->actionExtract_Time_Slice,SIGNAL(triggered(bool)),this,SLOT(Ext()));
    connect(ui->actionExtract_Single_Time_Slice,SIGNAL(triggered(bool)),this,SLOT(Ext_S()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(AboutCall()));
    connect(ui->actionCredits,SIGNAL(triggered(bool)),this,SLOT(credits()));
    connect(ui->actionExport,SIGNAL(triggered(bool)),this,SLOT(Xport()));
    connect(ui->actionImport,SIGNAL(triggered(bool)),this,SLOT(Iport()));
    connect(ui->actionImport_Folder,SIGNAL(triggered(bool)),this,SLOT(IportAll()));
    connect(ui->actionVerify_Database,SIGNAL(triggered(bool)),this,SLOT(Verify()));
    connect(ui->actionConvert,SIGNAL(triggered(bool)),this,SLOT(Conv()));
    connect(map,SIGNAL(selection(int,QList<int>,QList<int>)),this,SLOT(selection_changed(int,QList<int>,QList<int>)));
    connect(map,SIGNAL(selected(int,int,int)),this,SLOT(selected_changed(int,int,int)));
    connect(ui->actionChange_PV_Folder_2,SIGNAL(triggered(bool)),this,SLOT(Change_PV_Folder()));
    connect(ui->actionCreate_New_Working_Directory,SIGNAL(triggered(bool)),this,SLOT(New_Working_Directory()));
    connect(ui->actionChange_Working_Directory,SIGNAL(triggered(bool)),this,SLOT(Change_Working_Directory()));
    connect(pl1,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(pl2,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(map,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(ui->actionImport_Pangaea,SIGNAL(triggered(bool)),this,SLOT(ImportP()));
    connect(ui->actionConvert_Reservoir_Age_File,SIGNAL(triggered(bool)),this,SLOT(Convert_ResAge()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateGraph()));
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(updateGraph()));
    connect(ui->actionImport_Custom_Target,SIGNAL(triggered(bool)),this,SLOT(ImportTarget()));
    connect(ui->actionImport_Target_from_Excel,SIGNAL(triggered(bool)),this,SLOT(CallImportStack()));
    connect(ui->actionRefresh_Database,SIGNAL(triggered(bool)),this,SLOT(RefreshDatabase()));
    connect(ui->actionDelete_NetCDF_File,SIGNAL(triggered(bool)),this,SLOT(DeleteFile()));

    this->setWindowTitle("PaleoDataView ("+resources->get_workdir()+"->"+Proxy_Type+")");
}

// Select one of the following proxies. Not very elegant.
void MainWindow::setProxyForaminiferalIsotope(){

    setProxyType("Foraminiferal Isotopes",0);


}

void MainWindow::setProxyForaminiferalMgCa(){

    setProxyType("Foraminiferal MgCa",0);


}

void MainWindow::setProxyType(QString str){
    setProxyType(str,0);
}

void MainWindow::setProxyType(QString str, int status){



    QProgressDialog progress("Preparing Database...","Cancel",0,8,this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(0);
    progress.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setMaximumHeight(100);
    progress.setMaximumWidth(500);
    progress.setMinimumDuration(0);
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();
    progress.setGeometry(width/2-250,height/2-50,500,100);


    progress.setValue(0);
    progress.setLabelText("Preparing menus...");
    progress.show();
    progress.update();
    QApplication::processEvents();
    qDebug()<<"Setting new proxy";


    removeall();
    int proxynumber=-1;

    for(int i=0;i<inv->access_proxy_length();i++) {
        if (str==inv->access_proxy_name(i)) proxyAction[i]->setChecked(1); else proxyAction[i]->setChecked(0);
        if (str==inv->access_proxy_name(i)) proxynumber=i;
    }
    progress.setValue(1);


    QApplication::processEvents();
    if (inv->access_proxy_type(proxynumber).contains("CalibrateSST")) ui->actionCalibrate->setVisible(true); else ui->actionCalibrate->setVisible(false);

    progress.setValue(2);
    QApplication::processEvents();
    Proxy_Type=str;
    // create new Inventory for new selected proxy and update Mainwindow
    //delete inv;
    //inv=new Inventory(Proxy_Type);
    inv->setProxyType(Proxy_Type);
    progress.setValue(3);
    progress.setLabelText("Reading Basin Data...");
    QApplication::processEvents();
    inv->read_Basin();
    progress.setValue(4);
    progress.setLabelText("Generating Database...");
    QApplication::processEvents();
    // Check if inventory present or generate
    if (QDir(resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+Proxy_Type).exists("inventory.nc")&&status==0) {
        progress.setLabelText("Reading Database...");
        QApplication::processEvents();
        inv->read();
    } else {
        progress.setLabelText("Generating Database...");
        QApplication::processEvents();
        inv->generate();
    }

    progress.setValue(5);
    progress.setLabelText("Preparing Plots...");
    QApplication::processEvents();
    createEmptyPlots();
    progress.setValue(6);
    progress.setLabelText("Preparing Map...");
    QApplication::processEvents();
    mapmode=0;
    createMap(1);
    progress.setValue(7);
    progress.setLabelText("Ready...");
    QApplication::processEvents();
    map->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    ui->textBrowser->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_4->clear();
    ui->lineEdit->clear();
    ui->groupBox_10->setTitle("");
    ui->groupBox_11->setTitle("");
    progress.setValue(8);

    QApplication::processEvents();
    update();

    // Give control back

    progress.close();

    // find default values for combomenus
    plotdata_1=-1;
    plotdata_2=-1;
    for (int i=0;i<inv->access_data_length();i++) if (inv->access_data_type(i).contains("plot1")) {
        plotdata_1=i;
        break;
    }
    for (int i=0;i<inv->access_data_length();i++) if (inv->access_data_type(i).contains("plot2")) {
        plotdata_2=i;
        break;
    }

    qDebug()<<"Combobox default"+QString::number(plotdata_1);
     qDebug()<<"Combobox_2 default"+QString::number(plotdata_2);
    // set plot comboBoxes
     {
     const QSignalBlocker blocker(ui->comboBox);
     ui->comboBox->clear();
    for (int i=0;i<inv->access_data_length();i++){
        QString str=inv->access_data_name(i);
        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
        ui->comboBox->addItem(str);
        if (inv->access_data_name(i)=="Sample Thickness" || inv->access_data_name(i)=="Age"|| inv->access_data_name(i)=="Use Flag"||inv->access_data_name(i).contains("Error")||inv->access_data_name(i).contains("Comment")||inv->access_data_name(i)=="Depth"){
            // Deactivate/Hide the entry
            QListView* view = qobject_cast<QListView *>(ui->comboBox->view());
            Q_ASSERT(view != nullptr);
            view->setRowHidden(i, true);
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->comboBox->model());
            Q_ASSERT(model != nullptr);
            QStandardItem* item = model->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            if (ui->comboBox->currentIndex()==i) ui->comboBox->setCurrentIndex(-1);
        } else {
            if (ui->comboBox->currentIndex()==-1) ui->comboBox->setCurrentIndex(i);
        }

    }

    if (plotdata_1>0) ui->comboBox->setCurrentIndex(plotdata_1);
    }
    {
    const QSignalBlocker blocker(ui->comboBox_2);
    ui->comboBox_2->clear();
    for (int i=0;i<inv->access_data_length();i++){
        QString str=inv->access_data_name(i);
        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
        ui->comboBox_2->addItem(str);
        if (inv->access_data_name(i)=="Sample Thickness" || inv->access_data_name(i)=="Age"|| inv->access_data_name(i)=="Use Flag"||inv->access_data_name(i).contains("Error")||inv->access_data_name(i).contains("Comment")||inv->access_data_name(i)=="Depth"){
            // Deactivate/Hide the entry
            QListView* view = qobject_cast<QListView *>(ui->comboBox_2->view());
            Q_ASSERT(view != nullptr);
            view->setRowHidden(i, true);
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->comboBox_2->model());
            Q_ASSERT(model != nullptr);
            QStandardItem* item = model->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            if (ui->comboBox_2->currentIndex()==i) ui->comboBox_2->setCurrentIndex(-1);
        } else {
            if (ui->comboBox_2->currentIndex()==-1) ui->comboBox_2->setCurrentIndex(i);
        }

    }

    if (plotdata_2>0) ui->comboBox_2->setCurrentIndex(plotdata_2);
    }
     this->setWindowTitle("PaleoDataView ("+resources->get_workdir()+"->"+Proxy_Type+")");
    //connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateGraph()));
    //connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(updateGraph()));
}
//-------------------------------------------------------

MainWindow::~MainWindow()
{
    /* if the destructor is called it crashes
    delete ui;
    delete resources;
    delete inv;
    delete worldmap;
    delete amsdata;
    delete map;
    delete pl1;
    delete pl2;



    delete[] data1;
    delete[] com1;
    delete[] use1;
    delete[] col1;
    delete[] style;
    delete[] stype;

    delete[] data2;
    delete[] com2;
    delete[] use2;
    delete[] col2;

    delete[] data;
    delete[] com;
    delete[] lab;
    delete[] col;*/
}

/*******************************/
void MainWindow::paintEvent(QPaintEvent *)
{

    map->setSize(ui->graphicsView_3->width(),ui->graphicsView_3->height());
    pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());
    pl2->setSize(ui->graphicsView->width(),ui->graphicsView->height());

}

void MainWindow::mousePressEvent(QMouseEvent* mouseEvent1)
{
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* mouseEvent2)
{
}

void MainWindow::mouseMoveEvent(QMouseEvent* mouseEvent3)
{
}



void MainWindow::redraw(){
    if (inv->get_Entries()>0&&inv->get_currentCore()!=-1){


            createMap(0);
            createPlots(0);

            update();

    } else {

        createEmptyPlots();
        update();
    }

}




void MainWindow::redraw_score(){

    createTree();
    if (inv->get_Entries()>0&&inv->get_currentCore()!=-1){

            if(inv->get_currentCore()!=-1){


                if (inv->get_Selected(inv->get_currentCore())) map->setSelected_X(0); else map->setSelected_X(2);
                map->setSelected_Y(inv->get_currentCore());
                createMap(0);
                createPlots(1);

             } else {



                createMap(0);
                createEmptyPlots();
            }
            update();


    } else {



            createEmptyPlots();
            createMap(0);
            update();


    }
}

void MainWindow::commentContextMenu(QPoint pos){

        QPoint globalPos = ui->textBrowser_4->mapToGlobal(pos);

        QMenu commentMenu;
        commentMenu.addAction("Edit");


        QAction* selectedItem = commentMenu.exec(globalPos);
        if (selectedItem)
        {
            if (selectedItem->iconText()=="Edit"){
                inputDialog *indi=new inputDialog(this,ui->textBrowser_4->toPlainText(),"Comment");
                indi->setModal(true);
                indi->setAttribute( Qt::WA_DeleteOnClose );
                indi->show();

            }
        }

}

void MainWindow::referenceContextMenu(QPoint pos){

        QPoint globalPos = ui->textBrowser_3->mapToGlobal(pos);

        QMenu referenceMenu;
        referenceMenu.addAction("Edit");


        QAction* selectedItem = referenceMenu.exec(globalPos);
        if (selectedItem)
        {
            if (selectedItem->iconText()=="Edit"){

                inputDialog *indi=new inputDialog(this,ui->textBrowser_3->toPlainText(),"Reference");
                indi->setModal(true);
                indi->setAttribute( Qt::WA_DeleteOnClose );
                indi->show();
            }
        }

}


void MainWindow::DataViewCall(){
    if (inv->get_Entries()>0){
        //if (Proxy_Type=="Foraminiferal Isotopes"){
            DataView *iso=new DataView(this,inv,Proxy_Type);
            iso->setModal(true);
            iso->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
            iso->setAttribute( Qt::WA_DeleteOnClose );
            iso->show();
        //}
        //if (Proxy_Type=="Foraminiferal MgCa"){
        //    DataView_MgCa *iso=new DataView_MgCa(this,inv);
        //    iso->setModal(true);
        //    iso->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        //    iso->setAttribute( Qt::WA_DeleteOnClose );
        //    iso->show();
        //}
    } else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::CorrelateCall(){

    if (inv->get_currentCore()>-1){

        Correlate *cor=new Correlate(this,inv,worldmap,Proxy_Type);
        cor->setModal(true);
        cor->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        cor->setAttribute( Qt::WA_DeleteOnClose );
        cor->show();
    } else {
        QMessageBox msgBox;
        msgBox.setText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
        msgBox.exec();
    }
}

void MainWindow::ImportTarget(){



        TargetImport *ti=new TargetImport(this,inv,Proxy_Type);
        ti->setModal(true);
        ti->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        ti->setAttribute( Qt::WA_DeleteOnClose );
        ti->show();

}

void MainWindow::CallImportStack(){



        ImportStack *is=new ImportStack(this,inv,Proxy_Type);
        is->setModal(true);
        is->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        is->setAttribute( Qt::WA_DeleteOnClose );
        is->show();

}



void MainWindow::HydroCall(){
    if (inv->get_currentCore()>-1){

        Hydro *hy=new Hydro(this,inv,worldmap);
        hy->setModal(true);
        hy->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        hy->setAttribute( Qt::WA_DeleteOnClose );
        hy->show();
    } else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::AMSCall(){
    if (inv->get_currentCore()>-1){

        AMS *ams=new AMS(this,inv);
        ams->setModal(true);
        ams->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        ams->setAttribute( Qt::WA_DeleteOnClose );
        ams->show();
    } else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::AMSAllCall(){

        AMSAll *amsall=new AMSAll(this,inv,Proxy_Type);
        amsall->setModal(true);
        amsall->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        amsall->setAttribute( Qt::WA_DeleteOnClose );
        amsall->show();
}

void MainWindow::DTWCall(){
    if (inv->get_currentCore()>-1){
        DTW *dtw=new DTW(this,inv);
        dtw->setModal(true);
        dtw->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        dtw->setAttribute( Qt::WA_DeleteOnClose );
        dtw->show();
    } else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::textChanged(QString text,QString origin){
    if (origin=="Comment"&&inv->get_Entries()>0){
        //qDebug() << "Text:"+text;
        inv->set_att_Comment(text);
        inv->saveData();
        ui->textBrowser_4->setText(text);
        update();
    }
    if (origin=="Reference"&&inv->get_Entries()>0){
        //qDebug() << "Text:"+text;
        inv->set_att_Reference(text);
        inv->saveData();
        ui->textBrowser_3->setText(text);
        update();
    }



}

void MainWindow::AboutCall(){

    About *about=new About(this);
    about->setModal(true);
    about->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    about->setAttribute( Qt::WA_DeleteOnClose );

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();
    about->setGeometry(width/2-500,height/2-300,500,300);
    about->show();
}

void MainWindow::credits(){

    Credits* credit=new Credits(this);
    credit->setModal(true);
    credit->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    credit->setAttribute( Qt::WA_DeleteOnClose );
    credit->show();
}

void MainWindow::Xport(){
    if (inv->get_currentCore()>-1){

        Export *xport=new Export(this,inv,Proxy_Type);
        xport->setModal(true);
        xport->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        xport->setAttribute( Qt::WA_DeleteOnClose );
        xport->show();
    } else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }

}

void MainWindow::Iport(){

    Import *import=new Import(this,inv,Proxy_Type);
    import->setModal(true);
    import->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    import->setAttribute( Qt::WA_DeleteOnClose );
    import->show();
}
void MainWindow::ImportP(){

    ImportPangaea *import=new ImportPangaea(this,inv,Proxy_Type);
    import->setModal(true);
    import->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    import->setAttribute( Qt::WA_DeleteOnClose );
    import->show();
}

void MainWindow::IportAll(){

    ImportAll *importall=new ImportAll(this,inv,Proxy_Type);
    importall->setModal(true);
    importall->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    importall->setAttribute( Qt::WA_DeleteOnClose );
    importall->show();
}

void MainWindow::Conv(){

    Convert *conv=new Convert(this);
    conv->setModal(true);
    conv->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    conv->setAttribute( Qt::WA_DeleteOnClose );
    conv->show();
}

void MainWindow::Comp(){
    if (inv->get_Entries()>0){

        Compare *comp=new Compare(this,inv,worldmap,Proxy_Type);
        comp->setModal(true);
        comp->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        comp->setAttribute( Qt::WA_DeleteOnClose );
        comp->show();
    }
}

void MainWindow::Ext(){
    if(inv->get_Entries()>0){

        Extract *extract=new Extract(this,inv,Proxy_Type,worldmap);
        extract->setModal(true);
        extract->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        extract->setAttribute( Qt::WA_DeleteOnClose );
        extract->show();
    }
}

void MainWindow::Ext_S(){
    if(inv->get_Entries()>0){

        Extract_Single *extract=new Extract_Single(this,inv,Proxy_Type);
        extract->setModal(true);
        extract->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        extract->setAttribute( Qt::WA_DeleteOnClose );
        extract->show();
    }
}



void MainWindow::Calibrate(){
    if(inv->get_currentCore()>-1){

        Calib *cal=new Calib(this,inv,Proxy_Type);
        cal->setModal(true);
        cal->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        cal->setAttribute( Qt::WA_DeleteOnClose );
        cal->show();
    }else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::Convert_ResAge(){


        Convert_RA *ra=new Convert_RA(this);
        ra->setModal(true);
        ra->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        ra->setAttribute( Qt::WA_DeleteOnClose );
        ra->show();

}





void MainWindow::remove(){
    inv->set_Selected(inv->get_currentCore(),0);
    inv->set_currentCore(-1);
    redraw();
    createTree();
    createEmptyPlots();
    createMap(0);
    update();
}

void MainWindow::removeall(){
    for (int i=0;i<inv->get_Entries();i++) inv->set_Selected(i,0);
    inv->set_currentCore(-1);
    redraw();
    createTree();
    createEmptyPlots();
    createMap(0);
    update();
}

void MainWindow::set_EPaper(){
    if (inv->get_currentCore()>-1&&inv->get_Entries()>0){
        QString file = QFileDialog::getOpenFileName(this, tr("Select File"),
                                                QDir::homePath(),
                                                 tr("PDF (*.pdf)"));


        //qDebug() << file;
        if(file!=""){
            inv->set_att_EPaper(file);
            inv->saveData();
            proxyClicked();
            ui->lineEdit->setText(inv->get_att_EPaper());
        }
    } else {
        //ui->textBrowser_2->setText("No Core/Proxy selected.");
    }
}

void MainWindow::view_EPaper(){
    if (inv->get_currentCore()>-1&&inv->get_Entries()>0){
        QDesktopServices::openUrl(QUrl("file:///"+inv->get_att_EPaper(), QUrl::TolerantMode));
    } else {
        //ui->textBrowser_2->setText("No Core/Proxy selected.");
    }
}



void MainWindow::ResetLayout(){
    ui->splitter->restoreState(sp);
    ui->splitter_2->restoreState(sp_2);
    ui->splitter_3->restoreState(sp_3);
    ui->splitter_4->restoreState(sp_4);
    ui->splitter_5->restoreState(sp_5);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->graphicsView||obj==ui->graphicsView_2||obj==ui->graphicsView_3||obj==ui->treeWidget){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ResetLayout();
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::RunBacon(){
    if (inv->get_currentCore()>-1){

        Bacon *bacon=new Bacon(this,inv);
        bacon->setModal(true);
        bacon->setWindowFlags(Qt::Widget | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
        bacon->setAttribute( Qt::WA_DeleteOnClose );
        bacon->show();
    } else {

        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This tool requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    if (numCall>=1) return;// In case of multiple calls
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "PaleoDataView",
                                                                tr("Do you want to quit PaleoDataView?\n"),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
        numCall++;

    }
}

void MainWindow::createEmptyPlots(){
    delete[] style;
    style=new Qt::PenStyle[4];
    style[0]=Qt::SolidLine;
    style[1]=Qt::SolidLine;
    style[2]=Qt::NoPen;
    style[3]=Qt::NoPen;

    delete[] stype;
    stype=new int[4];
    stype[0]=0;
    stype[1]=0;
    stype[2]=1;
    stype[3]=1;

    qDebug()<<"create empty plots";
    // Estimate number of plots

    int plot1=0;/*
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("plot1")){
            plot1++;
        }
    }*/
    plot1=1;// We now support only single plots
    pl1->setNumber(plot1);

    // fill first plot with data
    // delete old graph data
    for (int i=0;i<data1.length();i++) delete[] data1[i];
    data1.clear();
    for (int i=0;i<use1.length();i++) delete[] use1[i];
    use1.clear();
    for (int i=0;i<com1.length();i++) delete[] com1[i];
    com1.clear();
    for (int i=0;i<data1.length();i++) delete[] data1[i];
    data1.clear();
    for (int i=0;i<col1.length();i++) delete[] col1[i];
    col1.clear();

    for (int k=0;k<plot1;k++){
        // fill first plot with data
        int length=1;
        data1.append(new float[length*4]);
        for (int i=0;i<length*4;i++) data1[k][i]=NAN;

        use1.append(new bool[length*4]);
        for (int i=0;i<length*4;i++) use1[k][i]=0;

        com1.append(new QString[length*4]);
        for (int i=0;i<length*4;i++) com1[k][i]="";

        col1.append(new QColor[length*4]);
        for (int i=0;i<length*4;i++) col1[k][i]=Qt::blue;
        pl1->setData(data1[k],0,0,k);
        pl1->setUse(use1[k],1,k);

        pl1->setTitel("",inv->access_data_name(0)+" in ["+inv->access_data_unit(0)+"]","",k);
        pl1->setMultiplicator(0,0,k);
        pl1->setTextSize(resources->get_Pointsize(),1,12,k);
        pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());


        pl1->setSetLineStyle(style,1,k);
        pl1->setColor(col1[k],1,k);
        pl1->setSetSymboltype(stype,1,k);
        pl1->setComment(com1[k],2,k);
        pl1->setComHide(k,1);

        pl1->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_plot1_"+inv->access_proxy_abbreviation()+"-"+QString::number(ui->comboBox->currentIndex())+".txt");
        pl1->autoSize(k);

    ui->graphicsView_2->setScene(pl1);
    ui->graphicsView_2->repaint();
    }

    // set plot comboBoxes
    {
    const QSignalBlocker blocker(ui->comboBox);
    for (int i=0;i<inv->access_data_length();i++){
        QString str="";

            str=inv->access_data_name(i);

        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");


        ui->comboBox->setItemText(i,str);
    }
    if (plotdata_1>0) ui->comboBox->setCurrentIndex(plotdata_1);
    }

    qDebug()<<"second plot";
    // Estimate number of plots

    int plot2=0;/*
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("plot2")){
            plot2++;
        }
    }*/
    plot2=1;// We now support only single plots
    pl2->setNumber(plot2);

    // delete old graph data
    for (int i=0;i<data2.length();i++) delete[] data2[i];
    data2.clear();
    for (int i=0;i<use2.length();i++) delete[] use2[i];
    use2.clear();
    for (int i=0;i<com2.length();i++) delete[] com2[i];
    com2.clear();
    for (int i=0;i<data2.length();i++) delete[] data2[i];
    data2.clear();
    for (int i=0;i<col2.length();i++) delete[] col2[i];
    col2.clear();

    for (int k=0;k<plot2;k++){
        // fill first plot with data
        int length=1;
        data2.append(new float[length*4]);
        for (int i=0;i<length*4;i++) data2[k][i]=NAN;

        use2.append(new bool[length*4]);
        for (int i=0;i<length*4;i++) use2[k][i]=0;

        com2.append(new QString[length*4]);
        for (int i=0;i<length*4;i++) com2[k][i]="";

        col2.append(new QColor[length*4]);
        for (int i=0;i<length*4;i++) col2[k][i]=Qt::blue;


        pl2->setData(data2[k],0,0,k);
        pl2->setUse(use2[k],1,k);

        pl2->setTitel("",inv->access_data_name(0)+" in ["+inv->access_data_unit(0)+"]","",k);
        pl2->setMultiplicator(0,0,k);
        pl2->setTextSize(resources->get_Pointsize(),1,12,k);
        pl2->setSize(ui->graphicsView->width(),ui->graphicsView->height());

        pl2->setSetLineStyle(style,1,k);
        pl2->setColor(col2[k],1,k);
        pl2->setSetSymboltype(stype,1,k);
        pl2->setComment(com2[k],2,k);
        pl2->setComHide(k,1);

        pl2->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_plot2_"+inv->access_proxy_abbreviation()+"-"+QString::number(ui->comboBox_2->currentIndex())+".txt");
        pl2->autoSize(k);

    ui->graphicsView->setScene(pl2);
    ui->graphicsView->repaint();
}
    // set plot comboBoxes
    {
    const QSignalBlocker blocker(ui->comboBox_2);
    for (int i=0;i<inv->access_data_length();i++){
        QString str="";

            str=inv->access_data_name(i);

        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");

        ui->comboBox_2->setItemText(i,str);
    }
    if (plotdata_2>0) ui->comboBox_2->setCurrentIndex(plotdata_2);
    }
}

void MainWindow::createPlots(int first){
    delete[] style;
    style=new Qt::PenStyle[4];
    style[0]=Qt::SolidLine;
    style[1]=Qt::SolidLine;
    style[2]=Qt::NoPen;
    style[3]=Qt::NoPen;

    delete[] stype;
    stype=new int[4];
    stype[0]=0;
    stype[1]=0;
    stype[2]=1;
    stype[3]=1;

    // Estimate number of plots

    int plot1=0;
    /*
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("plot1")){
            plot1++;
        }
    }*/
    plot1=1;// We now support only single plots
    QString titel1;
    pl1->setNumber(plot1);



    // delete old graph data
    for (int i=0;i<data1.length();i++) delete[] data1[i];
    data1.clear();
    for (int i=0;i<use1.length();i++) delete[] use1[i];
    use1.clear();
    for (int i=0;i<com1.length();i++) delete[] com1[i];
    com1.clear();
    for (int i=0;i<data1.length();i++) delete[] data1[i];
    data1.clear();
    for (int i=0;i<col1.length();i++) delete[] col1[i];
    col1.clear();


    // create plots
    for (int k=0;k<plot1;k++){
        // Search for plot information
        int c=-1;
        int i=0;
        /*for (i=0;i<inv->access_data_length();i++){
            if (inv->access_data_type(i).contains("plot1")) c++;
            if (c==k) break;
        }*/
        int data=plotdata_1;
        int flag=-1;
        int comment=-1;
        float multx=1;
        float multy=1;
        int metaflag=-1;
        QString str=inv->access_data_type(data);
        QStringList list=str.split(";");
        for(int i=0;i<list.length();i++) {
            if (list[i].contains("comment")) {
                QStringList list2=list[i].split("=");
                comment=list2[1].toInt();
            }
            if (list[i].contains("dFlag")) {
                QStringList list2=list[i].split("=");
                flag=list2[1].toInt();
            }
            if (list[i].contains("multx")) {
                QStringList list2=list[i].split("=");
                multx=list2[1].toFloat();
            }
            if (list[i].contains("multy")) {
                QStringList list2=list[i].split("=");
                multy=list2[1].toFloat();
            }
            if (list[i].contains("mFlag")) {
                QStringList list2=list[i].split("=");
                metaflag=list2[1].toInt();
            }

        }



        //get age model

        amsdata->AMSRead();

        // get length
        int length=inv->get_Length();
        if(amsdata->get_Length()>length) length=amsdata->get_Length();




        // fill first plot with data
        data1.append(new float[length*4]);
        for (int i=0;i<length*4;i++) data1[k][i]=NAN;

        use1.append(new bool[length*4]);
        for (int i=0;i<length*4;i++) use1[k][i]=0;

        com1.append(new QString[length*4]);
        for (int i=0;i<length*4;i++) com1[k][i]="";

        col1.append(new QColor[length*4]);
        for (int i=0;i<length*4;i++) col1[k][i]=Qt::blue;

        float low=INFINITY;
        float high=-INFINITY;
        int dataOK=0;
        for (int i=0;i<inv->get_Length();i++){
            data1[k][i+0*length]=inv->access_data_value(0,i).toFloat();
            data1[k][i+1*length]=inv->access_data_value(data,i).toFloat();
            if (data1[k][i+0*length]<100000 && data1[k][i+0*length]>-100000 && data1[k][i+1*length]<100000 && data1[k][i+1*length]>-100000) dataOK=1;
            if (low>inv->access_data_value(data,i).toFloat()) low=inv->access_data_value(data,i).toFloat();
            if (high<inv->access_data_value(data,i).toFloat()) high=inv->access_data_value(data,i).toFloat();
            if (flag!=-1){
                use1[k][i+0*length]=inv->access_data_value(flag,i).toInt();
                use1[k][i+1*length]=inv->access_data_value(flag,i).toInt();
            }
            if (comment!=-1){
                if (inv->access_data_value(comment,i).simplified()!="NaN"&&
                        inv->access_data_value(comment,i).simplified()!="NAN"&&
                        inv->access_data_value(comment,i).simplified()!="nan"&&
                        inv->access_data_value(comment,i).simplified()!="na"&&
                        inv->access_data_value(comment,i).simplified()!="NA") com1[k][i+1*length]=inv->access_data_value(comment,i);
            }
            col1[k][i+0*length]=Qt::black;
        }



        for (int i=0;i<amsdata->get_Length();i++){
            if (dataOK==1){
                data1[k][i+2*length]=amsdata->get_Depth(i);
                data1[k][i+3*length]=low-(high-low)*0.2;
            } else {
                data1[k][i+2*length]=NAN;
                data1[k][i+3*length]=NAN;
            }

            use1[k][i+2*length]=amsdata->get_Data(7,i);
            use1[k][i+3*length]=amsdata->get_Data(7,i);
            com1[k][i+2*length]=amsdata->get_Type(i);
            com1[k][i+3*length]="Age [kyr]:"+QString::number(amsdata->get_Data(4,i));
            if (amsdata->get_Type(i)=="AMS") {
                col1[k][i+3*length]=Qt::lightGray;
            }else{
                col1[k][i+3*length]=Qt::darkGray;
            }
            col1[k][i+2*length]=Qt::black;
        }


        pl1->setData(data1[k],4,length,k);
        pl1->setUse(use1[k],1,k);
        QString str1="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(data)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int i=0;i<list.size();i++) if(list[i].contains("%"+inv->access_data_name(data)+"=")) if (list[i].split("=").size()>0) str1=list[i].split("=").at(1);
        } else {
            str1=inv->access_data_name(data);
        }
        if (inv->access_data_unit(data)!="") str1.append(" ["+inv->access_data_unit(data)+"]");
        pl1->setTitel("",inv->access_data_name(0)+" in ["+inv->access_data_unit(0)+"]",str1,k);
        pl1->setMultiplicator(multx,multy,k);
        pl1->setTextSize(resources->get_Pointsize(),1,12,k);
        pl1->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());


        pl1->setSetLineStyle(style,1,k);
        pl1->setColor(col1[k],1,k);
        pl1->setSetSymboltype(stype,1,k);
        pl1->setComment(com1[k],2,k);
        pl1->setComHide(k,1);

        pl1->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_plot1_"+inv->access_proxy_abbreviation()+"-"+QString::number(ui->comboBox->currentIndex())+".txt");
        pl1->autoSize(k);



        titel1.append(str1);
        //if (inv->access_data_unit(data)!="") titel1.append(" ["+inv->access_data_unit(data)+"]");

        if (metaflag!=-1) {
            if (inv->access_meta_value(metaflag)=="1"){
            titel1.append(" (ready)");
            } else {
            titel1.append(" (in progress)");
            }

        }
        if (k+1<plot1) titel1.append(" / ");

    }
    // set plot comboBoxes
    {
    const QSignalBlocker blocker(ui->comboBox);
    for (int i=0;i<inv->access_data_length();i++){
        QString str="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(i)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int j=0;j<list.size();j++) if(list[j].contains("%"+inv->access_data_name(i)+"=")) if (list[j].split("=").size()>0) str=list[j].split("=").at(1);
        } else {
            str=inv->access_data_name(i);
        }
        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
        int metaflag=-1;
        QString strt=inv->access_data_type(i);
        QStringList list=strt.split(";");
        for(int j=0;j<list.length();j++) {
            if (list[j].contains("mFlag")) {
                QStringList list2=list[j].split("=");
                metaflag=list2[1].toInt();
            }

        }
        if (metaflag!=-1) {
            if (inv->access_meta_value(metaflag)=="1"){
            str.append(" (ready)");
            } else {
            str.append(" (in progress)");
            }

        }
        ui->comboBox->setItemText(i,str);
    }
    if (plotdata_1>0) ui->comboBox->setCurrentIndex(plotdata_1);
    }
    //ui->groupBox_10->setTitle(titel1);
    ui->graphicsView_2->setScene(pl1);
    ui->graphicsView_2->repaint();



    // Estimate number of plots

    int plot2=0;
    /*
    for (int i=0;i<inv->access_data_length();i++){
        if (inv->access_data_type(i).contains("plot2")){
            plot2++;
        }
    }*/
    plot2=1;// We now support only single plots
    QString titel2="";
    pl2->setNumber(plot2);



    // delete old graph data
    for (int i=0;i<data2.length();i++) delete[] data2[i];
    data2.clear();
    for (int i=0;i<use2.length();i++) delete[] use2[i];
    use2.clear();
    for (int i=0;i<com2.length();i++) delete[] com2[i];
    com2.clear();
    for (int i=0;i<data2.length();i++) delete[] data2[i];
    data2.clear();
    for (int i=0;i<col2.length();i++) delete[] col2[i];
    col2.clear();


    // create plots
    for (int k=0;k<plot2;k++){
        // Search for plot information
        int c=-1;
        int i=0;
        /*for (i=0;i<inv->access_data_length();i++){
            if (inv->access_data_type(i).contains("plot2")) c++;
            if (c==k) break;
        }*/
        int data=plotdata_2;
        int flag=-1;
        int comment=-1;
        float multx=1;
        float multy=1;
        int metaflag=-1;
        QString str=inv->access_data_type(data);
        QStringList list=str.split(";");
        for(int i=0;i<list.length();i++) {
            if (list[i].contains("comment")) {
                QStringList list2=list[i].split("=");
                comment=list2[1].toInt();
            }
            if (list[i].contains("dFlag")) {
                QStringList list2=list[i].split("=");
                flag=list2[1].toInt();
            }
            if (list[i].contains("multx")) {
                QStringList list2=list[i].split("=");
                multx=list2[1].toFloat();
            }
            if (list[i].contains("multy")) {
                QStringList list2=list[i].split("=");
                multy=list2[1].toFloat();
            }
            if (list[i].contains("mFlag")) {
                QStringList list2=list[i].split("=");
                metaflag=list2[1].toInt();
            }

        }



        //get age model

        amsdata->AMSRead();

        // get length
        int length=inv->get_Length();
        if(amsdata->get_Length()>length) length=amsdata->get_Length();




        // fill first plot with data
        data2.append(new float[length*4]);
        for (int i=0;i<length*4;i++) data2[k][i]=NAN;

        use2.append(new bool[length*4]);
        for (int i=0;i<length*4;i++) use2[k][i]=0;

        com2.append(new QString[length*4]);
        for (int i=0;i<length*4;i++) com2[k][i]="";

        col2.append(new QColor[length*4]);
        for (int i=0;i<length*4;i++) col2[k][i]=Qt::blue;

        float low=INFINITY;
        float high=-INFINITY;
        int dataOK=0;
        for (int i=0;i<inv->get_Length();i++){
            data2[k][i+0*length]=inv->access_data_value(0,i).toFloat();
            data2[k][i+1*length]=inv->access_data_value(data,i).toFloat();
            if (data2[k][i+0*length]<100000 && data2[k][i+0*length]>-100000 && data2[k][i+1*length]<100000 && data2[k][i+1*length]>-100000) dataOK=1;
            if (low>inv->access_data_value(data,i).toFloat()) low=inv->access_data_value(data,i).toFloat();
            if (high<inv->access_data_value(data,i).toFloat()) high=inv->access_data_value(data,i).toFloat();
            if (flag!=-1){
                use2[k][i+0*length]=inv->access_data_value(flag,i).toInt();
                use2[k][i+1*length]=inv->access_data_value(flag,i).toInt();
            }
            if (comment!=-1){
                if (inv->access_data_value(comment,i).simplified()!="NaN"&&
                        inv->access_data_value(comment,i).simplified()!="NAN"&&
                        inv->access_data_value(comment,i).simplified()!="nan"&&
                        inv->access_data_value(comment,i).simplified()!="na"&&
                        inv->access_data_value(comment,i).simplified()!="NA") com2[k][i+1*length]=inv->access_data_value(comment,i);
            }
            col2[k][i+0*length]=Qt::black;
        }



        for (int i=0;i<amsdata->get_Length();i++){
            if (dataOK==1){
                data2[k][i+2*length]=amsdata->get_Depth(i);
                data2[k][i+3*length]=low-(high-low)*0.2;
            } else {
                data2[k][i+2*length]=NAN;
                data2[k][i+3*length]=NAN;
            }

            use2[k][i+2*length]=amsdata->get_Data(7,i);
            use2[k][i+3*length]=amsdata->get_Data(7,i);
            com2[k][i+2*length]=amsdata->get_Type(i);
            com2[k][i+3*length]="Age [kyr]:"+QString::number(amsdata->get_Data(4,i));
            if (amsdata->get_Type(i)=="AMS") {
                col2[k][i+3*length]=Qt::lightGray;
            }else{
                col2[k][i+3*length]=Qt::darkGray;
            }
            col2[k][i+2*length]=Qt::black;
        }


        pl2->setData(data2[k],4,length,k);
        pl2->setUse(use2[k],1,k);
        QString str2="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(data)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int i=0;i<list.size();i++) if(list[i].contains("%"+inv->access_data_name(data)+"=")) if (list[i].split("=").size()>0) str2=list[i].split("=").at(1);
        } else {
            str2=inv->access_data_name(data);
        }
        if (inv->access_data_unit(data)!="") str2.append(" ["+inv->access_data_unit(data)+"]");
        pl2->setTitel("",inv->access_data_name(0)+" in ["+inv->access_data_unit(0)+"]",str2,k);
        pl2->setMultiplicator(multx,multy,k);
        pl2->setTextSize(resources->get_Pointsize(),1,12,k);
        pl2->setSize(ui->graphicsView->width(),ui->graphicsView->height());

        pl2->setSetLineStyle(style,1,k);
        pl2->setColor(col2[k],1,k);
        pl2->setSetSymboltype(stype,1,k);
        pl2->setComment(com2[k],2,k);
        pl2->setComHide(k,1);

        pl2->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_plot2_"+inv->access_proxy_abbreviation()+"-"+QString::number(ui->comboBox_2->currentIndex())+".txt");
        pl2->autoSize(k);



        titel2.append(str2);
        //if (inv->access_data_unit(data)!="") titel2.append(" ["+inv->access_data_unit(data)+"]");
        if (metaflag!=-1) {
            if (inv->access_meta_value(metaflag)=="1"){
            titel2.append(" (ready)");
            } else {
            titel2.append(" (in progress)");
            }

        }
        if (k+1<plot2) titel2.append(" / ");

    }

    // set plot comboBoxes
    {
    const QSignalBlocker blocker(ui->comboBox_2);
    for (int i=0;i<inv->access_data_length();i++){
        QString str="";
        if (inv->get_att_Optional().contains("%"+inv->access_data_name(i)+"=")) {
            QStringList list=inv->get_att_Optional().split(";");
            for(int j=0;j<list.size();j++) if(list[j].contains("%"+inv->access_data_name(i)+"=")) if (list[j].split("=").size()>0) str=list[j].split("=").at(1);
        } else {
            str=inv->access_data_name(i);
        }
        if (inv->access_data_unit(i)!="") str.append(" ["+inv->access_data_unit(i)+"]");
        int metaflag=-1;
        QString strt=inv->access_data_type(i);
        QStringList list=strt.split(";");
        for(int j=0;j<list.length();j++) {
            if (list[j].contains("mFlag")) {
                QStringList list2=list[j].split("=");
                metaflag=list2[1].toInt();
            }

        }
        if (metaflag!=-1) {
            if (inv->access_meta_value(metaflag)=="1"){
            str.append(" (ready)");
            } else {
            str.append(" (in progress)");
            }

        }
        ui->comboBox_2->setItemText(i,str);
    }

    if (plotdata_2>0) ui->comboBox_2->setCurrentIndex(plotdata_2);
    }
    //ui->groupBox_11->setTitle(titel2);
    ui->graphicsView->setScene(pl2);
    ui->graphicsView->repaint();





}

void MainWindow::createTree(){
    //create tree for variable names
    ui->treeWidget->clear();
    QList<QTreeWidgetItem *> cores;

    for (int i=0;i<inv->get_Entries();i++){

        if (inv->get_Selected(i)){
            //Check if already in list
            int index=-1;
            for (int j=0;j<cores.size();j++){
                if (cores.at(j)->text(0)==inv->get_Core(i)) index=j;
            }

            if (index==-1){
                // add new Core
                QStringList str;
                str<<inv->get_Core(i);
                cores.append(new QTreeWidgetItem((QTreeWidget*)0,str));
                // find new entry
                int index=-1;
                for (int j=0;j<cores.size();j++){
                    if (cores.at(j)->text(0)==inv->get_Core(i)) index=j;
                }
                // add proxy to core
                QTreeWidgetItem *proxy = new QTreeWidgetItem();
                proxy->setText(0, inv->get_Species(i));

                cores.at(index)->addChild(proxy);
            } else {
                // add proxy to core
                QTreeWidgetItem *proxy = new QTreeWidgetItem();
                proxy->setText(0, inv->get_Species(i));

                cores.at(index)->addChild(proxy);
            }
        }
    }
    ui->treeWidget->insertTopLevelItems(0,cores);
    ui->treeWidget->expandAll();
    if (inv->get_currentCore()!=-1){
        // mark current core
        for (int i=0;i<ui->treeWidget->topLevelItemCount();i++){
            if (ui->treeWidget->topLevelItem(i)->text(0)==inv->get_Core(inv->get_currentCore())){
                for(int j=0;j<ui->treeWidget->topLevelItem(i)->childCount();j++){
                    if (ui->treeWidget->topLevelItem(i)->child(j)->text(0)==inv->get_Species(inv->get_currentCore())) ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(i)->child(j));
                }
            }
        }
    }
    proxyClicked();
}

void MainWindow::proxyClicked(){
    int r_parent=ui->treeWidget->currentIndex().parent().row();
    int r_child=ui->treeWidget->currentIndex().row();
    if (r_parent==-1) {//clicked top level item
        r_parent=ui->treeWidget->currentIndex().row();
        r_child=-1;
    }
    if (r_child!=-1 && r_parent!=-1){
        QString core=ui->treeWidget->currentItem()->parent()->text(0);
        QString proxy=ui->treeWidget->currentItem()->text(0);
        // find in inventory
        int index=-1;
        for(int i=0;i<inv->get_Entries();i++){
            if (core==inv->get_Core(i) && proxy==inv->get_Species(i)) index=i;
        }
        if (index!=-1) {

            inv->set_currentCore(index);
            inv->readData(index);
            QFont font;
            font.setFamily("Courier");
            font.setStyleHint(QFont::Monospace);
            font.setFixedPitch(true);
            font.setPointSize(resources->get_Textsize());

            ui->textBrowser_3->setFont(font);
            ui->textBrowser_4->setFont(font);
            ui->textBrowser->setFont(font);

            QString str="";

            if (index>=0){
                if (inv->get_flag_Data_OK()){


                    for (int i=0;i<inv->access_meta_length();i++){
                        QString sub="";
                        sub.append("<b>"+inv->access_meta_name(i));
                        if (inv->access_meta_unit(i)!="") sub.append(" ["+inv->access_meta_unit(i)+"]");
                        for (int j=sub.length();j<20;j++) sub.append(".");
                        sub.append(": </b>"+inv->access_meta_value(i));
                        if (i+1<inv->access_meta_length()) sub.append("<br>");
                        str.append(sub);
                    }


                    ui->textBrowser_3->setText(inv->get_att_Reference());
                    ui->textBrowser_4->setText(inv->get_att_Comment());
                    ui->lineEdit->setText(inv->get_att_EPaper());
                } else {


                    for (int i=0;i<inv->access_inv_length();i++){
                        QString sub="";
                        sub.append("<b>"+inv->access_inv_name(i));
                        if (inv->access_inv_unit(i)!="") sub.append(" ["+inv->access_inv_unit(i)+"]");
                        for (int j=sub.length();j<20;j++) sub.append(".");
                        sub.append(": </b>"+inv->access_inv_data(i,inv->get_currentCore()));
                        if (i+1<inv->access_inv_length()) sub.append("<br>");
                        str.append(sub);
                    }


                }
                createPlots(0);
            } else {
                str.append("Select a Core");
                ui->textBrowser_3->setText("Select a Core");
                ui->textBrowser_4->setText("Select a Core");
                createEmptyPlots();
            }
            ui->textBrowser->setText(str);

        }
    }
    map->setSelected_Y(inv->get_currentCore());
    map->setSelected_X(0);
    redraw();
}

void MainWindow::createMap(int first){
    if (mapmode==0){
        delete[] data;
        delete[] com;
        delete[] lab;
        delete[] col;
        delete[] size;

        data=new float[inv->get_Entries()*4];
        com=new QString[inv->get_Entries()*4];
        lab=new QString[inv->get_Entries()*4];
        col=new QColor[inv->get_Entries()*4];
        size=new int[4];
        size[0]=8;
        size[1]=8;
        size[2]=5;
        size[3]=5;

        for (int i=0;i<inv->get_Entries();i++){
            data[i+2*inv->get_Entries()]=inv->get_Longitude(i);
            data[i+3*inv->get_Entries()]=inv->get_Latitude(i);
            com[i+2*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
            com[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
            lab[i+2*inv->get_Entries()]=inv->get_Core(i);
            lab[i+0*inv->get_Entries()]=inv->get_Core(i);
            col[i+2*inv->get_Entries()]=Qt::black;
            col[i+3*inv->get_Entries()]=QColor(0,0,255,128);
            if (inv->get_Selected(i)==1) {
                data[i+0*inv->get_Entries()]=inv->get_Longitude(i);
                data[i+1*inv->get_Entries()]=inv->get_Latitude(i);
                data[i+2*inv->get_Entries()]=NAN;
                data[i+3*inv->get_Entries()]=NAN;
                col[i+0*inv->get_Entries()]=Qt::black;
                col[i+1*inv->get_Entries()]=QColor(0,255,0,128);
            } else {
                data[i+0*inv->get_Entries()]=NAN;
                data[i+1*inv->get_Entries()]=NAN;
                col[i+0*inv->get_Entries()]=Qt::black;
                col[i+1*inv->get_Entries()]=Qt::black;
            }

        }
        map->setData(data,4,inv->get_Entries());
        map->setSetSymbolsize(size,1);

        map->setMultiplicator(1,1);

        map->setColor(col,true);
        map->setTitel("","Longitude","Latitude");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);


        map->setComment(com,2);

        map->setLabel(lab,label_flag);
        map->setLockAspect(1);

        map->setRepeat(2,-180,180,0,-90,90);
        map->setLimit(-900,900,-90,90);

        if (first) {
            map->setComHide(0,1);
            map->setBackground(worldmap,-180,180,-90,90,1);
        }
        map->setFolding(1,0);
        if (first) map->setView((-(180.0/ui->graphicsView_3->size().height())*ui->graphicsView_3->size().width())/2.0,((180.0/ui->graphicsView_3->size().height())*ui->graphicsView_3->size().width())/2.0,-90,90);
        map->setLineStyle(Qt::NoPen);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_Map0.txt");

        ui->graphicsView_3->setScene(map);
    }
    // data for basins
    double ATL_depth_min=-5500;
    double ATL_depth_max=0;
    double ATL_latt_min=-90;
    double ATL_latt_max=90;
    double PAC_depth_min=-5500;
    double PAC_depth_max=0;
    double PAC_latt_min=-90;
    double PAC_latt_max=60;
    double IND_depth_min=-5500;
    double IND_depth_max=0;
    double IND_latt_min=-90;
    double IND_latt_max=30;
    double MED_depth_min=-5500;
    double MED_depth_max=0;
    double MED_latt_min=-5.6;
    double MED_latt_max=36.4;

    if (mapmode==1){

        delete[] data;
        delete[] com;
        delete[] lab;
        delete[] col;
        delete[] size;

        data=new float[inv->get_Entries()*4];
        com=new QString[inv->get_Entries()*4];
        lab=new QString[inv->get_Entries()*4];
        col=new QColor[inv->get_Entries()*4];
        size=new int[4];
        size[0]=8;
        size[1]=8;
        size[2]=5;
        size[3]=5;
        for (int i=0;i<inv->get_Entries()*4;i++){
            data[i]=NAN;
            com[i]="";
            lab[i]="";
            col[i]=Qt::black;
        }

        for (int i=0;i<inv->get_Entries();i++){
            if (inv->get_Basin(inv->get_Longitude(i),inv->get_Latitude(i))==1){
                data[i+2*inv->get_Entries()]=inv->get_Latitude(i);
                data[i+3*inv->get_Entries()]=-inv->get_Water_Depth(i);
                com[i+2*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                com[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                lab[i+0*inv->get_Entries()]=inv->get_Core(i);
                lab[i+2*inv->get_Entries()]=inv->get_Core(i);
                col[i+2*inv->get_Entries()]=Qt::black;
                col[i+3*inv->get_Entries()]=QColor(0,0,255,128);
                if (inv->get_Selected(i)==1) {
                    data[i+0*inv->get_Entries()]=inv->get_Latitude(i);
                    data[i+1*inv->get_Entries()]=-inv->get_Water_Depth(i);
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=QColor(0,255,0,128);
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=Qt::black;
                }
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                }

        }
        map->setData(data,4,inv->get_Entries());
        map->setSetSymbolsize(size,1);
        map->setMultiplicator(1,1);
        map->setColor(col,true);
        map->setTitel("","Latitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,label_flag);
        map->setLockAspect(0);

        map->setRepeat(0,ATL_latt_min,ATL_latt_max,0,ATL_depth_min,ATL_depth_max);
        map->setLimit(ATL_latt_min,ATL_latt_max,ATL_depth_min,ATL_depth_max);
        if (first) {
             map->setComHide(0,1);
            map->setBackground(resources->filename_ATL,ATL_latt_min,ATL_latt_max,ATL_depth_min,ATL_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_Map1.txt");
        if (first) map->setView(ATL_latt_min,ATL_latt_max,ATL_depth_min,ATL_depth_max);
        ui->graphicsView_3->setScene(map);
    }
    if (mapmode==2){

        delete[] data;
        delete[] com;
        delete[] lab;
        delete[] col;
        delete[] size;

        data=new float[inv->get_Entries()*4];
        com=new QString[inv->get_Entries()*4];
        lab=new QString[inv->get_Entries()*4];
        col=new QColor[inv->get_Entries()*4];
        size=new int[4];
        size[0]=8;
        size[1]=8;
        size[2]=5;
        size[3]=5;
        for (int i=0;i<inv->get_Entries()*4;i++){
            data[i]=NAN;
            com[i]="";
            lab[i]="";
            col[i]=Qt::black;
        }

        for (int i=0;i<inv->get_Entries();i++){
            if (inv->get_Basin(inv->get_Longitude(i),inv->get_Latitude(i))==2){
                data[i+2*inv->get_Entries()]=inv->get_Latitude(i);
                data[i+3*inv->get_Entries()]=-inv->get_Water_Depth(i);
                com[i+2*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                com[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                lab[i+0*inv->get_Entries()]=inv->get_Core(i);
                lab[i+2*inv->get_Entries()]=inv->get_Core(i);
                col[i+2*inv->get_Entries()]=Qt::black;
                col[i+3*inv->get_Entries()]=QColor(0,0,255,128);
                if (inv->get_Selected(i)==1) {
                    data[i+0*inv->get_Entries()]=inv->get_Latitude(i);
                    data[i+1*inv->get_Entries()]=-inv->get_Water_Depth(i);
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=QColor(0,255,0,128);
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=Qt::black;
                }
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                }

        }
        map->setData(data,4,inv->get_Entries());
        map->setSetSymbolsize(size,1);
        map->setMultiplicator(1,1);
        map->setColor(col,true);
        map->setTitel("","Latitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,label_flag);
        map->setLockAspect(0);

        map->setRepeat(0,PAC_latt_min,PAC_latt_max,0,PAC_depth_min,PAC_depth_max);
        map->setLimit(PAC_latt_min,PAC_latt_max,PAC_depth_min,PAC_depth_max);

        if (first) {
             map->setComHide(0,1);
            map->setBackground(resources->filename_PAC,PAC_latt_min,PAC_latt_max,PAC_depth_min,PAC_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_Map2.txt");
        if (first) map->setView(PAC_latt_min,PAC_latt_max,PAC_depth_min,PAC_depth_max);
        ui->graphicsView_3->setScene(map);
    }
    if (mapmode==3){
        delete[] data;
        delete[] com;
        delete[] lab;
        delete[] col;
        delete[] size;

        data=new float[inv->get_Entries()*4];
        com=new QString[inv->get_Entries()*4];
        lab=new QString[inv->get_Entries()*4];
        col=new QColor[inv->get_Entries()*4];
        size=new int[4];
        size[0]=8;
        size[1]=8;
        size[2]=5;
        size[3]=5;
        for (int i=0;i<inv->get_Entries()*4;i++){
            data[i]=NAN;
            com[i]="";
            lab[i]="";
            col[i]=Qt::black;
        }

        for (int i=0;i<inv->get_Entries();i++){
            if (inv->get_Basin(inv->get_Longitude(i),inv->get_Latitude(i))==3){
                data[i+2*inv->get_Entries()]=inv->get_Latitude(i);
                data[i+3*inv->get_Entries()]=-inv->get_Water_Depth(i);
                com[i+2*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                com[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                lab[i+0*inv->get_Entries()]=inv->get_Core(i);
                lab[i+2*inv->get_Entries()]=inv->get_Core(i);
                col[i+2*inv->get_Entries()]=Qt::black;
                col[i+3*inv->get_Entries()]=QColor(0,0,255,128);
                if (inv->get_Selected(i)==1) {
                    data[i+0*inv->get_Entries()]=inv->get_Latitude(i);
                    data[i+1*inv->get_Entries()]=-inv->get_Water_Depth(i);
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=QColor(0,255,0,128);
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=Qt::black;
                }
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                }

        }
        map->setData(data,4,inv->get_Entries());
        map->setSetSymbolsize(size,1);
        map->setMultiplicator(1,1);
        map->setColor(col,true);
        map->setTitel("","Latitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,label_flag);
        map->setLockAspect(0);

        map->setRepeat(0,IND_latt_min,IND_latt_max,0,IND_depth_min,IND_depth_max);
        map->setLimit(IND_latt_min,IND_latt_max,IND_depth_min,IND_depth_max);
        if (first) {
             map->setComHide(0,1);
            map->setBackground(resources->filename_IND,IND_latt_min,IND_latt_max,IND_depth_min,IND_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_Map3.txt");
        if (first) map->setView(IND_latt_min,IND_latt_max,IND_depth_min,IND_depth_max);
        ui->graphicsView_3->setScene(map);
    }
    if (mapmode==4){
        delete[] data;
        delete[] com;
        delete[] lab;
        delete[] col;
        delete[] size;

        data=new float[inv->get_Entries()*4];
        com=new QString[inv->get_Entries()*4];
        lab=new QString[inv->get_Entries()*4];
        col=new QColor[inv->get_Entries()*4];
        size=new int[4];
        size[0]=8;
        size[1]=8;
        size[2]=5;
        size[3]=5;
        for (int i=0;i<inv->get_Entries()*4;i++){
            data[i]=NAN;
            com[i]="";
            lab[i]="";
            col[i]=Qt::black;
        }

        for (int i=0;i<inv->get_Entries();i++){
            if (inv->get_Basin(inv->get_Longitude(i),inv->get_Latitude(i))==4){
                data[i+2*inv->get_Entries()]=inv->get_Longitude(i);
                data[i+3*inv->get_Entries()]=-inv->get_Water_Depth(i);
                com[i+2*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                com[i+0*inv->get_Entries()]=inv->get_Core(i)+"\n"+inv->get_Species(i);
                lab[i+0*inv->get_Entries()]=inv->get_Core(i);
                lab[i+2*inv->get_Entries()]=inv->get_Core(i);
                col[i+2*inv->get_Entries()]=Qt::black;
                col[i+3*inv->get_Entries()]=QColor(0,0,255,128);
                if (inv->get_Selected(i)==1) {
                    data[i+0*inv->get_Entries()]=inv->get_Longitude(i);
                    data[i+1*inv->get_Entries()]=-inv->get_Water_Depth(i);
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=QColor(0,255,0,128);
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    col[i+0*inv->get_Entries()]=Qt::black;
                    col[i+1*inv->get_Entries()]=Qt::black;
                }
                } else {
                    data[i+0*inv->get_Entries()]=NAN;
                    data[i+1*inv->get_Entries()]=NAN;
                    data[i+2*inv->get_Entries()]=NAN;
                    data[i+3*inv->get_Entries()]=NAN;
                }

        }
        map->setData(data,4,inv->get_Entries());
        map->setSetSymbolsize(size,1);
        map->setMultiplicator(1,1);
        map->setColor(col,true);
        map->setTitel("","Longitude","Depth [m]");
        map->setAxisType(3,3);
        map->setTextSize(resources->get_Pointsize(),1,0);
        map->setComment(com,2);
        map->setLabel(lab,label_flag);
        map->setLockAspect(0);

        map->setRepeat(0,MED_latt_min,MED_latt_max,0,MED_depth_min,MED_depth_max);
        map->setLimit(MED_latt_min,MED_latt_max,MED_depth_min,MED_depth_max);
        if (first) {
             map->setComHide(0,1);
            map->setBackground(resources->filename_MED,MED_latt_min,MED_latt_max,MED_depth_min,MED_depth_max,1);
        }
        map->setFolding(0,0);
        map->setLineStyle(Qt::NoPen);
        map->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Main_Map4.txt");
        if (first) map->setView(MED_latt_min,MED_latt_max,MED_depth_min,MED_depth_max);
        ui->graphicsView_3->setScene(map);
    }
    ui->graphicsView_3->repaint();
}

void MainWindow::selection_changed(int plot,QList<int> x,QList<int> y){
    //qDebug()<<"Plot send:"+QString::number(plot);
    for (int i=0;i<x.length();i++){
        //qDebug()<<QString::number(x.at(i))+" : "+QString::number(y.at(i));
        if (inv->get_Selected(y.at(i))==1) {
            inv->set_Selected(y.at(i),0);
        }else{
            inv->set_Selected(y.at(i),1);
        }
        map->setSelected_X(x.at(i));
        map->setSelected_Y(y.at(i));
        inv->set_currentCore(y.at(i));



    }
    ui->treeWidget->setCurrentItem(nullptr);
    // mark current core
    for (int i=0;i<ui->treeWidget->topLevelItemCount();i++){
        if (ui->treeWidget->topLevelItem(i)->text(0)==inv->get_Core(inv->get_currentCore())){
            for(int j=0;j<ui->treeWidget->topLevelItem(i)->childCount();j++){
                if (ui->treeWidget->topLevelItem(i)->child(j)->text(0)==inv->get_Species(inv->get_currentCore())) ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(i)->child(j));
            }
        }
    }

    createMap(0);
    redraw_score();
    update();
}

void MainWindow::selected_changed(int plot,int x,int y){
    //qDebug()<<QString::number(plot)+":"+QString::number(x)+" "+QString::number(y);



        int index=y;

        if (index!=-1) {

            inv->set_currentCore(index);
            inv->readData(index);
            createPlots(0);
            QFont font;
            font.setFamily("Courier");
            font.setStyleHint(QFont::Monospace);
            font.setFixedPitch(true);
            font.setPointSize(resources->get_Textsize());

            ui->textBrowser_3->setFont(font);
            ui->textBrowser_4->setFont(font);
            ui->textBrowser->setFont(font);

            QString str="";

            if (index>=0){
                if (inv->get_flag_Data_OK()){


                    for (int i=0;i<inv->access_meta_length();i++){
                        QString sub="";
                        sub.append("<b>"+inv->access_meta_name(i));
                        if (inv->access_meta_unit(i)!="") sub.append(" ["+inv->access_meta_unit(i)+"]");
                        for (int j=sub.length();j<20;j++) sub.append(".");
                        sub.append(": </b>"+inv->access_meta_value(i));
                        if (i+1<inv->access_meta_length()) sub.append("<br>");
                        str.append(sub);
                    }
                    ui->textBrowser_3->setText(inv->get_att_Reference());
                    ui->textBrowser_4->setText(inv->get_att_Comment());
                    ui->lineEdit->setText(inv->get_att_EPaper());
                } else {

                    for (int i=0;i<inv->access_inv_length();i++){
                        QString sub="";
                        sub.append("<b>"+inv->access_inv_name(i));
                        if (inv->access_inv_unit(i)!="") sub.append(" ["+inv->access_inv_unit(i)+"]");
                        for (int j=sub.length();j<20;j++) sub.append(".");
                        sub.append(": </b>"+inv->access_inv_data(i,inv->get_currentCore()));
                        if (i+1<inv->access_inv_length()) sub.append("<br>");
                        str.append(sub);
                    }
                }
                ui->treeWidget->setCurrentItem(nullptr);
                // mark current core
                for (int i=0;i<ui->treeWidget->topLevelItemCount();i++){
                    if (ui->treeWidget->topLevelItem(i)->text(0)==inv->get_Core(inv->get_currentCore())){
                        for(int j=0;j<ui->treeWidget->topLevelItem(i)->childCount();j++){
                            if (ui->treeWidget->topLevelItem(i)->child(j)->text(0)==inv->get_Species(inv->get_currentCore())) ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(i)->child(j));
                        }
                    }
                }



            } else {
                str.append("Select a Core");
                ui->textBrowser_3->setText("Select a Core");
                ui->textBrowser_4->setText("Select a Core");
                createEmptyPlots();
            }
            ui->textBrowser->setText(str);

        }

    //redraw_score();
    update();
}

void MainWindow::invertLabel(){
    if (label_flag==1) {
        label_flag=0;
    } else {
        label_flag=1;
    }
    setButtons();
    createMap(0);
    update();
}

void MainWindow::MapToWorld(){


    mapmode=0;
    setButtons();
    createMap(1);
    update();
}

void MainWindow::MapToATL(){
    mapmode=1;
    setButtons();
    createMap(1);
    update();
}

void MainWindow::MapToPAC(){
    mapmode=2;
    setButtons();
    createMap(1);
    update();
}

void MainWindow::MapToIND(){
    mapmode=3;
    setButtons();
    createMap(1);
    update();
}

void MainWindow::MapToMED(){
    mapmode=4;
    setButtons();
    createMap(1);
    update();
}

void MainWindow::Change_PV_Folder(){
    QString old=resources->get_path_PaleoDataView();
    QString file = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
                                             resources->get_path_PaleoDataView(),
                                             QFileDialog::ShowDirsOnly);
    if (old!=file&&file!=""){
        QDir dir;
        if (dir.exists(file+"/Resources") &&
                dir.exists(file+"/Default")){
            resources->set_workdir("Default");
            resources->set_path_PaleoDataView(file);

            resources->save();
            QMessageBox msgBox;
                    msgBox.setText("The path has been changed.");
                    msgBox.setInformativeText("This requires a restart of PaleoDataView");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
            // Reset Programm
                    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
                    qApp->quit();
        } else {
            QMessageBox msgBox;
                    msgBox.setText("This folder is not a valid PaleoDataView folder.");
                    msgBox.setInformativeText("You can create a proper folder by copying the original installation folder in 'Documents'.");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
        }
    } else {
        QMessageBox msgBox;
                msgBox.setText("The path has NOT been changed.");
                msgBox.setInformativeText("Returning to PaleoDataView.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::New_Working_Directory(){
    QString Qfilename = QFileDialog::getSaveFileName(this, tr("Select Name For New Working Directory"),
                                             resources->path_PaleoDataView,
                                             tr(""));
    if (Qfilename!=""){
        QDir dir;
        int ok=1;

        if (!dir.mkpath(Qfilename)) ok=0;
        if (!dir.mkpath(Qfilename+"/Age")) ok=0;
        for (int i=0;i<inv->access_proxy_length();i++){
            if (!dir.mkpath(Qfilename+"/"+inv->access_proxy_name(i))) ok=0;
            if (!dir.mkpath(Qfilename+"/"+inv->access_proxy_name(i)+"/Data")) ok=0;
            if (!dir.mkpath(Qfilename+"/"+inv->access_proxy_name(i)+"/Import")) ok=0;
            if (!dir.mkpath(Qfilename+"/"+inv->access_proxy_name(i)+"/Derived Data")) ok=0;
            if (!dir.mkpath(Qfilename+"/"+inv->access_proxy_name(i)+"/Derived Data/Targets")) ok=0;
        }


        if (ok){
            QMessageBox msgBox;
                    msgBox.setText("Creation of a new working directory succesfull");
                    msgBox.setInformativeText("It can now be selected.");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
        } else {
            QMessageBox msgBox;
                    msgBox.setText("Was not able to create directory.");
                    msgBox.setInformativeText("Please check!");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
        }
    }
}

void MainWindow::Change_Working_Directory(){
    QString old=resources->get_workdir();
    QString file = QFileDialog::getExistingDirectory(this, tr("Select Working Directory"),
                                             resources->get_path_PaleoDataView(),
                                             QFileDialog::ShowDirsOnly);
    if (old!=file&&file!=""){
        // check if proper working directory
        QDir dir;
        // Check if all Folders are there, if not create them
        int ok=1;
        if (!dir.exists(file+"/Age")) ok=0;
        for (int i=0;i<inv->access_proxy_length();i++){
            if (!dir.mkpath(file+"/"+inv->access_proxy_name(i))) ok=0;
            if (!dir.mkpath(file+"/"+inv->access_proxy_name(i)+"/Data")) ok=0;
            if (!dir.mkpath(file+"/"+inv->access_proxy_name(i)+"/Import")) ok=0;
            if (!dir.mkpath(file+"/"+inv->access_proxy_name(i)+"/Derived Data")) ok=0;
            if (!dir.mkpath(file+"/"+inv->access_proxy_name(i)+"/Derived Data/Targets")) ok=0;
        }


        if (ok==1){
            int ok=0;
            for (int i=0;i<inv->access_proxy_length();i++){
                if (dir.exists(file+"/"+inv->access_proxy_name(i))) ok=1;
            }


            if (ok){
            QStringList filelist=file.split('/');
            file=filelist.last();


            resources->set_workdir(file);

            resources->save();
            /*
            QMessageBox msgBox;
                    msgBox.setText("The working directory has been changed.");
                    msgBox.setInformativeText("This requires a restart of PaleoDataView");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();*/
            // Reset Programm
            QStringList arg=qApp->arguments();
            int argnew=0;
            for (int i=0;i<qApp->arguments().length();i++) if(qApp->arguments()[i].contains("-NoIntro")) argnew=1;
            if (!argnew) arg.append("-NoIntro");
            QProcess::startDetached(qApp->arguments()[0],arg );
            qApp->quit();
            }
        } else {
            QMessageBox msgBox;
                    msgBox.setText("This folder is not a valid working directory.");
                    msgBox.setInformativeText("Use 'New Working Directory' to create a new one.");
                    msgBox.setIcon(QMessageBox::Warning);

                    msgBox.exec();
        }
    } else {
        QMessageBox msgBox;
                msgBox.setText("The path has NOT been changed.");
                msgBox.setInformativeText("Returning to PaleoDataView.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}

void MainWindow::updateGraph(){
    plotdata_1=ui->comboBox->currentIndex();

    plotdata_2=ui->comboBox_2->currentIndex();

    qDebug()<<"Combobox default"+QString::number(plotdata_1);
     qDebug()<<"Combobox_2 default"+QString::number(plotdata_2);
    createMap(0);
    if (inv->get_currentCore()>=0 && inv->get_currentCore()<inv->get_Entries()) createPlots(0); else createEmptyPlots();
    update();
}

void MainWindow::setButtons(){
    //if (label_flag==0) ui->pushButton_10->setStyleSheet("background-color: lightGray"); else ui->pushButton_10->setStyleSheet("background-color: darkGray");
    if (mapmode==0) ui->pushButton_15->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;");
    else ui->pushButton_15->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==1) ui->pushButton_14->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_14->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==2) ui->pushButton_13->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_13->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==3) ui->pushButton_12->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_12->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
    if (mapmode==4) ui->pushButton_11->setStyleSheet("background-color: lightGray; border-style: outset;border-width: 2px;border-color: black;"); else ui->pushButton_11->setStyleSheet("background-color: darkGray; border-style: outset;border-width: 2px;border-color: black;");
}

void MainWindow::Verify(){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "PaleoDataView",
                                                                tr("This will read and save any file in the currently active proxy-folder as well as any age model file connected to them. If your data files are from an earlier version of PaleoDataView, executing this tool will update the data to the current version of the software and prevent error messages from apearing that are connected to missing variables."),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {


    } else {
        qDebug()<<"Verify Database";
        QProgressDialog progress("Preparing Database...","Cancel",0,inv->get_Entries(),this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setCancelButton(0);
        progress.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        progress.setMaximumHeight(100);
        progress.setMaximumWidth(500);
        progress.setMinimumDuration(0);
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect  screenGeometry = screen->geometry();
        int height = screenGeometry.height();
        int width = screenGeometry.width();
        progress.setGeometry(width/2-250,height/2-50,500,100);


        progress.setValue(0);
        progress.show();
        progress.update();
        QApplication::processEvents();

        for (int i=0;i<inv->get_Entries();i++){
            inv->set_currentCore(i);
            inv->readData(i);
            amsdata->AMSRead();
            inv->saveData();
            amsdata->AMSSave();
            progress.setValue(i+1);
            QApplication::processEvents();
            qDebug()<<"Number :"+QString::number(i+1);
        }
        progress.close();
        setProxyType(Proxy_Type,0);

    }


}

void MainWindow::RefreshDatabase(){
    // save old inventory: Core, proxy, selected
    QStringList core;
    QStringList proxy;
    QStringList status;
    for(int i=0;i<inv->get_Entries();i++){
        core.push_back(inv->get_Core(i));
        proxy.push_back(inv->get_Species(i));
        status.push_back(QString::number(inv->get_Selected(i)));
    }
    // create new inventory
    setProxyType(Proxy_Type,1);
    // select all old entries
    for(int i=0;i<inv->get_Entries();i++){
        for (int j=0;j<core.length();j++){
            if (core.at(j)==inv->get_Core(i) && proxy.at(j)==inv->get_Species(i)) {
                inv->set_Selected(i,status.at(j).toInt());
                break;
            }
        }
    }
    inv->set_currentCore(-1);
    map->setSelected_X(-1);
    map->setSelected_Y(-1);
    inv->set_currentCore(-1);
    ui->textBrowser_3->setText("Select a Core");
    ui->textBrowser_4->setText("Select a Core");
    ui->textBrowser->setText("Select a Core");
    ui->lineEdit->clear();
    createTree();
    createMap(1);
    createEmptyPlots();
    update();
    inv->set_currentCore(-1);
}

void MainWindow::DeleteFile(){
    if(inv->get_currentCore()>-1&&inv->get_currentCore()<inv->get_Entries()){

        QMessageBox msgBox;
        msgBox.setText("Do you really want to delete this File? :"+inv->get_Core(inv->get_currentCore())+" / "+inv->get_Species(inv->get_currentCore()));
        msgBox.setInformativeText("The file will be permanently deleted from the harddisk.");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret==QMessageBox::No){

        } else {


            QString Filename=inv->get_Filename(inv->get_currentCore());
            QFile file (resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+Proxy_Type+"/Data/"+Filename);
            file.remove();

            // delete Agemodel
            if(inv->get_AgeModel(inv->get_currentCore())){
                QMessageBox msgBox;
                msgBox.setText("Also delete age model?");
                msgBox.setInformativeText("The age-file might be connected to other proxies.");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int ret = msgBox.exec();
                if (ret==QMessageBox::No){

                } else {
                    QString name=inv->get_Core(inv->get_currentCore());
                            name.replace('\0',' ');
                            name.replace(" ","");
                    QString QFilename=resources->path_age+name+".age";
                    //QFilename.replace(QString(".nc"),QString(".age"));
                    QFile file (QFilename);
                    file.remove();
                    // remove file from agemodel list
                    for (int i=0;i<inv->get_Entries();i++){
                        if (inv->get_Core(i)==inv->get_Core(inv->get_currentCore())) inv->set_AgeModel(i,0);
                    }
                }
            }
            inv->deleteEntry(inv->get_currentCore());

            if (inv->get_Entries())inv->save(); else {
                QFile file (resources->get_path_PaleoDataView()+"/"+resources->get_workdir()+"/"+Proxy_Type+"/inventory.nc");
                file.remove();
                inv->generate();
            }
            inv->set_currentCore(-1);
            map->setSelected_X(-1);
            map->setSelected_Y(-1);
            inv->set_currentCore(-1);
            ui->textBrowser_3->setText("Select a Core");
            ui->textBrowser_4->setText("Select a Core");
            ui->textBrowser->setText("Select a Core");
            ui->lineEdit->clear();
            createTree();
            createMap(1);
            createEmptyPlots();
            update();
            inv->set_currentCore(-1);
        }
    }else {
        QMessageBox msgBox;
                msgBox.setText("No data Selected");
                msgBox.setInformativeText("This function requires some proxy data to be selected. Use the map/list to select some data.");
                msgBox.setIcon(QMessageBox::Warning);

                msgBox.exec();
    }
}
