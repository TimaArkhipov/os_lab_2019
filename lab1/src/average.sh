#!/bin/bash

amount=$#
aver=1

for i in $@
do
aver=$(( $aver + $i ))
done
let "aver = aver / amount"
echo "Кол-во элементов - $amount"
echo "Среднее арифметическое - $aver"