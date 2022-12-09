/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef FEC_ROUTER_AGENT_HELPER_H
#define FEC_ROUTER_AGENT_HELPER_H

#include "ns3/fec-router-agent.h"
#include "ns3/node.h"

namespace ns3 {

class FecRouterAgentHelper
{
public:
  FecRouterAgentHelper ();

  typedef std::vector<Ptr<FecRouterAgent> >::const_iterator Iterator;
  Iterator Begin (void) const;
  Iterator End (void) const;

  uint32_t GetN (void) const;
  Ptr<FecRouterAgent> Get (uint32_t i) const;
  void Create (uint32_t n);
  void Set (uint32_t i, Ptr<Node> node, bool codeType); /*codeType 0 = encode, 1 = decode*/

private:
  std::vector<Ptr<FecRouterAgent> > m_fecRouterAgents;
};

}

#endif /* FEC_ROUTER_AGENT_HELPER_H */

