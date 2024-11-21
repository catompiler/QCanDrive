#include "regdelegate.h"
#include "flagseditdlg.h"
#include "reglistmodel.h"
#include "regselectdlg.h"
#include "sellineedit.h"
#include "regtypes.h"
#include "regutils.h"
#include "regentry.h"
#include "regvar.h"
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QString>
#include <QDebug>



RegDelegate::RegDelegate()
{
    m_regSelectDialog = nullptr;
}

RegDelegate::~RegDelegate()
{

}

QWidget* RegDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);

    //qDebug() << "RegVarDelegate::createEditor";

    if(!index.isValid()) return nullptr;

    const RegListModel* regListModel = qobject_cast<const RegListModel*>(index.model());

    if(regListModel == nullptr) return nullptr;

    RegObject* ro = regListModel->objectByModelIndex(index);

    if(ro == nullptr) return nullptr;

    bool isEntry = ro->parent() == nullptr;

    RegEntry* re = (isEntry) ? static_cast<RegEntry*>(ro) : static_cast<RegEntry*>(ro->parent());
    RegVar* rv = (isEntry) ? nullptr : static_cast<RegVar*>(ro);

    QWidget* res_widget = nullptr;

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:{
        QSpinBox* sb = new QSpinBox(parent);
        if(isEntry){
            sb->setMinimum(0);
            sb->setMaximum(65536);
            sb->setDisplayIntegerBase(16);
            sb->setPrefix("0x");
        }else{
            sb->setMinimum(0);
            sb->setMaximum(255);
            sb->setDisplayIntegerBase(10);
        }
        res_widget = sb;
    }break;
    case RegListModel::COL_NAME:{
        QLineEdit* le = new QLineEdit(parent);
        res_widget = le;
    }break;
    case RegListModel::COL_TYPE:{
        if(isEntry){
            QComboBox* cb = new QComboBox(parent);
            auto objTypes = RegTypes::objectTypes();
            for(auto& t: qAsConst(objTypes)){
                cb->addItem(RegTypes::typeStr(t));
            }
            res_widget = cb;
        }else{
            QComboBox* cb = new QComboBox(parent);
            auto dataTypes = RegTypes::dataTypes();
            if(re->type() == ObjectType::ARR){
                dataTypes.removeAll(DataType::MEM);
                dataTypes.removeAll(DataType::STR);
            }
            for(auto& t: qAsConst(dataTypes)){
                cb->addItem(RegTypes::dataTypeStr(t));
            }
            res_widget = cb;
        }
    }break;
    case RegListModel::COL_COUNT:{
        QSpinBox* sb = new QSpinBox(parent);
        if(rv && RegTypes::isMemory(rv->dataType())){
            sb->setMinimum(0);
        }else{
            sb->setMinimum(1);
        }
        sb->setMaximum(254);
        res_widget = sb;
    }break;
    case RegListModel::COL_MEM_ADDR:{
        QLineEdit* le = new QLineEdit(parent);
        le->setPlaceholderText(tr("Автоматически"));
        res_widget = le;
    }break;
    case RegListModel::COL_MIN_VAL:
    case RegListModel::COL_MAX_VAL:
    case RegListModel::COL_DEF_VAL:{
        // Var.
        if(ro->parent() != nullptr){
            RegVar* var = static_cast<RegVar*>(ro);
            DataType data_type = var->dataType();
            switch(data_type){
            case DataType::I32:
            case DataType::I16:
            case DataType::I8:
            case DataType::U32:
            case DataType::U16:
            case DataType::U8:
            case DataType::IQ24:
            case DataType::IQ15:
            case DataType::IQ7:
            case DataType::STR:
            case DataType::MEM:
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
                break;
            }
        }else{
            QLineEdit* le = new QLineEdit(parent);
            res_widget = le;
        }
    }break;
    case RegListModel::COL_BASE:{
        SelLineEdit* le = new SelLineEdit(parent);
        connect(le, &SelLineEdit::select, this, &RegDelegate::selectReg);
        connect(le, &SelLineEdit::editingFinished, this, &RegDelegate::editingFinished);
        res_widget = le;
    }break;
    case RegListModel::COL_FLAGS:{
        SelLineEdit* le = new SelLineEdit(parent);
        connect(le, &SelLineEdit::select, this, &RegDelegate::selectFlags);
        connect(le, &SelLineEdit::editingFinished, this, &RegDelegate::editingFinished);
        res_widget = le;
    }break;
    case RegListModel::COL_EXTFLAGS:{
        SelLineEdit* le = new SelLineEdit(parent);
        connect(le, &SelLineEdit::select, this, &RegDelegate::selectEFlags);
        connect(le, &SelLineEdit::editingFinished, this, &RegDelegate::editingFinished);
        res_widget = le;
    }break;
    case RegListModel::COL_DESCR:{
        QLineEdit* le = new QLineEdit(parent);
        res_widget = le;
    }break;
    }

    return res_widget;
}

void RegDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    const RegListModel* regListModel = qobject_cast<const RegListModel*>(index.model());

    if(regListModel == nullptr) return;

    RegObject* ro = regListModel->objectByModelIndex(index);

    if(ro == nullptr) return;

    //bool isEntry = ro->parent() == nullptr;

    QVariant data = index.data(Qt::EditRole); //regListModel->data(index, Qt::EditRole);

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:{
        QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
        if(sb == nullptr) break;
        sb->setValue(data.toInt());
    }break;
    case RegListModel::COL_NAME:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    case RegListModel::COL_TYPE:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        cb->setCurrentIndex(static_cast<int>(data.toInt()));
    }break;
    case RegListModel::COL_COUNT:{
        QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
        if(sb == nullptr) break;
        sb->setValue(data.toInt());
    }break;
    case RegListModel::COL_MEM_ADDR:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    case RegListModel::COL_MIN_VAL:
    case RegListModel::COL_MAX_VAL:
    case RegListModel::COL_DEF_VAL:{
        // Var.
        if(ro->parent() != nullptr){
            RegVar* var = static_cast<RegVar*>(ro);
            DataType data_type = var->dataType();
            switch(data_type){
            case DataType::I32:
            case DataType::I16:
            case DataType::I8:
            case DataType::U32:
            case DataType::U16:
            case DataType::U8:
            case DataType::IQ24:
            case DataType::IQ15:
            case DataType::IQ7:
            case DataType::STR:
            case DataType::MEM:
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                le->setText(data.toString());
                break;
            }
        }else{
            QLineEdit* le = qobject_cast<QLineEdit*>(editor);
            if(le == nullptr) break;
            le->setText(data.toString());
        }
    }break;
    case RegListModel::COL_BASE:{
        SelLineEdit* le = qobject_cast<SelLineEdit*>(editor);
        if(le == nullptr) break;
        switch(ro->type()){
        default:
            le->setText(data.toString());
            break;
        case ObjectType::VAR:{
            unsigned int base = data.toUInt();
            unsigned int base_index = base >> 8;
            unsigned int base_subindex = base & 0xff;
            le->setText(RegUtils::indexSubIndexToString(base_index, base_subindex));
            break;
        }
        }
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        SelLineEdit* le = qobject_cast<SelLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(QString("0b%1").arg(data.toUInt(), 0, 2));
    }break;
    case RegListModel::COL_DESCR:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    }
}

void RegDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    RegListModel* regListModel = qobject_cast<RegListModel*>(model);

    if(regListModel == nullptr) return;

    RegObject* ro = regListModel->objectByModelIndex(index);

    if(ro == nullptr) return;

    //bool isEntry = ro->parent() == nullptr;

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:{
        QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
        if(sb == nullptr) break;
        regListModel->setData(index, sb->value(), Qt::EditRole);
    }break;
    case RegListModel::COL_NAME:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        if(le->text().isEmpty()) break;
        regListModel->setData(index, le->text(), Qt::EditRole);
    }break;
    case RegListModel::COL_TYPE:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        regListModel->setData(index, cb->currentIndex(), Qt::EditRole);
    }break;
    case RegListModel::COL_COUNT:{
        QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
        if(sb == nullptr) break;
        regListModel->setData(index, sb->value(), Qt::EditRole);
    }break;
    case RegListModel::COL_MEM_ADDR:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        regListModel->setData(index, le->text(), Qt::EditRole);
    }break;
    case RegListModel::COL_MIN_VAL:
    case RegListModel::COL_MAX_VAL:
    case RegListModel::COL_DEF_VAL:{
        // Var.
        if(ro->parent() != nullptr){
            RegVar* var = static_cast<RegVar*>(ro);
            DataType data_type = var->dataType();
            switch(data_type){
            case DataType::I32:
            case DataType::I16:
            case DataType::I8:
            case DataType::U32:
            case DataType::U16:
            case DataType::U8:
            case DataType::IQ24:
            case DataType::IQ15:
            case DataType::IQ7:
            case DataType::STR:
            case DataType::MEM:
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                regListModel->setData(index, le->text(), Qt::EditRole);
                break;
            }
        }else{
            QLineEdit* le = qobject_cast<QLineEdit*>(editor);
            if(le == nullptr) break;
            regListModel->setData(index, le->text(), Qt::EditRole);
        }
    }break;
    case RegListModel::COL_BASE:{
        SelLineEdit* le = qobject_cast<SelLineEdit*>(editor);
        if(le == nullptr) break;

        bool ok = false;
        auto index_pair = RegUtils::indexSubIndexFromString(le->text(), &ok);

        if(ok){
            unsigned int base = (index_pair.first << 8) | index_pair.second;
            regListModel->setData(index, base, Qt::EditRole);
        }
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        SelLineEdit* le = qobject_cast<SelLineEdit*>(editor);
        if(le == nullptr) break;
        bool ok = false;
        int base = 0;
        QString str_val = le->text().trimmed();
        if(str_val.startsWith("0b")){
            str_val = str_val.mid(2);
            base = 2;
        }
        unsigned int flags = str_val.toUInt(&ok, base);
        if(ok) regListModel->setData(index, flags, Qt::EditRole);
    }break;
    case RegListModel::COL_DESCR:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        regListModel->setData(index, le->text(), Qt::EditRole);
    }break;
    }

    /*
    DataType data_type = DataType::INTEGER32;

    switch(data_type){
    case DataType::BOOLEAN:
    case DataType::INTEGER8:
    case DataType::INTEGER16:
    case DataType::INTEGER32:
    case DataType::INTEGER64:
    case DataType::UNSIGNED8:
    case DataType::UNSIGNED16:
    case DataType::UNSIGNED32:
    case DataType::UNSIGNED64:
    case DataType::REAL32:
    case DataType::REAL64:
    case DataType::VISIBLE_STRING:
    case DataType::OCTET_STRING:
    case DataType::UNICODE_STRING:
    case DataType::DOMAIN:
    default:
        QItemDelegate::setEditorData(editor, index);
        break;
    }
    */
}

void RegDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}

RegSelectDlg* RegDelegate::regSelectDialog() const
{
    return m_regSelectDialog;
}

void RegDelegate::setRegSelectDialog(RegSelectDlg* newRegSelectDialog)
{
    m_regSelectDialog = newRegSelectDialog;
}

FlagsEditDlg* RegDelegate::flagsEditDialog() const
{
    return m_flagsEditDialog;
}

void RegDelegate::setFlagsEditDialog(FlagsEditDlg* newFlagsEditDialog)
{
    m_flagsEditDialog = newFlagsEditDialog;
}

void RegDelegate::selectReg()
{
    SelLineEdit* le = qobject_cast<SelLineEdit*>(sender());
    if(le == nullptr) return;

    if(m_regSelectDialog == nullptr) return;

    bool ok = false;
    auto index_pair = RegUtils::indexSubIndexFromString(le->text(), &ok);

    if(ok){
        m_regSelectDialog->selectReg(index_pair.first, index_pair.second);
    }

    if(m_regSelectDialog->exec()){
        if(m_regSelectDialog->hasSelectedReg()){
            auto sel_pair = m_regSelectDialog->selectedRegIndex();
            le->setText(RegUtils::indexSubIndexToString(sel_pair.first, sel_pair.second));
        }
    }
}

void RegDelegate::selectFlags()
{
    SelLineEdit* le = qobject_cast<SelLineEdit*>(sender());
    if(le == nullptr) return;
    if(m_flagsEditDialog == nullptr) return;

    bool ok = false;
    int base = 0;
    QString str_val = le->text().trimmed();
    if(str_val.startsWith("0b")){
        str_val = str_val.mid(2);
        base = 2;
    }
    uint flags = str_val.toUInt(&ok, base);

    if(ok) m_flagsEditDialog->setFlagsValues(flags);
    m_flagsEditDialog->setFlagsNames(RegTypes::flagsNames().mid(1));

    if(m_flagsEditDialog->exec()){
        le->setText(QString("0b%1").arg(m_flagsEditDialog->flagsValues(), 0, 2));
    }
}

void RegDelegate::selectEFlags()
{
    SelLineEdit* le = qobject_cast<SelLineEdit*>(sender());
    if(le == nullptr) return;
    if(m_flagsEditDialog == nullptr) return;

    bool ok = false;
    int base = 0;
    QString str_val = le->text().trimmed();
    if(str_val.startsWith("0b")){
        str_val = str_val.mid(2);
        base = 2;
    }
    uint flags = str_val.toUInt(&ok, base);

    if(ok) m_flagsEditDialog->setFlagsValues(flags);
    m_flagsEditDialog->setFlagsNames(RegTypes::eflagsNames().mid(1));

    if(m_flagsEditDialog->exec()){
        le->setText(QString("0b%1").arg(m_flagsEditDialog->flagsValues(), 0, 2));
    }
}

void RegDelegate::editingFinished()
{
    QWidget* w = qobject_cast<QWidget*>(sender());
    if(w == nullptr) return;

    emit commitData(w);
    emit closeEditor(w, QAbstractItemDelegate::NoHint);
}
