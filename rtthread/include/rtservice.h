#ifndef __RTSERVICE_H__
#define __RTSERVICE_H__
#include "rtdef.h"


/* ��֪һ���ṹ������ĳ�Ա�ĵ�ַ,���Ƴ��ýṹ����׵�ַ
 */
#define rt_container_of(ptr,type,member)	\
				((type*)((char*)(ptr) - (unsigned long)(&((type*)0)->member)))
					
#define rt_list_entry(node,type,member)	rt_container_of(node,type,member)



/* rt_list_init
 * ��������,���ӽڵ��ʼ��
 * l:	����ڵ���
 */
rt_inline void rt_list_init(rt_list_t *l)
{
	l->next = l->prev = l;
}

/* rt_list_insert_after
 * �ڽڵ�l�ĺ������ڵ�n
 * l:	�ڵ���,Ҫ���뵽�Ǹ��ڵ�ĺ���
 * n: �ڵ���,Ҫ����Ľڵ�
 */
rt_inline void rt_list_insert_after(rt_list_t *l,rt_list_t *n)
{
	l->next->prev = n;	/*l�ĺ�һ���ڵ��ǰһ���ڵ�ָ��n�ڵ�*/
	n->next = l->next;	/*n�ĺ�һ���ڵ�ָ��l�ĺ�һ���ڵ�*/
	
	l->next = n;				/*l�ĺ�һ���ڵ�ָ��n*/
	n->prev = l;				/*n��ǰһ���ڵ�ָ��l*/
}

/* rt_list_insert_before
 * �ڽڵ�l��ǰ�������ڵ�n
 * l:	�ڵ���,Ҫ���뵽�Ǹ��ڵ��ǰ��
 * n: �ڵ���,Ҫ����Ľڵ�
 */
rt_inline void rt_list_insert_before(rt_list_t *l,rt_list_t *n)
{
	l->prev->next = n;	/*l��ǰһ���ڵ�ĺ�һ���ڵ�ָ��n*/
	n->prev = l->prev;	/*n��ǰһ���ڵ�ָ��l��ǰһ���ڵ�*/
	
	l->prev = n;				/*l��ǰһ���ڵ�ָ��n*/
	n->next = l;				/*n�ĺ�һ���ڵ�ָ��l*/
}

/* rt_list_remove
 * ɾ��һ���ڵ�
 * n: �ڵ���,Ҫɾ���Ľڵ�
 */
rt_inline void rt_list_remove(rt_list_t *n)
{
	n->prev->next = n->next;	/*n��ǰһ���ڵ�ĺ�һ���ڵ�ָ��n�ĺ�һ���ڵ�*/
	n->next->prev = n->prev;	/*n�ĺ�һ���ڵ��ǰһ���ڵ�ָ��n��ǰһ���ڵ�*/
	
	n->next = n->prev = n;
}

/* rt_list_isempty
 * �ж������Ƿ�Ϊ��
 * n:		Ҫ�ж������ͷ
 */
rt_inline int rt_list_isempty(rt_list_t *n)
{
	return (n->next == n);
}



#endif


