
#include "USmStream.h"
#include <string.h>

/***************************************************
                   CFileReadStream
***************************************************/

CFileReadStream::CFileReadStream(const char* szFileName)
{
	m_pFile = fopen(szFileName, "rb");

	this->m_isAvailable = (m_pFile != 0);
}

CFileReadStream::~CFileReadStream(void)
{
	if (this->m_isAvailable)
		fclose(m_pFile);
}

long CFileReadStream::Read(void* pBuffer, long count) const
{
	if (!this->m_isAvailable)
		return 0;

	return fread(pBuffer, 1, count, m_pFile);
}

long CFileReadStream::Seek(long Offset, unsigned short Origin) const
{
	if (!this->m_isAvailable)
		return -1;

	switch (Origin)
	{
	case SEEK_SET:
	case SEEK_CUR:
	case SEEK_END:
		fseek(m_pFile, Offset, Origin);
		break;
	default:
		return -1;
	}

	return ftell(m_pFile);
}

long CFileReadStream::GetSize(void) const
{
	if (!this->m_isAvailable)
		return -1;

	long  Result;
	long  CurrentPos = ftell(m_pFile);

	fseek(m_pFile, 0, SEEK_END);
	Result = ftell(m_pFile);

	fseek(m_pFile, CurrentPos, SEEK_SET);

	return Result;
}

bool CFileReadStream::IsValidPos(void) const
{
	if (!this->m_isAvailable)
		return false;

	return (feof(m_pFile) == 0);
}

/***************************************************
                   CMemoryReadStream
***************************************************/

CMemoryReadStream::CMemoryReadStream(void* pMemory, long size)
{
	m_pMemory   = pMemory;
	m_Size      = size;
	m_Position  = 0;
	m_isAvailable = (m_pMemory != NULL);
}

long CMemoryReadStream::Read(void* pBuffer, long count) const
{
	if (!this->m_isAvailable)
		return -1;

	long  Result;

	if ((m_Position >= 0) && (count >= 0 ))
	{
		Result = m_Size - m_Position;
		if (Result > 0)
		{
			if (Result > count)
				Result = count;

			memcpy(pBuffer, (char*)m_pMemory + m_Position, Result);

			m_Position += Result;

			return Result;
		}
	}

	return 0;
}

long CMemoryReadStream::Seek(long Offset, unsigned short Origin) const
{
	if (!this->m_isAvailable)
		return -1;

	switch (Origin)
	{
	case SEEK_SET:
		m_Position = Offset;
		break;
	case SEEK_CUR:
		m_Position += Offset;
		break;
	case SEEK_END:
		m_Position = m_Size + Offset;
		break;
	default:
		return -1;
	}

	// 특별한 범위 체크 안함
	return m_Position;
}

long CMemoryReadStream::GetSize(void) const
{
	if (!this->m_isAvailable)
		return -1;

	return m_Size;
}

void *CMemoryReadStream::GetPointer(void)
{
	if (!this->m_isAvailable)
		return NULL;

	return m_pMemory;
}

bool CMemoryReadStream::IsValidPos(void) const
{
	if (!this->m_isAvailable)
		return false;

	return ((m_Position >= 0) && (m_Position < m_Size));
}

/***************************************************
                   CFileWriteStream
***************************************************/

CFileWriteStream::CFileWriteStream(const char* szFileName)
{
	m_pFile = fopen(szFileName, "wb");

	this->m_isAvailable = (m_pFile != 0);
}

CFileWriteStream::~CFileWriteStream(void)
{
	if (this->m_isAvailable)
		fclose(m_pFile);
}

long CFileWriteStream::Write(void* pBuffer, long count) const
{
	if (!this->m_isAvailable)
		return 0;

	return fwrite(pBuffer, 1, count, m_pFile);
}
