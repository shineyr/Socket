/*******************************************************************************
* 服务器端 在线客户 链表结构与操作
* 2015-12-14 yrr实现
*
********************************************************************************/

#include "config.h"

/****************************************************
函数名：insertNode
功能：插入在线用户链表新节点
参数：list--当前在线用户链表 elem--要插入的元素
返回值：返回创建的链表
***************************************************/
void insertNode(ListNode *list , User *user)
{
	/*建立新节点*/
	ListNode *node = (ListNode *)calloc(1, sizeof(ListNode));
	copyUser(&(node->user) , user);
	node->next = NULL;
	if(list == NULL)
	{			
		list = node;
	}//if
	else{
		ListNode *p = list;
		while(p->next != NULL)
		{
			p = p->next;
		}//while
		p->next = node;
	}//else
}

/****************************************************
函数名：isOnLine
功能：查看某用户是否在线
参数：list--当前在线用户链表 elem--要查看的用户元素
返回值：true or false
***************************************************/
int isOnLine(ListNode *list , User *user)
{
	ListNode *p = list , *pre = p;
	while(p!=NULL && strcmp(p->user.userName , (*user).userName) != 0)
	{
		pre = p;
		p = p->next;
	}//while
	
	/*不存在该在线用户*/
	if(p == NULL)
		return 0;
	return 1;
}

/****************************************************
函数名：deleteNode
功能：删除在线用户链表指定节点
参数：list--当前在线用户链表 elem--要删除的元素
返回值：返回创建的链表
*****************************************************/
void deleteNode(ListNode *list , User *user)
{
	if(list == NULL)
		return;

	ListNode *p = list , *pre = p;
	while(p!=NULL && strcmp(p->user.userName , (*user).userName) != 0)
	{
		pre = p;
		p = p->next;
	}//while
	
	/*不存在该在线用户*/
	if(p == NULL)
		return ;
	/*该用户位于链表头部*/
	else if(p == list)
	{
		list = list->next;
	}//elif
	/*该用户位于链表尾部*/
	else if(p->next == NULL)
	{
		pre->next = NULL;
	}//elif
	/*该用户节点位于链表中间*/
	else
	{
		pre->next = p->next;
	}//else
	/*释放该用户节点占用的空间*/
	free(p);
	p = NULL;
}

/****************************************************
函数名：displayList
功能：显示在线用户链表
参数：list--当前在线用户链表
返回值：返回创建的链表
*****************************************************/
void displayList(ListNode *list)
{
	if(list == NULL)
		return;
	else
	{
		ListNode *p = list;
		while(p->next != NULL)
		{
			printf("%s --> ", p->user.userName);
		}//while
		printf("%s\n", p->user.userName);
	}//else
}



