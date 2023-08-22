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
	/* ��ʼ���̶߳�ʱ��
	 */
	rt_timer_init(&(thread->thread_timer),	/*��̬��ʱ������*/
								thread->name,							/*��ʱ������,ֱ��ʹ���߳���*/
								rt_thread_timeout,				/*��ʱ����*/
								thread,										/*��ʱ�����β�*/
								0,												/*��ʱʱ��*/
								RT_TIMER_FLAG_ONE_SHOT);	/*��ʱ���ı�־*/
	return RT_EOK;
}

/* rt_thread_suspend
 * �ú������ڹ���ָ�����߳�
 * ����ǹ����̱߳���,�ڵ��øú�����,
 * �������rt_schedule()����ϵͳ����
 */
rt_err_t rt_thread_suspend(rt_thread_t thread)
{
	register rt_base_t temp;
	
	/*ֻ�о������̲߳��ܱ�����,�����˳����ش�����*/
	if((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_READY)
	{
		return -RT_ERROR;
	}
	
	/*���ж�*/
	temp = rt_hw_interrupt_disable();
	
	/*�ı��߳�״̬*/
	thread->stat = RT_THREAD_SUSPEND;
	
	/*���̴߳Ӿ����б���ɾ��*/
	rt_schedule_remove_thread(thread);
	
	/*ֹͣ�̶߳�ʱ��*/
	rt_timer_stop(&(thread->thread_timer));
	
	/*���ж�*/
	rt_hw_interrupt_enable(temp);
	
	return RT_EOK;
}



/* rt_thread_sleep
 * �ú�������һ���߳�˯��һ��ʱ��,��λΪtick
 */
rt_err_t rt_thread_sleep(rt_tick_t tick)
{
	register rt_base_t temp;
	struct rt_thread *thread;
	
	/*���ж�*/
	temp = rt_hw_interrupt_disable();
	
	/*��ȡ��ǰ�̵߳��߳̿��ƿ�*/
	thread = rt_current_thread;
	
	/*�����߳�*/
	rt_thread_suspend(thread);
	
	/*�����̶߳�ʱ���ĳ�ʱʱ��*/
	rt_timer_control(&(thread->thread_timer),RT_TIMER_CTRL_SET_TIME,&tick);
	
	/*������ʱ��*/
	rt_timer_start(&(thread->thread_timer));
	
	/*���ж�*/
	rt_hw_interrupt_enable(temp);
	
	/*ִ��ϵͳ����*/
	rt_schedule();
	
	return RT_EOK;
}


/* rt_thread_delay
 * ������ʱ����
 * tick:		Ҫ��ʱ��ϵͳ����
 */
#if 0
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
#else
rt_err_t rt_thread_delay(rt_tick_t tick)
{
	return rt_thread_sleep(tick);
}
#endif

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

/* rt_thread_timeout
 * �̳߳�ʱ����
 * ���߳���ʱ���ڻ��ߵȴ�����Դ���û��߳�ʱʱ,�ú����ᱻ����
 */
void rt_thread_timeout(void *parameter)
{
	struct rt_thread *thread;
	
	thread = (struct rt_thread *)parameter;
	
	/* ���ô�����Ϊ��ʱ
	 */
	thread->error = -RT_ETIMEOUT;
	
	/*���̴߳ӹ����б�ɾ��
	 */
	rt_list_remove(&(thread->tlist));
	
	/* ���̲߳��뵽�����б�
	 */
	rt_schedule_insert_thread(thread);
	
	/* ϵͳ����
	 */
	rt_schedule();
}



