#ifndef OSCOPEVERTICAL_H
#define OSCOPEVERTICAL_H

#include <QtGlobal>


class OScopeVertical
{
public:
    static constexpr qreal MIN_VDIV = 1e-6;
    static constexpr qreal MAX_VDIV = 1e6;
    static constexpr qreal DEF_VDIV = 1e0;

    OScopeVertical();
    ~OScopeVertical();

    qreal vDiv() const;
    void setVDiv(qreal newVDiv);

    qreal vOffset() const;
    void setVOffset(qreal newVOffset);

    qreal invVDiv() const;

private:
    qreal m_vDiv;
    qreal m_vOffset;

    qreal m_invVDiv;
};

#endif // OSCOPEVERTICAL_H
