#include "rtthread.h"
#include "rtservice.h"

extern rt_uint8_t *rt_hw_stack_init(void				*tentry,
																		void				*parameter,
																		rt_uint8_t *stack_addr);



/* rt_thread_init
 * 线程初始化函数
 * thread:			线程句柄
 * entry:				线程任务入口函数
 * parameter:		线程形参
 * stack_start:	线程栈起始地址
 * stack_size:	线程栈大小,字节
 */
rt_err_t rt_thread_init(struct rt_thread *thread,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size)
{
	rt_list_init(&(thread->tlist));
	
	thread->entry = (void*)entry;
	thread->parameter = parameter;
	
	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/*初始化线程栈,并返回线程栈指针*/
	thread->sp = (void*)rt_hw_stack_init(thread->entry,
																			 thread->parameter,
																			(void*)((char*)thread->stack_addr + thread->stack_size -4));
	return RT_EOK;
}
