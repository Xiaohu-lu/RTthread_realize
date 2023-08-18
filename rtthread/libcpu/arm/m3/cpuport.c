#include "rtservice.h"

struct exception_stack_frame
{
	/* �쳣����ʱ,�Զ����ص�CPU�Ĵ���������
	 */
	rt_uint32_t r0;
	rt_uint32_t r1;
	rt_uint32_t r2;
	rt_uint32_t r3;
	rt_uint32_t r12;
	rt_uint32_t lr;
	rt_uint32_t pc;
	rt_uint32_t psr;
	
};

struct stack_frame
{
	/* һ�㷢��ʱ,�ֶ����ص�CPU�Ĵ���������
	 */
	rt_uint32_t r4;
	rt_uint32_t r5;
	rt_uint32_t r6;
	rt_uint32_t r7;
	rt_uint32_t r8;
	rt_uint32_t r9;
	rt_uint32_t r10;
	rt_uint32_t r11;
	struct exception_stack_frame exception_stack_frame;
};


/* ���ڴ洢��һ���̵߳�ջ��SP��ָ��
 */
rt_uint32_t rt_interrupt_from_thread;

/* ���ڴ洢��һ����Ҫ���е��̵߳�ջ��sp��ָ��
 */
rt_uint32_t rt_interrupt_to_thread;

/* PendSV�жϷ�����ִ�б�־
 */
rt_uint32_t rt_thread_switch_interrupt_flag;


/* rt_hw_stack_init
 * ��ʼ������ʱ,ջ��ʼ��
 * tentry:		������ں���
 * parameter:	�����β�
 * stack_addr:����ջ����ַ-4
 * return:		���س�ʼ�����ջ����ַadd-16*4-8����
 */
rt_uint8_t *rt_hw_stack_init(void				*tentry,
														 void				*parameter,
														 rt_uint8_t *stack_addr)
{
	struct stack_frame *stack_frame;
	rt_uint8_t				 *stk;
	unsigned long			 i;
	
	/* ��ȡջ��ָ��
	 * rt_hw_stack_init���ε�ʱ��stack_addr��ջ����ַ-4
	 */
	stk = stack_addr + sizeof(rt_uint32_t);
	
	/* ��stkָ��������8�ֽڶ���
	 */
	stk = (rt_uint8_t*)RT_ALIGN_DOWN((rt_uint32_t)stk,8);
	
	/* stkָ����������ƶ�sizeof(struct stack frame)��ƫ��
	 */
	stk -= sizeof(struct stack_frame);
	
	/* ��stkָ��ǿ��ת��Ϊstack frame����,�洢��stack_frame
	 */
	stack_frame = (struct stack_frame*)stk;
	
	/* ��stack_frameΪ��ʼ��ַ,��ջ�ռ������sizeof(struct stack_frame)
	 * �ڴ��ʼ��Ϊ0xdeadbeef
	 */
	for(i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i++)
	{
		((rt_uint32_t*)stack_frame)[i] = 0xdeadbeef;
	}
	
	/* ��ʼ���쳣����ʱ�Զ����ֵ��Ĵ���
	 */
	stack_frame->exception_stack_frame.r0 = (unsigned long)parameter;		/*r0: argument*/
	stack_frame->exception_stack_frame.r1 = 0;
	stack_frame->exception_stack_frame.r2 = 0;
	stack_frame->exception_stack_frame.r3 = 0;
	stack_frame->exception_stack_frame.r12 = 0;
	stack_frame->exception_stack_frame.lr = 0;
	stack_frame->exception_stack_frame.pc = (unsigned long)tentry;
	stack_frame->exception_stack_frame.psr = 0x01000000L;
	
	/*�����߳�ջָ��*/
	return stk;
}

