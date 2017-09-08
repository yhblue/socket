#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <arpa/inet.h>

#define _PORT 8000


void myhandle(int signal_id)
{
    printf("recv signal ：%d\n",signal_id);
    exit(0);//
}

int main(void)
{
    int sockfd = 0;
    int new_sockfd = 0;
    struct sockaddr_in server_addr;  
    struct sockaddr_in client_addr;  
    int optval = 1;
    
    signal(SIGUSR1,myhandle);
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket err:"); 
        exit(0);	     	
    }	
   
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   
    
    setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,&optval, sizeof(optval));
    
    bind(sockfd,(struct sockaddr*)(&server_addr),sizeof(server_addr));
    
    listen(sockfd,SOMAXCONN);//3 user connect

    socklen_t addrlen = sizeof(client_addr);
    new_sockfd = accept(sockfd, (struct sockaddr*)(&client_addr),&addrlen); 
    printf("recieve client addrenss is %s\n",inet_ntoa(client_addr.sin_addr));
    printf("recieve client PORT is %d\n",ntohs(client_addr.sin_port));

      if(new_sockfd == -1)
      {
          printf("accept err\n");
          exit(0);	
      }
      else
      {
      	  int pid = fork();
      	  
      	  if(pid == -1)
      	  {
      	     printf("fork err\n");
      	     exit(0);	
      	  }
      	  if(pid == 0)
      	  {
      	       char buffer[129];
      	       int rec_byte = 0;	
      	       
      	       close(sockfd);
      	       while(1)
      	       {
                    rec_byte = recv(new_sockfd, buffer, 128,0);
                    //sleep(1);

                    buffer[rec_byte] = '\0';
                 	  int size = strlen(buffer);
                    printf("strlen(buffer) = %d\n",size);
                    if( rec_byte > 0 )
                       printf("sever received : %s\n",buffer);	
                    if( rec_byte == 0 )  //当客户端按下ctrl+c时候，将会发送给服务器一个FIN指令(\0),rec_byte=0，执行下面语句
                    {
                       printf("client close\n");
                       break;
                    }
                    if(rec_byte == -1 )
                    {
                       printf("recv err\n");
                       break;	
                    }   
      	        }
      	       close(new_sockfd);
                     kill(getppid(),SIGUSR1);
                     exit(0);	
      	    }
      	 if(pid >0)
      	 {
      	     char buffer[129] = {0};	
      	     while( 1 )
      	     {
      		fgets(buffer,128,stdin);
      		send(new_sockfd,buffer,strlen(buffer),0);    
      		
      		if( strncmp(buffer,"end",3) == 0 )
      		{
      		   break;	
      		}  	
      	     }
      	     close(new_sockfd);
      	     close(sockfd);
      	     kill(pid,SIGUSR1);
      	     exit(0);	     	
      	 }             	
      }
    return 0;
}

