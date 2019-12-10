#!/bin/bash

if [ $# -lt 3 ]; then
	echo "Script failed to run!"
	echo "This script requires 3 arguements"
	echo "The format is './run.sh <targetProcess> <fileName> <chiselName>'"
	exit 1
fi

sudo sysdig proc.name=$1 -w $2.scap -M 60

sleep 5

sudo sysdig -r $2.scap -c $3
