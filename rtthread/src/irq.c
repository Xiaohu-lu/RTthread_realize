#include "rtthread.h"
#include "rthw.h"

/*中断计数器*/
volatile rt_uint8_t rt_interrupt_nest;

/* rt_interrupt_enter
 * 进入中断次数
 * 中断服务函数进入时会调用该函数
 * 不要在应用程序中调用该函数
 */
void rt_interrupt_enter(void)
{
	rt_base_t level;
	/*关中断*/
	level = rt_hw_interrupt_disable();
	
	/*中断计算器++*/
	rt_interrupt_nest++;
	
	/*开中断*/
	rt_hw_interrupt_enable(level);
}

/* rt_interrupt_leave
 * 减去进入中断的次数
 * 中断服务函数进入时会调用该函数
 * 不要在应用程序中调用该函数
 */
void rt_interrupt_leave(void)
{
	rt_base_t level;
	/*关中断*/
	level = rt_hw_interrupt_disable();
	
	/*中断计算器--*/
	rt_interrupt_nest--;
	
	/*开中断*/
	rt_hw_interrupt_enable(level);
}

