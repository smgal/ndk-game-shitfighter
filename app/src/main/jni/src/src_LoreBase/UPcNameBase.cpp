
#include "UPcNameBase.h"

#include "UResString.h"
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool CPcNameBase::_Load(const CReadStream& stream)
{
	if (stream.Read((void*)name, sizeof(name)) == sizeof(name))
	{
		// �ٸ� ��� ������ ���� �����ϱ� ����
		SetName(name);
		return true;
	}
	else
	{
		return false;
	}
}

bool CPcNameBase::_Save(const CWriteStream& stream) const
{
	return (stream.Write((void*)name, sizeof(name)) == sizeof(name));
}

////////////////////////////////////////////////////////////////////////////////
// public method

CPcNameBase::CPcNameBase(void)
{
	name[0] = 0;
}

CPcNameBase::~CPcNameBase(void)
{
}

bool CPcNameBase::Valid(void) const
{
	return (name[0] != 0);
}

void CPcNameBase::SetName(const char* szName)
{
	strncpy(name, szName, sizeof(name)-1);

	bool HasJongsung = resource::HasJongsung(resource::GetLastMultiByte(name));

	name_subject1  = name;
	name_subject1 += (HasJongsung) ? "��" : "��";

	name_subject2  = name;
	name_subject2 += (HasJongsung) ? "��" : "��";

	name_object    = name;
	name_object   += (HasJongsung) ? "��" : "��";
}

const char* CPcNameBase::GetName(EJosa method) const
{
	static const char* szNone = "";

	switch (method)
	{
	case EJOSA_NONE:
		return name;
	case EJOSA_SUB:
		return name_subject1;
	case EJOSA_SUB2:
		return name_subject2;
	case EJOSA_OBJ:
		return name_object;
	default:
		assert(false);
		return szNone;
	}
}
