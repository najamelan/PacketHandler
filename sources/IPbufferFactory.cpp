#include <cassert>
#include <iostream>

#include "IPbufferFactory.h"


IPbufferFactory* IPbufferFactory::instance = 0;

IPbufferFactory* IPbufferFactory::getInstance()
{
	if( !instance ) instance = new IPbufferFactory();
	return instance;
}

byte* IPbufferFactory::acquireBuffer()
{
	if( availableBufferCounter ) //There are available buffers, find one and return it.
	{
		for( std::list<IPbuffer*>::iterator it = pool->begin(); it != pool->end(); ++it )
		{
			if( (*it)->inUse ) continue;

			(*it)->inUse = true;

			--availableBufferCounter;
			return (*it)->buffer;
		}
	}

	else		//There are no available buffers, so we create a new one.
	{
		IPbuffer* ipBuffer = new IPbuffer();
		ipBuffer->inUse	= true;

		pool->push_back( ipBuffer );
		return ipBuffer->buffer;
	}

	 assert( !"we never get here, but otherwise compiler gives warnings about return value" ); byte* noCompilerWarning = 0;
	 return noCompilerWarning;
}


void IPbufferFactory::releaseBuffer( byte* buffer )
{
	assert( buffer );

	for( std::list<IPbuffer*>::iterator it = pool->begin(); it != pool->end(); ++it )
	{
		if( (*it)->buffer != buffer ) continue;

		if( availableBufferCounter > MINPOOLSIZE )
		{
			delete *it;													// Delete the ipBuffer

			//it-- otherwise the iterater skips one, yucc!
			pool->erase( it-- ); 									// Remove the pointer from the list.
		}

		else
		{
			(*it)->inUse = false;
			pool->splice( pool->begin(), *pool, it );			//move the liberated buffer to the front of the list.
			++availableBufferCounter;
		}

		break;
	}
}


IPbufferFactory::IPbufferFactory() : availableBufferCounter( 0 )
{
	pool = new std::list<IPbuffer*>();
}

IPbufferFactory::~IPbufferFactory()
{
	for( std::list<IPbuffer*>::iterator it = pool->begin(); it != pool->end(); ++it )
	{
		assert( !(*it)->inUse );

		delete *it;							// Delete the ipBuffer
	}

	delete pool;

	instance = 0;
}
