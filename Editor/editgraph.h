#ifndef EDITGRAPH_H
#define EDITGRAPH_H

#include <QDialog>
#include "editgraph_graph.h"
#include "Editor/graphdata.h"
#include "Editor/graphgrid.h"
#include "Editor/graphobject.h"
#include <QGraphicsScene>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>
#include <QClipboard>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "General/resources.h"
namespace Ui {
class EditGraph;
}

class EditGraph : public QDialog
{
    Q_OBJECT

public:
    explicit EditGraph(QWidget *parent = nullptr);
    ~EditGraph();
    void paintEvent(QPaintEvent *);
    void closeEvent (QCloseEvent *event);

    //****************************************************************
    // All the functions of a graph object accesible from this editor
    //****************************************************************



    // Data
    void setData(float *d,int c,int r,int n);
    void setComment(QString *s,int m,int n);
    void setLabel(QString *s,int m,int n);
    void setColor(QColor *c,bool m,int n);
    void setUse(bool *u,int m,int n);
    void setError(float *d_e,int m,int n);
    void setError2(float *d_e,int m,int n);
    void setMark(int *d_m,int m,int n);

    // Global
    void setTitel(QString t,QString tx,QString ty,int n);
    void setTextSize(int texts, int titels,int margins,int n);
    void setMultiplicator(double fx, double fy,int n);
    void setTicks(int tx,int ty,int n);
    void setLineWidth(int i,int n);
    void setLineStyle(Qt::PenStyle sty,int n);
    void setLineColor(QColor c,int n);
    void setSymbol(int size,int n);
    void addMarker(float x,float y,int m,int n);
    void fixRange(bool x,bool y,int n);
    void setAxisType(int x,int y,int n);
    void setComHide(int i, int n);
    void setLockAspect(int mode,int n);
    void setFolding(int x,int y,int n);
    void setBackground(QString filename,float x1,float x2,float y1,float y2,int mode,int n);
    void setBackground(QImage *i,float x1,float x2,float y1,float y2,int mode,int n);
    void setBackground2(QImage *i,float x1,float x2,float y1,float y2,int mode,int n);
    void setRepeat(int x,float x1,float x2,int y,float y1, float y2,int n);
    void setLimit(float x1, float x2, float y1, float y2,int n);
    void setColorLegend(int type,QString t,float min,float max,int i);
    void setColorScheme(int n,int i);

    // Local
    void setSetColor(QColor *c,int m,int n);
    void setSetSymbolsize(int *s,int m,int n);
    void setSetSymboltype(int *t,int m,int n);

    void setSetLineColor(QColor *c,int m,int n);
    void setSetLineWidth(int *s,int m,int n);
    void setSetLineStyle(Qt::PenStyle *s, int m,int n);

    void setSetPoly(int *s,int m,int n);

    void setSetArea(float* area_x,float* area_y,int area_mode,int n);

    void setAutosize(int* as,int mode,int n);

    // Objects
    void addLine(float x1,float y1,int n1,float x2,float y2,int n2,QColor c,int w,Qt::PenStyle s);
    void setAddMode(int i);
    void addLineClear();

    // Other
    void setNumber(int n);
    void autoSize(int n);
    void deleteAll();
    void setSettings(QString p,int n);







    //**************************************************************************************************

    void setupDataTab();
    void setupGlobalTab();
    void setupLocalTab();
    void setupObjectTab();
    void setupOptionsTab();
    void updateAllTabs();

private slots:
    void updateData();
    void updateAll();
    void editData(QModelIndex r,QModelIndex c);
    void editSymbolsize(QModelIndex r,QModelIndex c);
    void editSymboltype(QModelIndex r,QModelIndex c);
    void editSymbolcolor(QModelIndex r,QModelIndex c);
    void editLinewidth(QModelIndex r,QModelIndex c);
    void editLinestyle(QModelIndex r,QModelIndex c);
    void editLinecolor(QModelIndex r,QModelIndex c);
    void editPoly(QModelIndex r,QModelIndex c);
    void editArea(QModelIndex r,QModelIndex c);
    void editAutosize(QModelIndex r,QModelIndex c);
    void editLines(QModelIndex r,QModelIndex c);
    void copy();
    void applyGlobal();
    void applyLocal();
    void applyObjects();
    void changeColor();
    void saveSettings();
    void deleteSettings();
    void readSettings();
    void saveImage();

signals:
    void graphChangedEdit();

private:
    Ui::EditGraph *ui;
    EditGraph_Graph *graph;

// Variables that define plots
    int number=1;
    std::vector<double> x_min;
    std::vector<double> y_min;
    std::vector<double> y_max;
    std::vector<double> x_max;

