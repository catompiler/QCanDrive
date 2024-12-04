#include "sdovalueslidereditdlg.h"
#include "ui_sdovalueslidereditdlg.h"
#include "covaluetypes.h"
#include <QColorDialog>



SDOValueSliderEditDlg::SDOValueSliderEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SDOValueSliderEditDlg)
{
    ui->setupUi(this);

    setTroughColor(QColor(Qt::darkGray));
    setGrooveColor(QColor(Qt::darkGray).darker(240));
    setHandleColor(QColor(Qt::darkGray).darker(225));
    setScaleColor(QColor(Qt::gray));
    setTextColor(QColor(Qt::white));

    populateTypes();
    populateOrientations();
}

SDOValueSliderEditDlg::~SDOValueSliderEditDlg()
{
    delete ui;
}

QString SDOValueSliderEditDlg::name() const
{
    return ui->leName->text();
}

void SDOValueSliderEditDlg::setName(const QString& newName)
{
    ui->leName->setText(newName);
}

CO::NodeId SDOValueSliderEditDlg::nodeId() const
{
    return static_cast<CO::NodeId>(ui->sbNodeId->value());
}

void SDOValueSliderEditDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbNodeId->setValue(static_cast<int>(newNodeId));
}

CO::Index SDOValueSliderEditDlg::index() const
{
    return static_cast<CO::Index>(ui->sbIndex->value());
}

void SDOValueSliderEditDlg::setIndex(CO::Index newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

CO::SubIndex SDOValueSliderEditDlg::subIndex() const
{
    return static_cast<CO::SubIndex>(ui->sbSubIndex->value());
}

void SDOValueSliderEditDlg::setSubIndex(CO::SubIndex newSubIndex)
{
    ui->sbSubIndex->setValue(static_cast<int>(newSubIndex));
}

COValue::Type SDOValueSliderEditDlg::type() const
{
    bool ok = false;
    auto res =  static_cast<COValue::Type>(ui->cbType->currentData().toInt(&ok));
    if(ok) return res;
    return COValue::I32;
}

void SDOValueSliderEditDlg::setType(COValue::Type newType)
{
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(newType)));
}

int SDOValueSliderEditDlg::posRow() const
{
    return ui->sbPosRow->value();
}

void SDOValueSliderEditDlg::setPosRow(int newPosRow)
{
    ui->sbPosRow->setValue(newPosRow);
}

int SDOValueSliderEditDlg::posColumn() const
{
    return ui->sbPosCol->value();
}

void SDOValueSliderEditDlg::setPosColumn(int newPosColumn)
{
    ui->sbPosCol->setValue(newPosColumn);
}

int SDOValueSliderEditDlg::sizeRows() const
{
    return ui->sbSizeRow->value();
}

void SDOValueSliderEditDlg::setSizeRows(int newSizeRows)
{
    ui->sbSizeRow->setValue(newSizeRows);
}

int SDOValueSliderEditDlg::sizeColumns() const
{
    return ui->sbSizeCol->value();
}

void SDOValueSliderEditDlg::setSizeColumns(int newSizeColumns)
{
    ui->sbSizeCol->setValue(newSizeColumns);
}

QColor SDOValueSliderEditDlg::troughColor() const
{
    const QPalette& pal = ui->frTroughColor->palette();
    return pal.window().color();
}

void SDOValueSliderEditDlg::setTroughColor(const QColor& newBackOuterColor)
{
    QPalette pal = ui->frTroughColor->palette();
    pal.setColor(QPalette::Window, newBackOuterColor);
    ui->frTroughColor->setPalette(pal);
}

QColor SDOValueSliderEditDlg::grooveColor() const
{
    const QPalette& pal = ui->frGrooveColor->palette();
    return pal.window().color();
}

void SDOValueSliderEditDlg::setGrooveColor(const QColor& newBackInnerColor)
{
    QPalette pal = ui->frGrooveColor->palette();
    pal.setColor(QPalette::Window, newBackInnerColor);
    ui->frGrooveColor->setPalette(pal);
}

QColor SDOValueSliderEditDlg::handleColor() const
{
    const QPalette& pal = ui->frHandleColor->palette();
    return pal.window().color();
}

