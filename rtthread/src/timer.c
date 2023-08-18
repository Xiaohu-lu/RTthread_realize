#include "rtservice.h"
#include "rtconfig.h"
#include "rtthread.h"
#include "rthw.h"
static rt_list_t rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL];


/* rt_system_timer_init
 * 初始化系统定时器列表
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
 * 初始化定时器
 * timer：		定时器句柄
 * timeout：	定时器超时函数
 * parameter：定时器超时函数形参
 * time：			定时器超时需要的系统节拍数
 * flag：			定时器标志
 */
static void _rt_timer_init(rt_timer_t timer,void (*timeout)(void *parameter),void *parameter,rt_tick_t time,rt_uint8_t flag)
{
	int i;
	/*设置标志*/
	timer->parent.flag = flag;
	
	/*先设置为非激活态*/
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	
	timer->timeout_func = timeout;
	timer->parameter    = parameter;
	
	/*初始化定时器实际超时时的系统节拍数*/
	timer->timeout_tick = 0;
	/*初始化定时器需要超时的节拍数*/
	timer->init_tick    = time;
	
	/*初始化定时器的内置节点*/
	for(i=0;i<RT_TIMER_SKIP_LIST_LEVEL;i++)
	{
		rt_list_init(&(timer->row[i]));
	}
}


/* rt_timer_init
 * 初始化一个定时器
 * timer：		定时器结构体
 * name：			定时器名称
 * timeout：	超时函数
 * parameter：超时函数形参
 * time：			定时器超时时间
 * flag：			定时器标志
 */
void rt_timer_init(rt_timer_t timer,const char *name,void (*timeout)(void *parameter),void *parameter,rt_tick_t time,rt_uint8_t flag)
{
	/*定时器对象初始化*/
	rt_object_init((rt_object_t)timer,RT_Object_Class_Timer,name);
	
	/*定时器初始化*/
	_rt_timer_init(timer,timeout,parameter,time,flag);
}

/* _rt_timer_remove
 * 把定时器从定时器链表中删除,不是全局列表
 * 推测全局列表上面连接的是不同延时到系统节拍的定时器,同一延时时间到系统节拍的定时器链接到定时器结构体的链表上
 * timer：		要删除的定时器
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
	/*判断要关闭的定时器是否有效,定时器无效返回失败*/
	if(!(timer->parent.flag & RT_TIMER_FLAG_ACTIVATED))
	{
		return -RT_ERROR;
	}
	
	/*关中断*/
	level = rt_hw_interrupt_disable();
	
	/*将定时器从定时器列表中删除*/
	_rt_timer_remove(timer);
	
	/*开中断*/
	rt_hw_interrupt_enable(level);
	
	/*改变定时器状态为非活跃*/
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	return RT_EOK;
}

/* rt_timer_control
 * 控制定时器
 * timer：	要控制的定时器
 * cmd：		控制发送的命令
 * arg：		命令参数
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
 * 启动定时器
 * timer：	要启动的定时器
 */
