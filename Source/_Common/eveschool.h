#ifndef __EVE_SCHOOL_H__
#define	__EVE_SCHOOL_H__

#include "guild.h"

#define	MAX_SCHOOL	10

#ifdef __FL_GW_STATUE_V2
#include "defineObj.h"

const static D3DXVECTOR3 vMvpStatuePos = D3DXVECTOR3(6964.0f, 100.0f, 3297.8f);
const static D3DXVECTOR3 vLeaderStatuePos = D3DXVECTOR3(6955.0f, 100.0f, 3297.8f);

const static DWORD dwMvpStatueWorld = WI_WORLD_MADRIGAL;
const static DWORD dwLeaderStatueWorld = WI_WORLD_MADRIGAL;

const static float fMvpStatueAngle = 182.5f;
const static float fLeaderStatueAngle = 182.5f;

const static DWORD dwMvpStatueBuff = II_SYS_SYS_STATUE_BUFF_MVP; // IK2_BUFF
const static DWORD dwLeaderStatueBuff = II_SYS_SYS_STATUE_BUFF_GUILD; // IK2_BUFF

const static DWORD dwStatuesAppearSfx = XI_NPCMETEONYKER;
const static DWORD dwMvpAfterBuffEffect = XI_SKILL_JST_SUP_CRITICALSWING02;
const static DWORD dwLeaderAfterBuffEffect = XI_SKILL_JST_SUP_CRITICALSWING02;
#endif

class CGuildMng;

typedef struct __AUTO_OPEN
{
	BOOL bUseing;
	BYTE nHour;
	BYTE nMinute;
} __AUTO_OPEN;

class CGuildCombat
{
#ifdef __WORLDSERVER
	__AUTO_OPEN		__AutoOpen[7];
#endif // __WORLDSERVER

public:
	enum { OPEN_STATE, CLOSE_STATE, WAR_STATE, COMPLET_CLOSE_STATE, GM_COLSE_STATE };
	enum { NOTENTER_STATE = 100, NOTENTER_COUNT_STATE, ENTER_STATE, MAINTENANCE_STATE, WAR_WAR_STATE, WAR_CLOSE_STATE, WAR_CLOSE_WAIT_STATE, WAR_TELEPORT_STATE };
	enum { ALLMSG = 1000, GUILDMSG, JOINMSG, WORLDMSG, STATE, WARSTATE, WAIT };
	struct __REQUESTGUILD
	{
		u_long uidGuild;
		DWORD dwPenya;
	};
	struct __JOINPLAYER
	{
		u_long	uidPlayer;
		int		nlife;

		int		nPoint;
		int		uKillidGuild;
		DWORD	dwTelTime;
		__JOINPLAYER()
		{
			uidPlayer	= 0;
			nlife	= 0;
			nPoint	= 0;
			uKillidGuild	= 0;
			dwTelTime	= 0;
		}
	};
	struct __GuildCombatMember
	{
		u_long	uGuildId;
		vector<__JOINPLAYER*> vecGCSelectMember;

		DWORD dwPenya;
		BOOL  bRequest;
		u_long	m_uidDefender;
		int	  nJoinCount;
		int	  nWarCount;
		int   nGuildPoint;
		list<__JOINPLAYER*>	lspFifo;

		void Clear()
		{
			uGuildId = 0;
			SelectMemberClear();
			dwPenya = 0;
			bRequest = FALSE;
			m_uidDefender = 0;
			nJoinCount = 0;
			nWarCount = 0;
			nGuildPoint = 0;
		};
		void SelectMemberClear()
		{
			for( int veci = 0 ; veci < (int)( vecGCSelectMember.size() ) ; ++veci )
			{
				__JOINPLAYER* pJoinPlayer = vecGCSelectMember[veci];
				SAFE_DELETE( pJoinPlayer );
			}
			vecGCSelectMember.clear();
			lspFifo.clear();
		}
	};
	struct __GuildCombatProcess
	{
		int		nState;
		DWORD	dwTime;
		DWORD	dwCommand;
		DWORD	dwParam;
	};
	struct __GCGETPOINT
	{
		u_long uidGuildAttack;
		u_long uidGuildDefence;
		u_long uidPlayerAttack;
		u_long uidPlayerDefence;
		int nPoint;
		BOOL bKillDiffernceGuild;
		BOOL bMaster;
		BOOL bDefender;
		BOOL bLastLife;

		__GCGETPOINT()
		{
			uidGuildAttack = uidGuildDefence = uidPlayerAttack = uidPlayerDefence = nPoint = 0;
			bKillDiffernceGuild = bMaster = bDefender = bLastLife = FALSE;
		}
	};
	struct __GCPLAYERPOINT
	{
		u_long	uidPlayer;
		int		nJob;
		int		nPoint;

		__GCPLAYERPOINT()
		{
			uidPlayer = nJob = nPoint = 0;
		}
	};

	int		m_nGuildCombatIndex;
	u_long	m_uWinGuildId;
	int		m_nWinGuildCount;
	u_long	m_uBestPlayer;
	vector<__GCGETPOINT> m_vecGCGetPoint;
	vector<__GCPLAYERPOINT> m_vecGCPlayerPoint;
#ifdef __WORLDSERVER
	vector<__GuildCombatMember*> m_vecGuildCombatMem;
	DWORD	m_dwTime;
	int		m_nProcessGo;
	int	 	m_nProcessCount[ 25 ];
	__GuildCombatProcess GuildCombatProcess[250];
	int		m_nStopWar;

