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
#include <sys/stat.h>

// Port address
#define PORT 9000

// buffer size
#define MAX 200

volatile sig_atomic_t signal_flag;

// SIGINT signal handler
void sigint_handler(int sig)
{
    signal_flag = 1;
}

// SIGTERM signal handler
void sigterm_handler(int sig)
{
    signal_flag = 1;
}

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

// Main Function
int main(void)
{
    // Setting signal handling
    struct sigaction saint;
    struct sigaction saterm;

    signal_flag = 0;

    saint.sa_handler = sigint_handler;
    saint.sa_flags = 0;
    sigemptyset(&saint.sa_mask);

    saterm.sa_handler = sigint_handler;
    saterm.sa_flags = 0;
    sigemptyset(&saterm.sa_mask);

    if (sigaction(SIGINT, &saint, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    if (sigaction(SIGTERM, &saterm, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    // socket descriptor
    int sock, cli;
    struct sockaddr_in server, client;
    unsigned int len;
    char buff[MAX];
    // char sendbuff[MAX];
    // char *buff = (char *)calloc(200, sizeof(char));
    char *sendbuff = (char *)calloc(200, sizeof(char));
    // char *buff;
    // char *sendbuff;

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

    while(signal_flag == 0)
    {
        // ACCEPT
        if((cli = accept(sock, (struct sockaddr *)&client, &len)) == -1)
        {
            if (errno == EINTR) break;
            perror("accept");
            exit(-1);
        }

        // log ip address of connected client
        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client.sin_addr));

        // SEND/RECV
        // read message from client and copy it in buffer
        // buff = (char *)calloc(200, sizeof(char));
        bzero(buff, MAX);
        printf("size of buff = %ld\n", sizeof(buff));
        printf("buff = %s\n", buff);
        read(cli, buff, sizeof(buff));
        printf("received data = %s", buff);
        printf("size of recv buffer = %ld\n", strlen(buff));

        // write to file
        int sz;
        int fd1 = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_APPEND, 0644); 
        if (fd1 < 0) 
        {
            perror("r1");
            exit(1);
        }
        sz = write(fd1, buff, (strlen(buff)));
        // syslog(LOG_DEBUG, "write call returned %d", sz);
        printf("write call returned %d", sz);
        fsync(fd1);

        off_t ret = fsize("/var/tmp/aesdsocketdata");
        printf("size of file = %ld\n", ret);

        // free(buff);

        close(fd1);
        // sleep(1);
        int fd2 = open("/var/tmp/aesdsocketdata", O_RDWR | O_CREAT | O_APPEND, 0644); 
        if (fd2 < 0) 
        {
            perror("r1");
            exit(1);
        }

        // add content of aesdsocketdata to buffer
        // sendbuff = (char *)calloc(200, sizeof(char));
        // bzero(sendbuff, MAX);
        sz = read(fd2, sendbuff, ret);
        // sendbuff[ret] = '\0';
        // syslog(LOG_DEBUG, "read call returned %d", sz);
        printf("read call returned %d\n", sz);
        printf("sendbuff = %s", sendbuff);

        // send buffer data to client
        // sendbuff = "abcdefg\n";
        printf("strlen of sent buffer = %ld\n", strlen(sendbuff));
        printf("sizeof of sent buffer = %ld\n", sizeof(sendbuff));

        send(cli, sendbuff, strlen(sendbuff), 0);
        // system("cat /var/tmp/aesdsocketdata");

        // free(sendbuff);

        // CLOSE
        close(fd2);
        close(cli);
        // log ip address of connected client
        syslog(LOG_INFO, "Closed connection to %s", inet_ntoa(client.sin_addr));
    }

    // remove file where received client data is stored
    printf("check\n");
    system("rm /var/tmp/aesdsocketdata");
    syslog(LOG_CRIT, "Caught signal, exiting");

    return 0;
}

        // sleep(3);

        // for(int i=0; i<200; i++)
        // {
        //     if ( buff[i] == '\0' )
        //     {
        //         // buff[i+1] = '\0';
        //         buff[i] = '\n';
        //         break;
        //     }
        // }