/*
 * Copyright (c) 1982, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)if_ethersubr.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/syslog.h>

#include <machine/cpu.h>

#include <net/if.h>
#include <net/netisr.h>
#include <net/route.h>
#include <net/if_llc.h>
#include <net/if_dl.h>
#include <net/if_types.h>

#ifdef INET
#include <netinet/in.h>
#include <netinet/in_var.h>
#endif
#include <netinet/if_ether.h>

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#ifdef ISO
#include <netiso/argo_debug.h>
#include <netiso/iso.h>
#include <netiso/iso_var.h>
#include <netiso/iso_snpac.h>
#endif

#ifdef LLC
#include <netccitt/dll.h>
#include <netccitt/llc_var.h>
#endif

#if defined(LLC) && defined(CCITT)
extern struct ifqueue pkintrq;
#endif

u_char	etherbroadcastaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
extern	struct ifnet loif;
#define senderr(e) { error = (e); goto bad;}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 * Assumes that ifp is actually pointer to arpcom structure.
 */
int
ether_output(ifp, m0, dst, rt0)
	register struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
	struct rtentry *rt0;
{
	short type;
	int s, error = 0;
 	u_char edst[6];
	register struct mbuf *m = m0;
	register struct rtentry *rt;
	struct mbuf *mcopy = (struct mbuf *)0;
	register struct ether_header *eh;
	int off, len = m->m_pkthdr.len;
	struct arpcom *ac = (struct arpcom *)ifp;

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
		senderr(ENETDOWN);
	ifp->if_lastchange = time;
	if (rt = rt0) {
		if ((rt->rt_flags & RTF_UP) == 0) {
			if (rt0 = rt = rtalloc1(dst, 1))
				rt->rt_refcnt--;
			else 
				senderr(EHOSTUNREACH);
		}
		if (rt->rt_flags & RTF_GATEWAY) {
			if (rt->rt_gwroute == 0)
				goto lookup;
			if (((rt = rt->rt_gwroute)->rt_flags & RTF_UP) == 0) {
				rtfree(rt); rt = rt0;
			lookup: rt->rt_gwroute = rtalloc1(rt->rt_gateway, 1);
				if ((rt = rt->rt_gwroute) == 0)
					senderr(EHOSTUNREACH);
			}
		}
		if (rt->rt_flags & RTF_REJECT)
			if (rt->rt_rmx.rmx_expire == 0 ||
			    time.tv_sec < rt->rt_rmx.rmx_expire)
				senderr(rt == rt0 ? EHOSTDOWN : EHOSTUNREACH);
	}
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		if (!arpresolve(ac, rt, m, dst, edst))
			return (0);	/* if not yet resolved */
		/* If broadcasting on a simplex interface, loopback a copy */
		if ((m->m_flags & M_BCAST) && (ifp->if_flags & IFF_SIMPLEX))
			mcopy = m_copy(m, 0, (int)M_COPYALL);
		//off = m->m_pkthdr.len - m->m_len;
		type = ETHERTYPE_IP;
		break;
#endif

	case AF_UNSPEC:
		eh = (struct ether_header *)dst->sa_data;
 		bcopy((caddr_t)eh->ether_dhost, (caddr_t)edst, sizeof (edst));
		type = eh->ether_type;
		break;

	default:
		printf("%s%d: can't handle af%d\n", ifp->if_name, ifp->if_unit,
			dst->sa_family);
		senderr(EAFNOSUPPORT);
	}
    
    //liai todo open it once loop interface was added
	//if (mcopy)
		//(void) looutput(ifp, mcopy, dst, rt);
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	M_PREPEND(m, sizeof (struct ether_header), M_DONTWAIT);
	if (m == 0)
		senderr(ENOBUFS);
	eh = mtod(m, struct ether_header *);
	type = htons((u_short)type);
	bcopy((caddr_t)&type,(caddr_t)&eh->ether_type,
		sizeof(eh->ether_type));
 	bcopy((caddr_t)edst, (caddr_t)eh->ether_dhost, sizeof (edst));
 	bcopy((caddr_t)ac->ac_enaddr, (caddr_t)eh->ether_shost,
	    sizeof(eh->ether_shost));
	//s = splimp();
	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		//splx(s);
		senderr(ENOBUFS);
	}
	IF_ENQUEUE(&ifp->if_snd, m);
	if ((ifp->if_flags & IFF_OACTIVE) == 0)
		(*ifp->if_start)(ifp);
	//splx(s);
	ifp->if_obytes += len + sizeof (struct ether_header);
	if (m->m_flags & M_MCAST)
		ifp->if_omcasts++;
	return (error);

