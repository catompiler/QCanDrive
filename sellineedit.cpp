#include "sellineedit.h"
#include <QLineEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QDebug>



SelLineEdit::SelLineEdit(QWidget *parent)
    : QWidget{parent}
{
    createUi();

    setFocusProxy(m_lineEdit);

    connect(m_selButton, &QToolButton::clicked, this, &SelLineEdit::select);
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

void SelLineEdit::createUi()
{
    m_lineEdit = new QLineEdit();

    m_selButton = new QToolButton();
    m_selButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_selButton->setText("...");
    m_selButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_selButton->setSizeIncrement(1, 1);

    m_lMain = new QHBoxLayout();
    m_lMain->setContentsMargins(0, 0, 0, 0);
    m_lMain->setSpacing(0);
    m_lMain->setMargin(0);

    m_lMain->addWidget(m_lineEdit);
    m_lMain->addWidget(m_selButton);

    setLayout(m_lMain);
}
