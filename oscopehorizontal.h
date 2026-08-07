#ifndef OSCOPEHORIZONTAL_H
#define OSCOPEHORIZONTAL_H

#include <QtGlobal>


class OScopeHorizontal
{
public:
    static constexpr qreal MIN_HDIV = 1e-6;
    static constexpr qreal MAX_HDIV = 1e6;
    static constexpr qreal DEF_HDIV = 1e0;

    OScopeHorizontal();
    ~OScopeHorizontal();

    qreal hDiv() const;
    void setHDiv(qreal newHDiv);

    qreal hOffset() const;
    void setHOffset(qreal newHOffset);

    qreal invHDiv() const;

private:
    qreal m_hDiv;
    qreal m_hOffset;

    qreal m_invHDiv;
};

#endif // OSCOPEHORIZONTAL_H
