#ifndef __RTHW_H__
#define __RTHW_H__
#include "rtdef.h"

/***********************cpuport.c函数声明*****************************************/
rt_uint8_t *rt_hw_stack_init(void				*tentry,
														 void				*parameter,
														 rt_uint8_t *stack_addr);


														 
/***********************context_rvds.s函数声明*****************************************/
void rt_hw_context_switch_to(rt_uint32_t to);
void PendSV_Handler(void);
void rt_hw_context_switch(rt_uint32_t from,rt_uint32_t to);
rt_base_t rt_hw_interrupt_disable(void);
void rt_hw_interrupt_enable(rt_base_t level);




#endif


