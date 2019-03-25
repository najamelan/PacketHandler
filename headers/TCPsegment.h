#ifndef TCPSEGMENT_H
#define TCPSEGMENT_H

#include <global.h>
#include <IPfragment.h>
#include <IPdatagram.h>


class TCPconnectionID
{
  public:
	const IPaddress	sAddress, dAddress;
	const word			sPort, dPort;

	TCPconnectionID( IPaddress sAddr, IPaddress dAddr, word sPor, word dPor )
		:	sAddress	( sAddr ),	dAddress	( dAddr ),
			sPort		( sPor ),	dPort		( dPor ) {};

	// it is equal if the same two addr, port pairs are present, eg. both directions form one connection
	bool operator==( const TCPconnectionID& rhs ) const
	{
		if(	(	sAddress == rhs.sAddress
				&& dAddress == rhs.dAddress
				&& sPort		== rhs.sPort
				&& dPort		== rhs.dPort	)
				
				||
				
				(	sAddress == rhs.dAddress
				&& dAddress == rhs.sAddress
				&& sPort		== rhs.dPort
				&& dPort		== rhs.sPort	)
			)

			return true;

		else return false;
	};

};




class TCPsegment
{
	// for copy constructor, if this object gets copied, we need to make a copy of the datagram, and especially we need to delete it!
	const bool					isCopy;

	const IPdatagram* const ipDatagram;
	const TCPheader*			tcpHeader;

  public:
	const TCPconnectionID	connectionID;

	TCPsegment( const IPdatagram* const ipDatagram	);
	TCPsegment( const TCPsegment& src					);
  ~TCPsegment(													);

			word			dataOffset			() const	{ return	( tcpHeader->offset_reserved >> 4 )*4;													}; // in bytes
			dword			sequenceNum			() const	{ return	ntohl( tcpHeader->sequence );																};
			dword			acknowledgeNum		() const	{ return	ntohl( tcpHeader->acknowledge );															};
			uint			dataLength			() const { return ipDatagram->totalLength() - ipDatagram->headerLength() - dataOffset();		};
	const byte* const data					() const { return ipDatagram->getBuffer() + dataOffset();												};

	bool	finFlag() const { return ( tcpHeader->th_flags & 0x01 ) != 0;	};
	bool	synFlag() const { return ( tcpHeader->th_flags & 0x02 ) != 0;	};
	bool	rstFlag() const { return ( tcpHeader->th_flags & 0x04 ) != 0;	};
	bool	pshFlag() const { return ( tcpHeader->th_flags & 0x08 ) != 0;	};
	bool	ackFlag() const { return ( tcpHeader->th_flags & 0x10 ) != 0;	};
	bool	urgFlag() const { return ( tcpHeader->th_flags & 0x20 ) != 0;	};
	bool	eceFlag() const { return ( tcpHeader->th_flags & 0x40 ) != 0;	};
	bool	cwrFlag() const { return ( tcpHeader->th_flags & 0x80 ) != 0;	};

};




#endif // TCPSEGMENT_H

