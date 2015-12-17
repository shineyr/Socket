/*******************************************************************************
* 服务器处理用户聊天操作实现文件
* 2015-12-16 yrr实现
*
********************************************************************************/

#include "config.h"

extern ListNode *userList;

/**************************************************
函数名：groupChat
功能：群聊函数实现
参数：msg--用户发送的群聊消息 sockfd -- 发送者套接字
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
int groupChat(Message *msg , int sockfd)
{
	ListNode *p;
	
	int ret;

	/*声明数据库变量*/
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *tail;
	/*声明sql语句存储变量*/
	char sql[128];

	/*消息发送缓冲区*/
	char buf[MAX_LINE];
	/*消息内容*/
	Message message;	
	memset(&message , 0 , sizeof(message));
	
	/*查看在线用户*/
	p = userList;
	if(p == NULL)
	{
		printf("当前无人在线！消息暂存！\n");
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(ALL_NOT_ONLINE));
		strcpy(message.sendName , (*msg).sendName);
		strcpy(message.recvName , (*msg).recvName);
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send(sockfd , buf , sizeof(buf) , 0);
		return ALL_NOT_ONLINE;
	}//if
	/*向所有在线用户发送消息*/
	else
	{
		strcpy(message.sendName , (*msg).sendName);
		strcpy(message.recvName , "");
		message.msgType = message.msgType;
		strcpy(message.content , (*msg).content);
		strcpy(message.msgTime , (*msg).msgTime);
		while(p!=NULL && ((p->user).userName , message.sendName) != 0)
		{
			memset(buf , 0 , MAX_LINE);
			memcpy(buf , &message , sizeof(message));
			send((p->user).sockfd , buf , sizeof(buf) , 0);

			p = p->next;
		}//while
		/*（1）打开数据库*/
		ret = sqlite3_open(DB_NAME, &db);
		if(ret != SQLITE_OK)
		{
			printf("unable open database!\n");
			return FAILED;
		}//if
		printf("Opened database successfully.\n");

		/*（2）执行插入操作*/
		memset(sql , 0 , sizeof(sql));
		sprintf(sql , "insert into Message(msgType , sendName , recvName , content , msgTime)\
				values(%d,'%s','%s','%s', '%s');",message.msgType , message.sendName , 
				message.recvName,message.content , message.msgTime);
		printf("%s\n" , sql);

		ret = sqlite3_prepare(db , sql , strlen(sql) , &stmt , &tail);	
		if(ret != SQLITE_OK)
		{
			ret = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return FAILED;
		}//if
	
		/*（3）顺利插入*/
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		/*群聊处理成功*/	
		return SUCCESS;
	}//else	
}

/**************************************************
函数名：personalChat
功能：私聊函数实现
参数：msg--用户发送的群聊消息 sockfd -- 发送者套接字
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
int personalChat(Message *msg , int sockfd)
{
	ListNode *p;
	
	int ret;

	/*声明数据库变量*/
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *tail;
	/*声明sql语句存储变量*/
	char sql[128];

	/*消息发送缓冲区*/
	char buf[MAX_LINE];
	/*消息内容*/
	Message message;	
	memset(&message , 0 , sizeof(message));	

	/*消息发送对象和接收对象相同*/
	if(strcmp(message.sendName , message.recvName) == 0)
	{
		printf("消息不能发送到自己！\n");
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(MESSAGE_SELF));
		strcpy(message.sendName , (*msg).sendName);
		strcpy(message.recvName , (*msg).recvName);
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send(sockfd , buf , sizeof(buf) , 0);
		return MESSAGE_SELF;
	}//if

	/*查看在线用户*/
	p = userList;
	while(p!=NULL && ((p->user).userName , message.recvName) != 0)
	{		
		p = p->next;
	}//while

	if(p == NULL)
	{
		printf("该用户不在线！\n");
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(ID_NOT_ONLINE));
		strcpy(message.sendName , (*msg).sendName);
		strcpy(message.recvName , (*msg).recvName);
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send(sockfd , buf , sizeof(buf) , 0);
		return ID_NOT_ONLINE;
	}//if
	else{
		strcpy(message.sendName , (*msg).sendName);
		strcpy(message.recvName , (*msg).recvName);
		message.msgType = message.msgType;
		strcpy(message.content , (*msg).content);
		strcpy(message.msgTime , (*msg).msgTime);
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send((p->user).sockfd , buf , sizeof(buf) , 0);

		/*写到数据库*/
		/*（1）打开数据库*/
		ret = sqlite3_open(DB_NAME, &db);
		if(ret != SQLITE_OK)
		{
			printf("unable open database!\n");
			return FAILED;
		}//if
		printf("Opened database successfully.\n");

		/*（2）执行插入操作*/
		memset(sql , 0 , sizeof(sql));
		sprintf(sql , "insert into Message(msgType , sendName , recvName , content , msgTime)\
				values(%d,'%s','%s','%s', '%s');",message.msgType , message.sendName , 
				message.recvName,message.content , message.msgTime);
		printf("%s\n" , sql);

		ret = sqlite3_prepare(db , sql , strlen(sql) , &stmt , &tail);	
		if(ret != SQLITE_OK)
		{
			ret = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return FAILED;
		}//if
	
		/*（3）顺利插入*/
		ret = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		/*私聊处理成功*/	
		return SUCCESS;
	}//else	
}

