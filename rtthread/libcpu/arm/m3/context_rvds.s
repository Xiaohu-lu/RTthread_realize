;*****************************************************************************
;								ȫ�ֱ���
;*****************************************************************************
;IMPORT�������������ⲿ�ļ�,extern
	IMPORT rt_thread_switch_interrupt_flag	
	IMPORT rt_interrupt_from_thread
	IMPORT rt_interrupt_to_thread

;*****************************************************************************
;								����
;*****************************************************************************
;-----------------------------------------------------------------------------
;�й��ں�����Ĵ�������ɲο���STM32F10xxx Cortex-M3 programming manual
;ϵͳ��������SCB��ַ��Χ��0xE000ED00-0xE000ED3F
;-----------------------------------------------------------------------------
;EQU---->�����ֳ���ȥһ����,�൱��#define 
SCB_VTOR		EQU			0xE000ED08			;������ƫ�ƼĴ���,
NVIC_INT_CTRL	EQU			0xE000ED04			;�жϿ���״̬�Ĵ���
NVIC_SYSPRI2	EQU			0xE000ED20			;ϵͳ���ȼ��Ĵ���
NVIC_PENDSV_PRI	EQU			0x00FF0000			;PendSV���ȼ�ֵ
NVIC_PENDSVSET	EQU			0x10000000			;����PendSV exception��ֵ

;*****************************************************************************
;								�������ָ��
;*****************************************************************************
	AREA |.text|, CODE, READONLY, ALIGN=2	;AREA���һ�������,||����Ϊ��������ĸ��ʼ(.text)����,CODE��ʾ����,READONLY,��ʾֻ��,ALIGN=2,��ʾ2*2�ֽڶ���
	THUMB									;Thumbָ�
	REQUIRE8								;��ǰ�ļ�ջ��8�ֽڶ���
	PRESERVE8								;��ǰ�ļ�ջ��8�ֽڶ���

