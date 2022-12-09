/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Koga Laboratory
 *
 */

#ifndef FEC_AGENT_H
#define FEC_AGENT_H

#include <queue>
#include <stdint.h>
#include "fec-tag.h"
#include "ns3/ptr.h"
#include "ns3/enum.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/sequence-number.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

class FecAgent : public Object
{
  struct PacketInfo
  {
    uint32_t serial_; //sequence number
    bool rexmit_;
    bool check_;
    uint8_t gid_;
    bool type_;
    TcpHeader tcp_;
    bool flag_;
  };
  
  struct PacketList
  {
    std::list<PacketInfo> pi_;
  };
  
  struct RoundFecTable
  {
    uint32_t rid_;
    uint8_t nGroups_;
    std::list<PacketInfo> pi_;
  };

  struct GroupFecTable
  {
    uint32_t rid_;
    uint8_t nGroups_;
    std::map<uint32_t, PacketList> gi_; //groupinfo
  };

 struct TransitQueue
  {
    uint32_t serial_;
    Ptr<Packet> p_;
  };

  enum FecMode
    {
      Dynamic = 1,
      Static = 0,
    };
  
  enum BoundMode
    {
      IN = 1,
      OUT = 0,
    };
  
  enum AckMode
    {
      Suppression = 1,
      Send = 0,
    };
  
public:
  static TypeId GetTypeId (void); 
  FecAgent ();
  virtual ~FecAgent ();

  /*Encoder*/
  Ptr<Packet> CreateDataPacket (Ptr<Packet> packet, TcpHeader tcpHeader,
                                SequenceNumber32 highTxMark);
  Ptr<Packet> CreateFecPacket ();

  uint32_t GetFwin ();

  uint32_t RedundancyControl (uint32_t cwnd);
  uint32_t GetNGroups ();
  void Reset ();

  uint32_t GetSrcId ();
  double GetRateControlValue ();

  /*Decoder*/
  uint32_t Recv (Ptr<Packet> packet, Ipv4Header ipHeader);
  void MarkRoundFecTable (Ptr<Packet> packet);
  uint32_t CheckRoundFecTable (Ptr<Packet> packet);
  uint32_t MarkGroupFecTable (Ptr<Packet> packet);
  uint32_t CheckGroupFecTable (Ptr<Packet> packet, uint32_t nlost);
  void InsertTransitQueue (Ptr<Packet> packet, uint32_t serial);
  uint32_t TransitQueueSize ();
  Ptr<Packet> GetPacket ();
  void CreateRecoveryPacket (Ptr<Packet> packet, TcpHeader tcpHeader,
                             FecTag tag, uint32_t serial);



 /*Trace*/
  void SetTcpDumpInfo (uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq, SequenceNumber32 lastAckedSeq);
  void TcpDump (void);
  void ScheduleTcpDump (void);
  static void EnableTcpDump (Ptr<OutputStreamWrapper> stream);

  void SetTimeoutInfo (uint32_t timeout, uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq);
  void PrintTimeout (uint32_t timeout, uint32_t cwnd, SequenceNumber32 seq);
  static void EnablePrintTimeout (Ptr<OutputStreamWrapper> stream);

  void SetFastRecoveryInfo (uint32_t fastRecovery, uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq);
  void PrintFastRecovery (uint32_t fastRecovery, uint32_t cwnd, SequenceNumber32 seq);
  static void EnablePrintFastRecovery (Ptr<OutputStreamWrapper> stream);

  void SetRetransmitInfo (uint32_t retransmit, uint32_t cwnd, uint32_t ssThresh, SequenceNumber32 seq);
  void PrintRetransmit (uint32_t retransmit, uint32_t cwnd, SequenceNumber32 seq);
  static void EnablePrintRetransmit (Ptr<OutputStreamWrapper> stream);

  void SetRateControlInfo (uint32_t rateControl, uint32_t newcwnd, uint32_t oldcwnd, uint32_t ssThresh);
  void PrintRateControl (uint32_t rateControl, uint32_t cwnd, uint32_t oldcwnd, uint32_t ssThresh);
  static void EnablePrintRateControl (Ptr<OutputStreamWrapper> stream);

  void PrintFwin (uint32_t rid, uint32_t fwin, uint32_t ic, uint32_t cwnd);
  static void EnablePrintFwin (Ptr<OutputStreamWrapper> stream);
  bool GetPrintFwin ();

  void PrintRoundFecTable ();
  static void EnablePrintRoundFecTable (Ptr<OutputStreamWrapper> stream);

  void PrintGroupFecTable (uint32_t recover, uint8_t groupId);
  static void EnablePrintGroupFecTable (Ptr<OutputStreamWrapper> stream);

  void PrintLostPacket (uint32_t roundId, uint8_t groupId, 
                        uint32_t lostdatepkt, uint32_t lostfecpkt,
                        uint32_t seq);
  static void EnablePrintLostPacket (Ptr<OutputStreamWrapper> stream);

  void PrintRecovery (uint32_t recover, uint32_t roundId, uint8_t groupId);
  static void EnablePrintRecovery (Ptr<OutputStreamWrapper> stream);

private:
  static Ptr<OutputStreamWrapper> m_tcpDump;
  static Ptr<OutputStreamWrapper> m_printTimeout;
  static Ptr<OutputStreamWrapper> m_printFastRecovery;
  static Ptr<OutputStreamWrapper> m_printRetransmit;
  static Ptr<OutputStreamWrapper> m_printRateControl;
  static Ptr<OutputStreamWrapper> m_printFwin;
  static Ptr<OutputStreamWrapper> m_printRoundFecTable;
  static Ptr<OutputStreamWrapper> m_printGroupFecTable;
  static Ptr<OutputStreamWrapper> m_printLostPacket;
  static Ptr<OutputStreamWrapper> m_printRecovery;

  std::queue<RoundFecTable> rt_;
  std::queue<GroupFecTable> gt_;
  std::list<TransitQueue> tq_;

  FecMode m_fecMode;
  BoundMode m_boundMode;
  AckMode m_ackMode;

  uint32_t m_fwin;
  uint32_t m_fmax;
  uint32_t m_srcId;
  uint32_t m_dstId;
  uint32_t m_roundId;
  uint8_t  m_groupId;
  uint8_t  m_nGroups;
  uint32_t m_serial;
  uint32_t m_segmentSize;
  double m_rateControlValue;
 
  uint32_t m_cwnd;
  uint32_t m_ssThresh;
  SequenceNumber32 m_seq;
  SequenceNumber32 m_lastAckedSeq;
  uint32_t m_sentdatapkt;
  uint32_t m_lostdatapkt;
  uint32_t m_retPkt;
  uint32_t m_sentfecpkt;
  uint32_t m_lostfecpkt;
  uint32_t m_recover;
  uint32_t m_recover_rexmit;
  uint32_t m_timeout;
  uint32_t m_fastRecovery;
  uint32_t m_rateControl;
  uint32_t m_fmaxCount;
  uint32_t m_ackSuppression;

  uint32_t ic;
  uint32_t fwin;
  uint32_t ackSendId;
  uint8_t  nGroups;
};

} // namespace ns3

#endif /* FEC_AGENT_H */
