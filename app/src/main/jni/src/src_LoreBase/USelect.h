
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
		x: ���� x��ǥ
		y: ���� y��ǥ
		min: ���� �ּ� ��
		max: ���� �ִ� ��
		step: ������
		init: �ʱ� ��
		fgColor: ��»� (index color)
		bgColor: ���� (index color)
	*/
	CSelectUpDown(int x, int y, int min, int max, int step, int init, unsigned long fgColor, unsigned long bgColor);
	/*
		REUTRN VALUE: min~max ������ ���� �� ����, min ���� ������ ����
	*/
	int operator()(void)
	{
		return m_value;
	}
};

#endif // #ifndef __USELECT_H__
