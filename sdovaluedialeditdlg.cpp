#include "sdovaluedialeditdlg.h"
#include "ui_sdovaluedialeditdlg.h"
#include "covaluetypes.h"
#include <QColorDialog>



SDOValueDialEditDlg::SDOValueDialEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SDOValueDialEditDlg)
{
    ui->setupUi(this);

    setOutsideBackColor(Qt::black);
    setInsideBackColor(QColor(Qt::darkGray).darker(225));
    setInsideScaleBackColor(QColor(Qt::darkGray).darker(200));
    setTextScaleColor(Qt::white);
    setNeedleColor(Qt::darkRed);

    populateTypes();
}

SDOValueDialEditDlg::~SDOValueDialEditDlg()
{
    delete ui;
}

QString SDOValueDialEditDlg::name() const
{
    return ui->leName->text();
}

void SDOValueDialEditDlg::setName(const QString& newName)
{
    ui->leName->setText(newName);
}

CO::NodeId SDOValueDialEditDlg::nodeId() const
{
    return static_cast<CO::NodeId>(ui->sbNodeId->value());
}

void SDOValueDialEditDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbNodeId->setValue(static_cast<int>(newNodeId));
}

CO::Index SDOValueDialEditDlg::index() const
{
    return static_cast<CO::Index>(ui->sbIndex->value());
}

void SDOValueDialEditDlg::setIndex(CO::Index newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

CO::SubIndex SDOValueDialEditDlg::subIndex() const
{
    return static_cast<CO::SubIndex>(ui->sbSubIndex->value());
}

void SDOValueDialEditDlg::setSubIndex(CO::SubIndex newSubIndex)
{
    ui->sbSubIndex->setValue(static_cast<int>(newSubIndex));
}

COValue::Type SDOValueDialEditDlg::type() const
{
    bool ok = false;
    auto res =  static_cast<COValue::Type>(ui->cbType->currentData().toInt(&ok));
    if(ok) return res;
    return COValue::I32;
}

void SDOValueDialEditDlg::setType(COValue::Type newType)
{
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(newType)));
}

int SDOValueDialEditDlg::posRow() const
{
    return ui->sbPosRow->value();
}

void SDOValueDialEditDlg::setPosRow(int newPosRow)
{
    ui->sbPosRow->setValue(newPosRow);
}

int SDOValueDialEditDlg::posColumn() const
{
    return ui->sbPosCol->value();
}

void SDOValueDialEditDlg::setPosColumn(int newPosColumn)
{
    ui->sbPosCol->setValue(newPosColumn);
}

int SDOValueDialEditDlg::sizeRows() const
{
    return ui->sbSizeRow->value();
}

void SDOValueDialEditDlg::setSizeRows(int newSizeRows)
{
    ui->sbSizeRow->setValue(newSizeRows);
}

int SDOValueDialEditDlg::sizeColumns() const
{
    return ui->sbSizeCol->value();
}

void SDOValueDialEditDlg::setSizeColumns(int newSizeColumns)
{
    ui->sbSizeCol->setValue(newSizeColumns);
}

QColor SDOValueDialEditDlg::outsideBackColor() const
{
    const QPalette& pal = ui->frOutsideBackColor->palette();
    return pal.window().color();
}

void SDOValueDialEditDlg::setOutsideBackColor(const QColor& newBackOuterColor)
{
    QPalette pal = ui->frOutsideBackColor->palette();
    pal.setColor(QPalette::Window, newBackOuterColor);
    ui->frOutsideBackColor->setPalette(pal);
}

QColor SDOValueDialEditDlg::insideBackColor() const
{
    const QPalette& pal = ui->frInsideBackColor->palette();
    return pal.window().color();
}

void SDOValueDialEditDlg::setInsideBackColor(const QColor& newBackInnerColor)
{
    QPalette pal = ui->frInsideBackColor->palette();
    pal.setColor(QPalette::Window, newBackInnerColor);
    ui->frInsideBackColor->setPalette(pal);
}

QColor SDOValueDialEditDlg::insideScaleBackColor() const
{
    const QPalette& pal = ui->frInsideScaleBackColor->palette();
    return pal.window().color();
}

void SDOValueDialEditDlg::setInsideScaleBackColor(const QColor& newBackScaleColor)
{
    QPalette pal = ui->frInsideScaleBackColor->palette();
    pal.setColor(QPalette::Window, newBackScaleColor);
    ui->frInsideScaleBackColor->setPalette(pal);
}

QColor SDOValueDialEditDlg::textScaleColor() const
{
    const QPalette& pal = ui->frTextScaleColor->palette();
    return pal.window().color();
}

void SDOValueDialEditDlg::setTextScaleColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frTextScaleColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frTextScaleColor->setPalette(pal);
}

QColor SDOValueDialEditDlg::needleColor() const
{
    const QPalette& pal = ui->frNeedleColor->palette();
    return pal.window().color();
}

void SDOValueDialEditDlg::setNeedleColor(const QColor& newNeedleColor)
{
    QPalette pal = ui->frNeedleColor->palette();
    pal.setColor(QPalette::Window, newNeedleColor);
    ui->frNeedleColor->setPalette(pal);
}

qreal SDOValueDialEditDlg::rangeMin() const
{
    return ui->sbRangeMin->value();
}

void SDOValueDialEditDlg::setRangeMin(qreal newRangeMin)
{
    ui->sbRangeMin->setValue(newRangeMin);
}

qreal SDOValueDialEditDlg::rangeMax() const
{
    return ui->sbRangeMax->value();
}

void SDOValueDialEditDlg::setRangeMax(qreal newRangeMax)
{
    ui->sbRangeMax->setValue(newRangeMax);
}

qreal SDOValueDialEditDlg::penWidth() const
{
    return ui->sbPenWidth->value();
}

void SDOValueDialEditDlg::setPenWidth(qreal newPenWidth)
{
    ui->sbPenWidth->setValue(newPenWidth);
}

uint SDOValueDialEditDlg::precision() const
{
    return static_cast<uint>(ui->sbPrecision->value());
}

void SDOValueDialEditDlg::setPrecision(uint newPrecision)
{
    ui->sbPrecision->setValue(static_cast<int>(newPrecision));
}

void SDOValueDialEditDlg::on_tbOutsideBackColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frOutsideBackColor);
}

void SDOValueDialEditDlg::on_tbInsideBackColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frInsideBackColor);
}

void SDOValueDialEditDlg::on_tbInsideScaleBackColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frInsideScaleBackColor);
}

void SDOValueDialEditDlg::on_tbTextScaleColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frTextScaleColor);
}

void SDOValueDialEditDlg::on_tbNeedleColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frNeedleColor);
}

void SDOValueDialEditDlg::peekColor(QWidget* colHolder)
{
    QPalette pal = colHolder->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        colHolder->setPalette(pal);
    }
}

void SDOValueDialEditDlg::populateTypes()
{
    auto types = COValue::getNumericTypesNames();

    ui->cbType->clear();

    for(auto& type: types){
        ui->cbType->addItem(type.first, static_cast<int>(type.second));
    }
}
