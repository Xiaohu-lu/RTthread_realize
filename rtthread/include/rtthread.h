#ifndef __RTTHREAD_H__
#define __RTTHREAD_H__
#include "rtdef.h"

struct rt_thread
{
	void				*sp;					/*�߳�ջָ��*/
	void 				*entry;				/*�߳���ڵ�ַ*/
	void				*parameter;		/*�߳��β�*/
	void 				*stack_addr;	/*�߳�ջ��ʼ��ַ*/
	rt_uint32_t stack_size;		/*�߳�ջ��С����λΪ�ֽ�*/
	
	rt_list_t		tlist;				/*�߳�����ڵ�*/
};
typedef struct rt_thread *rt_thread_t;


#endif

