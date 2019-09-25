
#include <global.h>
#include <boost/lexical_cast.hpp>

using namespace std;

std::string IPaddress::printable()
{
	return      boost::lexical_cast<std::string>( (unsigned short) byte1 ) + "."
				+  boost::lexical_cast<std::string>( (unsigned short) byte2 ) + "."
				+  boost::lexical_cast<std::string>( (unsigned short) byte3 ) + "."
				+  boost::lexical_cast<std::string>( (unsigned short) byte4 );
}

int IPaddressToInt( const IPaddress& address )
{
	int result = 0;

	result += address.byte1 << 24;
	result += address.byte2 << 16;
	result += address.byte3 << 8;
	result += address.byte4;
	return result;
}

bool operator==( const IPaddress& lhs, const IPaddress& rhs )
{
	if(      lhs.byte1 == rhs.byte1
			&& lhs.byte2 == rhs.byte2
			&& lhs.byte3 == rhs.byte3
			&& lhs.byte4 == rhs.byte4 )

		return true;

	else return false;
}


std::string convBase(unsigned long v, long base)
{
	std::string digits = "0123456789abcdef";
	std::string result;
	if(  ( base < 2 ) || ( base > 16 ) ) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[ v % base ] + result;
			v /= base;
		}
		while( v );
	}
	return result;
}


// add some coloring to cout for debugging



std::ostream& operator<< (std::ostream& s,const ConsoleColor& c)
{
	HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hStdout,c );
	return s;
}

// This function translates two bytes to a url decoded byte (letter) returns 0 if not digits
byte convertHexNum( const byte& a, const byte& b )
{
	// check if the are valid digits 0-9 and a-f and A-F using the ascii codes
	if(
				!( ( 0x29 < a && a < 0x40 ) || ( 0x40 < a && a < 0x47 ) || ( 0x60 < a && a < 0x67 ) )
			|| !( ( 0x29 < b && b < 0x40)  || ( 0x40 < b && b < 0x47 ) || ( 0x60 < b && b < 0x67 ) )
	  )

		return 0x00;

	return      (   ( ( a & 0xf ) + ( ( a >= 'A' ) ? 9 : 0 ) ) << 4 )
				|      ( ( b & 0xf ) + ( ( b >= 'A' ) ? 9 : 0 ) );

}
