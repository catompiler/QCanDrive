#include "oscopehorizontal.h"
#include <algorithm>


OScopeHorizontal::OScopeHorizontal()
{
    m_hOffset = 0.0;

    m_hDiv = DEF_HDIV;
    m_invHDiv = 1.0/DEF_HDIV;
}

OScopeHorizontal::~OScopeHorizontal()
{

}

qreal OScopeHorizontal::hDiv() const
{
    return m_hDiv;
}

void OScopeHorizontal::setHDiv(qreal newHDiv)
{
    m_hDiv = std::max(std::min(newHDiv,MAX_HDIV), MIN_HDIV);
    m_invHDiv = 1.0 / m_hDiv;
}

qreal OScopeHorizontal::hOffset() const
{
    return m_hOffset;
}

void OScopeHorizontal::setHOffset(qreal newHOffset)
{
    m_hOffset = newHOffset;
}

qreal OScopeHorizontal::invHDiv() const
{
    return m_invHDiv;
}
