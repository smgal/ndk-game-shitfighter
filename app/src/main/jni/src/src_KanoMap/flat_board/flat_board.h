
#ifndef __FLAT_BOARD_H__
#define __FLAT_BOARD_H__

#include "pixel_format.h"

namespace flat_board
{
	template<typename TBaseType>
	class optional
	{
	public:
		optional()
			: m_initialized(false)
		{
		}
		optional(const TBaseType& t)
			: m_initialized(false)
		{
			reset(t);
		}
		~optional()
		{
			reset();
		}

		TBaseType& operator*()
		{
			// assert(m_initialized);
			return *static_cast<TBaseType*>(address());
		}
		const TBaseType& operator*() const
		{
			// assert(m_initialized);
			return *static_cast<const TBaseType*>(address());
		}
		TBaseType* get()
		{
			// assert(m_initialized);
			return static_cast<TBaseType*>(address());
		}
		const TBaseType* get() const
		{
			// assert(m_initialized);
			return static_cast<const TBaseType*>(address());
		}
		void reset()
		{
			if (m_initialized)
			{
				static_cast<TBaseType*>(address())->TBaseType::~TBaseType();
				m_initialized = false;
			}
		}
		void reset(const TBaseType& t)
		{
			reset();
			new (address()) TBaseType(t);
			m_initialized = true;
		}
		bool valid() const
		{
			return m_initialized;
		}
		bool invalid() const
		{
			return !m_initialized;
		}

	private:
		bool       m_initialized;
		TBaseType  m_storage;

		optional(const optional&);
		optional& operator=(const optional&);

		void* address()
		{
			return &m_storage;
		}
		const void* address() const
		{
			return &m_storage;
		}
	};

	////////////////////////////////////////////////////////////////////////////

	template <TPixelFormat pixel_format>
	struct TPixelFormatTraits
	{
	};

	template <>
	struct TPixelFormatTraits<PIXELFORMAT_RGB565>
	{
		typedef unsigned short TPixel;
		struct TAnnex {};
		static TPixel ConvertColor(unsigned long color32bits)
		{
			return TPixel((color32bits & 0x00F80000) >> 8 |
			              (color32bits & 0x0000FC00) >> 5 |
			              (color32bits & 0x000000F8) >> 3);
		}
	};

	template <>
	struct TPixelFormatTraits<PIXELFORMAT_ARGB8888>
	{
		typedef unsigned long TPixel;
		struct TAnnex {};
		static TPixel ConvertColor(unsigned long color32bits)
		{
			return color32bits;
		}
	};

	template <TPixelFormat pixel_format>
	struct TPixelInfo
	{
		typedef typename TPixelFormatTraits<pixel_format>::TPixel TPixel;
		typedef typename TPixelFormatTraits<pixel_format>::TAnnex TAnnex;
		static TPixel ConvertColor(unsigned long color32bits)
		{
			return TPixelFormatTraits<pixel_format>::ConvertColor(color32bits);
		}
	};

	struct TBufferDesc
	{
		TPixelFormat  pixel_format;
		int           width;
		int           height;
		int           depth;
		bool          has_color_key;
		unsigned long color_key;
	};

	struct TLockDesc
	{
		void*         p_buffer;
		long          bytes_per_line;
	};

	template <TPixelFormat pixel_format>
	class CFlatBoard
	{
	public:
		typedef typename TPixelInfo<pixel_format>::TPixel TPixel;

		typedef void (*TFnFillRect)(TPixel* p_dest, int w, int h, int dest_pitch, TPixel color, unsigned long alpha);
		typedef void (*TFnBitBlt)(TPixel* p_dest, int w, int h, int dest_pitch, TPixel* p_sour, int sour_pitch, unsigned long alpha, const optional<TPixel>& chroma_key);

		CFlatBoard()
			: m_fn_fill_rect(m_FillRect), m_fn_bit_blt(m_BitBlt)
		{
			m_buffer_desc.pixel_format  = PIXELFORMAT_INVALID;
			m_buffer_desc.width         = 0;
			m_buffer_desc.height        = 0;
			m_buffer_desc.depth         = 0;
			m_buffer_desc.has_color_key = false;
			m_buffer_desc.color_key     = 0;

			m_lock_desc.p_buffer        = 0;
			m_lock_desc.bytes_per_line  = 0;
		}

