
#ifndef __UPCPARTY_H__
#define __UPCPARTY_H__

#include "USerialize.h"

struct TPcParty: public CSerialize
{
	enum TPos
	{
		POS_PREV,
		POS_ABS,
		POS_REL,
	};

	union
	{
		struct
		{
			long x, y;
			long xPrev, yPrev;
			long face;
			long encounter;
			long maxEnemy;

			long food;
			long gold;

			struct
			{
				long magicTorch;
				long levitation;
				long walkOnWater;
				long walkOnSwamp;
				long mindControl;
				bool canUseESP; // etc[39] and bit1 > 0
				bool canUseSpecialMagic; // etc[38] and bit1 > 0
			} ability;
		};

		char save[20*sizeof(long)];
	};

	TPcParty(void);
	void Move(int x1, int y1);
	void Warp(TPos pos, int _x = 0, int _y = 0);
	void Face(int x1, int y1);

	void IgniteTorch(void);
	void Levitate(void);
	void WalkOnWater(void);
	void WalkOnSwamp(void);

protected:
	bool _Load(const CReadStream& stream);
	bool _Save(const CWriteStream& stream) const;
};

#endif // #ifndef __UPCPARTY_H__
