
#include "UPcParty.h"

#include "UExtern.h"
#include <assert.h>
#include <string.h>

#define CLEAR_MEMORY(var) memset(var, 0, sizeof(var));

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool TPcParty::_Load(const CReadStream& stream)
{
	return (stream.Read((void*)save, sizeof(save)) == sizeof(save));
}

bool TPcParty::_Save(const CWriteStream& stream) const
{
	return (stream.Write((void*)save, sizeof(save)) == sizeof(save));
}

////////////////////////////////////////////////////////////////////////////////
// public method

TPcParty::TPcParty(void)
{
	CLEAR_MEMORY(save);

	encounter = 3;
	maxEnemy  = 5;
	food      = 10;
	gold      = 100;
}

void TPcParty::Face(int x1, int y1)
{
	if (x1 == 0)
		this->face = (y1 < 0) ? 1 :0;
	else
		this->face = (x1 < 0) ? 3 :2;
}

void TPcParty::Move(int x1, int y1)
{
	if ((x1 != 0) || (y1 != 0))
	{
		xPrev = x;
		yPrev = y;

		// ���� �̵��� ���� ���� ������ �ٲ��
		this->Face(x1, y1);
	}
	x += x1;
	y += y1;

}

void TPcParty::Warp(TPos pos, int _x, int _y)
{
	switch (pos)
	{
	case POS_PREV:
		x = xPrev;
		y = yPrev;
		break;
	case POS_ABS:
		xPrev = x;
		yPrev = y;
		x = _x;
		y = _y;
		break;
	case POS_REL:
		xPrev = x;
		yPrev = y;
		x += _x;
		y += _y;
		break;
	default:
		assert(false);
	}
}

void TPcParty::IgniteTorch(void)
{
	++ability.magicTorch;
	if (ability.magicTorch > 255)
		ability.magicTorch = 255;

	game::console::ShowMessage(15, "������ ������ ȶ���� �������ϴ�.");

	game::window::DisplayMap();
}

void TPcParty::Levitate(void)
{
	ability.levitation = 255;
	game::console::ShowMessage(15, "������ ���� �λ� �� �Դϴ�.");
}

void TPcParty::WalkOnWater(void)
{
	ability.walkOnWater = 255;
	game::console::ShowMessage(15, "������ �� ���� ������ �ֽ��ϴ�.");
}

void TPcParty::WalkOnSwamp(void)
{
	ability.walkOnSwamp = 255;
	game::console::ShowMessage(15, "������ �� ���� ������ �ֽ��ϴ�.");
}
