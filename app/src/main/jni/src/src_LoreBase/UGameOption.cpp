
#include "UGameOption.h"

//#include "UAvejUtil.h"
//#include <assert.h>

//using namespace avej;

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool TGameOption::_Load(const CReadStream& stream)
{
	bool result = (stream.Read((void*)&flag, sizeof(flag)) == sizeof(flag)) &&
	              (stream.Read((void*)&variable, sizeof(variable)) == sizeof(variable)) &&
	              (stream.Read((void*)&scriptFile, sizeof(scriptFile)) == sizeof(scriptFile));
	return result;
}

bool TGameOption::_Save(const CWriteStream& stream) const
{
	bool result = (stream.Write((void*)&flag, sizeof(flag)) == sizeof(flag)) &&
	              (stream.Write((void*)&variable, sizeof(variable)) == sizeof(variable)) &&
	              (stream.Write((void*)&scriptFile, sizeof(scriptFile)) == sizeof(scriptFile));
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// public method

