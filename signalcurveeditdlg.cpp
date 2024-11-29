#include "signalcurveeditdlg.h"
#include "ui_signalcurveeditdlg.h"
#include "covaluetypes.h"
#include <QColorDialog>
#include <QList>
#include <QPair>
#include <QDebug>



SignalCurveEditDlg::SignalCurveEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalCurveEditDlg)
{
    ui->setupUi(this);

    setPenColor(Qt::black);
    setBrushColor(Qt::black);

    populateTypes();
    populatePenStyles();
    populateBrushStyles();
}

SignalCurveEditDlg::~SignalCurveEditDlg()
{
    delete ui;
}

QString SignalCurveEditDlg::name() const
{
    return ui->leName->text();
}

void SignalCurveEditDlg::setName(const QString& newName)
{
    ui->leName->setText(newName);
}

CO::NodeId SignalCurveEditDlg::nodeId() const
{
    return static_cast<CO::NodeId>(ui->sbNodeId->value());
}

void SignalCurveEditDlg::setNodeId(CO::NodeId newNodeId)
{
    ui->sbNodeId->setValue(static_cast<int>(newNodeId));
}

CO::Index SignalCurveEditDlg::index() const
{
    return static_cast<CO::Index>(ui->sbIndex->value());
}

void SignalCurveEditDlg::setIndex(CO::Index newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

CO::SubIndex SignalCurveEditDlg::subIndex() const
{
    return static_cast<CO::SubIndex>(ui->sbSubIndex->value());
}

void SignalCurveEditDlg::setSubIndex(CO::SubIndex newSubIndex)
{
    ui->sbSubIndex->setValue(static_cast<int>(newSubIndex));
}

COValue::Type SignalCurveEditDlg::type() const
{
    bool ok = false;
    auto res =  static_cast<COValue::Type>(ui->cbType->currentData().toInt(&ok));
    if(ok) return res;
    return COValue::I32;
}

void SignalCurveEditDlg::setType(COValue::Type newType)
{
    ui->cbType->setCurrentIndex(ui->cbType->findData(static_cast<int>(newType)));
}

QColor SignalCurveEditDlg::penColor() const
{
    const QPalette& pal = ui->frPenColor->palette();
    return pal.window().color();
}

void SignalCurveEditDlg::setPenColor(const QColor& newPenColor)
{
    QPalette pal = ui->frPenColor->palette();
    pal.setColor(QPalette::Window, newPenColor);
    ui->frPenColor->setPalette(pal);
}

Qt::PenStyle SignalCurveEditDlg::penStyle() const
{
    bool ok = false;
    auto res =  static_cast<Qt::PenStyle>(ui->cbPenStyle->currentData().toInt(&ok));
    if(ok) return res;
    return Qt::NoPen;
}

void SignalCurveEditDlg::setPenStyle(Qt::PenStyle newPenStyle)
{
    ui->cbPenStyle->setCurrentIndex(ui->cbPenStyle->findData(static_cast<int>(newPenStyle)));
}

QColor SignalCurveEditDlg::brushColor() const
{
    const QPalette& pal = ui->frBrushColor->palette();
    return pal.window().color();
}

void SignalCurveEditDlg::setBrushColor(const QColor& newBrushColor)
{
    QPalette pal = ui->frBrushColor->palette();
    pal.setColor(QPalette::Window, newBrushColor);
    ui->frBrushColor->setPalette(pal);
}

Qt::BrushStyle SignalCurveEditDlg::brushStyle() const
{
    bool ok = false;
    auto res =  static_cast<Qt::BrushStyle>(ui->cbBrushStyle->currentData().toInt(&ok));
    if(ok) return res;
    return Qt::NoBrush;
}

void SignalCurveEditDlg::setBrushStyle(Qt::BrushStyle newBrushStyle)
{
    ui->cbBrushStyle->setCurrentIndex(ui->cbBrushStyle->findData(static_cast<int>(newBrushStyle)));
}

void SignalCurveEditDlg::on_tbPenColorSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    QPalette pal = ui->frPenColor->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета линии"), QColorDialog::ShowAlphaChannel);
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        ui->frPenColor->setPalette(pal);
    }
}

void SignalCurveEditDlg::on_tbBrushColorSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    QPalette pal = ui->frBrushColor->palette();
    QColor curCol = pal.window().color();

    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета заливки"), QColorDialog::ShowAlphaChannel);
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        ui->frBrushColor->setPalette(pal);
    }
}

void SignalCurveEditDlg::populateTypes()
{

    QList<QPair<QString, COValue::Type>> types;

    types << qMakePair(tr("I32"), COValue::I32)
          << qMakePair(tr("I16"), COValue::I16)
          << qMakePair(tr("I8"), COValue::I8)
          << qMakePair(tr("U32"), COValue::U32)
          << qMakePair(tr("U16"), COValue::U16)
          << qMakePair(tr("U8"), COValue::U8)
          << qMakePair(tr("IQ24"), COValue::IQ24)
          << qMakePair(tr("IQ15"), COValue::IQ15)
          << qMakePair(tr("IQ7"), COValue::IQ7);

    ui->cbType->clear();

    for(auto& type: types){
        ui->cbType->addItem(type.first, static_cast<int>(type.second));
    }
}

void SignalCurveEditDlg::populatePenStyles()
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

void SignalCurveEditDlg::populateBrushStyles()
{
    QList<QPair<QString, Qt::BrushStyle>> styles;

    styles << qMakePair(tr("Нет"), Qt::NoBrush)
           << qMakePair(tr("Сплошная"), Qt::SolidPattern);
           //<< qMakePair(tr(""), Qt::Dense1Pattern)
           //<< qMakePair(tr(""), Qt::Dense2Pattern)
           //<< qMakePair(tr(""), Qt::Dense3Pattern)
           //<< qMakePair(tr(""), Qt::Dense4Pattern)
           //<< qMakePair(tr(""), Qt::Dense5Pattern)
           //<< qMakePair(tr(""), Qt::Dense6Pattern)
           //<< qMakePair(tr(""), Qt::Dense7Pattern)
           //<< qMakePair(tr(""), Qt::HorPattern)
           //<< qMakePair(tr(""), Qt::VerPattern)
           //<< qMakePair(tr(""), Qt::CrossPattern)
           //<< qMakePair(tr(""), Qt::BDiagPattern)
           //<< qMakePair(tr(""), Qt::FDiagPattern)
           //<< qMakePair(tr(""), Qt::DiagCrossPattern)
           //<< qMakePair(tr(""), Qt::LinearGradientPattern)
           //<< qMakePair(tr(""), Qt::ConicalGradientPattern)
           //<< qMakePair(tr(""), Qt::RadialGradientPattern)
           //<< qMakePair(tr(""), Qt::TexturePattern);

    ui->cbBrushStyle->clear();

    for(auto& style: styles){
        ui->cbBrushStyle->addItem(style.first, static_cast<int>(style.second));
    }
}
