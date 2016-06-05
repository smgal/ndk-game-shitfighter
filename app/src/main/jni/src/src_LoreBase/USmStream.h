
#ifndef __USMSTREAM_H__
#define __USMSTREAM_H__

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// CReadStream class

class CReadStream
{ 
public:
	CReadStream(void): m_isAvailable(false) { };
	virtual ~CReadStream(void) { };

	bool          IsVaild(void) const { return m_isAvailable; };
	virtual long  Read(void* pBuffer, long Count) const = 0;
	virtual long  Seek(long Offset, unsigned short Origin) const = 0;
	virtual long  GetSize(void) const = 0;
	virtual void* GetPointer(void) = 0;
	virtual bool  IsValidPos(void) const = 0;

protected:
	bool m_isAvailable;
};

class CFileReadStream : public CReadStream
{
public:
	CFileReadStream(const char* szFileName);
	~CFileReadStream(void);

private:
	FILE* m_pFile;

public:
	long  Read(void* pBuffer, long Count) const;
	long  Seek(long Offset, unsigned short Origin) const;
	long  GetSize(void) const;
	void* GetPointer(void) { return NULL; };
	bool  IsValidPos(void) const;
};

class CMemoryReadStream : public CReadStream
{
public:
	CMemoryReadStream(void* pMemory, long size);
	~CMemoryReadStream(void) {;};

private:
	void* m_pMemory;
	long  m_Size;
	mutable long  m_Position;

public:
	long  Read(void* pBuffer, long Count) const;
	long  Seek(long Offset, unsigned short Origin) const;
	long  GetSize(void) const;
	void* GetPointer(void);
	bool  IsValidPos(void) const;
};

////////////////////////////////////////////////////////////////////////////////
// CWriteStream class

class CWriteStream
{ 
public:
	CWriteStream(void): m_isAvailable(false) { };
	virtual ~CWriteStream(void) { ; };

	bool          IsVaild(void) const { return m_isAvailable; };
	virtual long  Write(void* pBuffer, long Count) const = 0;

protected:
	bool m_isAvailable;
};

class CFileWriteStream : public CWriteStream
{
public:
	CFileWriteStream(const char* szFileName);
	~CFileWriteStream(void);

private:
	FILE* m_pFile;

public:
	long  Write(void* pBuffer, long Count) const;
};

#endif // #ifndef __USMSTREAM_H__
