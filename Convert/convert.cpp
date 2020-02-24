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

#include "convert.h"
#include "ui_convert.h"

Convert::Convert(QMainWindow *mainWindow) :
    mainW(mainWindow),
    ui(new Ui::Convert)
{
    ui->setupUi(this);
    sp=ui->splitter->saveState();
    sp_2=ui->splitter_2->saveState();
    nvar=0;
    col=0;
    row=0;
    modelData= new QStandardItemModel(0,0,this);
    modelAtt = new QStandardItemModel(0,0,this);
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(optionsChanged()));
    connect(ui->checkBox_2,SIGNAL(clicked(bool)),this,SLOT(bundleChanged()));
    connect(ui->spinBox_2,SIGNAL(valueChanged(int)),this,SLOT(bundleChanged()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(parse()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(editNames()));

    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(addAtt()));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),this,SLOT(delAtt()));
    connect(ui->pushButton_7,SIGNAL(clicked(bool)),this,SLOT(editAtt()));
}

Convert::~Convert()
{
    delete ui;
    delete modelData;
    delete modelAtt;
}

void Convert::optionsChanged(){
    ui->plainTextEdit->setTabStopWidth(ui->spinBox->value());
    update();
}

void Convert::parse(){
    QString txt=ui->plainTextEdit->toPlainText();
    QStringList line=txt.split("\n");
    QString dlm="\t";
    col=line.at(0).split(dlm).length();
    ui->spinBox_2->setValue(col);
    ui->spinBox_2->setMaximum(col);
    row=line.length();
    if (ui->checkBox->isChecked() && row>1){
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
                if (i==0 && ui->checkBox->isChecked()){
                    // Header
                    header.clear();
                    var_child.clear();
                    for (int j=0;j<col;j++){
                        header<<fields.at(j);
                        var_child<<fields.at(j);
                    }
                } else {
                    // Data
                    for (int j=0;j<col;j++){
                        data<<fields.at(j);
                    }
                }
            }else{
                for (int j=0;j<fields.length();j++){
                    data<<fields.at(j);
                }
                for (int j=fields.length();j<col;j++) data<<"nan";
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
        //type.clear();
        //for (int i=0;i<col;i++) type<<"NC_DOUBLE";
        // put result to table
        delete modelData;
        modelData = new QStandardItemModel(row,col,this);
        ui->tableView->setModel(modelData);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


        for (int i=0;i<col;i++) modelData->setHorizontalHeaderItem(i, new QStandardItem(QString(header.at(i))));
        QStandardItem *dataItem = new QStandardItem[row*col];
        for (int i=0;i<row;i++){
            for (int j=0;j<col;j++){

                dataItem[i*col+j].setText(data.at(i*col+j));
                modelData->setItem(i,j,&dataItem[i*col+j]);
            }
        }
        ui->tableView->setSortingEnabled(0);
        ui->tableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
        ui->tableView->verticalHeader()->setDefaultSectionSize(ui->tableView->verticalHeader()->minimumSectionSize());
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setHorizontalScrollMode(ui->tableView->ScrollPerPixel);
        ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView->horizontalHeader()->setStretchLastSection(1);
        ui->tableView->repaint();
        bundleChanged();
    }
}

