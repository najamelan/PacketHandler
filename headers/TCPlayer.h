#ifndef TCPLAYER_H
#define TCPLAYER_H

#include <list>

#include <TCPconnection.h>
#include <TCPsegment.h>


class TCPlayer
{
  private:
	static std::list<TCPconnection*> connectionList;
	
  public:
	static void	processSegment( TCPsegment& segment );

};



#endif TCPLAYER_H