bad:
	if (m)
		m_freem(m);
	return (error);
}

/*
 * Process a received Ethernet packet;
 * the packet is in the mbuf chain m without
 * the ether header, which is provided separately.
 */
void
ether_input(ifp, eh, m)
	struct ifnet *ifp;
	register struct ether_header *eh;
	struct mbuf *m;
{
	register struct ifqueue *inq;
	register struct llc *l;
	struct arpcom *ac = (struct arpcom *)ifp;
	int s;

	if ((ifp->if_flags & IFF_UP) == 0) {
		m_freem(m);
		return;
	}
	ifp->if_lastchange = time;
	ifp->if_ibytes += m->m_pkthdr.len + sizeof (*eh);
	if (bcmp((caddr_t)etherbroadcastaddr, (caddr_t)eh->ether_dhost,
	    sizeof(etherbroadcastaddr)) == 0)
		m->m_flags |= M_BCAST;
	else if (eh->ether_dhost[0] & 1)
		m->m_flags |= M_MCAST;
	if (m->m_flags & (M_BCAST|M_MCAST))
		ifp->if_imcasts++;

	switch (eh->ether_type) {
	case ETHERTYPE_IP:
        //ipintr();
		//schednetisr(NETISR_IP);
		//inq = &ipintrq;
		break;

	case ETHERTYPE_ARP:
        //liai todo arp_input()
		//schednetisr(NETISR_ARP);
		//inq = &arpintrq;
		break;
	default:
	    m_freem(m);
	    return;
	}

    #if 0
	//s = splimp();
	if (IF_QFULL(inq)) {
		IF_DROP(inq);
		m_freem(m);
	} else
		IF_ENQUEUE(inq, m);
	//splx(s);
    #endif
}

/*
 * Convert Ethernet address to printable (loggable) representation.
 */
static char digits[] = "0123456789abcdef";
char *
ether_sprintf(ap)
	register u_char *ap;
{
	register i;
	static char etherbuf[18];
	register char *cp = etherbuf;

	for (i = 0; i < 6; i++) {
		*cp++ = digits[*ap >> 4];
		*cp++ = digits[*ap++ & 0xf];
		*cp++ = ':';
	}
	*--cp = 0;
	return (etherbuf);
}

/*
 * Perform common duties while attaching to interface list
 */
void
ether_ifattach(ifp)
	register struct ifnet *ifp;
{
	register struct ifaddr *ifa;
	register struct sockaddr_dl *sdl;

	ifp->if_type = IFT_ETHER;
	ifp->if_addrlen = 6;
	ifp->if_hdrlen = 14;
	ifp->if_mtu = ETHERMTU;
	for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
		if ((sdl = (struct sockaddr_dl *)ifa->ifa_addr) &&
		    sdl->sdl_family == AF_LINK) {
			sdl->sdl_type = IFT_ETHER;
			sdl->sdl_alen = ifp->if_addrlen;
			bcopy((caddr_t)((struct arpcom *)ifp)->ac_enaddr,
			      LLADDR(sdl), ifp->if_addrlen);
			break;
		}
}

u_char	ether_ipmulticast_min[6] = { 0x01, 0x00, 0x5e, 0x00, 0x00, 0x00 };
u_char	ether_ipmulticast_max[6] = { 0x01, 0x00, 0x5e, 0x7f, 0xff, 0xff };
/*
 * Add an Ethernet multicast address or range of addresses to the list for a
 * given interface.
 */
