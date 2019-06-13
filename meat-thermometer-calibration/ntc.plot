#set terminal svg size 600,400 dynamic enhanced fname 'arial' fsize 10 name "simple_1" butt solid 
#set output 's881_steinhart_hart.svg'

T(x) = 1 / ( A + B*(log(x)) + C * (log(x)) ** 3)

A=0.0005
B=0.0005
C=1e-7

fit T(x) 'ntc.dat' using 2:($1+273.15) via A, B, C
 
#set key right top
#set ylabel "Temperature"
#set xlabel "ln(R/Rn)"
#set yrange[-60:160]
set xrange[10000:500000]

plot T(x) - 273.15, 'ntc.dat' using 2:1
#with xerrorbars

#foo(x) = A + (B*log(x)) + C*log(x)**3

#fit foo(x) 'ntc.dat' using 2:1 via A, B, C

#plot  foo(x), 'ntc.dat' using 2:1

