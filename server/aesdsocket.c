/*
References:
Sockets: https://www.youtube.com/watch?v=eVYsIolL2gE&t=422s
Daemons: http://www2.lawrence.edu/fast/GREGGJ/CMSC480/Daemons.html
*/

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
#include <linux/fs.h>
#include <sys/queue.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

// Port address
#define PORT 9000

// buffer size
#define MAX 2500

pthread_mutex_t ll_lock;
pthread_mutex_t file_lock;

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

// linked list node data structure
struct node_data
{
    pthread_t thread;
    int complete_flag;
};

// linked list node
struct node
{
    struct node_data thread_data;
    SLIST_ENTRY(node) nodes;
};

// create data type for head of queue for nodes of type 'struct node'
SLIST_HEAD(head_s, node) head = SLIST_HEAD_INITIALIZER(head);

// thread argument struct
struct thread_arg_t
{
    int cli;
    struct sockaddr_in client;
};

// function to return size of file
off_t fsize(const char *filename) 
{
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

void* timestamp_function(void* thread_arg)
{
    char* time_buff;
    char buff[30];
    time_t time_ptr;

    while(signal_flag == 0)
    {        
        time(&time_ptr);
        time_buff = ctime(&time_ptr);
        // strcat(buff, time_buff);
        sprintf(buff, "timestamp:%s", time_buff);

        // open a file descriptor to write message to file
        int fd = open("/var/tmp/aesdsocketdata", O_WRONLY | O_CREAT | O_APPEND, 0644); 
        if (fd < 0) 
        {
            perror("r1");
            exit(1);
        }
        pthread_mutex_lock(&file_lock);
        write(fd, buff, strlen(buff));
        fsync(fd);
        pthread_mutex_unlock(&file_lock);
        close(fd);

        sleep(10);
    }

    pthread_exit((void *)0);
}

void* thread_function(void* thread_arg)
{
    // thread parameters
    struct thread_arg_t* threadParam = (struct thread_arg_t*) thread_arg;

    char recvbuff[MAX];
    char *sendbuff = (char *)calloc(2000, sizeof(char));
    
    // SEND/RECV
    // read client message
    bzero(recvbuff, MAX);
    read(threadParam->cli, recvbuff, sizeof(recvbuff));

    // open a file descriptor to write message to file
    int fd1 = open("/var/tmp/aesdsocketdata", O_WRONLY | O_CREAT | O_APPEND, 0644); 
    if (fd1 < 0) 
    {
        perror("r1");
        exit(1);
    }
    pthread_mutex_lock(&file_lock);
    write(fd1, recvbuff, (strlen(recvbuff)));
    fsync(fd1);
    pthread_mutex_unlock(&file_lock);
    close(fd1);

    // open a file descriptor to read whole content of file
    int fd2 = open("/var/tmp/aesdsocketdata", O_RDONLY | O_CREAT, 0644); 
    if (fd2 < 0) 
    {
        perror("r1");
        exit(1);
    }
    pthread_mutex_lock(&file_lock);
    off_t ret = fsize("/var/tmp/aesdsocketdata");
    read(fd2, sendbuff, ret);
    pthread_mutex_unlock(&file_lock);

    // send buffer data to client
    send(threadParam->cli, sendbuff, strlen(sendbuff), 0);
    close(fd2);

    // CLOSE SOCKET
    close(threadParam->cli);

    // log ip address of connected client
    syslog(LOG_INFO, "Closed connection to %s", inet_ntoa(threadParam->client.sin_addr));

    // set thread complete flag
    pthread_t self_id;
    self_id = pthread_self();
    struct node * llnode = NULL;
    pthread_mutex_lock(&ll_lock);
    SLIST_FOREACH(llnode, &head, nodes)
    {
        if(llnode->thread_data.thread == self_id)
        {
            llnode->thread_data.complete_flag = 1;
        }
    }
    pthread_mutex_unlock(&ll_lock);

    // pthread_exit((void *)0);
}

// Main Function
int main(int argc, char *argv[])
{
    // if this is set, process will run as daemon
    int daemon_flag = 0;

    int rc;

    // parse command line arguments
    int opt;
    while((opt = getopt(argc, argv, "d")) != -1)  
    {  
        switch(opt)  
        { 
            case 'd':  
                daemon_flag = 1;
                break;
            case '?':  
                printf("unknown option: %c\n", optopt);
                printf("Usage: ./aesdsocket [-d]\n");
                printf("-d : run the process as daemon\n");
                break;
        }  
    }

    // initialize head before use
    SLIST_INIT(&head);

    struct node * llnode = NULL;

    if (pthread_mutex_init(&ll_lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return -1; 
    }

    if (pthread_mutex_init(&file_lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return -1; 
    }

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

    // SOCKET
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
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

    // check if port is open, then run process as deamon if flag is set
    if(daemon_flag == 1)
    {
        printf("creating daemon\n");
        
        pid_t pid;

        /* create new process */
        pid = fork ();
        if (pid == -1)
            return -1;
        else if (pid != 0)
            exit (EXIT_SUCCESS);
        
        /* create new session and process group */
        if (setsid() == -1)
            return -1;

        /* set the working directory to the root directory */
        if (chdir ("/") == -1)
            return -1;

        /* redirect fd's 0,1,2 to /dev/null */
        open ("/dev/null", O_RDWR);     /* stdin */
        dup (0);                        /* stdout */
        dup (0);                        /* stderror */

        // a check to determine if daemon was created
        syslog(LOG_DEBUG, "daemon created by aesdsocket\n");
    }

    // LISTEN
    if((listen(sock, 5)) == -1)
    {
        perror("listen");
        exit(-1);
    }

    // create timestamp thread
    pthread_t timestamp_thread;
    if((rc = pthread_create(&timestamp_thread, NULL, timestamp_function, NULL)) == -1)
    {
        perror("pthread_create for timestamp_thread");
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

        // THINK ABOUT MEMORY FOR THIS
        struct thread_arg_t thread_arg;
        thread_arg.cli = cli;
        thread_arg.client = client;

        // join threads which are complete
        pthread_mutex_lock(&ll_lock);
        SLIST_FOREACH(llnode, &head, nodes)
        {
            if(llnode->thread_data.complete_flag == 1)
            {
                pthread_join(llnode->thread_data.thread, NULL);
                syslog(LOG_DEBUG, "thread joined tid = %ld", llnode->thread_data.thread);
                SLIST_REMOVE(&head, llnode, node, nodes);
                free(llnode);
                // llnode = NULL;
            }
        }
        pthread_mutex_unlock(&ll_lock);

        // create thread to handle client connection
        llnode = (struct node*)malloc(sizeof(struct node));
        if(llnode == NULL)
        {
            exit(EXIT_FAILURE);
        }
        llnode->thread_data.complete_flag = 0;
        pthread_mutex_lock(&ll_lock);
        SLIST_INSERT_HEAD(&head, llnode, nodes);
        pthread_mutex_unlock(&ll_lock);
        pthread_create(&llnode->thread_data.thread, NULL, thread_function, (void*)&thread_arg);
        syslog(LOG_DEBUG, "thread created tid = %ld", llnode->thread_data.thread);
        llnode = NULL;
    }

    // close socket
    close(sock);

    pthread_join(timestamp_thread, NULL);

    pthread_mutex_destroy(&ll_lock); 
    pthread_mutex_destroy(&file_lock); 

    // remove file where received client data is stored
    system("rm /var/tmp/aesdsocketdata");
    syslog(LOG_CRIT, "Caught signal, exiting");

    return 0;
}
