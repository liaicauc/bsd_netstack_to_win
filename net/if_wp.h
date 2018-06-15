#ifndef IF_WP_H
#define IF_WP_H

#define NWP 1;

struct wp_softc
{
    struct	arpcom sc_ac;
#define	sc_if	sc_ac.ac_if	/* network-visible interface */
#define	sc_addr	sc_ac.ac_enaddr	/* hardware Ethernet address */
    unsigned int reserved;
};

#endif
