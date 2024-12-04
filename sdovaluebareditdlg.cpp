#include "sdovaluebareditdlg.h"
#include "ui_sdovaluebareditdlg.h"
#include "covaluetypes.h"
#include <QColorDialog>



SDOValueBarEditDlg::SDOValueBarEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SDOValueBarEditDlg)
{
    ui->setupUi(this);

    setBarBackColor(QColor(Qt::darkGray));
    setBarColor(QColor(Qt::darkGray).darker(240));
    setBarAlarmColor(QColor(Qt::darkRed));
    setScaleColor(QColor(Qt::gray));
    setTextColor(QColor(Qt::white));

    populateTypes();
    populateOrientations();
    populateScalePositions();
}

SDOValueBarEditDlg::~SDOValueBarEditDlg()
{
    delete ui;
}

QString SDOValueBarEditDlg::name() const
{
    return ui->leName->text();
}

void SDOValueBarEditDlg::setName(const QString& newName)
{
    ui->leName->setText(newName);
}

CO::NodeId SDOValueBarEditDlg::nodeId() const
{
    return static_cast<CO::NodeId>(ui->sbNodeId->value());
}

void SDOValueBarEditDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbNodeId->setValue(static_cast<int>(newNodeId));
}

CO::Index SDOValueBarEditDlg::index() const
{
    return static_cast<CO::Index>(ui->sbIndex->value());
}

void SDOValueBarEditDlg::setIndex(CO::Index newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

CO::SubIndex SDOValueBarEditDlg::subIndex() const
{
    return static_cast<CO::SubIndex>(ui->sbSubIndex->value());
}

void SDOValueBarEditDlg::setSubIndex(CO::SubIndex newSubIndex)
{
    ui->sbSubIndex->setValue(static_cast<int>(newSubIndex));
}

COValue::Type SDOValueBarEditDlg::type() const
{
    bool ok = false;
    auto res =  static_cast<COValue::Type>(ui->cbType->currentData().toInt(&ok));
    if(ok) return res;
    return COValue::I32;
}

void SDOValueBarEditDlg::setType(COValue::Type newType)
{
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(newType)));
}

int SDOValueBarEditDlg::posRow() const
{
    return ui->sbPosRow->value();
}

void SDOValueBarEditDlg::setPosRow(int newPosRow)
{
    ui->sbPosRow->setValue(newPosRow);
}

int SDOValueBarEditDlg::posColumn() const
{
    return ui->sbPosCol->value();
}

void SDOValueBarEditDlg::setPosColumn(int newPosColumn)
{
    ui->sbPosCol->setValue(newPosColumn);
}

int SDOValueBarEditDlg::sizeRows() const
{
    return ui->sbSizeRow->value();
}

void SDOValueBarEditDlg::setSizeRows(int newSizeRows)
{
    ui->sbSizeRow->setValue(newSizeRows);
}

int SDOValueBarEditDlg::sizeColumns() const
{
    return ui->sbSizeCol->value();
}

void SDOValueBarEditDlg::setSizeColumns(int newSizeColumns)
{
    ui->sbSizeCol->setValue(newSizeColumns);
}

QColor SDOValueBarEditDlg::barBackColor() const
{
    const QPalette& pal = ui->frBarBackColor->palette();
    return pal.window().color();
}

void SDOValueBarEditDlg::setBarBackColor(const QColor& newBarBackColor)
{
    QPalette pal = ui->frBarBackColor->palette();
    pal.setColor(QPalette::Window, newBarBackColor);
    ui->frBarBackColor->setPalette(pal);
}

QColor SDOValueBarEditDlg::barColor() const
{
    const QPalette& pal = ui->frBarColor->palette();
    return pal.window().color();
}

void SDOValueBarEditDlg::setBarColor(const QColor& newBarColor)
{
    QPalette pal = ui->frBarColor->palette();
    pal.setColor(QPalette::Window, newBarColor);
    ui->frBarColor->setPalette(pal);
}

QColor SDOValueBarEditDlg::barAlarmColor() const
{
    const QPalette& pal = ui->frBarAlarmColor->palette();
    return pal.window().color();
}

void SDOValueBarEditDlg::setBarAlarmColor(const QColor& newBarAlarmColor)
{
    QPalette pal = ui->frBarAlarmColor->palette();
    pal.setColor(QPalette::Window, newBarAlarmColor);
    ui->frBarAlarmColor->setPalette(pal);
}

QColor SDOValueBarEditDlg::scaleColor() const
{
    const QPalette& pal = ui->frScaleColor->palette();
    return pal.window().color();
}

void SDOValueBarEditDlg::setScaleColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frScaleColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frScaleColor->setPalette(pal);
}

QColor SDOValueBarEditDlg::textColor() const
{
    const QPalette& pal = ui->frTextColor->palette();
    return pal.window().color();
}

