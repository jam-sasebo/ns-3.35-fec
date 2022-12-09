#!/bin/bash

dir="${1}/tcp-router-3/"
echo ${dir}
sumthp=0

for RngSeed in 1; do
    for nFlows in 10 20 50; do 
        for coredelayvalue in 100; do

                    echo "tcp-router-3 flow: ${nFlows}"
                    
                    DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/"

                    while read nodeId time time_v cwnd cwnd_v st st_v ack ack_v sent sent_v lost lost_v ret ret_v fec fec_v feclost feclost_v rec rec_v recret recret_v tout tout_v fastrec fastrec_v rec rec_v seq seq_v mc mc_v ac ac_v fwin fwin_v ngroups ngroups_v
                    do
                        sumthp=$(echo "${sumthp} + ${ack_v}/1000" | bc)
                    done < "${DIR}"TcpRouter.TcpDump

                    avethp=$(echo "scale=5; (${sumthp} * 1000 * 8) / (bc * 220 * 1000)" | bc)
                    
                    echo f ${nFlows}  thp ${avethp}
                    echo f ${nFlows}  thp ${avethp} >> tcp-router-3-throughput.dat

                    sumthp=0

        done
    done
done

mv tcp-router-3-throughput.dat "${dir}".