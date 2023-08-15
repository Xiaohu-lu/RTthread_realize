;*****************************************************************************
;								全局变量
;*****************************************************************************
;IMPORT声明符号来自外部文件,extern
	IMPORT rt_thread_switch_interrupt_flag	
	IMPORT rt_interrupt_from_thread
	IMPORT rt_interrupt_to_thread

;*****************************************************************************
;								常量
;*****************************************************************************
;-----------------------------------------------------------------------------
;有关内核外设寄存器定义可参考：STM32F10xxx Cortex-M3 programming manual
;系统控制外设SCB地址范围：0xE000ED00-0xE000ED3F
;-----------------------------------------------------------------------------
;EQU---->给数字常量去一个名,相当于#define 
SCB_VTOR		EQU			0xE000ED08			;向量表偏移寄存器,
NVIC_INT_CTRL	EQU			0xE000ED04			;中断控制状态寄存器
NVIC_SYSPRI2	EQU			0xE000ED20			;系统优先级寄存器
NVIC_PENDSV_PRI	EQU			0x00FF0000			;PendSV优先级值
NVIC_PENDSVSET	EQU			0x10000000			;触发PendSV exception的值

;*****************************************************************************
;								代码产生指令
;*****************************************************************************
	AREA |.text|, CODE, READONLY, ALIGN=2	;AREA汇编一个代码段,||是因为不是以字母开始(.text)段名,CODE表示代码,READONLY,表示只读,ALIGN=2,表示2*2字节对齐
	THUMB									;Thumb指令集
	REQUIRE8								;当前文件栈需8字节对齐
	PRESERVE8								;当前文件栈需8字节对齐

