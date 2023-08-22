#include "rtthread.h"
#include "rtconfig.h"
#include "rtservice.h"
#include "rtthread.h"
#include "rthw.h"
#include <stdint.h>
#include "ARMCM3.h"


extern uint32_t SystemCoreClock;
ALIGN(RT_ALIGN_SIZE)
/*定义线程栈*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];


/*定义线程控制块*/
struct rt_thread	rt_flag1_thread;
struct rt_thread  rt_flag2_thread;
struct rt_thread  rt_flag3_thread;

static unsigned char flag1;
static unsigned char flag2;
static unsigned char flag3;

void delay(uint32_t count);
void delay(uint32_t count)
{
	for(;count!=0;count--);
}


void flag1_thread_entry(void *p_arg)
{
	for(;;)
	{
		#if 0
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		/* 线程切换,这里手动切换
		 */
		rt_schedule();
		#else
		flag1 = 1;
		//rt_thread_delay(2);
		delay(100);
		flag1 = 0;
		delay(100);
		//rt_thread_delay(2);
		#endif
	}
}

void flag2_thread_entry(void *p_arg)
{
	for(;;)
	{
		#if 0
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		/* 线程切换,这里手动切换
		 */
		rt_schedule();
		#else
		flag2 = 1;
		//rt_thread_delay(2);
		delay(100);
		flag2 = 0;
		delay(100);
		//rt_thread_delay(2);
		#endif
	}
}

void flag3_thread_entry(void *p_arg)
{
	for(;;)
	{
		flag3 = 1;
		rt_thread_delay(3);
		flag3 = 0;
		rt_thread_delay(3);
	}
}

//#define  __Vendor_SysTickConfig			1
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
int main(void)
{
	/* 硬件初始化
	 */
	rt_hw_interrupt_disable();/*关中断*/
	/*SysTick中断频率设置*/
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	
	/* 系统定时器列表初始化
	 */
	rt_system_timer_init();
	
	/* 调度器初始化
	 */
	rt_system_scheduler_init();
	
	/* 初始化空闲线程
	 */
	rt_thread_idle_init();
	
	/* 初始化线程
	 */
	rt_thread_init(&rt_flag1_thread,							/*线程句柄*/
								 "f1_thread",										/*线程名称*/
								 flag1_thread_entry,						/*线程入口地址*/
								 RT_NULL,												/*线程形参*/
								 &rt_flag1_thread_stack[0],			/*线程栈地址*/
								 sizeof(rt_flag1_thread_stack), /*线程栈大小,单位字节*/	
								 2,															/*线程优先级*/
								 4);														/*时间片*/				
	/* 将线程插入到就绪列表
	 */
	//rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	rt_thread_startup(&rt_flag1_thread);
								 
	rt_thread_init(&rt_flag2_thread,							/*线程句柄*/
								 "f2_thread",										/*线程名称*/
								 flag2_thread_entry,						/*线程入口地址*/
								 RT_NULL,												/*线程形参*/
								 &rt_flag2_thread_stack[0],			/*线程栈地址*/
								 sizeof(rt_flag2_thread_stack), /*线程栈大小,单位字节*/
								 3,														  /*线程优先级*/
								 2);														/*时间片*/				
	rt_thread_startup(&rt_flag2_thread);
								 
	rt_thread_init(&rt_flag3_thread,							/*线程句柄*/
								 "f3_thread",										/*线程名称*/
								 flag3_thread_entry,						/*线程入口地址*/
								 RT_NULL,												/*线程形参*/
								 &rt_flag3_thread_stack[0],			/*线程栈地址*/
								 sizeof(rt_flag3_thread_stack), /*线程栈大小,单位字节*/
								 3,															/*线程优先级*/
								 2);														/*时间片*/		
	/* 将线程插入到就绪列表
	 */
	//rt_list_insert_before(&(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist));	
	rt_thread_startup(&rt_flag3_thread);
								 
	/* 启动系统调度器
	 */
	rt_system_scheduler_start();
	
	while(1);
}

