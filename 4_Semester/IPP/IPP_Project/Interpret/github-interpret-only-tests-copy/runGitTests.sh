#!usr/bin/env bash

tests=( $(find ./valid -print | grep -i '\.src$') )


for t in "${tests[@]}"
do
	echo "$t"
	python3.8 ../interpret.py --source="$t"
done
