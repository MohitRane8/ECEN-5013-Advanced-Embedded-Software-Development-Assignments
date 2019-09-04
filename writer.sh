#!/bin/bash
# Writer script for assignment 1.
# Author: Mohit Rane

# check if number of arguments are valid
if [ $# -ne 2 ]
then
    echo Wrong number of arguments
    exit 1
fi

# arg 1 - path to file
WRITEFILE=$1

# arg 2 - text string to be written
WRITESTR=$2

# check file exists and if not then create
if [ ! -f $WRITEFILE ]
then
    touch $WRITEFILE
fi

echo $WRITESTR >> $WRITEFILE
git config --global user.name >> $WRITEFILE
date >> $WRITEFILE