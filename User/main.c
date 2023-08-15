#include "rtthread.h"
#include "rtconfig.h"
#include "rtservice.h"
#include "rtthread.h"
#include <stdint.h>

ALIGN(RT_ALIGN_SIZE)
/*定义线程栈*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];


/*定义线程控制块*/
struct rt_thread	rt_flag1_thread;
struct rt_thread  rt_flag2_thread;

static unsigned char flag1;
static unsigned char flag2;
void delay(uint32_t count);
void delay(uint32_t count)
{
	for(;count!=0;count--);
}


void flag1_thread_entry(void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		/* 线程切换,这里手动切换
		 */
		rt_schedule();
	}
}

void flag2_thread_entry(void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		/* 线程切换,这里手动切换
		 */
		rt_schedule();
	}
}

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
int main(void)
{
	/* 硬件初始化
	 */
	
	/* 调度器初始化
	 */
	rt_system_scheduler_init();
	
	/* 初始化线程
	 */
	rt_thread_init(&rt_flag1_thread,							/*线程句柄*/
								 flag1_thread_entry,						/*线程入口地址*/
								 RT_NULL,												/*线程形参*/
								 &rt_flag1_thread_stack[0],			/*线程栈地址*/
								 sizeof(rt_flag1_thread_stack));/*线程栈大小,单位字节*/	
	/* 将线程插入到就绪列表
	 */
	rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	
	rt_thread_init(&rt_flag2_thread,							/*线程句柄*/
								 flag2_thread_entry,						/*线程入口地址*/
								 RT_NULL,												/*线程形参*/
								 &rt_flag2_thread_stack[0],			/*线程栈地址*/
								 sizeof(rt_flag2_thread_stack));/*线程栈大小,单位字节*/
	/* 将线程插入到就绪列表
	 */
	rt_list_insert_before(&(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist));	
	
	/* 启动系统调度器
	 */
	rt_system_scheduler_start();
	
	while(1);
}

