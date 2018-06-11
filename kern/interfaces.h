#pragma once

#define bzero(src, n) memset((src), 0, (n))
#define bcopy(dst, src, n) memcpy((dst), (src), (n))

extern void log(int level, char *msg);
extern void panic(char *msg);