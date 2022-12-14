#! /bin/sh

echo `date '+%y/%m/%d %H:%M:%S'`
#################################################################
for d in 100; do
for f in 10; do
for w in 20; do
for g in 1 2 3 4 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
#################################################################
./waf --run "scratch/fec-end --nFlows=${f} --coredelay=${d} --fmax=${w} --nGroups=${g}" > fec-end-f$f-d$d-w$w-g$g.dat
echo `date '+%y/%m/%d %H:%M:%S'`
#################################################################
./waf --run "scratch/fec-router --nFlows=${f} --coredelay=${d} --fwin=${w} --nGroups=${g}" > fec-router-f$f-d$d-w$w-g$g.dat
echo `date '+%y/%m/%d %H:%M:%S'`
#################################################################
#./waf --run "scratch/tcp --nFlows=${f} --coredelay=${d}" > tcp-f$f-d$d.dat
#echo `date '+%y/%m/%d %H:%M:%S'`
#################################################################
done
done
done
done
#################################################################
