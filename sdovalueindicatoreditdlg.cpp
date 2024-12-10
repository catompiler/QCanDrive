#include "sdovalueindicatoreditdlg.h"
#include "ui_sdovalueindicatoreditdlg.h"
#include "covaluetypes.h"
#include <stdint.h>
#include <QColorDialog>
#include <QDebug>



SDOValueIndicatorEditDlg::SDOValueIndicatorEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SDOValueIndicatorEditDlg)
{
    ui->setupUi(this);

    setBackColor(QColor::fromRgb(0xe7e7e7));
    setShadowColor(Qt::black);
    setIndicatorColor(QColor::fromRgb(0xbcfa24));
    setGlareColor(QColor::fromRgb(0xc7c5c1));
    setTextColor(QColor(Qt::black));

    populateTypes();
    populateCompares();
}

SDOValueIndicatorEditDlg::~SDOValueIndicatorEditDlg()
{
    delete ui;
}

QString SDOValueIndicatorEditDlg::text() const
{
    return ui->leName->text();
}

void SDOValueIndicatorEditDlg::setText(const QString& newName)
{
    ui->leName->setText(newName);
}

CO::NodeId SDOValueIndicatorEditDlg::nodeId() const
{
    return static_cast<CO::NodeId>(ui->sbNodeId->value());
}

void SDOValueIndicatorEditDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbNodeId->setValue(static_cast<int>(newNodeId));
}

CO::Index SDOValueIndicatorEditDlg::index() const
{
    return static_cast<CO::Index>(ui->sbIndex->value());
}

void SDOValueIndicatorEditDlg::setIndex(CO::Index newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

CO::SubIndex SDOValueIndicatorEditDlg::subIndex() const
{
    return static_cast<CO::SubIndex>(ui->sbSubIndex->value());
}

void SDOValueIndicatorEditDlg::setSubIndex(CO::SubIndex newSubIndex)
{
    ui->sbSubIndex->setValue(static_cast<int>(newSubIndex));
}

COValue::Type SDOValueIndicatorEditDlg::type() const
{
    bool ok = false;
    auto res =  static_cast<COValue::Type>(ui->cbType->currentData().toInt(&ok));
    if(ok) return res;
    return COValue::I32;
}

void SDOValueIndicatorEditDlg::setType(COValue::Type newType)
{
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(newType)));
}

int SDOValueIndicatorEditDlg::posRow() const
{
    return ui->sbPosRow->value();
}

void SDOValueIndicatorEditDlg::setPosRow(int newPosRow)
{
    ui->sbPosRow->setValue(newPosRow);
}

int SDOValueIndicatorEditDlg::posColumn() const
{
    return ui->sbPosCol->value();
}

void SDOValueIndicatorEditDlg::setPosColumn(int newPosColumn)
{
    ui->sbPosCol->setValue(newPosColumn);
}

int SDOValueIndicatorEditDlg::sizeRows() const
{
    return ui->sbSizeRow->value();
}

void SDOValueIndicatorEditDlg::setSizeRows(int newSizeRows)
{
    ui->sbSizeRow->setValue(newSizeRows);
}

int SDOValueIndicatorEditDlg::sizeColumns() const
{
    return ui->sbSizeCol->value();
}

void SDOValueIndicatorEditDlg::setSizeColumns(int newSizeColumns)
{
    ui->sbSizeCol->setValue(newSizeColumns);
}

QColor SDOValueIndicatorEditDlg::backColor() const
{
    const QPalette& pal = ui->frBackColor->palette();
    return pal.window().color();
}

void SDOValueIndicatorEditDlg::setBackColor(const QColor& newBackColor)
{
    QPalette pal = ui->frBackColor->palette();
    pal.setColor(QPalette::Window, newBackColor);
    ui->frBackColor->setPalette(pal);
}

QColor SDOValueIndicatorEditDlg::shadowColor() const
{
    const QPalette& pal = ui->frShadowColor->palette();
    return pal.window().color();
}

void SDOValueIndicatorEditDlg::setShadowColor(const QColor& newShadowColor)
{
    QPalette pal = ui->frShadowColor->palette();
    pal.setColor(QPalette::Window, newShadowColor);
    ui->frShadowColor->setPalette(pal);
}

QColor SDOValueIndicatorEditDlg::indicatorColor() const
{
    const QPalette& pal = ui->frIndicatorColor->palette();
    return pal.window().color();
}

void SDOValueIndicatorEditDlg::setIndicatorColor(const QColor& newIndicatorColor)
{
    QPalette pal = ui->frIndicatorColor->palette();
    pal.setColor(QPalette::Window, newIndicatorColor);
    ui->frIndicatorColor->setPalette(pal);
}

