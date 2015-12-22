/*******************************************************************************
* 客户端程序代码server.c
* 2015-12-09 yrr实现
*
********************************************************************************/

#include "config.h"


/*********************************************
函数名：main
功能：聊天室客户端main函数入口
参数：参数个数argc 用户链接地址argv
返回值：正常退出返回 0 否则返回 1
**********************************************/
int main(int argc , char *argv[])
{
	int sockfd , choice , ret; //choice代表用户在主界面所做选择,ret代表操作结果
	struct sockaddr_in servaddr;
	struct hostent *host;
	
	/*声明消息变量*/
	Message message;
	/*声明消息缓冲区*/
	char buf[MAX_LINE];

	/*UserInfo*/
	User user;
	strcpy(user.userName , "***");
	user.speak = 1;

	
    /*判断是否为合法输入*/
    if(argc != 2)
    {
        perror("usage:tcpcli <IPaddress>");
        exit(1);
    }//if

	while(1)
	{
		 /*(1) 创建套接字*/
		if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
		{
			perror("socket error");
			exit(1);
		}//if

		/*(2) 设置链接服务器地址结构*/
		bzero(&servaddr , sizeof(servaddr));		/*清空地址结构*/
		servaddr.sin_family = AF_INET;				/*使用IPV4通信域*/
		servaddr.sin_port = htons(PORT);			/*端口号转换为网络字节序*/
		//servaddr.sin_addr = *((struct in_addr *)host->h_addr);		/*可接受任意地址*/
		if(inet_pton(AF_INET , argv[1] , &servaddr.sin_addr) < 0)
		{
			printf("inet_pton error for %s\n",argv[1]);
			exit(1);
		}//if

		 /*(3) 发送链接服务器请求*/
		if( connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
		{
			perror("connect error");
			exit(1);
		}//if	

		/*(4) 显示聊天室主界面*/		
		mainInterface();	
		setbuf(stdin,NULL); //是linux中的C函数，主要用于打开和关闭缓冲机制
		scanf("%d",&choice);
		setbuf(stdin,NULL);
		while(choice != 1 && choice != 2 && choice != 3 && choice !=4)
		{
			printf("未找到命令，请重新输入！\n");
			setbuf(stdin,NULL); //是linux中的C函数，主要用于打开和关闭缓冲机制
			scanf("%d",&choice);
			setbuf(stdin,NULL);
		}//while

		/*清空缓冲区*/		
		switch(choice)
		{		
		case REGISTER:	/*注册请求*/		
			memset(&message , 0 , sizeof(message));
			memset(buf , 0 , MAX_LINE);		
			message.msgType = REGISTER;
			strcpy(message.content , "");
			message.sendAddr = servaddr;
			/*首先向服务器发送注册请求*/		
			memcpy(buf , &message , sizeof(message));	
			send(sockfd , buf , sizeof(buf) , 0);	
			registerUser(sockfd);
			//goto sign;
			break;
		case LOGIN:		/*登陆请求*/
			memset(&message , 0 , sizeof(message));
			memset(buf , 0 , MAX_LINE);
			message.msgType = LOGIN;
			strcpy(message.content , "");
			message.sendAddr = servaddr;
			/*向服务器发送登陆请求*/
			memcpy(buf , &message , sizeof(message));
			send(sockfd , buf , sizeof(buf) , 0);
			loginUser(sockfd);					
			break;	
		case HELP:		/*帮助请求，显示帮助界面*/
            helpInterface(); 
			//goto sign;                                                                
			break;
		case EXIT:
			close(sockfd);
			printf("退出聊天室!\n");
			exit(0);	/*用户退出*/
			break;
		default:
			printf("unknown operation.\n");
			//goto sign;
			break;	
		}//switch	
	}//while	
	close(sockfd);
	return 0;	
}
