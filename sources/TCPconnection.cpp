#include <cassert>
#include <iostream>
#include <iomanip>

#include <TCPconnection.h>

#include <boost/bind.hpp>


// TCPstream definitions
//----------------------

const dword TCPstream::MAXONHOLDPACKETS = 30;

TCPstream::TCPstream( bool first ) :
	  _finReceived			( false	)
	, _hasAcknowledgedFin( false	)
	, _finSequence			( 0		)
	, _first					( first	)
	, packetsReceived		( 0		)
	, nextExpectedSeq		( 0		)
{
	httpStream.setTCPstream( this	);
}

TCPstream::~TCPstream()
{
	// clean up all the packets on hold that were never used
	for( std::list<TCPsegment*>::iterator it = onHoldList.begin(); it != onHoldList.end(); ++it )

		delete *it;
}

// takes care of the TCP stream reassembly.
// returns true if the packet was in the right place and new data has successfully been copied into the stream.
// this return value can be used to determine if the application needs to check the newly arrived data.

bool TCPstream::processSegment( const TCPsegment& segment )
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "TCPstream::processSegment counter: " << ++packetCounter; }

//	ConsoleColor kleur = white;
//
//	if( !connection->firstIsSource( segment ) )
//
//		kleur =  red;
//
//	std::cout
//		<< kleur
//		<< (int) srcIP().byte1 << "." << (int) srcIP().byte2 << "."
//		<< (int) srcIP().byte3 << "." << (int) srcIP().byte4 << " -> "
//		<< (int) dstIP().byte1 << "." << (int) dstIP().byte2 << "."
//		<< (int) dstIP().byte3 << "." << (int) dstIP().byte4 << " - "
//		<< std::setw(2) << connection->friendlyID()
//		<< " seq: " << std::setw(10) << segment.sequenceNum() << " "
//		<< " ack: " << std::setw(10) << segment.acknowledgeNum() << " "
//		<< (segment.synFlag() ? "SYN," : "    " )
//		<< (segment.finFlag() ? "FIN," : "    " )
//		<< (segment.rstFlag() ? "RST," : "    " )
//		<< (segment.pshFlag() ? "PSH," : "    " )
//		<< (segment.ackFlag() ? "ACK," : "    " )
//		<< " dataLen: "
//		<< segment.dataLength()
//		<< std::endl << ( kleur == red? white: red );



	// we only follow connections from the start
	if( !packetsReceived && !segment.synFlag() )

		return false;


	// if this is the first packet, update nextExpectedSeq
	if( !packetsReceived )

		nextExpectedSeq = segment.sequenceNum();


	// this one is to early, put a copy of it on hold
	if( segment.sequenceNum() > nextExpectedSeq )
	{
		assert( onHoldList.size() <= MAXONHOLDPACKETS );

		// limit the amount of packets on hold by deleting the oldest one
		if( onHoldList.size() >= MAXONHOLDPACKETS )
		{
			delete onHoldList.front();
			onHoldList.erase( onHoldList.begin() );
		}

		onHoldList.push_back( new TCPsegment( segment ) );
//		std::cout << green << "putting packet on hold, conn: " << connection->friendlyID()
//					 << " seq: " << std::setw(10) << segment.sequenceNum() << " "
//					 << " ack: " << std::setw(10) << segment.acknowledgeNum() << " "
//					 << " dataLen: "
//					 << segment.dataLength()
//					 << white << std:: endl;

		return false;
	}


	// else there might be useful data in the packet
	if( streamSegment( segment ) )

		// check whether there are waiting packets that now hold useful data
		while( tryOnHold() ) ;

	return true;
}

/// Checks if one of the waiting TCPsegment 's is ready to be added to the stream

/**
 * 	Iterate through onHoldList to find a TCPsegment who's sequenceNum matches nextExpectedSeq.
 * 	If the TCPstream#sequenceNum is smaller, the packet is at least partially dated and we will check to see
 * 	if there is new info, if not it is removed.
 * 	If the sequenceNum is bigger, the packet needs to wait longer
 * 	If the sequenceNum equals the nextExpectedSeq, this packet is on time now and needs to be
 * 	processed by streamSegment.
 */
