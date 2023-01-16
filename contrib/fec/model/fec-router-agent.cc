/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <map>
#include <algorithm>
#include <functional>
#include "fec-router-agent.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

Ptr<OutputStreamWrapper> FecRouterAgent::m_printRoundFecTable;
Ptr<OutputStreamWrapper> FecRouterAgent::m_printGroupFecTable;
Ptr<OutputStreamWrapper> FecRouterAgent::m_printRecovery;
Ptr<OutputStreamWrapper> FecRouterAgent::m_printLostPacket;
Ptr<OutputStreamWrapper> FecRouterAgent::m_printSentPacket;

NS_LOG_COMPONENT_DEFINE ("FecRouterAgent");
NS_OBJECT_ENSURE_REGISTERED (FecRouterAgent);

TypeId
FecRouterAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FecRouterAgent")
    .SetParent<Object> ()
    .AddConstructor<FecRouterAgent> ()
    .AddAttribute ("FecWin", "Fec Window Size",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FecRouterAgent::m_fwin),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("NumberofGroups", "number of groups",
                   UintegerValue (1),
                   MakeUintegerAccessor (&FecRouterAgent::m_nGroups),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("SegmentSize", "Segment Size",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&FecRouterAgent::m_segmentSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SendRxRequest", "Send Retransmission Request Mode ON / OFF",
                   EnumValue (ON),
                   MakeEnumAccessor (&FecRouterAgent::m_sendRxRequest),
                   MakeEnumChecker (ON, "ON",
                                    OFF, "OFF"))
    .AddAttribute ("StoreMode", "Store Mode USE / DISUSE",
                   EnumValue (USE),
                   MakeEnumAccessor (&FecRouterAgent::m_storeMode),
                   MakeEnumChecker (USE, "USE",
                                    DISUSE, "DISUSE"))

  ;
  return tid;
}

FecRouterAgent::FecRouterAgent ()
  : tq_ (),
    m_serial (1),
    m_roundId (1),
    m_groupId (0),
    m_sentdatapkt (0),
    m_lostdatapkt (0),
    m_sentfecpkt (0),
    m_lostfecpkt (0),
    m_recover (0)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
FecRouterAgent::GetFwin ()
{
  NS_LOG_FUNCTION (this);
  return m_fwin;
}

uint8_t
FecRouterAgent::GetNGroups ()
{
  NS_LOG_FUNCTION (this);
  return m_nGroups;
}

uint32_t
FecRouterAgent::GetSegmentSize ()
{
  NS_LOG_FUNCTION (this);
  return m_segmentSize;
}

Ptr<Packet>
FecRouterAgent::CreateDataPacket (Ptr<Packet> packet, Ipv4Header ipHeader)
{
  NS_LOG_FUNCTION (this);

  if (m_nGroups == 1) m_groupId = 0;
  
  TcpHeader tcpHeader;
  packet->PeekHeader (tcpHeader);

  PacketInfo pi;
  pi.serial_ = m_serial;
  pi.check_ = false;
  pi.gid_ = m_groupId;
  pi.type_ = false;
  pi.tcp_ = tcpHeader;
  pi.ip_ = ipHeader;
  pi.flag_ = false;

  if (m_fwin)
    { 
      gt_.back().gi_[m_groupId].pi_.push_back(pi);
      rt_.back().pi_.push_back (pi);  
    }

  FecTag tag;
  tag.SetType (1);
  tag.SetSerial (m_serial);
  tag.SetRoundId (m_roundId);
  tag.SetGroupId (m_groupId);
  tag.SetNGroups (m_nGroups);
  packet->AddPacketTag (tag);

  if (m_nGroups > 1)
    {
      ++m_groupId;
      if (m_groupId == m_nGroups) m_groupId = 0;
    }

  m_serial++;
  m_sentdatapkt++;

  if (m_printSentPacket)
    {
      PrintSentPacket (tag.GetRoundId (), tag.GetGroupId (), tag.GetSerial (),
                       m_sentdatapkt, m_sentfecpkt, ipHeader.GetSource (),
                       ipHeader.GetDestination (), tcpHeader.GetSequenceNumber ());
    }

  return packet;
}

