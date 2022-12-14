#!/bin/bash
NOW=$( date +"%F" )

dir="${NOW}/fec-router-2/"

seed=$(( 1 + $RANDOM % 10 ))
for RngSeed in $seed ; do
    for nFlows in 10 ; do 
        for coredelayvalue in 100; do
            for nGroups in 1 3 5 7 9 ; do
                for fwin in 5 25 50; do #change this value to 平通常TCPの平均cwnd 
                        #結果ディレクトリ
                        mkdir -p "${dir}"s${RngSeed}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}

                        DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}/"

                        echo "./waf --run \"scratch/fec-router-2.cc   --RngSeed=${RngSeed} --nFlows=${nFlows} --coredelay=${coredelayvalue} --fwin=${fwin} --nGroups=${nGroups}\""
                        #echo "./waf --run \"scratch/fec-router-2.cc --RngSeed=${RngSeed} --nFlows=${nFlows} --coredelay=${coredelayvalue}\""
                        
                        ./waf --run "scratch/fec-router-2.cc --RngSeed=${RngSeed} --nFlows=${nFlows} --coredelay=${coredelayvalue} --fwin=${fwin} --nGroups=${nGroups}"
                        #./waf --run "scratch/fec-router-2.cc --RngSeed=${RngSeed} --nFlows=${nFlows} --coredelay=${coredelayvalue}"

                        mv FecRouter-2.TcpDump "${DIR}".
                        mv FecRouter-2.Timeout "${DIR}".
                        mv FecRouter-2.FastRecovery "${DIR}".
                        mv FecRouter-2.Retransmit "${DIR}".
                        mv FecRouter-2.LostPacket "${DIR}".
                        mv FecRouter-2.SentPacket "${DIR}".
                        mv FecRouter-2.Recovery "${DIR}".
                        mv FecRouter-2.RoundFecTable "${DIR}".
                        mv FecRouter-2.GroupFecTable "${DIR}".

                        for i in $(seq 0 ${nFlows})
                        do
                          mv fec-router-2-cwnd"${i}".dat "${DIR}".
                        done

                    done
                done
            done
        done
    done
done