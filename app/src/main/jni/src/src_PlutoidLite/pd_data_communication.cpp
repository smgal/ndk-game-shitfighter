
#include "pd_data_communication.h"

#include <queue>

static std::queue<TCommunication::TCommunicationData> s_communication;
static TCommunication::TCommunicationData s_null_communication;

TCommunication::TCommunication()
{
}

TCommunication::~TCommunication()
{
}

void TCommunication::operator>>(action::peek<TCommunicationData>& peek) const
{
	if (!s_communication.empty())
	{
		peek.data = s_communication.front();
	}
	else
	{
		peek.data = s_null_communication;
	}
}

void TCommunication::operator>>(action::get<TCommunicationData>& get) const
{
	if (!s_communication.empty())
	{
		get.data = s_communication.front();
		s_communication.pop();
	}
	else
	{
		get.data = s_null_communication;
	}
}

TCommunication& TCommunication::operator<<(const action::set<TCommunicationData> set)
{
	s_communication.push(set.data);

	return *this;
}
