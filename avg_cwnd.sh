#!/bin/bash

#平均cwndを計算する
dir="2022-11-10/tcp-router-3/"
echo "cwnd avg result" >> ${dir}cwnd_avg.dat

for RngSeed in 1; do
  for nFlows in 10 20 30 40 50; do
    for coredelayvalue in 5 10 25 50 100; do
      echo f:${nFlows} d:${coredelayvalue} 
      DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/"
      #各フローの平均cwndをファイルごとに計算し、結果ファイルに追加
      if [[ -f "${DIR}f${nFlows}-d${coredelayvalue}-avgcwnd.dat" ]];then
        rm -f "${DIR}f${nFlows}-d${coredelayvalue}-avgcwnd.dat"
      fi
      noff=$(( $nFlows - 1 ))
      for i in $(seq 0 ${noff})
      do
        # start processing when $2 (time) is above 20 seconds. syntax maybe something like if($2 > 20) do_something;
    	  awk -v N=4 -v Field_label="f${i}" '{
          if ($2 > 20) sum += $N/1000;c++ 
        } END { 
          if (NR > 0) print Field_label, sum/c
        }' "${DIR}"tcp-router-3-cwnd"${i}".dat >> "${DIR}"f"${nFlows}"-d"${coredelayvalue}"-avgcwnd.dat
      done

      #全フローの平均cwndを計算し、結果ファイルに追加
      awk -v N=2 '{ 
        sum += $N 
      } END { 
        if (NR > 0) print "Total_among_flows", sum/NR
      }' "${DIR}"f"${nFlows}"-d"${coredelayvalue}"-avgcwnd.dat >> "${DIR}"f"${nFlows}"-d"${coredelayvalue}"-avgcwnd.dat
      
      # #非効率的ですがこれより良い方法がなさそうでした。
      # tail -n 1 "${DIR}"f"${nFlows}"-d"${coredelayvalue}"-avgcwnd.dat | \
      # awk -v f="${nFlows}" -v d="${coredelayvalue}" '{print "f"f" "$2}' >> temp.dat
      
      #結果を全体的にまとめる
      echo "f ${nFlows} d ${coredelayvalue}" >> ${dir}cwnd_avg.dat
      cat "${DIR}"f"${nFlows}"-d"${coredelayvalue}"-avgcwnd.dat >> ${dir}cwnd_avg.dat
	  echo "------------------------------------" >> ${dir}cwnd_avg.dat
    printf "\n" >> ${dir}cwnd_avg.dat
    done
  done
done
# echo "------------------------------------" >> ${dir}cwnd_avg.dat
# awk -v N=1 '{ sum += $N } END { if (NR > 0) print "Total_avg", sum/NR}' temp.dat >> ${dir}cwnd_avg.dat
# rm -f temp.dat
