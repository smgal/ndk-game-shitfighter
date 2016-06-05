
#ifndef __USMFONT12X12_H__
#define __USMFONT12X12_H__

typedef char MCHAR;

class ISmBltBasedFont
{
public:
	enum { INVALID_INDEX = -1 };
	struct TBltParam
	{
		bool isAvailable;
		long xDest, yDest;
		long x1, y1, x2, y2;
		unsigned long color;
		int  indexColor;
	};
	virtual ~ISmBltBasedFont(void) {};
	virtual bool RenderText(int xDest, int yDest, const MCHAR* szText, unsigned long color, TBltParam rect[], int nRect = 1) = 0;
};

class CSmFont12x12: public ISmBltBasedFont
{
public:
	CSmFont12x12(void);
	~CSmFont12x12(void);
	bool RenderText(int xDest, int yDest, const MCHAR* szText, unsigned long color, TBltParam rect[], int nRect = 1);

private:
	int  m_indexColor;
};

#endif // __USMFONT12X12_H__
