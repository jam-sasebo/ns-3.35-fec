#!/bin/bash
dir="${1}" 

OUTPUTDIR_REGEX='s|\./||g;'   #パスの頭のドットスラッシュ「./」を削除
OUTPUTDIR_REGEX+='s|(?<=/)\w+/.*$||g;print;' #パスの2番目のスラッシュ以降のを削除、残りは結果用パス

DATAFILE_REGEX='s|(?:[^/]*/)||g;s|\..*$||g;print;' #TcpDump	のファイル名を切り取る（ファイル名以外は、あるいは最後のスラッシュまでのを、削除）

DATAPATH_REGEX='m|(?:[^/]*/)| and print$&;'

SIMPREFIX_REGEX='tr|A-Z|a-z|;print;' #大文字から小文字に変換

PARAMS_REGEX='s|\w+/\w+-+\w+/||g;' #結果用ディレクトリー名をパスから削除
PARAMS_REGEX+='s|/[^/]*TcpDump||g;' #結果ファイル名をパスから削除
PARAMS_REGEX+='s|/| |g;' #スラッシュをスペースに変換
PARAMS_REGEX+='s|([a-z](?=\d))|$1 |g;print;'  #パラメータを表示用にする

echo "nFlows,nRouters,coredelay,nGroups,fwin,fmax,総スループット,再送率,タイムアウト回数,冗長率,Fmaxカウント,ロス率,有効回復成功率" 
for filepath in $(find "${dir}" -name '*TcpDump' | sort); do

	datapath=$(echo $filepath | perl -ne "${DATAPATH_REGEX}")
	outputdir=$(echo $filepath | perl -ne "${OUTPUTDIR_REGEX}" )
	params=$(echo $filepath | perl -ne "${PARAMS_REGEX}")
	filename=$(echo $filepath | perl -ne "${DATAFILE_REGEX}" )
	sim_prefix=$(echo $filename | perl -ne "${SIMPREFIX_REGEX}")

	seed=$(echo $params | perl -wlne 'm|(?<=s )\d+| and print $&;')
	nRouters=$(echo $params | perl -wlne 'm|(?<=r )\d+| and print $&;')
	nFlows=$(echo $params | perl -wlne 'm|(?<=f )\d+| and print $&;' )
	coredelay=$(echo $params | perl -wlne 'm|(?<=d )\d+| and print $&;')
	nGroups=$(echo $params | perl -wlne '( m|(?<=g )\d+| and print $& ) or print 0;')
	fwin=$(echo $params | perl -wlne '(m|(?<=w )\d+| and print $&) or print 0;')
	fmax=$(echo $params | perl -wlne '(m|(?<=wmax )\d+| and print $&) or print 0;')

	#スループット
    thp=$(tail -n ${nFlows} "${filepath}" | \
	awk -F' ' '{
        sumthp += ( ($9-1) * 8 ) / ( 220 * 1000 * 1000 );
    }END{
        print sprintf("%0.2f",sumthp);
    }')

    #再送率
    retrate=$(tail -n ${nFlows} "${filepath}" | \
	awk -v N="${nFlows}" '{
		ret_sum+=($15/$11)*100;
	}END{
		print sprintf("%0.3f",ret_sum/N);
	}')

    # #ロス率
	# lostpkts=$(tail -n 1 "${datapath}""${filename}"".LostPacket" | awk '{print $7}')
	# sentpkts=$(tail -n "${nFlows}" "${filepath}" | \
	# awk -v N="${nFlows}" '{
	# 	sentpkts+=$11;
	# }END{
	# 	print sentpkts/N;
	# }')
	# lossrate=$(echo "scale=3; (${lostpkts}/${sentpkts}) * 100 " | bc -l )

    #ロス率
	lossrate=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		lost+=($13/$11)*100;
	}END{
		print sprintf("%0.3f",lost/N);
	}')

    #タイムアウト回数
    timeout=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		timeout+=$25;
	}END{
		print timeout;
	}')

    #冗長率
    redun=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		redun+=($17/$11)*100;
	}END{
		print sprintf("%0.2f",redun/N);
	}')

    #Fmaxカウント
    fmaxcount=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		mc+=$33;
	}END{
		print mc/N;
	}')

	#有効回復成功率
	effrec=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		if($17!=0)effrec+=($21/$17)*100;
	}END{
		print sprintf("%0.5f",effrec/N);
	}')

    echo ${sim_prefix} f ${nFlows} r ${nRouters} d ${coredelay}\
    	  g ${nGroups} w ${fwin} thp ${thp} retr ${retrate}\
    	  to ${timeout} redun ${redun} mc ${fmaxcount} lossr ${lossrate} effrec ${effrec}

    # echo ${sim_prefix} ${nFlows},${nRouters},${coredelay},\
    # 	  ${nGroups},${fwin},${thp},${retrate}\
    # 	  ${timeout},${redun},${fmaxcount},${lossrate}
    # 	  >> "${outputdir}""${sim_prefix}"-throughput.dat
done
