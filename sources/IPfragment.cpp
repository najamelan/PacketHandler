#include <cassert>
#include <cstring>
#include <iostream>

#include <IPfragment.h>

IPfragment::IPfragment( const byte* const pkt_data ) :
	dataBuffer      ( 0                                                                                         ),
	ethernetHeader  ( reinterpret_cast< const EthernetHeader* >( pkt_data                                     ) ),
	ipHeader        ( reinterpret_cast< const IPheader*       >( pkt_data + ETHER_HEADER_LEN                  ) ),
	payload         ( reinterpret_cast< const byte*           >( ipHeader ) + ( ipHeader->ver_hl & 0x0f ) *4  )

{
	assert( ethernetHeader->etherType == ETHER_TYPE_IP4 );

	//payload = new byte[ totalLength()-headerLength() ];
	//memcpy( payload, ipHeader+headerLength(), totalLength()-headerLength() );
}


IPfragment::IPfragment( const IPfragment& src ) :
	dataBuffer      ( new byte[ src.totalLength() + ETHER_HEADER_LEN ]                                          ),
	ethernetHeader  ( reinterpret_cast< const EthernetHeader* >( dataBuffer                                   ) ),
	ipHeader        ( reinterpret_cast< const IPheader*         >( dataBuffer + ETHER_HEADER_LEN              ) ),
	payload         ( reinterpret_cast< const byte*            >( ipHeader ) + ( ipHeader->ver_hl & 0x0f ) *4 )
{
	memcpy( dataBuffer, src.ethernetHeader, src.totalLength() + ETHER_HEADER_LEN );
}



IPfragment::~IPfragment()
{
	if( dataBuffer ) delete[] dataBuffer;
}
