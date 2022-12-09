/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (C) 2021 Sato Laboratory
 *
 */

#include <algorithm>
#include <functional>
#include "fec-tag.h"
#include "fec-agent.h"
#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/double.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/tcp-header.h"

namespace ns3 {
  
Ptr<OutputStreamWrapper> FecAgent::m_tcpDump;
Ptr<OutputStreamWrapper> FecAgent::m_printTimeout;
Ptr<OutputStreamWrapper> FecAgent::m_printFastRecovery;
Ptr<OutputStreamWrapper> FecAgent::m_printRetransmit;
Ptr<OutputStreamWrapper> FecAgent::m_printRateControl;
Ptr<OutputStreamWrapper> FecAgent::m_printFwin;
Ptr<OutputStreamWrapper> FecAgent::m_printRoundFecTable;
Ptr<OutputStreamWrapper> FecAgent::m_printGroupFecTable;
Ptr<OutputStreamWrapper> FecAgent::m_printLostPacket;
Ptr<OutputStreamWrapper> FecAgent::m_printRecovery;

NS_LOG_COMPONENT_DEFINE ("FecAgent");
NS_OBJECT_ENSURE_REGISTERED (FecAgent);

TypeId
FecAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FecAgent")
    .SetParent<Object> ()
    .AddConstructor<FecAgent> ()
    .AddAttribute ("FecMode", "FecMode Dynamic / Static",
                   EnumValue (Static),
                   MakeEnumAccessor (&FecAgent::m_fecMode),
                   MakeEnumChecker (Dynamic, "Dynamic",
                                    Static, "Static"))
    .AddAttribute ("BoundMode", "BoundMode IN / OUT",
                   EnumValue (IN),
                   MakeEnumAccessor (&FecAgent::m_boundMode),
                   MakeEnumChecker (IN, "IN",
                                    OUT, "OUT"))
    .AddAttribute ("AckMode", "AckMode Suppression / Send",
                   EnumValue (Suppression),
                   MakeEnumAccessor (&FecAgent::m_ackMode),
                   MakeEnumChecker (Suppression, "Suppression",
                                    Send, "Send"))
    .AddAttribute ("FecWin", "fec window size",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FecAgent::m_fwin),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MaxFecWin", "max fec window size",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FecAgent::m_fmax),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("NumberofGroups", "number of groups",
                   UintegerValue (1),
                   MakeUintegerAccessor (&FecAgent::m_nGroups),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("srcId", "Source Node Id ",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FecAgent::m_srcId),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("dstId", "Destination Node Id ",
                   UintegerValue (0),
                   MakeUintegerAccessor (&FecAgent::m_dstId),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SegmentSize", "Segment Size",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&FecAgent::m_segmentSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RateControlValue", "rate control value",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&FecAgent::m_rateControlValue),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}
  
FecAgent::FecAgent ()
   : tq_ (),
     m_roundId (1),
     m_groupId (0),
     m_cwnd (0),
     m_ssThresh (0),
     m_lastAckedSeq (0),
     m_sentdatapkt (0),
     m_lostdatapkt (0),
     m_retPkt (0),
     m_sentfecpkt (0),
     m_lostfecpkt (0),
     m_recover (0),
     m_recover_rexmit (0),
     m_timeout (0),
     m_fastRecovery (0),
     m_rateControl (0),
     m_fmaxCount (0),
     m_ackSuppression (0),
     ic (0),
     fwin (0),
     ackSendId (0),
     nGroups (0)
{
  NS_LOG_FUNCTION (this);
  ScheduleTcpDump ();
}

FecAgent::~FecAgent ()
{
  NS_LOG_FUNCTION (this);
}