;/*
; *-----------------------------------------------------------------------------
; * ����ԭ�ͣ�void rt_hw_context_switch_to(rt_uint32_t to)
; * r0->to
; * �ú������ڿ�����һ�ε��߳��л�
; * ����rt_interrupt_to_threadΪ��һ���̵߳�ջָ��,Ȼ������PendSV�쳣,����PendSV�쳣
; */
rt_hw_context_switch_to		PROC		;�����ӳ���,��ENDP�ɶ�ʹ��,

	;EXPORT,����һ����ž���ȫ������,���Ա��ⲿ�ļ�ʹ��
	EXPORT rt_hw_context_switch_to
	
	;����rt_interrupt_to_thread = to,������һ��Ҫ�����̵߳�ջ��SP��ֵ
	;LDR,�Ӵ洢���м���һ����(32bit)���Ĵ���
	;��rt_interrupt_to_thread�ĵ�ַ���ص�r1
	LDR		r1,	=rt_interrupt_to_thread
	;STR,���Ĵ����е�ֵ���ִ洢���洢����
	;��r0�Ĵ����е����ݼ�to,��(32bit)�洢��r1�Ĵ�������Ϊ��ַ��,��rt_interrupt_to_thread = to
	STR		r0, [r1]
	
	;����rt_interrupt_from_thread=0��ʾ������һ���߳��л�
	;��rt_interrupt_from_thread��ַ���ص�r1�Ĵ���
	LDR 	r1, =rt_interrupt_from_thread
	;��������0���ص�r0�Ĵ���
	MOV		r0, #0x0
	;��r0�Ĵ��������ݼ��ص�r1����Ϊ��ַ��,��rt_interrupt_from_thread = 0
	STR		r0, [r1]
	
	
	;����rt_thread_switch_interrupt_flag = 1,PendVS�жϷ����־����Ϊ1,��ִ�е�PendSV�жϷ�����ʱ,rt_thread_switch_interrupt_flag = 0
	;��rt_thread_switch_interrupt_flag�ĵ�ַ���ص�r1�Ĵ���
	LDR		r1, =rt_thread_switch_interrupt_flag	
	;��������1���ص�r0�Ĵ���
	MOV		r0, #1	
	;��r0�Ĵ����е�����1,��(32bit)�洢��r1�Ĵ�������Ϊ��ַ��,��rt_thread_switch_interrupt_flag
	STR		r0, [r1]
	
	;����PendSV�쳣�����ȼ�
	;��������ǰѼĴ���*0xE000ED20 |= 0x00FF0000
	;��������0xE000ED20,���ص��Ĵ���r0
	LDR		r0,	=NVIC_SYSPRI2
	;��������0x00FF0000,���ص��Ĵ���r1
	LDR		r1, =NVIC_PENDSV_PRI
	;��r0����+0��ַ��������ݼ��ص�r2�Ĵ���,��r2 = *(0xE000ED20)
	LDR.W 	r2,	[r0,#0x00]
	;��r1��r2�Ĵ�����������ݰ�λ��,�����r1�Ĵ�����
	ORR		r1, r1,r2
	;��r1�Ĵ���������д�뵽r0�Ĵ������ݵ�ַ�м� *(0xE000ED20) = r1
	STR		r1, [r0]
	
	;����PendSV�쳣(�����������л�)
	;��������0xE000ED04,���ص�r0�Ĵ���
	LDR		r0, =NVIC_INT_CTRL
	;��������0x10000000,���ص�r1�Ĵ���
	LDR		r1, =NVIC_PENDSVSET
	;��r1�Ĵ�������д�뵽r0���ݵ�ַ��,��0xE000ED04 = 0x10000000
	STR		r1, [r0]
	
	;���ж�
	CPSIE	F
	CPSIE	I
	
	;��Զ����ִ�е�����
	ENDP
		
		
		
		
;/*
; *-----------------------------------------------------------------------------
; * ����ԭ�ͣ�void PendSV_Handler(void);
; * psr,pc,lr,r12,r3,r2,r1,r0 are pushed into [from] stack
; * PendSV�жϷ�����,����ȫ�ֱ���rt_interrupt_to_thread,rt_interrupt_from_thread
; * ����������л�
; */		
PendSV_Handler PROC			;�����ӳ���,��ENDP�ɶ�ʹ��,	
	EXPORT PendSV_Handler	;EXPORT,����һ����ž���ȫ������,���Ա��ⲿ�ļ�ʹ��	
	
	;MRS�������⹦�ܼĴ�����ֵ��ͨ�üĴ���
	MRS		r2, PRIMASK
	CPSID	I				;�ر��ж�	
		
	;����rt_thread_switch_interrupt_flag���������ݵ�r1
	;�ж��Ƿ�Ϊ0,�˳��жϷ�����,���л��߳�
	LDR		r0, =rt_thread_switch_interrupt_flag
	LDR		r1, [r0]
	;�ȽϽ��Ϊ0,����ת
	CBZ		r1, pendsv_exit
	
	;rt_thread_switch_interrupt_flag=1,������
	MOV		r1, #0x00
	STR		r1, [r0]
	
	;����rt_interrupt_from_thread���������ݵ�r1
	;�ж��Ƿ����0
	LDR		r0, =rt_interrupt_from_thread
	LDR 	r1, [r0]
	;rt_interrupt_from_thread==0��ת��switch_to_threadִ��,����������һ���߳�,ֻ���л����缴��
	CBZ		r1, switch_to_thread
	
	;==============��������=========
	;
	;��psp�Ĵ��������ݼ��ص�r1�Ĵ���
	MRS		r1, psp
	;��CPU�Ĵ���r4-r11��ֵ�洢��r1ָ��ĵ�ַ(ÿ����һ�ε�ַ�ݼ�һ��)
	STMFD	r1!,{r4 - r11}
	;��rt_interrupt_from_thread�е���һ���̵߳�ջ��SP���ص�r0�Ĵ���
	LDR		r0, [r0]
	;�����º��ջ��ַд�뵽SP��,struct rt_thread thread.sp��
	STR		r1, [r0]
	
	;==============�л�����=========
switch_to_thread
	
	;rt_interrupt_to_thread = next_thread.sp;
	;����rt_interrupt_to_thread��ַ��r1�Ĵ���
	LDR		r1, =rt_interrupt_to_thread
	;����rt_interrupt_to_thread���ݵ�r1�Ĵ���,��next_thread.sp�ĵ�ַ
	LDR 	r1, [r1]
	;����next_thread.sp�е����ݵ�r1�Ĵ���
	LDR		r1, [r1]
	;��r1ָ��ĵ�ַ�е����ݼ��ص�r4-r11�Ĵ�����,(ÿ����һ�ε�ַ����һ��)
	LDMFD	r1, {r4 - r11}
	;MSR����ͨ�üĴ�����ֵ�����⹦�ܼĴ���,������һ���̵߳ĵ�ջ��ַ��pspջ�Ĵ���
	MSR		psp, r1
	
pendsv_exit
	; �ָ��ж�
	MSR		PRIMASK, r2
	;ȷ���˳��쳣������,ʹ�õ�ջָ����psp,��LR�Ĵ�����λ2ҪΪ1
	ORR 	lr, lr, #0x04
	
	;�쳣����,���ʱ����һ���߳�ջ�е������Զ����ص�CPU�Ĵ���
	;xPSR,PC(�߳���ڵ�ַ),R14,R12,R3,R2,R1,R0(�̵߳��β�)
	;ͬʱpsp��ֵҲ����,��ָ���߳�ջ��ջ��
	BX 		lr
	
	;PendSV_Handler�ӳ������
	ENDP
		
		
		
;/*
; *-----------------------------------------------------------------------------
; * ����ԭ�ͣ�void rt_hw_context_switch(rt_uint32_t from,rt_uint32_t to);
; * r0->switch form thread stack
; * r1->switch to thread stack
; * ������θ���ȫ�ֱ���rt_interrupt_from_thread,rt_interrupt_to_thread��ֵ
; */		
rt_hw_context_switch	PROC
	EXPORT rt_hw_context_switch
	
	;����rt_thread_switch_interrupt_flag��ֵ��r3�Ĵ���,
	;�ж��Ƿ�==1,==1��ת��_reswitchִ��,==0,��ֵΪ1
	LDR		r2, =rt_thread_switch_interrupt_flag	
	LDR		r3, [r2]
	;�ж�r3�Ĵ��������Ƿ�Ϊ1,=1ִ��BEQָ��,����ִ��
	CMP		r3, #1
	BEQ		_reswitch
	;��rt_thread_switch_interrupt_flag����Ϊ1
	MOV		r3, #1
	STR		r3, [r2]
	
	;rt_interrupt_from_thread = from_thread->sp,��һ���̵߳�ջ��ַ
	;����ȫ�ֱ���rt_interrupt_from_thread��ֵΪr0�Ĵ���,�����from
	;��rt_interrupt_from_thread = from_thread->sp;
	LDR		r2, =rt_interrupt_from_thread
	;��r0�Ĵ���������д�뵽����
	STR		r0, [r2]

_reswitch
	;rt_interrupt_to_thread = to_thread->sp,��һ���̵߳�ջ��ַ
	;����ȫ�ֱ���rt_interrupt_to_thread��ֵΪr1�Ĵ���,�����to
	;��rt_interrupt_to_thread = to_thread->sp;
	LDR		r2, =rt_interrupt_to_thread
	STR		r1, [r2]

	;����PendSV�쳣,ʵ���������л�
	LDR		r0, =NVIC_INT_CTRL
	LDR		r1, =NVIC_PENDSVSET
	STR		r1, [r0]

	;�ӳ��򷵻�,��������
	BX		LR
	;rt_hw_context_switch�ӳ������
	ENDP	
		
		
		
;/*
; *-----------------------------------------------------------------------------
; * ����ԭ�ͣ�rt_base_t rt_hw_interrupt_disable(void);
; * 
; * ���ж�
; */		
rt_hw_interrupt_disable		PROC
	EXPORT	rt_hw_interrupt_disable
	
	MRS	r0,	PRIMASK	;1bit,1ʱֻ��ӦNMI,�����쳣������Ӧ
	CPSID	I		;���ж�
	;�ӳ��򷵻�
	BX	LR
	;��������ֵ������r0��
	;rt_hw_interrupt_disable�ӳ������
	ENDP
	
	
;/*
; *-----------------------------------------------------------------------------
; * ����ԭ�ͣ�void rt_hw_interrupt_enable(rt_base_t level);
; * 
; * ���ж�
; */			
rt_hw_interrupt_enable		PROC
	EXPORT	rt_hw_interrupt_enable
	
	MSR	PRIMASK, r0	;��r0�Ĵ���ֵ���ص�PRIMASK�Ĵ���
	BX	LR			;�ӳ��򷵻�
	
	ENDP			;�ӳ������
		
		
		
		
		
		
		
		
		
	ALIGN	4		;��ǰ�ļ�����Ҫ��4�ֽڶ���
		
	END				;����ļ�����,ÿ������ļ�����Ҫһ��END
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
