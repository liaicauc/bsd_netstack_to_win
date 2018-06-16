#include "pcap.h"

extern char lh_mac_address[6];
extern char lh_ip_address[4];
extern char wpintf_name[];

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
static pcap_t *adhandle = NULL;

int wpcreate()
{
    char errbuf[PCAP_ERRBUF_SIZE];
  
    if ( (adhandle= pcap_open(wpintf_name,          // name of the device
                              65536,            // portion of the packet to capture
                                                // 65536 guarantees that the whole packet will be captured on all the link layers
                              PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
    {
        adhandle = NULL;
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", wpintf_name);
        return -1;
    }
	//liai todo start the loop later =
    //pcap_loop(adhandle, 0, packet_handler, NULL);
    return 0;
}

//liai todo this is a stub, implement ip_input logistics
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
    struct tm ltime;
    char timestr[16];
    time_t local_tv_sec;

    (VOID)(param);
    (VOID)(pkt_data);

    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);
    
    printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len);
    
}

wpout(char *packet, unsigned int len)
{
    if (pcap_sendpacket(adhandle, packet, len) != 0)
    {
        fprintf(stderr,"\nError sending the packet: %s\n", pcap_geterr(adhandle));
        return;
    }
}