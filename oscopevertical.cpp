#include "oscopevertical.h"
#include <algorithm>


OScopeVertical::OScopeVertical()
{
    m_vOffset = 0.0;

    m_vDiv = DEF_VDIV;
    m_invVDiv = 1.0/DEF_VDIV;
}

OScopeVertical::~OScopeVertical()
{

}

qreal OScopeVertical::vDiv() const
{
    return m_vDiv;
}

void OScopeVertical::setVDiv(qreal newVDiv)
{
    m_vDiv = std::max(std::min(newVDiv,MAX_VDIV), MIN_VDIV);
    m_invVDiv = 1.0 / m_vDiv;
}

qreal OScopeVertical::vOffset() const
{
    return m_vOffset;
}

void OScopeVertical::setVOffset(qreal newVOffset)
{
    m_vOffset = newVOffset;
}

qreal OScopeVertical::invVDiv() const
{
    return m_invVDiv;
}