Ptr<Packet>
FecRouterAgent::CreateFecPacket (Ptr<Packet> packet, Ipv4Header ipHeader)
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> p = Create<Packet> (m_segmentSize);

  PacketInfo pi;
  pi.serial_ = m_serial;
  pi.check_ = false;
  pi.gid_ = m_groupId;
  pi.type_ = true;
  pi.flag_ = false;

  gt_.back().gi_[m_groupId].pi_.push_back(pi);
  rt_.back().pi_.push_back (pi);  

  TcpHeader h;
  packet->PeekHeader (h);

  TcpHeader fecHeader;
  fecHeader.SetFlags (h.GetFlags ());
  fecHeader.SetSequenceNumber (SequenceNumber32 (0));
  fecHeader.SetAckNumber (h.GetAckNumber ());
  fecHeader.SetSourcePort (h.GetSourcePort ());
  fecHeader.SetDestinationPort (h.GetDestinationPort ());
  fecHeader.SetWindowSize(h.GetWindowSize ());
 
  FecTag tag;
  tag.SetType (2);
  tag.SetSerial (m_serial);
  tag.SetRoundId (m_roundId);
  tag.SetGroupId (m_groupId);
  tag.SetNGroups (m_nGroups);

  p->AddPacketTag (tag);
  p->AddHeader (fecHeader);

  if (m_nGroups > 1)
    {
      ++m_groupId;
      ++m_serial;
      if (m_groupId == m_nGroups) 
        { 
          m_serial = 1;
          m_groupId = 0;
          ++m_roundId;
        }
    }
  else if (m_nGroups == 1)
    {
          m_serial = 1;
          m_groupId = 0;
          ++m_roundId;
    }

  m_sentfecpkt++;
  
  if (m_printSentPacket)
    {
      PrintSentPacket (tag.GetRoundId (), tag.GetGroupId (), tag.GetSerial (),
                       m_sentdatapkt, m_sentfecpkt, ipHeader.GetSource (),
                       ipHeader.GetDestination (), fecHeader.GetSequenceNumber ());
    }

  return p;
}

void
FecRouterAgent::CreateRecoveryPacket (Ptr<Packet> packet, TcpHeader tcpHeader, Ipv4Header ipHeader, FecTag tag, uint32_t rec_serial)
{
  NS_LOG_FUNCTION (this);

  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();

  packet->RemovePacketTag (tag);
  TcpHeader fecTcpHeader;
  packet->RemoveHeader (fecTcpHeader);
  Ipv4Header fecIpHeader;
  packet->RemoveHeader (fecIpHeader);

  FecTag newTag;
  newTag.SetType (4);
  newTag.SetRoundId (rid);
  newTag.SetGroupId (gid);
  newTag.SetSerial (rec_serial);
                           
  packet->AddPacketTag (newTag);                         
  packet->AddHeader (tcpHeader);
  packet->AddHeader (ipHeader);

  InsertTransitQueue (packet, rec_serial);
}

void
FecRouterAgent::UpdateFecTable ()
{
  NS_LOG_FUNCTION (this);

  GroupFecTable gt;
  gt.rid_ = m_roundId;
  gt.nGroups_ = m_nGroups;
  gt_.push (gt);

  RoundFecTable rt;
  rt.rid_ = m_roundId;
  rt.nGroups_ = m_nGroups;
  rt_.push (rt);
}

uint32_t 
FecRouterAgent::Recv (Ptr<Packet> packet, Ipv4Header ipHeader)
{
  NS_LOG_FUNCTION (this);
  
  FecTag tag;
  packet->PeekPacketTag (tag);
  uint8_t type = tag.GetType ();
  uint8_t nGroups = tag.GetNGroups ();
  uint32_t serial = tag.GetSerial ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t flag = 0;

  MarkRoundFecTable (packet);
  uint32_t sumlost = CheckRoundFecTable (packet);
  if (sumlost > nGroups) return 2;

  uint32_t nlost = MarkGroupFecTable (packet);
  if (type == 2) flag = CheckGroupFecTable (packet, nlost);

  if (nlost > nGroups) return 2;
  if (sumlost > 0 && type == 1 && m_storeMode) 
    {
     packet->AddHeader (ipHeader);
     InsertTransitQueue (packet, serial);
     return 1;
    }
  if ((sumlost - nlost) > 0 && flag == 1 && type == 2 && gid < m_nGroups-1) return 1;
  if (type == 2 && gid == m_nGroups-1) return 2;

  return 0;
}

void
FecRouterAgent::MarkRoundFecTable (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  FecTag tag;
  packet->PeekPacketTag (tag);

  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t serial = tag.GetSerial ();

  if (rid < rt_.front ().rid_) return;
  if (rt_.empty () || rid > rt_.front ().rid_)
    {
      while (rt_.front ().rid_ < rid)
        {
          if (m_printRoundFecTable) PrintRoundFecTable ();
          rt_.pop ();
        }
    }
  while (true)
    {
     if (rt_.front ().rid_ == rid)
       {
         std::list<PacketInfo>& pi = rt_.front ().pi_;
         std::list<PacketInfo>::iterator itr;

         for (itr = pi.begin (); itr != pi.end (); itr++)
           {
             if (itr->serial_ == serial && itr->gid_ == gid)
               {
                 itr->check_ = true;
                 return;
               }
           }
       }
    }
}

