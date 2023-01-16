#!/bin/bash

NOW=$( date +"%F" )
dir="${NOW}/fec-end/"
echo ${dir}

for RngSeed in 1; do
    for nRouters in $1; do
        for nFlows in 10; do 

            if [ -f "./fec-end-f${nFlows}-throughput.dat" ]; then
                rm -f fec-end-f"${nFlows}"-throughput.dat

            for coredelay in 100; do
                for nGroups in 1 3 ; do
                    # for fmax in 5 25 50 247; do   #fmaxを設定するかどうか
                        for fwin in 10 20 ; do
                            for slowdown in 0.8; do
                                mkdir -p "${dir}"s${RngSeed}
                                mkdir -p "${dir}"s${RngSeed}/f${nFlows}
                                mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}
                                mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}
                                mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}
                                mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}/wmax${fmax}

                                DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}/wmax${fmax}/"

                                echo "./waf --run \"scratch/burst-error-model-fec-end.cc --RngSeed=${RngSeed} --nRouters=${nRouters} --slowdown=${slowdown} --nFlows=${nFlows} --coredelay=${coredelay} --fwin=${fwin} --fmax=${fmax} --nGroups=${nGroups}\""
                                ./waf --run "scratch/burst-error-model-fec-end.cc --RngSeed=${RngSeed} --nRouters=${nRouters} --slowdown=${slowdown} --nFlows=${nFlows} --coredelay=${coredelay} --fwin=${fwin} --nGroups=${nGroups} --burstSize=10 --burstRate=0.001"
                                notify-send "f${nFlows}のシミュレーション終わった。結果処理をお願いします。"
                               
                                #cwndをプロットする
                                ./cwndplot.sh fec-end
                                mv fec-endcwnd.eps "${dir}"fec-end-f"${nFlows}"-d"${coredelay}"-g"${nGroups}"-w"${fwin}"-cwnd.eps

                                #スループットを計算する
                                echo "fec-end flow: ${nFlows} routers: ${nRouters} delay: ${coredelay} group: ${nGroups} fwin: ${fwin}"
                                sumthp=$(tail -n ${nFlows} FecEnd.TcpDump | awk -v f=${nFlows} -F' ' '{
                                    sumthp += ( ($9-1) * 8 ) / ( 220 * 1000 * 1000 );
                                }END{
                                    print sprintf("%0.2f",sumthp);
                                }')
                                echo f ${nFlows} r ${nRouters} d ${coredelay} g ${nGroups} w ${fwin} thp ${sumthp}
                                echo f ${nFlows} r ${nRouters} d ${coredelay} g ${nGroups} w ${fwin} thp ${sumthp} >> fec-end-f"${nFlows}"-throughput.dat
                                
                                sumthp=0


                                mv FecEnd.TcpDump ${DIR}.
                                mv FecEnd.Timeout ${DIR}.
                                mv FecEnd.FastRecovery ${DIR}.
                                mv FecEnd.Retransmit ${DIR}.
                                mv FecEnd.RateControl ${DIR}.
                                mv FecEnd.Fwin ${DIR}.
                                mv FecEnd.RoundFecTable ${DIR}.
                                mv FecEnd.GroupFecTable ${DIR}.
                                mv FecEnd.LostPacket ${DIR}.
                                mv FecEnd.Recovery ${DIR}.

                                for i in $(seq 0 ${nFlows})
                                do
                                    mv fec-end-cwnd${i}.dat ${DIR}.
                                done
                            done
                        done
                    # done
                done
            done
        mv fec-end-f"${nFlows}"-throughput.dat "${dir}".
        done
    done
done
