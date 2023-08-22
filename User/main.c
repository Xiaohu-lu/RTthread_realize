#include "rtthread.h"
#include "rtconfig.h"
#include "rtservice.h"
#include "rtthread.h"
#include "rthw.h"
#include <stdint.h>
#include "ARMCM3.h"


extern uint32_t SystemCoreClock;
ALIGN(RT_ALIGN_SIZE)
/*�����߳�ջ*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];


/*�����߳̿��ƿ�*/
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
		/* �߳��л�,�����ֶ��л�
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
		/* �߳��л�,�����ֶ��л�
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
	/* Ӳ����ʼ��
	 */
	rt_hw_interrupt_disable();/*���ж�*/
	/*SysTick�ж�Ƶ������*/
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	
	/* ϵͳ��ʱ���б��ʼ��
	 */
	rt_system_timer_init();
	
	/* ��������ʼ��
	 */
	rt_system_scheduler_init();
	
	/* ��ʼ�������߳�
	 */
	rt_thread_idle_init();
	
	/* ��ʼ���߳�
	 */
	rt_thread_init(&rt_flag1_thread,							/*�߳̾��*/
								 "f1_thread",										/*�߳�����*/
								 flag1_thread_entry,						/*�߳���ڵ�ַ*/
								 RT_NULL,												/*�߳��β�*/
								 &rt_flag1_thread_stack[0],			/*�߳�ջ��ַ*/
								 sizeof(rt_flag1_thread_stack), /*�߳�ջ��С,��λ�ֽ�*/	
								 2,															/*�߳����ȼ�*/
								 4);														/*ʱ��Ƭ*/				
	/* ���̲߳��뵽�����б�
	 */
	//rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	rt_thread_startup(&rt_flag1_thread);
								 
	rt_thread_init(&rt_flag2_thread,							/*�߳̾��*/
								 "f2_thread",										/*�߳�����*/
								 flag2_thread_entry,						/*�߳���ڵ�ַ*/
								 RT_NULL,												/*�߳��β�*/
								 &rt_flag2_thread_stack[0],			/*�߳�ջ��ַ*/
								 sizeof(rt_flag2_thread_stack), /*�߳�ջ��С,��λ�ֽ�*/
								 3,														  /*�߳����ȼ�*/
								 2);														/*ʱ��Ƭ*/				
	rt_thread_startup(&rt_flag2_thread);
								 
	rt_thread_init(&rt_flag3_thread,							/*�߳̾��*/
								 "f3_thread",										/*�߳�����*/
								 flag3_thread_entry,						/*�߳���ڵ�ַ*/
								 RT_NULL,												/*�߳��β�*/
								 &rt_flag3_thread_stack[0],			/*�߳�ջ��ַ*/
								 sizeof(rt_flag3_thread_stack), /*�߳�ջ��С,��λ�ֽ�*/
								 3,															/*�߳����ȼ�*/
								 2);														/*ʱ��Ƭ*/		
	/* ���̲߳��뵽�����б�
	 */
	//rt_list_insert_before(&(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist));	
	rt_thread_startup(&rt_flag3_thread);
								 
	/* ����ϵͳ������
	 */
	rt_system_scheduler_start();
	
	while(1);
}

