#!/bin/bash

NOW=$( date +"%F" )
dir="${NOW}/fec-end/"
echo ${dir}
sumthp=0

for RngSeed in 1; do
    for nFlows in 10; do

        if [ -f "./fec-router-3-f${nFlows}-throughput.dat" ]; then
            rm -f fec-router-3-f"${nFlows}"-throughput.dat
        fi

        for coredelay in 100 50 25 10 5; do
            for nGroups in 1 3 5 7 9; do
                # for fmax in 5 25 50 247; do   #fmaxを設定するかどうか
                    for fwin in 10 20 50 75 100; do
                        for slowdown in 0.8; do
                            mkdir -p "${dir}"s${RngSeed}
                            mkdir -p "${dir}"s${RngSeed}/f${nFlows}
                            mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}
                            mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}
                            mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}
                            mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}/wmax${fmax}

                            DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelay}/g${nGroups}/w${fwin}/wmax${fmax}/"

                            echo "./waf --run \"scratch/fec-end.cc --RngSeed=${RngSeed} --slowdown=${slowdown} --nFlows=${nFlows} --coredelay=${coredelay} --fwin=${fwin} --fmax=${fmax} --nGroups=${nGroups}\""
                            ./waf --run "scratch/fec-end.cc --RngSeed=${RngSeed} --slowdown=${slowdown} --nFlows=${nFlows} --coredelay=${coredelay} --fwin=${fwin} --nGroups=${nGroups}"
                            notify-send "f${nFlows}のシミュレーション終わった。結果処理をお願いします。"
                           
                            #cwndをプロットする
                            ./cwndplot.sh fec-end-3 
                            mv fec-end-3cwnd.eps "${dir}"fec-end-3-f"${nFlows}"-d"${coredelay}"-g"${nGroups}"-w"${fwin}"-cwnd.eps

                            #スループットを計算する
                            echo "fec-router-3 flow: ${nFlows} delay: ${coredelay} group: ${nGroups} fwin: ${fwin}"
                            sumthp=$(tail -n ${nFlows} FecEnd.TcpDump | awk -v f=${nFlows} -F' ' '{
                                sumthp += ( ($9-1) * 8 ) / ( 220 * 1000 * 1000 );
                            }END{
                                print sprintf("%0.2f",sumthp);
                            }')
                            echo f ${nFlows} d ${coredelay} g ${nGroups} w ${fwin} thp ${sumthp}
                            echo f ${nFlows} d ${coredelay} g ${nGroups} w ${fwin} thp ${sumthp} >> fec-end-3-f"${nFlows}"-throughput.dat
                            
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
                                mv fec-end-3-cwnd${i}.dat ${DIR}.
                            done
                        done
                    done
                # done
            done
        done
    mv fec-end-3-f"${nFlows}"-throughput.dat "${dir}".
    done
done
