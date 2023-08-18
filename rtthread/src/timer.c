#include "rtservice.h"
#include "rtconfig.h"
#include "rtthread.h"
#include "rthw.h"
static rt_list_t rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL];


/* rt_system_timer_init
 * ��ʼ��ϵͳ��ʱ���б�
 */
void rt_system_timer_init(void)
{
	int i;
	for(i=0;i<sizeof(rt_timer_list)/sizeof(rt_timer_list[0]);i++)
	{
		rt_list_init(rt_timer_list + i);
	}
}

/* _rt_timer_init
 * ��ʼ����ʱ��
 * timer��		��ʱ�����
 * timeout��	��ʱ����ʱ����
 * parameter����ʱ����ʱ�����β�
 * time��			��ʱ����ʱ��Ҫ��ϵͳ������
 * flag��			��ʱ����־
 */
static void _rt_timer_init(rt_timer_t timer,void (*timeout)(void *parameter),void *parameter,rt_tick_t time,rt_uint8_t flag)
{
	int i;
	/*���ñ�־*/
	timer->parent.flag = flag;
	
	/*������Ϊ�Ǽ���̬*/
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	
	timer->timeout_func = timeout;
	timer->parameter    = parameter;
	
	/*��ʼ����ʱ��ʵ�ʳ�ʱʱ��ϵͳ������*/
	timer->timeout_tick = 0;
	/*��ʼ����ʱ����Ҫ��ʱ�Ľ�����*/
	timer->init_tick    = time;
	
	/*��ʼ����ʱ�������ýڵ�*/
	for(i=0;i<RT_TIMER_SKIP_LIST_LEVEL;i++)
	{
		rt_list_init(&(timer->row[i]));
	}
}


/* rt_timer_init
 * ��ʼ��һ����ʱ��
 * timer��		��ʱ���ṹ��
 * name��			��ʱ������
 * timeout��	��ʱ����
 * parameter����ʱ�����β�
 * time��			��ʱ����ʱʱ��
 * flag��			��ʱ����־
 */
void rt_timer_init(rt_timer_t timer,const char *name,void (*timeout)(void *parameter),void *parameter,rt_tick_t time,rt_uint8_t flag)
{
	/*��ʱ�������ʼ��*/
	rt_object_init((rt_object_t)timer,RT_Object_Class_Timer,name);
	
	/*��ʱ����ʼ��*/
	_rt_timer_init(timer,timeout,parameter,time,flag);
}

/* _rt_timer_remove
 * �Ѷ�ʱ���Ӷ�ʱ��������ɾ��,����ȫ���б�
 * �Ʋ�ȫ���б��������ӵ��ǲ�ͬ��ʱ��ϵͳ���ĵĶ�ʱ��,ͬһ��ʱʱ�䵽ϵͳ���ĵĶ�ʱ�����ӵ���ʱ���ṹ���������
 * timer��		Ҫɾ���Ķ�ʱ��
 */
rt_inline void _rt_timer_remove(rt_timer_t timer)
{
	int i;
	for(i=0;i<RT_TIMER_SKIP_LIST_LEVEL;i++)
	{
		rt_list_remove(&timer->row[i]);
	}
}


rt_err_t rt_timer_stop(rt_timer_t timer)
{
	register rt_base_t level;
	/*�ж�Ҫ�رյĶ�ʱ���Ƿ���Ч,��ʱ����Ч����ʧ��*/
	if(!(timer->parent.flag & RT_TIMER_FLAG_ACTIVATED))
	{
		return -RT_ERROR;
	}
	
	/*���ж�*/
	level = rt_hw_interrupt_disable();
	
	/*����ʱ���Ӷ�ʱ���б���ɾ��*/
	_rt_timer_remove(timer);
	
	/*���ж�*/
	rt_hw_interrupt_enable(level);
	
	/*�ı䶨ʱ��״̬Ϊ�ǻ�Ծ*/
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	return RT_EOK;
}

