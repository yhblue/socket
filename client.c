#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>


#include "message.pb-c.h"

#define _PORT  8000

void myhandle(int signal_id)
{
    printf("recv signal：%d\n",signal_id);
    exit(0);// exit father process  
}

       
#ifdef TEST_HERO_MSG

//这个程序是测试客户端发送英雄信息的
int main(void)
{ 
    int sockfd = 0;
    pid_t pid = 0;

    struct sockaddr_in server_addr;  
//    struct sockaddr_in client_addr; 


    signal(SIGUSR1,myhandle);
    
    //1.creat socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if( sockfd == -1 )
    {
       printf("creat socket err\n");
       exit(1); 
    }

    // client_addr.sin_family = AF_INET;
    // client_addr.sin_port = htons(9999);
    // //client_addr.sin_addr.s_addr = htonl(INADDR_ANY);   
    // bind(sockfd,(struct sockaddr*)(&client_addr),sizeof(client_addr)); //bind

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    if( (connect(sockfd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr))) == -1 )
    {
       printf("connect error!\n"); 
       exit(1); 
    }
    else
    {
        printf("connect success!\n"); 
    }

     while( 1 )
     {
        HeroMsg hero = HERO_MSG__INIT;
        hero.uid = 19876;
        hero.point_x = 90;
        hero.point_y = 450;
        int pack_size = hero_msg__get_packed_size(&hero);
        printf("client: pack_size = %d\n",pack_size);
        uint8_t *out_buf = (uint8_t*)malloc(pack_size+2);
        hero_msg__pack(&hero,out_buf+2);

        uint8_t* seria_buf = out_buf;
        HeroMsg* msg = NULL;
        msg = hero_msg__unpack(NULL,pack_size,seria_buf+2);
        
        printf("type is HeroMsg,msg->uid = %d,msg->point_x = %d,msg->point_y = %d\n",msg->uid,msg->point_x,msg->point_y);

        seria_buf[0] = pack_size+1;
        seria_buf[1] = 'H';
        send(sockfd,seria_buf,pack_size+2,0);  

        free(msg);
        free(seria_buf);    
        sleep(8);
     }  

     close(sockfd);
     kill(pid,SIGUSR1);
     exit(0);
     return 0;
}


#else

int main(void)
{ 
    int sockfd = 0;

    struct sockaddr_in server_addr; 
    bool test_bool = true;
    printf("sizeof(test_bool) = %d\n",sizeof(test_bool));
    //1.creat socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if( sockfd == -1 )
    {
       printf("creat socket err\n");
       exit(1); 
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    if( (connect(sockfd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr))) == -1 )
    {
       printf("connect error!\n"); 
       exit(1); 
    }
    else
    {
        printf("connect success!\n"); 
    }
    sleep(3);
     while( 1 )
     {
        LoginReq *req = (LoginReq*)malloc(sizeof(LoginReq));
        if(req == NULL)
        {
            printf("req malloc error\n");
            return -1;
        }

        login_req__init(req); //必须初始化

        char *user_name = "wiki"; 
        req->name = (char*)malloc(strlen(user_name)+1);
        if(req->name == NULL)
        {
            printf("req->name malloc error\n");
            return -1;            
        }
        strcpy(req->name,user_name);

        int pack_size = login_req__get_packed_size(req);

        printf("client: pack_size = %d\n",pack_size);

        uint8_t *out_buf = (uint8_t*)malloc(pack_size+2);
        login_req__pack(req,out_buf+2);// pack to out_buf+2

        uint8_t* seria_buf = out_buf;
        LoginReq* msg = NULL;
        msg = login_req__unpack(NULL,pack_size,seria_buf+2);
        
        printf("type is LoginReq,msg->LoginReq = %s\n",msg->name);

        seria_buf[0] = pack_size+1;
        seria_buf[1] = 'L';

        send(sockfd,seria_buf,pack_size+2,0);  
        
        sleep(3);

        StartReq* start_req = (StartReq*)malloc(sizeof(StartReq));
        if(start_req == NULL)
        {
            printf("start_req is NULL\n");
        }
        printf("sizoef(StartReq) = %d  sizeof(start_req->start)= %d\n",sizeof(StartReq),sizeof(start_req->start));
        start_req__init(start_req);
        start_req->start = true;
        int len = start_req__get_packed_size(start_req);
        uint8_t *send_buf = (uint8_t*)malloc(len+2);
        start_req__pack(start_req,send_buf+2);

        StartReq* msg02 = start_req__unpack(NULL,len,send_buf+2);
        printf("msg02->start = %d\n",msg02->start);

        send_buf[0] = len+1;
        send_buf[1] = 'S';
        send(sockfd,send_buf,len+2,0); 

        free(req->name);
        free(req);    
        free(seria_buf);
        free(msg);
        
        while(1)
        {
            printf("wait\n");
            sleep(8);
        }

     }  
     close(sockfd);
     exit(0);
     return 0;
}



#endif







#if 0 
//这个是以前的没用protobuf的能用的客户端程序代码
#define _PORT  8000

void myhandle(int signal_id)
{
    printf("recv signal ：%d\n",signal_id);
    exit(0);// exit father process	
}

       

int main(void)
{
    int sockfd = 0;
    pid_t pid = 0;
  //  char buffer[129];
    char* buffer = "123456789\n";
    struct sockaddr_in server_addr;  
    struct sockaddr_in client_addr; 


    signal(SIGUSR1,myhandle);
    
    //1.creat socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if( sockfd == -1 )
    {
       printf("creat socket err\n");
       exit(1);	
    }
    
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(9999);
    //client_addr.sin_addr.s_addr = htonl(INADDR_ANY);   
    bind(sockfd,(struct sockaddr*)(&client_addr),sizeof(client_addr)); //bind

    //设置要连接的服务器的地址
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    if( (connect(sockfd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr))) == -1 )
    {
       printf("connect error!\n"); 
       exit(1);	
    }


     while( 1 )
     {
       int size = strlen(buffer);
       printf("strlen(buffer) = %d\n",size);
	   send(sockfd,buffer,strlen(buffer),0);    
//	   usleep(800);
       sleep(100);
     }  
     close(sockfd);
     kill(pid,SIGUSR1);
     exit(0);
     return 0;
}

#endif