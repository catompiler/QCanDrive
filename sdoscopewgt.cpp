#include "sdoscopewgt.h"
#include "ui_sdoscopewgt.h"
#include "sdoscopechseditdlg.h"
#include <QPushButton>
#include "oscopeplot.h"
#include "oscopeplotseriesdata.h"
#include "oscopedata.h"
#include "sdoscope.h"
#include "slcanopennode.h"
#include "reglistmodel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QDebug>



SDOScopeWgt::SDOScopeWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SDOScopeWgt)
{
    ui->setupUi(this);

    m_chs_edit_dlg = new SDOScopeChsEditDlg();

    m_initialized = false;

    // Осциллограф.
    m_scope = new SDOScope();
    // Коннекты осциллографа.
    connect(m_scope, &SDOScope::initialized, this, &SDOScopeWgt::sdoscopeInitialized);
    connect(m_scope, &SDOScope::updated, this, &SDOScopeWgt::sdoscopeUpdated);
    connect(m_scope, &SDOScope::errorOccured, this, &SDOScopeWgt::sdoscopeErrorOccured);
    connect(m_scope, &SDOScope::done, this, &SDOScopeWgt::sdoscopeDone);
    connect(m_scope, &SDOScope::readed, this, &SDOScopeWgt::sdoscopeReaded);

    // Коннекты виджета.
    connect(ui->pbSingle, &QPushButton::clicked, this, &SDOScopeWgt::single);
}

SDOScopeWgt::~SDOScopeWgt()
{
    delete m_chs_edit_dlg;
    delete m_scope;
    delete ui;
}

const RegListModel* SDOScopeWgt::regListModel() const
{
    return m_scope->regListModel();
}

RegListModel* SDOScopeWgt::regListModel()
{
    return m_scope->regListModel();
}

void SDOScopeWgt::setRegListModel(RegListModel* newRegListModel)
{
     m_scope->setRegListModel(newRegListModel);
}

SLCanOpenNode* SDOScopeWgt::getSLCanOpenNode()
{
     return m_scope->getSLCanOpenNode();
}

void SDOScopeWgt::setSLCanOpenNode(SLCanOpenNode* slcon)
{
    m_scope->setSLCanOpenNode(slcon);
}

void SDOScopeWgt::connected()
{
    qDebug() << "SDOScopeWgt::connected()";

    if(!m_scope->init()){
        qDebug() << "SDOScope init failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка инициализации осциллографа!"));
    }
}

void SDOScopeWgt::disconnected()
{
    qDebug() << "SDOScopeWgt::disconnected()";

    m_initialized = false;
}

void SDOScopeWgt::single()
{
    if(!m_initialized) return;

    if(!m_scope->run()){
        qDebug() << "SDOScope run failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка запуска осциллографа!"));
    }
}

void SDOScopeWgt::sdoscopeInitialized()
{
    qDebug() << "SDOScopeWgt::sdoscopeInitialized()";

    m_initialized = true;

    if(!m_scope->update()){
        qDebug() << "SDOScope update failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка обновления осциллографа!"));
    }
}

void SDOScopeWgt::sdoscopeUpdated()
{
    qDebug() << "SDOScopeWgt::sdoscopeUpdated()";

    // Заполним график каналами осциллографа.

    auto plt = getPlot();

    // Очистим каналы.
    plt->removeSignals();

    // ...
}

void SDOScopeWgt::sdoscopeErrorOccured()
{
    qDebug() << "SDOScopeWgt::sdoscopeErrorOccured()" << static_cast<int>(m_scope->error());

    QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка работы осциллографа!\n%1").arg(static_cast<int>(m_scope->error())));
}

void SDOScopeWgt::sdoscopeDone()
{
    if(!m_scope->read()){
        qDebug() << "SDOScope read failed";

        QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка чтения осциллографа!"));
    }
}

void SDOScopeWgt::sdoscopeReaded()
{
    // Обновим график.

    auto plt = getPlot();

    plt->replot();
}

void SDOScopeWgt::on_tbChannels_clicked(bool checked)
{
    Q_UNUSED(checked)

    qDebug() << "on_tbChannels_clicked";

    if(m_chs_edit_dlg->exec()){
        //
    }
}

OScopePlot* SDOScopeWgt::getPlot()
{
    return ui->oscplt;
}

const OScopePlot* SDOScopeWgt::getPlot() const
{
    return ui->oscplt;
}

void SDOScopeWgt::refreshUi()
{
}
