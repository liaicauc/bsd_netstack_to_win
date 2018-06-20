#include<windows.h>

#define MAX_THREADS_NO 2
HANDLE ghThreads[MAX_THREADS_NO] = {0};
static int curth = 0;

void
start_thread(void *func)
{
    DWORD   id;
    HANDLE th;

	if ((th = CreateThread(NULL, 0, func, NULL, 0, &id)) == NULL)
		panic("\nCreate thread failed");
    
    ghThreads[curth++] = th;
}

void
scheduler()
{
    WaitForMultipleObjects(2, ghThreads, TRUE, INFINITE);
}

