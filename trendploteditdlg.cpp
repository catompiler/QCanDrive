#include "trendploteditdlg.h"
#include "ui_trendploteditdlg.h"
#include <QColorDialog>



TrendPlotEditDlg::TrendPlotEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrendPlotEditDlg)
{
    ui->setupUi(this);
}

TrendPlotEditDlg::~TrendPlotEditDlg()
{
    delete ui;
}

void TrendPlotEditDlg::on_slTransp_valueChanged(int value)
{
    if(value == 0){
        ui->lblTranspVal->setText(tr("Откл."));
    }else{
        ui->lblTranspVal->setText(QStringLiteral("%1%").arg(value));
    }
}

void TrendPlotEditDlg::on_tbColorSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    QPalette pal = ui->frBackColor->palette();
    QColor curCol = pal.window().color();
    QColor col = QColorDialog::getColor(curCol, this, tr("Выбор цвета фона"));
    if(col.isValid()){
        pal.setColor(QPalette::Window, col);
        ui->frBackColor->setPalette(pal);
    }
}
