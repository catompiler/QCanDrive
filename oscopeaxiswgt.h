#ifndef OSCOPEAXISWGT_H
#define OSCOPEAXISWGT_H

#include <QWidget>

namespace Ui {
class OScopeAxisWgt;
}

class OScopeAxisWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OScopeAxisWgt(QWidget *parent = nullptr);
    ~OScopeAxisWgt();

private:
    Ui::OScopeAxisWgt *ui;
};

#endif // OSCOPEAXISWGT_H
