
#include "USound.h"

namespace sound
{

void PlayFx(ESound ixSound)
{
	if ((ixSound < 0) || (ixSound >= SOUND_MAX))
		return;

	//?? index에 해당하는 효과음을 출력

	return;
}

void MuteFx(bool on)
{
	//?? 언젠가는 처리해야 함
}

}
