#ifndef JSONLAYER_H_
#define JSONLAYER_H_

#include <string>
#include <deque>

#include <global.h> // we need to include stdint.h before boost... otherwise we get a macro redefinition

#include <json_spirit.h>

using json_spirit::mValue;

class JSONlayer
{
		static std::deque<mValue*> valueList;

	public:
		static void	processString( std::string& contentString );
};

#endif /* JSONLAYER_H_ */
