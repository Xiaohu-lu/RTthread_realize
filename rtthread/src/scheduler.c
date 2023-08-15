#include "rtservice.h"
#include "rtconfig.h"
#include "rtthread.h"
#include "rthw.h"

/* 已知一个结构体里面的成员的地址,反推出该结构体的首地址
 */
#define rt_container_of(ptr,type,member)	\
				((type*)((char*)(ptr) - (unsigned long)(&((type*)0)->member)))
					
#define rt_list_entry(node,type,member)	rt_container_of(node,type,member)


/* 线程就绪列表
 */
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

rt_thread_t rt_current_thread;

/* rt_system_scheduler_init
 * 调度器初始化
 */
void rt_system_scheduler_init(void)
{
	/* regisetr c语言关键字修饰
	 * 防止编译器优化
	 */
	register rt_base_t offset;
	/* 线程就绪列表初始化
	 */
	for(offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset++)
	{
		rt_list_init(&rt_thread_priority_table[offset]);
	}
	
	/* 初始化当前线程控制块指针
	 */
	rt_current_thread = RT_NULL;
	
	
}

/* rt_system_scheduler_start
 * 启动调度器
 */
void rt_system_scheduler_start(void)
{
	register struct rt_thread *to_thread;
	
	/* 手动指定第一个运行的线程
	 */
	to_thread = rt_list_entry(rt_thread_priority_table[0].next,struct rt_thread,tlist);
	
	rt_current_thread = to_thread;
	
	/* 切换到第一个线程,该函数在context rvds.s中实现
	 * 在rthw.h中声明,用于实现第一个线程切换
	 * 当一个汇编函数在c文件中调用的时候,如果有形参
	 * 则执行的时候会将形参传入到cpu寄存器r0
	 */
	rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);
}

/* rt_schedule
 * 系统调度
 */
void rt_schedule(void)
{
	struct rt_thread *to_thread;	
	struct rt_thread *from_thread;
#if 0	
	/* 两个线程轮流切换
	 */
	if(rt_current_thread == rt_list_entry(rt_thread_priority_table[0].next,struct rt_thread,tlist))
	{
		from_thread = rt_current_thread;
		/* 需要改变,因为第一个成员不是SP指针了
		 */
		to_thread = rt_list_entry(rt_thread_priority_table[1].next,struct rt_thread,tlist);
		rt_current_thread = to_thread;
	}
	else
	{
		from_thread = rt_current_thread;
		to_thread = rt_list_entry(rt_thread_priority_table[0].next,struct rt_thread,tlist);
		rt_current_thread = to_thread;
	}
#else/*加入阻塞延时*/
	
#endif


	/* 发起PendSV异常
	 * 产生上下文切换
	 */
	rt_hw_context_switch((rt_uint32_t)&from_thread->sp,(rt_uint32_t)&to_thread->sp);
	
}

