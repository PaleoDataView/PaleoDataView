#include "calib.h"
#include "ui_calib.h"

Calib::Calib(QMainWindow *parent,Inventory *inventory,QString str) :
    mainW(parent),inv(inventory),proxy(str),
    ui(new Ui::Calib)
{
    ui->setupUi(this);
     this->setWindowTitle("Calibrate");
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    qApp->installEventFilter(this);

    resources=new Resources();
    inv->readData(inv->get_currentCore());

    // Init Plot Variables
    data1=new float[0];
    com1=new QString[0];
    use1=new bool[0];
    pl1=new Graph(this,data1,0,0);
    pl1->comHide(0);

    data2=new float[0];
    com2=new QString[0];
    use2=new bool[0];
    pl2=new Graph(this,data2,0,0);
    pl2->comHide(0);
    // estimate source and target
    for (int i=0;i<inv->access_data_length();i++) {
        if (inv->access_data_type(i).contains("SST=Target")) target=i;
        if (inv->access_data_type(i).contains("SST=Source")) source=i;
    }
    // prepare dropdown menu and parameters
    if (proxy=="Foraminiferal MgCa"){
        ui->comboBox->clear();
        ui->comboBox->addItem("(ln(MgCa/a))/b");
        ui->doubleSpinBox->setValue(0.381);
        ui->doubleSpinBox_2->setValue(0.09);
    }
    if (proxy=="Alkenones"){
        ui->comboBox->clear();
        ui->comboBox->addItem("(UK-a)/b");
        ui->doubleSpinBox->setValue(0.044);
        ui->doubleSpinBox_2->setValue(0.033);
    }

    createPlot();
    createSSTPlot();
    setupTable();
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setPlainText(inv->get_att_Comment());
    update();
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(apply()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(pl1,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
    connect(pl2,SIGNAL(graphChanged()),this,SLOT(updateGraph()));
}

Calib::~Calib()
{
    delete ui;
}

void Calib::paintEvent(QPaintEvent *)
{
    pl1->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    pl2->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());

}

void Calib::createPlot(){

    // get length
    int length=inv->get_Length();
    // fill first plot with data
    delete[] data1;
    data1=new float[length*2];
    for (int i=0;i<length*2;i++) data1[i]=NAN;
    delete[] use1;
    use1=new bool[length*2];
    for (int i=0;i<length*2;i++) use1[i]=0;
    delete[] com1;
    com1=new QString[length*2];
    for (int i=0;i<length*2;i++) com1[i]="";


    float low=INFINITY;
    float high=-INFINITY;
    for (int i=0;i<inv->get_Length();i++){
        data1[i+0*length]=inv->get_data_Depth(i);
        data1[i+1*length]=inv->access_data_value(source,i).toFloat();
        if (low>inv->access_data_value(source,i).toFloat()) low=inv->access_data_value(source,i).toFloat();
        if (high<inv->access_data_value(source,i).toFloat()) high=inv->access_data_value(source,i).toFloat();
        use1[i+0*length]=inv->get_data_Use_Flag(i);
        use1[i+1*length]=inv->get_data_Use_Flag(i);
        if (inv->get_data_Comment(i).simplified()!="NaN")com1[i+1*length]=inv->get_data_Comment(i);
    }

    pl1->setData(data1,2,length);
    pl1->setUse(use1,1);
    QString str=inv->access_data_name(source);
    if (inv->access_data_unit(source)!="") str.append(" ["+inv->access_data_unit(source)+"]");
    pl1->setTitel("",inv->access_data_name(0)+" ["+inv->access_data_unit(0)+"]",str);
    pl1->setMultiplicator(1,1);
    pl1->setTextSize(resources->get_Pointsize(),1,0);
    pl1->setSize(ui->graphicsView->width(),ui->graphicsView->height());
    pl1->setComment(com1,2);
    pl1->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Calib_plot_"+inv->access_proxy_abbreviation()+".txt");
    pl1->autoSize();
    ui->graphicsView->setScene(pl1);
    ui->graphicsView->repaint();
}


void Calib::createSSTPlot(){
    // get length
    int length=inv->get_Length();
    // fill first plot with data
    delete[] data2;
    data2=new float[length*2];
    for (int i=0;i<length*2;i++) data2[i]=NAN;
    delete[] use2;
    use2=new bool[length*2];
    for (int i=0;i<length*2;i++) use2[i]=0;
    delete[] com2;
    com2=new QString[length*2];
    for (int i=0;i<length*2;i++) com2[i]="";

    float low=INFINITY;
    float high=-INFINITY;
    for (int i=0;i<length;i++){
        data2[i+0*length]=inv->get_data_Depth(i);
        data2[i+1*length]=inv->access_data_value(target,i).toFloat();
        use2[i+0*length]=inv->get_data_Use_Flag(i);
        use2[i+1*length]=inv->get_data_Use_Flag(i);
        if (low>inv->access_data_value(source,i).toFloat()) low=inv->access_data_value(source,i).toFloat();
        if (high<inv->access_data_value(source,i).toFloat()) high=inv->access_data_value(source,i).toFloat();
        if (inv->get_data_Comment(i).simplified()!="NaN")com2[i+1*length]=inv->get_data_Comment(i);

    }

    pl2->setData(data2,2,length);
    pl2->setUse(use2,1);
    pl2->setTitel("",inv->access_data_name(0)+" ["+inv->access_data_unit(0)+"]",inv->access_data_name(target)+" ["+inv->access_data_unit(target)+"]");
    pl2->setMultiplicator(1,1);
    pl2->setTextSize(resources->get_Pointsize(),1,0);
    pl2->setSize(ui->graphicsView_2->width(),ui->graphicsView_2->height());



    pl2->setComment(com2,2);

    pl2->setSettings(resources->path_PaleoDataView+"/Resources/Plot/Calib_SST_"+inv->access_proxy_abbreviation()+".txt");
    pl2->autoSize();
    ui->graphicsView_2->setScene(pl2);
    ui->graphicsView_2->repaint();
}

void Calib::setupTable(){
    // create the model for Isoptopes
    //delete modelIsotope;
    modelIsotope = new QStandardItemModel(inv->get_Length(),inv->access_data_length(),this);


    for (int i=0;i<inv->access_data_length();i++) modelIsotope->setHorizontalHeaderItem(i, new QStandardItem(inv->access_data_name(i)+"\n"+inv->access_data_unit(i)));


    //ui->tableView_5->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (int i=0;i<inv->get_Length();i++){
        for (int j=0;j<inv->access_data_length();j++){

            if (inv->access_data_type(j).contains("flag")){
                QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                //var->setCheckable(true);
                if (inv->access_data_value(j,i).contains("1")){
                    var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    //var->setCheckState(Qt::Checked);
                    //var->setFlags(var->flags() & ~Qt::ItemIsEditable);

                    modelIsotope->setItem(i,j,var);
                    modelIsotope->setData(modelIsotope->index(i, j), Qt::AlignCenter,Qt::TextAlignmentRole);
                    modelIsotope->setData(modelIsotope->index(i, j), QColor(Qt::green), Qt::BackgroundRole);

                }else{
                    var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    //var->setCheckState(Qt::Unchecked);
                    //var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    modelIsotope->setItem(i,j,var);
                    modelIsotope->setData(modelIsotope->index(i, j), Qt::AlignCenter,Qt::TextAlignmentRole);
                    modelIsotope->setData(modelIsotope->index(i, j), QColor(Qt::red), Qt::BackgroundRole);
                }


            } else {
                if (inv->access_data_type(j).contains("string")){
                    QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                    var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    modelIsotope->setItem(i,j,var);

                }
                if (inv->access_data_type(j).contains("float") || inv->access_data_type(j).contains("int")){
                    QStandardItem *var=new QStandardItem(inv->access_data_value(j,i));
                    var->setData(inv->access_data_value(j,i).toFloat(),Qt::EditRole);
                    var->setFlags(var->flags() & ~Qt::ItemIsEditable);
                    modelIsotope->setItem(i,j,var);

                }
            }
        }
    }
    QFont font;
    font.setPointSize(resources->get_Textsize());
    ui->tableView->setFont(font);
    ui->tableView->horizontalHeader()->setFont(font);
    ui->tableView->verticalHeader()->setFont(font);
    ui->tableView->setModel(modelIsotope);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
    ui->tableView->horizontalHeader()->setStretchLastSection(1);
    ui->tableView->repaint();
}

