
#ifndef __PD_DATA_COMMUNICATION_H__
#define __PD_DATA_COMMUNICATION_H__

#include "pd_util.h"
#include "avej_lite.h"

class CPlayer;

struct TCommunication
{
	THIS_STRUCT_IS_A_SINGLETON(TCommunication);

	enum TTryTo
	{
		TRY_TO_NONE,
		TRY_TO_TALK,
		TRY_TO_END
	};

	struct TCommunicationData
	{
		TTryTo   try_to;
		CPlayer* p_from;
		CPlayer* p_to;

		TCommunicationData()
		:	try_to(TRY_TO_NONE), p_from(0), p_to(0) {}
		TCommunicationData(TTryTo _try_to, CPlayer* _p_from, CPlayer* _p_to)
		:	try_to(_try_to), p_from(_p_from), p_to(_p_to) {}
	};

	void operator>>(action::peek<TCommunicationData>& peek) const;
	void operator>>(action::get<TCommunicationData>& get) const;
	TCommunication& operator<<(const action::set<TCommunicationData> set);
};

#endif
