/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/uinteger.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4-l3-protocol-with-fec-encode.h"
#include "ns3/ipv4-l3-protocol-with-fec-decode.h"
#include "fec-router-agent-helper.h"

namespace ns3 {


FecRouterAgentHelper::FecRouterAgentHelper ()
{
}

FecRouterAgentHelper::Iterator
FecRouterAgentHelper::Begin (void) const
{
  return m_fecRouterAgents.begin ();
}

FecRouterAgentHelper::Iterator
FecRouterAgentHelper::End (void) const
{
  return m_fecRouterAgents.end ();
}

uint32_t
FecRouterAgentHelper::GetN (void) const
{
  return m_fecRouterAgents.size ();
}

Ptr<FecRouterAgent>
FecRouterAgentHelper::Get (uint32_t i) const
{
  return m_fecRouterAgents[i];
}

void
FecRouterAgentHelper::Create (uint32_t n)
{
  for (uint32_t i = 0; i < n; i++)
    {
      Ptr<FecRouterAgent> fecRouterAgent = CreateObject<FecRouterAgent> ();
      m_fecRouterAgents.push_back (fecRouterAgent);
    }
}

void
FecRouterAgentHelper::Set (uint32_t i, Ptr<Node> node, bool codeType)
{
 if (codeType == 0)
   {
     Ptr<Ipv4L3ProtocolwithFecEncode> FecNode;
     FecNode = node->GetObject<Ipv4L3ProtocolwithFecEncode> ();
     FecNode->SetFec (m_fecRouterAgents[i]);
   }
 else if (codeType == 1)
   {
     Ptr<Ipv4L3ProtocolwithFecDecode> FecNode;
     FecNode = node->GetObject<Ipv4L3ProtocolwithFecDecode> ();
     FecNode->SetFec (m_fecRouterAgents[i]);
   }
 else
   {
     Ptr<Ipv4L3Protocol> FecNode;
     FecNode = node->GetObject<Ipv4L3Protocol> ();
     FecNode->SetFec (m_fecRouterAgents[i]);
   } 
}
 
}
