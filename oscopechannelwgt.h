#ifndef OSCOPECHANNELWGT_H
#define OSCOPECHANNELWGT_H

#include <QWidget>


class OScopePlot;

namespace Ui {
class OScopeChannelWgt;
}

class OScopeChannelWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OScopeChannelWgt(QWidget *parent = nullptr);
    ~OScopeChannelWgt();

    OScopePlot* plot() const;
    void setPlot(OScopePlot* newPlot);

    int channel() const;
    void setChannel(int newChannel);

private slots:
    void vertScaleChanged(qreal value);
    void vertOffsetChanged(qreal value);

private:
    Ui::OScopeChannelWgt *ui;

    OScopePlot* m_plot;
    int m_channel;
};

#endif // OSCOPECHANNELWGT_H
