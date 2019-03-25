// general declarations and typesets and configuration options
#ifndef GLOBAL_H
#define GLOBAL_H

// On MSVC this will automatically convert the unsafe printf family functions to printf_s
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
	#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

#include <stdint.h>
#include <string>

#include <Winsock2.h>
#define _WINSOCKAPI_ // stops some MS bug with windows.h including winsock2.h twice...

#include <windows.h>

#include <log.h>



// Library parameters
// ------------------

// you can set some configuration for this library in this section

// if true, everything will be followed, otherwise only connections of which we have
// also gotten the start will be.
#define START_CAPTURE_IN_MIDDLE true


// --------------------------------------------

// the following typedefs are meant to get specific sized variables. should be replaced with u_int32_t etc on platforms that support it
// vs express 2008 does not suport fixed sizes but int is 32bits.

typedef uint8_t			byte;
typedef uint16_t			word;
typedef uint32_t			dword;
typedef uint_fast8_t		uchar;
typedef uint_fast16_t	ushort;
typedef uint_fast32_t	uint;


#define ETHER_ADDR_LEN		6	// Ethernet addresses are 6 bytes
#define ETHER_HEADER_LEN	14	// Ethernet headers are 14 bytes
#define ETHER_TYPE_IP4		8

/* Ethernet header */
struct EthernetHeader
{
	byte	etherDestHost[ETHER_ADDR_LEN];	// Destination host address
	byte	etherSrcHost [ETHER_ADDR_LEN];	// Source host address
	word 	etherType;								// IP? ARP? RARP? etc
};

/* 4 bytes IP address */
struct IPaddress
{
    byte byte1, byte2, byte3, byte4;
    std::string printable();
};

int	IPaddressToInt	( const	IPaddress&		address								);
bool	operator==		( const	IPaddress&		lhs, const	IPaddress&	rhs	);
std::string convBase	(			unsigned long	v,				long			base	);



/* IPv4 header */
struct IPheader
{
    byte			ver_hl;				// Version (4 bits) + Internet header length (4 bits)
    byte			tos;					// Type of service
    word			tlen;					// Total length
    word			identification;	// Identification
    word			flags_fo;			// Flags (3 bits) + Fragment offset (13 bits)
    byte			ttl;					// Time to live
    byte			proto;				// Protocol
    word			crc;					// Header checksum
    IPaddress	saddr;				// Source address
    IPaddress	daddr;				// Destination address
    dword		op_pad;				// Option + Padding
};



// TCP header
struct TCPheader
{
	word	sport;				// source port
	word	dport;				// destination port
	dword sequence;			// sequence number
	dword	acknowledge;		// acknowledgement number
	byte	offset_reserved;	// data offset, reserved
	byte	th_flags;			// flags
	word	window;				// window
	word	checksum;			// checksum
	word	urgent;				// urgent pointer
};


// for console coloring
enum ConsoleColor
{
	red		= FOREGROUND_RED		| FOREGROUND_INTENSITY,
	green		= FOREGROUND_GREEN	| FOREGROUND_INTENSITY,
	blue		= FOREGROUND_BLUE		| FOREGROUND_INTENSITY,
	yellow	= FOREGROUND_GREEN	| FOREGROUND_RED				| FOREGROUND_INTENSITY,
	white		= FOREGROUND_RED		| FOREGROUND_GREEN			| FOREGROUND_BLUE
};


std::ostream& operator<< (std::ostream& s,const ConsoleColor& c);

byte convertHexNum( const byte& a, const byte& b );



#endif //GLOBAL_H
