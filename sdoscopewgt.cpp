#include "sdoscopewgt.h"
#include "ui_sdoscopewgt.h"
#include <QPushButton>
#include "oscplot.h"
#include "oscplotdata.h"
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

    size_t samplesCount = m_scope->samplesCount();

    for(uint i = 0; i < samplesCount; i ++){
        // Данные сигнала.
        OscPlotData* pltData = new OscPlotData(m_scope, i);
        // Добавим сигнал.
        int sig_n = plt->addSignal(pltData);
        // При ошибке - отрицательный номер.
        if(sig_n < 0){
            QMessageBox::critical(this, tr("Предупреждение!"), tr("Ощибка добавления сигнала %1 в осциллограф!").arg(i));
            break;
        }

        // Данные ещё не читались - не показывать канал.
        plt->setSignalVisible(sig_n, false);

        // Первоначальная настройка.
        plt->setBrush(sig_n, QBrush(Qt::NoBrush));
    }
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
    auto plt = getPlot();

    size_t samplesCount = m_scope->samplesCount();

    for(uint i = 0; i < samplesCount; i ++){
        // Данные канала.
        OscPlotData* pltData = plt->plotData(i);
        // Обновим.
        if(pltData) pltData->update();

        // Канал.
        SDOScope::Channel* ch = m_scope->channel(i);
        // Если указатель не NULL
        // и канал включен - показать сигнал.
        plt->setSignalVisible(i, (ch && ch->enabled()));
    }

    plt->replot();
}

OscPlot* SDOScopeWgt::getPlot()
{
    return ui->oscplt;
}

const OscPlot* SDOScopeWgt::getPlot() const
{
    return ui->oscplt;
}

void SDOScopeWgt::refreshUi()
{
}
