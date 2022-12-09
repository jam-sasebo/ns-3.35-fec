#!/bin/sh
X_MIN=0
X_MAX=220
Y_MIN=0
Y_MAX=120
gnuplot<<EOF
plot './cwnd0.dat' u 2:4 title '0' with l lt 0,\
 './cwnd1.dat' u 2:4 title '1' with l lt 1,\
 './cwnd2.dat' u 2:4 title '2' with l lt 2,\
 './cwnd3.dat' u 2:4 title '3' with l lt 3,\
 './cwnd4.dat' u 2:4 title '4' with l lt 4,\
 './cwnd5.dat' u 2:4 title '5' with l lt 5,\
 './cwnd6.dat' u 2:4 title '6' with l lt 6,\
 './cwnd7.dat' u 2:4 title '7' with l lt 7,\
 './cwnd8.dat' u 2:4 title '8' with l lt 8,\
 './cwnd9.dat' u 2:4 title '9' with l lt 9,\
 './cwnd10.dat' u 2:4 title '10' with l lt 10,\
 './cwnd11.dat' u 2:4 title '11' with l lt 11,\
 './cwnd12.dat' u 2:4 title '12' with l lt 12,\
 './cwnd13.dat' u 2:4 title '13' with l lt 13,\
 './cwnd14.dat' u 2:4 title '14' with l lt 14,\
 './cwnd15.dat' u 2:4 title '15' with l lt 15,\
 './cwnd16.dat' u 2:4 title '16' with l lt 16,\
 './cwnd17.dat' u 2:4 title '17' with l lt 17,\
 './cwnd18.dat' u 2:4 title '18' with l lt 18,\
 './cwnd19.dat' u 2:4 title '19' with l lt 19,\
 './cwnd20.dat' u 2:4 title '20' with l lt 20,\
 './cwnd21.dat' u 2:4 title '21' with l lt 21,\
 './cwnd22.dat' u 2:4 title '22' with l lt 22,\
 './cwnd23.dat' u 2:4 title '23' with l lt 23,\
 './cwnd24.dat' u 2:4 title '24' with l lt 24,\
 './cwnd25.dat' u 2:4 title '25' with l lt 25,\
 './cwnd26.dat' u 2:4 title '26' with l lt 26,\
 './cwnd27.dat' u 2:4 title '27' with l lt 27,\
 './cwnd28.dat' u 2:4 title '28' with l lt 28,\
 './cwnd29.dat' u 2:4 title '29' with l lt 29,\
 './cwnd30.dat' u 2:4 title '30' with l lt 30,\
 './cwnd31.dat' u 2:4 title '31' with l lt 31,\
 './cwnd32.dat' u 2:4 title '32' with l lt 32,\
 './cwnd33.dat' u 2:4 title '33' with l lt 33,\
 './cwnd34.dat' u 2:4 title '34' with l lt 34,\
 './cwnd35.dat' u 2:4 title '35' with l lt 35,\
 './cwnd36.dat' u 2:4 title '36' with l lt 36,\
 './cwnd37.dat' u 2:4 title '37' with l lt 37,\
 './cwnd38.dat' u 2:4 title '38' with l lt 38,\
 './cwnd39.dat' u 2:4 title '39' with l lt 39,\
 './cwnd40.dat' u 2:4 title '40' with l lt 40,\
 './cwnd41.dat' u 2:4 title '41' with l lt 41,\
 './cwnd42.dat' u 2:4 title '42' with l lt 42,\
 './cwnd43.dat' u 2:4 title '43' with l lt 43,\
 './cwnd44.dat' u 2:4 title '44' with l lt 44,\
 './cwnd45.dat' u 2:4 title '45' with l lt 45,\
 './cwnd46.dat' u 2:4 title '46' with l lt 46,\
 './cwnd47.dat' u 2:4 title '47' with l lt 47,\
 './cwnd48.dat' u 2:4 title '48' with l lt 48,\
 './cwnd49.dat' u 2:4 title '49' with l lt 49
#set xrange [$X_MIN:$X_MAX]
#set yrange [$Y_MIN:$Y_MAX]
set yrange [0:]
set xrange [0:]
set xlabel 'Time[sec]'
set ylabel 'CWND[packet]'
#set key top right
unset key
set grid
set term postscript eps enhanced color solid "Helvetica" 24
set output 'cwnd.eps'
replot
EOF
