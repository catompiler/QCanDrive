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

    QString placeholderText() const;
    void setPlaceholderText(const QString& newPlaceholderText);

signals:
    void select();
    void editingFinished();

private:
    QLineEdit* m_lineEdit;
    QToolButton* m_selButton;
    QHBoxLayout* m_lMain;

    void createUi();

    // QObject interface
public:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // SELLINEEDIT_H
