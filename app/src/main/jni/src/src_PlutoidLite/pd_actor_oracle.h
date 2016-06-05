
#ifndef __PD_ACTOR_ORACLE_H__
#define __PD_ACTOR_ORACLE_H__

#include "pd_util.h"
#include "pd_actor.h"
#include "avej_lite.h"

class COracle: public CActor
{
	THIS_CLASS_IS_A_SINGLETON(COracle);

protected:
	virtual void _Act(void* p_param) const;

};

#endif
