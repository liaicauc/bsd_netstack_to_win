#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>
#include <net/netisr.h>
#include <net/route.h>

#ifdef INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#endif
#include <net/if_wp.h>

//liai todo this is the my workstation's ip, change it to a proper one
// after this project is done
extern char lh_mac_address[6];
extern char lh_ip_address[4];

extern struct wp_softc wp_softc;
extern struct ifnet *ifnet;

int wpinit()
{
	register struct ifnet *ifp = &wp_softc.sc_if;

    ifp->if_flags |= IFF_RUNNING;
	return 0;
}

#define MINETHSIZE 64
static char sndbuf[1500];
/*
 * Routine to copy from mbuf chain to transmit
 * buffer for Winpcap.
 */
wpput(wpbuf, m)
	register char *wpbuf;
	register struct mbuf *m;
{
	register struct mbuf *mp;
	register int len, tlen = 0;

	for (mp = m; mp; mp = mp->m_next) {
		len = mp->m_len;
		if (len == 0)
			continue;
		tlen += len;
		bcopy(mtod(mp, char *), wpbuf, len);
		wpbuf += len;
	}
	m_freem(m);
	if (tlen < MINETHSIZE) {
		bzero(wpbuf, MINETHSIZE - tlen);
		tlen = MINETHSIZE;
	}
	return(tlen);
}

/*
 * startting transmit a packet
 * get the ongoing packet from Ifnet queue
 , call winpcap sending routine 
 */
int wpstart(ifp)
	struct ifnet *ifp;

{
	register struct wp_softc *wp = &wp_softc;
	register struct mbuf *m;
	int len;

	if ((wp->sc_if.if_flags & IFF_RUNNING) == 0)
		return (0);
    
	IF_DEQUEUE(&wp->sc_if.if_snd, m);
	if (m == 0)
		return (0);
	len = wpput(sndbuf, m);
    wpout(sndbuf, len);
	wp->sc_if.if_flags |= IFF_OACTIVE;
	return (0);
}


int wpreset()
{
	return 0;
}

int wpioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int cmd;
	caddr_t data;
{   
    register struct ifaddr *ifa = (struct ifaddr *)data;
    
    switch (cmd) {
        case SIOCSIFADDR:
            ifp->if_flags |= IFF_UP;

            wpinit(ifp->if_unit);	
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN(ifa)->sin_addr;
            //liai todo 
            //arpwhohas((struct arpcom *)ifp, &IA_SIN(ifa)->sin_addr);
            break;
        default:
            printf("wpioctl(): option not support yet\n");
        }
    return 0;
}


int wpattach()
{
	struct wp_softc *wp = &wp_softc;
	struct ifnet *ifp = &wp->sc_if;
    int i;

   wpcreate();
   
    for(i = 0; i < MAC_ADDRESS_LEN; i++) 
        wp->sc_addr[i] = lh_mac_address[i];  
        
	ifp->if_unit = 1;
	ifp->if_name = "winpcap_intf";
	ifp->if_mtu = ETHERMTU;
	ifp->if_init = wpinit;
	ifp->if_reset = wpreset;
	ifp->if_ioctl = wpioctl;
	ifp->if_output = ether_output;
	ifp->if_start = wpstart;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;

	if_attach(ifp);
    //wpcreate();
	return (1);
}

