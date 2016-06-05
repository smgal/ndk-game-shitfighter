
#ifndef __TARGET_DEP_H__
#define __TARGET_DEP_H__

namespace target
{
	////////////////////////////////////////////////////////////////////////////
	// Input

	enum TKey
	{
		KEY_A        = 0x0001,
		KEY_B        = 0x0002,
		KEY_X        = 0x0004,
		KEY_Y        = 0x0008,
		KEY_L        = 0x0010,
		KEY_R        = 0x0020,
		KEY_MENU     = 0x0040,
		KEY_SELECT   = 0x0080,
		KEY_LEFT     = 0x0100,
		KEY_RIGHT    = 0x0200,
		KEY_UP       = 0x0400,
		KEY_DOWN     = 0x0800,
		KEY_VOLUP    = 0x1000,
		KEY_VOLDOWN  = 0x2000,
	};

	struct TInputUpdateInfo
	{
		unsigned long        time_stamp;
		bool                 is_touched;
		struct { int x, y; } touch_pos;
		unsigned long        key_down_flag;
		unsigned long        key_pressed_flag;
	};

	class IInputDevice
	{
	public:
		virtual ~IInputDevice() {}
		virtual  const TInputUpdateInfo& Update(void) = 0;
		virtual  const TInputUpdateInfo& GetUpdateInfo(void) const = 0;
		virtual  bool  CheckKeyDown(TKey key) = 0;
		virtual  bool  CheckKeyPressed(TKey key) = 0;
	};

	IInputDevice* GetInputDevice(void);

	////////////////////////////////////////////////////////////////////////////
	// File I/O

	namespace file_io
	{
		class CStreamRead
		{ 
		public:
			enum TSeek
			{
				SEEK_BEGIN,
				SEEK_CURRENT,
				SEEK_END
			};

			virtual ~CStreamRead(void) { };
				
			virtual long  Read(void* p_buffer, long count) = 0;
			virtual long  Seek(TSeek Origin, long offset) = 0;
			virtual long  GetSize(void) = 0;
			virtual void* GetPointer(void) = 0;
			virtual bool  IsValidPos(void) = 0;
		};

		class CStreamReadFile : public CStreamRead
		{
		public:
			CStreamReadFile(const char* sz_file_name);
			~CStreamReadFile(void);

			long  Read(void* pBuffer, long Count);
			long  Seek(TSeek Origin, long offset);
			long  GetSize(void);
			void* GetPointer(void) { return 0; };
			bool  IsValidPos(void);

		private:
			struct TImpl;
			TImpl* p_impl;
		};
	}

	////////////////////////////////////////////////////////////////////////////
	// System

	namespace system
	{
		unsigned long GetTicks(void);
	}
}

#endif
