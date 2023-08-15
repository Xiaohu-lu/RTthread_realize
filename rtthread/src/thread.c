#include "rtthread.h"
#include "rtservice.h"

extern rt_uint8_t *rt_hw_stack_init(void				*tentry,
																		void				*parameter,
																		rt_uint8_t *stack_addr);



/* rt_thread_init
 * �̳߳�ʼ������
 * thread:			�߳̾��
 * entry:				�߳�������ں���
 * parameter:		�߳��β�
 * stack_start:	�߳�ջ��ʼ��ַ
 * stack_size:	�߳�ջ��С,�ֽ�
 */
rt_err_t rt_thread_init(struct rt_thread *thread,
												void (*entry)(void *parameter),
												void 						*parameter,
												void 						*stack_start,
												rt_uint32_t			stack_size)
{
	rt_list_init(&(thread->tlist));
	
	thread->entry = (void*)entry;
	thread->parameter = parameter;
	
	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/*��ʼ���߳�ջ,�������߳�ջָ��*/
	thread->sp = (void*)rt_hw_stack_init(thread->entry,
																			 thread->parameter,
																			(void*)((char*)thread->stack_addr + thread->stack_size -4));
	return RT_EOK;
}
