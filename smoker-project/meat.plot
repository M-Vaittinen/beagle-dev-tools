#!/usr/bin/env gnuplot

#first, plot to pdf files                                                        
set terminal png truecolor                                                       
set title "Ruoan lämpö"                          
set xlabel "minuuttia"                                                           
set ylabel "Celsiusta"
plot '/meat_temp.txt' using ($2*5/60):1  "Meat Temperature  %lf, cycle %lf"

