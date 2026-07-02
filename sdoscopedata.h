#ifndef SDOSCOPEDATA_H
#define SDOSCOPEDATA_H

#include "oscopedata.h"

class SDOScope;


class SDOScopeData : public OScopeData
{
public:
    SDOScopeData(SDOScope* newSdoScope);
    ~SDOScopeData();

    SDOScope* sdoScope() const;
    void setSdoScope(SDOScope* newSdoScope);

    qreal Ts() const override;
    size_t samplesCount() const override;
    uint channelsCount() const override;
    qreal sample(uint ch_n, size_t i) override;

protected:
    SDOScope* m_scope;
};

#endif // SDOSCOPEDATA_H