int
ether_addmulti(ifr, ac)
	struct ifreq *ifr;
	register struct arpcom *ac;
{
	register struct ether_multi *enm;
	struct sockaddr_in *sin;
	u_char addrlo[6];
	u_char addrhi[6];
	//int s = splimp();

	switch (ifr->ifr_addr.sa_family) {

	case AF_UNSPEC:
		bcopy(ifr->ifr_addr.sa_data, addrlo, 6);
		bcopy(addrlo, addrhi, 6);
		break;

#ifdef INET
	case AF_INET:
		sin = (struct sockaddr_in *)&(ifr->ifr_addr);
		if (sin->sin_addr.s_addr == INADDR_ANY) {
			/*
			 * An IP address of INADDR_ANY means listen to all
			 * of the Ethernet multicast addresses used for IP.
			 * (This is for the sake of IP multicast routers.)
			 */
			bcopy(ether_ipmulticast_min, addrlo, 6);
			bcopy(ether_ipmulticast_max, addrhi, 6);
		}
		else {
			ETHER_MAP_IP_MULTICAST(&sin->sin_addr, addrlo);
			bcopy(addrlo, addrhi, 6);
		}
		break;
#endif

	default:
		//splx(s);
		return (EAFNOSUPPORT);
	}

	/*
	 * Verify that we have valid Ethernet multicast addresses.
	 */
	if ((addrlo[0] & 0x01) != 1 || (addrhi[0] & 0x01) != 1) {
		//splx(s);
		return (EINVAL);
	}
	/*
	 * See if the address range is already in the list.
	 */
	ETHER_LOOKUP_MULTI(addrlo, addrhi, ac, enm);
	if (enm != NULL) {
		/*
		 * Found it; just increment the reference count.
		 */
		++enm->enm_refcount;
		//splx(s);
		return (0);
	}
	/*
	 * New address or range; malloc a new multicast record
	 * and link it into the interface's multicast list.
	 */
	enm = (struct ether_multi *)malloc(sizeof(*enm));
	if (enm == NULL) {
		//splx(s);
		return (ENOBUFS);
	}
	bcopy(addrlo, enm->enm_addrlo, 6);
	bcopy(addrhi, enm->enm_addrhi, 6);
	enm->enm_ac = ac;
	enm->enm_refcount = 1;
	enm->enm_next = ac->ac_multiaddrs;
	ac->ac_multiaddrs = enm;
	ac->ac_multicnt++;
	//splx(s);
	/*
	 * Return ENETRESET to inform the driver that the list has changed
	 * and its reception filter should be adjusted accordingly.
	 */
	return (ENETRESET);
}

/*
 * Delete a multicast address record.
 */
int
ether_delmulti(ifr, ac)
	struct ifreq *ifr;
	register struct arpcom *ac;
{
	register struct ether_multi *enm;
	register struct ether_multi **p;
	struct sockaddr_in *sin;
	u_char addrlo[6];
	u_char addrhi[6];
	//int s = splimp();

	switch (ifr->ifr_addr.sa_family) {

	case AF_UNSPEC:
		bcopy(ifr->ifr_addr.sa_data, addrlo, 6);
		bcopy(addrlo, addrhi, 6);
		break;

#ifdef INET
	case AF_INET:
		sin = (struct sockaddr_in *)&(ifr->ifr_addr);
		if (sin->sin_addr.s_addr == INADDR_ANY) {
			/*
			 * An IP address of INADDR_ANY means stop listening
			 * to the range of Ethernet multicast addresses used
			 * for IP.
			 */
			bcopy(ether_ipmulticast_min, addrlo, 6);
			bcopy(ether_ipmulticast_max, addrhi, 6);
		}
		else {
			ETHER_MAP_IP_MULTICAST(&sin->sin_addr, addrlo);
			bcopy(addrlo, addrhi, 6);
		}
		break;
#endif

	default:
		//splx(s);
		return (EAFNOSUPPORT);
	}

	/*
	 * Look up the address in our list.
	 */
	ETHER_LOOKUP_MULTI(addrlo, addrhi, ac, enm);
	if (enm == NULL) {
		//splx(s);
		return (ENXIO);
	}
	if (--enm->enm_refcount != 0) {
		/*
		 * Still some claims to this record.
		 */
		//splx(s);
		return (0);
	}
	/*
	 * No remaining claims to this record; unlink and free it.
	 */
	for (p = &enm->enm_ac->ac_multiaddrs;
	     *p != enm;
	     p = &(*p)->enm_next)
		continue;
	*p = (*p)->enm_next;
	free((caddr_t)enm);
	ac->ac_multicnt--;
	//splx(s);
	/*
	 * Return ENETRESET to inform the driver that the list has changed
	 * and its reception filter should be adjusted accordingly.
	 */
	return (ENETRESET);
}
