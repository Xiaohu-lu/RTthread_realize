#include "rtthread.h"
#include "rtconfig.h"
#include "rtservice.h"
#include "rtthread.h"
#include <stdint.h>

ALIGN(RT_ALIGN_SIZE)
/*�����߳�ջ*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];


/*�����߳̿��ƿ�*/
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
		/* �߳��л�,�����ֶ��л�
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
		/* �߳��л�,�����ֶ��л�
		 */
		rt_schedule();
	}
}

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
int main(void)
{
	/* Ӳ����ʼ��
	 */
	
	/* ��������ʼ��
	 */
	rt_system_scheduler_init();
	
	/* ��ʼ���߳�
	 */
	rt_thread_init(&rt_flag1_thread,							/*�߳̾��*/
								 flag1_thread_entry,						/*�߳���ڵ�ַ*/
								 RT_NULL,												/*�߳��β�*/
								 &rt_flag1_thread_stack[0],			/*�߳�ջ��ַ*/
								 sizeof(rt_flag1_thread_stack));/*�߳�ջ��С,��λ�ֽ�*/	
	/* ���̲߳��뵽�����б�
	 */
	rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	
	rt_thread_init(&rt_flag2_thread,							/*�߳̾��*/
								 flag2_thread_entry,						/*�߳���ڵ�ַ*/
								 RT_NULL,												/*�߳��β�*/
								 &rt_flag2_thread_stack[0],			/*�߳�ջ��ַ*/
								 sizeof(rt_flag2_thread_stack));/*�߳�ջ��С,��λ�ֽ�*/
	/* ���̲߳��뵽�����б�
	 */
	rt_list_insert_before(&(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist));	
	
	/* ����ϵͳ������
	 */
	rt_system_scheduler_start();
	
	while(1);
}