;/*
; *-----------------------------------------------------------------------------
; * 函数原型：void rt_hw_context_switch_to(rt_uint32_t to)
; * r0->to
; * 该函数用于开启第一次的线程切换
; * 设置rt_interrupt_to_thread为第一个线程的栈指针,然后设置PendSV异常,发起PendSV异常
; */
rt_hw_context_switch_to		PROC		;定义子程序,与ENDP成对使用,

	;EXPORT,声明一个标号具有全局属性,可以被外部文件使用
	EXPORT rt_hw_context_switch_to
	
	;设置rt_interrupt_to_thread = to,设置下一个要运行线程的栈的SP的值
	;LDR,从存储器中加载一个字(32bit)到寄存器
	;将rt_interrupt_to_thread的地址加载到r1
	LDR		r1,	=rt_interrupt_to_thread
	;STR,将寄存器中的值按字存储到存储器中
	;将r0寄存器中的内容即to,按(32bit)存储到r1寄存器内容为地址中,即rt_interrupt_to_thread = to
	STR		r0, [r1]
	
	;设置rt_interrupt_from_thread=0表示启动第一次线程切换
	;将rt_interrupt_from_thread地址加载到r1寄存器
	LDR 	r1, =rt_interrupt_from_thread
	;将立即数0加载到r0寄存器
	MOV		r0, #0x0
	;将r0寄存器的内容加载到r1内容为地址中,即rt_interrupt_from_thread = 0
	STR		r0, [r1]
	
	
	;设置rt_thread_switch_interrupt_flag = 1,PendVS中断服务标志设置为1,当执行到PendSV中断服务函数时,rt_thread_switch_interrupt_flag = 0
	;将rt_thread_switch_interrupt_flag的地址加载到r1寄存器
	LDR		r1, =rt_thread_switch_interrupt_flag	
	;把立即数1加载到r0寄存器
	MOV		r0, #1	
	;将r0寄存器中的内容1,按(32bit)存储到r1寄存器内容为地址中,即rt_thread_switch_interrupt_flag
	STR		r0, [r1]
	
	;设置PendSV异常的优先级
	;下面操作是把寄存器*0xE000ED20 |= 0x00FF0000
	;将立即数0xE000ED20,加载到寄存器r0
	LDR		r0,	=NVIC_SYSPRI2
	;将立即数0x00FF0000,加载到寄存器r1
	LDR		r1, =NVIC_PENDSV_PRI
	;将r0内容+0地址里面的内容加载到r2寄存器,即r2 = *(0xE000ED20)
	LDR.W 	r2,	[r0,#0x00]
	;将r1和r2寄存器里面的内容按位或,保存的r1寄存器中
	ORR		r1, r1,r2
	;将r1寄存器的内容写入到r0寄存器内容地址中即 *(0xE000ED20) = r1
	STR		r1, [r0]
	
	;触发PendSV异常(产生上下文切换)
	;将立即数0xE000ED04,加载到r0寄存器
	LDR		r0, =NVIC_INT_CTRL
	;将立即数0x10000000,加载到r1寄存器
	LDR		r1, =NVIC_PENDSVSET
	;将r1寄存器内容写入到r0内容地址中,即0xE000ED04 = 0x10000000
	STR		r1, [r0]
	
	;开中断
	CPSIE	F
	CPSIE	I
	
	;永远不会执行到这里
	ENDP
		
		
		
		
;/*
; *-----------------------------------------------------------------------------
; * 函数原型：void PendSV_Handler(void);
; * psr,pc,lr,r12,r3,r2,r1,r0 are pushed into [from] stack
; * PendSV中断服务函数,根据全局变量rt_interrupt_to_thread,rt_interrupt_from_thread
; * 完成上下文切换
; */		
PendSV_Handler PROC			;定义子程序,与ENDP成对使用,	
	EXPORT PendSV_Handler	;EXPORT,声明一个标号具有全局属性,可以被外部文件使用	
	
	;MRS加载特殊功能寄存器的值到通用寄存器
	MRS		r2, PRIMASK
	CPSID	I				;关闭中断	
		
	;加载rt_thread_switch_interrupt_flag变量的内容到r1
	;判断是否为0,退出中断服务函数,不切换线程
	LDR		r0, =rt_thread_switch_interrupt_flag
	LDR		r1, [r0]
	;比较结果为0,则跳转
	CBZ		r1, pendsv_exit
	
	;rt_thread_switch_interrupt_flag=1,则清零
	MOV		r1, #0x00
	STR		r1, [r0]
	
	;加载rt_interrupt_from_thread变量的内容到r1
	;判断是否等于0
	LDR		r0, =rt_interrupt_from_thread
	LDR 	r1, [r0]
	;rt_interrupt_from_thread==0跳转到switch_to_thread执行,表明启动第一个线程,只需切换下午即可
	CBZ		r1, switch_to_thread
	
	;==============保护上文=========
	;
	;将psp寄存器的内容加载到r1寄存器
	MRS		r1, psp
	;将CPU寄存器r4-r11的值存储到r1指向的地址(每操作一次地址递减一次)
	STMFD	r1!,{r4 - r11}
	;将rt_interrupt_from_thread中的上一个线程的栈的SP加载到r0寄存器
	LDR		r0, [r0]
	;将更新后的栈地址写入到SP中,struct rt_thread thread.sp中
	STR		r1, [r0]
	
	;==============切换下文=========
switch_to_thread
	
	;rt_interrupt_to_thread = next_thread.sp;
	;加载rt_interrupt_to_thread地址到r1寄存器
	LDR		r1, =rt_interrupt_to_thread
	;加载rt_interrupt_to_thread内容到r1寄存器,即next_thread.sp的地址
	LDR 	r1, [r1]
	;加载next_thread.sp中的内容到r1寄存器
	LDR		r1, [r1]
	;将r1指向的地址中的内容加载到r4-r11寄存器中,(每操作一次地址递增一次)
	LDMFD	r1, {r4 - r11}
	;MSR加载通用寄存器的值到特殊功能寄存器,加载下一次线程的的栈地址到psp栈寄存器
	MSR		psp, r1
	
pendsv_exit
	; 恢复中断
	MSR		PRIMASK, r2
	;确保退出异常服务函数,使用的栈指针是psp,即LR寄存器的位2要为1
	ORR 	lr, lr, #0x04
	
	;异常返回,这个时候下一个线程栈中的内容自动加载到CPU寄存器
	;xPSR,PC(线程入口地址),R14,R12,R3,R2,R1,R0(线程的形参)
	;同时psp的值也更新,即指向线程栈的栈顶
	BX 		lr
	
	;PendSV_Handler子程序结束
	ENDP
		
		
		
;/*
; *-----------------------------------------------------------------------------
; * 函数原型：void rt_hw_context_switch(rt_uint32_t from,rt_uint32_t to);
; * r0->switch form thread stack
; * r1->switch to thread stack
; * 根据入参更新全局变量rt_interrupt_from_thread,rt_interrupt_to_thread的值
; */		
rt_hw_context_switch	PROC
	EXPORT rt_hw_context_switch
	
	;加载rt_thread_switch_interrupt_flag的值到r3寄存器,
	;判断是否==1,==1跳转到_reswitch执行,==0,赋值为1
	LDR		r2, =rt_thread_switch_interrupt_flag	
	LDR		r3, [r2]
	;判断r3寄存器内容是否为1,=1执行BEQ指令,否则不执行
	CMP		r3, #1
	BEQ		_reswitch
	;将rt_thread_switch_interrupt_flag设置为1
	MOV		r3, #1
	STR		r3, [r2]
	
	;rt_interrupt_from_thread = from_thread->sp,上一个线程的栈地址
	;更新全局变量rt_interrupt_from_thread的值为r0寄存器,即入参from
	;即rt_interrupt_from_thread = from_thread->sp;
	LDR		r2, =rt_interrupt_from_thread
	;将r0寄存器的内容写入到变量
	STR		r0, [r2]

_reswitch
	;rt_interrupt_to_thread = to_thread->sp,上一个线程的栈地址
	;更新全局变量rt_interrupt_to_thread的值为r1寄存器,即入参to
	;即rt_interrupt_to_thread = to_thread->sp;
	LDR		r2, =rt_interrupt_to_thread
	STR		r1, [r2]

	;触发PendSV异常,实现上下文切换
	LDR		r0, =NVIC_INT_CTRL
	LDR		r1, =NVIC_PENDSVSET
	STR		r1, [r0]

	;子程序返回,函数返回
	BX		LR
	;rt_hw_context_switch子程序结束
	ENDP	
		
		
		
;/*
; *-----------------------------------------------------------------------------
; * 函数原型：rt_base_t rt_hw_interrupt_disable(void);
; * 
; * 关中断
; */		
rt_hw_interrupt_disable		PROC
	EXPORT	rt_hw_interrupt_disable
	
	MRS	r0,	PRIMASK	;1bit,1时只响应NMI,其他异常都不响应
	CPSID	I		;关中断
	;子程序返回
	BX	LR
	;函数返回值保存在r0中
	;rt_hw_interrupt_disable子程序结束
	ENDP
	
	
;/*
; *-----------------------------------------------------------------------------
; * 函数原型：void rt_hw_interrupt_enable(rt_base_t level);
; * 
; * 开中断
; */			
rt_hw_interrupt_enable		PROC
	EXPORT	rt_hw_interrupt_enable
	
	MSR	PRIMASK, r0	;将r0寄存器值加载到PRIMASK寄存器
	BX	LR			;子程序返回
	
	ENDP			;子程序结束
		
		
		
		
		
		
		
		
		
	ALIGN	4		;当前文件代码要求4字节对齐
		
	END				;汇编文件结束,每个汇编文件都需要一个END
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
