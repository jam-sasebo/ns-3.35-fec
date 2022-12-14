#!/bin/bash

dir="20220113/fec-end"

sumthp=0


for s in 1; do
 for f in 50; do
  for d in 150; do
   for g in 5; do
    for w in 10; do

	echo "fec-end flow: ${f} delay: ${d} group: ${g} fwin: ${w}"

	for i in $(seq 0 ${f});do

		grep "^${i} time" FecEnd.TcpDump >> FecEnd.Flow${i}.TcpDump

		while read nodeId time time_v cwnd cwnd_v st st_v ack ack_v sent sent_v lost lost_v ret ret_v fec fec_v feclost feclost_v rec rec_v recret recret_v tout tout_v fastrec fastrec_v rec rec_v seq seq_v mc mc_v ac ac_v fwin fwin_v ngroups ngroups_v
		do
			sumthp=$(echo "${sumthp} + ${cwnd_v}" | bc)

		done < FecEnd.Flow${i}.TcpDump
		
		avethp=$(echo "scale=5; ( ${sumthp} * 8 ) / ( 220 * 1024 * 1024 )" | bc)
		printf "flow %d thp %.3f\n" ${i} ${avethp} >> FE-throughput-f${f}d${d}g${g}w${w}.dat
		rm FecEnd.Flow${i}.TcpDump
		sumthp=0
	done


# transient
#while read nodeId time time_v cwnd cwnd_v st st_v ack ack_v sent sent_v lost lost_v ret ret_v fec fec_v feclost feclost_v rec rec_v recret recret_v tout tout_v fastrec fastrec_v rec rec_v seq seq_v mc mc_v ac ac_v fwin fwin_v ngroups ngroups_v
#do


#	transient=$(echo "${time_v} > 20.0" | bc)

#	if [ ${transient} -eq 1 ]; then
#		sumthp_tran=$(echo "${sumthp_tran} + ${cwnd_v}" | bc)
#	else
#		sumthp_steady=$(echo "${sumthp_steady} + ${cwnd_v}" | bc)
#	fi

	



#done < ${dir}/s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}/FecEnd.TcpDump
#done < FecEnd.TcpDump

 #avethp_tran=$(echo "scale=5; (${sumthp_tran} * 8) / (50 * 20 * 1024 * 1024)" | bc)
 #printf "transient thp %.3f\n" ${avethp_tran}

 #avethp_steady=$(echo "scale=5; (${sumthp_steady} * 8) / (50 * 200 * 1024 * 1024)" | bc)
 #printf "steady thp %.3f\n" ${avethp_steady}


 #sumthp_tran=0
 #sumthp_steady=0

    done
   done
  done
 done
done
