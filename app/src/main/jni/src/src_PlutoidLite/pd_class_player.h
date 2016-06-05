
#ifndef __PD_CLASS_PLAYER_H__
#define __PD_CLASS_PLAYER_H__

#include "pd_actor.h"
#include "pd_util.h"
#include "pd_base_world.h"
#include "util_convert_to_ucs.h"

#include <vector>

//?? 최종적으로는 사라져야 함
#include <math.h>

class CStringToInt
{
public:
	CStringToInt(const char* sz_text)
	:	m_string(sz_text), m_sz_text(m_string.c_str())
	{
	}

	int Get(void)
	{
		if ((m_sz_text == NULL) || (*m_sz_text == 0))
			return -1;

		// 0..9, A..Z, a..z
		if      ((*m_sz_text >= '0') && (*m_sz_text <= '9'))
			return (*m_sz_text++ - '0');
		else if ((*m_sz_text >= 'A') && (*m_sz_text <= 'Z'))
			return (*m_sz_text++ - 'A' + 10);
		else if ((*m_sz_text >= 'a') && (*m_sz_text <= 'z'))
			return (*m_sz_text++ - 'a' + 36);
		else
			return -1;
	}

private:
	std::string m_string;
	const char* m_sz_text;

};

class CFace
{
public:
	struct TFace
	{
		int  face;
		bool is_horiz_flipped;

		TFace(int _face, bool _is_horiz_flipped)
		:	face(_face), is_horiz_flipped(_is_horiz_flipped) {}
	};

	CFace(int face)
	:	m_single_face(face) {}

	CFace(int faces[], unsigned int num)
	: m_single_face(0)
	{
		if (num > 0)
		{
			m_single_face = faces[0];
			int* p = &faces[0];
			while (num-- > 0)
				m_face_list.push_back(*p++);
		}
	}

	void  SetFace(int dx, int dy, const char* sz_index_standing, const char* sz_index_moving)
	{
		unsigned int index = m_GetIndex(dx, dy);

		// clear all
		std::vector<int>().swap(m_index_list[0][index]);
		std::vector<int>().swap(m_index_list[1][index]);

		int number;

		// standing motion
		{
			CStringToInt stream(sz_index_standing);

			while ((number = stream.Get()) >= 0)
			{
				m_index_list[0][index].push_back(number);
			}
		}

		// moving motion
		{
			CStringToInt stream(sz_index_moving);

			while ((number = stream.Get()) >= 0)
			{
				m_index_list[1][index].push_back(number);
			}
		}
	}

	TFace GetDefaultFace(unsigned long ref_count) const
	{
		return m_GetFace(ref_count, m_GetIndex(0, 0), false);
	}

	TFace GetFaceFromDirection(unsigned long ref_count, int dx, int dy, bool is_moving) const
	{
		return m_GetFace(ref_count, m_GetIndex(dx, dy), is_moving);
	}

	TFace GetFaceFromDirection(unsigned long ref_count, TDirection direction, bool is_moving) const
	{
		return m_GetFace(ref_count, m_GetIndex(direction.dx, direction.dy), is_moving);
	}
	

private:
	int               m_single_face;
	std::vector<int>  m_face_list;
	std::vector<int>  m_index_list[2][9];

	unsigned int m_GetIndex(int dx, int dy) const
	{
		dx = sign(dx) + 1;
		dy = sign(dy) + 1;

		/*	index
			+---+---+---+
			| 0 | 1 | 2 |
			+---+---+---+
			| 3 | 4 | 5 |   <- 4 is a default face
			+---+---+---+
			| 6 | 7 | 8 |
			+---+---+---+
		 */
		return (dy * 3 + dx);
	}

	TFace m_GetFace(unsigned long ref_count, unsigned int index, bool is_moving) const
	{
		assert(index >= 0 && index < 9);

		//?? 아직 standing motion에 대해서는 고려되어 있지 않음
		int ix_motion = (is_moving) ? 1 : 0;

		if (m_index_list[ix_motion][index].empty())
		{
			return TFace(abs(m_single_face), m_single_face < 0);
		}
		else
		{
			unsigned int sub_index  = ref_count % m_index_list[ix_motion][index].size();
			unsigned int face_index = m_index_list[ix_motion][index][sub_index];
			int          face       = m_face_list[face_index];

			return TFace(abs(face), face < 0);
		}
	}

};

