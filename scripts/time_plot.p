set title "Conway's Game of Life"
set datafile separator ","
set xlabel "Threads"
set ylabel "ms"
set autoscale
plot filename
set xrange [0:GPVAL_X_MAX+GPVAL_X_MAX*0.2]
set yrange [0:GPVAL_Y_MAX+GPVAL_X_MAX*0.2]
set terminal ps
plot filename title filename 
