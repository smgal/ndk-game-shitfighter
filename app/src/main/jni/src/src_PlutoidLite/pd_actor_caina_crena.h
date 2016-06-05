
#ifndef __PD_ACTOR_CAINA_CRENA_H__
#define __PD_ACTOR_CAINA_CRENA_H__

#include "pd_util.h"
#include "pd_actor.h"
#include "avej_lite.h"

class CCainaCrena: public CActor
{
	THIS_CLASS_IS_A_SINGLETON(CCainaCrena);

protected:
	virtual void _Act(void* p_param) const;

};

#endif
