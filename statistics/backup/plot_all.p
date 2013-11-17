set title "Conway's Game of Life"
set datafile separator ","
set xlabel "size"
set ylabel "ms"
set autoscale
plot "MPI_time.plot",\
     "OPENMP_time.plot",\
     "SERIAL_time.plot"
set xrange [0:GPVAL_X_MAX+10]
set yrange [0:GPVAL_Y_MAX+10]
set terminal postscript enhanced color
plot "MPI_time.plot" pt 3 title "MPI",\
     "OPENMP_time.plot" pt 3 title "OPENMP",\
     "SERIAL_time.plot" pt 3 title "Serial program"
