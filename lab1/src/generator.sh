#!/bin/bash
COUNTER=0
while [ $COUNTER -lt 150 ]
do
od -An -d -N 2 /dev/urandom >> numbers.txt
(( COUNTER++ ))
done
