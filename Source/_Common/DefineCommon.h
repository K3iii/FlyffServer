#ifndef __COMMON_DEFINE__
#define __COMMON_DEFINE__

#include <windows.h>

const int MAX_SHORTCUT_STRING = 128;
const int MAX_GOLD_COIN = 32767;
const int MAX_GOLD_REWARD = INT_MAX;
const int MAX_INPUT_REWARD = 200000000;
const int MIN_INPUT_REWARD = 1000;

const unsigned char	REMOVE_VOTE = 0;
const unsigned char	CLOSE_VOTE = 1;
const unsigned char	CAST_VOTE = 2;

const	int		TIMEWAIT_CLOSE = 3;
const	int		TIMEWAIT_RELEASE = 3600;
const	int		TIMEWAIT_INSTANT_BANKER = 180000;

const int MAX_CHARGE_LEVEL = 4;
const int PROP_BONUSES = 6;

extern const int OLD_MPU;
extern int g_MPU;

#define SKILLPOINT_DIVIDE					20
#define SKILLPOINT_BASE						2
#define SKILLPOINT_LEGENDHERO_BASE			4
#define SKILLPOINT_LEGENDHERO_EXCEPTION		6

enum ITEM_USING_TYPE
{
	TYPE_INVENTORY
	, TYPE_CHEST
	, TYPE_BANK_0
	, TYPE_BANK_1
	, TYPE_BANK_2


	, TYPE_MAX
};

#if defined (__FL_FFA) || defined (__FL_TDM)
enum FFA_STATES
{
	STATE_CHILL,
	STATE_ANNOUNCE,
	STATE_PREPARE_BATTLE, 
	STATE_PROCESS_BATTLE, 
	STATE_PREPARE_CLOSE
};
#endif

#ifdef __FL_TDM
enum TDM_TEAMS { TDM_TEAM_NONE, TDM_TEAM_RED, TDM_TEAM_BLUE, TDM_TEAM_MAX };
#endif

enum ATK_TYPE
{
	ATK_FORCE,
	ATK_MELEESKILL,
	ATK_MAGICSKILL,
	ATK_MAGIC,
	ATK_GENERIC,
};

class CMover;

struct ATTACK_INFO
{
	CMover* pAttacker;
	CMover* pDefender;
	DWORD		dwAtkFlags;
	int			nParam;
	int			nParts;
	int			nDEFFactor;

	ATK_TYPE	GetAtkType() const;
	BOOL		CanIgnoreDEF() const;
	int			GetChargeLevel() const;
	int			GetAttackCount() const;
	int			GetSkill() const;
	BOOL		IsRangeAttack() const;
	int			GetSkillLevel() const;
};

#ifdef __CLIENT
inline int ATTACK_INFO::GetSkill() const
{
	return 0;
	}
#endif

#endif	// __COMMON_DEFINE__
