// Object pool of fragmentation buffers for ip packet reassembly

#ifndef IPBUFFERFACTORY_H
#define IPBUFFERFACTORY_H

#include <list>

#include <global.h>


struct IPbuffer
{
	bool 	inUse;
	byte* buffer; 

	IPbuffer() : inUse( false ), buffer( new byte[66000] ) {}; // max size of ip datagram is 64k
  ~IPbuffer() { delete[] buffer; };
};

class IPbufferFactory
{
  private:
	static			IPbufferFactory*			instance;
	static	const dword 						MINPOOLSIZE = 4;
						std::list<IPbuffer*>* 	pool;
						dword 						availableBufferCounter;


	public:
		static IPbufferFactory*		getInstance		();
		static void						destroyInstance() { if( instance ) delete instance; instance = 0; };
		 		 byte* 					acquireBuffer();
		 		 void				 		releaseBuffer( byte* );


	private:
								IPbufferFactory();
								IPbufferFactory( const IPbufferFactory& );	// prevent copying
		IPbufferFactory&	operator=		( const IPbufferFactory& );	// prevent assigning 
							  ~IPbufferFactory();
};

#endif /*BUFFERFACTORY_H*/
