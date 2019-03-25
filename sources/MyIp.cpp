#include <WinSock2.h>
#include <iostream>

#include <MyIp.h>

std::string myIp()
{
// Add 'ws2_32.lib' to your linker options

    WSADATA WSAData;

    // Initialize winsock dll
    if(::WSAStartup(MAKEWORD(1, 0), &WSAData))
    {
        // Error handling
    }

    // Get local host name
    char szHostName[128] = "";

    if(::gethostname(szHostName, sizeof(szHostName)))
    {
        // Error handling -> call 'WSAGetLastError()'
    }

    // Get local IP addresses
    struct sockaddr_in SocketAddress;
    struct hostent     *pHost        = 0;

    pHost = ::gethostbyname(szHostName);
    if(!pHost)
    {
        // Error handling -> call 'WSAGetLastError()'
    }

    char aszIPAddresses[10][16]; // maximum of ten IP addresses
	std::string address;

    for(int iCnt = 0; ((pHost->h_addr_list[iCnt]) && (iCnt < 10)); ++iCnt)
    {
        memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[iCnt], pHost->h_length);
        strcpy_s(aszIPAddresses[iCnt], 16, inet_ntoa(SocketAddress.sin_addr));
		address.append( inet_ntoa( SocketAddress.sin_addr ) );
		//std::cout << aszIPAddresses[iCnt] << std::endl;
		break;
    }

    // Cleanup
    WSACleanup();
	return address;

}