#include "rtthread.h"
#include "rthw.h"
#include "rtservice.h"


static rt_tick_t rt_tick = 0;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;
/* rt_tick_increase
 * ϵͳʱ������
 */
 #if 0
void rt_tick_increase(void)
{
	rt_ubase_t i;
	struct rt_thread *thread;
	rt_tick++;
#if 0
	/* ɨ������б��е������̵߳�remaining_tick,�����Ϊ0,���1
	 */
	for(i=0; i<RT_THREAD_PRIORITY_MAX;i++)
	{
		thread = rt_list_entry(rt_thread_priority_table[i].next,struct rt_thread,tlist);
		if(thread->remaining_tick>0)
		{
			thread->remaining_tick--;
		}
	}
	#else
	for(i=0;i<RT_THREAD_PRIORITY_MAX;i++)
	{
		thread = rt_list_entry(rt_thread_priority_table[i].next,struct rt_thread,tlist);
		if(thread->remaining_tick>0)
		{
			thread->remaining_tick--;
			if(thread->remaining_tick == 0)
			{
				rt_thread_ready_priority_group |= thread->number_mask;/*���¼������ȼ�*/
			}
		}
	}
	#endif
	/*ϵͳ����*/
	rt_schedule();
}
#else
void rt_tick_increase(void)
{
	++rt_tick;
	
	/*ɨ��ϵͳ��ʱ���б�*/
	rt_timer_check();
}

#endif

void SysTick_Handler(void)
{
	/* �����ж�
	 */
	rt_interrupt_enter();
	/* ʱ������
	 */
	rt_tick_increase();
	/* �뿪�ж�
	 */
	rt_interrupt_leave();
}

rt_tick_t rt_tick_get(void)
{
	return rt_tick;
}
