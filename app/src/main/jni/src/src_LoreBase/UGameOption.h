
#ifndef __UGAMEOPTION_H__
#define __UGAMEOPTION_H__

#include "USmSola.h"
#include "USerialize.h"
#include "avej_lite.h"

struct TGameOption: public CSerialize
{
	enum
	{
		MAX_FLAG     = 256,
		MAX_VARIABLE = 256
	};

	sola::boolflag<MAX_FLAG> flag;
	sola::intflag<unsigned char, MAX_VARIABLE> variable;
	avej_lite::util::string scriptFile;

protected:
	bool _Load(const CReadStream& stream);
	bool _Save(const CWriteStream& stream) const;
};

#endif // #ifndef __UGAMEOPTION_H__