void Convert::save(){
    QString file = QFileDialog::getSaveFileName(this, tr("Select Save Filename"),
                                             "data",
                                             tr("netCDF (*.nc)"));

    if (file!="" && row>0 && col>0){
        // get file name
        char* filename;
        string fname=file.toStdString();
        filename=new char[fname.size()+1];
        strcpy(filename,fname.c_str());
        //qDebug() << "Save to :"+QFilename;


        // Initialize ids and meta
        int ncid;
        int retval;// for Errors

        // Create the file
        if ((retval = nc_create(filename,NC_WRITE,&ncid)))
           ERR(retval);
        int varid_header[nvar];
        int varid_data[nvar];
        // define variables
        for (int i=0;i<nvar;i++){
            if (ui->checkBox->isChecked()){
                // header
                int dim1=0;
                int dim2=0;
                // get maximum length of header
                int header_length=0;
                for (int j=0;j<ui->spinBox_2->value();j++) if (header_length<header.at(i*ui->spinBox_2->value()+j).length()) header_length=header.at(i*ui->spinBox_2->value()+j).length();
                QString h1="header_n_"+var_parent.at(i);
                if ((retval = nc_def_dim(ncid,h1.toLatin1().data(),ui->spinBox_2->value(),&dim1)))
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
            }

            // Add dimensions for table
            // first get nrow for variable
            int nrow_max=0;
            for (int j=0;j<ui->spinBox_2->value();j++){
                if (nrow[i*ui->spinBox_2->value()+j]>nrow_max) nrow_max=nrow[i*ui->spinBox_2->value()+j];
            }

            int dimidrow=0;
            QString s1="rows_"+var_parent.at(i);
            if ((retval = nc_def_dim(ncid,s1.toLatin1().data(),nrow_max,&dimidrow)))
               ERR(retval);

            int dimidcol=0;
            QString s2="columns_"+var_parent.at(i);
            if ((retval = nc_def_dim(ncid,s2.toLatin1().data(),ui->spinBox_2->value(),&dimidcol)))
               ERR(retval);

            int dim[2];
            dim[0]=dimidrow;
            dim[1]=dimidcol;
            if ((retval = nc_def_var(ncid,var_parent.at(i).toLatin1().data(),NC_DOUBLE,2,dim,&varid_data[i])))
                ERR(retval);


        }

        // end definition mode
        if ((retval = nc_enddef(ncid)))
            ERR(retval);

        for (int i=0;i<nvar;i++){
            if (ui->checkBox->isChecked()){
                // create unsigned char field for headers
                int header_length=0;
                for (int j=0;j<ui->spinBox_2->value();j++) if (header_length<header.at(i*ui->spinBox_2->value()+j).length()) header_length=header.at(i*ui->spinBox_2->value()+j).length();
                unsigned char h[header_length][ui->spinBox_2->value()];
                for (int ii=0;ii<header_length;ii++) for (int j=0;j<ui->spinBox_2->value();j++) h[ii][j]=' ';
                for (int j=0;j<ui->spinBox_2->value();j++) for (int k=0;k<header.at(i*ui->spinBox_2->value()+j).length();k++) h[k][j]=header.at(i*ui->spinBox_2->value()+j).toLatin1().at(k);

                if ((retval = nc_put_var(ncid,varid_header[i],&h[0][0])))
                    ERR(retval);

            }



            // write data
            // turn StringList into arrays
            // length of variable
            int nrow_max=0;
            for (int j=0;j<ui->spinBox_2->value();j++){
                if (nrow[i*ui->spinBox_2->value()+j]>nrow_max) nrow_max=nrow[i*ui->spinBox_2->value()+j];
            }

            double d[ui->spinBox_2->value()*nrow_max];

            for (int j=0;j<ui->spinBox_2->value();j++) {
                for (int k=0;k<nrow_max;k++) d[k*ui->spinBox_2->value()+j]=data.at(k*col+(i*ui->spinBox_2->value()+j)).toDouble();
            }
            if ((retval = nc_put_var(ncid,varid_data[i],&d[0])))
                ERR(retval);



        }

        // activate defMode for global attributes
        if ((retval = nc_redef(ncid)))
            ERR(retval);
        for (int i=0;i<att_name.size();i++){
            // save attributes
            if (att_type.at(i)=="NC_CHAR") {
                if ((retval = nc_put_att(ncid,NC_GLOBAL,att_name.at(i).toLatin1().data(),NC_CHAR,att_value.at(i).length(),att_value.at(i).toLatin1().data())))
                    ERR(retval);
            }
            if (att_type.at(i)=="NC_DOUBLE" || att_type.at(i)=="NC_FLOAT"){
                double val=att_value.at(i).toDouble();
                if ((retval = nc_put_att_double(ncid,NC_GLOBAL,att_name.at(i).toLatin1().data(),NC_DOUBLE,1,&val)))
                    ERR(retval);
            }
            if (att_type.at(i)=="NC_INT"){
                int val=att_value.at(i).toInt();
                if ((retval = nc_put_att_int(ncid,NC_GLOBAL,att_name.at(i).toLatin1().data(),NC_INT,1,&val)))
                    ERR(retval);
            }
        }
        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid)))
            ERR(retval);
        delete[] filename;
    }

}

void Convert::bundleChanged(){
    if (col>0&&row>0){
        float v=(float)(col)/(float)(ui->spinBox_2->value());
        int vv=(int)(col/ui->spinBox_2->value());
        if (v==vv) {
            ui->checkBox_2->setCheckState(Qt::Checked);
            ui->label_3->setText("This results in "+QString::number(vv)+" Variables");
            nvar=vv;
            while (var_parent.length()<nvar){
                var_parent<<"Variable: "+QString::number(var_parent.length());
            }
            createTree();
        } else {
            ui->checkBox_2->setCheckState(Qt::Unchecked);
            ui->label_3->setText("Not Possible");
        }
    }
}

void Convert::createTree(){
    //create tree for variable names
    ui->treeWidget->clear();
    QList<QTreeWidgetItem *> variables;
    for (int i=0;i<nvar;i++){
        QStringList str;
        str<<var_parent.at(i);
        // length of variable
        int nrow_max=0;
        for (int j=0;j<ui->spinBox_2->value();j++){
            if (nrow[i*ui->spinBox_2->value()+j]>nrow_max) nrow_max=nrow[i*ui->spinBox_2->value()+j];
        }
        str<<QString::number(nrow_max);


        variables.append(new QTreeWidgetItem((QTreeWidget*)0,str));
        for (int j=0;j<ui->spinBox_2->value();j++){
            QString str="";
            str.append(var_child.at(i*ui->spinBox_2->value()+j));
            QTreeWidgetItem *ch = new QTreeWidgetItem();
            ch->setText(0, str);
            // Add Type of each row

            ch->setText(2,header.at(i*ui->spinBox_2->value()+j));
            variables.at(i)->addChild(ch);
        }
    }
    ui->treeWidget->insertTopLevelItems(0,variables);
    ui->treeWidget->expandAll();
}

