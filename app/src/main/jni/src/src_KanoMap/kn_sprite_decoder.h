
#ifndef __KN_SPRITE_DECODER_H__
#define __KN_SPRITE_DECODER_H__

namespace kano
{
	template <typename TPixel>
	struct TKanoSprite
	{
		int     width;
		int     height;
		TPixel* p_buffer;
	};

	class CKanoSpriteData
	{
	public:
		CKanoSpriteData()
			: p_impl(0) {}

		CKanoSpriteData(const char* sz_file_name);
		~CKanoSpriteData();

		const TKanoSprite<unsigned short>& GetData16(int index);
		const TKanoSprite<unsigned long>&  GetData32(int index);

	private:
		struct TKanoSpriteDataImpl;
		TKanoSpriteDataImpl* p_impl;
	};
}

#endif // #ifndef __KN_SPRITE_DECODER_H__
