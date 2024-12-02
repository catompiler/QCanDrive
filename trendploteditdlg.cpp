#include "trendploteditdlg.h"
#include "ui_trendploteditdlg.h"
#include "signalcurveeditdlg.h"
#include "signalcurvepropmodel.h"
#include "signalplot.h"
#include <QColorDialog>



TrendPlotEditDlg::TrendPlotEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrendPlotEditDlg)
{
    ui->setupUi(this);

    m_signalCurvePropMdl = new SignalCurvePropModel();
    ui->lvSignals->setModel(m_signalCurvePropMdl);

    setBackColor(Qt::darkGray);
    setPlotName(tr("График"));
    setTransparency(-1);
}

TrendPlotEditDlg::~TrendPlotEditDlg()
{
    delete m_signalCurvePropMdl;
    delete ui;
}

void TrendPlotEditDlg::on_slTransp_valueChanged(int value)
{
    if(value < 0){
        ui->lblTranspVal->setText(tr("Откл."));
    }else{
        ui->lblTranspVal->setText(QStringLiteral("%1%").arg(value));
    }
}

void TrendPlotEditDlg::on_tbColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    QPalette pal = ui->frBackColor->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета фона"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        ui->frBackColor->setPalette(pal);
    }
}

void TrendPlotEditDlg::on_pbAdd_clicked(bool checked)
{
    Q_UNUSED(checked)

    if(m_signalCurveEditDlg == nullptr) return;

    SignalCurveProp p;
    auto& d = m_signalCurveEditDlg;

    int n = m_signalCurvePropMdl->propsCount();

    d->setName(tr("Сигнал %1").arg(n + 1));
    d->setPenStyle(Qt::SolidLine);
    d->setBrushStyle(Qt::NoBrush);

    auto colors = SignalPlot::getDefaultColors();
    auto col = colors.value(n % colors.size(), Qt::black);
    d->setPenColor(col);
    d->setBrushColor(col);

    if(m_signalCurveEditDlg->exec()){

        p.name = d->name();
        p.nodeId = d->nodeId();
        p.index = d->index();
        p.subIndex = d->subIndex();
        p.type = d->type();
        p.penColor = d->penColor();
        p.penStyle = d->penStyle();
        p.penWidth = d->penWidth();
        p.brushColor = d->brushColor();
        p.brushStyle = d->brushStyle();

        m_signalCurvePropMdl->addProp(p);
    }
}

void TrendPlotEditDlg::on_pbEdit_clicked(bool checked)
{
    Q_UNUSED(checked)

    if(m_signalCurveEditDlg == nullptr) return;

    auto curmi = ui->lvSignals->currentIndex();
    if(!curmi.isValid()) return;

    SignalCurveProp p = m_signalCurvePropMdl->prop(curmi.row());
    auto& d = m_signalCurveEditDlg;

    d->setName(p.name);
    d->setNodeId(p.nodeId);
    d->setIndex(p.index);
    d->setSubIndex(p.subIndex);
    d->setType(p.type);
    d->setPenColor(p.penColor);
    d->setPenStyle(p.penStyle);
    d->setPenWidth(p.penWidth);
    d->setBrushColor(p.brushColor);
    d->setBrushStyle(p.brushStyle);

    if(m_signalCurveEditDlg->exec()){

        p.name = d->name();
        p.nodeId = d->nodeId();
        p.index = d->index();
        p.subIndex = d->subIndex();
        p.type = d->type();
        p.penColor = d->penColor();
        p.penStyle = d->penStyle();
        p.penWidth = d->penWidth();
        p.brushColor = d->brushColor();
        p.brushStyle = d->brushStyle();

        m_signalCurvePropMdl->setProp(curmi.row(), p);
    }
}

void TrendPlotEditDlg::on_pbUp_clicked(bool checked)
{
    Q_UNUSED(checked)

    auto curmi = ui->lvSignals->currentIndex();
    if(!curmi.isValid()) return;

    m_signalCurvePropMdl->moveRow(QModelIndex(), curmi.row(), QModelIndex(), curmi.row() - 1);
}

