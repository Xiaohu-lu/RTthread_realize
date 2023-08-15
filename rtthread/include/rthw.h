#ifndef __RTHW_H__
#define __RTHW_H__
#include "rtdef.h"




void rt_hw_context_switch_to(rt_uint32_t to);

void PendSV_Handler(void);
void rt_hw_context_switch(rt_uint32_t from,rt_uint32_t to);
#endif