struct TDirectionHolded
{
	bool value;

	TDirectionHolded()
	:	value(false) {}
	TDirectionHolded(bool _holded)
	:	value(_holded) {}
};

struct TDistance
{
	unsigned int value;

	TDistance()
	:	value(0) {}
	TDistance(unsigned int _distance)
	:	value(_distance) {}
};

struct TAngleRange
{
	int min_value, max_value;

	TAngleRange()
	:	min_value(0), max_value(0) {}
	TAngleRange(int _min_value, int _max_value)
	:	min_value(_min_value), max_value(_max_value) {}
};

struct TOpaque
{
	unsigned int value;

	TOpaque()
	:	value(255) {}
	TOpaque(unsigned int _opaque)
	:	value(_opaque) {}
};

enum TPlayerTrait
{
	PLAYER_TRAIT_NONE     = 0x00000000,
	PLAYER_TRAIT_HAS_MASS = 0x00000001, // 물질로서 존재하는가?
	PLAYER_TRAIT_TALKABLE = 0x00000002, // 대화가 가능한가?
	PLAYER_TRAIT_MOVEABLE = 0x00000004, // 이동이 가능한가?
	PLAYER_TRAIT_CONFLICT = 0x00010000, // main character와는 상반된 속성을 가지고 있는가?
	PLAYER_TRAIT_HOSTILE  = 0x00020000, // 적개심을 품고 있는가?
	PLAYER_TRAIT_DWORD    = 0x7FFFFFFF
};

//?? 최대 길이나 그 이상에 대해 m_raw, m_alias가 안전하게 들어가갈 수 있는 지 검증 필요
template <size_t max_byte>
struct TStringManip
{
	TStringManip()
	{
		m_Reset();
	}
	TStringManip(const char* str)
	{
		m_Reset();
		m_AssignStr(str);
	}

	inline operator const char*()
	{
		return m_raw;
	}

	inline operator const unsigned short*()
	{
		return m_alias;
	}
	
	TStringManip& operator<<(const action::set<const char*>& name)
	{
		m_AssignStr(name.data);
		return *this;
	}

	unsigned long GetId(void)
	{
		return m_id;
	}

private:
	enum
	{
		MAX_BYTE_OF_CHARA_NAME = max_byte,
		MAX_LEN_OF_CHARA_NAME  = max_byte / sizeof(unsigned short)
	};

	char           m_raw[MAX_BYTE_OF_CHARA_NAME];
	unsigned short m_alias[MAX_LEN_OF_CHARA_NAME+1];
	unsigned long  m_id;

	void m_Reset(void)
	{
		m_raw[0]   = 0;
		m_alias[0] = 0;
		m_id       = 0;
	}
	void m_AssignStr(const char* str)
	{
		if (str)
		{
			memset(m_raw, 0, sizeof(m_raw));
			strncpy(m_raw, str, MAX_BYTE_OF_CHARA_NAME-1);

			size_t str_len = strlen(m_raw);

			// m_alias에는 내부 문자 코드인 UCS2가 들어간다.
			ConvertUHC2UCS2(m_alias, MAX_LEN_OF_CHARA_NAME, m_raw, str_len+1);
			// 만약 최대 길이가 지 갔을 경우를 대비한 terminating 문자 삽입
			m_alias[MAX_LEN_OF_CHARA_NAME] = 0;

			m_id = ConvertStringToId(m_raw, str_len);
		}
	}
};

struct TPlayerDesc
{
	TStringManip<32> name;

	unsigned long    trait_flag;
	TPosition        position;
	CFace            face;
	TDistance        distance;
	TAngleRange      angle_range;
	TOpaque          opaque; // 0~255
	int              shy;
	TDirectionHolded direction_holded;
	TDirection       sight_direction;
	int              sight_angle;

	mutable int       internal_count;
	mutable TPosition prev_position;
	mutable bool      is_moving;

