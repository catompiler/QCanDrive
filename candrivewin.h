#ifndef CANDRIVEWIN_H
#define CANDRIVEWIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CanDriveWin; }
QT_END_NAMESPACE

class QLabel;
class SettingsDlg;
class SLCanOpenNode;
class CoValuesHolder;


class CanDriveWin : public QMainWindow
{
    Q_OBJECT

public:
    CanDriveWin(QWidget *parent = nullptr);
    ~CanDriveWin();

private slots:
    void CANopen_connected();
    void CANopen_disconnected();

    void m_ui_actQuit_triggered(bool checked);
    void m_ui_actDebugExec_triggered(bool checked);
    void m_ui_actConnect_triggered(bool checked);
    void m_ui_actDisconnect_triggered(bool checked);
    void m_ui_actSettings_triggered(bool checked);

private:
    Ui::CanDriveWin *ui;
    QLabel* m_sblblConStatus;
    SettingsDlg* m_settingsDlg;

    SLCanOpenNode* m_slcon;
    CoValuesHolder* m_valsHolder;

    void updateStatusBar();
    void applySettings();
};
#endif // CANDRIVEWIN_H
