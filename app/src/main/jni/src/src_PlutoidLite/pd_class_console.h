
#ifndef __PD_CLASS_CONSOLE_H__
#define __PD_CLASS_CONSOLE_H__

#include <vector>
#include <string>
#include "util_render_text.h"

class CConsole
{
public:
	enum TTextAlign
	{
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

	CConsole(void);
	~CConsole(void);

	bool IsModified(void);
	void Clear(void);
	void SetTextColor(unsigned long color);
	void SetTextColorIndex(unsigned long index);
	void SetTextAlign(TTextAlign align);
	void Write(const char* szText);
	void Write(const std::string text);
	void Display(IorzRenderText::TFnBitBlt bitblt_callback);

	static bool SetRegion(int x, int y, int w, int h);

private:
	bool          m_isModified;
	unsigned long m_color;
	TTextAlign    m_align;
	std::vector<IorzRenderText::TBltParam*> m_line;

	unsigned long m_GetLocalTextColorFromIndex(unsigned long index, unsigned long defaultColor);
};

extern CConsole& GetConsole(void);

#endif // #ifndef __UCONSOLE_H__
