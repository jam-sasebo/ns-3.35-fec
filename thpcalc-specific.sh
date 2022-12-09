#!/bin/bash

#スループットを計算する
nFlows=10
coredelayvalue=100
sumthp=$(tail -n ${nFlows} TcpRouter.TcpDump | awk -v f=${nFlows} -F' ' '{
        sumthp+=(($9-1)*8)/(220 * 1000 * 1000 * f);
    }END{
        print sprintf("%0.2f",sumthp);
    }')
echo tcp-router-3 f ${nFlows} d ${coredelayvalue} thp ${sumthp}
echo f ${nFlows} d ${coredelayvalue} thp ${sumthp} >> tcp-router-3-throughput.dat
