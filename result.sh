#! /bin/sh

echo `date '+%y/%m/%d %H:%M:%S'`
#################################################################
for d in 100; do
for f in 10; do
for w in 20; do
for g in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
#################################################################
grep "Throughput"  fec-router-f$f-d$d-w$w-g$g.dat > fec-router-f$f-d$d-w$w-g$g-thp.dat
grep "Throughput"  fec-end-f$f-d$d-w$w-g$g.dat    > fec-end-f$f-d$d-w$w-g$g-thp.dat
#grep "Throughput"  tcp-f$f-d$d.dat                > tcp-f$f-d$d-thp.dat
#################################################################
echo `date '+%y/%m/%d %H:%M:%S'`
done
done
done
done
