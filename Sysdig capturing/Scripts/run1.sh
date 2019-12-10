#!/bin/bash

if [ $# -lt 2 ]; then
	echo "Script failed to run!"
	echo "This script requires 2 arguements"
	echo "The format is './run.sh <fileName> <chiselName>'"
	exit 1
fi

sudo sysdig -w $1.scap -M 60

sleep 5

sudo sysdig -r $1.scap -c $2