bool Calib::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->tableView||obj==ui->graphicsView||obj==ui->graphicsView_2){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);
                ui->splitter_2->restoreState(sp_2);

                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void Calib::apply(){
    if (proxy=="Foraminiferal MgCa"){
        // erase old calibration
        QString str=inv->get_att_Comment();
        QStringList str2=str.split(";");
        QString nstr="";
        for (int i=0;i<str2.length();i++) {
            if (!str2[i].contains("SST calibrated by using log(MgCa(i)/")) {
                if (nstr!="") nstr.append(";");
                nstr.append(str2[i]);
            }
        }
        inv->set_att_Comment(nstr);

        if (inv->get_att_Comment()=="" || inv->get_att_Comment()=="NAN" || inv->get_att_Comment()=="NA" || inv->get_att_Comment()=="NaN" || inv->get_att_Comment()=="Na" || inv->get_att_Comment()=="na" || inv->get_att_Comment()=="nan") {
            inv->set_att_Comment("SST calibrated by using log(MgCa(i)/"+QString::number(ui->doubleSpinBox->value())+")/"+QString::number(ui->doubleSpinBox_2->value()));
        } else {
            inv->set_att_Comment(inv->get_att_Comment()+"; SST calibrated by using log(MgCa(i)/"+QString::number(ui->doubleSpinBox->value())+")/"+QString::number(ui->doubleSpinBox_2->value()));
        }
        ui->plainTextEdit->clear();
        ui->plainTextEdit->setPlainText(inv->get_att_Comment());
        update();
        if (ui->comboBox->currentIndex()==0){
            for (int i=0;i<inv->get_Length();i++){
                inv->set_data_SST(log(inv->get_data_MgCa(i)/ui->doubleSpinBox->value())/ui->doubleSpinBox_2->value(),i);
            }
        }
        createSSTPlot();
        setupTable();
        update();
    }
    if (proxy=="Alkenones"){
        // erase old calibration
        QString str=inv->get_att_Comment();
        QStringList str2=str.split(";");
        QString nstr="";
        for (int i=0;i<str2.length();i++) {
            if (!str2[i].contains("SST calibrated by using (UK-")) {
                if (nstr!="") nstr.append(";");
                nstr.append(str2[i]);
            }
        }
        inv->set_att_Comment(nstr);

        if (inv->get_att_Comment()=="" || inv->get_att_Comment()=="NAN" || inv->get_att_Comment()=="NA" || inv->get_att_Comment()=="NaN" || inv->get_att_Comment()=="Na" || inv->get_att_Comment()=="na" || inv->get_att_Comment()=="nan") {
            inv->set_att_Comment("SST calibrated by using (UK-"+QString::number(ui->doubleSpinBox->value())+")/"+QString::number(ui->doubleSpinBox_2->value()));
        } else {
            inv->set_att_Comment(inv->get_att_Comment()+"; SST calibrated by using (UK-"+QString::number(ui->doubleSpinBox->value())+")/"+QString::number(ui->doubleSpinBox_2->value()));
        }
        ui->plainTextEdit->clear();
        ui->plainTextEdit->setPlainText(inv->get_att_Comment());
        update();
        if (ui->comboBox->currentIndex()==0){
            for (int i=0;i<inv->get_Length();i++){
                inv->set_data_SST((inv->get_data_UK37(i)-ui->doubleSpinBox->value())/ui->doubleSpinBox_2->value(),i);
            }
        }
        createSSTPlot();
        setupTable();
        update();
    }

}

void Calib::save(){
        inv->set_att_Comment(ui->plainTextEdit->toPlainText());

        inv->saveData();
        QMessageBox msgBox;
                msgBox.setText("SST succesfully saved.");

                msgBox.exec();






}

void Calib::updateGraph(){

    createPlot();
    createSSTPlot();
}