Ptr<Packet>
FecAgent::CreateDataPacket (Ptr<Packet> packet, TcpHeader tcpHeader, SequenceNumber32 highTxMark)
{
  NS_LOG_FUNCTION (this);

  if (m_fwin)
    {
      SequenceNumber32 seq = tcpHeader.GetSequenceNumber ();
      m_serial = seq.GetValue (); //cast

      if (m_nGroups == 1) m_groupId = 0;
  
      bool check = false;
      if (seq < highTxMark) check = true;

      PacketInfo pi;
      pi.serial_ = m_serial;
      pi.check_ = check;
      pi.gid_ = m_groupId;
      pi.type_ = false;
      pi.tcp_ = tcpHeader;
      pi.flag_ = false;

      gt_.back().gi_[m_groupId].pi_.push_back(pi);
      rt_.back().pi_.push_back (pi);  

      FecTag tag;

      if (!check)
        {
          tag.SetType (1);
        }
      else
        {
          tag.SetType (3);
        }

      tag.SetSerial (m_serial);
      tag.SetRoundId (m_roundId);
      tag.SetGroupId (m_groupId);
      tag.SetNGroups (nGroups);
      packet->AddPacketTag (tag);

      if (nGroups > 1)
        {
          ++m_groupId;
          if (m_groupId == nGroups) m_groupId = 0;
        }

      m_serial++;
   }
  m_sentdatapkt++;
  return packet;
}

Ptr<Packet>
FecAgent::CreateFecPacket ()
{
  NS_LOG_FUNCTION (this);
  
  Ptr<Packet> packet = Create<Packet> (m_segmentSize);

  PacketInfo pi;
  pi.serial_ = m_serial;
  pi.check_ = false;
  pi.gid_ = m_groupId;
  pi.type_ = true;
  pi.flag_ = false;

  gt_.back().gi_[m_groupId].pi_.push_back(pi);
  rt_.back().pi_.push_back (pi);  

  FecTag tag;
  tag.SetType (2);
  tag.SetSerial (m_serial);
  tag.SetRoundId (m_roundId);
  tag.SetGroupId (m_groupId);
  tag.SetNGroups (nGroups);

  packet->AddPacketTag (tag);

  if (nGroups > 1)
    {
      ++m_groupId;
      ++m_serial;
      if (m_groupId == nGroups) 
        { 
          m_groupId = 0;
          ++m_roundId;
        }
    }
  else if (nGroups == 1)
    {
          m_groupId = 0;
          ++m_roundId;
    }

  m_sentfecpkt++;
  
  return packet;
}

uint32_t
FecAgent::GetFwin ()
{
  NS_LOG_FUNCTION (this);

  return m_fwin;
}

uint32_t
FecAgent::RedundancyControl (uint32_t cwnd)
{
  NS_LOG_FUNCTION (this);

  uint32_t win = cwnd / m_segmentSize;

  if (m_fecMode == Static)
    {
      if (m_boundMode == OUT)
        {
          fwin = m_fwin * m_nGroups;
          nGroups = m_nGroups;
        }
      else if (m_boundMode == IN)
        {
          if (win < ((m_fwin*m_nGroups)+m_nGroups))
            {
              fwin = win;
              nGroups = 1;
            }
          else if (win >= ((m_fwin*m_nGroups)+m_nGroups))
           {
             fwin = m_fwin * m_nGroups;
             nGroups = m_nGroups; 
           }
        }
    }
  else if (m_fecMode == Dynamic)
    {
      if (m_boundMode == OUT)
        {
          nGroups = m_nGroups;
          if (win < nGroups)
            {
              nGroups = 1;
            }
          uint32_t fc = win / nGroups;
          if (m_fmax && fc >= m_fmax) 
            { 
              fc = m_fmax;
              m_fmaxCount++;
            }
          fwin = fc * nGroups;
        }
      else if (m_boundMode == IN)
        {
          if (win < ((m_fwin*m_nGroups)+m_nGroups))
            {
              fwin = win;
              nGroups = 1;
            }
          else if (win >= ((m_fwin*m_nGroups)+m_nGroups))
           {
             uint32_t fc = (win - m_nGroups) / m_nGroups;
             if (m_fmax && fc >= m_fmax) 
               {
                 fc = m_fmax;
                 m_fmaxCount++;
               }
             fwin = fc * m_nGroups;
             nGroups = m_nGroups;
           }  
        }
    }
  
  GroupFecTable gt;
  gt.rid_ = m_roundId;
  gt.nGroups_ = nGroups;
  gt_.push (gt);

  RoundFecTable rt;
  rt.rid_ = m_roundId;
  rt.nGroups_ = nGroups;
  rt_.push (rt);

  if (m_printFwin)
    {
      PrintFwin (m_roundId, fwin, nGroups, cwnd);
    }

  return fwin;
}

