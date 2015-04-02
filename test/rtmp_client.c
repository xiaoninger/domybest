#include <stdio.h>
#define srs_trace(msg, ...)   printf("pid %d,%s",get_id(), msg, ##__VA_ARGS__)
#define ERROR_SUCCESS  0
#define SRS_SUCCESS    0
#define SRS_ERROR     -1

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include "amf0.h"
#include "types.h"
#include "io.h"
#include "amf.h"
typedef struct srs_handshake {
    char *c0c1;
    char *s0s1s2;
    char *c2;
}srs_handshake_t;


srs_handshake_t handshake_info;
void srs_random_generate(char* bytes, int size)
{
    static bool _random_initialized = false;
    int i;
    for (i = 0; i < size; i++) {
        // tddhe common value in [0x0f, 0xf0]
        bytes[i] = 0x0f + (rand() % (256 - 0x0f - 0x0f));
    }
}

void write_1bytes(char *p,int8_t value)
{

    *p++ = value;
}

void write_2bytes(char *p,int16_t value)
{
    char *pp = (char*)&value;
    *p++ = pp[1];
    *p++ = pp[0];
}

void write_8bytes(char *p,double value)
{
    char *pp = (char *)&value;
    *p++ = p[7];
    *p++ = p[6];
    *p++ = p[5];
    *p++ = p[4];
    *p++ = p[3];
    *p++ = p[2];
    *p++ = p[1];
    *p++ = p[0];
    
}
void write_4bytes(char *p,int32_t value)
{

    char *pp = (char*)&value;
    *p++ = pp[3];
    *p++ = pp[2];
    *p++ = pp[1];
    *p++ = pp[0];
}

void write_bytes(char *p,char* data, int size)
{
    
    memcpy(p, data, size);
}
int create_c0c1()
{
    int ret = ERROR_SUCCESS;

    handshake_info.c0c1 = (char *)malloc(1537*sizeof(char));
    srs_random_generate(handshake_info.c0c1, 1537);

    // plain text required.

    write_1bytes(handshake_info.c0c1,0x03);
    write_4bytes(handshake_info.c0c1+1,(int32_t)time(NULL));
    write_4bytes(handshake_info.c0c1+5,0x00);

    return ret;
}

int create_s0s1s2(const char* c1)
{
    int ret = ERROR_SUCCESS;

    handshake_info.s0s1s2 = (char *)malloc(3073*sizeof(char));
    srs_random_generate(handshake_info.s0s1s2, 3073);

    write_1bytes(handshake_info.s0s1s2,0x03);
    write_4bytes(handshake_info.s0s1s2+1,(int32_t)time(NULL));
    // s1 time2 copy from c1
    if (handshake_info.c0c1) {
        //stream.write_bytes(c0c1 + 1, 4);
    }

    // if c1 specified, copy c1 to s2.
    // @see: https://github.com/winlinvip/simple-rtmp-server/issues/46
    /*if (c1) {
        memcpy(s0s1s2 + 1537, c1, 1536);
    }*/

    return ret;
}

int create_c2()
{
    int ret = ERROR_SUCCESS;


    handshake_info.c2 = (char *)malloc(1536*sizeof(char));
    srs_random_generate(handshake_info.c2, 1536);

    // time
    write_4bytes(handshake_info.c2,(int32_t)time(NULL));
    // c2 time2 copy from s1
    if (handshake_info.s0s1s2) {
        write_bytes(handshake_info.c2+4,handshake_info.s0s1s2 + 1, 4);
    }

    return ret;
}

int handshake()
{
    int ret = SRS_SUCCESS; 

    return ret; 


}


bool srs_is_little_endian()
{
    // convert to network(big-endian) order, if not equals, 
    // the system is little-endian, so need to convert the int64
    static int little_endian_check = -1;
    
    if(little_endian_check == -1) {
        union {
            int32_t i;
            int8_t c;
        } little_check_union;
        
        little_check_union.i = 0x01;
        little_endian_check = little_check_union.c;
    }
    
    return (little_endian_check == 1);
}