/**************************************************
函数名：viewUserList
功能：查看在线用户列表函数实现 
参数：msg--用户发送的群聊消息 sockfd -- 发送者套接字
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
int viewUserList(Message *msg , int sockfd)
{
	ListNode *p;
	int ret;

	/*消息发送缓冲区*/
	char buf[MAX_LINE];
	/*消息内容*/
	Message message;	
	memset(&message , 0 , sizeof(message));
	
	/*查看在线用户*/
	p = userList;
	if(p == NULL)
	{
		printf("当前无人在线！消息暂存！\n");
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(ALL_NOT_ONLINE));
		strcpy(message.sendName , (*msg).sendName);
		strcpy(message.recvName , (*msg).recvName);
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send(sockfd , buf , sizeof(buf) , 0);
		return ALL_NOT_ONLINE;
	}//if
	else{
		strcpy(message.sendName , (*msg).sendName);
		message.msgType = message.msgType;
		strcpy(message.content , "");
		while(p!=NULL && ((p->user).userName , message.sendName) != 0)
		{
			strcat(message.content , "\t");
			strcat(message.content , (p->user).userName);
			strcat(message.content , "\t");
		}//while
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send(sockfd , buf , sizeof(buf) , 0);
	}
	return SUCCESS;
}

/**************************************************
函数名：viewUserList
功能：查看聊天记录
参数：msg--用户发送的群聊消息 sockfd -- 发送者套接字
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
int viewRecords(Message *msg , int sockfd)
{
	return SUCCESS;
}

/**************************************************
函数名：enterChat
功能：服务器处理登录成功函数实现
参数：sockfd -- 用户套接字
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
void enterChat(int *fd)
{
	int n,ret,sockfd;
	User user;
	/*消息发送缓冲区*/
	char buf[MAX_LINE];
	memset(buf , 0 , MAX_LINE);

	/*消息内容*/
	Message message;	
	memset(&message , 0 , sizeof(message));
	
	sockfd = *fd;

	while(1)
	{
		//接收用户发送的消息
		n = recv(sockfd , buf , sizeof(buf)+1 , 0);
		//printf("enterChat n = %d\n" , n);
		if(n == 0)
		{
			//关闭当前描述符
			close(sockfd);
			return ;					
		}//if				
		else{		
			memcpy(&message , buf , sizeof(buf));				
			//printf("server msgType = %d\n" , message.msgType);
			switch(message.msgType)
			{
			case GROUP_CHAT:
				{
					printf("来自%s的群聊请求！\n", message.sendName);
					/*转到群聊处理函数*/
					ret = groupChat(&message , sockfd);
					printf("群聊：%s\n", stateMsg(ret));
					break;
				}
			case PERSONAL_CHAT:
				{
					printf("来自%s的私聊请求！\n", message.sendName);
					/*转到私聊处理函数*/
					ret = personalChat(&message , sockfd);
					printf("私聊：%s\n", stateMsg(ret));
				}
				break;		
			case VIEW_USER_LIST:
				{
					printf("来自%s的查看在线用户列表请求！\n", message.sendName);
					/*转到查看在线用户列表处理函数*/
					ret = viewUserList(&message , sockfd);
					printf("查看在线列表：%s\n", stateMsg(ret));
					break;
				}
			case VIEW_RECORDS:
				{
					printf("来自%s的查看聊天记录的请求！\n", message.sendName);
					ret = viewRecords(&message , sockfd);
					printf("查看聊天记录：%s\n", stateMsg(ret));
					break;
				}
			case EXIT:
				{
					/*用户退出聊天室*/
					printf("用户%s退出聊天室！\n", message.sendName);
					memset(&user , 0 , sizeof(user));
					strcpy(user.userName , message.sendName);
					deleteNode(userList , &user);
					close(sockfd);
					return;
				}
			default:			
				break;
			}//switch
		}//else
	}//while
	return ;
}

