
#include "avej_lite.h"
#include "UConfig.h"

#if   (SCREEN_WIDTH == 480)
	#define CONFIG_NUM 1
#elif (SCREEN_WIDTH == 320)
	#define CONFIG_NUM 2
//?? 임시
#elif (SCREEN_WIDTH == 800)
	#define CONFIG_NUM 2
#endif

#if !defined(CONFIG_NUM)
	#error cannot support current platform
#endif

#define TILE_X_SIZE 24
#define TILE_Y_SIZE 24
#define MAP_X_SIZE  (TILE_X_SIZE*c_wMapRange)
#define MAP_Y_SIZE  (TILE_Y_SIZE*c_hMapRange)

namespace config
{

#if (CONFIG_NUM == 0)

	// 실제로 사용되지는 않는 디폴트 값
	const int c_wMapRange = 9;
	const int c_hMapRange = 9;

#elif (CONFIG_NUM == 1)

	const int c_wMapRange = 9;
	const int c_hMapRange = 9;

#elif (CONFIG_NUM == 2)

	const int c_wMapRange = 6;
	const int c_hMapRange = 7;

#else

	#error cannot support current platform

#endif

	const TRect c_mapWindowRegion     = {0, 0, MAP_X_SIZE, MAP_Y_SIZE-18};
	const TRect c_consoleWindowRegion = {MAP_X_SIZE, 0, SCREEN_WIDTH-MAP_X_SIZE, MAP_Y_SIZE-18};
	const TRect c_statusWindowRegion  = {0, c_mapWindowRegion.h, SCREEN_WIDTH, SCREEN_HEIGHT-c_mapWindowRegion.h};

	const int c_wFont = 6;
	const int c_hFont = 12;
}
