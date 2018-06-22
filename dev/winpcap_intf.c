#include "pcap.h"
#include "libkern\sync.h"
//liai todo find a place for this guy
#define WPI_RUNNING 1

extern char lh_mac_address[6];
extern char lh_ip_address[4];
extern char wpintf_name[];

static pcap_t *adhandle = NULL;

extern int dbg_swc;
void showpkt(
    const struct pcap_pkthdr *header, 
    const u_char *pkt_data
    )
{
#define HEX_LEN 3
#define MAX_FRAME_LEN 1522
#define DBGBUF_LEN (MAX_FRAME_LEN * HEX_LEN + 2)
static unsigned int pktno = 0;

	static char debug_buf[DBGBUF_LEN];
    struct tm ltime;
    char timestr[16];
    time_t local_tv_sec;
    int i = 0, n = 0;
    
    if(dbg_swc == 0)
        return;
    
    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime( timestr, sizeof timestr, "\n%H:%M:%S", &ltime);
    printf("\npkt[%d] %s,%.6d len:%d", 
        pktno++, 
        timestr, 
        header->ts.tv_usec, 
        header->len
        );
    printf("\ncaplen = 0x%x, len = 0x%x", header->caplen, header->len);

    for(i = 0; i < header->len; i++) 
        n += sprintf_s(debug_buf + n, 4, "%02x ", pkt_data[i]);
    debug_buf[n] = 0;
    /* the magic number 0xeafa is used for observe memory */
	debug_buf[n+1] = 0xea;
    debug_buf[n+2] = 0xfa;
    printf("\npkt = \n%s", debug_buf);
}

void
wpintr(
	u_char *param,
	const struct pcap_pkthdr *header,
	const u_char *pkt_data
	)
{
	if (wpsate() & WPI_RUNNING) {
        if(dbg_swc)
		    showpkt(header, pkt_data);
		wpread(pkt_data, header->len);
		if (header->caplen != header->len)
			log("\nheader->caplen != header->len");
	}
	else{
		wait_event(WPRUN);
   }
}

int start_recv()
{
    char errbuf[PCAP_ERRBUF_SIZE];
  
    if((adhandle = pcap_open(
                wpintf_name,
                65536, 
                PCAP_OPENFLAG_PROMISCUOUS, 
                1000, NULL,
                errbuf)) 
                    == NULL)
    {
        adhandle = NULL;
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", wpintf_name);
        return -1;
    }
    
    pcap_loop(adhandle, 0, wpintr, NULL);
    return 0;
}

unsigned int wpstate_reg = 0;
void
wpup()
{
    wpstate_reg |= WPI_RUNNING;
    set_event(WPRUN);
}

void
wpdown()
{
    wpstate_reg &= ~WPI_RUNNING;
    reset_event(WPRUN);
}

unsigned
wpsate()
{
    return wpstate_reg;
}

void
wpout(
    char *packet, 
    unsigned int len
    )
{
    if (pcap_sendpacket(adhandle, packet, len) != 0)
    {
        fprintf(stderr,"\nError sending the packet: %s\n", 
            pcap_geterr(adhandle));
        return;
    }
}

