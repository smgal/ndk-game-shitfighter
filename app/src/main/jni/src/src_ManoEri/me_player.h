
#ifndef __ME_PLAYER_H__
#define __ME_PLAYER_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include "me_type.h"
#include <vector>


////////////////////////////////////////////////////////////////////////////////
// type definition

namespace manoeri
{
	enum EPlayerType
	{
		PLAYERTYPE_MAIN,
		PLAYERTYPE_NONE,
		PLAYERTYPE_DUMMY,
		PLAYERTYPE_FIRE
	};

	class CPlayer;
}

////////////////////////////////////////////////////////////////////////////////
// export definition

// Main player instance
extern manoeri::CPlayer* g_p_main_player;

// Vector of standby object
extern std::vector<manoeri::CPlayer*> g_standby_list;

// Vector of playable object
extern std::vector<manoeri::CPlayer*> g_player_list;

// Factory of playable objects
namespace manoeri
{
	extern CPlayer* CreateCharacter(EPlayerType chara_type, int x_pos, int y_pos);
}

////////////////////////////////////////////////////////////////////////////////
// class definition

namespace manoeri
{

class CPlayer
{
public:	
	int    m_type;
	int    m_h_texture;
	TPoint m_pos;
	int    m_face;
	int    m_face_dir;
	int    m_face_add;
	int    m_face_inc;
	TPoint m_space;
	int    m_move_inc;

	int    m_is_alive;
	int    m_mass;

	virtual ~CPlayer();

	virtual bool IsAlive(void);

	virtual void Move(int dx, int dy, int force, bool is_self_control);
	virtual void Display(int lighten = 255);
	virtual void DoAction(void);

	static  bool Sort(const CPlayer* a, const CPlayer* b);

protected:
	virtual void m_Display(int lighten = 255);
	bool         m_MoveTest(int x, int y);
};

class CDummyPlayer: public CPlayer
{
public:
	virtual ~CDummyPlayer();

	bool         IsMyEyesReached(int x1, int y1, int x2, int y2);
	virtual void Display(int lighten = 255);

protected:
	bool         m_IsSeen(int x1, int y1, int x2, int y2, int& num_reached);
	virtual void m_Display(int lighten = 255);
};

class CNonPlayer: public CDummyPlayer
{
public:
	virtual ~CNonPlayer();
	virtual void DoAction(void);

protected:
	virtual void m_Display(int lighten = 255);
};

class CFirePlayer: public CDummyPlayer
{
public:
	CFirePlayer();
	virtual ~CFirePlayer();

	virtual void DoAction(void);
	void         SetVelocity(int vx, int vy);

private:
	int m_life;
	int m_vx, m_vy;

	virtual void m_Display(int lighten = 255);
};

class CMainPlayer: public CPlayer
{
public:
	virtual ~CMainPlayer();

	virtual void DoAction(void);
};

} // namespace manoeri

#endif // #ifndef __ME_PLAYER_H__
