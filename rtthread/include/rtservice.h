#ifndef __RTSERVICE_H__
#define __RTSERVICE_H__
#include "rtdef.h"

/* rt_list_init
 * 内联函数,链接节点初始化
 * l:	链表节点句柄
 */
rt_inline void rt_list_init(rt_list_t *l)
{
	l->next = l->prev = l;
}

/* rt_list_insert_after
 * 在节点l的后面插入节点n
 * l:	节点句柄,要插入到那个节点的后面
 * n: 节点句柄,要插入的节点
 */
rt_inline void rt_list_insert_after(rt_list_t *l,rt_list_t *n)
{
	l->next->prev = n;	/*l的后一个节点的前一个节点指向n节点*/
	n->next = l->next;	/*n的后一个节点指向l的后一个节点*/
	
	l->next = n;				/*l的后一个节点指向n*/
	n->prev = l;				/*n的前一个节点指向l*/
}

/* rt_list_insert_before
 * 在节点l的前面面插入节点n
 * l:	节点句柄,要插入到那个节点的前面
 * n: 节点句柄,要插入的节点
 */
rt_inline void rt_list_insert_before(rt_list_t *l,rt_list_t *n)
{
	l->prev->next = n;	/*l的前一个节点的后一个节点指向n*/
	n->prev = l->prev;	/*n的前一个节点指向l的前一个节点*/
	
	l->prev = n;				/*l的前一个节点指向n*/
	n->next = l;				/*n的后一个节点指向l*/
}

/* rt_list_remove
 * 删除一个节点
 * n: 节点句柄,要删除的节点
 */
rt_inline void rt_list_remove(rt_list_t *n)
{
	n->prev->next = n->next;	/*n的前一个节点的后一个节点指向n的后一个节点*/
	n->next->prev = n->prev;	/*n的后一个节点的前一个节点指向n的前一个节点*/
	
	n->next = n->prev = n;
}





#endif


