#!/bin/bash

NOW=$( date +"%F" )
dir="${NOW}/tcp-router/"
echo ${dir}

if [[ -f "./tcp-router-throughput.dat" ]]; then
  rm -f tcp-router-throughput.dat
fi

for RngSeed in 1; do
  for nFlows in 10 20; do
    for nRouters in $1; do
      for coredelayvalue in 100; do
        
        #結果ディレクトリを作成する
        mkdir -p "${dir}"s${RngSeed}
        mkdir -p "${dir}"s${RngSeed}/f${nFlows}
        mkdir -p "${dir}"s${RngSeed}/f${nFlows}/d${coredelayvalue}
        DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/"

        #シミュレーションを実行する
        echo "./waf --run \"scratch/tcp-router.cc --RngSeed=${RngSeed} --nRouters=${nRouters} --nFlows=${nFlows} --coredelay=${coredelayvalue} \" "
        ./waf --run "scratch/tcp-router.cc --RngSeed=${RngSeed} --nRouters=${nRouters} --nFlows=${nFlows} --coredelay=${coredelayvalue}" 
        # notify-send "f${nFlows}のシミュレーション終わった。結果処理をお願いします。"

        #cwndをプロットする
        ./cwndplot.sh tcp-router 
        mv tcp-routercwnd.eps "${dir}"tcp-router-f"${nFlows}"-r"${nRouters}"-d"${coredelayvalue}"-cwnd.eps

        #スループットを計算する
        sumthp=$(tail -n ${nFlows} TcpRouter.TcpDump | awk -v f=${nFlows} -F' ' '{
          sumthp+=(($9-1)*8)/(220 * 1000 * 1000);
        }END{
          print sprintf("%0.2f",sumthp);
        }')
        echo tcp-router f ${nFlows} r ${nRouters} d ${coredelayvalue} thp ${sumthp}
        echo f ${nFlows} r ${nRouters} d ${coredelayvalue} thp ${sumthp} >> tcp-router-throughput.dat

        #結果ファイルを移動
        mv TcpRouter.TcpDump "${DIR}".
        mv TcpRouter.Timeout "${DIR}".
        mv TcpRouter.FastRecovery "${DIR}".
        mv TcpRouter.Retransmit "${DIR}".
        mv TcpRouter.LostPacket "${DIR}".
        mv TcpRouter.SentPacket "${DIR}".
        mv TcpRouter.Recovery "${DIR}".

        for i in $(seq 0 ${nFlows})
        do
          mv tcp-router-cwnd"${i}".dat "${DIR}".
        done

      done
    done
  done
done

mv tcp-router-throughput.dat "${dir}".