    std::vector<QString*> titel;
    std::vector<QString*> titel_x;
    std::vector<QString*> titel_y;

    std::vector<QString*> path;

    std::vector<int> textsize;
    std::vector<int> titelsize;
    std::vector<int> margin;

    std::vector<double> factor_x;
    std::vector<double> factor_y;

    std::vector<bool> fix_x;
    std::vector<bool> fix_y;

    std::vector<float*> data;
    std::vector<int> column;
    std::vector<int> row;

    std::vector<bool*> useflag;
    std::vector<int> usemode;

    std::vector<QString*> dat_com;
    std::vector<int> com_mode;
    std::vector<int> com_hide;

    std::vector<QString*> dat_label;
    std::vector<int> label_mode;

    std::vector<QColor*> col_dat;
    std::vector<bool> color;

    std::vector<float*> data_err;
    std::vector<float*> data_err2;
    std::vector<int> error;
    std::vector<int> error2;

    std::vector<int*> data_mark;
    std::vector<int> marker;

    std::vector<int> symbol_size;
    std::vector<int> line_width;
    std::vector<Qt::PenStyle> line_style;
    std::vector<QColor> line_color;

    std::vector<float> marker_x;
    std::vector<float> marker_y;
    std::vector<int> marker_mode;

    std::vector<int> setcolor_mode;
    std::vector<int> setlinecolor_mode;
    std::vector<int> setlinewidth_mode;
    std::vector<int> setsymbolsize_mode;
    std::vector<int> setsymboltype_mode;
    std::vector<int> setlinestyle_mode;
    std::vector<QColor*> setcolor_color;
    std::vector<QColor*> setlinecolor_color;
    std::vector<int*> setsymbolsize;
    std::vector<int*> setsymboltype;
    std::vector<int*> setlinewidth;
    std::vector<Qt::PenStyle*> setlinestyle;

    std::vector<int> setpoly_mode;
    std::vector<int*> setpoly;
    std::vector<int> setarea_mode;
    std::vector<float*> setarea_x;
    std::vector<float*> setarea_y;

    std::vector<int> x_axis_type;
    std::vector<int> y_axis_type;

    std::vector<int> ticks_x;
    std::vector<int> ticks_y;

    std::vector<QString*> background_filename;
    std::vector<QImage*> background_fileimage;
    std::vector<QImage*> background_image;
    std::vector<float> background_x1;
    std::vector<float> background_x2;
    std::vector<float> background_y1;
    std::vector<float> background_y2;
    std::vector<int> background;

    std::vector<int> repeat_x;
    std::vector<float> repeat_x1;
    std::vector<float> repeat_x2;
    std::vector<int> repeat_y;
    std::vector<float> repeat_y1;
    std::vector<float> repeat_y2;

    std::vector<int> fold_x;
    std::vector<int> fold_y;

    std::vector<float> limit_x1;
    std::vector<float> limit_x2;
    std::vector<float> limit_y1;
    std::vector<float> limit_y2;
    std::vector<int> lock_aspect;

    std::vector<int> colorLegend_type;
    std::vector<int> colorLegend_size_x;
    std::vector<int> colorLegend_size_y;
    std::vector<QString*> colorLegend_titel;
    std::vector<float> colorLegend_min;
    std::vector<float> colorLegend_max;

    std::vector<int> colorScheme;

    std::vector<int*> autosize;
    std::vector<int> autosize_mode;

    int object_mode;
    std::vector<float> object_line_x1;
    std::vector<float> object_line_y1;
    std::vector<int> object_line_n1;
    std::vector<float> object_line_x2;
    std::vector<float> object_line_y2;
    std::vector<int> object_line_n2;
    std::vector<QColor> object_line_color;
    std::vector<int> object_line_width;
    std::vector<Qt::PenStyle> object_line_style;







    QStandardItemModel *modelData;
    QStandardItemModel *modelSymbolsize;
    QStandardItemModel *modelSymboltype;
    QStandardItemModel *modelSymbolcolor;
    QStandardItemModel *modelLinewidth;
    QStandardItemModel *modelLinestyle;
    QStandardItemModel *modelLinecolor;
    QStandardItemModel *modelPoly;
    QStandardItemModel *modelArea;
    QStandardItemModel *modelAutosize;
    QStandardItemModel *modelObject;

    Resources *resources;
};

#endif // EDITGRAPH_H
