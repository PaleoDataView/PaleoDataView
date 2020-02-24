#ifndef BUSY_H
#define BUSY_H

#include <QDialog>

namespace Ui {
class Busy;
}

class Busy : public QDialog
{
    Q_OBJECT

public:
    explicit Busy(QWidget *parent = nullptr);
    ~Busy();

private:
    Ui::Busy *ui;
};

#endif // BUSY_H
