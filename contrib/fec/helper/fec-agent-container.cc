/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Koga Laboratory
 *
 */

#include "fec-agent-container.h"

namespace ns3 {

FecAgentContainer::FecAgentContainer ()
{
}

FecAgentContainer::Iterator
FecAgentContainer::Begin (void) const
{
  return m_agents.begin ();
}

FecAgentContainer::Iterator
FecAgentContainer::End (void) const
{
  return m_agents.end ();
}

uint32_t
FecAgentContainer::GetN (void) const
{
  return m_agents.size();
}

Ptr<FecAgent>
FecAgentContainer::Get (uint32_t i) const
{
  return m_agents[i];
} 

void
FecAgentContainer::Create (uint32_t n)
{
 for (uint32_t i = 0; i < n; i++)
  {
  m_factory.SetTypeId (FecAgent::GetTypeId ());
  Ptr<FecAgent> agent = m_factory.Create<FecAgent> ();
  m_agents.push_back (agent);
  }
}

void
FecAgentContainer::Install (uint32_t j, ApplicationContainer c) const
{
  for (ApplicationContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      (*i)->SetFec (m_agents[j]);
    }
}

void
FecAgentContainer::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

} // namespace ns3
