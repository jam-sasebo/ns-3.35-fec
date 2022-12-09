#!/bin/bash
LC_NUMERIC=C 
LC_COLLATE=C

dir="${1}/fec-router-3/"

echo ${dir}
sumthp=0

for RngSeed in 1; do
    for nFlows in 10 20 50; do 
        for coredelayvalue in 100; do
            for nGroups in 1 3 5 7 9 ; do
                for fwin in 5 25 50; do 

                    echo "fec-router-3 flow: ${nFlows} delay: ${coredelayvalue} group: ${nGroups} fwin: ${fwin}"
                    
                    DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}/"

                    while read nodeId time time_v cwnd cwnd_v st st_v ack ack_v sent sent_v lost lost_v ret ret_v fec fec_v feclost feclost_v rec rec_v recret recret_v tout tout_v fastrec fastrec_v rec rec_v seq seq_v mc mc_v ac ac_v fwin fwin_v ngroups ngroups_v
                    do
                        sumthp=$(echo "${sumthp} + ${cwnd_v}" | bc)
                    done < "${DIR}"/FecRouter.TcpDump

                    avethp=$(echo "scale=5; (${sumthp} * 8) / (50 * 220 * 1024 * 1024)" | bc)
                    
                    echo f ${nFlows} d ${coredelayvalue} g ${nGroups} w ${fwin} thp ${avethp}
                    echo f ${nFlows} d ${coredelayvalue} g ${nGroups} w ${fwin} thp ${avethp} >> fec-router-3-f"${nFlows}"-throughput.dat
                    
                    mv fec-router-3-f"${nFlows}"-throughput.dat "${dir}".

                    sumthp=0

                done
            done
        done
    done
done