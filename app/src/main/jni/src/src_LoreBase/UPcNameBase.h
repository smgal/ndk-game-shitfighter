
#ifndef __UPCNAMEBASE_H__
#define __UPCNAMEBASE_H__

#include <stdio.h>
#include "avej_lite.h"
#include "USerialize.h"

////////////////////////////////////////////////////////////////////////////////
// enum definition

enum EJosa
{
	EJOSA_NONE = 0, // �̸� �״��
	EJOSA_SUB,      // '��' �Ǵ� '��'
	EJOSA_SUB2,     // '��' �Ǵ� '��'
	EJOSA_OBJ,      // '��' �Ǵ� '��'
};

////////////////////////////////////////////////////////////////////////////////
// class definition

class CPcNameBase: public CSerialize
{
private:
	// ���� ���ϴ� �κ�
	avej_lite::util::string name_subject1;
	avej_lite::util::string name_subject2;
	avej_lite::util::string name_object;

protected:
	char  name[24];

	bool _Load(const CReadStream& stream);
	bool _Save(const CWriteStream& stream) const;

public:
	CPcNameBase(void);
	virtual ~CPcNameBase(void);

	bool  Valid(void) const;

	void SetName(const char* szName);
	const char* GetName(EJosa method = EJOSA_NONE) const;
};

#endif // #ifndef __UPCNAMEBASE_H__
