#include "regdelegate.h"
#include "reglistmodel.h"
#include "regtypes.h"
#include "regvar.h"
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QStringList>
#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>



RegDelegate::RegDelegate()
{

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

    QWidget* res_widget = nullptr;

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:{
        QSpinBox* sb = new QSpinBox(parent);
        sb->setMinimum(0);
        sb->setMaximum(65536);
        sb->setDisplayIntegerBase(16);
        sb->setPrefix("0x");
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
            for(auto t: qAsConst(objTypes)){
                cb->addItem(RegTypes::typeStr(t));
            }
            res_widget = cb;
        }else{
            QComboBox* cb = new QComboBox(parent);
            auto dataTypes = RegTypes::dataTypes();
            for(auto t: qAsConst(dataTypes)){
                cb->addItem(RegTypes::dataTypeStr(t));
            }
            res_widget = cb;
        }
    }break;
    case RegListModel::COL_COUNT:{
        QSpinBox* sb = new QSpinBox(parent);
        sb->setMinimum(0);
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
            case DataType::BOOLEAN:{
                QComboBox* cb = new QComboBox(parent);
                cb->addItems(RegTypes::boolStringList());
                res_widget = cb;
            }break;
            case DataType::INTEGER8:
            case DataType::INTEGER16:
            case DataType::INTEGER32:
            case DataType::INTEGER64:
            case DataType::UNSIGNED8:
            case DataType::UNSIGNED16:
            case DataType::UNSIGNED32:
            case DataType::UNSIGNED64:{
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
            }break;
            case DataType::REAL32:
            case DataType::REAL64:{
                QDoubleSpinBox* sb = new QDoubleSpinBox(parent);
                sb->setMinimum(INT32_MIN);
                sb->setMaximum(INT32_MAX);
                res_widget = sb;
            }break;
            case DataType::VISIBLE_STRING:
            case DataType::OCTET_STRING:
            case DataType::UNICODE_STRING:{
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
            }break;
            default:
            case DataType::DOMAIN:{
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
            }break;
            }
        }else{
            QLineEdit* le = new QLineEdit(parent);
            res_widget = le;
        }
    }break;
    case RegListModel::COL_BASE:{
        QLineEdit* le = new QLineEdit(parent);
        res_widget = le;
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        QLineEdit* le = new QLineEdit(parent);
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
            case DataType::BOOLEAN:{
                QComboBox* cb = qobject_cast<QComboBox*>(editor);
                if(cb == nullptr) break;
                cb->setCurrentIndex(static_cast<int>(data.toBool()));
            }break;
            case DataType::INTEGER8:
            case DataType::INTEGER16:
            case DataType::INTEGER32:
            case DataType::INTEGER64:
            case DataType::UNSIGNED8:
            case DataType::UNSIGNED16:
            case DataType::UNSIGNED32:
            case DataType::UNSIGNED64:{
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                le->setText(data.toString());
            }break;
            case DataType::REAL32:
            case DataType::REAL64:{
                QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(editor);
                if(sb == nullptr) break;
                sb->setValue(data.toDouble());
            }break;
            case DataType::VISIBLE_STRING:
            case DataType::OCTET_STRING:
            case DataType::UNICODE_STRING:{
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                le->setText(data.toString());
            }break;
            default:
            case DataType::DOMAIN:{
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                le->setText(data.toString());
            }break;
            }
        }else{
            QLineEdit* le = qobject_cast<QLineEdit*>(editor);
            if(le == nullptr) break;
            le->setText(data.toString());
        }
    }break;
    case RegListModel::COL_BASE:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        switch(ro->type()){
        default:
            le->setText(data.toString());
            break;
        case ObjectType::VAR:{
            unsigned int base = data.toUInt();
            unsigned int base_index = base >> 8;
            unsigned int base_subindex = base & 0xff;
            le->setText(
                        QString("0x%1.%2")
                            .arg(base_index, 4, 16, QChar('0'))
                            .arg(base_subindex, 2, 16, QChar('0'))
                        );
            break;
        }
        }
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
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
        if(le->text().isEmpty()) break;
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
            case DataType::BOOLEAN:{
                QComboBox* cb = qobject_cast<QComboBox*>(editor);
                if(cb == nullptr) break;
                regListModel->setData(index, cb->currentIndex(), Qt::EditRole);
            }break;
            case DataType::INTEGER8:
            case DataType::INTEGER16:
            case DataType::INTEGER32:
            case DataType::INTEGER64:
            case DataType::UNSIGNED8:
            case DataType::UNSIGNED16:
            case DataType::UNSIGNED32:
            case DataType::UNSIGNED64:{
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                regListModel->setData(index, le->text(), Qt::EditRole);
            }break;
            case DataType::REAL32:
            case DataType::REAL64:{
                QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(editor);
                if(sb == nullptr) break;
                regListModel->setData(index, sb->value(), Qt::EditRole);
            }break;
            case DataType::VISIBLE_STRING:
            case DataType::OCTET_STRING:
            case DataType::UNICODE_STRING:{
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                regListModel->setData(index, le->text(), Qt::EditRole);
            }break;
            default:
            case DataType::DOMAIN:{
                QLineEdit* le = qobject_cast<QLineEdit*>(editor);
                if(le == nullptr) break;
                regListModel->setData(index, le->text(), Qt::EditRole);
            }break;
            }
        }else{
            QLineEdit* le = qobject_cast<QLineEdit*>(editor);
            if(le == nullptr) break;
            regListModel->setData(index, le->text(), Qt::EditRole);
        }
    }break;
    case RegListModel::COL_BASE:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;

        QString base_str = le->text();

        bool ok_index = false, ok_subindex = false;
        unsigned int base_index = base_str.section(QChar('.'), 0, 0).toUInt(&ok_index, 16);
        unsigned int base_subindex = base_str.section(QChar('.'), 1, 1).toUInt(&ok_subindex, 16);

        if(ok_index && ok_subindex){
            unsigned int base = (base_index << 8) | base_subindex;
            regListModel->setData(index, base, Qt::EditRole);
        }
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        bool ok = false;
        int base = 10;
        QString str_val = le->text().trimmed();
        if(str_val.startsWith("0x")){
            str_val = str_val.mid(2);
            base = 16;
        }else if(str_val.startsWith("0b")){
            str_val = str_val.mid(2);
            base = 2;
        }else if(str_val.startsWith("0")){
            base = 8;
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
