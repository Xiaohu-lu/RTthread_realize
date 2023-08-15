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
	
};
typedef struct rt_thread *rt_thread_t;

/***********************thread.c��������*****************************************/
rt_err_t rt_thread_init(struct rt_thread *thread,
												const char *name,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size);

void rt_thread_delay(rt_tick_t tick);
												
												
												
/***********************scheduler.c��������*****************************************/												
void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);												
void rt_schedule(void);											
				

/***********************kservice.c��������*****************************************/		
char *rt_strncpy(char *dst,const char *src,rt_ubase_t n);		


/***********************object.c��������*****************************************/														
struct rt_object_information * rt_object_get_information(enum rt_object_class_type type);
void rt_object_init(struct rt_object *object,enum rt_object_class_type type,const char *name);						




#endif

