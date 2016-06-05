
#ifndef __UPCNAMEBASE_H__
#define __UPCNAMEBASE_H__

#include <stdio.h>
#include "avej_lite.h"
#include "USerialize.h"

////////////////////////////////////////////////////////////////////////////////
// enum definition

enum EJosa
{
	EJOSA_NONE = 0, // 이름 그대로
	EJOSA_SUB,      // '은' 또는 '는'
	EJOSA_SUB2,     // '이' 또는 '가'
	EJOSA_OBJ,      // '을' 또는 '를'
};

////////////////////////////////////////////////////////////////////////////////
// class definition

class CPcNameBase: public CSerialize
{
private:
	// 저장 안하는 부분
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
