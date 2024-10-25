#include "sellineedit.h"
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>


SelLineEdit::SelLineEdit(QWidget *parent)
    : QWidget{parent}
{
    createUi();

    //setFocusPolicy(Qt::NoFocus);
    setFocusProxy(m_lineEdit);

    m_lineEdit->installEventFilter(this);

    connect(m_selButton, &QToolButton::clicked, this, &SelLineEdit::select);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &SelLineEdit::editingFinished);
}

SelLineEdit::~SelLineEdit()
{
    delete m_selButton;
    delete m_lineEdit;
    delete m_lMain;
}

QString SelLineEdit::text() const
{
    return m_lineEdit->text();
}

void SelLineEdit::setText(const QString& newText)
{
    m_lineEdit->setText(newText);
}

QString SelLineEdit::placeholderText() const
{
    return m_lineEdit->placeholderText();
}

void SelLineEdit::setPlaceholderText(const QString& newPlaceholderText)
{
    m_lineEdit->setPlaceholderText(newPlaceholderText);
}

void SelLineEdit::createUi()
{
    m_lineEdit = new QLineEdit();

    m_selButton = new QToolButton();
    m_selButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_selButton->setText("...");

    m_lMain = new QHBoxLayout();
    m_lMain->setContentsMargins(0, 0, 0, 0);
    m_lMain->setSpacing(0);
    m_lMain->setMargin(0);

    m_lMain->addWidget(m_lineEdit);
    m_lMain->addWidget(m_selButton);

    setLayout(m_lMain);
}


bool SelLineEdit::eventFilter(QObject* watched, QEvent* event)
{
    //qDebug() << "SelLineEdit::eventFilter" << watched << event << event->type() <<
    //            "focus:" << qApp->focusWidget() << isActiveWindow();

    if(!isActiveWindow()){
        event->accept();
        return true;
    }

    return QWidget::eventFilter(watched, event);
}
