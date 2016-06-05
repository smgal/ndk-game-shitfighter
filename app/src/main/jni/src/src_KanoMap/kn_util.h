
#ifndef __SFWV_UTIL_H__
#define __SFWV_UTIL_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <stdlib.h>

typedef char     tchar;
#define TCHAR(s) s

////////////////////////////////////////////////////////////////////////////////
// definition

#define STRLEN     util::tstrlen
#define STRCPY     util::tstrcpy
#define STRCAT     util::tstrcat
#define STRNCPY    util::tstrncpy
#define STRNCAT    util::tstrncat
#define STRNCMP    util::tstrncmp
#define NUMCHAR    util::tnumchar
#define SPRINTF    util::tsprintf
#define STD_STRING util::tstring

namespace kano
{
	namespace util
	{
		int    tstrlen(const tchar* str1);
		tchar* tstrcpy(tchar* dest, const tchar* src);
		tchar* tstrncpy(tchar* dest, const tchar* src, size_t n);
		tchar* tstrcat(tchar* dest, const tchar* src);
		tchar* tstrncat(tchar* dest, const tchar* src, size_t n);
		int    tstrncmp(const tchar* s1, const tchar* s2, size_t n);
		int    tnumchar(const tchar* str1);
		void   tsprintf(tchar p_buffer[], const tchar* sz_format, const int data...);

		class tstring
		{
			enum { MAX_LEN = 256 };

		private:
			tchar m_string[MAX_LEN+1];

		public:
			tstring(void)
			{
				m_string[0] = 0;
			}
			tstring(const tchar* lpsz)
			{
				tstrncpy(m_string, lpsz, MAX_LEN);
			}

			operator const tchar*() const
			{
				return m_string;
			};

			const tchar* c_str() const
			{
				return m_string;
			};

			const tstring& operator=(const tchar* lpsz)
			{
				tstrncpy(m_string, lpsz, MAX_LEN);
				return *this;
			}

			const tstring& operator+=(const tchar* lpsz)
			{
				tstrncat(m_string, lpsz, MAX_LEN);
				return *this;
			}

			void copy_to_front(const tstring& lpsz)
			{
				tchar m_temp[MAX_LEN+1];
				tstrncpy(m_temp, m_string, MAX_LEN);
				tstrncpy(m_string, lpsz.m_string, MAX_LEN);
				tstrncat(m_string, m_temp, MAX_LEN);
			}

			bool is_empty(void) const
			{
				return (m_string[0] == 0);
			}
		};
	}
} // namespace miku

#endif // #ifndef __SFWV_UTIL_H__

