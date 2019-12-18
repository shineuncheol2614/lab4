#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 100
#define NAMESIZE 50

void * send_message(void *arg);
void * recv_message(void *arg);
void   error_handling(char *message);

char name[NAMESIZE] = "";
char message[BUFSIZE];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_result;

    if(argc!=4){
        printf("Usage : %s <IP> <PORT> <name>\n", argv[0]);
        exit(1);
    }

    strcpy(name, argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons((unsigned short)atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connet() error");

    pthread_create(&snd_thread, NULL, send_message, (void *)sock);
    pthread_create(&rcv_thread, NULL, recv_message, (void *)sock);

    pthread_join(snd_thread, &thread_result);
    pthread_join(rcv_thread, &thread_result);

    close(sock);

    return EXIT_SUCCESS;
}

void * send_message(void * arg)
{
    int sock = (int)arg;
    char name_message[NAMESIZE+BUFSIZE];

    while(1){
        fgets(message, BUFSIZE, stdin);
        if(!strcmp(message, "q\n")){
            close(sock);
             exit(0);
        }
        strcpy(name_message, "[");
        strcat(name_message, name);
        strcat(name_message, "] ");
        strcat(name_message, message);
        write(sock, name_message, strlen(name_message));
    }
}

void * recv_message(void * arg)
{
    int sock = (int)arg;
    char name_message[NAMESIZE+BUFSIZE];
    int str_len;
    int rtn;

    while(1){
        str_len = read(sock, name_message, NAMESIZE+BUFSIZE-1);

        if(str_len==-1)
        {
            rtn = 1;  
            return (void *)rtn;
        }
        name_message[str_len] = 0; 
        fputs(name_message, stdout);
    }
}

// 에러 발생시 처리할 메시지 처리 함수
void error_handling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
