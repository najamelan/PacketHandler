#include <IPlayer.h>
#include <sstream>

std::list<IPdatagram*> IPlayer::datagramList;


void IPlayer::processFragment( IPfragment& fragment )
{
//	static uint packetCounter = 0;
//	{ FILE_LOG( logDEBUG ) << "IPlayer::processFragment counter: " << ++packetCounter; }
//
//	std::stringstream output( std::ios_base::out | std::ios_base::in | std::ios_base::binary );
//	const char* const input = reinterpret_cast<const char* const >( fragment.ipHeader );
//
//	output.write( input, fragment.totalLength() );
//	for( int i = 0;  fragment.totalLength()  < i; ++i  )
//
//		output.put( input[i] );
//
//	{
//		FILE_LOG( logDEBUG ) << "IPlayer::processFragment: { ";
//		FILE_LOG( logDEBUG ).write( input, fragment.totalLength() )  << " }";
//	}


	// filter some traffic. We only want urban rivals
	if(		fragment.srcAddr().byte1 != 78
			|| fragment.srcAddr().byte2 != 109
			|| fragment.srcAddr().byte3 != 87	)

	{
		{ FILE_LOG( logDEBUG ) << "IPlayer::processFragment: dropping packet with IP address: " << fragment.srcAddr().printable(); }
		return;
	}


	for( std::list<IPdatagram*>::iterator it = datagramList.begin(); it != datagramList.end(); ++it )
	{
		// if this fragment belongs to this datagram
		if( (*it)->uniqueID == fragment.getDatagramID() )
		{
			// if this one completes the datagram, all is processed, do cleanup
			if( (*it)->processFragment( fragment ) )
			{
				delete *it;
				datagramList.erase( it ); // must return after this because it is will be invalid if incremented at end of loop
				return;
			}
		}
	}

	// there isn't yet a datagram for this ip packet, make one
	datagramList.push_back( new IPdatagram( fragment.getDatagramID(), reinterpret_cast< const byte* >( fragment.ethernetHeader ) ) );

	// if it is unfragmented, all will be processed, do cleanup
	if( datagramList.back()->processFragment( fragment ) )
	{
		delete datagramList.back();
		datagramList.erase( --datagramList.rbegin().base() );
	}
}

