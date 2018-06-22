#include "sys/param.h"
#include "sys/types.h"

u_int max(u_int a, u_int b)
{
	return (a < b) ? a : b;
}

u_int min(u_int a, u_int b) 
{
    return (a < b) ? a : b;
}

int imax(int a, int b)
{
	return (a < b) ? a : b;
}

int imin(int a, int b) 
{
    return (a < b) ? a : b;
}

long lmax(long a, long b)
{
	return (a < b) ? a : b;
}

long lmin(long a, long b) 
{
    return (a < b) ? a : b;
}
