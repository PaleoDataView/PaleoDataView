#include "importstack.h"
#include "ui_importstack.h"

ImportStack::ImportStack(QMainWindow *mainWindow,Inventory *inventory,QString str) :
     mainW(mainWindow),inv2(inventory),proxy(str),
    ui(new Ui::ImportStack)
{
    ui->setupUi(this);
    this->setWindowTitle("Import Targets from Excel");
    sp=ui->splitter->saveState();

    nvar=0;
    col=0;
    row=0;
    modelData= new QStandardItemModel(0,0,this);
    //modelAtt = new QStandardItemModel(0,0,this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(browse()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(parse()));
}

ImportStack::~ImportStack()
{
    delete ui;
    delete modelData;
    //delete modelAtt;
}

void ImportStack::browse(){
    QString filename = QFileDialog::getOpenFileName(this, tr("Select File"),
                                             resources.path_PaleoDataView+"/Resources/Calibration/Target_Stacks.xlsx",
                                             tr("Excel (*.xlsx)"));
    //qDebug() << filename;
    ui->lineEdit->setText(filename);
    // read File and put to TextBrowser
    // get file name
    QString QFilename=ui->lineEdit->text();

    QString txt;
    txt.clear();

    QXlsx::Document xlsx(QFilename);

    CellRange cr=xlsx.dimension();
    int lastC=cr.lastColumn()+1;
    int lastR=cr.lastRow()+1;


        for (int i=1;i<lastR;i++){
            for (int j=1;j<lastC;j++){
                txt.append(xlsx.read(i,j).toString());
                if (j<lastC-1) txt.append("\t"); else txt.append("\n");
            }
        }

        ui->plainTextEdit->setPlainText(txt);// This also starts parsing

}



void ImportStack::parse(){
    QString txt=ui->plainTextEdit->toPlainText();
    QStringList line=txt.split("\n");
    QString dlm="\t";
    col=line.at(0).split(dlm).length();

    row=line.length();
    if (row>1){
        row--;

    }
    // init header
    for (int i=0;i<col;i++) header<<QString::number(i);
    for (int i=0;i<col;i++) var_child<<QString::number(i);
    // init data

    data.clear();
    for (int i=0;i<row;i++){
        if (line.at(i)!=""){
            QStringList fields=line.at(i).split(dlm);
            if (fields.length()>=col){
                if (i==0 ){
                    // Header
                    header.clear();
                    var_child.clear();
                    for (int j=0;j<col;j++){
                        header<<fields.at(j).simplified();
                        var_child<<fields.at(j).simplified();
                    }
                } else {
                    // Data
                    for (int j=0;j<col;j++){
                        data<<fields.at(j).simplified();
                    }
                }
            }else{
                for (int j=0;j<fields.length();j++){
                    data<<fields.at(j).simplified();
                }
                for (int j=fields.length();j<col;j++) data<<"NAN";
            }
        }
    }
    row=data.length()/col;
    if (row!=0){
        // estimate row length of Column
        nrow.clear();
        nrow.reserve(row);
        for (int i=0;i<col;i++){
            int count=0;
            for (int j=0;j<row;j++){
                if (data[j*col+i]!="") count++;
            }
            nrow[i]=count;
        }

        // default data type


        if (col>0&&row>0){
            float v=(float)(col)/(float)(4.0);
            int vv=(int)(col/4);
            if (v==vv) {

                qDebug()<<"This results in "+QString::number(vv)+" Variables";
                nvar=vv;
                while (var_parent.length()<nvar){
                    var_parent<<"Variable: "+QString::number(var_parent.length());
                }
                for (int i=0;i<var_parent.length();i++) var_parent.replace(i,header.at(i*4+2).simplified());
                createTree();
            } else {

                qDebug()<<"Not Possible!!!";
            }
        }


    }
}

void ImportStack::createTree(){
    //create tree for variable names
    ui->treeWidget->clear();
    QList<QTreeWidgetItem *> variables;
    QStandardItemModel *model1=new QStandardItemModel(0,1,this);

    model1->setHeaderData(0, Qt::Horizontal, "Found Target Data");


    ui->treeWidget->header()->setModel(model1);

    for (int i=0;i<nvar;i++){
        QStringList str;
        str<<var_parent.at(i);
        // length of variable
        int nrow_max=0;
        for (int j=0;j<4;j++){
            if (nrow[i*4+j]>nrow_max) nrow_max=nrow[i*4+j];
        }
        str<<QString::number(nrow_max);


        variables.append(new QTreeWidgetItem((QTreeWidget*)0,str));
        for (int j=0;j<4;j++){
            QString str="";
            str.append(var_child.at(i*4+j));
            QTreeWidgetItem *ch = new QTreeWidgetItem();
            ch->setText(0, str);


            ch->setText(2,header.at(i*4+j));
            variables.at(i)->addChild(ch);

        }
    }

    ui->treeWidget->insertTopLevelItems(0,variables);
    ui->treeWidget->collapseAll();

}

