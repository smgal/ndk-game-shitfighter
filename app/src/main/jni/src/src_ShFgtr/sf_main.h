
#ifndef __SF_MAIN_H__
#define __SF_MAIN_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include "avej_lite.h"

////////////////////////////////////////////////////////////////////////////////
// state

enum TState
{
	STATE_TITLE = 0,
	STATE_MENU_ABOUT,
	STATE_MENU_STORY,
	STATE_MENU_OPTION,
	STATE_GAME_PLAY,
	STATE_ENDING_BAD,
	STATE_ENDING_HAPPY,
	STATE_EXIT
};

extern void g_ChangeState(TState state);

#endif // #ifndef __SF_MAIN_H__
