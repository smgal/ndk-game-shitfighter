
#ifndef __PD_CLASS_APP_BASE_H__
#define __PD_CLASS_APP_BASE_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include "avej_lite.h"
//#include "device/avej_device_input.h"
//#include "event/avej_event_receiver.h"

////////////////////////////////////////////////////////////////////////////////
// class CAppBase

class CAppBase
{
protected:
	bool                m_is_terminated;
	unsigned long       m_ref_tick;
	avej::CInputDevice& m_input_device;

	CAppBase()
		: m_is_terminated(false), m_ref_tick(0),
		  m_input_device(avej_lite::singleton<avej::CInputDevice>::get())
	{
	}

	void _RegisterDefaultCallback(CAppBase* thiz)
	{
		avej::event::CReceiver& receiver = avej_lite::singleton<avej::event::CReceiver>::get();

		{
			MAKE_PROCESS_CALLBACK(callback_process_create, CAppBase, thiz, &CAppBase::OnCreate);
			receiver << avej::event::CReceiver::Operation::RegisterProcessEvent(avej::event::CReceiver::PROCESS_CREATE, callback_process_create);
		}
		{
			MAKE_PROCESS_CALLBACK(callback_process_destroy, CAppBase, thiz, &CAppBase::OnDestroy);
			receiver << avej::event::CReceiver::Operation::RegisterProcessEvent(avej::event::CReceiver::PROCESS_DESTROY, callback_process_destroy);
		}
		{
			MAKE_PROCESS_CALLBACK(callback_process_idle, CAppBase, thiz, &CAppBase::OnIdle);
			receiver << avej::event::CReceiver::Operation::RegisterProcessEvent(avej::event::CReceiver::PROCESS_IDLE, callback_process_idle);
		}
		{
			MAKE_PROCESS_CALLBACK(callback_process_terminate, CAppBase, thiz, &CAppBase::OnTerminate);
			receiver << avej::event::CReceiver::Operation::RegisterProcessEvent(avej::event::CReceiver::PROCESS_TERMINATE, callback_process_terminate);
		}
	}

	virtual void _Run(unsigned long m_ref_tick, const avej::CInputDevice& input_device) = 0;

public:
	virtual ~CAppBase()
	{
	}

	void Run(void)
	{
		{
			const avej::event::CReceiver& receiver = avej_lite::singleton<avej::event::CReceiver>::get();
			receiver << avej::event::CReceiver::Operation::Event(avej::event::CReceiver::TYPE_PROCESS, avej::event::CReceiver::PROCESS_CREATE, 0, 0);
		}

		while (!m_is_terminated)
		{
			//?? 포팅된 함수를 불러야 함
			++m_ref_tick;
			m_input_device.UpdateInputState();

			//?? key 상태를 파악하고 그 결과를 _Run()에 넘겨 주어야 한다.
			_Run(m_ref_tick, m_input_device);
		}

		{
			const avej::event::CReceiver& receiver = avej_lite::singleton<avej::event::CReceiver>::get();
			receiver << avej::event::CReceiver::Operation::Event(avej::event::CReceiver::TYPE_PROCESS, avej::event::CReceiver::PROCESS_DESTROY, 0, 0);
		}
	}

	virtual void OnCreate(unsigned long ref_time, unsigned long param) = 0;

	virtual void OnDestroy(unsigned long ref_time, unsigned long param) = 0;

	virtual void OnIdle(unsigned long ref_time, unsigned long param) = 0;

	void OnTerminate(unsigned long ref_time, unsigned long param)
	{
		m_is_terminated = true;
	}
};

#endif
