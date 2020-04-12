#!/bin/bash
n=0
sum=0
for i do
    sum=$(expr $sum + $i)
    (( n++ ))
done
if (( $n > 0 )); then
avg=$(echo "scale=4; $sum/$n" |bc)
echo "Average is $avg"
else
echo "No average"
fi
echo "Number of elements: $n"
