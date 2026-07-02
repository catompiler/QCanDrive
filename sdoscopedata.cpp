#include "sdoscopedata.h"
#include "sdoscope.h"


SDOScopeData::SDOScopeData(SDOScope* newSdoScope)
{
    m_scope = newSdoScope;
}

SDOScopeData::~SDOScopeData()
{
}

SDOScope* SDOScopeData::sdoScope() const
{
    return m_scope;
}

void SDOScopeData::setSdoScope(SDOScope* newSdoScope)
{
    m_scope = newSdoScope;
}


qreal SDOScopeData::Ts() const
{
    if(m_scope == nullptr) return 0.0;

    uint Fs = m_scope->sampleRate();

    if(Fs == 0) return 0.0;

    return 1.0 / Fs;
}

size_t SDOScopeData::samplesCount() const
{
    if(m_scope == nullptr) return 0;

    return m_scope->samplesCount();
}

uint SDOScopeData::channelsCount() const
{
    if(m_scope == nullptr) return 0;

    return m_scope->channelsCount();
}

qreal SDOScopeData::sample(uint ch_n, size_t i)
{
    if(m_scope == nullptr) return 0.0;

    SDOScope::Channel* ch = m_scope->channel(ch_n);

    if(ch == nullptr) return 0.0;

    return ch->value(i);
}
