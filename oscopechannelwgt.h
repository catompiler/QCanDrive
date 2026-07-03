#ifndef OSCOPECHANNELWGT_H
#define OSCOPECHANNELWGT_H

#include <QWidget>

namespace Ui {
class OScopeChannelWgt;
}

class OScopeChannelWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OScopeChannelWgt(QWidget *parent = nullptr);
    ~OScopeChannelWgt();

private:
    Ui::OScopeChannelWgt *ui;
};

#endif // OSCOPECHANNELWGT_H
