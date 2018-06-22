
#include <sys/param.h>
#include <sys/systm.h>
//#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <test/vars.h>

void *
pfind(pid)
	register int pid;
{
    return NULL;
}

void
selwakeup(sip)
	register struct selinfo *sip;
{

}


void
rt_ifmsg(ifp)
	register struct ifnet *ifp;
{
    
}

void
rt_missmsg(type, rtinfo, flags, error)
	int type, flags, error;
	register struct rt_addrinfo *rtinfo;
{
    ;
}

void
rt_newaddrmsg(cmd, ifa, error, rt)
int cmd, error;
register struct ifaddr *ifa;
register struct rtentry *rt;
{
    
}

void
gsignal(pgid, signum)
	int pgid, signum;
{
    
}

void
psignal(p, signum)
	register void *p;
	register int signum;
{

}
    
