#include "rtdef.h"

/* rt_strncpy
 * �ַ���������
 * dst:		Ҫ�������ĵ�ַ
 * src:		Դ�ַ�
 * n:			Ҫ�������ֽ���
 * return:Ŀ�ĵ�ַ
 */
char *rt_strncpy(char *dst,const char *src,rt_ubase_t n)
{
	if(n!=0)
	{
		char *d = dst;
		const char *s = src;
		do{
			if((*d++ = *s++) == 0)/*����ʧ��*/
			{
				while(--n != 0)/*�������Ϊ0*/
					*d++ = 0;
				break;
			}
		}while(--n != 0);
	}
	return (dst);
}
	

