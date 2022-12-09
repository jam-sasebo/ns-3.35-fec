#include "fec-tag.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (FecTag);

TypeId 
FecTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FecTag")
    .SetParent<Tag> ()
    .AddConstructor<FecTag> ()
  ;
  return tid;
}

TypeId
FecTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
FecTag::GetSerializedSize (void) const
{
  return sizeof (m_type)
   + sizeof (m_serial)
   + sizeof (m_roundId)
   + sizeof (m_groupId)
   + sizeof (m_nGroups)
  ;
}

void
FecTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_type);
  i.WriteU32 (m_serial);
  i.WriteU32 (m_roundId);
  i.WriteU8 (m_groupId);
  i.WriteU8 (m_nGroups);
}

void
FecTag::Deserialize (TagBuffer i)
{
  m_type = i.ReadU8 ();
  m_serial = i.ReadU32 ();
  m_roundId = i.ReadU32 ();
  m_groupId = i.ReadU8 ();
  m_nGroups = i.ReadU8 ();
}

void
FecTag::Print (std::ostream &os) const
{
  os << "Type: ";
  switch (m_type)
  {
  case 0:
    os << "NONE";
  case 1:
    os << "DATA";
  case 2:
    os << "REDUNDANT";
  case 3:
    os << "RETRASMISSION";
  case 4:
    os << "RECOVERED";
  case 5:
    os << "RxREQUEST";
  }

  os << "Serial: "   << static_cast<uint32_t> (m_serial)
     << "RoundId: "  << static_cast<uint32_t> (m_roundId)
     << "GroupId: "  << static_cast<uint8_t> (m_groupId)
     << "nGroups: "  << static_cast<uint8_t> (m_nGroups)
  ;
}

FecTag::FecTag ()
  : Tag (),
    m_serial (0),
    m_roundId (0),
    m_groupId (0),
    m_nGroups (0)
{
}

void
FecTag::SetType (uint8_t type)
{
  m_type = type;
}

uint8_t
FecTag::GetType (void) const
{
  return m_type;
}

void 
FecTag::SetSerial (uint32_t serial)
{
  m_serial = serial;
}

uint32_t
FecTag::GetSerial (void) const
{
  return m_serial;
}

void 
FecTag::SetRoundId (uint32_t id)
{
  m_roundId = id;
}

uint32_t
FecTag::GetRoundId (void) const
{
  return m_roundId;
}

void 
FecTag::SetGroupId (uint8_t id)
{
  m_groupId = id;
}

uint8_t
FecTag::GetGroupId (void) const
{
  return m_groupId;
}

void 
FecTag::SetNGroups (uint8_t nGroups)
{
  m_nGroups = nGroups;
}

uint8_t
FecTag::GetNGroups (void) const
{
  return m_nGroups;
}

} // namespace ns3
