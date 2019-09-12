#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <libgen.h>

int main( int argc, char* argv[] )
{
    int ret;
    
    // set up system logging
    openlog (NULL, LOG_CONS | LOG_NDELAY, LOG_USER);
    
    if(argc<3)
    {
        syslog(LOG_ERR, "Wrong number of arguments passed");
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
        syslog(LOG_ERR, "Error opening/creating the file passed in argument");
        puts("Couldn't open/create file");
        exit(1);
    }
    else
    {
        // write string
        char* base;
        base = basename(writefile);
        syslog(LOG_DEBUG, "Writing %s to %s", writestr, base);
        ret = fputs(writestr, fp);
        if(ret != 0) {
            syslog(LOG_ERR, "fputs error");
        }
        ret = fputs("\n", fp);
        if(ret != 0) {
            syslog(LOG_ERR, "fputs error");
        }
        
        // write github user name
        fputs("MohitRane8\n", fp);
        if(ret != 0) {
            syslog(LOG_ERR, "fputs error");
        }

        // close file pointer - at this point whatever is present in fputs buffer will be written in file
        ret = fclose(fp);
        if(ret != 0) {
            syslog(LOG_ERR, "fputs error");
        }

        // write date output
        char buffer[50];
        memset(buffer,'\0',sizeof(buffer));
        sprintf(buffer, "date >> %s", writefile);
        ret = system(buffer);
        if(ret != 0) {
            syslog(LOG_ERR, "system command error");
        }

        closelog();
    }
    return 0;
}