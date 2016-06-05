
#ifndef __PD_ACTOR_ALBIREO_H__
#define __PD_ACTOR_ALBIREO_H__

#include "pd_util.h"
#include "pd_actor.h"
#include "avej_lite.h"

enum TDreamColorAlias
{
	DREAM_COLOR_BLACK,
	DREAM_COLOR_WHITE,
	DREAM_COLOR_NORMAL,
	DREAM_COLOR_APPROCH,
	DREAM_COLOR_SHADOW
};

class CAlbireo: public CActor
{
	THIS_CLASS_IS_A_SINGLETON(CAlbireo);

public:
	TPosition            GetBeholdingPosition(void);

	static unsigned long GetDreamColor(TDreamColorAlias alias);
	static unsigned long GetCurrentDreamColor(void);

protected:
	virtual void _Act(void* p_param) const;

};

#endif