uint32_t
FecRouterAgent::CheckRoundFecTable (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  FecTag tag;
  packet->PeekPacketTag (tag);

  uint8_t type = tag.GetType ();
  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t serial = tag.GetSerial ();
  uint8_t nGroups = tag.GetNGroups ();
  uint32_t sumlost = 0;
  uint32_t fwin = m_fwin * nGroups; 
 
  if (rid < rt_.front ().rid_) return 0;
  if (rt_.empty () || rid > rt_.front ().rid_)
    {
      while (rt_.front ().rid_ < rid)
        {
          if (m_printRoundFecTable) PrintRoundFecTable ();
          rt_.pop ();
        }
    }
  while (true)
    {
     if (rt_.front ().rid_ == rid)
       {
         std::list<PacketInfo>& pi = rt_.front ().pi_;
         std::list<PacketInfo>::iterator itr;

         for (itr = pi.begin (); itr != pi.end (); itr++)
           {
             if (itr->check_ == false && itr->serial_ < fwin)
               {
                 sumlost++;

                 if (type == 1 && itr->flag_ == false && m_sendRxRequest)
                   {
                     LostPacketInfo (rid, itr->gid_, itr->serial_,
                                     itr->tcp_, itr->ip_);
                     itr->flag_ = true;
                   }
               }
             if (type == 1 && serial == itr->serial_) return sumlost;
           }
       }
     if (type == 2 && gid == rt_.front ().nGroups_-1 )
       {
         if (m_printRoundFecTable) PrintRoundFecTable ();
         rt_.front ().pi_.clear ();
         rt_.pop ();
       }
     return sumlost;
    }
  return sumlost;
}

uint32_t
FecRouterAgent::MarkGroupFecTable (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  FecTag tag;
  packet->PeekPacketTag (tag);
 
  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t serial = tag.GetSerial ();

  if (rid < rt_.front ().rid_) return 0;
  if (gt_.empty () || rid > gt_.front().rid_)
    {
     while (gt_.front ().rid_ < rid)
       {
         if (m_printGroupFecTable)
           {
             for (uint32_t i = 0; i < gt_.front ().nGroups_; i++)
               {
                 PrintGroupFecTable (0, i);
                 gt_.front().gi_[i].pi_.clear (); 
               }
           }
         gt_.pop ();
        }
    }
  while (true)
    {
      if (gt_.front().rid_ == rid)
        {
          std::list<PacketInfo>& pi = gt_.front().gi_[gid].pi_;
          std::list<PacketInfo>::iterator itr;
          uint32_t nlost = 0;
          for (itr = pi.begin (); itr != pi.end (); itr++)
            {
              if (itr->serial_ == serial)
                {  
                  itr->check_ = true;
                  return (nlost);
                }
              else if (itr->serial_ < serial )
                {
                  if (itr->check_ == false)
                    {
                      nlost++;
                    }
                }
            }
        }
    }
}

uint32_t
FecRouterAgent::CheckGroupFecTable (Ptr<Packet> packet, uint32_t nlost)
{
  NS_LOG_FUNCTION (this);

  FecTag tag;
  packet->PeekPacketTag (tag);
  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t serial = tag.GetSerial ();

  if (rid < rt_.front ().rid_) return 0;
  if (gt_.empty () || rid > gt_.front().rid_)
    {
     while (gt_.front ().rid_ < rid)
       {
         if (m_printGroupFecTable)
           {
            for (uint32_t i = 0; i < gt_.front ().nGroups_; i++)
               {
                 PrintGroupFecTable (0, i);
               }
           }
         gt_.pop ();
        }
    }
  while (true)
    {
      if (gt_.front().rid_ == rid)
        {
          std::list<PacketInfo>& pi = gt_.front().gi_[gid].pi_;
          std::list<PacketInfo>::iterator itr;
          uint32_t mlost = 0;
          uint32_t recover = 0;
          uint32_t rec_serial;
          TcpHeader oldTcpHeader;
          Ipv4Header oldIpHeader;
         
          for (itr = pi.begin (); itr != pi.end (); itr++)
            {
              if (itr->serial_ == serial)
                { 
                  if (nlost == 1 && mlost == 1) 
                        {
                          recover = 1;
                          m_recover++;
                          CreateRecoveryPacket (packet, oldTcpHeader, oldIpHeader, tag, rec_serial);
                          if (m_printRecovery) PrintRecovery (m_recover, rid, gid);
                        }

                  if (m_printGroupFecTable) PrintGroupFecTable (recover, gid);
                  gt_.front().gi_[gid].pi_.clear (); 
                  return (mlost);
                }
              else if (itr->serial_ < serial)
                {
                  if (itr->check_ == false)
                    {
                      mlost++;
                      if (nlost == 1)
                        {
                          rec_serial = itr->serial_;
                          oldTcpHeader = itr->tcp_;
                          oldIpHeader = itr->ip_;
                        }
                    }
                }
            }
        }
    }
  return 0;
}

