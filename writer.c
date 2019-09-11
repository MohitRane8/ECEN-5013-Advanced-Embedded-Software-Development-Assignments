#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main( int argc, char* argv[] )
{
    if(argc<3)
    {
        syslog(LOG_ERR, "Wrong number of arguments passed");
        puts("Wrong number of arguments");
        puts("Usage: ./writer.sh [1st arg] [2nd arg]");
        puts("[1st arg] - path to file");
        puts("[2nd arg] - text string to be written");
        exit(1);
    }

    openlog (NULL, LOG_CONS | LOG_NDELAY, LOG_USER);
    
    // path to file
    char* writefile = argv[1];

    // string to be written
    char* writestr = argv[2];
    
    FILE *fp = fopen(writefile, "w");
    if (fp == NULL)
    {
        syslog(LOG_ERR, "Error opening the file passed in argument");
        puts("Couldn't open file");
        exit(1);
    }
    else
    {
        // write string
        syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
        fputs(writestr, fp);
        fputs("\n", fp);
        
        // write github user name
        fputs("MohitRane8", fp);

        // write date output
        char buffer[50];
        sprintf(buffer, "pwd >> %s", writefile);
        system(buffer);
        fputs("\n", fp);

        sprintf(buffer, "date >> %s", writefile);
        system(buffer);
        fputs("\n", fp);
        
        // system("date >> ./asdf.txt");

        // puts("Done");

        fclose(fp);
        closelog();
    }
    return 0;
}