uint32_t
FecAgent::GetNGroups ()
{
  NS_LOG_FUNCTION (this);

  return nGroups;
}

void
FecAgent::Reset ()
{
  NS_LOG_FUNCTION (this);
  
  ++m_roundId;
  m_groupId = 0;
}

double
FecAgent::GetRateControlValue ()
{
  NS_LOG_FUNCTION (this);

  return m_rateControlValue;
}

uint32_t
FecAgent::GetSrcId ()
{
  NS_LOG_FUNCTION (this);

  return m_srcId;
}

uint32_t
FecAgent::Recv (Ptr<Packet> packet, Ipv4Header ipHeader)
{
  NS_LOG_FUNCTION (this);
  
  FecTag tag;
  packet->PeekPacketTag (tag);
  uint8_t type = tag.GetType ();
  uint8_t nGroups = tag.GetNGroups ();
  uint32_t serial = tag.GetSerial ();
  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t flag = 0;

  MarkRoundFecTable (packet);
  uint32_t sumlost = CheckRoundFecTable (packet);
  if (sumlost > nGroups) return 2;
  
  uint32_t nlost = MarkGroupFecTable (packet);
  if (type == 2) flag = CheckGroupFecTable (packet, nlost);
  
  if (nlost > nGroups) return 2;

  if (type == 3) ackSendId = rid;
  if (ackSendId == rid && type != 2) return 2; //ack suppression cancel

  if (sumlost > 0 && type == 1 && m_ackMode) 
    {
     packet->AddHeader (ipHeader);
     InsertTransitQueue (packet, serial);
     m_ackSuppression++;
     return 1;
    }
  
  if ((sumlost - nlost) > 0 && flag == 1 && type == 2 && gid < m_nGroups-1) return 1;
  if (type == 2 && gid == m_nGroups-1) return 2;
  
  return 0;
}

void
FecAgent::MarkRoundFecTable (Ptr<Packet> packet)
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
FecAgent::CheckRoundFecTable (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this);

  FecTag tag;
  packet->PeekPacketTag (tag);

  uint8_t type = tag.GetType ();
  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();
  uint32_t serial = tag.GetSerial ();
  uint32_t sumlost = 0;
 
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
             if (itr->check_ == false && itr->serial_ < serial)
               {
                 sumlost++;
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
FecAgent::MarkGroupFecTable (Ptr<Packet> packet)
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
FecAgent::CheckGroupFecTable (Ptr<Packet> packet, uint32_t nlost)
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
          bool retpkt = false;
          TcpHeader oldTcpHeader;
         
          for (itr = pi.begin (); itr != pi.end (); itr++)
            {
              if (itr->serial_ == serial)
                { 
                  if (nlost == 1 && mlost == 1) 
                        {
                          recover = 1;

                          if (retpkt) 
                            {
                              m_recover_rexmit++;
                            }
                          else if (!retpkt)
                            {
                              m_recover++;
                            }
                          CreateRecoveryPacket (packet, oldTcpHeader, tag, rec_serial);
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
                          retpkt = itr->check_;
                        }
                    }
                }
            }
        }
    }
  return 0;
}

void
FecAgent::InsertTransitQueue (Ptr<Packet> packet, uint32_t serial)
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

