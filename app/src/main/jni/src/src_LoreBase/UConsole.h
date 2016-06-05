
#ifndef __UCONSOLE_H__
#define __UCONSOLE_H__

#include <vector>
#include <string>
#include "util_render_text.h"

struct TBound
{
	int x1, y1, x2, y2;
	TBound(int _x1, int _y1, int _x2, int _y2)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
	TBound()
		: x1(0), y1(0), x2(0), y2(0) {}
};

struct TBltParam
{
	bool          is_available;
	TBound        dst_bound;
	TBound        src_bound;
	unsigned long color;

	TBltParam(int xDest, int yDest, int width, int height, int xSour, int ySour, unsigned long _color)
	:	is_available(true), dst_bound(xDest, yDest, xDest+width, yDest+height), src_bound(xSour, ySour, xSour+width, ySour+height), color(_color) {}
	TBltParam()
	:	is_available(false) {}
};

class CLoreConsole
{
public:
	enum TTextAlign
	{
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

	CLoreConsole(void);
	~CLoreConsole(void);

	bool IsModified(void);
	void Clear(void);
	void SetBgColor(unsigned long color);
	unsigned long GetBgColor(void);
	void SetTextColor(unsigned long color);
	void SetTextColorIndex(unsigned long index);
	void SetTextAlign(TTextAlign align);
	void Write(const char* szText);
	void Write(const std::string text);
	void Display(void);

	bool SetRegion(int x, int y, int w, int h);
	void GetRegion(int* pX, int* pY, int* pW, int* pH) const;

private:
	bool          m_isModified;
	unsigned long m_textColor;
	unsigned long m_bgColor;
	TTextAlign    m_align;
	int           m_xOffset;
	int           m_yOffset;
	int           m_width;
	int           m_height;
	std::vector<TBltParam*> m_line;

};

extern CLoreConsole& GetConsole(void);

#endif // #ifndef __UCONSOLE_H__
