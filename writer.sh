#!/bin/bash
# Writer script for assignment 1.
# Author: Mohit Rane

# check if number of arguments are valid
if [ $# -ne 2 ]
then
    echo Wrong number of arguments
    echo Usage: ./writer.sh [1st arg] [2nd arg]
    echo [1st arg] - path to file
    echo [2nd arg] - text string to be written
    exit 1
fi

# arg 1 - path to file
WRITEFILE=$1

# arg 2 - text string to be written
WRITESTR=$2

DIR=$(dirname "${WRITEFILE}")

# check if directory exists
if [ ! -d $DIR ]
then
    echo "Directory does not exist"
    echo "Please pass in valid directory"
    exit 1
fi

# write specified details in file
echo $WRITESTR >> $WRITEFILE
git config --global user.name >> $WRITEFILE
date >> $WRITEFILE