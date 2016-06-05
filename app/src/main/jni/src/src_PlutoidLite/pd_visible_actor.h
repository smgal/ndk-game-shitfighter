
#ifndef __PD_VISIBLE_ACTOR_H__
#define __PD_VISIBLE_ACTOR_H__

#include "pd_actor.h"
#include "pd_visible.h"

template <typename TRenderableObject>
class CVisibleActor
{
	typedef iu::shared_ptr<CVisible<TRenderableObject> > TSharedVisible;
	typedef iu::shared_ptr<CActor>                       TSharedActor;

public:
	CVisibleActor(TSharedVisible visible, TSharedActor actor)
		: m_visible(visible), m_actor(actor)
	{
	}
	~CVisibleActor()
	{
	}

	const TSharedVisible& GetVisible(void)
	{
		return m_visible;
	}
	const TSharedActor& GetActor(void)
	{
		return m_actor;
	}

private:
	TSharedVisible m_visible;
	TSharedActor   m_actor;

};

#endif
