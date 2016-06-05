
#ifndef __PD_ACTOR_H__
#define __PD_ACTOR_H__

#include "avej_lite.h"
#include <string>

class CActor
{
public:
	typedef void (*FnAct)(void* p_param);

	CActor(void)
		: m_fn_act(0), m_p_param(0)
	{
	};
	CActor(void* p_param)
		: m_fn_act(0), m_p_param(p_param)
	{
	};
	CActor(FnAct fn_act, void* p_param)
		: m_fn_act(fn_act), m_p_param(p_param)
	{
	};
	virtual ~CActor()
	{
	};

	inline void Act() const
	{
		if (m_fn_act)
			m_fn_act(m_p_param);
		else
			_Act(m_p_param);
	}

	inline void SetName(const char* name)
	{
		m_name = name;
	}

	inline const char* GetName(void) const
	{
		return m_name.c_str(); 
	}

	inline static void ActorAct(CActor* p_actor)
	{
		p_actor->Act();
	};

	inline static void SharedActorAct(iu::shared_ptr<CActor> actor)
	{
		actor->Act();
	};

protected:
	inline virtual void _Act(void* p_param) const
	{
	}

private:
	FnAct       m_fn_act;
	void*       m_p_param;
	std::string m_name;

};

#endif
