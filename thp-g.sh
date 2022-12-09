#!/bin/sh

X_MIN=1
X_MAX=20
Y_MIN=60
Y_MAX=100
	    gnuplot<<EOF

plot './fec-router-thp.dat' u 8:10 title '    Proposed'  with lp lc 1 lt 1 lw 8 pt 2 ps 4, \
     './fec-end-thp.dat'    u 8:10 title 'Conventional'  with lp lc 2 lt 1 lw 8 pt 4 ps 4, \
     './tcp-thp.dat'        u 8:10 title '         TCP'  with lp lc 3 lt 1 lw 8 pt 6 ps 4

set xrange [$X_MIN:$X_MAX]
set yrange [$Y_MIN:$Y_MAX]
set xlabel "Number of groups"
set ylabel 'Total throughput [Mbps]' 

set xtics font "Times New Roman,38"
set ytics font "Times New Roman,38"
set ylabel font "Times New Roman,38"
set xlabel font "Times New Roman,38"
set key below

unset grid

set term postscript eps enhanced color solid "Times-Roman" 30
set output 'eps/f10-d100-w20-thp.eps'
replot

set terminal emf enhanced 'Times New Roman' 30
set output 'emf/f10-d100-w20-thp.emf'
replot
EOF

