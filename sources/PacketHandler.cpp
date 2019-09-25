#include <boost/bind.hpp>

#include <IPfragment.h>
#include <IPlayer.h>

#include <PacketHandler.h>
#include <pcap.h>

#include <log.h>

boost::signal< void ( const json_spirit::mValue& ) > PacketHandler::emitJSON;

pcap_t* PacketHandler::adhandle = 0;

uint PacketHandler::listen()
{
	pcap_if_t*   alldevs;
	pcap_if_t*   d;
	int          inum;
	int          i = 0;
	char         errbuf[PCAP_ERRBUF_SIZE];

	// Retrieve the device list from the local machine
	if ( pcap_findalldevs_ex( PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf ) == -1 )
	{
		fprintf( stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf );
		exit( 1 );
	}
/*
	// Print the list
	for( d = alldevs; d != NULL; d = d->next )
	{
		printf( "%d. %s", ++i, d->name );
		if ( d->description )
			printf( " (%s)\n", d->description );
		else
			printf( " (No description available)\n" );
	}

	if ( i == 0 )
	{
		printf( "\nNo interfaces found! Make sure WinPcap is installed.\n" );
		exit( 1 );
	}

	printf( "Enter the interface number (1-%d):", i );
	scanf_s( "%d", &inum );

	if ( inum < 1 || inum > i )
	{
		printf( "\nInterface number out of range.\n" );
		// Free the device list
		pcap_freealldevs( alldevs );
		exit( 1 );
	}*/

	// hardcode my network card for now
	inum = 1;

	// Jump to the selected adapter
	for( d = alldevs, i = 0; i < inum - 1; d = d->next, i++ )
		;

	// Open the device
	if
	( ( adhandle = pcap_open
		(
			d->name,       // name of the device
			65536,         // portion of the packet to capture
								// 65536 guarantees that the whole packet will be captured on all the link layers
			0,             // not in promiscuous mode
			1000,          // read timeout
			NULL,          // authentication on the remote machine
			errbuf          error buffer
		)
		) == NULL
	)
	{
		fprintf( stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name );

		// Free the device list
		pcap_freealldevs( alldevs );
		return -1;
	}

	ULONG netmask;
	struct bpf_program fcode;

	if ( d->addresses != NULL )
		// Retrieve the mask of the first address of the interface
		netmask = ( ( struct sockaddr_in* ) ( d->addresses->netmask ) )->sin_addr.S_un.S_addr;
	else
		// If the interface is without an address we suppose to be in a C class network
		netmask = 0xffffff;

	// compile the filter
	std::string filter;
	filter.append( "tcp port 80" );// and dst host " ).append( myIp() );

	if ( pcap_compile( adhandle, &fcode, filter.c_str(), 1, netmask ) < 0 )
	{
		fprintf( stderr, "\nUnable to compile the packet filter. Check the syntax.\n" );
		// Free the device list
		pcap_freealldevs( alldevs );
		return -1;
	}

	// set the filter
	if ( pcap_setfilter( adhandle, &fcode ) < 0 )
	{
		fprintf( stderr, "\nError setting the filter.\n" );
		// Free the device list
		pcap_freealldevs( alldevs );
		return -1;
	}

	printf( "\nlistening on %s...\n", d->description );

	// We don't need any more the device list. Free it
	pcap_freealldevs( alldevs );

	// start the capture
	pcap_loop( adhandle, 0, pcapCallback, NULL );

	return 0;
}

void pcapCallback( byte* param, const struct pcap_pkthdr* header, const byte* pkt_data )
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "pcapCallback counter: " << ++packetCounter; }



	int statSize =-1, dropped =-1;
	dropped = pcap_stats_ex( PacketHandler::adhandle, &statSize )->ps_drop;

	if( dropped !=0 )

		{ FILE_LOG( logDEBUG ) << "packets dropped by winpcap: " << dropped; }


	IPfragment fragment( pkt_data );
	IPlayer::processFragment( fragment );
}
