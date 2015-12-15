/*******************************************************************************
* 基本配置文件 -- 包含所需头文件
* 用户信息结构体定义
* 在线用户链表定义
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h> /*使用memcpy所需的头文件*/

#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>

#include <sqlite3.h>

/*FD_SETSIZE定义描述符集的大小，定义在sys/types.h中*/
#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif

#define MAX_LINE  8192
#define PORT  8888
#define LISTENEQ  6000
 
/*预定义数据库名称*/
#define DB_NAME "/home/yangrui/projects/Socket/select_chat/chatRome.db"

/*是否禁言标志*/
enum SpeakFlag{
	YES,
	No
};

/*定义服务器--客户端 消息传送类型*/
enum MessageType{	
	REGISTER = 1,	/*注册请求*/		
	LOGIN,		/*登陆请求*/
	HELP,		/*帮助请求*/
	EXIT,				/*退出请求*/
	VIEW_USER_LIST,		/*查看在线列表*/
	GROUP_CHAT,		/*群聊请求*/
	PERSONAL_CHAT,		/*私聊请求*/
	RESULT,				/*结果消息类型*/
	UNKONWN				/*未知请求类型*/
};

/*定义操作结果 */
enum StateRet{
    EXCEED, //已达服务器链接上限
    SUCCESS, //成功
	FAILED,  //失败
    DUPLICATEID, //重复的用户名
	INVALID,	//不合法的用户名
    IDNOTEXIST, //账号不存在
    WRONGPWD, //密码错误
	ALREADY_ONLINE		//已经在线
};

/*定义服务器 -- 客户端 消息传送结构体*/
typedef struct _Message{
	char content[2048];		/*针对聊天类型的消息，填充该字段*/
	int msgType;	/*消息类型 即为MessageType中的值*/
	int msgRet;		/*针对操作结果类型的消息，填充该字段*/
	struct sockaddr_in sendAddr;
	struct sockaddr_in recvAddr;	
	char registerTime[20];	//针对注册操作类型的消息记录消息发送时间	
}Message;

//用户信息结构体
typedef struct _User{
	char userName[20]; 		//用户名
	char password[20];
	struct sockaddr_in userAddr;	//用户IP地址，选择IPV4
	int sockfd;			//当前用户套接字描述符
	int speak;			//是否禁言标志
	char registerTime[20];	//记录用户注册时间	
}User;

