#!/bin/bash
dir="${1}" 

OUTPUTDIR_REGEX='s|\./||g;'   #パスの頭のドットスラッシュ「./」を削除
OUTPUTDIR_REGEX+='s|(?<=/)\w+/.*$||g;print;' #パスの2番目のスラッシュ以降のを削除、残りは結果用パス

DATAFILE_REGEX='s|(?:[^/]*/)||g;s|\..*$||g;print;' #TcpDump	のファイル名を切り取る（ファイル名以外は、あるいは最後のスラッシュまでのを、削除）

DATAPATH_REGEX='m|(?:[^/]*/)+| and print$&;' 

SIMID_REGEX='tr|A-Z|a-z|;print;' #大文字から小文字に変換

PARAMS_REGEX='s|\w+/\w+-+\w+/||g;' #結果用ディレクトリー名をパスから削除
PARAMS_REGEX+='s|/[^/]*TcpDump||g;' #結果ファイル名をパスから削除
PARAMS_REGEX+='s|/| |g;' #スラッシュをスペースに変換
PARAMS_REGEX+='s|([a-z](?=\d))|$1 |g;print;'  #パラメータを表示用にする

# echo "nFlows,nRouters,coredelay,nGroups,fwin,fmax,総スループット,再送率,タイムアウト回数,冗長率,Fmaxカウント,ロス率,有効回復成功率" 
# echo "sim_id,nFlows,nRouters,coredelay,nGroups,fwin,fmax,thp,retr,timeout,redun,fmaxcount,lossr,effrec,fr" 
for filepath in $(find "${dir}" -name '*TcpDump' | sort); do

	datapath=$(echo $filepath | perl -ne "${DATAPATH_REGEX}")
	outputdir=$(echo $filepath | perl -ne "${OUTPUTDIR_REGEX}" )
	params=$(echo $filepath | perl -ne "${PARAMS_REGEX}")
	echo ${params}
	filename=$(echo $filepath | perl -ne "${DATAFILE_REGEX}" )
	sim_id=$(echo $filename | perl -ne "${SIMID_REGEX}")

	seed=$(echo $params | perl -wlne 'm|(?<=s )\d+| and print $&;')
	nRouters=$(echo $params | perl -wlne 'm|(?<=r )\d+| and print $&;')
	nFlows=$(echo $params | perl -wlne 'm|(?<=f )\d+| and print $&;' )
	coredelay=$(echo $params | perl -wlne 'm|(?<=d )\d+| and print $&;')
	nGroups=$(echo $params | perl -wlne '( m|(?<=g )\d+| and print $&) or print 0;')
	fwin=$(echo $params | perl -wlne '(m|(?<=w )\d+| and print $&) or print 0;')
	wmax=$(echo $params | perl -wlne '(m|(?<=wmax )\d+| and print $&) or print 0;')
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

    #ロス率
    if [ "$sim_id" = "fecend" ] || [ "$sim_id" = "tcprouter" ]; then
    	lossrate=$(tail -n ${nFlows} "${filepath}" | \
	    awk -v N="${nFlows}" '{
			lost+=($13/$11)*100;
		}END{
			print sprintf("%0.3f",lost/N);
		}')
    else
    	lostpkts=$(tail -n 1 "${datapath}""${filename}"".LostPacket" | awk '{print $7}')
		sentpkts=$(tail -n 1 "${datapath}""${filename}"".SentPacket" | awk '{print $9}')
		lossrate=$(echo ${lostpkts} ${sentpkts} ${nFlows} | awk -F' ' '{
			lossr = ($1/$2)*100;
			print sprintf("%0.3f",lossr);
		}')
    fi

    #タイムアウト回数
    timeout=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		timeout+=$25;
	}END{
		print timeout;
	}')

    #冗長率
    if [ "$sim_id" = "fecend" ] || [ "$sim_id" = "tcprouter" ]; then
   		redun=$(tail -n ${nFlows} "${filepath}" | \
	    awk -v N="${nFlows}" '{
			redun+=($17/$11)*100;
		}END{
			print sprintf("%0.3f",redun/N);
		}')
    else
    	redun=$(tail -n 1 "${datapath}""${filename}"".SentPacket" | awk '{
    		redun=($11/$9)*100;
    		print sprintf("%0.3f",redun);
    	}')
    fi

    #Fmaxカウント
    fmaxcount=$(tail -n ${nFlows} "${filepath}" | \
    awk -v N="${nFlows}" '{
		mc+=$33;
	}END{
		print mc/N;
	}')

	#有効回復成功率
    if [ "$sim_id" = "fecend" ] || [ "$sim_id" = "tcprouter" ]; then
		effrec=$(tail -n ${nFlows} "${filepath}" | \
	    awk -v N="${nFlows}" '{
			if($17!=0)effrec+=($21/$17)*100;
		}END{
			print sprintf("%0.5f",effrec/N);
		}')
    elif [ "${nGroups}" != "1" ]; then
    	rec=$(tail -n 1 "${datapath}""${filename}"".Recovery" | awk '{print $3}')
    	fec=$(tail -n 1 "${datapath}""${filename}"".SentPacket" | awk '{print $11}')
    	effrec=$(echo ${rec} ${fec} | awk -F' ' '{
			if($1!=0 && $2!=0){effrec = ($1/$2)*100};
			print sprintf("%0.3f",effrec);
		}')
	else 
		effrec="0"
    fi

    #高速レカバリー
    fr=$(tail -n ${nFlows} "${filepath}" | \
	awk -v N="${nFlows}" '{
		fastrec+=$27;
	}END{
		print sprintf("%0.1f",fastrec/N);
	}')

    if [ "${sim_id}" == "fecend" ]; then
    #Fecウインドウの200秒間の平均値
    fwin=$(cat "${filepath}" | \
	awk '{
		if($3>20) fwin+=$37;
	}END{
		print sprintf("%.0f",fwin/NR);
	}')
    fi

    if [ "${sim_id}" == "fecend" ]; then
    #ロス再送率
    lostret=$(tail -n ${nFlows} "${filepath}" | \
	awk '{
		if($3>20) lostret+=($15/$13)*100;
	}END{
		print sprintf("%.2f",lostret/NR);
	}')
	fi

    echo id ${sim_id} f ${nFlows} r ${nRouters} d ${coredelay}\
    	  g ${nGroups} w ${fwin} thp ${thp} retr ${retrate}\
    	  to ${timeout} redun ${redun} mc ${fmaxcount} \
    	  lossr ${lossrate} effrec ${effrec} fr ${fr} lostret ${lostret} wmax ${wmax}

    # echo ${sim_id},${nFlows},${nRouters},${coredelay},\
    # 	  ${nGroups},${fwin},${thp},${retrate}\
    # 	  ${timeout},${redun},${fmaxcount},${lossrate},${effrec},${fr}
    # 	  >> "${outputdir}""${sim_id}"-throughput.dat
done