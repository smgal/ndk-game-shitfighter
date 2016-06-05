
#pragma warning(disable: 4786)

#include <vector>

#include "avej_lite.h"
#include "sf_util.h"

void util::ClearKeyBuffer(void)
{
	avej_lite::CInputDevice& input_device = avej_lite::singleton<avej_lite::CInputDevice>::get();
	input_device.UpdateInputState();

	// 키버퍼 소거
	for (avej_lite::TInputKey key = avej_lite::INPUT_KEY_BEGIN; key < avej_lite::INPUT_KEY_END; inc_(key))
		input_device.WasKeyPressed(key);
}

////////////////////////////////////////////////////////////////////////////////
// class CTextFileFromRes

struct util::CTextFileFromRes::TImpl
{
public:
	TImpl(const char* sz_text_stream, int size)
	{
		const char* p_stream_beg = sz_text_stream;
		const char* p_stream_end = sz_text_stream + size;

		const char* p_stream = sz_text_stream;

		while (p_stream < p_stream_end)
		{
			if ((*p_stream != 0x0D) && (*p_stream != 0x0A))
			{
				++p_stream;
				continue;
			}

			m_paragraph_list.push_back(std::make_pair(p_stream_beg, p_stream));

			++p_stream;

			// <CR>로 종료
			if (p_stream >= p_stream_end)
				break;

			if (*p_stream == 0x0A)
				++p_stream;

			p_stream_beg = p_stream;
		}

		m_current_paragraph = m_paragraph_list.begin();
	}

	bool IsValid(void)
	{
		if (m_paragraph_list.size() == 0)
			return false;

		return (m_current_paragraph != m_paragraph_list.end());
	}

	bool GetString(char* out_str, int str_len)
	{
		if (m_current_paragraph == m_paragraph_list.end())
			return false;

		int sour_len = m_current_paragraph->second - m_current_paragraph->first;
		int copy_len = min(str_len, sour_len);

		strncpy(out_str, m_current_paragraph->first, copy_len);

		if (copy_len < str_len)
			out_str[copy_len] = 0;

		++m_current_paragraph;

		return true;
	}

private:
	typedef std::pair<const char*, const char*> TParagraph;

	std::vector<TParagraph>           m_paragraph_list;
	std::vector<TParagraph>::iterator m_current_paragraph;

};

util::CTextFileFromRes::CTextFileFromRes(const char* sz_text_stream, int size)
: m_p_impl(new TImpl(sz_text_stream, size))
{
}

util::CTextFileFromRes::~CTextFileFromRes()
{
	delete m_p_impl;
}

bool util::CTextFileFromRes::IsValid(void)
{
	return m_p_impl->IsValid();
}

bool util::CTextFileFromRes::GetString(char* out_str, int str_len)
{
	return m_p_impl->GetString(out_str, str_len);
}

////////////////////////////////////////////////////////////////////////////////
// ComposeString()

static void s_Int2Str(char **ppBuffer, int value)
{
	if (value == 0)
	{
		*((*ppBuffer)++) = '0';
	}
	else
	{
		if (value < 0)
		{
			*((*ppBuffer)++) = '-';
			value = -value;
		}

		int nNum = 0;
		int temp = value;
		while (temp)
		{
			temp = temp / 10;
			++nNum;
		}

		int nLoop = nNum;
		while (nLoop-- > 0)
		{
			(*ppBuffer)[nLoop] = (value % 10) + '0';
			value = value / 10;
		}
		*ppBuffer += nNum;
	}
}

void util::ComposeString(char pBuffer[], const char* szFormat, const int data ...)
{
	if (pBuffer == 0)
		return;

	const int* pData = &data;

	while (*szFormat)
	{
		if (*szFormat != '@')
		{
			*pBuffer++ = *szFormat++;
			continue;
		}
		++szFormat;
		s_Int2Str(&pBuffer, *pData++);
	}

	*pBuffer = 0;
}