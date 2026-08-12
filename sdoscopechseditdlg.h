#ifndef SDOSCOPECHSEDITDLG_H
#define SDOSCOPECHSEDITDLG_H

#include <QDialog>
#include "sdoscopechannelsmodel.h"

class SDOScopeChannelDelegate;
class RegSelectDlg;


namespace Ui {
class SDOScopeChsEditDlg;
}

class SDOScopeChsEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOScopeChsEditDlg(QWidget *parent = nullptr);
    ~SDOScopeChsEditDlg();

    using ChannelData = SDOScopeChannelsModel::ChannelData;
    using ChannelsData = SDOScopeChannelsModel::ChannelsData;

    void setChannelsData(ChannelsData newChannelsData);
    const ChannelsData& channelsData() const;

    RegSelectDlg* regSelectDialog() const;
    void setRegSelectDialog(RegSelectDlg* newRegSelectDialog);

private:
    Ui::SDOScopeChsEditDlg *ui;
    SDOScopeChannelsModel* m_channelsModel;
    SDOScopeChannelDelegate* m_channelDelegate;
};

#endif // SDOSCOPECHSEDITDLG_H
