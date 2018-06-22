#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/syslog.h>

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
#include <test/vars.h>

//liai todo this is the my workstation's ip, change it to a proper one
// after this project is done
extern char lh_mac_address[6];
extern char lh_ip_address[4];

extern struct wp_softc wp_softc;
extern struct ifnet *ifnet;

int wpinit()
{
	register struct ifnet *ifp = &wp_softc.sc_if;

	if ((ifp->if_flags & IFF_RUNNING) == 0) {
        //liai todo implement those later
		//s = splimp();
		ifp->if_flags |= IFF_RUNNING;
		//wpreset(unit);
	    //(void) wpstart(ifp);
		//splx(s);
	}
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
    struct wp_softc *wp = &wp_softc;
    
    switch (cmd) {
        case SIOCSIFFLAGS:
            if ((ifp->if_flags & IFF_UP) == 0 &&
                ifp->if_flags & IFF_RUNNING) {
                wpdown();
                ifp->if_flags &= ~IFF_RUNNING;
            } else if (ifp->if_flags & IFF_UP &&
                (ifp->if_flags & IFF_RUNNING) == 0) {
                wpinit();
                wpup();
            }
            break;

        case SIOCSIFADDR:
            ifp->if_flags |= IFF_UP;
            wpup();
            wpinit();   
            ((struct arpcom *)ifp)->ac_ipaddr = IA_SIN(ifa)->sin_addr;
            //liai todo 
            //arpwhohas((struct arpcom *)ifp, &IA_SIN(ifa)->sin_addr);
            //
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
	return (1);
}

void
wpread(char *buf, int len)
{
    register struct wp_softc *wp = &wp_softc;
    register struct ether_header *et;
    struct mbuf *m;
    int flags;

    wp->sc_if.if_ipackets++;
    et = (struct ether_header *)buf;
    et->ether_type = ntohs((u_short)et->ether_type);
    len = len - sizeof(struct ether_header);
    if (len <= 0) {
        if (dbg_swc)
            log(LOG_WARNING,
                "wp: ierror(runt packet): from %s: len=%d\n",
                ether_sprintf(et->ether_shost), len);
        wp->sc_if.if_ierrors++;
        return;
    }
    
    flags = 0;
    if (bcmp((caddr_t)etherbroadcastaddr,
        (caddr_t)et->ether_dhost, sizeof(etherbroadcastaddr)) == 0)
        flags |= M_BCAST;
    if (et->ether_dhost[0] & 1)
        flags |= M_MCAST;

    if((flags & (M_BCAST | M_MCAST)) == 0 &&
        bcmp(et->ether_dhost, wp->sc_addr, 
             sizeof(et->ether_dhost)) != 0)
		return;

    m = m_devget((char *)(et + 1), len, 0, &wp->sc_if, 
        (void (*)())0);
    if (m == 0)
        return;
    m->m_flags |= flags;
    ether_input(&wp->sc_if, et, m);
}


