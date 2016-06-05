
#include "kn_sprite_decoder.h"
#include "target/target_dep.h"

#include <vector>
#include <memory>

namespace kano
{

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef TKanoSprite<word>  TKanoSprite16;
typedef TKanoSprite<dword> TKanoSprite32;

namespace _auto
{
	typedef std::auto_ptr<kano::TKanoSprite32> TKanoSprite32;
}

struct CKanoSpriteData::TKanoSpriteDataImpl
{
	/* data structure
	 * The sprite set of a character consist of 20-frame
	 * 0~3: 1 9 3 7 (still)
	 *  ~7: 1 (4-frame)
	 * ~11: 3 (4-frame)
	 * ~15: 9 (4-frame)
	 * ~19: 7 (4-frame)
	 */
	std::vector<TKanoSprite16> sprite_list;
	std::auto_ptr<byte>        buffer;
	int                        buffer_size;

	std::vector<_auto::TKanoSprite32> sprite_list_32;

	TKanoSpriteDataImpl(const char* sz_file_name)
		: buffer_size(0)
	{
		m_LoadDat(sz_file_name);
	}

private:
	void m_LoadDat(const char* sz_file_name)
	{
		target::file_io::CStreamReadFile file(sz_file_name);

		if (!file.IsValidPos())
			return;

		this->buffer_size = file.GetSize();
		this->buffer = std::auto_ptr<byte>(new byte[this->buffer_size]);

		if (this->buffer.get())
		{
			file.Read(this->buffer.get(), this->buffer_size);

			byte* p_buffer     = this->buffer.get();
			byte* p_buffer_end = p_buffer + this->buffer_size;

			while (p_buffer < p_buffer_end)
			{
				static TKanoSprite16 s_null_sprite = { 0, 0, 0 };
		
				if (*p_buffer++ == 255)
				{
					int sprite_size_w = word(p_buffer[0]) | (word(p_buffer[1]) << 8);
					int sprite_size_h = word(p_buffer[2]) | (word(p_buffer[3]) << 8);
					int tile_size     = word(p_buffer[4]) | (word(p_buffer[5]) << 8);

					TKanoSprite16 sprite = { sprite_size_w+1, sprite_size_h+1, (word*)&p_buffer[6] };
					this->sprite_list.push_back(sprite);
					
					p_buffer += (tile_size + 3 * sizeof(word));
				}
				else
				{
					this->sprite_list.push_back(s_null_sprite);
				}
			}
		}

//		this->sprite_list_32.resize(10);//this->sprite_list.size());
	}
};

CKanoSpriteData::CKanoSpriteData(const char* sz_file_name)
	: p_impl(new TKanoSpriteDataImpl(sz_file_name))
{
}

CKanoSpriteData::~CKanoSpriteData()
{
	delete p_impl;
}

const TKanoSprite16& CKanoSpriteData::GetData16(int index)
{
	static TKanoSprite16 s_null_sprite = { 0, 0, 0 };

	if (p_impl == 0 || index < 0 || index >= int(p_impl->sprite_list.size()))
		return s_null_sprite;

	const TKanoSprite16& sprite = p_impl->sprite_list[index];

	return sprite;
}

const TKanoSprite32& CKanoSpriteData::GetData32(int index)
{
	static TKanoSprite32 s_null_sprite = { 0, 0, 0 };

	if (p_impl == 0 || index < 0 || index >= int(p_impl->sprite_list.size()))
		return s_null_sprite;

	const TKanoSprite32& sprite = s_null_sprite;

	return sprite;
}

}
