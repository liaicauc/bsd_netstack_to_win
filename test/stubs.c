
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

int
arpresolve(
register struct arpcom *ac,
register struct rtentry *rt,
struct mbuf *m,
register struct sockaddr *dst,
register unsigned char *desten)
{
	return 1;
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


