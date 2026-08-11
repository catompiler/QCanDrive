#ifndef SDOSCOPEPARAMSDLG_H
#define SDOSCOPEPARAMSDLG_H

#include <QDialog>

namespace Ui {
class SDOScopeParamsDlg;
}

class SDOScopeParamsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOScopeParamsDlg(QWidget *parent = nullptr);
    ~SDOScopeParamsDlg();

    uint prescaler() const;
    void setPrescaler(uint newPrescaler);

    uint samplesCount() const;
    void setSamplesCount(uint newSamplesCount);

    uint histSamplesCount() const;
    void setHistSamplesCount(uint newHistSamplesCount);

    void setMaxSamplesCount(uint newMaxSamplesCount);

private slots:
    // void sbSamplesCount_valueChanged(int value);

private:
    Ui::SDOScopeParamsDlg *ui;
};

#endif // SDOSCOPEPARAMSDLG_H
