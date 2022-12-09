#!/bin/bash

#平均「再送率」を計算する

dir="2022-11-12/fec-router-3/"

for RngSeed in 5; do  #シード番号をよく注意を！！！！！
	for nFlows in 10 ; do
		if [[ -f "${dir}fec-router-3-f${nFlows}-retrate.dat" ]];then
			rm -f "${dir}fec-router-3-f${nFlows}-retrate.dat"
		fi
		for coredelayvalue in 5 10 25 50 100; do
			for nGroups in 1 3 5 7 9; do
                for fwin in 10 20 50 75 100; do
                    echo "fec-router-3 f: ${nFlows} d: ${coredelayvalue} g: ${nGroups} f: ${fwin}"
                    DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}/"
					#各フローの平均再送率をファイルごとに計算し、結果ファイルに追加
					out_filename="${DIR}fec-router-3-f"${nFlows}"-d"${coredelayvalue}"-g"${nGroups}"-w"${fwin}"-retrate.dat"
					if [[ -f "${out_filename}" ]];then
						rm -f "${out_filename}"
					fi
					tail -n ${nFlows} ${DIR}FecRouter.TcpDump | \
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
					printf "f %2d d %3d g %d w %3d retrate %2.3f stdev %0.3f \n" $nFlows $coredelayvalue $nGroups $fwin $retrate $stdev >> ${dir}fec-router-3-f${nFlows}-retrate.dat
				done
			done	
		done
	done
done