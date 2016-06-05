
#ifndef __PD_CONFIG_H__
#define __PD_CONFIG_H__

//?? 각 presentation 의 특징에 해당되는 것은 그 쪽 config를 만들어 거기에 집어 넣자

////////////////////////////////////////////////////////////////////////////////
// CONFIG begin

#define CONFIG_TYPE_DREAM  1
#define CONFIG_TYPE_KANO   2

////////////////////////////////////////////////////////////////////////////////
// 전체

// text 출력을 위한 texture의 format
#define FONT_TEXTURE_FORMAT  avej_lite::PIXELFORMAT_RGBA4444

////////////////////////////////////////////////////////////////////////////////
// Albireo, world

// floater에게 다가갈 때 색이 변하는 단계
#define APPROACH_STEP_MAX  100

////////////////////////////////////////////////////////////////////////////////
// player

// 움직이는 단위
#define PLAYER_MOVE_RUNNING  4
#define PLAYER_MOVE_WALKING  2

////////////////////////////////////////////////////////////////////////////////
// presentation

// 이 옵션을 켜면 시야에서 없어진 object가 반투명하게 사라진다.
#define USE_SMOOTH_HIDING

// 더 이상 계산의 가치가 없는 거리
#define CONFIG_DISTANCE_NO_WORTH  200

////////////////////////////////////////////////////////////////////////////////
// presentation - map 설정 관련

#define X_MAP_STRIDE (2*X_MAP_RADIUS+1)
#define X_TILE_SIZE  (28)
#define Y_TILE_SIZE  (32)

#define BLOCK_WSIZE  28
#define BLOCK_HSIZE  32

extern int         CONFIG_TYPE;
extern const char* CONFIG_NAME;
extern int         X_MAP_RADIUS;
extern int         Y_MAP_RADIUS;
extern const char* TEXTURE_FILE_NAME1;

////////////////////////////////////////////////////////////////////////////////
// CONFIG end

extern void SetConfig(int config_id);

#endif
