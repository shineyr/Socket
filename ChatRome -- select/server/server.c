/*******************************************************************************
* 服务器端程序代码server.c
* 2015-12-09 yrr实现
*
********************************************************************************/

#include "config.h"

/*定义全局变量 -- 在线用户链表*/
ListNode *userList = NULL;

/*********************************************
函数名：main
功能：聊天室服务器main函数入口
参数：无
返回值：正常退出返回 0 否则返回 1
**********************************************/
int main(void)
{
	/*声明服务器监听描述符和客户链接描述符*/
	int i , n , ret , maxi , maxfd , listenfd , connfd , sockfd;

	socklen_t clilen;

	pthread_t pid;

	/*套接字选项*/
	int opt = 1;
 
	/*声明服务器地址和客户地址结构*/
	struct sockaddr_in servaddr , cliaddr;
	
	/*声明描述符集*/
	fd_set rset , allset;
	//nready为当前可用的描述符数量
	int nready , client_sockfd[FD_SETSIZE];

	/*声明消息变量*/
	Message message;
	/*声明消息缓冲区*/
	char buf[MAX_LINE];

	/*UserInfo*/
	User user;	

	/*(1) 创建套接字*/
	if((listenfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
	{
		perror("socket error.\n");
		exit(1);
	}//if

	/*(2) 初始化地址结构*/
	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	/*(3) 绑定套接字和端口*/
	setsockopt(listenfd , SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt));
	
	if(bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
	{
		perror("bind error.\n");
		exit(1);
	}//if

	/*(4) 监听*/
	if(listen(listenfd , LISTENEQ) < 0)
	{
		perror("listen error.\n");
		exit(1);
	}//if	

	/*(5) 首先初始化客户端描述符集*/
	maxfd = listenfd;
	maxi = -1;
	for(i=0; i<FD_SETSIZE; ++i)
	{
		client_sockfd[i] = -1;
	}//for

	/*清空allset描述符集*/
	FD_ZERO(&allset);

	/*将监听描述符加到allset中*/
	FD_SET(listenfd , &allset);

	/*(6) 接收客户链接*/
	while(1)
	{
		rset = allset;
		/*得到当前可读的文件描述符数*/
		nready = select(maxfd+1 , &rset , NULL , NULL , 0);
		
		/*测试listenfd是否在rset描述符集中*/
		if(FD_ISSET(listenfd , &rset))
		{
			/*接收客户端的请求*/
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
			{
				perror("accept error.\n");
				exit(1);
			}//if
			
			printf("server: got connection from %s\n", inet_ntoa(cliaddr.sin_addr));

			/*查找空闲位置，设置客户链接描述符*/
			for(i=0; i<FD_SETSIZE; ++i)
			{
				if(client_sockfd[i] < 0)
				{
					client_sockfd[i] = connfd; /*将处理该客户端的链接套接字设置在该位置*/
					break;				
				}//if
			}//for

			if(i == FD_SETSIZE)
			{		
				perror("too many connection.\n");
				exit(1);
			}//if

			/* 将来自客户的连接connfd加入描述符集 */
			FD_SET(connfd , &allset);

			/*新的连接描述符 -- for select*/
			if(connfd > maxfd)
				maxfd = connfd;
			
			/*max index in client_sockfd[]*/
			if(i > maxi)
				maxi = i;

			/*no more readable descriptors*/
			if(--nready <= 0)
				continue;
		}//if
		/*接下来逐个处理连接描述符*/
		for(i=0 ; i<=maxi ; ++i)
		{
			if((sockfd = client_sockfd[i]) < 0)
				continue;
				
			if(FD_ISSET(sockfd , &rset))
			{
				/*如果当前没有可以读的套接字，退出循环*/
				if(--nready < 0)
					break;							
				pthread_create(&pid , NULL , (void *)handleRequest , (void *)&sockfd);
				
			}//if
			/*清除处理完的链接描述符*/
			FD_CLR(sockfd , &allset);
			client_sockfd[i] = -1;			
		}//for
	}//while
		
	close(listenfd);
	return 0;
}

/*处理客户请求的线程*/
void* handleRequest(int *fd)
{
	int sockfd , ret , n;
	/*声明消息变量*/
	Message message;
	/*声明消息缓冲区*/
	char buf[MAX_LINE];

	sockfd = *fd;

	memset(buf , 0 , MAX_LINE);
	memset(&message , 0 , sizeof(message));

	//接收用户发送的消息
	n = recv(sockfd , buf , sizeof(buf)+1 , 0);
	if(n <= 0)
	{
		//关闭当前描述符，并清空描述符数组 
		fflush(stdout);
		close(sockfd);
		*fd = -1;
		printf("来自%s的退出请求！\n", inet_ntoa(message.sendAddr.sin_addr));		
		return NULL;			
	}//if				
	else{
		memcpy(&message , buf , sizeof(buf));				
		/*为每个客户操作链接创建一个线程*/					
		switch(message.msgType)
		{
		case REGISTER:						
			{
				printf("来自%s的注册请求！\n", inet_ntoa(message.sendAddr.sin_addr));
				ret = registerUser(&message , sockfd);
				memset(&message , 0 , sizeof(message));
				message.msgType = RESULT;
				message.msgRet = ret;
				strcpy(message.content , stateMsg(ret));		
				memset(buf , 0 , MAX_LINE);
				memcpy(buf , &message , sizeof(message));						
				/*发送操作结果消息*/
				send(sockfd , buf , sizeof(buf) , 0);
				printf("注册：%s\n", stateMsg(ret));	
				close(sockfd);
				*fd = -1;
				return NULL;
				break;
			}//case
		case LOGIN:
			{
				printf("来自%s的登陆请求！\n", inet_ntoa(message.sendAddr.sin_addr));
				ret = loginUser(&message , sockfd);							
				memset(&message , 0 , sizeof(message));
				message.msgType = RESULT;
				message.msgRet = ret;
				strcpy(message.content , stateMsg(ret));							
				memset(buf , 0 , MAX_LINE);
				memcpy(buf , &message , sizeof(message));						
				/*发送操作结果消息*/
				send(sockfd , buf , sizeof(buf) , 0);
				printf("登录：%s\n", stateMsg(ret));
				/*进入服务器处理聊天界面*/
				enterChat(&sockfd);												
				break;
			}//case			
		default:
			printf("unknown operation.\n");
			break;
		}//switch					
	}//else				

	close(sockfd);
	*fd = -1;
	return NULL;
}
