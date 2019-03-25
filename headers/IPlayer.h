#ifndef IPLAYER_H
#define IPLAYER_H

#include <list>

#include <IPfragment.h>
#include <IPdatagram.h>

class IPlayer
{
  private:
	static std::list<IPdatagram*> datagramList;
	
  public:
	static void	processFragment( IPfragment& fragment );
};


#endif // IPLAYER_H
