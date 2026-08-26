#ifndef OSCOPECURSORSWGT_H
#define OSCOPECURSORSWGT_H

#include <QWidget>

namespace Ui {
class OScopeCursorsWgt;
}

class OScopeCursorsWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OScopeCursorsWgt(QWidget *parent = nullptr);
    ~OScopeCursorsWgt();

    bool floatingEnabled() const;
    void setFloatingEnabled(bool newEnabled);

signals:
    void floatingEnabledChanged(bool newEnabled);

private slots:
    void floatingEnabled_stateChanged(Qt::CheckState newState);

private:
    Ui::OScopeCursorsWgt *ui;
};

#endif // OSCOPECURSORSWGT_H
