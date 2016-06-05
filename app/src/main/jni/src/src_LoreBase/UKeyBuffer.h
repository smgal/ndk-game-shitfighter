
#ifndef __UKEYBUFFER_H__
#define __UKEYBUFFER_H__

#include "avej_lite.h"

//?? 디펜던시 문제 있음
namespace lorebase
{
	extern void OnProcessKey(void);
}

//! Key event를 DOS 때의 key buffer 형식으로 만들어 주는 class
/*!
 * \ingroup AVEJ library utilities
*/
class CKeyBuffer
{
	//! Key의 타입
	typedef int TKey;

	//! 버퍼의 최대 queue 크기
	enum
	{
		MAX_KEY_BUFFER = 100
	};

	unsigned long m_keyHeadPtr;
	unsigned long m_keyTailPtr;
	TKey          m_KeyBuffer[MAX_KEY_BUFFER];
	unsigned long m_keyMap[avej_lite::INPUT_KEY_MAX];

	unsigned long m_IncPtr(unsigned long ptr)
	{
		if (++ptr >= 100)
			ptr -= 100;

		return ptr;
	}

	bool  m_PushKeyChar(TKey key)
	{
		if (m_IncPtr(m_keyTailPtr) != m_keyHeadPtr)
		{
			m_KeyBuffer[m_keyTailPtr] = key;
			m_keyTailPtr = m_IncPtr(m_keyTailPtr);
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	//! CKeyBuffer의 생성자
	CKeyBuffer(void);
	//! CKeyBuffer의 소멸자
	~CKeyBuffer(void);

	//! Key가 눌려졌다는 것을 알려준다.
	bool SetKeyDown(TKey key)
	{
		if (key < avej_lite::INPUT_KEY_MAX)
			m_keyMap[key] = 1;

		return m_PushKeyChar(key);
	}
	//! Key가 떨어졌다는 것을 알려준다.
	bool SetKeyUp(TKey key)
	{
		if (key < avej_lite::INPUT_KEY_MAX)
			m_keyMap[key] = 0;

		return true;
	}
	//! 현재 Key buffer에 key가 남아 있는지 알려 준다.
	bool IsKeyPressed(void)
	{
		lorebase::OnProcessKey();
		return  (m_keyHeadPtr != m_keyTailPtr);
	}
	//! Key buffer에 남아 있는 key를 돌려 준다.
	TKey GetKey()
	{
		TKey key = -1;

		if (IsKeyPressed())
		{
			key = m_KeyBuffer[m_keyHeadPtr];
			m_keyHeadPtr = m_IncPtr(m_keyHeadPtr);
		}

		return key;
	}
	//! 현재 특정 Key가 눌려진 상태인지를 판별한다.
	bool IsKeyPressing(TKey key)
	{
		if (key >= avej_lite::INPUT_KEY_MAX)
			return false;

		return (m_keyMap[key] > 0);
	}
};

extern CKeyBuffer& GetKeyBuffer(void);

#endif // #ifndef __UKEYBUFFER_H__
