#include "oscopechannelwgt.h"
#include "ui_oscopechannelwgt.h"
#include "oscopeplot.h"



OScopeChannelWgt::OScopeChannelWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeChannelWgt)
{
    m_plot = nullptr;
    m_channel = -1;

    ui->setupUi(this);

    ui->asVert->setScaleMin(1e-5);
    ui->asVert->setScaleMax(1e+4);
    ui->asVert->setScaleTurns(2);
    ui->asVert->setUnit(
        tr("В"),
        QStringList() << tr("к") << tr("М"),
        QStringList() << tr("м") << tr("мк")
        );
    connect(ui->asVert, &OScopeAxisWgt::scaleChanged, this, &OScopeChannelWgt::vertScaleChanged);
    connect(ui->asVert, &OScopeAxisWgt::offsetChanged, this, &OScopeChannelWgt::vertOffsetChanged);
}

OScopeChannelWgt::~OScopeChannelWgt()
{
    delete ui;
}

OScopePlot* OScopeChannelWgt::plot() const
{
    return m_plot;
}

void OScopeChannelWgt::setPlot(OScopePlot* newPlot)
{
    m_plot = newPlot;
}

int OScopeChannelWgt::channel() const
{
    return m_channel;
}

void OScopeChannelWgt::setChannel(int newChannel)
{
    m_channel = newChannel;
}

void OScopeChannelWgt::vertScaleChanged(qreal value)
{
    if(m_plot == nullptr || m_channel == -1) return;

    m_plot->setVDiv(m_channel, value);

    m_plot->replot();
}

void OScopeChannelWgt::vertOffsetChanged(qreal value)
{
    if(m_plot == nullptr || m_channel == -1) return;

    m_plot->setVOffset(m_channel, value);

    m_plot->replot();
}