	TPlayerDesc()
	:	trait_flag(PLAYER_TRAIT_NONE), position(0, 0), face(0), shy(0), sight_angle(0), internal_count(0), prev_position(0, 0), is_moving(false) {}
	TPlayerDesc(TPosition _position, CFace _face)
	:	trait_flag(PLAYER_TRAIT_NONE), position(_position), face(_face), shy(0), sight_angle(0), internal_count(0), prev_position(0, 0), is_moving(false) {}
};

class CPlayer: public CActor
{
public:
	CPlayer()
	:	CActor()
	{
	}
	CPlayer(void* p_param)
	:	CActor(p_param)
	{
	}
	CPlayer(FnAct fn_act, void* p_param)
	:	CActor(fn_act, p_param)
	{
	}

	~CPlayer()
	{
	}

	const TPlayerDesc& GetDesc(void) const
	{
		return _desc;
	}

	CPlayer& operator<<(const TPlayerDesc& player_desc)
	{
		unsigned long trait_flag = _desc.trait_flag;

		_desc = player_desc;

		// 이 값은 항상 복원
		_desc.trait_flag = trait_flag;

		return *this;
	}

	CPlayer& operator<<(const char* name)
	{
		if (name)
		{
			_desc.name << action::set<const char*>(name);
		}

		return *this;
	}

	CPlayer& operator<<(const TDirectionHolded& holded)
	{
		_desc.direction_holded = holded.value;
		return *this;
	}

	CPlayer& operator<<(const TPosition& position)
	{
		//?? 원래는 모두 달라야 함
		TExtent size(28, 32);

		if (!_desc.direction_holded.value)
			_desc.sight_direction = TDirection(_desc.position, position);

		if (TWorld::CanIGoThere(_desc.position, position.x, position.y, size))
		{
			_desc.position = position;
		}
		else if (TWorld::CanIGoThere(_desc.position, position.x, _desc.position.y, size))
		{
			_desc.position.x = position.x;
		}
		else if (TWorld::CanIGoThere(_desc.position, _desc.position.x, position.y, size))
		{
			_desc.position.y = position.y;
		}

		// 실제로 위치 변경이 있다면.
		if (!_desc.direction_holded.value)
		{
			if ((_desc.sight_direction.dx != 0) || (_desc.sight_direction.dy != 0))
			{
				_desc.sight_angle = int(atan2f(_desc.sight_direction.dy, _desc.sight_direction.dx) * 180 / 3.141592f);

				while (_desc.sight_angle < 0)
					_desc.sight_angle += 360;

				/*
					/\  270도
					<-  180도
					\/   90도
					->    0도
				*/
			}
		}

		return *this;
	}

	CPlayer& operator<<(const TDistance& distance)
	{
		_desc.distance = distance;

		return *this;
	}

	CPlayer& operator<<(const TAngleRange& angle_range)
	{
		_desc.angle_range = angle_range;

		return *this;
	}

	CPlayer& operator<<(const TOpaque& opaque)
	{
		_desc.opaque = opaque;

		return *this;
	}
	
	CPlayer& operator<<(const action::line_of_sight<bool>& sight)
	{
		_ActLineOfSight(sight.data);

		return *this;
	}

	CPlayer& operator<<(const action::act<int>& act)
	{
		iu::shared_ptr<class CPlayer> focused_chara = TWorld::GetFocusedChara();

		if (focused_chara.get())
		{
			// 대화 시도
			*focused_chara << action::talk<CPlayer*>(this);
		}

		return *this;
	}
	
	CPlayer& operator<<(const action::talk<CPlayer*>& talk)
	{
		action::set<TTryToCommunicate> set(TTryToCommunicate(talk.data, this));

		avej_lite::singleton<TWorld>::get() << set;
		
		return *this;
	}

	inline static void SharedPlayerAct(iu::shared_ptr<CPlayer> player)
	{
		player->Act();
	};

protected:
 	virtual void _Act(void* p_param) const
 	{
		_desc.internal_count++;

		// 이전과 위치가 바뀌었으면 움직이고 있는 것으로 간주 한다.
		_desc.is_moving = ((_desc.position.x != _desc.prev_position.x) || (_desc.position.y != _desc.prev_position.y));

		_desc.prev_position.x = _desc.position.x;
		_desc.prev_position.y = _desc.position.y;
 	}

