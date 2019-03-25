// Empty project additions:
//		Added "AutoIt3.h" include
//		Added "AutoItX3.lib" to the input linker libraries
//
// AutoItX3.dll needs to be in the run path during execution

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>


#include <pcap.h>

#include <IPfragment.h>
#include <IPlayer.h>
#include <myIp.h>
#include <runTests.h>




/*
typedef struct _IP_HEADER
{
	UCHAR VersionNHeaderLength;		//This byte contains the version number and header length
	UCHAR TypeOfService;			// This byte stores the TOS bits of IP header
	UCHAR TotalLength[2];
	UCHAR Identification[2];
	UCHAR FlagsNFragmentOffset[2];	// This word contains 3 bits of flags and the

	// remaining bits for fragment offset
	UCHAR TTL; // Time To Live Field
	UCHAR ProtocolIdentifier; // eg. 1->ICMP,17->UDP,6->TCP
	UCHAR ChkSum[2]; // IpHeader Checksum
	ULONG ulSourceIP; // Source IP Address
	ULONG ulDestinationIP; // Destination IP Address
}IP_HEADER,*PIP_HEADER;


typedef struct _TCP_HEADER
{
USHORT SourcePortNumber; //(16 bits) The source port number.
USHORT DestinationPortNumber; //(16 bits) The destination port number.
UCHAR SequenceNumber[4]; // The sequence number of the first data octet in this
// segment (except when SYN is present). If SYN is
//present the sequence number is the initial sequence
//number and the first data octet is ISN+1.

UCHAR AcknowledgementNumber[4]; //If the ACK control bit is set this field contains
//the value of the next sequence number the sender
//of the segment is expecting to receive.

UCHAR HeaderLength; // First 4 bytes of octet
//This indicates where the data begins.
UCHAR Flags; //Last six bytes of octet are flags
UCHAR WindowSize[2]; //The number of data octets beginning with the one
//indicated in the acknowledgment field which the
//sender of this segment is willing to accept
UCHAR TCPChecksum[2]; // (16 bits) Checksum

UCHAR UrgentPointer[2]; //Valid only if URG flag is set

}TCP_HEADER,*PTCP_HEADER;


typedef struct _UDP_HEADER
{
USHORT SourcePortNumber; // Source Port Number
USHORT DestinationPortNumber; // Destination Port Number
UCHAR UDPLength[2]; // Length of Header plus data
UCHAR UDPChecksum[2]; // CheckSum of Header plus data


}UDP_HEADER,*PUDP_HEADER;

*/

/* prototype of the packet handler */
void packet_handler(byte *param, const struct pcap_pkthdr *header, const byte *pkt_data);



int main()
{
	runTests();

	myIp();
    pcap_if_t *alldevs;
    pcap_if_t *d;
	pcap_t *adhandle;
	int inum;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    
    // Retrieve the device list from the local machine 
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
        exit(1);
    }
    
    // Print the list
    for(d= alldevs; d != NULL; d= d->next)
    {
        printf("%d. %s", ++i, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }
    
    if (i == 0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        exit(1);
    }

	printf("Enter the interface number (1-%d):",i);
    scanf_s("%d", &inum);
    
    if(inum < 1 || inum > i)
    {
        printf("\nInterface number out of range.\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        exit(1);
    }

    // Jump to the selected adapter
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
    

	
	// Open the device
    if ( (adhandle= pcap_open(d->name,          // name of the device
                              65536,            // portion of the packet to capture
                                                // 65536 guarantees that the whole packet will be captured on all the link layers
                              0,    // not in promiscuous mode
                              1000,             // read timeout
                              NULL,             // authentication on the remote machine
                              errbuf            // error buffer
                              ) ) == NULL)
    {
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }

	ULONG netmask;
	struct bpf_program fcode;

	if (d->addresses != NULL)
        // Retrieve the mask of the first address of the interface
        netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        // If the interface is without an address we suppose to be in a C class network
        netmask=0xffffff; 


	// compile the filter
	std::string filter;
	filter.append( "tcp port 80" );// and dst host " ).append( myIp() );

	if( pcap_compile( adhandle, &fcode, filter.c_str(), 1, netmask ) < 0 )
    {
        fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
        // Free the device list 
        pcap_freealldevs(alldevs);
        return -1;
    }
    
	// set the filter
    if ( pcap_setfilter( adhandle, &fcode ) < 0 )
    {
        fprintf(stderr,"\nError setting the filter.\n");
        // Free the device list
        pcap_freealldevs(alldevs);
        return -1;
    }
    
    printf("\nlistening on %s...\n", d->description);


    // We don't need any more the device list. Free it
    pcap_freealldevs( alldevs );

	// start the capture 
    pcap_loop( adhandle, 0, packet_handler, NULL );


	system ("pause");
	return 0;
}

// Callback function invoked by libpcap for every incoming packet 
void packet_handler( byte* unused, const struct pcap_pkthdr* header, const byte* pkt_data)
{

		IPfragment fragment( pkt_data );

	// print ip addresses
	/*printf("%d.%d.%d.%d -> %d.%d.%d.%d - ",
		  fragment.srcAddr().byte1,
        fragment.srcAddr().byte2,
        fragment.srcAddr().byte3,
        fragment.srcAddr().byte4,
        fragment.destAddr().byte1,
        fragment.destAddr().byte2,
        fragment.destAddr().byte3,
        fragment.destAddr().byte4);
		  */

	IPlayer::processFragment( fragment );

	//std::cout << "id: " << fragment.identification() << " and datagramID: " << ( fragment.getDatagramID() ) << std::endl;
	//std::cout << "---- MF: " << fragment.moreFragFlag() << " - offset: " <<  fragment.fragOffset() << std::endl;
	/*
    struct tm	ltime;
    char		timestr[16];
    ip_header*	ih;
    time_t		local_tv_sec;

    // convert the timestamp to readable format
    local_tv_sec = header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);

    // print timestamp and length of the packet
    printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

    // retrieve the position of the ip header
    ih = (ip_header *) (pkt_data + 14); //length of ethernet header

    // print ip addresses and udp ports
    printf("%d.%d.%d.%d -> %d.%d.%d.%d\n",
        ih->saddr.byte1,
        ih->saddr.byte2,
        ih->saddr.byte3,
        ih->saddr.byte4,
        ih->daddr.byte1,
        ih->daddr.byte2,
        ih->daddr.byte3,
        ih->daddr.byte4);*/
}


/*

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	
	

 	// You can now call AutoIt commands, e.g. to send the keystrokes "hello"
	au3_sleep(1000);
	au3_run( l"notepad.exe", l"", 1);
	au3_winwaitactive( l"untitled -", l"", 0);
	au3_send( l"hello{!}", 0);

	// Get the text in the status bar
	//char szText[1000];
	//AU3_StatusbarGetText("Untitled -", "", 2, szText, 1000);
	//MessageBox(NULL, szText, "Text:", MB_OK);

	return 0;
}
*/