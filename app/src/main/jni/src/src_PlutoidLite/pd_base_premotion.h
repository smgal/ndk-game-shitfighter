
#ifndef __PD_BASE_PREMOTION_H__
#define __PD_BASE_PREMOTION_H__

#include "pd_util.h"
#include "avej_lite.h"

struct TPremotion
{
	THIS_STRUCT_IS_A_SINGLETON(TPremotion);

	enum TMessage
	{
		MESSAGE_NONE,
		MESSAGE_MOVE,
		MESSAGE_HOLD,
		MESSAGE_ACTION,
		MESSAGE_RUN,
		MESSAGE_LOCK,
		MESSAGE_END,
	};

	struct TPremotionData
	{
		TMessage message;
		long     param[2];

		TPremotionData()
		:	message(MESSAGE_NONE)
		{
			param[0] = 0;
			param[1] = 0;
		}
		TPremotionData(TMessage message)
		:	message(message)
		{
			param[0] = 0;
			param[1] = 0;
		}
		TPremotionData(TMessage message, long param1, long param2)
		:	message(message)
		{
			param[0] = param1;
			param[1] = param2;
		}
	};

	void operator>>(action::peek<TPremotionData>& peek) const;
	void operator>>(action::get<TPremotionData>& get) const;
	TPremotion& operator<<(const action::set<TPremotionData> set);
};

#endif
