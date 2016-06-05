
#ifndef __USERIALIZE_H__
#define __USERIALIZE_H__

#include "USmStream.h"

class CSerialize
{
	friend  class CSerializeStream;

protected:
	virtual bool _Load(const CReadStream& stream) = 0;
	virtual bool _Save(const CWriteStream& stream) const = 0;
};

class CSerializeStream
{
public:
	enum EStreamType
	{
		STREAM_READ,
		STREAM_WRITE,
	};

	CSerializeStream(const char* szFileName, EStreamType streamType)
		: pReadStream(NULL), pWriteStream(NULL)
	{
		switch (streamType)
		{
		case STREAM_READ:
			pReadStream = new CFileReadStream(szFileName);
			break;
		case STREAM_WRITE:
			pWriteStream = new CFileWriteStream(szFileName);
			break;
		}
	}

	~CSerializeStream(void)
	{
		delete pReadStream;
		delete pWriteStream;
	}

	virtual void operator<<(const CSerialize& stream)
	{
		if (pWriteStream)
			stream._Save(*pWriteStream);
	}
	virtual void operator>>(CSerialize& stream)
	{
		if (pReadStream)
			stream._Load(*pReadStream);
	}

private:
	CReadStream*  pReadStream;
	CWriteStream* pWriteStream;
};

#endif // #ifndef __USERIALIZE_H__
