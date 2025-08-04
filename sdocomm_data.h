#ifndef SDOCOMM_DATA_H
#define SDOCOMM_DATA_H

#include "sdocomm.h"
#include "cotypes.h"

struct SDOComm_data {
    int/*SDOComm::Type*/ m_type;
    CO::NodeId m_nodeId;
    CO::Index m_index;
    CO::SubIndex m_subIndex;
    void* m_data;
    size_t m_dataSize;
    int m_timeout;
    int/*SDOComm::State*/ m_state;
    int/*SDOComm::Error*/ m_error;
    bool m_cancel;
    size_t m_transferSize;
    size_t m_dataTransfered;
    size_t m_dataBuffered;
};

#endif // SDOCOMM_DATA_H
