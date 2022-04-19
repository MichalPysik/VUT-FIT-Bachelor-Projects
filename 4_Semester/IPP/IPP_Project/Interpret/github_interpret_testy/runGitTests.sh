#!usr/bin/env bash

tests=( $(find . -print | grep -i '\.in$') )


for t in "${tests[@]}"
do
	echo "$t"
	python3.8 ../interpret.py --source="$t"
done
