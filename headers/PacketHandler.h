#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

#include <boost/signal.hpp>
#include <json_spirit.h>


#include <global.h>

struct pcap_pkthdr;

struct pcap;
typedef struct pcap pcap_t;

class PacketHandler
{

	public:
		static boost::signal< void ( const json_spirit::mValue& ) > emitJSON;
		static pcap_t*		adhandle;

		uint listen();

};


void pcapCallback( byte* param, const struct pcap_pkthdr* header, const byte* pkt_data );


#endif /* PACKETHANDLER_H_ */
