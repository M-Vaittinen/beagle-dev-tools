#!/usr/bin/env gnuplot

#first, plot to pdf files                                                        
set terminal png truecolor                                                       
set title "Sähkösavustin lämpötila"                          
set xlabel "minuuttia"                                                           
set ylabel "Celsiusta"                                                           
plot '/savustin/tmp/temps.txt' using ($2*5/60):1  "Temperature  %lf, cycle %lf" title "mitattu lämpötila"

