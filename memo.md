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

Fwinに関する調査：
How fwin varies according to: delay d, group number g, fwin_max wmax.

     {
     ```
     ./calcTHP.sh /mnt/data/backup/r3/fec-end-3-f20 | grep fecend | perl -wlne "s|thp.*(?=wmax)||g; print;" 
     ```

     id fecend f  20  r  3  d  50 g  7  w  62 wmax 75
     1    2      3  4   5   6  7  8  9  10 11 12 13    14  

     ./calcTHP.sh /mnt/data/backup/r3/fec-end-3-f20 | grep fecend | perl -wlne "s|thp.*(?=wmax)||g; print;" | awk '{print $8,"+",$10,"+",$14,"=",($8+$10+$14),"\t",$12,"w/d",sprintf("%0.f",$12/$10)}' | sort -n
     10 + 1 + 10 = 21     9 w/d 9
     10 + 1 + 100 = 111   32 w/d 32
     10 + 1 + 20 = 31     17 w/d 17
     10 + 1 + 50 = 61     29 w/d 29
     10 + 1 + 75 = 86     31 w/d 31
     10 + 3 + 10 = 23     20 w/d 7
     10 + 3 + 100 = 113   29 w/d 10
     10 + 3 + 20 = 33     27 w/d 9
     10 + 3 + 50 = 63     29 w/d 10
     10 + 3 + 75 = 88     29 w/d 10
     10 + 5 + 10 = 25     23 w/d 5
     10 + 5 + 100 = 115   25 w/d 5
     10 + 5 + 20 = 35     25 w/d 5
     10 + 5 + 50 = 65     25 w/d 5
     10 + 5 + 75 = 90     25 w/d 5
     10 + 7 + 10 = 27     23 w/d 3
     10 + 7 + 20 = 37     23 w/d 3
     10 + 7 + 50 = 67     20 w/d 3
     25 + 1 + 10 = 36     9 w/d 9
     25 + 1 + 100 = 126   45 w/d 45
     25 + 1 + 20 = 46     18 w/d 18
     25 + 1 + 50 = 76     37 w/d 37
     25 + 1 + 75 = 101    43 w/d 43
     25 + 3 + 10 = 38     23 w/d 8
     25 + 3 + 100 = 128   45 w/d 15
     25 + 3 + 20 = 48     36 w/d 12
     25 + 3 + 50 = 78     44 w/d 15
     25 + 3 + 75 = 103    45 w/d 15
     25 + 5 + 10 = 40     30 w/d 6
     25 + 5 + 100 = 130   40 w/d 8
     25 + 5 + 20 = 50     39 w/d 8
     25 + 5 + 50 = 80     40 w/d 8
     25 + 5 + 75 = 105    40 w/d 8
     25 + 7 + 10 = 42     33 w/d 5
     25 + 7 + 100 = 132   36 w/d 5
     25 + 7 + 20 = 52     36 w/d 5
     25 + 7 + 50 = 82     36 w/d 5
     25 + 7 + 75 = 107    36 w/d 5
     25 + 9 + 10 = 44     33 w/d 4
     25 + 9 + 100 = 134   33 w/d 4
     25 + 9 + 20 = 54     33 w/d 4
     25 + 9 + 50 = 84     33 w/d 4
     25 + 9 + 75 = 109    33 w/d 4
     50 + 1 + 10 = 61     9 w/d 9
     50 + 1 + 100 = 151   65 w/d 65
     50 + 1 + 20 = 71     18 w/d 18
     50 + 1 + 50 = 101    43 w/d 43
     50 + 1 + 75 = 126    57 w/d 57
     50 + 3 + 10 = 63     25 w/d 8
     50 + 3 + 100 = 153   70 w/d 23
     50 + 3 + 20 = 73     44 w/d 15
     50 + 3 + 50 = 103    68 w/d 23
     50 + 3 + 75 = 128    71 w/d 24
     50 + 5 + 10 = 65     37 w/d 7
     50 + 5 + 100 = 155   66 w/d 13
     50 + 5 + 20 = 75     57 w/d 11
     50 + 5 + 50 = 105    66 w/d 13
     50 + 5 + 75 = 130    66 w/d 13
     50 + 7 + 10 = 67     44 w/d 6
     50 + 7 + 100 = 157   62 w/d 9
     50 + 7 + 20 = 77     59 w/d 8
     50 + 7 + 50 = 107    61 w/d 9
     50 + 7 + 75 = 132    62 w/d 9
     50 + 9 + 10 = 69     46 w/d 5
     50 + 9 + 100 = 159   55 w/d 6
     50 + 9 + 20 = 79     55 w/d 6
     50 + 9 + 50 = 109    55 w/d 6
     50 + 9 + 75 = 134    55 w/d 6
     100 + 1 + 10 = 111   9 w/d 9
     100 + 1 + 100 = 201       81 w/d 81
     100 + 1 + 20 = 121   18 w/d 18
     100 + 1 + 50 = 151   45 w/d 45
     100 + 1 + 75 = 176   66 w/d 66
     100 + 3 + 10 = 113   27 w/d 9
     100 + 3 + 100 = 203       120 w/d 40
     100 + 3 + 20 = 123   51 w/d 17
     100 + 3 + 50 = 153   103 w/d 34
     100 + 3 + 75 = 178   119 w/d 40
     100 + 5 + 10 = 115   42 w/d 8
     100 + 5 + 100 = 205       117 w/d 23
     100 + 5 + 20 = 125   68 w/d 14
     100 + 5 + 50 = 155   113 w/d 23
     100 + 5 + 75 = 180   116 w/d 23
     100 + 7 + 10 = 117   54 w/d 8
     100 + 7 + 100 = 207       112 w/d 16
     100 + 7 + 20 = 127   91 w/d 13
     100 + 7 + 50 = 157   113 w/d 16
     100 + 7 + 75 = 182   113 w/d 16
     100 + 9 + 10 = 119   62 w/d 7
     100 + 9 + 100 = 209       107 w/d 12
     100 + 9 + 20 = 129   92 w/d 10
     100 + 9 + 50 = 159   108 w/d 12
     100 + 9 + 75 = 184   107 w/d 12

How it varies according to group number alone (g=1,3,d=100)
     ```./calcTHP.sh Data/ | grep fecend | awk '{print "fec-end","g",$10,"fwin",$12,"fwin/g",sprintf("%0.f",$12/$10)}' | sort | uniq -c ```
          24 fec-end g 1 fwin 32 fwin/g 32
           1 fec-end g 3 fwin 82 fwin/g 27
           1 fec-end g 3 fwin 83 fwin/g 28
           1 fec-end g 3 fwin 84 fwin/g 28
           1 fec-end g 3 fwin 85 fwin/g 28
           1 fec-end g 3 fwin 86 fwin/g 29
           2 fec-end g 3 fwin 87 fwin/g 29
           1 fec-end g 3 fwin 88 fwin/g 29
           2 fec-end g 3 fwin 89 fwin/g 30
           1 fec-end g 3 fwin 91 fwin/g 30
           1 fec-end g 3 fwin 92 fwin/g 31
           1 fec-end g 3 fwin 93 fwin/g 31
           1 fec-end g 3 fwin 94 fwin/g 31
          10 fec-end g 3 fwin 95 fwin/g 32



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