/* rt_timer_control
 * ���ƶ�ʱ��
 * timer��	Ҫ���ƵĶ�ʱ��
 * cmd��		���Ʒ��͵�����
 * arg��		�������
 */
rt_err_t rt_timer_control(rt_timer_t timer,int cmd,void *arg)
{
	switch(cmd)
	{
		case RT_TIMER_CTRL_GET_TIME:
			*(rt_tick_t *)arg = timer->init_tick;
			break;
		case RT_TIMER_CTRL_SET_TIME:
			timer->init_tick = *(rt_tick_t *)arg;
			break;
		case RT_TIMER_CTRL_SET_ONESHOT:
			timer->parent.flag &= ~RT_TIMER_FLAG_PERIODIC;
			break;
		case RT_TIMER_CTRL_SET_PERIODIC:
			timer->parent.flag &= ~RT_TIMER_FLAG_PERIODIC;
			break;
	}
	return RT_EOK;
}

/* rt_timer_start
 * ������ʱ��
 * timer��	Ҫ�����Ķ�ʱ��
 */
rt_err_t rt_timer_start(rt_timer_t timer)
{
	unsigned int row_lvl = 0;
	rt_list_t *timer_list;
	register rt_base_t level;
	rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];
	unsigned int tst_nr;
	static unsigned int random_nr;
	
	/*���ж�*/
	level = rt_hw_interrupt_disable();
	
	/* ����ʱ����ϵͳ��ʱ���б��Ƴ�
	 * ��Ϊ��ʱ����������,���ĳ�ʱϵͳ����ʱ�������Ҫ���³�ʼ��
	 */
	_rt_timer_remove(timer);
	
	/*�ı䶨ʱ��״̬Ϊ��active*/
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	
	/*���ж�*/
	rt_hw_interrupt_enable(level);
	/* ��ȡtimeout_tick
	 * ����timeout_tick���ܳ���RT_TICK_MAX/2
	 */
	timer->timeout_tick = rt_tick_get() + timer->init_tick;
	
	/*���ж�*/
	level = rt_hw_interrupt_disable();
	
	/*����ʱ�����뵽��ʱ���б�*/
	/*��ȡϵͳ��ʱ���б���ڵ��ַ*/
	timer_list = rt_timer_list;
	/* ��ȡϵͳ��ʱ���б��һ��������ڵ��ַ
	 * �������Ϸŵ��ǲ�ͬ��ʱϵͳ���Ķ�ʱ��������
	 */
	row_head[0] = &timer_list[0];
	/* ��������ϵͳһ���м���ȫ�ֶ�ʱ���б�
	 */
	for(row_lvl = 0;row_lvl < RT_TIMER_SKIP_LIST_LEVEL;row_lvl++)
	{
		/* ����������ͬ��ʱʱ�������
	   */
		for(;row_head[row_lvl] != timer_list[row_lvl].prev;row_head[row_lvl] = row_head[row_lvl]->next)
		{
			struct rt_timer *t;
			
			/*��ȡ��ʱ���б�ڵ��ַ*/
			rt_list_t *p = row_head[row_lvl]->next;
			
			/*���ݽڵ��ַ��ȡ���ṹ��ָ��*/
			t = rt_list_entry(p,struct rt_timer,row[row_lvl]);
			/*������ʱ���ĳ�ʱ����ϵͳ������ͬ,����һ��*/
			if((t->timeout_tick-timer->timeout_tick) == 0)
			{
				continue;
			}
			/* timeout_tick��uint32��,Ҳ����˵
			 * t->timeout_tick-timer->timeout_tick<RT_TICK_MAX/2һ����t��ʱ��>timer��ʱ��
			 * 
			 */
			else if((t->timeout_tick - timer->timeout_tick) < RT_TICK_MAX/2)
			{
				break;
			}
		}
		/*row_lelһ����RT_TIMER_SKIP_LIST_LEVEL*/
		if(row_lvl != RT_TIMER_SKIP_LIST_LEVEL-1)
		{
			/* row_headָ������
			 * �ⲽ��Ҫ��ʲô��������
			 */
			row_head[row_lvl + 1] = row_head[row_lvl] + 1;
		}
		
	}
	
	/*random_nr�Ǿ�̬����,���ڼ�¼�����˶��ٶ�ʱ��*/
	random_nr++;
	tst_nr = random_nr;
	
	/* ����ʱ�����뵽ϵͳ��ʱ���б���
	 
	 */
	rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL-1],&(timer->row[RT_TIMER_SKIP_LIST_LEVEL-1]));
	
	/* ��forѭ������ִ��
	 */
	for(row_lvl=2;row_lvl<=RT_TIMER_SKIP_LIST_LEVEL;row_lvl++)
	{
		if(!(tst_nr & RT_TIMER_SKIP_LIST_MASK))
		{
			rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL-row_lvl],&(timer->row[RT_TIMER_SKIP_LIST_LEVEL-row_lvl]));
		}
		else
		{
			break;
		}
		tst_nr >>= (RT_TIMER_SKIP_LIST_MASK + 1)>>1;
	}
	
	timer->parent.flag |= RT_TIMER_FLAG_ACTIVATED;
	
	/*���ж�*/
	rt_hw_interrupt_enable(level);
	
	return -RT_EOK;
}


