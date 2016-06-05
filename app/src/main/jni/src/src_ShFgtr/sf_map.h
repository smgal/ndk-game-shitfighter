
#ifndef __SF_MAP_H__
#define __SF_MAP_H__

#include "avej_lite.h"

class CStageMap
{
	THIS_CLASS_IS_A_SINGLETON(CStageMap);

public:
	unsigned char GetTile(int a_x, int a_y);
	void          Move(int x, int y);
	bool          NotOnPlatform(int x, int y);
	int           GetGravity(int x, int y);
	void          SetCurrentMap(int stage);
	int           GetXOffset(void);
	int           GetYOffset(void);
	bool          IsEndOfMap(void);
};

#endif // #ifndef __SF_MAP_H__
