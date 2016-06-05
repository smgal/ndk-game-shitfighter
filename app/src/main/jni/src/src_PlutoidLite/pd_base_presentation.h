
#ifndef __PD_BASE_PRESENTATION_H__
#define __PD_BASE_PRESENTATION_H__

#include "pd_util.h"
#include "pd_base_world.h"
#include "pd_data_communication.h"
#include "avej_lite.h"

struct TPresentation
{
	friend class ::avej_lite::singleton<TPresentation>;
	friend class ::avej_lite::auto_deletor<TPresentation>;
public:

	virtual TPresentation& operator<<(const TWorld& ref_world)
	{
		if (m_p_ref_current)
			m_p_ref_current->operator<<(ref_world);

		return *this;
	}

	virtual TPresentation& operator<<(action::run<unsigned int>& run)
	{
		if (m_p_ref_current)
			m_p_ref_current->operator<<(run);

		return *this;
	}

	virtual TPresentation& operator<<(action::run<TCommunication::TCommunicationData>& communication)
	{
		if (m_p_ref_current)
			m_p_ref_current->operator<<(communication);

		return *this;
	}

	TPresentation& operator<<(const char* sz_name);

	static void Register(const char* name, TPresentation* (*fnGetPresentationInstance)(void));

protected:
	TPresentation()
	:	m_p_ref_current(0) {}
	virtual ~TPresentation() {};

private:
	TPresentation* m_p_ref_current;

	TPresentation(const TPresentation&);
	TPresentation& operator=(const TPresentation&);
};

#endif
