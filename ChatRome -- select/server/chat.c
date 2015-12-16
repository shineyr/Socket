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
参数：msg--用户发送的群聊消息
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
int groupChat(Message *msg)
{
	ListNode *p;
	
	int ret , sockfd;

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
	strcpy(message.sendName , (*msg).sendName);
	strcpy(message.recvName , "");
	message.msgType = message.msgType;
	strcpy(message.content , (*msg).content);
	strcpy(message.msgTime , (*msg).msgTime);

	/*查看在线用户*/
	p = userList;
	if(p == NULL)
	{
		printf("当前无人在线！消息暂存！\n");
	}//if
	
	/*向所有在线用户发送消息*/
	while(p!=NULL && ((p->user).userName , message.sendName) != 0)
	{
		sockfd = (p->user).sockfd;
		memset(buf , 0 , MAX_LINE);
		memcpy(buf , &message , sizeof(message));
		send(sockfd , buf , sizeof(buf) , 0);

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
}

/**************************************************
函数名：personalChat
功能：私聊函数实现
参数：msg--用户发送的群聊消息
返回值：成功登陆返回SUCCESS 否则返回异常类型
****************************************************/
int personalChat(Message *msg)
{
}