int main(int argc,char **argv)
{
    int cfd;
    int recbytes;
    int sin_size;
    char buffer[1024]={0};   
    struct sockaddr_in s_add,c_add;
    unsigned short portnum=1935; 
    printf("Hello,welcome to client !\r\n");

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == cfd)
    {
        printf("socket fail ! \r\n");
        return -1;
    }
    printf("socket ok !\r\n");
    double x=1;
    unsigned char *p=(unsigned char *)&x;
    int i;
    for (i=0;i<8;i++) {
        printf("%x ",*(p+i));
    }
    printf("\n");
    bzero(&s_add,sizeof(struct sockaddr_in));
    s_add.sin_family=AF_INET;
    s_add.sin_addr.s_addr= inet_addr("192.168.0.188");
    s_add.sin_port=htons(portnum);
    printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

        
    
    if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
    {
        printf("connect fail !\r\n");
        return -1;
    }
    printf("connect ok !\r\n");
        
    create_c0c1();
    if(-1 == (recbytes = write(cfd,handshake_info.c0c1,1537)))
    {
        printf("read data fail !\r\n");
        return -1;
    }
    printf("send: %d bytes\n",recbytes);
    handshake_info.s0s1s2 = (char *)malloc(3073*sizeof(char));
    if(-1 == (recbytes = read(cfd,handshake_info.s0s1s2,3073)))
    {
        printf("read data fail !\r\n");
        return -1;
    }
    
    printf("receive: %d bytes\n",recbytes);
    printf("read ok\r\nREC:\r\n");
    
    create_c2();
    if(-1 == (recbytes = write(cfd,handshake_info.c2,1536)))
    {
        printf("read data fail !\r\n");
        return -1;
    }
    printf("send c2: %d bytes\n",recbytes);
    
    unsigned char *connect = (unsigned char *)malloc(1024*sizeof(char));
    if (connect == NULL) {
        fprintf(stderr,"bad alloc\n");
        return -1;
    }
    connect[12] = 0x02;
    unsigned short len = 0x07;
    
    amf0_data * test;
    test = amf0_object_new();
    amf0_object_add(test, "toto", amf0_str("une chaine de caracteres"));
    amf0_object_add(test, "test_bool", amf0_boolean_new(1));
    char *ab = test;
    printf("test %s",ab);
    amf0_data_dump(stdout, test, 0);
    amf0_data_free(test);

    unsigned char *ok = (unsigned char *)&len;
    memcpy(connect+13,ok+1,1);
    memcpy(connect+14,ok,1);
    memcpy(connect+15,"connect",7);
    connect[22] = 0x00;
    write_8bytes(connect+23,1);
    

    for (i=12;i<22;i++) {
        printf("%x ",connect[i]);
    }

    unsigned char *af = (unsigned char *)malloc(sizeof(char)*512);
    int length=0;
    amf0_data * amf_connect;
    amf_connect = amf0_str("connect");
    write_proc_t write_proc;
    
    length =amf0_data_buffer_write(amf_connect,af,512);
    
    amf0_data *amf_transid;
    amf_transid = amf0_number_new(1);
    length += amf0_data_buffer_write(amf_transid,af+length,512-length);
    amf0_data *amf_object;
    amf_object = amf0_object_new();
    
    amf0_object_add(amf_object, "app", amf0_str("live"));
    amf0_object_add(amf_object, "flashVer", amf0_str("liu 9,0,124,2"));



    for (i=0;i<length;i++) {
        printf("%x ",af[i]);
    }

    amf0_data_dump(stdout, amf_connect, 0);
     
    //connect[13] = 0x00;
    char *tcUrl="rtmp://192.168.0.188/live/stream";
    buffer[recbytes]='\0';
    //printf("%s\r\n",handshake_info.s0s1s2);
    if (srs_is_little_endian()) 
        printf("little_endian\n");
    else 
        printf("big_endian\n");
        
    getchar();
    close(cfd);   
 return 0;
}