		CFlatBoard(TPixel* p_buffer, int width, int height, int pitch)
			: m_fn_fill_rect(m_FillRect), m_fn_bit_blt(m_BitBlt)
		{
			m_buffer_desc.pixel_format  = pixel_format;
			m_buffer_desc.width         = width;
			m_buffer_desc.height        = height;
			m_buffer_desc.depth         = sizeof(TPixel) * 8;
			m_buffer_desc.has_color_key = false;
			m_buffer_desc.color_key     = 0;

			m_lock_desc.p_buffer        = (void*)p_buffer;
			m_lock_desc.bytes_per_line  = pitch * sizeof(TPixel);
		}

		CFlatBoard(TPixel* p_buffer, int width, int height, int pitch, TPixel chroma_key)
			: m_chroma_key(chroma_key), m_fn_fill_rect(m_FillRect), m_fn_bit_blt(m_BitBlt)
		{
			m_buffer_desc.pixel_format  = pixel_format;
			m_buffer_desc.width         = width;
			m_buffer_desc.height        = height;
			m_buffer_desc.depth         = sizeof(TPixel) * 8;
			m_buffer_desc.has_color_key = true;
			m_buffer_desc.color_key     = chroma_key;

			m_lock_desc.p_buffer        = (void*)p_buffer;
			m_lock_desc.bytes_per_line  = pitch * sizeof(TPixel);
		}

		void FillRect(int x, int y, int w, int h, TPixel color, unsigned long alpha = 255)
		{
			if (m_lock_desc.p_buffer == 0)
				return;

			if (!m_CropInBoundary(x, y, w, h))
				return;

			if (m_chroma_key.valid())
			{
				if (*m_chroma_key == color)
					color = (color) ? (color-1) : (color+1);
			}

			m_fn_fill_rect(m_BufferPointerAt(x, y), w, h, m_lock_desc.bytes_per_line / sizeof(TPixel), color, alpha);
		}

		void BitBlt(int x_dest, int y_dest, CFlatBoard* p_src_flat_board, int x_sour, int y_sour, int w_sour, int h_sour, unsigned long alpha = 255)
		{
			if (m_lock_desc.p_buffer == 0)
				return;

			if (p_src_flat_board == 0 || p_src_flat_board->m_lock_desc.p_buffer == 0)
				return;

			{
				int x = x_sour;
				int y = y_sour;

				if (!p_src_flat_board->m_CropInBoundary(x_sour, y_sour, w_sour, h_sour))
					return;

				x_dest += (x_sour - x);
				y_dest += (y_sour - y);
			}

			{
				int x = x_dest;
				int y = y_dest;

				if (!m_CropInBoundary(x_dest, y_dest, w_sour, h_sour))
					return;

				x_sour += (x_dest - x);
				y_sour += (y_dest - y);
			}

			m_fn_bit_blt(m_BufferPointerAt(x_dest, y_dest), w_sour, h_sour, m_lock_desc.bytes_per_line / sizeof(TPixel), p_src_flat_board->m_BufferPointerAt(x_sour, y_sour), p_src_flat_board->m_lock_desc.bytes_per_line * 8 / p_src_flat_board->m_buffer_desc.depth, alpha, p_src_flat_board->m_chroma_key);
		}

		TBufferDesc GetBufferDesc(void) const
		{
			return m_buffer_desc;
		}

		CFlatBoard<pixel_format>& operator<<(TFnFillRect fn_fill_rect)
		{
			m_fn_fill_rect = (fn_fill_rect) ? fn_fill_rect : m_FillRect;
			return *this;
		}

		CFlatBoard<pixel_format>& operator<<(TFnBitBlt fn_bit_blt)
		{
			m_fn_bit_blt = (fn_bit_blt) ? fn_bit_blt : m_BitBlt;
			return *this;
		}

		inline TFnBitBlt DefaultFillRect()
		{
			return 0;
		}

		inline TFnBitBlt DefaultBitBlt()
		{
			return 0;
		}

		void SetChromaKey(TPixel chroma_key)
		{
			m_buffer_desc.has_color_key = true;
			m_buffer_desc.color_key     = chroma_key;

			m_chroma_key.reset(chroma_key);
		}

		void ResetChromaKey(void)
		{
			m_buffer_desc.has_color_key = false;
			m_buffer_desc.color_key     = 0;

			m_chroma_key.reset();
		}

	private:
		TBufferDesc      m_buffer_desc;
		TLockDesc        m_lock_desc;

		optional<TPixel> m_chroma_key;
		TFnFillRect      m_fn_fill_rect;
		TFnBitBlt        m_fn_bit_blt;

