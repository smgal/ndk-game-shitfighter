
#ifndef __UKEYBUFFER_H__
#define __UKEYBUFFER_H__

#include "avej_lite.h"

//?? ������� ���� ����
namespace lorebase
{
	extern void OnProcessKey(void);
}

//! Key event�� DOS ���� key buffer �������� ����� �ִ� class
/*!
 * \ingroup AVEJ library utilities
*/
class CKeyBuffer
{
	//! Key�� Ÿ��
	typedef int TKey;

	//! ������ �ִ� queue ũ��
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
	//! CKeyBuffer�� ������
	CKeyBuffer(void);
	//! CKeyBuffer�� �Ҹ���
	~CKeyBuffer(void);

	//! Key�� �������ٴ� ���� �˷��ش�.
	bool SetKeyDown(TKey key)
	{
		if (key < avej_lite::INPUT_KEY_MAX)
			m_keyMap[key] = 1;

		return m_PushKeyChar(key);
	}
	//! Key�� �������ٴ� ���� �˷��ش�.
	bool SetKeyUp(TKey key)
	{
		if (key < avej_lite::INPUT_KEY_MAX)
			m_keyMap[key] = 0;

		return true;
	}
	//! ���� Key buffer�� key�� ���� �ִ��� �˷� �ش�.
	bool IsKeyPressed(void)
	{
		lorebase::OnProcessKey();
		return  (m_keyHeadPtr != m_keyTailPtr);
	}
	//! Key buffer�� ���� �ִ� key�� ���� �ش�.
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
	//! ���� Ư�� Key�� ������ ���������� �Ǻ��Ѵ�.
	bool IsKeyPressing(TKey key)
	{
		if (key >= avej_lite::INPUT_KEY_MAX)
			return false;

		return (m_keyMap[key] > 0);
	}
};

extern CKeyBuffer& GetKeyBuffer(void);

#endif // #ifndef __UKEYBUFFER_H__
