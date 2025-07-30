#include "sdovaluebuttoneditdlg.h"
#include "ui_sdovaluebuttoneditdlg.h"
#include "covaluetypes.h"
#include <stdint.h>
#include <QColorDialog>
#include <QDebug>



SDOValueButtonEditDlg::SDOValueButtonEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SDOValueButtonEditDlg)
{
    ui->setupUi(this);

    setButtonColor(QColor::fromRgb(0x8c857b));
    setBorderColor(Qt::black);
    setIndicatorColor(QColor::fromRgb(0xfb9972));
    setActivateColor(QColor::fromRgb(0xc7c5c1));
    setHighlightColor(palette().highlight().color());
    setTextColor(QColor(Qt::black));

    populateTypes();
    populateCompares();
}

SDOValueButtonEditDlg::~SDOValueButtonEditDlg()
{
    delete ui;
}

QString SDOValueButtonEditDlg::text() const
{
    return ui->leName->text();
}

void SDOValueButtonEditDlg::setText(const QString& newName)
{
    ui->leName->setText(newName);
}

CO::NodeId SDOValueButtonEditDlg::nodeId() const
{
    return static_cast<CO::NodeId>(ui->sbNodeId->value());
}

void SDOValueButtonEditDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbNodeId->setValue(static_cast<int>(newNodeId));
}

CO::Index SDOValueButtonEditDlg::index() const
{
    return static_cast<CO::Index>(ui->sbIndex->value());
}

void SDOValueButtonEditDlg::setIndex(CO::Index newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

CO::SubIndex SDOValueButtonEditDlg::subIndex() const
{
    return static_cast<CO::SubIndex>(ui->sbSubIndex->value());
}

void SDOValueButtonEditDlg::setSubIndex(CO::SubIndex newSubIndex)
{
    ui->sbSubIndex->setValue(static_cast<int>(newSubIndex));
}

COValue::Type SDOValueButtonEditDlg::type() const
{
    bool ok = false;
    auto res =  static_cast<COValue::Type>(ui->cbType->currentData().toInt(&ok));
    if(ok) return res;
    return COValue::I32;
}

void SDOValueButtonEditDlg::setType(COValue::Type newType)
{
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(newType)));
}

int SDOValueButtonEditDlg::posRow() const
{
    return ui->sbPosRow->value();
}

void SDOValueButtonEditDlg::setPosRow(int newPosRow)
{
    ui->sbPosRow->setValue(newPosRow);
}

int SDOValueButtonEditDlg::posColumn() const
{
    return ui->sbPosCol->value();
}

void SDOValueButtonEditDlg::setPosColumn(int newPosColumn)
{
    ui->sbPosCol->setValue(newPosColumn);
}

int SDOValueButtonEditDlg::sizeRows() const
{
    return ui->sbSizeRow->value();
}

void SDOValueButtonEditDlg::setSizeRows(int newSizeRows)
{
    ui->sbSizeRow->setValue(newSizeRows);
}

int SDOValueButtonEditDlg::sizeColumns() const
{
    return ui->sbSizeCol->value();
}

void SDOValueButtonEditDlg::setSizeColumns(int newSizeColumns)
{
    ui->sbSizeCol->setValue(newSizeColumns);
}

QColor SDOValueButtonEditDlg::buttonColor() const
{
    const QPalette& pal = ui->frButtonColor->palette();
    return pal.window().color();
}

void SDOValueButtonEditDlg::setButtonColor(const QColor& newButtonColor)
{
    QPalette pal = ui->frButtonColor->palette();
    pal.setColor(QPalette::Window, newButtonColor);
    ui->frButtonColor->setPalette(pal);
}

QColor SDOValueButtonEditDlg::borderColor() const
{
    const QPalette& pal = ui->frBorderColor->palette();
    return pal.window().color();
}

void SDOValueButtonEditDlg::setBorderColor(const QColor& newBorderColor)
{
    QPalette pal = ui->frBorderColor->palette();
    pal.setColor(QPalette::Window, newBorderColor);
    ui->frBorderColor->setPalette(pal);
}

QColor SDOValueButtonEditDlg::indicatorColor() const
{
    const QPalette& pal = ui->frIndicatorColor->palette();
    return pal.window().color();
}

void SDOValueButtonEditDlg::setIndicatorColor(const QColor& newIndicatorColor)
{
    QPalette pal = ui->frIndicatorColor->palette();
    pal.setColor(QPalette::Window, newIndicatorColor);
    ui->frIndicatorColor->setPalette(pal);
}

QColor SDOValueButtonEditDlg::activateColor() const
{
    const QPalette& pal = ui->frActivateColor->palette();
    return pal.window().color();
}

void SDOValueButtonEditDlg::setActivateColor(const QColor& newActivateColor)
{
    QPalette pal = ui->frActivateColor->palette();
    pal.setColor(QPalette::Window, newActivateColor);
    ui->frActivateColor->setPalette(pal);
}

QColor SDOValueButtonEditDlg::highlightColor() const
{
    const QPalette& pal = ui->frHighlightColor->palette();
    return pal.window().color();
}

void SDOValueButtonEditDlg::setHighlightColor(const QColor& newHlColor)
{
    QPalette pal = ui->frHighlightColor->palette();
    pal.setColor(QPalette::Window, newHlColor);
    ui->frHighlightColor->setPalette(pal);
}

