
#ifndef __USMSET_H__
#define __USMSET_H__

class CSmSet
{
public:
	enum { SET_SIZE = 256/8 };
	typedef unsigned char TSet[SET_SIZE];

private:
	TSet  m_set;

	int   m_strlen(const char* pStr);
	char* m_strchr(const char* pStr, char ch, int n);
	int   m_atoi(const char* pStr, int n);
	bool  m_makeSetArray(const char* szStream, TSet& set);

public:
	CSmSet(void);
	CSmSet(const char* szStream);
	void  Assign(const char* szStream);
	inline const TSet& GetSet(void) const { return m_set; }
	inline bool  IsSet(unsigned char index) const { return (m_set[index/8] & (1 << (index%8))) > 0; }
};

#endif // #ifndef __USMSET_H__