bool TCPstream::tryOnHold()
{
	std::list<TCPsegment*>::iterator it = onHoldList.begin();

	// has given an exception before, mismatch of iterators...
	while( it != onHoldList.end() )
	{
		// needs to wait longer
		if( (*it)->sequenceNum() > nextExpectedSeq )
		{
			++it;
			continue;
		}

		// dated sequence
		if( (*it)->sequenceNum() + (*it)->dataLength() <= nextExpectedSeq )
		{
//		std::cout	<< green << "Deleting dated waiting packet: " << connection->friendlyID()
//						<< ", # waiting: " << onHoldList.size()
//						<< " seq: " << std::setw(10) << (**it).sequenceNum() << " "
//						<< " ack: " << std::setw(10) << (**it).acknowledgeNum() << " "
//						<< " dataLen: "
//						<< (**it).dataLength()
//						<< std::endl;

			delete *it;

			it = onHoldList.erase( it );
			continue;
		}

		// holds useful data
//		std::cout	<< green << "Found waiting packet for connection: " << connection->friendlyID()
//						<< ", # waiting: " << onHoldList.size()
//						<< " seq: " << std::setw(10) << (**it).sequenceNum() << " "
//						<< " ack: " << std::setw(10) << (**it).acknowledgeNum() << " "
//						<< " dataLen: "
//						<< (**it).dataLength()
//						<< std::endl;

		streamSegment( **it );
		return true;
	}

	// if we fell trough the entire for loop, we didn't find a useful packet.
	return false;
}

void TCPstream::setConnection( const TCPconnection* connection	)
{
	this->connection = connection;
}


// this actually copies the data into the _dataStream
bool TCPstream::streamSegment( const TCPsegment& segment )
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "TCPstream::streamSegment counter: " << ++packetCounter; }

	// offset is used for packets with overlapping data. We will only copy the new data
	uint offset		  	= 	nextExpectedSeq		 	- segment.sequenceNum()		;
	uint newDataSize	= 	segment.dataLength() 	- offset							;
	bool newData		= 	newDataSize > 0												;
	nextExpectedSeq 	+= newDataSize														;
	++packetsReceived;

	assert( offset >= 0 );

	// starting connection
	if( segment.synFlag() )
	{
		assert( !segment.dataLength() );
		++nextExpectedSeq;
	}

	// closing connection
	if( segment.finFlag() )
	{
		finReceived( true );
		_finSequence = segment.sequenceNum() + segment.dataLength() + 1; // fin adds 1 to ack and seq numbers
		++nextExpectedSeq;
	}


	for( ; offset<segment.dataLength(); ++offset )
	{
		_dataStream.push_back( segment.data()[ offset ] );
	}

	// parse the http. The packet is usually complete at psh, but not always
	if( segment.pshFlag() || segment.finFlag() )
	{
		std::string str = "";

		std::for_each( _dataStream.begin(), _dataStream.end(), boost::bind( &std::string::push_back, boost::ref( str ), _1 ) );
		{ FILE_LOG( logDEBUG ) << "TCPstream::streamSegment: connID: " << friendlyID() << " { " << str << "}"; }

		// std::cout << green << "size of datastream: " << _dataStream.size() << std::endl;
		httpStream.parse();
	}

	// let the calling function know if any new data was actually found.
	return newData;
}



IPaddress TCPstream::srcIP() const
{
	// if we are first, the we sent first, so the source ip of the packets arriving here
	// have been sent by the destination address of the connectionID
	return _first?		connection->connectionID().dAddress
						:	connection->connectionID().sAddress;
}

IPaddress TCPstream::dstIP() const
{
	return _first?		connection->connectionID().sAddress
						:	connection->connectionID().dAddress;
}



// TCPconnection definitions
//--------------------------

uint TCPconnection::counter = 0;

TCPconnection::TCPconnection( const TCPconnectionID& id )
:
  _closed			( false			)
, _friendlyID		( ++counter		)
, _connectionID	( id				)
,  firstSrcIP		( id.sAddress	)
,  firstPort		( id.sPort		)
,	firstStream		( true			)
,	secondStream	( false			)

{
	firstStream.setConnection	( this );
	secondStream.setConnection	( this );
}





void	TCPconnection::processSegment( const TCPsegment& segment )
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "TCPconnection::processSegment counter: " << ++packetCounter; }

	// check the direction. port is important in case the packet is between two ports on the same machine.
	TCPstream& srcStream = firstIsSource( segment )? firstStream  : secondStream;
	TCPstream& dstStream = firstIsSource( segment )? secondStream : firstStream;

	dstStream.processSegment( segment );

	// check if the connection is closing
	// if the receiving stream has sent a fin flag earlier, see if this is an acknowledgement
	if(		srcStream.finReceived()
			&& segment.ackFlag()
			&& segment.acknowledgeNum() == srcStream.finSequence() )
	{
		srcStream.hasAcknowlegdedFin( true );
	}

	_closed = srcStream.hasAcknowledgedFin() & dstStream.hasAcknowledgedFin();
}

// Let's you know to which stream (direction) a segment will belong
bool TCPconnection::firstIsSource( const TCPsegment& segment ) const
{
	return		segment.connectionID.sAddress == firstSrcIP
				&& segment.connectionID.sPort		== firstPort;
}