/* rt_timer_check
 * �ú�������ɨ��ϵͳ��ʱ���б�,���г�ʱʱ�䷢��ʱ
 * �͵��ö�Ӧ�ĳ�ʱ����
 * �ú����ڲ���ϵͳ��ʱ���ж��е���
 */
void rt_timer_check(void)
{
	struct rt_timer *t;
	rt_tick_t current_tick;
	register rt_base_t level;
	
	/* ��ȡϵͳʱ��������rt_tick��ֵ
	 */
	current_tick = rt_tick_get();
	
	/* ���ж�
	 */
	level = rt_hw_interrupt_disable();
	
	/* ϵͳ��ʱ���б�Ϊ��,��ɨ�趨ʱ���б�
	 */
	while(!rt_list_isempty(&rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1]))
	{
		/* ��ȡ��һ���ڵ㶨ʱ���ĵ�ַ
		 */
		t = rt_list_entry(rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1].next,struct rt_timer,row[RT_TIMER_SKIP_LIST_LEVEL-1]);
		/* ˵����ʱʱ�䵽
		 * curretn_tick > t->timeout_tick
		 */
		if((current_tick - t->timeout_tick) < RT_TICK_MAX/2)
		{
			/* �Ƚ���ʱ����ϵͳ��ʱ���б��Ƴ�
			 */
			_rt_timer_remove(t);
			
			/* ���ó�ʱ����
			 */
			t->timeout_func(t->parameter);
			
			/* ���»�ȡ rt_tick
			 */
			current_tick = rt_tick_get();
			
			/* ���ڶ�ʱ��
			 */
			if((t->parent.flag & RT_TIMER_FLAG_PERIODIC) && (t->parent.flag & RT_TIMER_FLAG_ACTIVATED))
			{
				/* ������ʱ��
				 */
				t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
				rt_timer_start(t);
			}
			else/*���ζ�ʱ��*/
			{
				/*ֹͣ��ʱ��*/
				t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
			}
		}
		else/*��һ���ڵ�Ķ�ʱʱ�䶼����,�����һ�������ᵽ*/
		{
			break;
		}
		
	}
	/* ���ж�
	 */
	rt_hw_interrupt_enable(level);
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
	
	/* ���̴߳ӹ����б���ɾ��
	 */
	rt_list_remove(&(thread->tlist));
	
	/*���̲߳��뵽�����б�*/
	rt_schedule_insert_thread(thread);
	
	/*ϵͳ����*/
	rt_schedule();
}