void SDOValueSliderEditDlg::setHandleColor(const QColor& newBackScaleColor)
{
    QPalette pal = ui->frHandleColor->palette();
    pal.setColor(QPalette::Window, newBackScaleColor);
    ui->frHandleColor->setPalette(pal);
}

QColor SDOValueSliderEditDlg::scaleColor() const
{
    const QPalette& pal = ui->frScaleColor->palette();
    return pal.window().color();
}

void SDOValueSliderEditDlg::setScaleColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frScaleColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frScaleColor->setPalette(pal);
}

QColor SDOValueSliderEditDlg::textColor() const
{
    const QPalette& pal = ui->frTextColor->palette();
    return pal.window().color();
}

void SDOValueSliderEditDlg::setTextColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frTextColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frTextColor->setPalette(pal);
}

qreal SDOValueSliderEditDlg::rangeMin() const
{
    return ui->sbRangeMin->value();
}

void SDOValueSliderEditDlg::setRangeMin(qreal newRangeMin)
{
    ui->sbRangeMin->setValue(newRangeMin);
}

qreal SDOValueSliderEditDlg::rangeMax() const
{
    return ui->sbRangeMax->value();
}

void SDOValueSliderEditDlg::setRangeMax(qreal newRangeMax)
{
    ui->sbRangeMax->setValue(newRangeMax);
}

qreal SDOValueSliderEditDlg::penWidth() const
{
    return ui->sbPenWidth->value();
}

void SDOValueSliderEditDlg::setPenWidth(qreal newPenWidth)
{
    ui->sbPenWidth->setValue(newPenWidth);
}

uint SDOValueSliderEditDlg::steps() const
{
    return static_cast<uint>(ui->sbSteps->value());
}

void SDOValueSliderEditDlg::setSteps(uint newSteps)
{
    ui->sbSteps->setValue(static_cast<int>(newSteps));
}

bool SDOValueSliderEditDlg::hasTrough() const
{
    return ui->cbTrough->isChecked();
}

void SDOValueSliderEditDlg::setHasTrough(bool newHasTrough)
{
    ui->cbTrough->setChecked(newHasTrough);
}

bool SDOValueSliderEditDlg::hasGroove() const
{
    return ui->cbGroove->isChecked();
}

void SDOValueSliderEditDlg::setHasGroove(bool newHasGroove)
{
    ui->cbGroove->setChecked(newHasGroove);
}

Qt::Orientation SDOValueSliderEditDlg::orientation() const
{
    bool ok = false;
    auto res =  static_cast<Qt::Orientation>(ui->cbOrientation->currentData().toInt(&ok));
    if(ok) return res;
    return Qt::Vertical;
}

void SDOValueSliderEditDlg::setOrientation(Qt::Orientation newOrientation)
{
    ui->cbOrientation->setCurrentIndex(ui->cbOrientation->findData(static_cast<int>(newOrientation)));
}

void SDOValueSliderEditDlg::on_tbTroughColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frTroughColor);
}

void SDOValueSliderEditDlg::on_tbGrooveColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frGrooveColor);
}

void SDOValueSliderEditDlg::on_tbHandleColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frHandleColor);
}

void SDOValueSliderEditDlg::on_tbScaleColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frScaleColor);
}

void SDOValueSliderEditDlg::on_tbTextColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frTextColor);
}

void SDOValueSliderEditDlg::peekColor(QWidget* colHolder)
{
    QPalette pal = colHolder->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        colHolder->setPalette(pal);
    }
}

void SDOValueSliderEditDlg::populateTypes()
{
    auto types = COValue::getTypesNames();

    ui->cbType->clear();

    for(auto& type: types){
        ui->cbType->addItem(type.first, static_cast<int>(type.second));
    }
}

void SDOValueSliderEditDlg::populateOrientations()
{
    ui->cbOrientation->clear();

    ui->cbOrientation->addItem(tr("Вертикально"), static_cast<int>(Qt::Vertical));
    ui->cbOrientation->addItem(tr("Горизонтально"), static_cast<int>(Qt::Horizontal));
}
