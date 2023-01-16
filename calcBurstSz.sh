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
for filepath in $(find "${dir}" -name '*LostPacket' | sort); do

	datapath=$(echo $filepath | perl -ne "${DATAPATH_REGEX}")
	outputdir=$(echo $filepath | perl -ne "${OUTPUTDIR_REGEX}" )
	params=$(echo $filepath | perl -ne "${PARAMS_REGEX}")
	filename=$(echo $filepath | perl -ne "${DATAFILE_REGEX}" )
	sim_id=$(echo $filename | perl -ne "${SIMID_REGEX}")

	seed=$(echo $params | perl -wlne 'm|(?<=s )\d+| and print $&;')
	nRouters=$(echo $params | perl -wlne 'm|(?<=r )\d+| and print $&;')
	nFlows=$(echo $params | perl -wlne 'm|(?<=f )\d+| and print $&;' )
	coredelay=$(echo $params | perl -wlne 'm|(?<=d )\d+| and print $&;')
	nGroups=$(echo $params | perl -wlne '( m|(?<=g )\d+| and print $&) or print 0;')
	fwin=$(echo $params | perl -wlne '(m|(?<=w )\d+| and print $&) or print 0;')
	burstRate="0.0001"

	awk '{print $1}' Data/fec-router/s1/r15/f20/d100/g3/w20/FecRouter.LostPacket \
	| sort -n | uniq -c | awk 'BEGIN{print "time"",""burstSize"}{print $2","$1}' \
	> 2023-01-10/test2.dat


done