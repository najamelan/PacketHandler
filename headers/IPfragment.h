#ifndef IPFRAGMENT_H
#define IPFRAGMENT_H

//#include <Windows.h>
#include <global.h>

//#include <pcap.h>



class IPfragment
{
  public:

			byte*					const dataBuffer;		// if we have been copied, we need to free this in the destructor, if original winpcap will free the buffer
	const	IPheader*			const ipHeader;

	const	EthernetHeader*	const ethernetHeader;
	const	byte*					const payload;

				 IPfragment( const byte* const pkt_data );
				 IPfragment( const IPfragment& src );
	virtual  ~IPfragment();

	word getDatagramID() const { return	IPaddressToInt( ipHeader->saddr ) ^
													IPaddressToInt( ipHeader->daddr ) ^
													identification(); };

	word			version()			const	{ return		ipHeader->ver_hl >> 4;									};
	word			headerLength()		const { return		( ipHeader->ver_hl & 0x0f ) *4;						}; // IP header length in bytes
	word			totalLength()		const { return		ntohs( ipHeader->tlen );								};
	word			identification()	const { return		ntohs( ipHeader->identification );					};
	bool			reservedFragFlag()const	{ return		( ntohs( ipHeader->flags_fo )	& 0x8000 ) != 0;	};
	bool			dontFragFlag()		const	{ return		( ntohs( ipHeader->flags_fo )	& 0x4000 ) != 0;	};
	bool			moreFragFlag()		const	{ return		( ntohs( ipHeader->flags_fo ) & 0x2000 ) != 0;	};
	word			fragOffset()		const	{ return		ntohs( ipHeader->flags_fo ) & 0x1fff;				};
	IPaddress	srcAddr()			const { return		ipHeader->saddr;											};
	IPaddress	destAddr()			const { return		ipHeader->daddr;											};

  private:
	  IPfragment& operator=( const IPfragment& rhs ) {}; // prevent accidental assigning
};


#endif // IPFRAGMENT_H
