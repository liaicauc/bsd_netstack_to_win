#include "windows.h"
#include "libkern\event.h"

HANDLE ghThreads[2];

void
wait()
{
    printf("\nwait event;");
    wait_event(WPRUN);
    printf("\nwait end;");
}

void
set()
{
    printf("\nset event, Sleep(3000);");
    Sleep(3000);
    set_event(WPRUN);
    printf("\nset end;");
}

void 
ut_event()
{
    DWORD dwWaitResult;

    printf("\n thread main begin");
    ghThreads[0] = start_thread(wait);
    printf("\nstart_thread(wait) end");

    printf("\nstart_thread(set) begin");
    ghThreads[1] = start_thread(set);
    printf("\nstart_thread(set); end");

    printf("\nmain() wait for all complete");
    dwWaitResult = WaitForMultipleObjects(
        2,   // number of handles in array
        ghThreads,     // array of thread handles
        TRUE,          // wait until all are signaled
        INFINITE);
    printf("\nmain() WaitForMultipleObjects() return");
}
