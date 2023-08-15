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
	
};
typedef struct rt_thread *rt_thread_t;

/***********************thread.c函数声明*****************************************/
rt_err_t rt_thread_init(struct rt_thread *thread,
												const char *name,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size);

void rt_thread_delay(rt_tick_t tick);
												
												
												
/***********************scheduler.c函数声明*****************************************/												
void rt_system_scheduler_init(void);
void rt_system_scheduler_start(void);												
void rt_schedule(void);											
				

/***********************kservice.c函数声明*****************************************/		
char *rt_strncpy(char *dst,const char *src,rt_ubase_t n);		


/***********************object.c函数声明*****************************************/														
struct rt_object_information * rt_object_get_information(enum rt_object_class_type type);
void rt_object_init(struct rt_object *object,enum rt_object_class_type type,const char *name);						




#endif

