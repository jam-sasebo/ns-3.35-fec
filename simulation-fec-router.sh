#!/bin/bash 

#実行したあとにちゃんとディレクトリが作られたかどうかを確認！！！！！！！！！！

NOW=$( date +"%F" )
dir="${NOW}/fec-router/"
echo ${dir}

for RngSeed in 1; do
    for nRouters in $1; do
        for nFlows in 10 20; do 

            if [ -f "./fec-router-f${nFlows}-throughput.dat" ]; then
                rm -f fec-router-f"${nFlows}"-throughput.dat
            fi

            for coredelayvalue in 100; do #5 10 25 50 100
                for nGroups in 1 3; do #1 3 5 7 9
                    for fwin in 10; do #10 20 50 75 100

                        #結果ディレクトリを作成する
                        mkdir -p "${dir}"s${RngSeed}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}
                        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}
                        DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}/"
                        
                        #シミュレーションを実行する
                        echo "./waf --run \"scratch/fec-router.cc --RngSeed=${RngSeed} --nRouters=${nRouters} --nFlows=${nFlows} --coredelay=${coredelayvalue} --fwin=${fwin} --nGroups=${nGroups}\" "
                        ./waf --run "scratch/fec-router.cc --RngSeed=${RngSeed} --nRouters=${nRouters} --nFlows=${nFlows} --coredelay=${coredelayvalue} --fwin=${fwin} --nGroups=${nGroups}" 
                        notify-send "f${nFlows}のシミュレーション終わった。結果処理をお願いします。"

                        #cwndをプロットする
                        ./cwndplot.sh fec-router
                        mv fec-routercwnd.eps "${dir}"fec-router-f"${nFlows}"-d"${coredelayvalue}"-g"${nGroups}"-w"${fwin}"-cwnd.eps
                        
                        #スループットを計算する
                        echo "fec-router flow: ${nFlows} routers: ${nRouters} delay: ${coredelayvalue} group: ${nGroups} fwin: ${fwin}"
                        sumthp=$(tail -n ${nFlows} FecRouter.TcpDump | awk -v f=${nFlows} -F' ' '{
                            sumthp += ( ($9-1) * 8 ) / ( 220 * 1000 * 1000 );
                        }END{
                            print sprintf("%0.2f",sumthp);
                        }')
                        echo f ${nFlows} r ${nRouters} d ${coredelayvalue} g ${nGroups} w ${fwin} thp ${sumthp}
                        echo f ${nFlows} r ${nRouters} d ${coredelayvalue} g ${nGroups} w ${fwin} thp ${sumthp} >> fec-router-f"${nFlows}"-throughput.dat
                        sumthp=0


                        #結果ファイルを移動
                        mv FecRouter.TcpDump "${DIR}".
                        mv FecRouter.Timeout "${DIR}".
                        mv FecRouter.FastRecovery "${DIR}".
                        mv FecRouter.Retransmit "${DIR}".
                        mv FecRouter.LostPacket "${DIR}".
                        mv FecRouter.SentPacket "${DIR}".
                        mv FecRouter.Recovery "${DIR}".
                        mv FecRouter.RoundFecTable "${DIR}".
                        mv FecRouter.GroupFecTable "${DIR}".
                        for i in $(seq 0 ${nFlows})
                        do
                            mv fec-router-cwnd"${i}".dat "${DIR}".
                        done

                    done
                done
            done
        
        mv fec-router-f"${nFlows}"-throughput.dat "${dir}".

        done
    done
done