uint32_t
FecAgent::TransitQueueSize ()
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
FecAgent::GetPacket ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> pkt = tq_.front ().p_;
  tq_.pop_front ();

  return pkt;
}

void
FecAgent::CreateRecoveryPacket (Ptr<Packet> packet, TcpHeader oldTcpHeader,
                                FecTag tag, uint32_t rec_serial)
{
  NS_LOG_FUNCTION (this);

  uint32_t rid = tag.GetRoundId ();
  uint8_t gid = tag.GetGroupId ();

  packet->RemovePacketTag (tag);
  TcpHeader oldFecTcpHeader;
  packet->RemoveHeader (oldFecTcpHeader);

  FecTag newTag;
  newTag.SetType (4);
  newTag.SetRoundId (rid);
  newTag.SetGroupId (gid);
  newTag.SetSerial (rec_serial);
                           
  packet->AddPacketTag (newTag);                         
  packet->AddHeader(oldTcpHeader);

  InsertTransitQueue (packet, rec_serial);
}

void
FecAgent::SetTcpDumpInfo (uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq, SequenceNumber32 lastAckedSeq)
{
  NS_LOG_FUNCTION (this);
  
  m_cwnd = cwnd;
  m_ssThresh = ssThresh;
  m_seq = seq;
  m_lastAckedSeq = lastAckedSeq;
}

void
FecAgent::SetTimeoutInfo (uint32_t timeout, uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);

  m_timeout = timeout;
  m_cwnd = cwnd;
  m_ssThresh = ssThresh;
  m_seq = seq;

  if (m_printTimeout)
    {
      PrintTimeout (timeout, cwnd, seq);
    }
}

void
FecAgent::SetFastRecoveryInfo (uint32_t fastRecovery, uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);

  m_fastRecovery = fastRecovery;
  m_cwnd = cwnd;
  m_ssThresh = ssThresh;
  m_seq = seq;

  if (m_printFastRecovery)
    {
      PrintFastRecovery (fastRecovery, cwnd, seq);
    }
}

void
FecAgent::SetRetransmitInfo (uint32_t retPkt, uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);

  m_retPkt = retPkt;
  m_cwnd = cwnd;
  m_ssThresh = ssThresh;
  m_seq = seq;

  if (m_printRetransmit)
    {
      PrintRetransmit (retPkt, cwnd, seq);
    }
}

void
FecAgent::SetRateControlInfo (uint32_t rateControl, uint32_t newcwnd, uint32_t oldcwnd, uint32_t ssThresh)
{
  NS_LOG_FUNCTION (this);

  m_rateControl = rateControl;
  m_cwnd = newcwnd;
  m_ssThresh = ssThresh;

  if (m_printRateControl)
    {
      PrintRateControl (rateControl, newcwnd, oldcwnd, ssThresh);
    }
}

void
FecAgent::TcpDump (void)
{
  NS_LOG_FUNCTION (this);

  if (m_tcpDump)
    {
      std::ostream * os = m_tcpDump->GetStream ();
      * os << m_srcId
           << " time "    << Simulator::Now ().GetSeconds ()
           << " cwnd "    << m_cwnd                //Window size
           << " st "      << m_ssThresh            //Slowdown threshold
           << " ack "     << m_lastAckedSeq        //Last ACk number
           << " sent "    << m_sentdatapkt         //Number of sent data packets
           << " lost "    << m_lostdatapkt         //Number of lost packets
           << " ret "     << m_retPkt              //Number of retransmit packets
           << " fec "     << m_sentfecpkt          //Number of redundant packets
           << " feclost " << m_lostfecpkt          //Number of lost redundant packets
           << " rec "     << m_recover             //Number of recoveries
           << " rec_ret " << m_recover_rexmit      //Number of recoveries (ret packets)
           << " to "      << m_timeout             //Number of timeouts
           << " fr "      << m_fastRecovery        //Number of fast recoveries  
           << " rc "      << m_rateControl         //Number of rate control counts
           << " seq "     << m_seq                 //Sequence number
           << " mc "      << m_fmaxCount           //Number of fmax counts
           << " ac "      << m_ackSuppression      //Number of ack suppression counts
           << " fwin "    << fwin                  //fec window size
           << " nGroups " << uint32_t (nGroups)    //Number of groups
           << std::endl;
   }
 Simulator::Schedule (MilliSeconds (10.0), &FecAgent::TcpDump,this);
}
 
