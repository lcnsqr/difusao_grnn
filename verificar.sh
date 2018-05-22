#!/bin/bash

X=$1_x.idx
Y=$1_y.idx

# Total de amostras no conjunto
TOTAL=`od -A n -t u4 -j 4 -w4 -N4 < $X`

# Dimensões de cada amostra
DIMX=`od -A n -t u4 -j 8 -w4 -N4 < $X`
DIMY=`od -A n -t u4 -j 8 -w4 -N4 < $Y`

# Inteiro aleatório entre 0 e 2^32
RAND=`od -N 4 -A n -t u4 < /dev/urandom`

# Inteiro aleatório entre 0 e TOTAL - 1
A=$((($TOTAL-1)*$RAND/2**32))

# Exibir par
echo "Par $A:"
od -A n -t f4 -j $((12+$A*4*$DIMX)) -w4 -N$((4*$DIMX)) < $X > x.tmp
od -A n -t f4 -j $((12+$A*4*$DIMY)) -w4 -N$((4*$DIMY)) < $Y > y.tmp
#paste x.tmp y.tmp
echo "x:"
cat x.tmp
echo "y:"
cat y.tmp
rm x.tmp y.tmp
