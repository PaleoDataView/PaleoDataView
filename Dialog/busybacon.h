#ifndef BUSYBACON_H
#define BUSYBACON_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class BusyBacon;
}

class BusyBacon : public QDialog
{
    Q_OBJECT

public:
    explicit BusyBacon(QWidget *parent = nullptr);
    ~BusyBacon();
signals:
    void abortBacon();

private slots:
    void abort();

private:
    Ui::BusyBacon *ui;
};

#endif // BUSYBACON_H
