#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

char buf_read[256] = "";
char buf_write[256] = "";
pthread_t threads[2];
int sockfd;

void errors(char *msg){
    perror(msg);
    exit(1);
}

void *readf(void *arg){
    // while(1){
    //     bzero(buf_read,256);
    //     if(read(sockfd,buf_read,255) < 0){
    //         perror("Error reading from socket. Connection closed.\n");
    //         break;
    //     }
    //     printf("Receiving message from Arduino: %s\n",buf_read);
    // }
    pthread_exit(NULL);
}

void *writet(void *arg){
    while(1){
        printf("Send message to Arduino: ");
        bzero(buf_write,256);
        fgets(buf_write,255,stdin);
        if(write(sockfd,buf_write,strlen(buf_write))<0){
            perror("Error writing to socket. Connection closed.\n");
            break;
        }
        printf("Sent successfully!");
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    //portno = 5000;
    int portno;
    printf("Port No: ");
    scanf("%d",&portno);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        errors("ERROR opening socket");
    //server = gethostbyname("192.168.22.1");
    server = gethostbyname("localhost");
    if (server == NULL)
        errors("No such host.\n");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        errors("ERROR connecting");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0],&attr, readf, (void*)1);
    pthread_create(&threads[1],&attr, writet, (void*)2);

    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);
    pthread_attr_destroy(&attr);

    if(close(sockfd)<0){
        perror("Connection already closed.\n");
    }

    printf("Program terminating...\n");

    pthread_exit(NULL);
    return 0;
}