 	virtual void _ActLineOfSight(bool am_i_seen)
 	{
 	}

protected:
	TPlayerDesc _desc;

};

//?? lock을 거는 과정이 들어 가야 한다.
class CMainPlayer: public CPlayer
{
public:
	CMainPlayer()
	:	CPlayer()
	{
		_desc.trait_flag = PLAYER_TRAIT_HAS_MASS;
	}

protected:
 	virtual void _ActLineOfSight(bool am_i_seen)
 	{
 	}
};

class CNpcPlayer: public CPlayer
{
public:
	CNpcPlayer()
	:	CPlayer()
	{
		_desc.trait_flag = PLAYER_TRAIT_HAS_MASS | PLAYER_TRAIT_TALKABLE;
	}

protected:
 	virtual void _Act(void* p_param) const
	{
		((CNpcPlayer*)this)->_ActInternal(p_param);

		// default action을 해줘야 함. animation 등을 위함
		CPlayer::_Act(p_param);
	}

 	virtual void _ActInternal(void* p_param)
	{
	}

};

// 아무런 의미 없이 걸어다니기만 한다.
class CNpcPlayerWanderer: public CNpcPlayer
{
public:
	CNpcPlayerWanderer()
	:	CNpcPlayer(),
		m_dx(0), m_dy(0)
	{
		_desc.trait_flag = PLAYER_TRAIT_HAS_MASS;
	}

protected:
 	void _ActInternal(void* p_param)
	{

		if (_desc.is_moving)
		{
			//?? 이 값은 class 고유 값이어야 함
			TExtent size(28, 32);

			// 갈 수 있는 옆길이 있다면 50% 확률로 꺾음
			switch (avej_lite::util::Random(2))
			{
			case 0:
				if (!TWorld::CanIGoThere(_desc.position, _desc.position.x - m_dx + m_dy, _desc.position.y - m_dy + m_dx, size) &&
					 TWorld::CanIGoThere(_desc.position, _desc.position.x        + m_dy, _desc.position.y        + m_dx, size))
				{
					m_dx ^= m_dy;
					m_dy ^= m_dx;
					m_dx ^= m_dy;
				}
				break;
			case 1:
				if (!TWorld::CanIGoThere(_desc.position, _desc.position.x - m_dx - m_dy, _desc.position.y - m_dy - m_dx, size) &&
					 TWorld::CanIGoThere(_desc.position, _desc.position.x        - m_dy, _desc.position.y        - m_dx, size))
				{
					m_dx ^= m_dy;
					m_dy ^= m_dx;
					m_dx ^= m_dy;
					m_dx  = -m_dx;
					m_dy  = -m_dy;
				}
				break;
			}
		}
		else
		{
			// 멈춘 경우 랜덤으로 방향을 설정
			switch (avej_lite::util::Random(4))
			{
			case 0: m_dx = -1, m_dy =  0; break;
			case 1: m_dx =  1, m_dy =  0; break;
			case 2: m_dx =  0, m_dy = -1; break;
			case 3: m_dx =  0, m_dy =  1; break;
			}
		}

		*this << TPosition(_desc.position.x + m_dx, _desc.position.y + m_dy);
	}

private:
	int m_dx, m_dy;

};

class CFloater: public CPlayer
{
public:
	CFloater()
	:	CPlayer()
	{
		_desc.trait_flag = PLAYER_TRAIT_CONFLICT;
	}

protected:
 	virtual void _ActLineOfSight(bool am_i_seen)
 	{
		if (am_i_seen)
		{
			//?? 임의의 값
			_desc.shy += 1;

			if (_desc.shy > 200)
				_desc.shy = 200;
		}
		else
		{
			//?? 임의의 값
			_desc.shy -= 5;

			if (_desc.shy < 0)
				_desc.shy = 0;
		}

		if (_desc.shy >= 128)
			_desc.opaque = 255;
		else
			_desc.opaque = _desc.shy * 2;
 	}
};

#endif
