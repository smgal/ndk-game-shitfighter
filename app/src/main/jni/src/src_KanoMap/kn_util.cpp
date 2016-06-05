
#pragma warning(disable: 4786)

#include <vector>

#include "kn_util.h"

namespace kano
{
	int util::tstrlen(const tchar* str1)
	{
		const tchar* str2 = str1;
		while (*str2++)
			;
		return str2-str1-1;
	}

	tchar* util::tstrcpy(tchar* dest, const tchar* src)
	{
		return util::tstrncpy(dest, src, util::tstrlen(src)+1);
	}

	tchar* util::tstrncpy(tchar* dest, const tchar* src, size_t n)
	{
		size_t i;

		for (i = 0; i < n && src[i]; i++)
			dest[i] = src[i];

		for (; i < n; i++)
			dest[i] = TCHAR('\0');

		return dest;
	}

	tchar* util::tstrcat(tchar* dest, const tchar* src)
	{
		return util::tstrncat(dest, src, util::tstrlen(src)+1);
	}

	tchar* util::tstrncat(tchar* dest, const tchar* src, size_t n)
	{
	   size_t dest_len = util::tstrlen(dest);
	   size_t i;

	   for (i = 0 ; i < n && src[i]; i++)
		   dest[dest_len + i] = src[i];

	   dest[dest_len + i] = TCHAR('\0');

	   return dest;
	}

	int util::tstrncmp(const tchar* s1, const tchar* s2, size_t n)
	{
		if (n == 0)
			return 0;

		while (*s1 == *s2 && n-- > 0)
		{
			if (*s1 == 0 || n == 0)
				return 0;
			s1++;
			s2++;
		}

		if (n <= 0)
			return 0;

		return (*s1 > *s2) ? 1 : -1;
	}

	int util::tnumchar(const tchar* str1)
	{
		if (sizeof(tchar) == 1)
		{
			return STRLEN(str1);
		}
		else
		{
			int count = 0;

			while (*str1)
			{
				count += (*str1++ < 256) ? 1 : 2;
			}

			return count;
		}
	}

	namespace
	{
		void s_Int2Str(tchar** pp_buffer, int value)
		{
			if (value == 0)
			{
				*((*pp_buffer)++) = TCHAR('0');
			}
			else
			{
				if (value < 0)
				{
					*((*pp_buffer)++) = TCHAR('-');
					value = -value;
				}

				int n_num = 0;
				int temp = value;
				while (temp)
				{
					temp = temp / 10;
					++n_num;
				}

				int n_loop = n_num;
				while (n_loop-- > 0)
				{
					(*pp_buffer)[n_loop] = (value % 10) + TCHAR('0');
					value = value / 10;
				}
				*pp_buffer += n_num;
			}
		}
	}

	void util::tsprintf(tchar p_buffer[], const tchar* sz_format, const int data...)
	{
		if (p_buffer == 0)
			return;

		const int* pData = &data;

		while (*sz_format)
		{
			if (*sz_format != TCHAR('@'))
			{
				*p_buffer++ = *sz_format++;
				continue;
			}
			++sz_format;
			s_Int2Str(&p_buffer, *pData++);
		}

		*p_buffer = 0;
	}
} // namespace kano