void
FecAgent::ScheduleTcpDump (void)
{  
  NS_LOG_FUNCTION (this);
  Simulator::Schedule (MilliSeconds (1.0), &FecAgent::TcpDump,this);
}

void
FecAgent::EnableTcpDump (Ptr<OutputStreamWrapper> stream)
{
  m_tcpDump = stream;
}

void
FecAgent::PrintTimeout (uint32_t timeout, uint32_t cwnd, SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);
  
  std::ostream * os = m_printTimeout->GetStream ();
  * os << m_srcId 
       << " "      << Simulator::Now ().GetSeconds ()
       << " to "   << timeout
       << " cwnd " << cwnd
       << " seq "  << seq
       << std::endl;
}

void
FecAgent::EnablePrintTimeout (Ptr<OutputStreamWrapper> stream)
{
  m_printTimeout = stream;
}

void
FecAgent::PrintFastRecovery (uint32_t fastRecovery, uint32_t cwnd, SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);
  
  std::ostream * os = m_printFastRecovery->GetStream ();
  * os << m_srcId 
       << " "      << Simulator::Now ().GetSeconds ()
       << " fr "   << fastRecovery
       << " cwnd " << cwnd
       << " seq "  << seq
       << std::endl;
}

void
FecAgent::EnablePrintFastRecovery (Ptr<OutputStreamWrapper> stream)
{
  m_printFastRecovery = stream;
}

void
FecAgent::PrintRetransmit (uint32_t retPkt, uint32_t cwnd, SequenceNumber32 seq)
{
  NS_LOG_FUNCTION (this);
  
  std::ostream * os = m_printRetransmit->GetStream ();
  * os << m_srcId 
       << " "      << Simulator::Now ().GetSeconds ()
       << " ret "  << retPkt
       << " cwnd " << cwnd
       << " seq "  << seq
       << std::endl;
}

void
FecAgent::EnablePrintRetransmit (Ptr<OutputStreamWrapper> stream)
{
  m_printRetransmit = stream;
}

void
FecAgent::PrintRateControl (uint32_t rateControl, uint32_t newcwnd, uint32_t oldcwnd, uint32_t ssThresh)
{
  NS_LOG_FUNCTION (this);
  
  std::ostream * os = m_printRateControl->GetStream ();
  * os << m_srcId 
       << " "          << Simulator::Now ().GetSeconds ()
       << " rc "       << rateControl
       << " new "      << newcwnd
       << " old "      << oldcwnd
       << " ssthresh " << ssThresh
       << std::endl;
}

void
FecAgent::EnablePrintRateControl (Ptr<OutputStreamWrapper> stream)
{
  m_printRateControl = stream;
}

void
FecAgent::PrintFwin (uint32_t rid, uint32_t fc, uint32_t ic, uint32_t cwnd)
{
  NS_LOG_FUNCTION (this);
 
  uint32_t fw = 0;
  uint32_t fw_s = 0;

  if (fc > 0 && ic > 0) 
    {
      fw = fc / ic;
      fw_s = fw * m_segmentSize;
    }
  if (fc == 0 && ic > 0)
    {
      fw = ic;
      fw_s = fw * m_segmentSize;
    }

  std::ostream * os = m_printFwin->GetStream ();
  * os << m_srcId 
       << " "      << Simulator::Now ().GetSeconds ()
       << " rid "  << rid
       << " ic "   << ic
       << " fc "   << fc
       << " fw "   << fw
       << " fw_s " << fw_s
       << " cwnd " << cwnd
       << std::endl;
}

