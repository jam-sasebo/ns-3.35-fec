/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef FEC_ROUTER_AGENT_H
#define FEC_ROUTER_AGENT_H

#include <map>
#include <queue>
#include "fec-tag.h"
#include "ns3/enum.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/sequence-number.h"

namespace ns3 {

class FecRouterAgent: public Object
{

 struct PacketInfo
  {
   uint32_t serial_;
   bool check_;
   uint32_t gid_;
   bool type_;
   TcpHeader tcp_;
   Ipv4Header ip_;
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
   std::map<uint32_t, PacketList> gi_;
  };

 struct TransitQueue
  {
    uint32_t serial_;
    Ptr<Packet> p_;
  };

 enum SendRxRequest
  {
    ON = 1,
    OFF = 0,
  }; 
 
 enum StoreMode
  {
    USE = 1,
    DISUSE = 0,
  }; 

public:
 
  static TypeId GetTypeId (void);
  FecRouterAgent ();

  uint32_t GetFwin ();
  uint8_t GetNGroups ();
  uint32_t GetSegmentSize ();

  Ptr<Packet> CreateDataPacket (Ptr<Packet> packet, Ipv4Header ipHeader);
  Ptr<Packet> CreateFecPacket (Ptr<Packet> packet, Ipv4Header ipHeader);
  void CreateRecoveryPacket (Ptr<Packet> packet, TcpHeader tcpHeader,
                            Ipv4Header ipHeader, FecTag tag, uint32_t serial);

  uint32_t Recv (Ptr<Packet> packet, Ipv4Header ipHeader);

  void UpdateFecTable ();
  void MarkRoundFecTable (Ptr<Packet> packet);
  uint32_t CheckRoundFecTable (Ptr<Packet> packet);
  uint32_t MarkGroupFecTable (Ptr<Packet> packet);
  uint32_t CheckGroupFecTable (Ptr<Packet> packet, uint32_t nlost);

  uint32_t TransitQueueSize ();
  Ptr<Packet> GetPacket ();
  void InsertTransitQueue (Ptr<Packet> packet, uint32_t serial);

  void LostPacketInfo (uint32_t roundId, uint8_t groupId, uint32_t serial,
                       TcpHeader tcpHeader, Ipv4Header ipHeader);
  uint32_t LostPacketQueueSize ();
  Ptr<Packet> GetLostPacket ();

  void PrintRoundFecTable ();
  static void EnablePrintRoundFecTable (Ptr<OutputStreamWrapper> stream);
  void PrintGroupFecTable (uint32_t recover, uint8_t groupId);
  static void EnablePrintGroupFecTable (Ptr<OutputStreamWrapper> stream);
  void PrintRecovery (uint32_t recover, uint32_t roundId, uint8_t groupId);
  static void EnablePrintRecovery (Ptr<OutputStreamWrapper> stream);
  void PrintLostPacket (uint32_t roundId, uint8_t groupId, 
                        uint32_t lostdatepkt, uint32_t lostfecpkt,
                        SequenceNumber32 seq,
                        Ipv4Address src, Ipv4Address dst);
  static void EnablePrintLostPacket (Ptr<OutputStreamWrapper> stream);
  void PrintSentPacket (uint32_t roundId, uint8_t groupId, uint32_t serial,
                        uint32_t lostdatepkt, uint32_t lostfecpkt,
                        Ipv4Address src, Ipv4Address dst,
                        SequenceNumber32 seq);
  static void EnablePrintSentPacket (Ptr<OutputStreamWrapper> stream);

private:
 SendRxRequest m_sendRxRequest; 
 StoreMode m_storeMode; 

 std::queue<RoundFecTable> rt_;
 std::queue<GroupFecTable> gt_;
 std::list<TransitQueue> tq_;
 std::queue<Ptr<Packet>> lpq_;
 static Ptr<OutputStreamWrapper> m_printRoundFecTable;
 static Ptr<OutputStreamWrapper> m_printGroupFecTable;
 static Ptr<OutputStreamWrapper> m_printRecovery;
 static Ptr<OutputStreamWrapper> m_printLostPacket;
 static Ptr<OutputStreamWrapper> m_printSentPacket;

 uint32_t m_fwin;        /*Fec Window Size*/
 uint8_t m_nGroups;  
 uint32_t m_segmentSize;
 uint32_t m_serial;
 uint32_t m_roundId;
 uint8_t m_groupId;
 uint32_t m_sentdatapkt;
 uint32_t m_lostdatapkt;
 uint32_t m_sentfecpkt;
 uint32_t m_lostfecpkt;
 uint32_t m_recover;
};

}

#endif /* FEC_ROUTER_AGENT_H */
