
#include "target_dep.h"
#include <FIo.h>

namespace target
{
	struct file_io::CStreamReadFile::TImpl
	{
		TImpl()
			: file_size(0)
		{
		}
		Osp::Io::File file;
		int           file_size;
	};

	#define IS_AVAILABLE  (p_impl != 0)

	file_io::CStreamReadFile::CStreamReadFile(const char* sz_file_name)
	{
		p_impl = new TImpl;

		if (p_impl)
		{
			try
			{
				result r;
				Osp::Base::String fileName(sz_file_name);

				fileName = "/Res/" + fileName;

				r = p_impl->file.Construct(fileName, L"r");

				if (r == E_SUCCESS)
				{
					p_impl->file_size = this->GetSize();
				}
				else
				{
					AppLog("[SMGAL] File not found (%s)", sz_file_name);

					throw 1;
				}
			}
			catch (...)
			{
				delete p_impl;
				p_impl = 0;
			}
		}
	}

	file_io::CStreamReadFile::~CStreamReadFile(void)
	{
		delete p_impl;
	}

	long file_io::CStreamReadFile::Read(void* p_buffer, long count)
	{
		if (!IS_AVAILABLE)
			return 0;

		return p_impl->file.Read(p_buffer, count);
	}

	long file_io::CStreamReadFile::Seek(TSeek origin, long offset)
	{
		if (!IS_AVAILABLE)
			return -1;

		switch (origin)
		{
		case Osp::Io::FILESEEKPOSITION_BEGIN:
			p_impl->file.Seek(Osp::Io::FileSeekPosition(origin), offset);
			break;
		case Osp::Io::FILESEEKPOSITION_CURRENT:
			p_impl->file.Seek(Osp::Io::FileSeekPosition(origin), offset);
			break;
		case Osp::Io::FILESEEKPOSITION_END:
			p_impl->file.Seek(Osp::Io::FileSeekPosition(origin), offset);
			break;
		default:
			return -1;
		}

		return p_impl->file.Tell();
	}

	long file_io::CStreamReadFile::GetSize(void)
	{
		if (!IS_AVAILABLE)
			return -1;

		long res;
		long current_pos = p_impl->file.Tell();

		p_impl->file.Seek(Osp::Io::FILESEEKPOSITION_END, 0);
		res = p_impl->file.Tell();

		p_impl->file.Seek(Osp::Io::FILESEEKPOSITION_BEGIN, current_pos);

		return res;
	}

	bool file_io::CStreamReadFile::IsValidPos(void)
	{
		if (!IS_AVAILABLE)
			return false;

		return (p_impl->file.Tell() < p_impl->file_size);
	}
}
