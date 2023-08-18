#include "rtthread.h"
#include "rtservice.h"
#include "rthw.h"

extern rt_thread_t rt_current_thread;
extern rt_uint32_t rt_thread_ready_priority_group;
/* rt_thread_init
 * �̳߳�ʼ������
 * thread:			�߳̾��
 * name:				�߳�����
 * entry:				�߳�������ں���
 * parameter:		�߳��β�
 * stack_start:	�߳�ջ��ʼ��ַ
 * stack_size:	�߳�ջ��С,�ֽ�
 */
rt_err_t rt_thread_init(struct rt_thread *thread,
												const char *name,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size,
												rt_uint8_t			priority)
{
	rt_object_init((rt_object_t)thread,RT_Object_Class_Thread,name);
	rt_list_init(&(thread->tlist));
	
	thread->entry = (void*)entry;
	thread->parameter = parameter;
	
	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/*��ʼ���߳�ջ,�������߳�ջָ��*/
	thread->sp = (void*)rt_hw_stack_init(thread->entry,
																			 thread->parameter,
																			(void*)((char*)thread->stack_addr + thread->stack_size -4));
	/*��ʼ�����ȼ�*/
	thread->init_priority	= priority;
	thread->current_priority = priority;
	thread->number_mask = 0;
	
	/*�������״̬*/
	thread->error = RT_EOK;
	thread->stat  = RT_THREAD_INIT;
	return RT_EOK;
}

/* rt_thread_delay
 * ������ʱ����
 * tick:		Ҫ��ʱ��ϵͳ����
 */
void rt_thread_delay(rt_tick_t tick)
{
	#if 0
	struct rt_thread *thread;
	/* ��ȡ��ǰ���е��߳̿��ƿ�
	 */
	thread = rt_current_thread;
	/* �����̵߳���ʱʱ��
	 */
	thread->remaining_tick = tick;
	/* ����ϵͳ����
	 */
	rt_schedule();
	#else
	register rt_base_t temp;
	struct rt_thread *thread;
	/*�ر��ж�*/
	temp = rt_hw_interrupt_disable();
	thread = rt_current_thread;
	thread->remaining_tick = tick;
	
	/*�ı��߳�״̬*/
	thread->stat = RT_THREAD_SUSPEND;
	/*�������?Ӧ�û����������߳���������ȼ�*/
	rt_thread_ready_priority_group &= ~thread->number_mask;
	/*ʹ���ж�*/
	rt_hw_interrupt_enable(temp);
	/*����ϵͳ����*/
	rt_schedule();
	#endif
}

/* rt_thread_self
 * ���ص�ǰ���е��߳�
 */
rt_thread_t rt_thread_self(void)
{
	return rt_current_thread;
}


/* rt_thread_resume
 * �ָ��߳�
 * thread:		Ҫ�ָ����߳�
 */
rt_err_t rt_thread_resume(rt_thread_t thread)
{
	register rt_base_t temp;
	/*�����ָ����̱߳����ǹ���״̬,���򷵻�ʧ��*/
	if((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_SUSPEND)
	{
		return -RT_ERROR;
	}
	
	/*���ж�*/
	temp = rt_hw_interrupt_disable();
	
	/*�ӹ���������Ƴ�*/
	rt_list_remove(&(thread->tlist));
	
	/*���ж�*/
	rt_hw_interrupt_enable(temp);
	
	/*��������б�*/
	rt_schedule_insert_thread(thread);
	return RT_EOK;
}


/* rt_thread_startup
 * ����һ���߳�
 * thread:		Ҫ�������߳�
 */
rt_err_t rt_thread_startup(rt_thread_t thread)
{
	/*���õ�ǰ���ȼ�Ϊ��ʼ�����ȼ�*/
	thread->current_priority = thread->init_priority;
	thread->number_mask = 1L<<thread->current_priority;
	
	/*�ı��̵߳�״̬Ϊ����״̬*/
	thread->stat = RT_THREAD_SUSPEND;
	
	/*Ȼ��ָ��߳�*/
	rt_thread_resume(thread);
	
	if(rt_thread_self() != RT_NULL)
	{
		/*ϵͳ����*/
		rt_schedule();
	}
	return RT_EOK;
}