uint32_t
FecRouterAgent::TransitQueueSize ()
{
  NS_LOG_FUNCTION (this);

  uint32_t size = 0;

  if (!tq_.empty ())
    {
      size = tq_.size ();
    }

  return size;
}

Ptr<Packet>
FecRouterAgent::GetPacket ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> pkt = tq_.front ().p_;
  tq_.pop_front ();

  return pkt;
}

void
FecRouterAgent::InsertTransitQueue (Ptr<Packet> packet, uint32_t serial)
{
  NS_LOG_FUNCTION (this);

 TransitQueue tq;
 tq.serial_ = serial;
 tq.p_ = packet;

 std::list<TransitQueue>::iterator itr;
 for (itr = tq_.begin (); itr != tq_.end (); itr++)
    {
      if (itr->serial_ > serial)
        {
          break;
        }
    }
 tq_.insert (itr, tq);
}

void
FecRouterAgent::LostPacketInfo (uint32_t roundId, uint8_t groupId, uint32_t serial,
                          TcpHeader oldTcpHeader, Ipv4Header oldIpHeader)
{
  NS_LOG_FUNCTION (this);
  
  TcpHeader newTcpHeader;
  newTcpHeader.SetFlags (TcpHeader::ECE);
  newTcpHeader.SetSequenceNumber (oldTcpHeader.GetAckNumber ());
  newTcpHeader.SetAckNumber (oldTcpHeader.GetSequenceNumber ());
  newTcpHeader.SetSourcePort (oldTcpHeader.GetDestinationPort ());
  newTcpHeader.SetDestinationPort (oldTcpHeader.GetSourcePort ());
  newTcpHeader.SetWindowSize (oldTcpHeader.GetWindowSize ());

  Ipv4Header newIpHeader;
  newIpHeader.SetSource (oldIpHeader.GetDestination ());
  newIpHeader.SetDestination (oldIpHeader.GetSource ());
  newIpHeader.SetProtocol (oldIpHeader.GetProtocol ());
  newIpHeader.SetPayloadSize (oldIpHeader.GetPayloadSize ());
  newIpHeader.SetTtl (64);
  newIpHeader.SetTos (0);

  FecTag newTag;
  newTag.SetType (5);
  newTag.SetRoundId (roundId);
  newTag.SetGroupId (groupId);
  newTag.SetSerial (serial);
   
  Ptr<Packet> packet = Create<Packet> ();                        
  packet->AddPacketTag (newTag);                         
  packet->AddHeader (newTcpHeader);
  packet->AddHeader (newIpHeader);
  
  lpq_.push (packet); 
}

uint32_t
FecRouterAgent::LostPacketQueueSize ()
{
  NS_LOG_FUNCTION (this);

  uint32_t size = 0;

  if (!lpq_.empty ())
    {
      size = lpq_.size ();
    }

  return size;
}

Ptr<Packet>
FecRouterAgent::GetLostPacket ()
{
 NS_LOG_FUNCTION (this);
 
 Ptr<Packet> packet = lpq_.front ();  
 lpq_.pop ();
 return packet;
}

