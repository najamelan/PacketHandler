#include <iostream>
#include <cstring>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>


#include <HTTPconnection.h>
#include <TCPconnection.h>
#include <JSONlayer.h>

HTTPstream::HTTPstream() :
	tcpStream( tcpStream )
{}

void
HTTPstream::setTCPstream( TCPstream* tcpStream )
{
	this->tcpStream = tcpStream;
}


void
HTTPstream::parse()
{
	static uint packetCounter = 0;
	{ FILE_LOG( logDEBUG ) << "HTTPstream::parse: connID: " << tcpStream->friendlyID() << " counter: " << ++packetCounter; }

	if( tcpStream->dataStream().empty() )
	{
		{ FILE_LOG( logDEBUG ) << "HTTPstream::parse: " << "tcpStream->dataStream = empty, return"; }

		return;
	}

	std::string str = "";

	// TODO change to boost::FOREACH
	std::for_each( tcpStream->dataStream().begin(), tcpStream->dataStream().end(), boost::bind( &std::string::push_back, boost::ref( str ), _1 ) );
	{ FILE_LOG( logDEBUG ) << "HTTPstream::parse: data to parse: { " << str << " } "; }

	// read bytes from socket into buffer, break on error
	httpParserStatus = httpParser.addBytes( tcpStream->dataStream() );

	if( httpParserStatus != HTTPparser::Done )
	{
		{ FILE_LOG( logDEBUG ) << "HTTPstream::parse: " << "parserstatus is not done, return. status: " << httpParserStatus; }

		return;
	}

	const char*       type  = httpParser.getValue( "content-type" );
	const char*       body  = httpParser.getBody();
	const int         len   = httpParser.getContentLength();
			std::string output;


	// if there is no body or the type is not text/jason
	if( !type || !body || strcmp( type, "text/json; charset=UTF-8" ) )
	{
		{ FILE_LOG( logDEBUG ) << "HTTPstream::parse: " << "if there is no body or the type is not text/jason, return. type: " << (type? type : "null") << ", body: " << (body? body : "null"); }

		// Reset the parser since it's status was HTTPparser::Done!
		// This prevents accidentally feeding the next packet after this one, in which case it would be lost.
		httpParser.reset();

		return;
	}


	// url decode
	// TODO: make less reallocations throughout the whole process
	uint i     = 0;
	byte token = 0;

	while( body[i] )
	{
		// and there are at least 3 bytes in the stream
		// and there is an % at first position
		// and the next two bytes form a hex number

		if(        strlen( &body[i] ) > 2
				&&   body[i] == '%'
				&& ( token = convertHexNum( body[i+1], body[i+2] ) ) !=0 )
		{
			output += token;
			i      += 3;
		}

		else
		{
			output += body[i];
			++i;
		}
	}

	//   let json parse the output


	JSONlayer::processString( output );
	{ FILE_LOG( logDEBUG ) << "HTTPstream::parse: " << "call jsonLayer"; }

//      std::cout
//
//         << white << std::endl << "_______________" <<  (type? type: "") << " clength: " << (len? len: 0) << "________________________________________________________"
//         << std::endl
//
//         << output << std::endl ;
//
//
//
//         std::cout << std::endl
//         << "_______________________________________________________________________" << std::endl;

	// Reset the parser since it's status was HTTPparser::Done!
	// This prevents accidentally feeding the next packet after this one, in which case it would be lost.
	httpParser.reset();
}








