
////////////////////////////////////////////////////////////////////////////////
// CONFIG begin

#include "pd_config.h"

////////////////////////////////////////////////////////////////////////////////
// CONFIG end

extern int  CONFIG_TYPE        = 0;
const char* CONFIG_NAME        = "";
int         X_MAP_RADIUS       = 0;
int         Y_MAP_RADIUS       = 0;
const char* TEXTURE_FILE_NAME1 = "";

////////////////////////////////////////////////////////////////////////////////
// 

void SetConfig(int config_id)
{
	switch (config_id)
	{
	case CONFIG_TYPE_DREAM:
		CONFIG_TYPE        = CONFIG_TYPE_DREAM;
		CONFIG_NAME        = "Dream";
		X_MAP_RADIUS       = 8;
		Y_MAP_RADIUS       = 5;
		TEXTURE_FILE_NAME1 = "avej_tex.bmp";
		break;
	case CONFIG_TYPE_KANO:
		CONFIG_TYPE        = CONFIG_TYPE_KANO;
		CONFIG_NAME        = "Kano";
		X_MAP_RADIUS       = 7;
		Y_MAP_RADIUS       = 5;
		TEXTURE_FILE_NAME1 = "tex_512_2.tga";
		break;
	default:
		break;
	}
}
