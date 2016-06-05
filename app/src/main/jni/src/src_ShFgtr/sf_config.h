
/*
	목표

	- 전역 변수와 관련된 것을 제거
	- standard lib 과 관련된 include 제거
		특히 file I/O와 STL 등
	- Event driven 구조에 맞는 형식으로 app frame 설계
*/


#ifndef __SF_CONFIG_H__
#define __SF_CONFIG_H__

////////////////////////////////////////////////////////////////////////////////
// type

typedef unsigned char  byte;
typedef unsigned char  TImage;

////////////////////////////////////////////////////////////////////////////////
// constant

#define TILE_X_SIZE    48
#define TILE_Y_SIZE    48

#define SCREEN_X_MAX   640
#define SCREEN_Y_MAX   400

#define VIEWPORT_X1    0
#define VIEWPORT_Y1    0
#define VIEWPORT_X2    SCREEN_X_MAX
#define VIEWPORT_Y2    399

#define FOOT_STEP      6
#define Y_DOWN         17

#define OBJECTS_No     100
#define FRIEND_No      2
#define ENEMY_No       10

#define MIN_OBJECTS    1
#define MAX_OBJECTS    MIN_OBJECTS + (OBJECTS_No-1)
#define MIN_FRIEND     (MAX_OBJECTS+1)
#define MAX_FRIEND     MIN_FRIEND + (FRIEND_No-1)
#define MIN_ENEMY      (MAX_FRIEND+1)
#define MAX_ENEMY      MIN_ENEMY + (ENEMY_No-1)

#define MAX_LEVEL      3

#define GLYPH_W        12
#define GLYPH_H        24

#if (TARGET_DEVICE == TARGET_BADA)
#define SCROLL_SPEED          4
#else
#define SCROLL_SPEED          2
#endif

#define SCROLL_DELAY_CALC(a)  int(8.0f * (a) / SCROLL_SPEED + 0.5f)
#define SCROLL_SPEED_CALC(a)  ((a) * 1.0f * SCROLL_SPEED / 8.0f)


#define STORY_FILE_NAME "stroy.dat"

#endif // #ifndef __SF_CONFIG_H__
