///////////////////

#include "pd_base_presentation.h"
#include <map>
#include <assert.h>

typedef TPresentation* (*TFnGetPresentationInstance)(void);
typedef std::map<const char*, TFnGetPresentationInstance> TPresentationList;

static TPresentationList& s_GetPresentationList(void)
{
	static TPresentationList s_presentation_list;
	return s_presentation_list;
}

TPresentation& TPresentation::operator<<(const char* sz_name)
{
	TPresentationList& presentation_list = s_GetPresentationList();

	TPresentationList::iterator found = presentation_list.find(sz_name);

	assert(found != presentation_list.end());

	if (found != presentation_list.end())
		m_p_ref_current = (*found->second)();

	return *this;
}

void TPresentation::Register(const char* name, TPresentation* (*fnGetPresentationInstance)(void))
{
	TPresentationList& presentation_list = s_GetPresentationList();

	presentation_list[name] = fnGetPresentationInstance;
}
