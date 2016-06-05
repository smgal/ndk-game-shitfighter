
#ifndef __USOUND_H__
#define __USOUND_H__

namespace sound
{
	enum ESound
	{
		SOUND_HIT   = 0,
		SOUND_SCREAM1,
		SOUND_SCREAM2,
		SOUND_MAX
	};

	void PlayFx(ESound ixSound);
	void MuteFx(bool on);
}

#endif // #ifndef __USOUND_H__
