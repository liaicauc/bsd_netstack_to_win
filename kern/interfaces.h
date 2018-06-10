#pragma once

#define bzero(src, n) memset((src), 0, (n))
#define bcopy(dst, src, n) memcpy((dst), (src), (n))

static __inline u_int
max(a, b)
	u_int a, b;
{
	return (a > b ? a : b);
}
static __inline u_int
min(a, b)
	u_int a, b;
{
	return (a < b ? a : b);
}

extern void log(int level, char *msg);
extern void panic(char *msg);