Print fields and their values
```tail -n 1 [FILENAME] | awk '{ for(n=2;n<NF;n++){ print "$"n,$n,"\t","$"n+1,$(n+1);n++}}' ```
.TcpDumpの形式
$2 time    $3 219.991
$4 cwnd    $5 97277
$6 st      $7 60500
$8 ack     $9 83094001
$10 sent   $11 83375
$12 lost   $13 191
$14 ret    $15 184
$16 fec    $17 2468
$18 feclost     $19 33
$20 rec    $21 7
$22 rec_ret     $23 0
$24 to     $25 0
$26 fr     $27 6
$28 rc     $29 6
$30 seq    $31 83190001
$32 mc     $33 671
$34 ac     $35 773
$36 fwin   $37 93
$38 nGroups     $39 3

bashで作成した指標計算結果ファイルの形式
$1 id      $2 tcprouter
$3 f       $4 20
$5 r       $6 7
$7 d       $8 100
$9 g       $10 0
$11 w      $12 0
$13 thp    $14 76.45
$15 retr   $16 0.159
$17 to     $18 9
$19 redun       $20 0.000
$21 mc     $22 0
$23 lossr       $24 0.000
$25 effrec      $26 0.00000
$27 fr     $28 9.3


FecAgent::TcpDump 
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

FecRouterAgent::PrintSentPacket
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

FecRouterAgent::PrintLostPacket
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
 
FecAgent::PrintRecovery 
  * os << m_srcId
       << " "         << Simulator::Now ().GetSeconds ()
       << " recover " << recover
       << " rid "     << roundId
       << " gid "     << uint32_t (groupId)
       << std::endl; 

FecAgent::PrintRetransmit
  * os << m_srcId 
       << " "      << Simulator::Now ().GetSeconds ()
       << " ret "  << retPkt
       << " cwnd " << cwnd
       << " seq "  << seq
       << std::endl;

FecAgent::PrintTimeout 
  * os << m_srcId 
       << " "      << Simulator::Now ().GetSeconds ()
       << " to "   << timeout
       << " cwnd " << cwnd
       << " seq "  << seq
       << std::endl;
}