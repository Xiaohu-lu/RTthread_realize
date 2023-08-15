#include "rtthread.h"
#include "rtservice.h"
#include "rthw.h"

extern rt_thread_t rt_current_thread;

/* rt_thread_init
 * 线程初始化函数
 * thread:			线程句柄
 * name:				线程名称
 * entry:				线程任务入口函数
 * parameter:		线程形参
 * stack_start:	线程栈起始地址
 * stack_size:	线程栈大小,字节
 */
rt_err_t rt_thread_init(struct rt_thread *thread,
												const char *name,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size)
{
	rt_object_init((rt_object_t)thread,RT_Object_Class_Thread,name);
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

/* rt_thread_delay
 * 阻塞延时函数
 * tick:		要延时的系统节拍
 */
void rt_thread_delay(rt_tick_t tick)
{
	struct rt_thread *thread;
	/* 获取当前运行的线程控制块
	 */
	thread = rt_current_thread;
	/* 设置线程的延时时间
	 */
	thread->remaining_tick = tick;
	/* 进行系统调度
	 */
	rt_schedule();
}


