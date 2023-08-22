#ifndef __RTTHREAD_H__
#define __RTTHREAD_H__
#include "rtdef.h"

struct rt_thread
{
	/* rt 对象
	 */
	char				name[RT_NAME_MAX];	/*内核对象的名字*/
	rt_uint8_t	type;								/*内核对象的类型*/
	rt_uint8_t 	flag;								/*内核对象的状态*/	
	rt_list_t		list;								/*内核对象的列表节点*/
	
	rt_list_t		tlist;							/*线程链表节点*/
	void				*sp;								/*线程栈指针*/
	void 				*entry;							/*线程入口地址*/
	void				*parameter;					/*线程形参*/
	void 				*stack_addr;				/*线程栈起始地址*/
	rt_uint32_t stack_size;					/*线程栈大小，单位为字节*/
	
	rt_ubase_t	remaining_tick;			/*用于实现阻塞延时*/
	
	rt_uint8_t	current_priority;		/*线程当前优先级*/
	rt_uint8_t	init_priority;			/*线程初始优先级*/
	rt_uint32_t	number_mask;				/*线程当前优先级掩码*/
	
	rt_err_t		error;							/*错误码*/
	rt_uint8_t	stat;								/*线程状态*/
	
	struct rt_timer	thread_timer;		/*内置的线程定时器*/
	
};
typedef struct rt_thread *rt_thread_t;

/***********************thread.c函数声明*****************************************/
rt_err_t rt_thread_init(struct rt_thread *thread,
												const char *name,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size,
												rt_uint8_t			priority);

rt_err_t rt_thread_delay(rt_tick_t tick);
rt_thread_t rt_thread_self(void);
rt_err_t rt_thread_resume(rt_thread_t thread);												
rt_err_t rt_thread_startup(rt_thread_t thread);												
												
/***********************scheduler.c函数声明*****************************************/												
void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);												
void rt_schedule(void);											
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_schedule_remove_thread(struct rt_thread *thread);

												

/***********************kservice.c函数声明*****************************************/		
char *rt_strncpy(char *dst,const char *src,rt_ubase_t n);		
int __rt_ffs(int value);

/***********************object.c函数声明*****************************************/														
struct rt_object_information * rt_object_get_information(enum rt_object_class_type type);
void rt_object_init(struct rt_object *object,enum rt_object_class_type type,const char *name);						

												
/***********************idle.c函数声明*****************************************/														

void rt_thread_idle_init(void);


/***********************irq.c函数声明*****************************************/		
void rt_interrupt_enter(void);
void rt_interrupt_leave(void);

/***********************clock.c函数声明*****************************************/		
rt_tick_t rt_tick_get(void);


/***********************timer.c函数声明*****************************************/		
void rt_system_timer_init(void);
rt_err_t rt_timer_stop(rt_timer_t timer);
rt_err_t rt_timer_control(rt_timer_t timer,int cmd,void *arg);
rt_err_t rt_timer_start(rt_timer_t timer);

void rt_timer_init(rt_timer_t timer,const char *name,void (*timeout)(void *parameter),void *parameter,rt_tick_t time,rt_uint8_t flag);

void rt_timer_check(void);
void rt_thread_timeout(void *parameter);


















#endif