		bool m_CropInBoundary(int& x1, int& y1, int& w, int& h)
		{
			int x2 = x1 + w;
			int y2 = y1 + h;

			if (x1 < 0)
				x1 = 0;
			if (y1 < 0)
				y1 = 0;

			if (x2 > m_buffer_desc.width)
				x2 = m_buffer_desc.width;
			if (y2 > m_buffer_desc.height)
				y2 = m_buffer_desc.height;

			w = x2 - x1;
			h = y2 - y1;

			return (w >= 0) && (h >= 0);
		}

		inline TPixel* m_BufferPointerAt(int x, int y) const
		{
			return reinterpret_cast<TPixel*>((unsigned char*)m_lock_desc.p_buffer + y * m_lock_desc.bytes_per_line + x * sizeof(TPixel));
		}

		static void m_FillRect(TPixel* p_dest, int w, int h, int dest_pitch, TPixel color, unsigned long alpha);
		static void m_BitBlt(TPixel* p_dest, int w, int h, int dest_pitch, TPixel* p_sour, int sour_pitch, unsigned long alpha, const optional<TPixel>& chroma_key);
	};

	template <>
	void CFlatBoard<PIXELFORMAT_ARGB8888>::m_FillRect(unsigned long* p_dest_32, int w, int h, int dest_pitch, unsigned long color, unsigned long alpha)
	{
		alpha += (alpha >> 7);
		alpha  = ((alpha * (color >> 24)) << 16) & 0xFF000000;
		color &= 0x00FFFFFF;

		while (--h >= 0)
		{
			int copy = w;
			while (--copy >= 0)
				*p_dest_32++ = TPixel((color & 0x00FFFFFF) | alpha);

			p_dest_32 += (dest_pitch - w);
		}
	}

	template <>
	void CFlatBoard<PIXELFORMAT_ARGB8888>::m_BitBlt(unsigned long* p_dest_32, int w, int h, int dest_pitch, unsigned long* p_sour_32, int sour_pitch, unsigned long alpha, const optional<unsigned long>& chroma_key)
	{
		if (chroma_key.valid())
		{
			unsigned long color_key = *chroma_key;

			alpha  += (alpha >> 7);
			alpha <<= 16;

			TPixel gray;

			while (--h >= 0)
			{
				for (int copy = w; --copy >= 0; ++p_dest_32, ++p_sour_32)
				{
					if (*p_sour_32 != color_key)
					{
						gray = TPixel(alpha * (*p_sour_32 >> 24));
						*p_dest_32 = (*p_sour_32 & 0x00FFFFFF) | (gray & 0xFF000000);
					}
				}
				p_dest_32 += (dest_pitch - w);
				p_sour_32 += (sour_pitch - w);
			}
		}
		else
		{
			alpha  += (alpha >> 7);
			alpha <<= 16;

			TPixel gray;

			while (--h >= 0)
			{
				int copy = w;
				while (--copy >= 0)
				{
					gray = TPixel(alpha * (*p_sour_32 >> 24));
					*p_dest_32++ = (*p_sour_32++ & 0x00FFFFFF) | (gray & 0xFF000000);
				}
				p_dest_32 += (dest_pitch - w);
				p_sour_32 += (sour_pitch - w);
			}
		}
	}

	template <>
	void CFlatBoard<PIXELFORMAT_RGB565>::m_FillRect(unsigned short* p_dest_16, int w, int h, int dest_pitch, unsigned short color, unsigned long alpha)
	{
		unsigned short color16bit = (unsigned short)color;

		while (--h >= 0)
		{
			int copy = w;
			while (--copy >= 0)
				*p_dest_16++ = color16bit;

			p_dest_16 += (dest_pitch - w);
		}
	}

	template <>
	void CFlatBoard<PIXELFORMAT_RGB565>::m_BitBlt(unsigned short* p_dest_16, int w, int h, int dest_pitch, unsigned short* p_sour_16, int sour_pitch, unsigned long alpha, const optional<unsigned short>& chroma_key)
	{
		if (chroma_key.valid())
		{
			unsigned short color_key = *chroma_key;

			while (--h >= 0)
			{
				for (int copy = w; --copy >= 0; ++p_dest_16, ++p_sour_16)
				{
					if (*p_sour_16 != color_key)
						*p_dest_16 = *p_sour_16;
				}
				p_dest_16 += (dest_pitch - w);
				p_sour_16 += (sour_pitch - w);
			}
		}
		else
		{
			while (--h >= 0)
			{
				int copy = w;
				while (--copy >= 0)
				{
					*p_dest_16++ = *p_sour_16++;
				}
				p_dest_16 += (dest_pitch - w);
				p_sour_16 += (sour_pitch - w);
			}
		}
	}
}

#endif // #ifndef __FLAT_BOARD_H__
