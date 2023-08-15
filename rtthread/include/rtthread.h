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

/***********************thread.c��������*****************************************/
rt_err_t rt_thread_init(struct rt_thread *thread,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size);


												
												
												
/***********************scheduler.c��������*****************************************/												
void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);												
void rt_schedule(void);											
												
#endif

