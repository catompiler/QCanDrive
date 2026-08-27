#include "oscopechannelwgt.h"
#include "ui_oscopechannelwgt.h"
#include "oscopeplot.h"
#include "voltageformatter.h"
#include <QColorDialog>
#include <QPen>
#include <QDebug>


OScopeChannelWgt::OScopeChannelWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeChannelWgt)
{
    m_plot = nullptr;
    m_channel = -1;

    ui->setupUi(this);

    ui->asVert->setScaleAdjustType(OScopeAxisWgt::ADJUST_TO_HIGHEST);
    ui->asVert->setScaleMin(1e-5);
    ui->asVert->setScaleMax(1e+4);
    ui->asVert->setScaleTurns(2);
    ui->asVert->setUnitFormatter(new VoltageFormatter());

    populatePenStyles();

    connect(ui->asVert, &OScopeAxisWgt::scaleChanged, this, &OScopeChannelWgt::vertScaleChanged);
    connect(ui->asVert, &OScopeAxisWgt::offsetChanged, this, &OScopeChannelWgt::vertOffsetChanged);

    connect(ui->cbVisible, &QCheckBox::checkStateChanged, this, &OScopeChannelWgt::visiblityCheckStateChanged);
    connect(ui->tbPenColorSel, &QToolButton::clicked, this, &OScopeChannelWgt::penColorSel_clicked);
    connect(ui->cbPenStyle, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OScopeChannelWgt::penStyle_currentIndexChanged);
    connect(ui->sbLineWidth, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &OScopeChannelWgt::lineWidth_valueChanged);
    connect(ui->leName, &QLineEdit::textChanged, this, &OScopeChannelWgt::nameTextChanged);
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

qreal OScopeChannelWgt::vDiv() const
{
    return ui->asVert->scale();
}

void OScopeChannelWgt::setVDiv(qreal newVDiv)
{
    ui->asVert->blockSignals(true);
    ui->asVert->setScale(newVDiv);
    ui->asVert->blockSignals(false);
}

qreal OScopeChannelWgt::vOffset() const
{
    return ui->asVert->offset();
}

void OScopeChannelWgt::setVOffset(qreal newVOffset)
{
    ui->asVert->blockSignals(true);
    ui->asVert->setOffset(newVOffset);
    ui->asVert->blockSignals(false);
}

bool OScopeChannelWgt::signalVisible() const
{
    return ui->cbVisible->isChecked();
}

void OScopeChannelWgt::setSignalVisible(bool newVisible)
{
    ui->cbVisible->blockSignals(true);
    ui->cbVisible->setChecked(newVisible);
    ui->cbVisible->blockSignals(false);
}

QString OScopeChannelWgt::signalName() const
{
    return ui->leName->text();
}

void OScopeChannelWgt::setSignalName(const QString& newName)
{
    ui->leName->setText(newName);
}

QColor OScopeChannelWgt::penColor() const
{
    const QPalette& pal = ui->frPenColor->palette();
    return pal.window().color();
}

void OScopeChannelWgt::setPenColor(const QColor& newPenColor)
{
    QPalette pal = ui->frPenColor->palette();
    pal.setColor(QPalette::Window, newPenColor);
    ui->frPenColor->setPalette(pal);
}

Qt::PenStyle OScopeChannelWgt::penStyle() const
{
    bool ok = false;
    auto res =  static_cast<Qt::PenStyle>(ui->cbPenStyle->currentData().toInt(&ok));
    if(ok) return res;
    return Qt::NoPen;
}

void OScopeChannelWgt::setPenStyle(Qt::PenStyle newPenStyle)
{
    ui->cbPenStyle->blockSignals(true);
    ui->cbPenStyle->setCurrentIndex(ui->cbPenStyle->findData(static_cast<int>(newPenStyle)));
    ui->cbPenStyle->blockSignals(false);
}

qreal OScopeChannelWgt::penWidth() const
{
    return ui->sbLineWidth->value();
}

void OScopeChannelWgt::setPenWidth(qreal newPenWidth)
{
    ui->sbLineWidth->blockSignals(true);
    ui->sbLineWidth->setValue(newPenWidth);
    ui->sbLineWidth->blockSignals(false);
}

void OScopeChannelWgt::updateValues()
{
    if(m_plot == nullptr || m_channel == -1) return;

    ui->cbVisible->setChecked(m_plot->signalVisible(m_channel));

    ui->leName->setText(m_plot->signalName(m_channel));

    QPen pen = m_plot->pen(m_channel);
    setPenColor(pen.color());
    setPenStyle(pen.style());
    setPenWidth(pen.widthF());

    ui->asVert->setScale(m_plot->vDiv(m_channel));
    ui->asVert->setOffset(m_plot->vOffset(m_channel));
}

void OScopeChannelWgt::applyValues() const
{
    if(m_plot == nullptr || m_channel == -1) return;

    m_plot->setSignalVisible(m_channel, ui->cbVisible->isChecked() && isEnabled());

    m_plot->setSignalName(m_channel, ui->leName->text());

    QPen pen = m_plot->pen(m_channel);
    pen.setColor(penColor());
    pen.setStyle(penStyle());
    pen.setWidthF(penWidth());
    m_plot->setPen(m_channel, pen);

    m_plot->setVDiv(m_channel, ui->asVert->scale());
    m_plot->setVOffset(m_channel, ui->asVert->offset());
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

void OScopeChannelWgt::visiblityCheckStateChanged(Qt::CheckState checkState)
{
    if(m_plot == nullptr || m_channel == -1) return;

    m_plot->setSignalVisible(m_channel, (checkState == Qt::Checked) && isEnabled());

    m_plot->replot();
}

void OScopeChannelWgt::nameTextChanged(const QString& newName)
{
    if(m_plot == nullptr || m_channel == -1) return;

    m_plot->setSignalName(m_channel, newName);
}

void OScopeChannelWgt::penColorSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    QPalette pal = ui->frPenColor->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета линии"), QColorDialog::ShowAlphaChannel);
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        ui->frPenColor->setPalette(pal);

        if(m_plot != nullptr && m_channel != -1){
            QPen pen = m_plot->pen(m_channel);
            pen.setColor(col);
            m_plot->setPen(m_channel, pen);

            m_plot->replot();
        }
    }
}

void OScopeChannelWgt::penStyle_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    if(m_plot != nullptr && m_channel != -1){
        QPen pen = m_plot->pen(m_channel);
        pen.setStyle(penStyle());
        m_plot->setPen(m_channel, pen);

        m_plot->replot();
    }
}

void OScopeChannelWgt::lineWidth_valueChanged(double value)
{
    if(m_plot != nullptr && m_channel != -1){
        QPen pen = m_plot->pen(m_channel);
        pen.setWidthF(value);
        m_plot->setPen(m_channel, pen);

        m_plot->replot();
    }
}

void OScopeChannelWgt::populatePenStyles()
{
    QList<QPair<QString, Qt::PenStyle>> styles;

    styles << qMakePair(tr("Нет"), Qt::NoPen)
           << qMakePair(tr("————"), Qt::SolidLine)
           << qMakePair(tr("- - - - - -"), Qt::DashLine)
           << qMakePair(tr("· · · · · ·"), Qt::DotLine)
           << qMakePair(tr("- · - · - ·"), Qt::DashDotLine)
           << qMakePair(tr("- · · - · ·"), Qt::DashDotDotLine);
    //<< qMakePair(tr(""), Qt::CustomDashLine);

    ui->cbPenStyle->clear();

    for(auto& style: styles){
        ui->cbPenStyle->addItem(style.first, static_cast<int>(style.second));
    }
}
