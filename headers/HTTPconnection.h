#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <deque>

#include <global.h>
#include <HTTPparser.h>

class TCPstream;

class HTTPstream
{
		TCPstream* 				tcpStream;
		HTTPparser 				httpParser;
		HTTPparser::status_t httpParserStatus;

	public:
		HTTPstream();

		void setTCPstream( TCPstream* tcpStream );
		void parse();

};

class HTTPconnection
{

};

#endif HTTPCONNECTION_H
