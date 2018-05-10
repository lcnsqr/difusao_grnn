#!/bin/bash

X=$1_x.idx
Y=$1_y.idx

# Total de amostras no conjunto
TOTAL=`od -A n -t u4 -j 4 -w4 -N4 < $X`

# Dimensões de cada amostra
DIM=`od -A n -t u4 -j 8 -w4 -N4 < $X`

# Inteiro aleatório entre 0 e 2^32
RAND=`od -N 4 -A n -t u4 < /dev/urandom`

# Inteiro aleatório entre 0 e TOTAL - 1
A=$((($TOTAL-1)*$RAND/2**32))

# Exibir par
echo "Par $A:"
od -A n -t f4 -j $((12+$A*4*$DIM)) -w4 -N$((4*$DIM)) < $X > x.tmp
od -A n -t f4 -j $((12+$A*4*$DIM)) -w4 -N$((4*$DIM)) < $Y > y.tmp
paste x.tmp y.tmp
rm x.tmp y.tmp