rt_err_t rt_timer_start(rt_timer_t timer)
{
	unsigned int row_lvl = 0;
	rt_list_t *timer_list;
	register rt_base_t level;
	rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];
	unsigned int tst_nr;
	static unsigned int random_nr;
	
	/*关中断*/
	level = rt_hw_interrupt_disable();
	
	/* 将定时器从系统定时器列表移出
	 * 因为定时器重新启动,它的超时系统节拍时间变了需要重新初始化
	 */
	_rt_timer_remove(timer);
	
	/*改变定时器状态为非active*/
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	
	/*开中断*/
	rt_hw_interrupt_enable(level);
	/* 获取timeout_tick
	 * 最大的timeout_tick不能超过RT_TICK_MAX/2
	 */
	timer->timeout_tick = rt_tick_get() + timer->init_tick;
	
	/*关中断*/
	level = rt_hw_interrupt_disable();
	
	/*将定时器插入到定时器列表*/
	/*获取系统定时器列表根节点地址*/
	timer_list = rt_timer_list;
	/* 获取系统定时器列表第一条链表根节点地址
	 * 该链表上放的是不同超时系统节拍定时器的链表
	 */
	row_head[0] = &timer_list[0];
	/* 遍历链表，系统一共有几个全局定时器列表
	 */
	for(row_lvl = 0;row_lvl < RT_TIMER_SKIP_LIST_LEVEL;row_lvl++)
	{
		/* 遍历链表，不同超时时间的链表
	   */
		for(;row_head[row_lvl] != timer_list[row_lvl].prev;row_head[row_lvl] = row_head[row_lvl]->next)
		{
			struct rt_timer *t;
			
			/*获取定时器列表节点地址*/
			rt_list_t *p = row_head[row_lvl]->next;
			
			/*根据节点地址获取父结构的指针*/
			t = rt_list_entry(p,struct rt_timer,row[row_lvl]);
			/*两个定时器的超时到的系统节拍相同,找下一个*/
			if((t->timeout_tick-timer->timeout_tick) == 0)
			{
				continue;
			}
			/* timeout_tick是uint32的,也就是说
			 * t->timeout_tick-timer->timeout_tick<RT_TICK_MAX/2一定是t的时间>timer的时间
			 * 
			 */
			else if((t->timeout_tick - timer->timeout_tick) < RT_TICK_MAX/2)
			{
				break;
			}
		}
		/*row_lel一定是RT_TIMER_SKIP_LIST_LEVEL*/
		if(row_lvl != RT_TIMER_SKIP_LIST_LEVEL-1)
		{
			/* row_head指针数组
			 * 这步是要干什么？？？？
			 */
			row_head[row_lvl + 1] = row_head[row_lvl] + 1;
		}
		
	}
	
	/*random_nr是静态变量,用于记录启动了多少定时器*/
	random_nr++;
	tst_nr = random_nr;
	
	/* 将定时器插入到系统定时器列表中
	 
	 */
	rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL-1],&(timer->row[RT_TIMER_SKIP_LIST_LEVEL-1]));
	
	/* 该for循环不会执行
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
	
	/*开中断*/
	rt_hw_interrupt_enable(level);
	
	return -RT_EOK;
}


/* rt_timer_check
 * 该函数用于扫描系统定时器列表,当有超时时间发生时
 * 就调用对应的超时函数
 * 该函数在操作系统定时器中断中调用
 */
void rt_timer_check(void)
{
	struct rt_timer *t;
	rt_tick_t current_tick;
	register rt_base_t level;
	
	/* 获取系统时基计数器rt_tick的值
	 */
	current_tick = rt_tick_get();
	
	/* 关中断
	 */
	level = rt_hw_interrupt_disable();
	
	/* 系统定时器列表不为空,则扫描定时器列表
	 */
	while(!rt_list_isempty(&rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1]))
	{
		/* 获取第一个节点定时器的地址
		 */
		t = rt_list_entry(rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1].next,struct rt_timer,row[RT_TIMER_SKIP_LIST_LEVEL-1]);
		/* 说明定时时间到
		 * curretn_tick > t->timeout_tick
		 */
		if((current_tick - t->timeout_tick) < RT_TICK_MAX/2)
		{
			/* 先将定时器从系统定时器列表移除
			 */
			_rt_timer_remove(t);
			
			/* 调用超时函数
			 */
			t->timeout_func(t->parameter);
			
			/* 重新获取 rt_tick
			 */
			current_tick = rt_tick_get();
			
			/* 周期定时器
			 */
			if((t->parent.flag & RT_TIMER_FLAG_PERIODIC) && (t->parent.flag & RT_TIMER_FLAG_ACTIVATED))
			{
				/* 启动定时器
				 */
				t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
				rt_timer_start(t);
			}
			else/*单次定时器*/
			{
				/*停止定时器*/
				t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
			}
		}
		else/*第一个节点的定时时间都不到,后面的一定都不会到*/
		{
			break;
		}
		
	}
	/* 开中断
	 */
	rt_hw_interrupt_enable(level);
}

/* rt_thread_timeout
 * 线程超时函数
 * 当线程延时到期或者等待的资源可用或者超时时,该函数会被调用
 */
void rt_thread_timeout(void *parameter)
{
	struct rt_thread *thread;
	
	thread = (struct rt_thread *)parameter;
	
	/* 设置错误码为超时
	 */
	thread->error = -RT_ETIMEOUT;
	
	/* 将线程从挂起列表中删除
	 */
	rt_list_remove(&(thread->tlist));
	
	/*将线程插入到就绪列表*/
	rt_schedule_insert_thread(thread);
	
	/*系统调度*/
	rt_schedule();
}




