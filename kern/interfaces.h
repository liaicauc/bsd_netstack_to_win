#pragma once

#define bzero(src, n) memset((src), 0, (n))
#define bcopy(src, dst, n) memcpy((dst), (src), (n))

extern void log(int level, char *msg);
extern void panic(char *msg);