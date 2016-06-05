
#include "UResString.h"
#include "USmSet.h"

////////////////////////////////////////////////////////////////////////////////
// 게임의 기본 속성 문자열 (player, weapon 등)

#define RETURN_HAN_STRING(josa_set, default_has_jongsung) \
	THanString hanStr; \
\
	if ((index >= 0) && (index < sizeof(c_NAME)/sizeof(c_NAME[0]))) \
		hanStr.szName = c_NAME[index]; \
	else \
		hanStr.szName = c_NAME_UNKOWN; \
\
	static CSmSet s_josa(josa_set); \
\
	if (s_josa.IsSet(index) || ((hanStr.szName == c_NAME_UNKOWN) && default_has_jongsung)) \
	{ \
		hanStr.szJosaSub1 = "은"; \
		hanStr.szJosaSub2 = "이"; \
		hanStr.szJosaObj  = "을"; \
		hanStr.szJosaWith = "으"; \
	} \
	else \
	{ \
		hanStr.szJosaSub1 = "는"; \
		hanStr.szJosaSub2 = "가"; \
		hanStr.szJosaObj  = "를"; \
		hanStr.szJosaWith = ""; \
	} \
\
	return hanStr;


namespace resource
{

THanString GetWeaponName(int index)
{
	const char* c_NAME_UNKOWN = "불확실한 무기";
	const char* c_NAME[] =
	{
		"맨손",
		"단도",
		"곤봉",
		"미늘창",
		"장검",
		"철퇴",
		"기병창",
		"도끼창",
		"삼지창",
		"화염검",
	};

	RETURN_HAN_STRING("0,2..4,6..9", false)
}

THanString GetShieldName(int index)
{
	const char* c_NAME_UNKOWN = "불확실한 방패";
	const char* c_NAME[] =
	{
		"없음",
		"가죽 방패",
		"청동 방패",
		"강철 방패",
		"은제 방패",
		"금제 방패",
	};

	RETURN_HAN_STRING("0", false)
}

THanString GetArmorName(int index)
{
	const char* c_NAME_UNKOWN = "불확실한 갑옷";
	const char* c_NAME[] =
	{
		"없음",
		"가죽 갑옷",
		"청동 갑옷",
		"강철 갑옷",
		"은제 갑옷",
		"금제 갑옷",
	};

	RETURN_HAN_STRING("0..5", true)
}

THanString Get3rdPersonName(int index)
{
	const char* c_NAME_UNKOWN = "그";
	const char* c_NAME[] =
	{
		"그",
		"그녀",
	};

	RETURN_HAN_STRING("", false)
}

THanString GetGenderName(int index)
{
	const char* c_NAME_UNKOWN = "불확실함";
	const char* c_NAME[] =
	{
		"남성",
		"여성",
	};

	RETURN_HAN_STRING("0, 1", true)
}

THanString GetConditionName(ECondition index)
{
	const char* c_NAME_UNKOWN = "이상상태";
	const char* c_NAME[] =
	{
		"good",
		"poisoned",
		"unconscious",
		"dead",
	};

	RETURN_HAN_STRING("", false);
}

THanString GetClassName(int index)
{
	const char* c_NAME_UNKOWN = "불확실함";
	const char* c_NAME[] =
	{
		"불확실함",
		"기사",
		"마법사",
		"에스퍼",
		"전사",
		"전투승",
		"닌자",
		"사냥꾼",
		"떠돌이",
		"혼령",
		"반신반인",
	};

	RETURN_HAN_STRING("0,5,7,9,10", true)
}

THanString GetMagicName(int index)
{
	const char* c_NAME_UNKOWN = "";
	const char* c_NAME[46] =
	{
		"",
		"마법 화살",
		"마법 화구",
		"마법 단창",
		"독 바늘",
		"맥동 광선",
		"직격 뇌전",
		"공기 폭풍",
		"열선 파동",
		"초음파",
		// 10
		"초냉기",
		"인공 지진",
		"차원 이탈",
		"독",
		"기술 무력화",
		"방어 무력화",
		"능력 저하",
		"마법 불능",
		"탈 초인화",
		"한명 치료",
		"한명 독 제거",
		"한명 치료와 독제거",
		"한명 의식 돌림",
		"한명 부활",
		"한명 치료와 독제거와 의식돌림",
		"한명 복합 치료",
		"모두 치료",
		"모두 독 제거",
		"모두 치료와 독제거",
		"모두 의식 돌림",
		"모두 치료와 독제거와 의식돌림",
		"모두 부활",
		"모두 복합 치료",
		"마법의 햇불",
		"공중 부상",
		"물위를 걸음",
		"늪위를 걸음",
		"기화 이동",
		"지형 변화",
		"공간 이동",
		"식량 제조",
		"투시",
		"예언",
		"독심",
		"천리안",
		"염력",
	};

	RETURN_HAN_STRING("1,3..8,11..13,17,22..24,29..31,33..37,39,42..45", false)
}

THanString GetAuxName(EAux index)
{
	const char* c_NAME[] =
	{
		"Reserved",
	};

	THanString hanStr;
	
	if ((index >= 0) && (index < AUX_MAX))
	{
		hanStr.szName = c_NAME[index];
	}

	return hanStr;
}

////////////////////////////////////////////////////////////////////////////////
// 게임의 메시지 문자열

const char* MESSAGE_STRING_TABLE[MESSAGE_MAX] =
{
	"마법 지수가 부족했다",
	"Noname",
	"전투 모드가 아닐때는 공격 마법을 사용할 수 없습니다.",
	"누구에게",
	"모든 사람들에게",
	"선택",
	"강한 치료 마법은 아직 불가능 합니다.",
	"@F<<<  방향을 선택하시오  >>>@@",
	"북쪽으로 공간이동",
	"남쪽으로 공간이동",
	"동쪽으로 공간이동",
	"서쪽으로 공간이동",
};

const char* GetMessageString(EMessage message)
{
	return MESSAGE_STRING_TABLE[message];
}

////////////////////////////////////////////////////////////////////////////////
// Resource 관리 utility

static unsigned char TABLE0[] =
{
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFB, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFE, 0xC0, 0xFF, 0xFF, 
0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xF7, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xEF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xF7, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xC0, 0xFF, 0xFF, 0xFF, 0x40, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFD, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFE, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xBF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xDF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xBF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFB, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xF7, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xF7, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xDF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xF7, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 
0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 
0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static unsigned char TABLE1[] =
{
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xDF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xEF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xDC, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 
0xFF, 0xFF, 0xEF, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 
0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 
0xDF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xE7, 0xFF, 0xFF, 0xFF, 0xFE, 
0xFF, 0xFF, 0x77, 0xBF, 0xFF, 0xFC, 0xFE, 0xFF, 0xFF, 0xDF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFC, 0xEF, 0xFF, 
0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 
0xFE, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 
0xFF, 0xEC, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 
0xFE, 0xFF, 0xFF, 0xFC, 0xFB, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xBF, 0xFF, 
0xFF, 0xF6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFB, 
0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xDF, 0xEC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xF7, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFE, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 
0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xBF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 0xDF, 0xEF, 0xB7, 0xFF, 
0xEF, 0xEF, 0xFE, 0xF7, 0xFF, 0xBC, 0xF7, 0xDF, 0xDE, 0xFF, 
0xF7, 0xD7, 0xED, 0xFF, 0x3F, 0xFB, 0xFF, 0xBC, 0xF6, 0xFF, 
0xBE, 0xF9, 0xFF, 0xBD, 0xBE, 0x7F, 0xDD, 0xFB, 0xCF, 0xFC, 
0xBF, 0xBF, 0xFF, 0xDF, 0xEF, 0xBF, 0xFD, 0xFE, 0xFF, 0x5F, 
0xFB, 0xBC, 0xBF, 0x7F, 0xA7, 0xBE, 0xFF, 0xED, 0xFF, 0x7F, 
0xFF, 0xDF, 0xE7, 0xFC, 0xF7, 0xFB, 0xD7, 0xFF, 0xB5, 0xF3, 
0xFA, 0xBE, 0xF7, 0xFC, 0xFF, 0xDC, 0xFD, 0xFE, 0xFF, 0xEF, 
0xF5, 0xE9, 0x7E, 0x7D, 0xFD, 0xEF, 0xDF, 0xFC, 0x7F, 0xBD, 
0xFF, 0x7F, 0x7F, 0xBB, 0xFB, 0xBF, 0x7D, 0xFD, 0x3F, 0x7C, 
0xDF, 0xFB, 0xFB, 0xFF, 0xF7, 0xFD, 0xDF, 0xFB, 0xFD, 0xFC, 
0xFF, 0xB8, 0xFB, 0xDF, 0xFF, 0x79, 0xBD, 0xEF, 0xFF, 0x7F, 
0xFE, 0xFF, 0xBB, 0xFC, 0xDF, 0xF7, 0xFA, 0xFF, 0x6B, 0xEB, 
0xFF, 0xD9, 0xEF, 0xBF, 0x7F, 0xEC, 0xFF, 0x7F, 0xB7, 0xFA, 
0xFD, 0xF9, 0x7E, 0xBD, 0xFD, 0xFF, 0xF7, 0xF8, 0xFE, 0xF7, 
0xFF, 0xEF, 0xF7, 0xFB, 0xBF, 0xF7, 0xBE, 0xFB, 0xFB, 0xFC, 
0xEB, 0xEF, 0xEF, 0xBF, 0xDF, 0xFB, 0xFE, 0xFF, 0x7F, 0xBB, 
0xFD, 0xD4, 0xE7, 0xE9, 0xBF, 0xBB, 0xFB, 0xFF, 0xFB, 0xFD, 
0xFF, 0xBB, 0xFF, 0xFC, 0x7F, 0x7F, 0xFF, 0x7E, 0xFF, 0xF7, 
0xFB, 0xEF, 0xEF, 0xEF, 0xFB, 0xF8, 0xFD, 0xFD, 0xFE, 0xFF, 
0xFD, 0xEF, 0xFF, 0xBF, 0xFD, 0xFE, 0xFD, 0xBC, 0xFB, 0xFB, 
0xF3, 0xFF, 0xBE, 0xDF, 0xBB, 0xFE, 0x9F, 0xBB, 0xFB, 0xFC, 
0xF7, 0xFE, 0xFF, 0x6F, 0xF5, 0x7F, 0xBA, 0xFB, 0xF6, 0x3B, 
0xFD, 0xFC, 0xEF, 0xF7, 0xDF, 0xEF, 0xED, 0xBF, 0xBB, 0xF5, 
0xFD, 0x5F, 0xBD, 0xFC, 0x7F, 0xDF, 0xDF, 0xED, 0xFF, 0x7F, 
0x7F, 0x3F, 0xBE, 0xA7, 0xF7, 0x5C, 0xF7, 0x7E, 0xFE, 0xFF, 
0xBF, 0xD7, 0xFD, 0xFD, 0xAF, 0xF4, 0xE7, 0xF4, 0x3F, 0x7B, 
0xFD, 0xEF, 0xEF, 0xFE, 0xFF, 0x5F, 0xEF, 0xEF, 0xDD, 0xFC, 
0x57, 0xBF, 0xEB, 0xDF, 0x7D, 0xFD, 0xFE, 0xFF, 0x5F, 0xEF, 
0xEF, 0xF4, 0xDF, 0xF7, 0xDE, 0xFD, 0xEF, 0xEF, 0x7B, 0xFB, 
0xF7, 0xFF, 0x7D, 0xFC, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xF7, 
0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFC, 0xFF, 0xFE, 0xFF, 0xFD, 
0xDF, 0xEF, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFD, 0xDF, 0xFF, 0x7F, 0xBD, 0xFF, 0xF7, 0xFF, 0xDF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xBD, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0xFB, 
0xEF, 0x7B, 0xFF, 0xFC, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 
0xDF, 0xFD, 0xEF, 0xFF, 0xFE, 0xFC, 0xF7, 0xFF, 0xFF, 0xFF, 
0xF7, 0xFF, 0xF6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFB, 0xFC, 
0xFF, 0xFF, 0x7F, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xBF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFB, 0xEF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xEF, 0x7F, 0xFF, 0xFD, 0xFC, 0xFF, 0xFF, 
0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 
0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xEF, 0xDF, 0xFC, 0xFF, 0x79, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFE, 0xFF, 0xFF, 
0xBF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDD, 0xFD, 0xF4, 
0xFF, 0xFF, 0xFF, 0x7F, 0xFD, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xDC, 0xFF, 0xFD, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xEF, 0xFF, 0xFF, 0xFF, 0x9F, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xDF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFC, 
0xFD, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xDD, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xF7, 0x7F, 
0xFF, 0xF7, 0xFF, 0xEC, 0xDF, 0xFF, 0xFE, 0xF7, 0x3F, 0xFB, 
0xF7, 0xFF, 0xFF, 0xFE, 0xFF, 0xDC, 0xEF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFD, 0xDF, 0xD7, 0xFF, 0xFF, 0xFC, 0xFD, 0xFF, 
0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFD, 0xFC, 
0xFF, 0xFF, 0xDF, 0xFF, 0xDF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 
0xFE, 0xFC, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0x7F, 0xFF, 0xEF, 0xDF, 0xFF, 
0xEF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFD, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xBF, 
0xFF, 0xEF, 0xFF, 0xEF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xBC, 
0xFF, 0xFF, 0xFF, 0xBE, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0x7D, 0xFF, 0xF7, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFC, 0xFF, 0xBF, 0xFF, 0xFF, 
0xEB, 0xFF, 0xFD, 0xF7, 0xFF, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xDB, 
0xFF, 0xFF, 0xF7, 0xFC, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xF7, 
0xEF, 0xFD, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFB, 0xDF, 0xFF, 0x7F, 0xFC, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 
0xF7, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBD, 0xFC, 0xFF, 0xDF, 
0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0x7C, 
0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xEC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xBC, 0xFD, 0xBF, 0xFF, 0xFF, 
0xFF, 0xF7, 0xFF, 0xFF, 0xBF, 0x7E, 0xFF, 0x7C, 0xFF, 0xFF, 
0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFC, 
};

unsigned short GetLastMultiByte(const char* szText)
{
	const unsigned char* pPrev;
	const unsigned char* pCh = (const unsigned char*)szText;
	while (1)
	{
		pPrev = pCh;
		if (*pCh++ & 0x80)
		{
			if (*pCh == 0)
				return *pPrev;
			++pCh;
		}

		if (*pCh)
			continue;

		if (*(pPrev+1))
			return int(*pPrev) << 8 | int(*(pPrev+1));
		else
			return int(*pPrev);
	}
}

typedef struct
{
	unsigned char   minLoByte;
	unsigned char   maxLoByte;
	unsigned char   minHiByte;
	unsigned char   maxHiByte;
	unsigned char* pTable;
} CONVERT_BLOCK;

static CONVERT_BLOCK s_table[] =
{
	{ 0x41, 0xA0, 0x81, 0xC6, TABLE0 },
	{ 0xA1, 0xFE, 0x81, 0xFD, TABLE1 },
};

bool HasJongsung(unsigned short code)
{
	unsigned char loByte = code & 0xFF;
	unsigned char hiByte = (code >> 8) & 0xFF;

	if (hiByte)
	{
		for (int ixTable = 0; ixTable < sizeof(s_table) / sizeof(s_table[0]); ixTable++)
		{
			CONVERT_BLOCK& table = s_table[ixTable];

			if ((loByte >= table.minLoByte) && (loByte <= table.maxLoByte) &&
				(hiByte >= table.minHiByte) && (hiByte <= table.maxHiByte))
			{
				loByte  -= table.minLoByte;
				hiByte  -= table.minHiByte;

				int pitch = (table.maxLoByte - table.minLoByte + 8) / 8;

				return (table.pTable[hiByte * pitch + loByte / 8] & (0x80 >> (loByte % 8))) > 0;
			}
		}
	}
	//?? 영어에 대해서도 적용해야 함

	// default
	return true;
}

} // namespace resource
