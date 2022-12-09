
#ifdef NS3_MODULE_COMPILATION
# error "Do not include ns3 module aggregator headers from other modules; these are meant only for end user scripts."
#endif

#ifndef NS3_MODULE_FEC
    

// Module headers:
#include "fec-agent-container.h"
#include "fec-agent.h"
#include "fec-router-agent-helper.h"
#include "fec-router-agent.h"
#include "fec-tag.h"
#include "ipv4-l3-protocol-with-fec-decode.h"
#include "ipv4-l3-protocol-with-fec-encode.h"
#endif
