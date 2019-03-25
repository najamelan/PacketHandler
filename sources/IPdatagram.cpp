#include <cassert>
#include <cstring>
#include <iostream>

#include <IPdatagram.h>
#include <TCPsegment.h>
#include <TCPlayer.h>

bool IPdatagram::processFragment( const IPfragment& fragment )
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "IPdatagram::processFragment counter: " << ++packetCounter; }

	assert( fragment.getDatagramID() == uniqueID );
//	std::cout << "fragment offset: " << std::hex << fragment.fragOffset() << " and MF " << fragment.moreFragFlag() << std::endl;

	// for now we will only implement unfragmented datagrams because we never get fragmented onces, so cannot test an impl.
	assert( !fragment.moreFragFlag() && fragment.fragOffset() == 0 );

//	std::cout << "fragment total length: " << fragment.totalLength() << std::endl;

	// if this is the first fragment create an appropriate buffer
	if( !buffer )
	{
		// If this is an unfragmented datagram, just copy pointer, don't copy data
		if( !fragment.moreFragFlag() && fragment.fragOffset() == 0 )
		{
			buffer		= const_cast<byte*>( fragment.payload );
			bufferSize	= fragment.totalLength() - fragment.headerLength();

			TCPsegment tcpSegment( this );
			TCPlayer::processSegment( tcpSegment );
			return true; // needs to return a bool complete indicating this datagram is complete to allow cleanup
		}

		else
		{
			// this is a fragmented ip packet, not implemented!!!

			buffer		= new byte[MAX_TCP_SIZE];
			bufferSize	= MAX_TCP_SIZE;
			deleteBuffer= true;
			return false;
		}
	}
	return false; // never get here: compiler warning
}


IPdatagram::IPdatagram( uint id, const byte* const pkt_data ) :
	IPfragment	( pkt_data	),
	MAX_TCP_SIZE( 66000		),
	buffer		( 0			),
	bufferSize	( 0			),
	deleteBuffer( false		),
	uniqueID		( id			)
{
}


IPdatagram::IPdatagram( const IPdatagram& src ) :
	IPfragment	( src						),
	MAX_TCP_SIZE( src.MAX_TCP_SIZE	),
	buffer		( 0						),
	bufferSize	( 0						),
	deleteBuffer( false					),
	uniqueID		( src.uniqueID			)
{
	if( src.buffer )
	{
		assert( src.bufferSize );

		buffer		= new byte[src.bufferSize];
		bufferSize	= src.bufferSize;
		deleteBuffer= true;

		memcpy( buffer, src.buffer, src.bufferSize );
	}
}



IPdatagram::~IPdatagram()
{
	if( deleteBuffer )

		delete[] buffer;
}
