#include <cassert>
#include <iostream>
#include <globalTests.h>

void runTests()
{
	assert( testIPaddress() );
	std::cout << "All tests passed" << std::endl;
}