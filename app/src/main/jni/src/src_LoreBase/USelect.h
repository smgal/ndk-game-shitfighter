
#ifndef __USELECT_H__
#define __USELECT_H__

#pragma warning( disable: 4786 )
#include <vector>
//#include "UAvejUtil.h"

typedef std::vector<avej_lite::util::string> TMenuList;

class CSelect
{
private:
	int  m_selected;
	void m_Display(const TMenuList& menu, int nMenu, int nEnabled, int selected);

public:
	CSelect(const TMenuList& menu, int nEnabled = -1, int ixInit = 1);
	int operator()(void)
	{
		return m_selected;
	}
};

class CSelectUpDown
{
private:
	int  m_value;

public:
	/*
		INPUT PARAMETER
		x: 시작 x좌표
		y: 시작 y좌표
		min: 시작 최소 값
		max: 시작 최대 값
		step: 증감폭
		init: 초기 값
		fgColor: 출력색 (index color)
		bgColor: 배경색 (index color)
	*/
	CSelectUpDown(int x, int y, int min, int max, int step, int init, unsigned long fgColor, unsigned long bgColor);
	/*
		REUTRN VALUE: min~max 사이의 값일 때 성공, min 보다 작으면 실패
	*/
	int operator()(void)
	{
		return m_value;
	}
};

#endif // #ifndef __USELECT_H__
