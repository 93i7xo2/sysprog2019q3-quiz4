set title ""
set ylabel 'Elapsed time (s)'
set xlabel 'Quantity of testdata'
set terminal png font " Times_New_Roman,12 "
set key left
set logscale y 10
set output "output.png"
set datafile separator ","
plot for [IDX=0:3] "merge.txt" i IDX u 1:2 w lines title columnheader(1)