QColor SDOValueButtonEditDlg::textColor() const
{
    const QPalette& pal = ui->frTextColor->palette();
    return pal.window().color();
}

void SDOValueButtonEditDlg::setTextColor(const QColor& newTextScaleColor)
{
    QPalette pal = ui->frTextColor->palette();
    pal.setColor(QPalette::Window, newTextScaleColor);
    ui->frTextColor->setPalette(pal);
}

int SDOValueButtonEditDlg::borderWidth() const
{
    return ui->sbBorderWidth->value();
}

void SDOValueButtonEditDlg::setBorderWidth(int newBorderWidth)
{
    ui->sbBorderWidth->setValue(newBorderWidth);
}

bool SDOValueButtonEditDlg::indicatorEnabled() const
{
    return ui->cbIndicationEnabled->isChecked();
}

void SDOValueButtonEditDlg::setIndicatorEnabled(bool newEnabled)
{
    ui->cbIndicationEnabled->setChecked(newEnabled);
}

int SDOValueButtonEditDlg::fontPointSize() const
{
    return ui->sbFontSize->value();
}

void SDOValueButtonEditDlg::setFontPointSize(int newSize)
{
    ui->sbFontSize->setValue(newSize);
}

bool SDOValueButtonEditDlg::fontCapitalization() const
{
    return ui->cbFontCapitalization->isChecked();
}

void SDOValueButtonEditDlg::setFontCapitalization(bool newCap)
{
    ui->cbFontCapitalization->setChecked(newCap);
}

bool SDOValueButtonEditDlg::fontBold() const
{
    return ui->cbFontBold->isChecked();
}

void SDOValueButtonEditDlg::setFontBold(bool newBold)
{
    ui->cbFontBold->setChecked(newBold);
}

SDOValueButton::CompareType SDOValueButtonEditDlg::indicatorCompare() const
{
    bool ok = false;
    auto res =  static_cast<SDOValueButton::CompareType>(ui->cbIndicatorCompare->currentData().toInt(&ok));
    if(ok) return res;
    return SDOValueButton::EQUAL;
}

void SDOValueButtonEditDlg::setIndicatorCompare(SDOValueButton::CompareType newIndicatorCompare)
{
    ui->cbIndicatorCompare->setCurrentIndex(ui->cbIndicatorCompare->findData(static_cast<int>(newIndicatorCompare)));
}

uint32_t SDOValueButtonEditDlg::indicatorValue() const
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

void SDOValueButtonEditDlg::setIndicatorValue(uint32_t newIndicatorValue)
{
    ui->leIndicatorValue->setText(QStringLiteral("0x%1").arg(newIndicatorValue, 0, 16));
}

uint32_t SDOValueButtonEditDlg::activateValue() const
{
    QString valueStr = ui->leActivateValue->text().trimmed();

    if(valueStr.isEmpty()){
        valueStr = ui->leActivateValue->placeholderText();
    }

    //qDebug() << valueStr;

    bool isOk = false;

    auto val = valueStr.toUInt(&isOk, 0);

    //qDebug() << val;

    if(isOk) return val;

    return 0;
}

void SDOValueButtonEditDlg::setActivateValue(uint32_t newActivateValue)
{
    //qDebug() << newActivateValue;

    ui->leActivateValue->setText(QStringLiteral("0x%1").arg(newActivateValue, 0, 16));
}

void SDOValueButtonEditDlg::on_tbButtonColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frButtonColor);
}

void SDOValueButtonEditDlg::on_tbBorderColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frBorderColor);
}

void SDOValueButtonEditDlg::on_tbIndicatorColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frIndicatorColor);
}

void SDOValueButtonEditDlg::on_tbActivateColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frActivateColor);
}

void SDOValueButtonEditDlg::on_tbHighlightColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frHighlightColor);
}

void SDOValueButtonEditDlg::on_tbTextColorSel_clicked(bool checked)
{
    Q_UNUSED(checked)

    peekColor(ui->frTextColor);
}

void SDOValueButtonEditDlg::peekColor(QWidget* colHolder)
{
    QPalette pal = colHolder->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        colHolder->setPalette(pal);
    }
}

void SDOValueButtonEditDlg::populateTypes()
{
    auto types = COValue::getNumericTypesNames();

    ui->cbType->clear();

    for(auto& type: types){
        ui->cbType->addItem(type.first, static_cast<int>(type.second));
    }
}

void SDOValueButtonEditDlg::populateCompares()
{
    ui->cbIndicatorCompare->addItem(tr("Равно"), static_cast<int>(SDOValueButton::EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Не равно"), static_cast<int>(SDOValueButton::NOT_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Меньше"), static_cast<int>(SDOValueButton::LESS));
    ui->cbIndicatorCompare->addItem(tr("Меньше или равно"), static_cast<int>(SDOValueButton::LESS_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Больше"), static_cast<int>(SDOValueButton::GREATER_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Больше или равно"), static_cast<int>(SDOValueButton::GREATER_EQUAL));
    ui->cbIndicatorCompare->addItem(tr("Маска, равно"), static_cast<int>(SDOValueButton::MASK));
    ui->cbIndicatorCompare->addItem(tr("Маска, ноль"), static_cast<int>(SDOValueButton::MASK_ZERO));
}
