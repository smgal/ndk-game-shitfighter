
#ifndef __PD_CONFIG_H__
#define __PD_CONFIG_H__

//?? �� presentation �� Ư¡�� �ش�Ǵ� ���� �� �� config�� ����� �ű⿡ ���� ����

////////////////////////////////////////////////////////////////////////////////
// CONFIG begin

#define CONFIG_TYPE_DREAM  1
#define CONFIG_TYPE_KANO   2

////////////////////////////////////////////////////////////////////////////////
// ��ü

// text ����� ���� texture�� format
#define FONT_TEXTURE_FORMAT  avej_lite::PIXELFORMAT_RGBA4444

////////////////////////////////////////////////////////////////////////////////
// Albireo, world

// floater���� �ٰ��� �� ���� ���ϴ� �ܰ�
#define APPROACH_STEP_MAX  100

////////////////////////////////////////////////////////////////////////////////
// player

// �����̴� ����
#define PLAYER_MOVE_RUNNING  4
#define PLAYER_MOVE_WALKING  2

////////////////////////////////////////////////////////////////////////////////
// presentation

// �� �ɼ��� �Ѹ� �þ߿��� ������ object�� �������ϰ� �������.
#define USE_SMOOTH_HIDING

// �� �̻� ����� ��ġ�� ���� �Ÿ�
#define CONFIG_DISTANCE_NO_WORTH  200

////////////////////////////////////////////////////////////////////////////////
// presentation - map ���� ����

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
