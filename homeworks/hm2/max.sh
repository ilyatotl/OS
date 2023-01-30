#!/bin/bash

function max {
    k=$1
    res=-1000000000
    while [ $k -gt 0 ]
    do
    k=$[ $k - 1]
    read x
    if [ $res -lt $x ]
    then
    res=$x
    fi
    done
    echo $res
}

echo "Enter the number of elements in array: "
read n
echo "Enter elements: "

ans=$(max $n)

echo "The max value is $ans"
