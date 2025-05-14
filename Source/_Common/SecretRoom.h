// SecretRoom.h: interface for the CSecretRoom class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_SECRETROOM_H__E848C037_E4D5_44ED_BC88_2CDBE643B64F__INCLUDED_)
#define AFX_SECRETROOM_H__E848C037_E4D5_44ED_BC88_2CDBE643B64F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 12 // __SECRET_ROOM
#include "Continent.h"
//////////////////////////////////////////////////////////

enum { SRMNG_CLOSE = 1, SRMNG_OPEN, SRMNG_LINEUP, SRMNG_ENTRANCE, SRMNG_WARWAIT, SRMNG_WAR };
enum { SRCONT_CLOSE = 101, SRCONT_OPEN, SRCONT_WAR, SRCONT_CLOSEWAIT };

#define MONSTER_FAILED	0
#define MONSTER_NORMAL	1
#define MONSTER_MIDBOSS	2
#define MONSTER_BOSS	3
#define MONSTER_WIN		4



struct __SECRETROOM_MONSTER
{
	DWORD	dwId;
	int		nNum;
	BOOL	bRed;
	int		x1, z1, x2, z2, y;
	__SECRETROOM_MONSTER()
	{
		dwId = NULL_ID;
		bRed = FALSE;
		nNum = x1 = z1 = x2 = z2 = y = 0;
	}
};


struct __SECRETROOM_TENDER
{
	DWORD	dwGuildId;
	int		nPenya;
	int		nKillCount;
	DWORD	dwWorldId;
	int		nWarState;
	vector<DWORD>	vecLineUpMember;
	vector<__SECRETROOM_MONSTER> vecMonster;
	__SECRETROOM_TENDER()
	{
		dwGuildId = NULL_ID;
		nPenya = 0;
		nKillCount = 0;
		dwWorldId = NULL_ID;
		nWarState = MONSTER_NORMAL;
	}

	bool operator ==( DWORD dwGuildId )
	{
		return this->dwGuildId == dwGuildId;
	}
};

class CSecretRoomContinent;
class CSecretRoom;

class CSecretRoomMng
{
public:
	CSecretRoomMng();
	virtual ~CSecretRoomMng();

	static CSecretRoomMng*	GetInstance( void );

	BYTE GetContinent( CMover* pMover );
	BOOL IsInTheSecretRoom( CMover* pMover );

	map<BYTE, CSecretRoomContinent*>	m_mapSecretRoomContinent;

	int	m_nMinGuildLevel;
	int m_nMinGuildMemberLevel;
	int m_nMinPenya;
	int m_nMinGuildNum;
	int m_nMaxGuildNum;
	int m_nMinGuildMemberNum;
	int m_nMaxGuildMemberNum;
	int m_nCancelReturnRate;
	int m_nDropoutReturnRate;

	int m_nMonsterGenNum;

	int		m_nState;
	DWORD	m_dwRemainTime;

	map<int, int>	m_mapMonsterNum;
#ifdef __CLIENT
	CSecretRoomContinent* m_pSRCont;
#endif // __CLIENT
#ifdef __WORLDSERVER
	BOOL LoadScript();
	void Process();
	BOOL CheckOpenTime();
	void SendNowState( CUser* pUser, int nState, DWORD dwRemainTime );
	void SendNowStateAllMember( BYTE nCont = CONT_NODATA, int nState = 0, DWORD dwRemainTime = 0 );
	void GetAllInfo( CUser* pUser );
	BOOL IsPossibleGuild( CUser* pUser );
	BOOL IsGuildMaster( CUser* pUser );
	BOOL IsOtherTenderGuild( CUser* pUser );
	void SetTenderOpenWnd( CUser* pUser );
	void SetTender( CUser* pUser, int nPenya );
	void SetTenderCancelReturn( CUser* pUser );
	void SetLineUpOpenWnd( CUser* pUser );
	void SetLineUp( CUser* pUser, vector<DWORD> vecLineUpMember );
	void GetTenderView( CUser* pUser );
	void SecretRoomOpen();
	void SetTeleportSecretRoom( CUser* pUser );
	void SetSecretRoomKill( DWORD dwWorldId, DWORD dwMonsterId );
	void SetFailGuild( CUser* pUser );
	D3DXVECTOR3 GetRevivalPos( CUser* pUser );

	CLuaBase m_Lua;

	vector<__SECRETROOM_MONSTER> m_vecNormalMonster;
	vector<__SECRETROOM_MONSTER> m_vecMidBossMonster;
	vector<__SECRETROOM_MONSTER> m_vecBossMonster;
#endif // __WORLDSERVER
};


class CSecretRoomContinent
{
public:
	CSecretRoomContinent( BYTE nCont );
	virtual ~CSecretRoomContinent();



	int		m_nState;
	DWORD	m_dwRemainTime;
	BYTE	m_nContinent;
	vector<__SECRETROOM_TENDER> m_vecSecretRoomTender;

	int		GetTenderGuild( DWORD dwGuildId );
#ifdef __WORLDSERVER
	void	Process( DWORD dwTick );
	BOOL	IsLineUpMember( CUser* pUser );
	void	SetContTender( CUser* pUser, int nPenya );
	void	SortTenderGuild();
	void	SetContTenderCancelReturn( CUser* pUser );
	void	SetContLineUp( CUser* pUser, vector<DWORD> vecLineUpMember );
	void	SecretRoomContOpen();
	void	SetSecretRoomWorld( int nContNum );
	void	SetTeleportSecretRoomCont( CUser* pUser );
	void	SetTeleportNormalRoom( CUser* pUser, int nIndex );
	void	SetTeleportBossRoom( CUser* pUser, int nIndex );
	void	CreateMonster();
	void	RemoveAllSecretRoomObj( DWORD dwWorldId );
	void	SetSecretRoomContKill( DWORD dwWorldId, DWORD dwMonsterId, BOOL bKill = TRUE );
	BOOL	IsSecretRoomMonster( DWORD dwMonsterId, int nWarState );
	void	UpdateInfoToMember( CUser* pUser, BYTE nType, int nIndex = NULL_ID );
	void	UpdateInfoToAllMember( BYTE nType, int nIndex = NULL_ID );
	void	ProcessInGuildMaster();
	void	SetContFailGuild( DWORD dwGuildId );
	void	SetContCloseWait();
	void	SetContClose();

	int		m_nFailedGuild;
#endif // __WORLDSERVER

};

///////////////////////////////////////////////////////////////
#endif // __SECRET_ROOM

#endif // !defined(AFX_SECRETROOM_H__E848C037_E4D5_44ED_BC88_2CDBE643B64F__INCLUDED_)


