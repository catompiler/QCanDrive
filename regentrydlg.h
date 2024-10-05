#ifndef REGENTRYDLG_H
#define REGENTRYDLG_H

#include <QDialog>
#include "regtypes.h"

namespace Ui {
class RegEntryDlg;
}

class RegEntryDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RegEntryDlg(QWidget *parent = nullptr);
    ~RegEntryDlg();

    reg_index_t index() const;
    void setIndex(reg_index_t newIndex);

    ObjectType objectType() const;
    void setObjectType(ObjectType newObjectType);

    QString name() const;
    void setName(const QString &newName);

    QString description() const;
    void setDescription(const QString &newDescription);

private:
    Ui::RegEntryDlg *ui;

    void populateTypesList();
};

#endif // REGENTRYDLG_H
