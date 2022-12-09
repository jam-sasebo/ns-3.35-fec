#!/bin/bash

#平均「再送率」を計算する

dir="2022-11-25/fec-end/"

for RngSeed in 1; do  #シード番号をよく注意を！！！！！
	for nFlows in 10 ; do
		if [[ -f "${dir}fec-end-f${nFlows}-retrate.dat" ]];then
			rm -f "${dir}fec-end-f${nFlows}-retrate.dat"
		fi
		for coredelayvalue in 5 10 25 50 100; do
			for nGroups in 1 3 5 7 9; do
                for fwin in 2; do
                	for fmax in 10 20 50 75 100; do
                	    for slowdown in 0.8; do
		                    echo "fec-end f: ${nFlows} d: ${coredelayvalue} g: ${nGroups} wmax: ${fmax}"
		                    DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}/wmax${fmax}/"
							#各フローの平均再送率をファイルごとに計算し、結果ファイルに追加
							out_filename="${DIR}fec-end-f"${nFlows}"-d"${coredelayvalue}"-g"${nGroups}"-wmax"${fmax}"-retrate.dat"
							if [[ -f "${out_filename}" ]];then
								rm -f "${out_filename}"
							fi
							tail -n ${nFlows} ${DIR}FecEnd.TcpDump | \
							awk -v N="${nFlows}" '{
								print "flow",$1,"ret_rate",sprintf("%.3f",($15/$11)*100);
								ret_sum+=($15/$11)*100;
								ret_sqsum+=(($15/$11)*100)^2;
							}END{
								ret_stdev=sqrt((1/N)*((ret_sqsum) - (((ret_sum)^2)/N)));
								print "avg-ret-rate",sprintf("%0.3f",ret_sum/N),"stdev",sprintf("%0.3f",ret_stdev);
							}' >> "${out_filename}"
							
							#結果を全体的にまとめる
							retrate=`tail -n 1 "${out_filename}" | awk '{print $2}'`
							stdev=`tail -n 1 "${out_filename}" | awk '{print $4}'`
							printf "f %2d d %3d g %d w %3d retrate %2.3f stdev %0.3f \n" $nFlows $coredelayvalue $nGroups $fmax $retrate $stdev >> ${dir}fec-end-f${nFlows}-retrate.dat
						done
					done	
				done
			done	
		done
	done
done