QColor SDOValueIndicatorEditDlg::glareColor() const
{
    const QPalette& pal = ui->frGlareColor->palette();
    return pal.window().color();
}

void SDOValueIndicatorEditDlg::setGlareColor(const QColor& newGlareColor)
{
    QPalette pal = ui->frGlareColor->palette();
    pal.setColor(QPalette::Window, newGlareColor);
    ui->frGlareColor->setPalette(pal);
}

QColor SDOValueIndicatorEditDlg::textColor() const
{
    const QPalette& pal = ui->frTextColor->palette();
    return pal.window().color();
}

void SDOValueIndicatorEditDlg::setTextColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frTextColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frTextColor->setPalette(pal);
}

int SDOValueIndicatorEditDlg::borderWidth() const
{
    return ui->sbBorderWidth->value();
}

void SDOValueIndicatorEditDlg::setBorderWidth(int newBorderWidth)
{
    ui->sbBorderWidth->setValue(newBorderWidth);
}

int SDOValueIndicatorEditDlg::fontPointSize() const
{
    return ui->sbFontSize->value();
}

void SDOValueIndicatorEditDlg::setFontPointSize(int newSize)
{
    ui->sbFontSize->setValue(newSize);
}

bool SDOValueIndicatorEditDlg::fontCapitalization() const
{
    return ui->cbFontCapitalization->isChecked();
}

void SDOValueIndicatorEditDlg::setFontCapitalization(bool newCap)
{
    ui->cbFontCapitalization->setChecked(newCap);
}

bool SDOValueIndicatorEditDlg::fontBold() const
{
    return ui->cbFontBold->isChecked();
}

void SDOValueIndicatorEditDlg::setFontBold(bool newBold)
{
    ui->cbFontBold->setChecked(newBold);
}

SDOValueIndicator::CompareType SDOValueIndicatorEditDlg::indicatorCompare() const
{
    bool ok = false;
    auto res =  static_cast<SDOValueIndicator::CompareType>(ui->cbIndicatorCompare->currentData().toInt(&ok));
    if(ok) return res;
    return SDOValueIndicator::EQUAL;
}

void SDOValueIndicatorEditDlg::setIndicatorCompare(SDOValueIndicator::CompareType newIndicatorCompare)
{
    ui->cbIndicatorCompare->setCurrentIndex(ui->cbIndicatorCompare->findData(static_cast<int>(newIndicatorCompare)));
}

uint32_t SDOValueIndicatorEditDlg::indicatorValue() const
{
    QString valueStr = ui->leIndicatorValue->text().trimmed();

    if(valueStr.isEmpty()){
        valueStr = ui->leIndicatorValue->placeholderText();
    }

    //qDebug() << valueStr;

    bool isOk = false;

    auto val = valueStr.toUInt(&isOk, 0);

    //qDebug() << val;

    if(isOk) return val;

    return 0;
}

void SDOValueIndicatorEditDlg::setIndicatorValue(uint32_t newIndicatorValue)
{
    ui->leIndicatorValue->setText(QStringLiteral("0x%1").arg(newIndicatorValue, 0, 16));
}

void SDOValueIndicatorEditDlg::on_tbBackColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frBackColor);
}

void SDOValueIndicatorEditDlg::on_tbShadowColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frShadowColor);
}

void SDOValueIndicatorEditDlg::on_tbIndicatorColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frIndicatorColor);
}

void SDOValueIndicatorEditDlg::on_tbGlareColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frGlareColor);
}

void SDOValueIndicatorEditDlg::on_tbTextColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frTextColor);
}

void SDOValueIndicatorEditDlg::peekColor(QWidget* colHolder)
{
    QPalette pal = colHolder->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        colHolder->setPalette(pal);
    }
}

void SDOValueIndicatorEditDlg::populateTypes()
{
    auto types = COValue::getTypesNames();

    ui->cbType->clear();

    for(auto& type: types){
        ui->cbType->addItem(type.first, static_cast<int>(type.second));
    }
}

void SDOValueIndicatorEditDlg::populateCompares()
{
    ui->cbIndicatorCompare->addItem(tr("Равно"), static_cast<int>(SDOValueIndicator::EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Не равно"), static_cast<int>(SDOValueIndicator::NOT_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Меньше"), static_cast<int>(SDOValueIndicator::LESS));
    ui->cbIndicatorCompare->addItem(tr("Меньше или равно"), static_cast<int>(SDOValueIndicator::LESS_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Больше"), static_cast<int>(SDOValueIndicator::GREATER_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Больше или равно"), static_cast<int>(SDOValueIndicator::GREATER_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Маска, равно"), static_cast<int>(SDOValueIndicator::MASK));
    ui->cbIndicatorCompare->addItem(tr("Маска, ноль"), static_cast<int>(SDOValueIndicator::MASK_ZERO));
}
