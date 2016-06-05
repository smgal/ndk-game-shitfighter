
#pragma warning(disable: 4786)

#include "pd_base_premotion.h"
#include <assert.h>
#include <queue>

////////////////////////////////////////////////////////////////////////////////
// struct CTheAbsolute::TPremotion

static std::queue<TPremotion::TPremotionData> s_premotion;
static TPremotion::TPremotionData s_null_message;

TPremotion::TPremotion()
{
}

TPremotion::~TPremotion()
{
}

void TPremotion::operator>>(action::peek<TPremotionData>& peek) const
{
	if (!s_premotion.empty())
	{
		peek.data = s_premotion.front();
	}
	else
	{
		peek.data = s_null_message;
	}
}

void TPremotion::operator>>(action::get<TPremotionData>& get) const
{
	if (!s_premotion.empty())
	{
		get.data = s_premotion.front();
		s_premotion.pop();
	}
	else
	{
		get.data = s_null_message;
	}
}

TPremotion& TPremotion::operator<<(const action::set<TPremotionData> set)
{
	s_premotion.push(set.data);

	return *this;
}
