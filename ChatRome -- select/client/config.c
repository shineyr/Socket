/*******************************************************************************
* 基本配置文件实现 -- 包含所需头文件
* 用户信息结构体定义
* 在线用户链表定义
********************************************************************************/
#include "config.h"

/*************************************
函数名：StateMsg
功能：根据操作结果得到相应的消息内容
参数：stateRet -- 操作结果整数值
返回值：操作结果字符串
**************************************/
char *stateMsg(int stateRet)
{
	switch(stateRet)
	{
	case EXCEED://已达服务器链接上限
		return "已达服务器链接上限！\n";
		break;
    case SUCCESS: //成功
		return "操作成功！\n";
		break;
	case FAILED:  //失败
		return "操作失败！\n";
		break;    
	case DUPLICATEID: //重复的用户名
		return "重复的用户名！\n";
		break;	
	case INVALID:	//不合法的用户名
		return "不合法输入！\n";
		break;    
	case ID_NOT_EXIST: //账号不存在
		return "账号不存在！\n";
		break;
    case WRONGPWD: //密码错误
		return "密码错误！\n";
		break;
	case ALREADY_ONLINE:
		return "该用户已在线！\n";
		break;
	case ID_NOT_ONLINE:
		return "该用户不在线！\n";
		break;
	case ALL_NOT_ONLINE:
		return "无人在线！\n";
		break;
	case MESSAGE_SELF:   //消息对象不能选择自己
		return "不能给自己发送消息\n";	
		break;
	default:
		return "未知操作结果！\n";
		break;
	}//switch
};

/*************************************
函数名：copyUser
功能：用户结构体对象拷贝操作
参数：user1--目标拷贝对象 user2--源拷贝对象
返回值：无
**************************************/
void copyUser(User *user1 , User *user2)
{
	strcpy((*user1).userName , (*user2).userName);
	strcpy((*user1).password , (*user2).password);
	(*user1).userAddr = (*user2).userAddr;
	(*user1).sockfd = (*user2).sockfd;
	(*user1).speak = (*user2).speak;
	strcpy((*user2).registerTime , (*user2).registerTime);

}
