#!/bin/bash

#スループットを計算する
nFlows=10
coredelayvalue=100
nGroups=1
fwin=50
sumthp=0

line_cnt=$(cat FecRouter.TcpDump| wc -l)
cnt=0
echo "fec-router-3 f ${nFlows} d ${coredelayvalue} g ${nGroups} w ${fwin}"

while read nodeId time time_v cwnd cwnd_v st st_v ack ack_v sent sent_v lost lost_v ret ret_v fec fec_v feclost feclost_v rec rec_v recret recret_v tout tout_v fastrec fastrec_v rc rc_v seq seq_v mc mc_v ac ac_v fwin fwin_v ngroups ngroups_v
do
    sumthp=$(echo "${sumthp} + ${ack_v}" | bc)
    cnt=$(( $cnt + 1 ))
    progress=$(echo "scale=2; (${cnt} / ${line_cnt}) * 100 " | bc -l)
    progress=$(echo $progress | awk '{rounded=sprintf("%.0f", $1); print rounded}')
    printf "sumthp: ${sumthp} ${progress}%% \r"
done < FecRouter.TcpDump
echo avethp
avethp=$(echo "scale=5; (${sumthp} * 8) / ( 220 * 1000 * 1000)" | bc)
# avethp=$(echo "scale=5; ($RANDOM * 100000 * 8) / ( 220 * 1000 * 1000)" | bc) #ディバッグのため
echo avethp ${avethp}
echo f ${nFlows} d ${coredelayvalue} g ${nGroups} w ${fwin} thp ${avethp}
echo f ${nFlows} d ${coredelayvalue} g ${nGroups} w ${fwin} thp ${avethp} \ 
    >> fec-router-3-f"${nFlows}"-g"${nGroups}"-w"${fwin}"-throughput.dat
sumthp=0