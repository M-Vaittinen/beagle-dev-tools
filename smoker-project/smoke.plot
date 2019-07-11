#!/usr/bin/env gnuplot

#first, plot to pdf files                                                        
set terminal png truecolor                                                       
set title "Sähkösavustin, spare-ribs 2-kerrosta (100C)"                          
set xlabel "minuuttia"                                                           
set ylabel "Celsiusta"                                                           
plot '/temps.txt' using ($2*5/60):1  "Temperature  %lf, cycle %lf" title "mitattu lämpötila"

