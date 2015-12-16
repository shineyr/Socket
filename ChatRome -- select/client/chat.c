/*******************************************************************************
* 客户端用户聊天界面处理实现文件
* 2015-12-14 yrr实现
*
********************************************************************************/

#include "config.h"


/***********************************************
函数名：enterChat
功能：用户登陆成功后进入聊天模式
参数：user--当前用户 ， sockfd -- 套接字描述符
返回值：正常退出返回 0 ， 否则返回 1
*************************************************/
void recvMsg(int *sockfd)
{
	int connfd = *sockfd;
	int nRead;

	char buf[MAX_LINE] , str[MAX_LINE];
	Message message;
	
	time_t timep;

	printf("接收聊天信息中～ connfd = %d\n" , connfd);
	while(1)
	{
		/*接收服务器发来的消息*/
		nRead = recv(connfd , buf , sizeof(message) , 0);
		/*recv函数返回值 <0 出错  =0 链接关闭  >0接收到的字节数*/
		if(nRead <= 0)
		{
			printf("您已经异常掉线，请重新登录！\n");
			close(connfd);
			exit(0);
		}//if

		memset(&message , 0 , sizeof(message));
		memcpy(&message , buf , sizeof(buf));

		switch(message.msgType)
		{
		case VIEW_USER_LIST:
			printf("当前在线用户有：\n %s\n", message.content);
			break;
		case PERSONAL_CHAT:
			sprintf(str , "%s \t %s \t对你说： %s\n", message.sendName , message.msgTime , message.content);
			printf("\n%s\n", str);
			break;
		case GROUP_CHAT:
			sprintf(str , "%s \t %s \t发送群消息： %s\n", message.sendName , message.msgTime , message.content);
			printf("\n%s\n", str);
			break;
		case VIEW_RECORDS:
			printf("你和%s的聊天记录：\n", message.recvName);
			printf("%s\n" , message.content);
			break;
		default:
			break; 
		}//switch
	}//while	
}

/***********************************************
函数名：enterChat
功能：用户登陆成功后进入聊天模式
参数：user--当前用户 ， sockfd -- 套接字描述符
返回值：正常退出返回 0 ， 否则返回 1
*************************************************/
void enterChat(User *user , int sockfd)
{
	int nRead , choice;
	char buf[MAX_LINE] , str[MAX_LINE];
	Message message;	/*消息对象*/
	time_t timep;	/*存储当前时间*/

	pthread_t pid;	/*处理接收消息线程*/
	
	/*创建接收消息线程*/
	pthread_create(&pid , NULL , (void *)recvMsg , (void *)&sockfd);
	
	/*清空标准输入缓冲区*/
	setbuf(stdin, NULL);
	
	/*进入处理用户发送消息缓冲区*/
	while(1)
	{
		memset(&message , 0 , sizeof(message));
		strcpy(message.sendName , (*user).userName);
		memset(&str , 0 , MAX_LINE);
		memset(buf , 0 , MAX_LINE);
		/*usleep函数将该进程挂起一定时间，单位微秒，头文件unistd.h*/
		usleep(100000);

		/*进入聊天主界面*/
		chatInterface((*user).userName);
		setbuf(stdin,NULL); //是linux中的C函数，主要用于打开和关闭缓冲机制
		scanf("%d",&choice);
		while(choice != 1 && choice != 2 && choice != 3 && choice !=4 && choice != 5)
		{
			printf("未知操作，请重新输入！\n");
			setbuf(stdin,NULL); //是linux中的C函数，主要用于打开和关闭缓冲机制
			scanf("%d",&choice);
			setbuf(stdin,NULL);
		}//while

		switch(choice)
		{
		case 1: /*查看当前在线用户列表*/
			message.msgType = VIEW_USER_LIST;
			memcpy(buf , &message , sizeof(message));
			send(sockfd , buf , sizeof(buf) , 0);			
			
			break;	
		case 2: /*私聊*/
			message.msgType = PERSONAL_CHAT;
			printf("请输入聊天对象：\n");
			setbuf(stdin , NULL);
			scanf("%s" , str);
			strcpy(message.recvName , str);
			printf("请输入聊天内容：\n");
			setbuf(stdin , NULL);
			scanf("%s" , message.content);
			/*获得当前时间*/
			time(&timep);
			strcpy(message.msgTime , ctime(&timep));
			memcpy(buf , &message , sizeof(message));
			send(sockfd , buf , sizeof(buf) , 0);
			break;
		case 3: /*群聊*/
			message.msgType = GROUP_CHAT;
			printf("请输入聊天内容：\n");
			setbuf(stdin , NULL);
			scanf("%s" , message.content);
			/*获得当前时间*/
			time(&timep);
			strcpy(message.msgTime , ctime(&timep));
			memcpy(buf , &message , sizeof(message));
			send(sockfd , buf , sizeof(buf) , 0);
			break;
		case 4: /*查看聊天记录*/
			message.msgType = VIEW_RECORDS;			
			printf("请输入查看的聊天对象：\n");
			setbuf(stdin , NULL);
			scanf("%s" , str);
			strcpy(message.recvName , str);
			memcpy(buf , &message , sizeof(message));
			send(sockfd , buf , sizeof(buf) , 0);
			break;
		case 5: /*退出登陆*/
			message.msgType = EXIT;
			memcpy(buf , &message , sizeof(message));
			send(sockfd , buf , sizeof(buf) , 0);
			close(sockfd);
			exit(0);
		default: 	/*未知操作类型*/
			break;
		}//switch
	}//while
	//close(sockfd);
}


