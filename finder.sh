#!/bin/bash
# Finder script for assignment 1.
# Author: Mohit Rane

# check if number of arguments are valid
if [ $# -ne 2 ]
then
    echo Wrong number of arguments
    echo Usage: ./finder.sh [1st arg] [2nd arg]
    echo [1st arg] - path to directory
    echo [2nd arg] - text string to be searched
    exit 1
fi

# arg 1 - path to directory
FILESDIR=$1

# arg 2 - text string to be searched
SEARCHSTR=$2

# check if directory exists
if [ ! -d $FILESDIR ]
then
    echo Directory does not exist
    echo "Please pass in valid directory"
    exit 1
fi

# find number of files in given directory
NUM_OF_FILES=$( find $FILESDIR -type f | wc -l )

# find total matching lines
MATCHING_TOTAL_LINES=0
#find $FILESDIR -type f | while read line;
for line in $(find $FILESDIR -type f)
do
    MATCHING_LINES=$(cat $line | tr ' ' '\n' | grep $SEARCHSTR | wc -l)
    #echo $MATCHING_LINES :: $line
    ((MATCHING_TOTAL_LINES+=MATCHING_LINES))
done

echo The number of files are $NUM_OF_FILES and the number of matching lines are $MATCHING_TOTAL_LINES