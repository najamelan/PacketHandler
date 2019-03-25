#include <iostream>
#include <TCPsegment.h>

TCPsegment::TCPsegment( const IPdatagram* const ipDatagram ) :
	isCopy		( false																										),
	ipDatagram	( ipDatagram																								),
	tcpHeader	( reinterpret_cast< const TCPheader* const>( ipDatagram->getBuffer() )					),
	connectionID( ipDatagram->srcAddr(), ipDatagram->destAddr(), tcpHeader->sport, tcpHeader->dport	)
{
}


TCPsegment::TCPsegment( const TCPsegment& src ) :
	isCopy		( true																										),
	ipDatagram	( new IPdatagram( *(src.ipDatagram)	)																),
	tcpHeader	( reinterpret_cast< const TCPheader* const>( ipDatagram->getBuffer() )					),
	connectionID( ipDatagram->srcAddr(), ipDatagram->destAddr(), tcpHeader->sport, tcpHeader->dport	)
{
}


TCPsegment::~TCPsegment()
{
	if( isCopy ) delete ipDatagram;
}