#include <exception>
#include <iostream>

#include <JSONlayer.h>
#include <PacketHandler.h>

std::deque<mValue*> JSONlayer::valueList;

void JSONlayer::processString( std::string& contentString )
{
	{ FILE_LOG( logDEBUG ) << "JSONlayer::processString: " << "{ " << contentString << " }"; }

	mValue value;

//   if( !json_spirit::read( contentString, *value ) )
//   {
//      std::cout   << "we have a json problem with:" << std::endl
//                  << contentString << std::endl;
//
//      //      return;
//   }

	// we need to remove the word output because json_spirit doesn't take a top level name
	if( contentString.find( "output=" ) != std::string::npos ) contentString.erase( 0, 7 );

	try
	{
		json_spirit::read_or_throw( contentString, value );
	}
	catch ( const json_spirit::Error_position& e )
	{
		std::cout   << "we have a json problem with:" << std::endl << std::endl
						<< contentString.insert( e.column_, "<-ERROR->" ) << std::endl << std::endl
						<< "reason: " << e.reason_ << std::endl << std::endl;
	}

	{ FILE_LOG( logDEBUG ) << "JSONlayer::processString: " << json_spirit::write_formatted( value ); }


	PacketHandler::emitJSON( value );

	// TODO: if we are never going to use the list, clean it up
	// is no longer a pointer -> valueList.push_back( value );
}
