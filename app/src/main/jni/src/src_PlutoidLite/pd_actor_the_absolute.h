
#ifndef __PD_ACTOR_THE_ABSOLUTE_H__
#define __PD_ACTOR_THE_ABSOLUTE_H__

#include "pd_util.h"
#include "pd_actor.h"
#include "avej_lite.h"

class CTheAbsolute: public CActor
{
	THIS_CLASS_IS_A_SINGLETON(CTheAbsolute);

public:
	enum TState
	{
		STATE_COMMING,
		STATE_BEGING,
		STATE_GONE,
	};

	static TState GetState(void);

protected:
	virtual void _Act(void* p_param) const;

};

#endif
