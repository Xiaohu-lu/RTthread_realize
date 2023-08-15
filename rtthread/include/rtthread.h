#ifndef __RTTHREAD_H__
#define __RTTHREAD_H__
#include "rtdef.h"

struct rt_thread
{
	void				*sp;					/*线程栈指针*/
	void 				*entry;				/*线程入口地址*/
	void				*parameter;		/*线程形参*/
	void 				*stack_addr;	/*线程栈起始地址*/
	rt_uint32_t stack_size;		/*线程栈大小，单位为字节*/
	
	rt_list_t		tlist;				/*线程链表节点*/
};
typedef struct rt_thread *rt_thread_t;

/***********************thread.c函数声明*****************************************/
rt_err_t rt_thread_init(struct rt_thread *thread,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size);


												
												
												
/***********************scheduler.c函数声明*****************************************/												
void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);												
void rt_schedule(void);											
												
#endif