void
FecAgent::EnablePrintFwin (Ptr<OutputStreamWrapper> stream)
{
  m_printFwin = stream;
}

bool
FecAgent::GetPrintFwin ()
{
  NS_LOG_FUNCTION (this);

  bool flag = false;
  if (m_printFwin) flag = true;

  return flag; 
}

void
FecAgent::PrintRoundFecTable ()
{
  NS_LOG_FUNCTION (this);

  std::list<PacketInfo>& pi = rt_.front().pi_;
  std::list<PacketInfo>::iterator itr;
  std::ostream * os = m_printRoundFecTable->GetStream ();
  * os << m_srcId
       << " rid " << rt_.front ().rid_;

  for (itr = pi.begin (); itr != pi.end (); itr++)
    {
    * os << " seq " << itr->serial_  
         << " "     << itr->check_;
      if (itr->type_ == false && itr->check_ == false) 
        {  
          m_lostdatapkt++;
          if (m_printLostPacket) 
            {
              PrintLostPacket (rt_.front ().rid_, itr->gid_, m_lostdatapkt, 
                               m_lostfecpkt, itr->serial_);
            }
         
        }
      if (itr->type_ == true && itr->check_ == false) 
        {
          m_lostfecpkt++;
          if (m_printLostPacket) 
            {
              PrintLostPacket (rt_.front ().rid_, itr->gid_, m_lostdatapkt,
                               m_lostfecpkt, itr->serial_);
            }
        }
    }
  * os << std::endl;
}
 
void
FecAgent::EnablePrintRoundFecTable (Ptr<OutputStreamWrapper> stream)
{
  m_printRoundFecTable = stream;
}

void
FecAgent::PrintGroupFecTable (uint32_t recover, uint8_t groupId)
{
  NS_LOG_FUNCTION (this);

  if (!gt_.front ().gi_.size ())
    {
          std::ostream * os = m_printGroupFecTable->GetStream ();
          * os << m_srcId
               << " rid "  << gt_.front().rid_
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
          * os << m_srcId
               << " rid "     << gt_.front().rid_
               << " gid "     << uint32_t (i)
               << " recover " << recover ;
         
          for (itr = pi.begin (); itr != pi.end (); itr++)
            {
              * os << " seq " << itr->serial_ 
                   << " "     << itr->check_;
            }
          * os << std::endl;
        }
    }
}
 
void
FecAgent::EnablePrintGroupFecTable (Ptr<OutputStreamWrapper> stream)
{
  m_printGroupFecTable = stream;
}


void
FecAgent::PrintLostPacket (uint32_t roundId, uint8_t groupId, 
                           uint32_t lostdatapkt, uint32_t lostfecpkt, 
                           uint32_t seq)
{
  NS_LOG_FUNCTION (this);

  std::ostream * os = m_printLostPacket->GetStream ();
  * os << m_srcId
       << " "      << Simulator::Now ().GetSeconds ()
       << " rid "  << roundId
       << " gid "  << uint32_t (groupId)
       << " data " << m_lostdatapkt
       << " fec "  << m_lostfecpkt
       << " seq "  << seq
       << std::endl; 
}
 
void
FecAgent::EnablePrintLostPacket (Ptr<OutputStreamWrapper> stream)
{
  m_printLostPacket = stream;
}

void
FecAgent::PrintRecovery (uint32_t recover, uint32_t roundId, uint8_t groupId)
{
  NS_LOG_FUNCTION (this);

  std::ostream * os = m_printRecovery->GetStream ();
  * os << m_srcId
       << " "         << Simulator::Now ().GetSeconds ()
       << " recover " << recover
       << " rid "     << roundId
       << " gid "     << uint32_t (groupId)
       << std::endl; 
}
 
void
FecAgent::EnablePrintRecovery (Ptr<OutputStreamWrapper> stream)
{
  m_printRecovery = stream;
}

} // /amespace ns3
