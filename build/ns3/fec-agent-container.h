/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Koga Laboratory
 *
 */

#ifndef FEC_AGENT_CONTAINER_H
#define FEC_AGENT_CONTAINER_H

#include <vector>
#include <stdint.h>
#include <string>
#include "ns3/fec-agent.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/fec-agent-container.h"
#include "ns3/application-container.h"

namespace ns3 {

class FecAgentContainer
{
public:
  FecAgentContainer ();

  typedef std::vector<Ptr<FecAgent> >::const_iterator Iterator;
  Iterator Begin (void) const;
  Iterator End (void) const;

  uint32_t GetN (void) const;
  Ptr<FecAgent> Get (uint32_t i) const;

  void Create (uint32_t n);
  void Install (uint32_t j, ApplicationContainer c) const;
  void SetAttribute (std::string name, const AttributeValue & value);

private:
  ObjectFactory m_factory;
  std::vector<Ptr<FecAgent> > m_agents;
};

} // namespace ns3

#endif /* FEC_AGENT_CONTAINER_H */
