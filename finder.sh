#!/bin/bash

# check if number of arguments are valid
if [ $# -ne 2 ]
then
    echo Wrong number of arguments
    exit 1
fi

# arg 1 - path to directory
filesdir=$1

# arg 2 - text string to be searched
searchstr=$2

# check file exists and is a directory
if [ ! -d $filesdir ]
then
    echo Directory does not exist
    exit 1
fi

# find number of files in given directory
NUM_OF_FILES=$( find $filesdir -type f | wc -l )
echo number of files is $NUM_OF_FILES
echo

: '
MATCHING_TOTAL_LINES=0
find $filesdir -type f > files_path.txt

while read -r line;
do
    MATCHING_LINES=$(cat $line | grep $searchstr | wc -l)
    echo $MATCHING_LINES :: $line
    ((MATCHING_TOTAL_LINES+=MATCHING_LINES))
done < "files_path.txt"
echo 
echo $MATCHING_TOTAL_LINES lines found
echo

rm files_path.txt
'

# find total matching lines
MATCHING_TOTAL_LINES=0
#find $filesdir -type f | while read line;
for line in $(find $filesdir -type f)
do
    MATCHING_LINES=$(cat $line | grep $searchstr | wc -l)
    echo $MATCHING_LINES :: $line
    ((MATCHING_TOTAL_LINES+=MATCHING_LINES))
done
echo
echo $MATCHING_TOTAL_LINES

echo The number of files are $NUM_OF_FILES and the number of matching lines are $MATCHING_TOTAL_LINES