void TrendPlotEditDlg::on_pbDown_clicked(bool checked)
{
    Q_UNUSED(checked)

    auto curmi = ui->lvSignals->currentIndex();
    if(!curmi.isValid()) return;

    m_signalCurvePropMdl->moveRow(QModelIndex(), curmi.row(), QModelIndex(), curmi.row() + 1);
}

void TrendPlotEditDlg::on_pbDel_clicked(bool checked)
{
    Q_UNUSED(checked)

    auto curmi = ui->lvSignals->currentIndex();

    if(!curmi.isValid()) return;

    m_signalCurvePropMdl->removeRow(curmi.row());
}

SignalCurveEditDlg* TrendPlotEditDlg::signalCurveEditDialog() const
{
    return m_signalCurveEditDlg;
}

void TrendPlotEditDlg::setSignalCurveEditDialog(SignalCurveEditDlg* newSignalCurveEditDialog)
{
    m_signalCurveEditDlg = newSignalCurveEditDialog;
}

int TrendPlotEditDlg::samplesCount() const
{
    return ui->sbSamplesCount->value();
}

void TrendPlotEditDlg::setSamplesCount(int newCount)
{
    ui->sbSamplesCount->setValue(newCount);
}

QString TrendPlotEditDlg::plotName() const
{
    return ui->lePlotName->text();
}

void TrendPlotEditDlg::setPlotName(const QString& newName)
{
    ui->lePlotName->setText(newName);
}

QColor TrendPlotEditDlg::backColor() const
{
    const QPalette& pal = ui->frBackColor->palette();
    return pal.window().color();
}

void TrendPlotEditDlg::setBackColor(const QColor& newBackColor)
{
    QPalette pal = ui->frBackColor->palette();
    pal.setColor(QPalette::Window, newBackColor);
    ui->frBackColor->setPalette(pal);
}

int TrendPlotEditDlg::transparency() const
{
    return ui->slTransp->value();
}

void TrendPlotEditDlg::setTransparency(int newTransparency)
{
    ui->slTransp->setValue(newTransparency);
}

int TrendPlotEditDlg::posRow() const
{
    return ui->sbPosRow->value();
}

void TrendPlotEditDlg::setPosRow(int newPosRow)
{
    ui->sbPosRow->setValue(newPosRow);
}

int TrendPlotEditDlg::posColumn() const
{
    return ui->sbPosCol->value();
}

void TrendPlotEditDlg::setPosColumn(int newPosColumn)
{
    ui->sbPosCol->setValue(newPosColumn);
}

int TrendPlotEditDlg::sizeRows() const
{
    return ui->sbSizeRow->value();
}

void TrendPlotEditDlg::setSizeRows(int newSizeRows)
{
    ui->sbSizeRow->setValue(newSizeRows);
}

int TrendPlotEditDlg::sizeColumns() const
{
    return ui->sbSizeCol->value();
}

void TrendPlotEditDlg::setSizeColumns(int newSizeColumns)
{
    ui->sbSizeCol->setValue(newSizeColumns);
}

int TrendPlotEditDlg::signalsCount() const
{
    return m_signalCurvePropMdl->propsCount();
}

void TrendPlotEditDlg::setSignalsCount(int newSignalsCount)
{
    if(newSignalsCount < 0) return;

    int curCnt = m_signalCurvePropMdl->propsCount();

    if(curCnt < newSignalsCount){
        int dc = newSignalsCount - curCnt;

        m_signalCurvePropMdl->insertRows(curCnt, dc);
    }else if(curCnt > newSignalsCount){
        int dc = curCnt - newSignalsCount;

        m_signalCurvePropMdl->removeRows(newSignalsCount, dc);
    }
}

const SignalCurveProp& TrendPlotEditDlg::signalCurveProp(int n) const
{
    return m_signalCurvePropMdl->prop(n);
}

void TrendPlotEditDlg::setSignalCurveProp(int n, const SignalCurveProp& newSignalCurveProp)
{
    m_signalCurvePropMdl->setProp(n, newSignalCurveProp);
}