void SDOValueBarEditDlg::setTextColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frTextColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frTextColor->setPalette(pal);
}

qreal SDOValueBarEditDlg::rangeMin() const
{
    return ui->sbRangeMin->value();
}

void SDOValueBarEditDlg::setRangeMin(qreal newRangeMin)
{
    ui->sbRangeMin->setValue(newRangeMin);
}

qreal SDOValueBarEditDlg::rangeMax() const
{
    return ui->sbRangeMax->value();
}

void SDOValueBarEditDlg::setRangeMax(qreal newRangeMax)
{
    ui->sbRangeMax->setValue(newRangeMax);
}

int SDOValueBarEditDlg::barWidth() const
{
    return ui->sbBarWidth->value();
}

void SDOValueBarEditDlg::setBarWidth(int newBarWidth)
{
    ui->sbBarWidth->setValue(newBarWidth);
}

int SDOValueBarEditDlg::borderWidth() const
{
    return ui->sbBorderWidth->value();
}

void SDOValueBarEditDlg::setBorderWidth(int newBorderWidth)
{
    ui->sbBorderWidth->setValue(newBorderWidth);
}

qreal SDOValueBarEditDlg::penWidth() const
{
    return ui->sbPenWidth->value();
}

void SDOValueBarEditDlg::setPenWidth(qreal newPenWidth)
{
    ui->sbPenWidth->setValue(newPenWidth);
}

Qt::Orientation SDOValueBarEditDlg::orientation() const
{
    bool ok = false;
    auto res = static_cast<Qt::Orientation>(ui->cbOrientation->currentData().toInt(&ok));
    if(ok) return res;
    return Qt::Vertical;
}

void SDOValueBarEditDlg::setOrientation(Qt::Orientation newOrient)
{
    ui->cbOrientation->setCurrentIndex(ui->cbOrientation->findData(static_cast<int>(newOrient)));
}

QwtThermo::ScalePosition SDOValueBarEditDlg::scalePosition() const
{
    bool ok = false;
    auto res = static_cast<QwtThermo::ScalePosition>(ui->cbScalePos->currentData().toInt(&ok));
    if(ok) return res;
    return QwtThermo::NoScale;
}

void SDOValueBarEditDlg::setScalePosition(QwtThermo::ScalePosition newPos)
{
    ui->cbScalePos->setCurrentIndex(ui->cbScalePos->findData(static_cast<int>(newPos)));
}

bool SDOValueBarEditDlg::alarmEnabled() const
{
    return ui->cbAlarmEnable->isChecked();
}

void SDOValueBarEditDlg::setAlarmEnabled(bool newEnabled)
{
    ui->cbAlarmEnable->setChecked(newEnabled);
}

qreal SDOValueBarEditDlg::alarmLevel() const
{
    return ui->sbAlarmLevel->value();
}

void SDOValueBarEditDlg::setAlarmLevel(qreal newLevel)
{
    ui->sbAlarmLevel->setValue(newLevel);
}

void SDOValueBarEditDlg::on_tbBarBackColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frBarBackColor);
}

void SDOValueBarEditDlg::on_tbBarColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frBarColor);
}

void SDOValueBarEditDlg::on_tbBarAlarmColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frBarAlarmColor);
}

void SDOValueBarEditDlg::on_tbScaleColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frScaleColor);
}

void SDOValueBarEditDlg::on_tbTextColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frTextColor);
}

void SDOValueBarEditDlg::on_sbRangeMin_valueChanged(double d)
{
    ui->sbAlarmLevel->setMinimum(d);
}

void SDOValueBarEditDlg::on_sbRangeMax_valueChanged(double d)
{
    ui->sbAlarmLevel->setMaximum(d);
}

void SDOValueBarEditDlg::peekColor(QWidget* colHolder)
{
    QPalette pal = colHolder->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        colHolder->setPalette(pal);
    }
}

void SDOValueBarEditDlg::populateTypes()
{
    auto types = COValue::getTypesNames();

    ui->cbType->clear();

    for(auto& type: types){
        ui->cbType->addItem(type.first, static_cast<int>(type.second));
    }
}

void SDOValueBarEditDlg::populateOrientations()
{
    ui->cbOrientation->clear();

    ui->cbOrientation->addItem(tr("Вертикально"), static_cast<int>(Qt::Vertical));
    ui->cbOrientation->addItem(tr("Горизонтально"), static_cast<int>(Qt::Horizontal));
}

void SDOValueBarEditDlg::populateScalePositions()
{
    ui->cbScalePos->clear();
    ui->cbScalePos->addItem(tr("Нет"), static_cast<int>(QwtThermo::NoScale));
    ui->cbScalePos->addItem(tr("После"), static_cast<int>(QwtThermo::LeadingScale));
    ui->cbScalePos->addItem(tr("Перед"), static_cast<int>(QwtThermo::TrailingScale));
}
