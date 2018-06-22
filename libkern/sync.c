#include <windows.h>
#include <stdio.h>
#include <libkern/sync.h>
#include <test/vars.h>

static struct evtmap{
    HANDLE hdl;
    char *name;
}evtmap[MAX_EVENT_NO] = 
    {
        {0, "IFUP"},
    };

#define sethdl(evt, ehdl) (evtmap[evt].hdl = ehdl)
#define gethdl(evt) (evtmap[evt].hdl)
#define getname(evt) (evtmap[evt].name)

void 
init_event(unsigned int evt)
{
    HANDLE ehdl; 
    int i;

    for(i = 0; i < MAX_EVENT_NO; i++)
    {
        ehdl = CreateEvent(NULL, FALSE, FALSE, getname(evt)); 
        if (ehdl == NULL) 
        { 
            printf("CreateEvent failed (%d)\n", GetLastError());
            return;
        }
        sethdl(evt, ehdl);
    }
}

void
set_event(unsigned int evt)
{
    if(!SetEvent(gethdl(evt))) 
    {
        printf("SetEvent failed (%d)\n", GetLastError());
        return;
    }
}

void
reset_event(unsigned int evt)
{
    if(!ResetEvent(gethdl(evt))) 
    {
        printf("ResetEvent failed (%d)\n", GetLastError());
        return;
    }
}


int
wait_event(unsigned int evt)
{
    int ret;
    DWORD dwWaitResult;

    dwWaitResult = WaitForSingleObject(gethdl(evt),INFINITE);
    switch (dwWaitResult) 
    {
        case WAIT_OBJECT_0: 
            printf("Thread %d reading from buffer\n", 
                   GetCurrentThreadId());
            ret = 0;
            break; 

        default: 
            ret = -1;
            printf("Wait error (%d)\n", GetLastError()); 
            return 0; 
    }
    
    if(dbg_swc)
        printf("Thread %d exiting\n", GetCurrentThreadId());
    return ret;
}

CRITICAL_SECTION CriticalSection;

void
init_csect()
{
	if (!InitializeCriticalSectionAndSpinCount(
		&CriticalSection, 0x00000400))
		panic("\ncritical section initilization failed!");
}

void
splimp()
{
	EnterCriticalSection(&CriticalSection);
}

void
splnet()
{
	splimp();
}

void
splx()
{
	LeaveCriticalSection(&CriticalSection);
}

