#include <windows.h>

unsigned long _iptime()
{
    SYSTEMTIME st;
    unsigned long t, oldt;
    
    GetSystemTime(&st);
	t = st.wHour * 60;
    t = (t +  st.wMinute) * 60;
    t = (t +  st.wSecond) * 1000;
    t += st.wMilliseconds;

    return t;
}


