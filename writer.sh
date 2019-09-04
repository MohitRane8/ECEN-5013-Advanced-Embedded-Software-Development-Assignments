#!/bin/bash

# check if number of arguments are valid
if [ $# -ne 2 ]
then
    echo Wrong number of arguments
    exit 1
fi

# arg 1 - path to file
writefile=$1

# arg 2 - text string to be written
writestr=$2

echo $writestr >> $writefile
git config --global user.name >> $writefile
date >> $writefile