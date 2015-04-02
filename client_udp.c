#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("请输入要传送的内容.\r\n");
        exit(0);
    }
    int sock;
    //sendto中使用的对方地址
    struct sockaddr_in toAddr;
    struct sockaddr_in local_addr;
    //在recvfrom中使用的对方主机地址
    struct sockaddr_in fromAddr;
    unsigned int fromLen;
    char recvBuffer[128];
    sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sock < 0)
    {
        printf("创建套接字失败了.\r\n");
        exit(1);
    }
    memset(&toAddr,0,sizeof(toAddr));
    toAddr.sin_family=AF_INET;
    toAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    toAddr.sin_port = htons(4000);

    memset(&local_addr,0,sizeof(local_addr));
    local_addr.sin_family=AF_INET;
    local_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    local_addr.sin_port = htons(5000);

    if(bind(sock,(struct sockaddr*)&local_addr,sizeof(local_addr))<0) {
        printf("bind error\n");
    }
    printf("port: %d\n",ntohs(local_addr.sin_port));
    while (1) {
        printf("ok1\n");
        if(sendto(sock,argv[1],strlen(argv[1]),0,(struct sockaddr*)&toAddr,sizeof(toAddr)) != strlen(argv[1]))
        {
            printf("sendto() 函数使用失败了.\r\n");
            close(sock);
            exit(1);
        }
        fromLen = sizeof(fromAddr);
        printf("ok2\n");
        if(recvfrom(sock,recvBuffer,128,0,(struct sockaddr*)&fromAddr,&fromLen)<0)
        {
            printf("()recvfrom()函数使用失败了.\r\n");
            close(sock);
            exit(1);
        }
        printf("ok3\n");
        sleep(20);
    }
    printf("recvfrom() result:%s\r\n",recvBuffer);
    close(sock);
}
