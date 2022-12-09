#!/bin/bash

NOW=$( date +"%F" )

dir="${NOW}/tcp-router-2/"

for RngSeed in 1; do
 for nFlows in 10 50 40 30 20; do
  for coredelayvalue in 150 100 50 10; do
   for nGroups in 1 2 3 4 5; do
    for fwin in 1 2 3 4 5 6 7 8 9 10; do
   
      mkdir ${dir}s${RngSeed}
      mkdir ${dir}s${RngSeed}/f${nFlows}
      mkdir ${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}
      mkdir ${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}
      mkdir ${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}

      DIR="${dir}s${RngSeed}/f${nFlows}/d${coredelayvalue}/g${nGroups}/w${fwin}/"

      echo "./waf --run \"scratch/tcp.cc --RngSeed=${RngSeed} --nFlows=${nFlows} --coredelay=${coredelayvalue} --fwin=${fwin} --nGroups=${nGroups}\" --vis"
      
      ./waf --run "scratch/tcp.cc --RngSeed=${RngSeed} --nFlows=${nFlows} --coredelay=${coredelayvalue} --fwin=${fwin} --nGroups=${nGroups}" --vis


      cp Tcp.TcpDump ${DIR}.
      cp Tcp.Timeout ${DIR}.
      cp Tcp.FastRecovery ${DIR}.
      cp Tcp.Retransmit ${DIR}.
      cp Tcp.LostPacket ${DIR}.
      cp Tcp.SentPacket ${DIR}.
      cp Tcp.Recovery ${DIR}.
      for i in $(seq 0 ${nFlows})
      do
      mv cwnd"${i}".dat "${DIR}".
      done
    done
   done
  done
 done
done