void
FecRouterAgent::PrintRoundFecTable ()
{
  NS_LOG_FUNCTION (this);

  std::list<PacketInfo>& pi = rt_.front().pi_;
  std::list<PacketInfo>::iterator itr;
  std::ostream * os = m_printRoundFecTable->GetStream ();
  * os << "rid " << rt_.front ().rid_;

  for (itr = pi.begin (); itr != pi.end (); itr++)
    {
    * os << " ser " << itr->serial_ <<  " seq " << itr->tcp_.GetSequenceNumber () 
         << " " << itr->check_;
      if (itr->type_ == false && itr->check_ == false) 
        {  
          m_lostdatapkt++;
          if (m_printLostPacket) 
            {
              PrintLostPacket (rt_.front ().rid_, itr->gid_, m_lostdatapkt, m_lostfecpkt,
                               itr->tcp_.GetSequenceNumber (),    
                               itr->ip_.GetSource (), itr->ip_.GetDestination ());
            }
        }
      if (itr->type_ == true && itr->check_ == false) 
        {
          m_lostfecpkt++;
          if (m_printLostPacket) 
            {
              PrintLostPacket (rt_.front ().rid_, itr->gid_, m_lostdatapkt, m_lostfecpkt,
                               itr->tcp_.GetSequenceNumber (),    
                               itr->ip_.GetSource (), itr->ip_.GetDestination ());
            }
        }
    }
  * os << std::endl;
}
 
void
FecRouterAgent::EnablePrintRoundFecTable (Ptr<OutputStreamWrapper> stream)
{
  m_printRoundFecTable = stream;
}

void
FecRouterAgent::PrintGroupFecTable (uint32_t recover, uint8_t groupId)
{
  NS_LOG_FUNCTION (this);

  if (!gt_.front ().gi_.size ())
    {
          std::ostream * os = m_printGroupFecTable->GetStream ();
          * os << " rid "  << gt_.front().rid_
               << " gid 0"  
               << std::endl;
     return;
    }
 
  for(uint8_t i = 0;  i <= groupId; i++)
    {
      if (gt_.front().gi_[i].pi_.size () > 0)
        {
          std::list<PacketInfo>& pi = gt_.front().gi_[i].pi_;
          std::list<PacketInfo>::iterator itr;
          std::ostream * os = m_printGroupFecTable->GetStream ();
          * os << " rid "     << gt_.front().rid_
               << " gid "     << uint32_t (i)
               << " recover " << recover ;
         
          for (itr = pi.begin (); itr != pi.end (); itr++)
            {
              * os << " ser " << itr->serial_ 
                   << " seq " << itr->tcp_.GetSequenceNumber () 
                   << " "     << itr->check_;
            }
          * os << std::endl;
        }
    }
}
 
void
FecRouterAgent::EnablePrintGroupFecTable (Ptr<OutputStreamWrapper> stream)
{
  m_printGroupFecTable = stream;
}

void
FecRouterAgent::PrintRecovery (uint32_t recover, uint32_t roundId, uint8_t groupId)
{
  NS_LOG_FUNCTION (this);

  std::ostream * os = m_printRecovery->GetStream ();
  * os << Simulator::Now ().GetSeconds ()
       << " recover " << recover
       << " rid "     << roundId
       << " gid "     << uint32_t (groupId)
       << std::endl; 
}
 
void
FecRouterAgent::EnablePrintRecovery (Ptr<OutputStreamWrapper> stream)
{
  m_printRecovery = stream;
}

void
FecRouterAgent::PrintLostPacket (uint32_t roundId, uint8_t groupId, 
                           uint32_t lostdatapkt, uint32_t lostfecpkt, 
                           SequenceNumber32 seq,
                           Ipv4Address src, Ipv4Address dst)
{
  NS_LOG_FUNCTION (this);

  std::ostream * os = m_printLostPacket->GetStream ();
  * os << Simulator::Now ().GetSeconds ()
       << " rid "  << roundId
       << " gid "  << uint32_t (groupId)
       << " data " << lostdatapkt
       << " fec "  << lostfecpkt
       << " seq "  << seq
       << " src "  << src
       << " dst "  << dst
       << std::endl; 
}
 
void
FecRouterAgent::EnablePrintLostPacket (Ptr<OutputStreamWrapper> stream)
{
  m_printLostPacket = stream;
}

void
FecRouterAgent::PrintSentPacket (uint32_t roundId, uint8_t groupId, uint32_t serial, 
                           uint32_t sentdatapkt, uint32_t sentfecpkt, 
                           Ipv4Address src, Ipv4Address dst,
                           SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);

  std::ostream * os = m_printSentPacket->GetStream ();
  * os << Simulator::Now ().GetSeconds ()
       << " rid "  << roundId
       << " gid "  << uint32_t (groupId)
       << " ser "  << serial
       << " data " << sentdatapkt
       << " fec "  << sentfecpkt
       << " src "  << src
       << " dst "  << dst
       << " seq "  << seq
       << std::endl; 
}
 
void
FecRouterAgent::EnablePrintSentPacket (Ptr<OutputStreamWrapper> stream)
{
  m_printSentPacket = stream;
}

}
