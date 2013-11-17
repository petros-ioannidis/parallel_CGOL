set title "Conway's Game of Life"
set datafile separator ","
set xlabel "size"
set ylabel "accelaration"
set autoscale
plot "MPI_accelaration.plot",\
     "OPENMP_accelaration.plot"
set xrange [0:GPVAL_X_MAX+GPVAL_X_MAX*0.2]
set yrange [0:GPVAL_Y_MAX+GPVAL_Y_MAX*0.2]
set terminal postscript enhanced color
plot "MPI_accelaration.plot" pt 3 title "MPI",\
     "OPENMP_accelaration.plot" pt 3 title "OPENMP"
