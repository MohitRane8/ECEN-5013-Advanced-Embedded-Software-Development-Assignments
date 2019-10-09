// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h> 

#define PORT 9000
#define MAX 200

volatile sig_atomic_t signal_flag;

void sigterm_handler(int sig)
{
    signal_flag = 1;
}

void sigint_handler(int sig)
{
    signal_flag = 1;
}


/* Signal Handler for SIGINT and SIGTERM */
// void signalHandler(int sig_num) 
// { 
//     signal(SIGINT, signalHandler); 
//     printf("\n Cannot be terminated using Ctrl+C \n"); 
//     fflush(stdout);

//     system("rm /var/tmp/aesdsocketdata");
//     syslog(LOG_CRIT, "Caught signal, exiting");
//     exit(0);
// }

// Main Function
void main()
{
    // signal(SIGINT, signalHandler);
    // signal(SIGTERM, signalHandler);
    
    // socket descriptor
    int sock, cli;
    struct sockaddr_in server, client;
    unsigned int len;
    int sent;
    char* buff;
    // char recvbuff[MAX];
    // char sendbuff[MAX];
    // char* sendbuff;

    // SOCKET
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket: ");
        exit(-1);
    }

    // BIND
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);

    len = sizeof(struct sockaddr_in);

    if((bind(sock, (struct sockaddr *)&server, len)) == -1)
    {
        perror("bind");
        exit(-1);
    }

    // LISTEN
    if((listen(sock, 5)) == -1)
    {
        perror("listen");
        exit(-1);
    }

    while(1)
    {
        // ACCEPT
        if((cli = accept(sock, (struct sockaddr *)&client, &len)) == -1)
        {
            perror("accept");
            exit(-1);
        }

        // log ip address of connected client
        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client.sin_addr));

        // SEND/RECV
        //sent = send(cli, mesg, strlen(mesg), 0);

        //printf("Sent %d bytes to client: %s\n", sent, inet_ntoa(client.sin_addr));

        // read message from client and copy it in buffer 
        bzero(buff, MAX);
        read(cli, buff, sizeof(buff));

        //
        // for(int i=0; i<200; i++)
        // {
        //     if ( buff[i] == '\0' )
        //     {
        //         // buff[i+1] = '\0';
        //         buff[i] = '\n';
        //         break;
        //     }
        // }

        // write to file
        int sz;
        int fd = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_APPEND, 0644); 
        if (fd < 0) 
        {
            perror("r1");
            exit(1);
        }
        sz = write(fd, buff, strlen(buff));
        syslog(LOG_DEBUG, "write call returned %d", sz);

        // add content of aesdsocketdata to buffer
        bzero(buff, MAX);
        read(fd, buff, sizeof(buff));
        printf("%s", buff);
        close(fd);

        // send buffer data to client
        // sendbuff = "abcdefg\n";
        send(cli, buff, sizeof(buff), 0);
        // system("cat /var/tmp/aesdsocketdata");

        // CLOSE
        close(cli);
        // log ip address of connected client
        syslog(LOG_INFO, "Closed connection to %s", inet_ntoa(client.sin_addr));
    }
}