void ImportStack::save(){
    QString file = resources.path_PaleoDataView+"/Resources/Calibration/target_"+proxy+".nc";

    if (file!="" && row>0 && col>0){
        // get file name
        char* filename;
        string fname=file.toStdString();
        filename=new char[fname.size()+1];
        strcpy(filename,fname.c_str());
        qDebug() << "Save to :"+file;


        // Initialize ids and meta
        int ncid;
        int retval;// for Errors

        // Create the file
        if ((retval = nc_create(filename,NC_WRITE,&ncid)))
           ERR(retval);
        int varid_header[nvar];
        int varid_data[nvar];
        qDebug()<<"file open";
        // define variables
        for (int i=0;i<nvar;i++){
                qDebug()<<var_parent.at(i);
                // header
                int dim1=0;
                int dim2=0;
                // get maximum length of header
                int header_length=0;
                for (int j=0;j<4;j++) if (header_length<header.at(i*4+j).length()) header_length=header.at(i*4+j).length();
                QString h1="header_n_"+var_parent.at(i);
                if ((retval = nc_def_dim(ncid,h1.toLatin1().data(),4,&dim1)))
                    ERR(retval);
                QString h2="header_nchar_"+var_parent.at(i);
                if ((retval = nc_def_dim(ncid,h2.toLatin1().data(),header_length,&dim2)))
                    ERR(retval);
                int dimm[2];
                dimm[0]=dim1;
                dimm[1]=dim2;
                QString h="header_"+var_parent.at(i);
                if ((retval = nc_def_var(ncid,h.toLatin1().data(),NC_CHAR,2,dimm,&varid_header[i])))
                    ERR(retval);

            qDebug()<<"header ready :"+header.at(i*4+2);
            // Add dimensions for table
            // first get nrow for variable
            int nrow_max=0;
            for (int j=0;j<4;j++){
                if (nrow[i*4+j]>nrow_max) nrow_max=nrow[i*4+j];
            }

            int dimidrow=0;
            QString s1="rows_"+var_parent.at(i);
            if ((retval = nc_def_dim(ncid,s1.toLatin1().data(),nrow_max,&dimidrow)))
               ERR(retval);

            int dimidcol=0;
            QString s2="columns_"+var_parent.at(i);
            if ((retval = nc_def_dim(ncid,s2.toLatin1().data(),4,&dimidcol)))
               ERR(retval);

            int dim[2];
            dim[0]=dimidrow;
            dim[1]=dimidcol;
            if ((retval = nc_def_var(ncid,var_parent.at(i).toLatin1().data(),NC_DOUBLE,2,dim,&varid_data[i])))
                ERR(retval);

            qDebug()<<"child ready "+var_parent.at(i);
        }

        // end definition mode
        if ((retval = nc_enddef(ncid)))
            ERR(retval);
        qDebug()<<"defined variables";

        for (int i=0;i<nvar;i++){

                // create unsigned char field for headers
                int header_length=0;
                for (int j=0;j<4;j++) if (header_length<header.at(i*4+j).length()) header_length=header.at(i*4+j).length();
                unsigned char h[header_length][4];
                for (int ii=0;ii<header_length;ii++) for (int j=0;j<4;j++) h[ii][j]=' ';
                for (int j=0;j<4;j++) for (int k=0;k<header.at(i*4+j).length();k++) h[k][j]=header.at(i*4+j).toLatin1().at(k);

                if ((retval = nc_put_var(ncid,varid_header[i],&h[0][0])))
                    ERR(retval);





            // write data
            // turn StringList into arrays
            // length of variable
            int nrow_max=0;
            for (int j=0;j<4;j++){
                if (nrow[i*4+j]>nrow_max) nrow_max=nrow[i*4+j];
            }

            double d[4*nrow_max];

            for (int j=0;j<4;j++) {
                for (int k=0;k<nrow_max;k++) d[k*4+j]=data.at(k*col+(i*4+j)).toDouble();
            }
            if ((retval = nc_put_var(ncid,varid_data[i],&d[0])))
                ERR(retval);



        }
        qDebug()<<"data written";

        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
            ERR(retval);
        delete[] filename;
        qDebug()<<"file closed";
        close();
    }

}



bool ImportStack::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->plainTextEdit||obj==ui->lineEdit){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key()==Qt::Key_F1){
                ui->splitter->restoreState(sp);

                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

