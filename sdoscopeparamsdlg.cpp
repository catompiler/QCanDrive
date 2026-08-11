#include "sdoscopeparamsdlg.h"
#include "ui_sdoscopeparamsdlg.h"

SDOScopeParamsDlg::SDOScopeParamsDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SDOScopeParamsDlg)
{
    ui->setupUi(this);
    // connect(ui->sbSamples, QSpinBox::valueChanged, this, &SDOScopeParamsDlg::sbSamplesCount_valueChanged);
}

SDOScopeParamsDlg::~SDOScopeParamsDlg()
{
    delete ui;
}

uint SDOScopeParamsDlg::prescaler() const
{
    return ui->sbPrescaler->value();
}

void SDOScopeParamsDlg::setPrescaler(uint newPrescaler)
{
    ui->sbPrescaler->setValue(newPrescaler);
}

uint SDOScopeParamsDlg::samplesCount() const
{
    return ui->sbSamples->value();
}

void SDOScopeParamsDlg::setSamplesCount(uint newSamplesCount)
{
    ui->sbSamples->setValue(newSamplesCount);
}

uint SDOScopeParamsDlg::histSamplesCount() const
{
    return ui->sbHistory->value();
}

void SDOScopeParamsDlg::setHistSamplesCount(uint newHistSamplesCount)
{
    ui->sbHistory->setValue(newHistSamplesCount);
}

void SDOScopeParamsDlg::setMaxSamplesCount(uint newMaxSamplesCount)
{
    ui->sbSamples->setMaximum(newMaxSamplesCount);
    ui->sbHistory->setMaximum(newMaxSamplesCount);
}

// void SDOScopeParamsDlg::sbSamplesCount_valueChanged(int value)
// {
//     ui->sbHistory->setMaximum(value);
// }
