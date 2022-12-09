#!/bin/bash

#平均「再送率」を計算する
dir="2022-11-10/tcp-router-3/"
if [[ -f "${dir}tcp-router-3-retrate.dat" ]];then
	rm -f "${dir}tcp-router-3-retrate.dat"
fi

for RngSeed in 1; do
	for nFlows in 10 20 30 40 50; do
		for coredelayvalue in 5 10 25 50 100; do
			echo f:${nFlows} d:${coredelayvalue} 
			DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/"
			#各フローの平均再送率をファイルごとに計算し、結果ファイルに追加
			out_filename="${DIR}f${nFlows}-d${coredelayvalue}-retrate.dat"
			if [[ -f "${out_filename}" ]];then
				rm -f "${out_filename}"
			fi
			tail -n ${nFlows} ${DIR}TcpRouter.TcpDump | \
			gawk -v N="${nFlows}" '{
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
			printf "f %2d d %3d retrate %0.3f stdev %0.3f \n" $nFlows $coredelayvalue $retrate $stdev >> ${dir}tcp-router-3-retrate.dat
		done
	done
done