void Convert::editNames(){
    int r_parent=ui->treeWidget->currentIndex().parent().row();
    int r_child=ui->treeWidget->currentIndex().row();
    if (r_parent==-1) {//clicked top level item
        r_parent=ui->treeWidget->currentIndex().row();
        r_child=-1;
    }
    //qDebug()<<"Parent: "+QString::number(r_parent)+" Child: "+QString::number(r_child);
    if (r_child==-1 && r_parent!=-1){
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("Variable Name(parent):"), QLineEdit::Normal,
                                             var_parent.at(r_parent), &ok);
    if (ok && !text.isEmpty()) var_parent.replace(r_parent,text);
    }
    if (r_child!=-1 && r_parent!=-1){
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("Variable Name(child):"), QLineEdit::Normal,
                                             var_child.at(r_parent*ui->spinBox_2->value()+r_child), &ok);
    if (ok && !text.isEmpty()) var_child.replace(r_parent*ui->spinBox_2->value()+r_child,text);
    }
    createTree();
}


void Convert::addAtt(){
    att_name.push_back("Attribute ("+QString::number(att_name.size())+"):");
    att_value.push_back("0.0");
    att_type.push_back("NC_DOUBLE");
    createAttTable();
}

void Convert::delAtt(){
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    int r=-1;
    int c=-1;
    for (int i=0;i<3;i++){
        for (int j=0;j<att_name.size();j++){
            if (ui->tableView_2->selectionModel()->isSelected(model->indexFromItem(model->item(j,i)))) {
                c=i;
                r=j;
            }
        }
    }
    //qDebug()<<QString::number(r)+":"+QString::number(c);
    att_name.removeAt(r);
    att_value.removeAt(r);
    att_type.removeAt(r);
    createAttTable();
}

void Convert::editAtt(){
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    int r=-1;
    int c=-1;
    for (int i=0;i<3;i++){
        for (int j=0;j<att_name.size();j++){
            if (ui->tableView_2->selectionModel()->isSelected(model->indexFromItem(model->item(j,i)))) {
                c=i;
                r=j;
            }
        }
    }
    //qDebug()<<QString::number(r)+":"+QString::number(c);
    // edit name
    if (c==0){
        bool ok;
        QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                                 tr("Change attribute name:"), QLineEdit::Normal,
                                                 att_name.at(r), &ok);
        if (ok && !text.isEmpty()) att_name.replace(r,text);
    }
    // edit type
    if (c==2){
        QStringList items;
        items << tr("NC_DOUBLE") << tr("NC_FLOAT") << tr("NC_INT") << tr("NC_CHAR");

        bool ok;
        QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                             tr("Attribute Type:"), items, 0, false, &ok);
        if (ok && !item.isEmpty()) {
            att_type[r]=item;
        }
    }
    // edit value
    if (c==1){
        bool ok;
        if (att_type.at(r)=="NC_DOUBLE" || att_type.at(r)=="NC_FLOAT"){

            double d = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
                                               tr("Value :"), att_value.at(r).toDouble(), -99999999999999e999, 999999999999999e999, 3, &ok);
            if (ok)  att_value.replace(r,QString::number(d));
        }

        if (att_type.at(r)=="NC_INT" ){

            int i = QInputDialog::getInt(this, tr("QInputDialog::getInteger()"),
                                               tr("Value :"), att_value.at(r).toInt(), -99999999999999e999, 999999999999999e999,1, &ok);
            if (ok)  att_value.replace(r,QString::number(i));
        }
        if (att_type.at(r)=="NC_CHAR"){
            QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                                     tr("Value :"), QLineEdit::Normal,
                                                     att_value.at(r), &ok);
            if (ok && !text.isEmpty()) att_value.replace(r,text);
        }

    }
    createAttTable();
}

void Convert::createAttTable(){
    // put att to table
    int len=att_name.size();
    delete modelAtt;
    modelAtt = new QStandardItemModel(len,3,this);
    ui->tableView_2->setModel(modelAtt);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    modelAtt->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    modelAtt->setHorizontalHeaderItem(1, new QStandardItem("Value"));
    modelAtt->setHorizontalHeaderItem(2, new QStandardItem("Type"));

    QStandardItem *dataItem = new QStandardItem[3*len];
    for (int i=0;i<len;i++){
        dataItem[i*3+0].setText(att_name.at(i));
        modelAtt->setItem(i,0,&dataItem[i*3+0]);
        dataItem[i*3+1].setText(att_value.at(i));
        modelAtt->setItem(i,1,&dataItem[i*3+1]);
        dataItem[i*3+2].setText(att_type.at(i));
        modelAtt->setItem(i,2,&dataItem[i*3+2]);
    }
    ui->tableView_2->setSortingEnabled(0);
    ui->tableView_2->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(ui->tableView_2->verticalHeader()->minimumSectionSize());
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setHorizontalScrollMode(ui->tableView_2->ScrollPerPixel);
    ui->tableView_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(1);
    ui->tableView_2->repaint();
}

bool Convert::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if (obj==ui->plainTextEdit||obj==ui->lineEdit||obj==ui->spinBox||obj==ui->tableView){
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

