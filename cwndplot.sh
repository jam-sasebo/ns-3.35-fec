#!/bin/bash

#シミュレーションパラメータの範囲を定義する。
X_MIN=0
X_MAX=220
Y_MIN=0
Y_MAX=500 

#入力：　空白ー＞結果のディレクトリに入っており、tcpかfecか指定する必要はない。
#　      tcp,fec,tcp-router,fec-router,tcp-router-3,fec-router-3などとマッチする
#注意！　複数のシミュレーションの結果ファイルがあったら入力を指定してください。
if ! [[ "$1" =~ ((tcp|fec)*(-)*(end|router)*(-)*([0-9])*|[[:blank:]]) ]]; then  #regexr.com/70cgf
    echo "tcp,fec,tcp-router,fec-router,tcp-router-3,fec-router-3などから入力を！"
    echo "注意：複数のシミュレーションの結果ファイルがあったら入力を指定してください。"
    exit
fi


echo "=========================== gnuplot start ========================="
#現在のディレクトリとその中身を表示する(cwndに関するもののみ)。
pwd && ls | grep cwnd

#結果ファイルの名前を取得する。
FILE_NAME=$(pwd | ls | grep 'cwnd[0-9]*[0-9]*.dat' | grep "${1}" | tail -n 1 | 
            awk '{sub(/-cwnd[0-9][0-9]*.dat/, "", $0); print $0}')

#結果ファイルの個数を計算する。
FILE_COUNT=$(pwd | ls | grep 'cwnd[0-9]*[0-9]*.dat' | grep "${1}" | wc -w)


#取得したファイル名を表示する。
echo "Filename: $FILE_NAME"
echo "Filecount: $FILE_COUNT"
#プロットのファイル名を作成する。入力していされていない場合は自動的に設定される。
OUTPUT_NAME="${FILE_NAME}"
echo "Outputname: $OUTPUT_NAME"

FILE_COUNT=$(( $FILE_COUNT - 1 ))   #ループするときに0から始まるので。。。
gnuplot<<EOF
plot for [i=0:${FILE_COUNT}] './${FILE_NAME}-cwnd'.i.'.dat' u (\$2):(\$4/1000) title 'f'.i with l lt (i+1)
#set xrange [$X_MIN:$X_MAX]
#set yrange [$Y_MIN:$Y_MAX]
set yrange [0:]
set xrange [0:$X_MAX]
set xlabel 'Time[s]'
set ylabel 'CWND[packet]'
set key top right
#unset key
set grid
set term postscript eps enhanced color solid "Helvetica" 24
set title 
show title
set output '${OUTPUT_NAME}cwnd.eps'
replot
EOF


echo "=========================== gnuplot end ========================="