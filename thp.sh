#!/bin/bash

dir="20220113/fec-end"
sumthp=0


for RngSeed in 1; do
 for nFlows in 50; do
  for coredelay in 150; do
   for nGroups in 1 2 3 4 5; do
    for fwin in 1 2 3 4 5 6 7 8 9 10; do

	echo "fec-end flow: ${nFlows} delay: ${coredelay} group: ${nGroups} fwin: ${fwin}"

    while read nodeId time time_v cwnd cwnd_v st st_v ack ack_v sent sent_v lost lost_v ret ret_v fec fec_v feclost feclost_v rec rec_v recret recret_v tout tout_v fastrec fastrec_v rec rec_v seq seq_v mc mc_v ac ac_v fwin fwin_v ngroups ngroups_v
    do
    	sumthp=$(echo "${sumthp} + ${cwnd_v}" | bc)

    done < ${dir}/s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}/FecRouter.TcpDump

    avethp=$(echo "scale=5; (${sumthp} * 8) / (50 * 220 * 1024 * 1024)" | bc)
    printf "f %d d %d g %d w %d thp %.7f\n" ${nFlows} ${coredelay} ${nGroups} ${fwin} ${avethp} >> throughput.dat

    sumthp=0

    done
   done
  done
 done
done
