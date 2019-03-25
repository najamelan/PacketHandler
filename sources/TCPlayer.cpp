#include <iostream>

#include <TCPlayer.h>


std::list<TCPconnection*> TCPlayer::connectionList;

void TCPlayer::processSegment( TCPsegment& segment )
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "TCPlayer::processSegment counter: " << ++packetCounter; }


	// find which connection this segment belongs to
	for( std::list<TCPconnection*>::iterator it = connectionList.begin(); it != connectionList.end(); ++it )

		// found the connection
		if( (*it)->connectionID() == segment.connectionID )
		{
			(*it)->processSegment( segment );

			// if this was the last packet and the connection is closed, tear it down
			if( (*it)->closed() )
			{
				// std::cout << yellow << "Connection " << (*it)->friendlyID() << " closed. Open connections: " << connectionList.size()-1 << white << std::endl;
				delete *it;
				connectionList.erase( it ); // must return after this because it will be invalid at end of loop after increment..
			}

			return;
		}

	// if this is a connection we have not seen, but which is already established
	if( !segment.synFlag() && !START_CAPTURE_IN_MIDDLE )
	{
		{ FILE_LOG( logDEBUG ) << "TCPlayer::processSegment: ignoring new connection without syn flag"; }
// std::cout << "ignoring new connection without syn flag" << std::endl;
		return;
	}

	// we have a new connection need to follow
	connectionList.push_back( new TCPconnection( segment.connectionID ) );
	connectionList.back()->processSegment( segment );

	if( connectionList.back()->closed() )
	{
		delete connectionList.back();
		connectionList.erase( --connectionList.rbegin().base() );
	}
}


