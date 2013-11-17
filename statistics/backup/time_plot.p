set title "Conway's Game of Life"
set datafile separator ","
set xlabel "size"
set ylabel "ms"
set autoscale
plot filename
set xrange [0:GPVAL_X_MAX+10]
set yrange [0:GPVAL_Y_MAX+10]
set terminal postscript 
plot filename title "Time"
