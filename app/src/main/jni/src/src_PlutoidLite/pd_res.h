
#ifndef __PD_RES_H__
#define __PD_RES_H__

namespace resource
{
	struct TResStream
	{
		unsigned char* p_stream;
		unsigned int   stream_length;
	};

	bool GetResouceNameToStream(const char* sz_res_name, TResStream& res_stream);
}

#endif
