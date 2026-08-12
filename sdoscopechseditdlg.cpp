#include "sdoscopechseditdlg.h"
#include "ui_sdoscopechseditdlg.h"
#include "sdoscopechanneldelegate.h"



#define COL_WIDTH_ENABLED 75
#define COL_WIDTH_NAME 100
#define COL_WIDTH_REG 100
#define COL_WIDTH_DATA_TYPE 75
#define COL_WIDTH_BASE_VALUE 100
//#define COL_WIDTH_ 50


static const int col_width[] = {
    COL_WIDTH_ENABLED,
    COL_WIDTH_NAME,
    COL_WIDTH_REG,
    COL_WIDTH_DATA_TYPE,
    COL_WIDTH_BASE_VALUE
};
static const int col_width_len = ((sizeof(col_width))/(sizeof(col_width[0])));


SDOScopeChsEditDlg::SDOScopeChsEditDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SDOScopeChsEditDlg)
{
    ui->setupUi(this);

    m_channelsModel = new SDOScopeChannelsModel();
    m_channelDelegate = new SDOScopeChannelDelegate();
    ui->tvChannels->setModel(m_channelsModel);
    ui->tvChannels->setItemDelegate(m_channelDelegate);

    for(int i = 0; i < col_width_len; i ++){
        ui->tvChannels->setColumnWidth(i, col_width[i]);
    }
}

SDOScopeChsEditDlg::~SDOScopeChsEditDlg()
{
    delete m_channelDelegate;
    delete m_channelsModel;
    delete ui;
}

void SDOScopeChsEditDlg::setChannelsData(ChannelsData newChannelsData)
{
    m_channelsModel->setChannelsData(newChannelsData);
}

RegSelectDlg* SDOScopeChsEditDlg::regSelectDialog() const
{
    return m_channelDelegate->regSelectDialog();
}

void SDOScopeChsEditDlg::setRegSelectDialog(RegSelectDlg* newRegSelectDialog)
{
    m_channelDelegate->setRegSelectDialog(newRegSelectDialog);
}

const SDOScopeChsEditDlg::ChannelsData& SDOScopeChsEditDlg::channelsData() const
{
    return m_channelsModel->channelsData();
}
