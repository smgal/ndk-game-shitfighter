
#ifndef __PD_VISIBLE_H__
#define __PD_VISIBLE_H__

template <typename TRenderableObject>
class CVisible
{
public:
	typedef void (*FnShow)(TRenderableObject renderable_object);

	CVisible()
	:	m_renderable_object_is_valid(false),
		m_fn_show(0)
	{
	}
	CVisible(TRenderableObject renderable_object, FnShow fn_show)
	:	m_renderable_object_is_valid(true),
		m_renderable_object(renderable_object),
		m_fn_show(fn_show)
	{
	}
	virtual ~CVisible()
	{
	}

	void Show() const
	{
		if (m_renderable_object_is_valid)
		{
			m_renderable_object->Begin();

			if (m_fn_show)
				m_fn_show(m_renderable_object);
			else
				_Show(m_renderable_object);

			m_renderable_object->End();
		}
		else
		{
			_Show(m_renderable_object);
		}
	};

	static void VisibleShow(CVisible* p_visual_object)
	{
		p_visual_object->Show();
	};

protected:
	virtual void _Show(TRenderableObject renderable_object) const
	{
		if (m_renderable_object_is_valid)
		{
			m_renderable_object->DrawPrimitive();
		}
	}

private:
	bool              m_renderable_object_is_valid;
	TRenderableObject m_renderable_object;
	FnShow            m_fn_show;

};

#endif
