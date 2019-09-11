#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    if(argc<3)
    {
        puts("Wrong number of arguments");
        puts("Usage: ./writer.sh [1st arg] [2nd arg]");
        puts("[1st arg] - path to file");
        puts("[2nd arg] - text string to be written");
        exit(1);
    }
    
    // path to file
    char* writefile = argv[1];

    // string to be written
    char* writestr = argv[2];
    
    FILE *fp = fopen(writefile, "w");
    if (fp == NULL)
    {
        puts("Couldn't open file");
        exit(1);
    }
    else
    {
        // write string
        fputs(writestr, fp);
        fputs("\n", fp);
        
        // write github user name
        fputs("MohitRane8", fp);
        fputs("\n", fp);

        // write date output
        char buffer[200];
        // sprintf(buffer, "pwd >> %s", writefile);
        // system(buffer);
        // fputs("\n", fp);

        sprintf(buffer, "date >> %s", writefile);
        system(buffer);
        fputs("\n", fp);
        
        system("date >> ./asdf.txt");

        puts("Done");

        fclose(fp);
    }
    return 0;
}