/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef FEC_TAG_H
#define FEC_TAG_H

#include <stdint.h>
#include "ns3/tag.h"

namespace ns3 {

class FecTag : public Tag
{
public:
  static TypeId GetTypeId (void);
  TypeId GetInstanceTypeId (void) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (TagBuffer i) const;
  void Deserialize (TagBuffer i);
  void Print (std::ostream &os) const;

  FecTag ();
  void SetType (uint8_t type);
  uint8_t GetType (void) const;
  void SetSerial (uint32_t serial);
  uint32_t GetSerial (void) const;
  void SetRoundId (uint32_t id);
  uint32_t GetRoundId (void) const;
  void SetGroupId (uint8_t id);
  uint8_t GetGroupId (void) const;
  void SetNGroups (uint8_t nGroups);
  uint8_t GetNGroups (void) const;

private:
  uint8_t m_type;     /*Type of packet*/
                      /* 0 = NONE
                       * 1 = Data packet
                       * 2 = Redundant packet
                       * 3 = Retransmission Packet 
                       * 4 = Recovered Packet 
                       * 5 = Retransmission Request Packet 
                       * 6 = Ack of Recovered Packet
                       */
  uint32_t m_serial; 
  uint32_t m_roundId; 
  uint8_t m_groupId; 
  uint8_t m_nGroups;
};

} // namespace ns3

#endif /* FEC_TAG_H */
