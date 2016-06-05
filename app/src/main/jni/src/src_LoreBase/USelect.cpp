
#include <assert.h>
#include <vector>
#include <string>

#include "UExtern.h"
#include "USelect.h"
#include "UConsole.h"
#include "UKeyBuffer.h"
#include "UMain.h"

void CSelect::m_Display(const TMenuList& menu, int nMenu, int nEnabled, int selected)
{
	assert(nMenu > 0);
	assert(nEnabled <= nMenu);
	assert(selected > 0 && selected <= nMenu);

	CLoreConsole& console = GetConsole();

	console.Clear();
	console.SetTextColor(0xFFFF0000);
	console.Write(menu[0]);
	console.Write("");

	for (int i = 1; i <= nMenu; ++i)
	{
		console.SetTextColor((i == selected) ? 0xFFFFFFFF : ((i <= nEnabled) ? 0xFF808080 : 0xFF000000));
		console.Write(menu[i]);
	}

	console.Display();
	game::UpdateScreen();
}

CSelect::CSelect(const TMenuList& menu, int nEnabled, int ixInit)
	: m_selected(0)
{
	int nMenu = menu.size() - 1;

	assert(nMenu > 0);

	if (nEnabled < 0)
		nEnabled = nMenu;

	if (ixInit < 0)
		ixInit = 1;
	if (ixInit > nMenu)
		ixInit = nMenu;

	int selected = ixInit;

	do
	{
		m_Display(menu, nMenu, nEnabled, selected);

		bool bUpdate = false;

		do
		{
			unsigned short key;
			while ((key = GetKeyBuffer().GetKey()) < 0)
				;

			switch (key)
			{
			case avej_lite::INPUT_KEY_UP:
			case avej_lite::INPUT_KEY_DOWN:
				{
					int dy = (key == avej_lite::INPUT_KEY_UP) ? -1 : +1;
					selected += dy;

					if (selected <= 0)
						selected = nEnabled;
					if (selected > nEnabled)
						selected = 1;

					bUpdate = true;
				}
				break;
			case avej_lite::INPUT_KEY_A:
				selected = 0;
				// pass through
			case avej_lite::INPUT_KEY_B:
				{
					CLoreConsole& console = GetConsole();
					console.Clear();
					console.Display();
				}

				m_selected = selected;

				return;
			}
		} while (!bUpdate);
		
	} while (1);
}

//////////////////////////////////////////////////////
// CSelectUpDown

CSelectUpDown::CSelectUpDown(int x, int y, int min, int max, int step, int init, unsigned long fgColor, unsigned long bgColor)
	: m_value(init)
{
	do
	{
		//@@ IntToStr를 2번하니 비효율적
		pBackBuffer->FillRect(bgColor, x, y, 6*strlen(IntToStr(m_value)()), 12);
		g_DrawText(x, y, IntToStr(m_value)(), fgColor);
		game::UpdateScreen();

		bool bUpdate = false;

		do
		{
			unsigned short key;
			while ((key = GetKeyBuffer().GetKey()) < 0)
				;

			switch (key)
			{
			case avej_lite::INPUT_KEY_UP:
			case avej_lite::INPUT_KEY_DOWN:
			case avej_lite::INPUT_KEY_LEFT:
			case avej_lite::INPUT_KEY_RIGHT:
				{
					int dy = ((key == avej_lite::INPUT_KEY_DOWN) || (key == avej_lite::INPUT_KEY_LEFT)) ? -1 : +1;
					m_value += dy * step;

					if (m_value < min)
						m_value = min;
					if (m_value > max)
						m_value = max;

					bUpdate = true;
				}
				break;
			case avej_lite::INPUT_KEY_A:
				m_value = min-1;
				// pass through
			case avej_lite::INPUT_KEY_B:
				{
					CLoreConsole& console = GetConsole();
					console.Clear();
					console.Display();
				}
				return;
			}
		} while (!bUpdate);
		
	} while (1);
}
