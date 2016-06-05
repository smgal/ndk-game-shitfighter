
#include "UKeyBuffer.h"
#include "avej_lite.h"

#include <string.h>

#define CLEAR_MEMORY(var) memset(var, 0, sizeof(var));

CKeyBuffer::CKeyBuffer(void)
	: m_keyHeadPtr(0), m_keyTailPtr(0)
{
	CLEAR_MEMORY(m_KeyBuffer);
	CLEAR_MEMORY(m_KeyBuffer);
}

CKeyBuffer::~CKeyBuffer(void)
{
}

static CKeyBuffer s_keyBuffer;

CKeyBuffer& GetKeyBuffer(void)
{
	return s_keyBuffer;
}
