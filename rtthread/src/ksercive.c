#include "rtdef.h"

/* rt_strncpy
 * 字符拷贝函数
 * dst:		要拷贝到的地址
 * src:		源字符
 * n:			要拷贝的字节数
 * return:目的地址
 */
char *rt_strncpy(char *dst,const char *src,rt_ubase_t n)
{
	if(n!=0)
	{
		char *d = dst;
		const char *s = src;
		do{
			if((*d++ = *s++) == 0)/*拷贝失败*/
			{
				while(--n != 0)/*后面填充为0*/
					*d++ = 0;
				break;
			}
		}while(--n != 0);
	}
	return (dst);
}
	

