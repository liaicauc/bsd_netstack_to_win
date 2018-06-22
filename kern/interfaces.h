#pragma once

#define bzero(src, n) memset((src), 0, (n))
#define bcopy(src, dst, n) memcpy((dst), (src), (n))
#define ovbcopy bcopy
#define bcmp(s1, s2, n) memcmp((s1), (s2), (n))

#define suser(para1, para2) 0

extern unsigned int max(unsigned int, unsigned int);
extern unsigned int min(unsigned int, unsigned int);

extern void log(int level, char *msg);
extern void panic(char *msg);

