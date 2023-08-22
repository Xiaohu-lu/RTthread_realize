#ifndef __RTTHREAD_H__
#define __RTTHREAD_H__
#include "rtdef.h"

struct rt_thread
{
	/* rt ����
	 */
	char				name[RT_NAME_MAX];	/*�ں˶��������*/
	rt_uint8_t	type;								/*�ں˶��������*/
	rt_uint8_t 	flag;								/*�ں˶����״̬*/	
	rt_list_t		list;								/*�ں˶�����б�ڵ�*/
	
	rt_list_t		tlist;							/*�߳�����ڵ�*/
	void				*sp;								/*�߳�ջָ��*/
	void 				*entry;							/*�߳���ڵ�ַ*/
	void				*parameter;					/*�߳��β�*/
	void 				*stack_addr;				/*�߳�ջ��ʼ��ַ*/
	rt_uint32_t stack_size;					/*�߳�ջ��С����λΪ�ֽ�*/
	
	rt_ubase_t	remaining_tick;			/*����ʵ��������ʱ*/
	
	rt_uint8_t	current_priority;		/*�̵߳�ǰ���ȼ�*/
	rt_uint8_t	init_priority;			/*�̳߳�ʼ���ȼ�*/
	rt_uint32_t	number_mask;				/*�̵߳�ǰ���ȼ�����*/
	
	rt_err_t		error;							/*������*/
	rt_uint8_t	stat;								/*�߳�״̬*/
	
	struct rt_timer	thread_timer;		/*���õ��̶߳�ʱ��*/
	
};
typedef struct rt_thread *rt_thread_t;

/***********************thread.c��������*****************************************/
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
												
/***********************scheduler.c��������*****************************************/												
void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);												
void rt_schedule(void);											
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_schedule_remove_thread(struct rt_thread *thread);

												

/***********************kservice.c��������*****************************************/		
char *rt_strncpy(char *dst,const char *src,rt_ubase_t n);		
int __rt_ffs(int value);

/***********************object.c��������*****************************************/														
struct rt_object_information * rt_object_get_information(enum rt_object_class_type type);
void rt_object_init(struct rt_object *object,enum rt_object_class_type type,const char *name);						

												
/***********************idle.c��������*****************************************/														

void rt_thread_idle_init(void);


/***********************irq.c��������*****************************************/		
void rt_interrupt_enter(void);
void rt_interrupt_leave(void);

/***********************clock.c��������*****************************************/		
rt_tick_t rt_tick_get(void);


/***********************timer.c��������*****************************************/		
void rt_system_timer_init(void);
rt_err_t rt_timer_stop(rt_timer_t timer);
rt_err_t rt_timer_control(rt_timer_t timer,int cmd,void *arg);
rt_err_t rt_timer_start(rt_timer_t timer);

void rt_timer_init(rt_timer_t timer,const char *name,void (*timeout)(void *parameter),void *parameter,rt_tick_t time,rt_uint8_t flag);

void rt_timer_check(void);
void rt_thread_timeout(void *parameter);


















#endif

