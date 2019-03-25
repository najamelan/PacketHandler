#ifndef IPPACKET_H
#define IPPACKET_H

#include <cassert>
#include <global.h>

#include <IPfragment.h>

class IPdatagram : public IPfragment
{
	const uint	MAX_TCP_SIZE;	
			byte* buffer;			// start of TCP header
			uint	bufferSize;
			bool	deleteBuffer;

  public:
   const uint  uniqueID;
	
				IPdatagram( uint id, const byte* const pkt_data );
				IPdatagram( const IPdatagram& src					);
	virtual ~IPdatagram();

	const byte* const getBuffer		()										 const { assert( buffer ); return buffer; }	;
			bool			processFragment( const IPfragment& fragment )															;
};


#endif // IPPACKET_H