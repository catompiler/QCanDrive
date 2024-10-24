#ifndef SELLINEEDIT_H
#define SELLINEEDIT_H

#include <QWidget>

class QLineEdit;
class QToolButton;
class QHBoxLayout;


class SelLineEdit : public QWidget
{
    Q_OBJECT
public:
    explicit SelLineEdit(QWidget *parent = nullptr);
    ~SelLineEdit();

    QString text() const;
    void setText(const QString& newText);

signals:
    void select();

private:
    QLineEdit* m_lineEdit;
    QToolButton* m_selButton;
    QHBoxLayout* m_lMain;

    void createUi();
};

#endif // SELLINEEDIT_H
