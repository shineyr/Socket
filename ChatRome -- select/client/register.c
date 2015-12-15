/*******************************************************************************
* 客户端用户基本操作处理实现文件
* 2015-12-10 yrr实现
*
********************************************************************************/

#include "config.h"

/*********************************************
函数名：registerUser
功能：用户注册函数实现
参数：套接字描述符
返回值：成功登陆返回SUCCESS 否则返回异常类型
**********************************************/
int registerUser(int sockfd)
{
	int ret;
	/*声明用户需要的注册信息*/
	User user;
	char buf[MAX_LINE];
	Message message;
	/*获取用户输入*/
	printf("请输入注册用户名：\n");
	memset(user.userName , 0 , sizeof(user.userName));
	scanf("%s" , user.userName);
	printf("user.UserName = %s\n" , user.userName);
	
	printf("请输入注册用户密码：\n");
	memset(user.password , 0 , sizeof(user.password));
	scanf("%s" , user.password);
	printf("user.password = %s\n" , user.password);
	//当前用户允许发言
	user.speak = YES;

	memset(buf , 0 , MAX_LINE);
	memcpy(buf , &user , sizeof(user));
	send(sockfd , buf , sizeof(buf) , 0);

	/*接收注册结果*/
	memset(buf , 0 , MAX_LINE);
	recv(sockfd , buf , sizeof(buf) , 0);
	memset(&message , 0 , sizeof(message));
	memcpy(&message , buf , sizeof(buf));
	
	printf("%s\n",message.content);	
	return message.msgRet;
}
