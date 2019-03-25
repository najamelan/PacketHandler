#include <cassert>
#include <iostream>

#include <globalTests.h>


bool testIPaddress()
{
	IPaddress address;

	address.byte1 = 0xf3;
	address.byte2 = 0x0;
	address.byte3 = 0x13;
	address.byte4 = 0x7f;

	return IPaddressToInt( address ) != 0;
}