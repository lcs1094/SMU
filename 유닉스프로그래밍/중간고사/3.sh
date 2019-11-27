#!/bin/bash
count=0
for i in $*
do
	((sum = $sum + $i))
	let count+=1
done


echo $sum
echo $(($sum / $count))

