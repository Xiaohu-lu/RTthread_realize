#include "rtthread.h"
#include "rthw.h"

/*�жϼ�����*/
volatile rt_uint8_t rt_interrupt_nest;

/* rt_interrupt_enter
 * �����жϴ���
 * �жϷ���������ʱ����øú���
 * ��Ҫ��Ӧ�ó����е��øú���
 */
void rt_interrupt_enter(void)
{
	rt_base_t level;
	/*���ж�*/
	level = rt_hw_interrupt_disable();
	
	/*�жϼ�����++*/
	rt_interrupt_nest++;
	
	/*���ж�*/
	rt_hw_interrupt_enable(level);
}

/* rt_interrupt_leave
 * ��ȥ�����жϵĴ���
 * �жϷ���������ʱ����øú���
 * ��Ҫ��Ӧ�ó����е��øú���
 */
void rt_interrupt_leave(void)
{
	rt_base_t level;
	/*���ж�*/
	level = rt_hw_interrupt_disable();
	
	/*�жϼ�����--*/
	rt_interrupt_nest--;
	
	/*���ж�*/
	rt_hw_interrupt_enable(level);
}

