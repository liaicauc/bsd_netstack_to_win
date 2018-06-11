#include "stdio.h"

void log(int level, char *msg)
{
	return;
}

void panic(char *msg)
{
    printf(msg);
	return;
}