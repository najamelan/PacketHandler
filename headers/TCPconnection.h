#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <iostream>
#include <list>
#include <deque>
#include <string>

#include <boost/lexical_cast.hpp>

#include <global.h>
#include <TCPsegment.h>
#include <HTTPlayer.h>
#include <HTTPconnection.h>

class TCPconnection;
class HTTPstream;

/// a half-duplex part of a TCPconnection, containing the actual data in a stream of bytes
class TCPstream
{
						bool							_finReceived;
						bool							_hasAcknowledgedFin;
						dword							_finSequence;
			 const	bool							_first;
						uint							packetsReceived;
						uint	 						nextExpectedSeq;
	static const	uint							MAXONHOLDPACKETS;
			 const	TCPconnection*				connection;
						std::deque<byte>			_dataStream;
						std::list<TCPsegment*>	onHoldList;
						HTTPstream					httpStream;

  public:
			TCPstream		( bool						first			);
		  ~TCPstream		(												);
	bool	processSegment	( const TCPsegment&		segment		);
	void	setConnection	( const TCPconnection*	connection	);

	std::string 		friendlyID	() const;
	std::deque<byte>& dataStream	() 			{ return _dataStream;			}
	bool  finReceived					() const		{ return _finReceived;			}
	dword finSequence					() const		{ return _finSequence;			}
	bool	hasAcknowledgedFin		() const		{ return _hasAcknowledgedFin; }
	void  finReceived					( bool set ){ _finReceived = set;			}
	void  hasAcknowlegdedFin		( bool set ){ _hasAcknowledgedFin = set;	}


	// helper function for getting your ip as TCPstream
	IPaddress srcIP() const;
	IPaddress dstIP() const;


  private:
	bool	streamSegment	( const TCPsegment& segment );
	bool	tryOnHold		(									 );


};


/// Symbolizes a full-duplex connection
class TCPconnection
{
							bool					_closed;
	static				uint					counter;
							uint					_friendlyID;
				const		TCPconnectionID	_connectionID;
				const		IPaddress			firstSrcIP;
				const		word					firstPort;
							TCPstream			firstStream, secondStream;

  public:
						 TCPconnection	( const TCPconnectionID&	id			);
	void				 processSegment( const TCPsegment&			segment	);
	bool				 firstIsSource	( const TCPsegment&			segment	) const;

	bool				 closed			() const { return _closed;			};
	uint				 friendlyID		() const { return _friendlyID;   };
	TCPconnectionID connectionID	() const { return _connectionID; };
};

inline
std::string TCPstream::friendlyID() const
{
	return boost::lexical_cast< std::string >( connection->friendlyID() ) + ( _first? "A" : "B" );
}
#endif // TCPCONNECTION_H
