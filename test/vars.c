#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>

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

int dbg_swc = 1;
//liai todo those are my PC's address & name
char lh_mac_address[6] = { 0x94, 0xde, 0x80, 0x70, 0x65, 0xdf };
char lh_ip_address[4] = { 0xc0, 0xa8, 0x01, 0x99 };
char wpintf_name[] = "rpcap://\\Device\\NPF_{3546EAA9-22BC-4825-BF81-24547FAD8D92}";

struct wp_softc wp_softc = { 0 };
struct ifnet *wpif = &wp_softc.sc_if;
struct sockaddr_in hcdst = {
	sizeof(struct sockaddr_in),
	AF_INET,
	0,
	0x0101a8c0, //192.168.1.1
	0
};

struct in_ifaddr tifaddr = {0};
