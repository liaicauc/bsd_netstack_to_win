#include <sys/types.h>
#include <kern/interfaces.h>

u_int max(u_int a, u_int b)
{
	return (a < b) ? a : b;
}

u_int min(u_int a, u_int b) 
{
    return (a < b) ? a : b;
}