	int		m_nJoinPanya;
	int		m_nGuildLevel;
#if __VER >= 8 // __GUILDCOMBAT_85
	int		m_nMinGuild;
#endif // __VER >= 8
	int		m_nMaxGuild;
	int		m_nMaxJoinMember;
	int		m_nMaxPlayerLife;
	int		m_nMaxWarPlayer;
	int		m_nMaxMapTime;
	int		m_nMaxGuildPercent;
	int		m_nMaxPlayerPercent;
	int		m_nRequestCanclePercent;
	int		m_nNotRequestPercent;
	int		m_nItemPenya;
	BOOL    m_bMutex;
	BOOL    m_bMutexMsg;
	CTimer   m_ctrMutexOut;
#if __VER >= 8 // __GUILDCOMBAT_85
	vector< CString > m_vecstrGuildMsg;
#endif // __VER >= 8

	vector<__REQUESTGUILD>	vecRequestRanking;
#endif // __WORLDSERVER
	int		m_nState;
	int		m_nGCState;
#ifdef __CLIENT
	BOOL	m_bRequest;
	BOOL	IsRequest( void ) { return m_bRequest; };
#endif // __CLIENT
public:
	//	Constructions
	CGuildCombat();
	virtual ~CGuildCombat();
	void	GuildCombatClear( int Clear = 1 );
	void	GuildCombatGameClear();
	void	SelectPlayerClear( u_long uidGuild );
	void	AddvecGCGetPoint( u_long uidGuildAttack, u_long uidGuildDefence, u_long uidPlayerAttack, u_long uidPlayerDefence, int nPoint,
							BOOL bKillDiffernceGuild, BOOL bMaster, BOOL bDefender, BOOL bLastLife );
	void	AddvecGCPlayerPoint( u_long uidPlayer, int nJob, int nPoint );
#ifdef __WORLDSERVER
	BOOL	LoadScript( LPCSTR lpszFileName );
	void	JoinGuildCombat( u_long idGuild, DWORD dwPenya, BOOL bRequest );
	void	AddSelectPlayer( u_long idGuild, u_long uidPlayer );
	void	GetSelectPlayer( u_long idGuild, vector<__JOINPLAYER> &vecSelectPlayer );
	void	OutGuildCombat( u_long idGuild, DWORD dwChannel);
	void	SetMaintenance();
	void	SetEnter();
	void	SetGuildCombatStart();
	void	SetGuildCombatClose( BOOL bGM = FALSE );
	void	SetGuildCombatCloseWait( BOOL bGM = FALSE );
	void	GuildCombatCloseTeleport();

	void	SetRequestRanking( void );
	void	SetDefender( u_long uidGuild, u_long uidDefender );
	void	SetPlayerChange( CUser* pUser, CUser* pLeader );
	u_long	GetDefender( u_long uidGuild );
	u_long	GetBestPlayer( u_long* dwGetGuildId, int* nGetPoint );
	DWORD	GetRequstPenya( u_long uidGuild );
	void	GetPoint( CUser* pAttacker, CUser* pDefender );
	__int64	GetPrizePenya( int nFlag );
	BOOL	IsRequestWarGuild( u_long uidGuild, BOOL bAll );
	BOOL	IsSelectPlayer( CUser* pUser );

	void	JoinWar( CUser* pUser, BOOL bWar = TRUE );
	void	OutWar( CUser* pUser, CUser* pLeader, BOOL bLogOut = FALSE );
	void	JoinObserver( CUser* pUser, BOOL bMessage);
	void	GuildCombatRequest( CUser* pUser, DWORD dwPenya );
	void	GuildCombatCancel( CUser* pUser );
	void	GuildCombatOpen( void );
	void	GuildCombatEnter( CUser* pUser );
	void	UserOutGuildCombatResult( CUser* pUser );
	void	GuildCombatResult( BOOL nResult = FALSE, u_long idGuildWin = 0 );
	void	Process();
	void	ProcessCommand();
	void	ProcessJoinWar();
	void	SendJoinMsg( LPCTSTR lpszString );
	void	SendGuildCombatEnterTime( void );
	void	SendGCLog( void );
	void	SerializeGCWarPlayerList( CAr & ar );

#ifdef __FL_GW_STATUE_V2
	void DeleteStatues();
	void ApplyStatueBuff(CUser* pUser, BOOL bIsMvp, int nClientSidedPrice);
#endif

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
	CMover* CreatePlayerCopy(BYTE nType, PLAYER_COPY struc);
#endif

#if __VER >= 11 // __GUILDCOMBATCHIP
	void	GuildCombatResultRanking();
#endif // __GUILDCOMBATCHIP

	CTime	GetNextGuildCobmatTime(void);
	int		m_nDay;

	__GuildCombatMember* FindGuildCombatMember( u_long GuildId );
#endif // __WORLDSERVER
};

#endif	// __EVE_SCHOOL_H__