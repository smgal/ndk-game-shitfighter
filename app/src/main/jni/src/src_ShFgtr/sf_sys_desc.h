
#ifndef __SF_SYS_DESC_H__
#define __SF_SYS_DESC_H__

#include "avej_lite.h"

enum TPlayerMode
{
	PLAYERMODE_PLAYER1_ONLY,
	PLAYERMODE_PLAYER2_ONLY,
	PLAYERMODE_DOUBLE_MODE1,
	PLAYERMODE_DOUBLE_MODE2
};

enum TLanguage
{
	LANGUAGE_KOREAN,
	LANGUAGE_ENGLISH
};

struct TSysDesc
{
	TLanguage language;
	bool      use_sound;

	struct TStage
	{
		struct TMesaage
		{
			int remained_ticks;

			TMesaage();

			void        Set(const char* sz_message);
			const char* Get(void) const;

		private:
			char stage_message[32];

		} message;

		unsigned int current_stage;
		bool         has_cleared;

		TStage()
			: current_stage(0), has_cleared(false)
		{
		}
	} stage;

	TPlayerMode GetPlayerMode(void) const;
	void        SetPlayerMode(TPlayerMode player_mode);

	bool        IsAutoShot(void) const;

private:
	THIS_CLASS_IS_A_SINGLETON(TSysDesc);
};

const TSysDesc& GetSysDesc(void);
      TSysDesc& SetSysDesc(void);

#endif // #ifndef __SF_SYS_DESC_H__
