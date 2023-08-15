#include "rtthread.h"
#include "rthw.h"
#include "rtservice.h"


extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

#define IDLE_THREAD_STACK_SIZE			512

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_thread_stack[IDLE_THREAD_STACK_SIZE];


/* �����̵߳��߳̿��ƿ�
 */
struct rt_thread idle;


rt_ubase_t rt_idletask_ctr = 0;

/* rt_thread_idle_entry
 * �����̵߳���ں���
 */
void rt_thread_idle_entry(void *parameter)
{
	parameter = parameter;
	for(;;)
	{
		rt_idletask_ctr++;
	}
}

/* rt_thread_idle_init
 * �����̳߳�ʼ��
 */
void rt_thread_idle_init(void)
{
	/* ��ʼ���߳�
	 */
	rt_thread_init(&idle,"idle",rt_thread_idle_entry,RT_NULL,&rt_thread_stack[0],sizeof(rt_thread_stack));
	/* ���̲߳��뵽�����б�
	 */
	rt_list_insert_before(&(rt_thread_priority_table[RT_THREAD_PRIORITY_MAX-1]),&(idle.tlist));
}


