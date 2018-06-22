#include <windows.h>
#include <stdio.h>

unsigned int hz = 1000;
unsigned int tick = 1000;

HANDLE hTimerQueue;

init_timer()
{
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = CreateTimerQueue();

	if (NULL == hTimerQueue)
	{
		printf("CreateTimerQueue failed (%d)\n", GetLastError());
		return;
	}
}

void timeout(void(*ftn) (void *), void *arg, register int ticks)
{
	HANDLE hTimer = NULL;
	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)ftn, arg, ticks, 0, 0))
	{
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
	}
}


