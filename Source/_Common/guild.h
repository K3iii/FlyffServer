#ifndef __GUILD_H__
#define	__GUILD_H__

#include "mempooler.h"
#include <map>
#include <string>
#include "ar.h"
#include "script.h"
#if defined(__DBSERVER) || defined(__CORESERVER)
#include "..\DatabaseServer\project.h"
#include "..\_Network\Objects\obj.h"
#endif

#include "guildquest.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

const int MAX_GM_ALIAS			= 48;
const int MAX_GM_LEVEL			= 5;
const int MAX_GM_SIZE			= 128;
const int MAX_SQL_G_NAME		= 16;
const int MAX_G_NAME			= 48;
const int MAX_BYTE_NOTICE		= 128;
const int MAX_GUILD_LEVEL		= 50;

#define GM_MASTER 1
#define GM_KINGPIN 5
#define GM_CAPTAIN 10
#define GM_SUPPORTER 20
#define GM_ROOKIE 80

const DWORD PF_MEMBERLEVEL		= 0x00000001;
const DWORD PF_LEVEL			= 0x00000002;
const DWORD PF_INVITATION		= 0x00000004;
const DWORD PF_PENYA			= 0x00000008;
const DWORD PF_ITEM				= 0x00000010;
#if __VER >= 15 // __GUILD_HOUSE
const DWORD PF_GUILDHOUSE_FURNITURE		= 0x00000020;
const DWORD PF_GUILDHOUSE_UPKEEP		= 0x00000040;
#endif // __GUILD_HOUSE

typedef struct _SGuildMsgHeader
{
	enum
	{
		GUILD_BANK		= 0x0001,
		PENYA			= 0x0001,
		ITEM			= 0x0002,
	};

	union
	{
		DWORD		HeadA;
		struct
		{
			WORD	HeadASub;
			WORD	HeadAMain;
		};
	};

	union
	{
		DWORD		HeadB;
		struct
		{
			WORD	HeadBSub;
			WORD	HeadBMain;
		};
	};

}GUILD_MSG_HEADER, *LPGUILD_MSG_HEADER;

struct CONTRIBUTION_CHANGED_INFO
{
	u_long idGuild;
	u_long idPlayer;
	DWORD dwPxpCount, dwPenya;
	DWORD dwGuildPxpCount, dwGuildPenya;
	WORD nGuildLevel;
};

extern CAr&  operator<<(CAr& ar, CONTRIBUTION_CHANGED_INFO& info);
extern CAr&  operator>>(CAr& ar, CONTRIBUTION_CHANGED_INFO& info);

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

struct GUILD_TABLE_ENTRY
{
	DWORD	dwPxpCount;
	DWORD	dwPenya;
	WORD	nMaxMember;
};

class CGuildTable
{
public:
	virtual ~CGuildTable();

	BOOL	Load(LPCTSTR szFileName);
	DWORD	GetPxpCount(WORD nLevel) const;
	DWORD	GetPenya(WORD nLevel) const;
	WORD	GetMaxMemeber(WORD nLevel) const;
	void	Serialize( CAr & ar );
	BOOL	ReadBlock( CScript & script );
	int		GetMaxLevel() const { return m_nCount; }

	static CGuildTable& GetInstance();

private:
	CGuildTable();
	GUILD_TABLE_ENTRY	m_table[MAX_GUILD_LEVEL];
	int					m_nCount;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class CGuildMember
{
public:
	u_long	m_idPlayer;						// Player ID
	int		m_nPay;
	int		m_nGiveGold;
	DWORD	m_dwGivePxpCount;
	char	m_szAlias[MAX_GM_ALIAS];
	BYTE	m_nMemberLv;
	int		m_nClass;

public:
//	Constructions
	CGuildMember();
	virtual ~CGuildMember();
//	Operations
	void	Serialize( CAr & ar );

	CGuildMember &	operator = ( CGuildMember & source );

#ifdef __CLIENT
	CMover *GetMover( void ) { return prj.GetUserByID( m_idPlayer ); }
#endif // client
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CGuildMember>*	sm_pPool;
	void*	operator new( size_t nSize )	{	return CGuildMember::sm_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CGuildMember::sm_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CGuildMember::sm_pPool->Free( (CGuildMember*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CGuildMember::sm_pPool->Free( (CGuildMember*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////


enum CONTRIBUTION_RESULT
{
	CONTRIBUTION_OK,
	CONTRIBUTION_FAIL_MAXLEVEL,
	CONTRIBUTION_FAIL_GUILD_OVERFLOW_PXP,
	CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA,
	CONTRIBUTION_FAIL_INVALID_CONDITION,
	CONTRIBUTION_FAIL_OVERFLOW_PXP,
	CONTRIBUTION_FAIL_OVERFLOW_PENYA
};

#define	GF_WARTIME		(DWORD)0x00000001

typedef	struct	_GUILD_MEMBER_INFO
{
	u_long	idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
	LONG	nLevel;
	LONG	nJob;
	DWORD	dwSex;
#endif	// __SYS_PLAYER_DATA
	BYTE	nMultiNo;
	_GUILD_MEMBER_INFO()
	{	idPlayer	= 0;
#if __VER < 11 // __SYS_PLAYER_DATA
		nLevel	= nJob	= dwSex	= 0;
#endif	// __SYS_PLAYER_DATA
		nMultiNo	= 100;
	}
}
GUILD_MEMBER_INFO, *PGUILD_MEMBER_INFO;

class	CGuildWar;

class CGuild
{
public:
	u_long	m_idGuild;							// Guild ID
	char	m_szGuild[MAX_G_NAME];				// Guild Name
	map<u_long, CGuildMember*>	m_mapPMember;
	u_long	m_idMaster;
	int		m_nLevel;							// Guild Level
	DWORD	m_adwPower[MAX_GM_LEVEL];
	BOOL	m_bActive;
	DWORD   m_dwContributionPxp;
	char	m_szNotice[MAX_BYTE_NOTICE];
	DWORD	m_dwFlag;
#ifdef __FL_GUILD_FINDER
	BOOL	m_bFinder;
#endif

#if !defined( __CORESERVER)
	CItemContainer<CItemElem>	m_GuildBank;
#endif
	DWORD						m_nGoldGuild;

public:
//	Constructions
	CGuild();
	virtual ~CGuild();
	void	Clear( void );
//	Operations
//	If an error occurs, return nonzero.
	u_long	GetGuildId( void )		{ return m_idGuild; };
	BOOL	AddMember( CGuildMember* pMember );
	BOOL	RemoveMember( u_long idPlayer );
	BOOL	IsMember( u_long idPlayer )		{	return( GetMember( idPlayer ) != NULL );	}
	BOOL	IsMaster( u_long idPlayer )	{	return( m_idMaster == idPlayer );	}
	void	Serialize( CAr & ar, BOOL bDesc );
	void	SetNotice( const char* szNotice );

	void	MeritResultMsg( CONTRIBUTION_RESULT cbResult );
	void	SetContribution( CONTRIBUTION_CHANGED_INFO & info );
	BOOL	AddContribution( DWORD dwPxp, DWORD dwPenya, u_long idPlayer );
	CONTRIBUTION_RESULT CanContribute( DWORD dwPxp, DWORD dwPenya, u_long idPlayer );
	void	DecrementMemberContribution( u_long idPlayer, DWORD dwPenya, DWORD dwPxpCount );


	BOOL	IsGetPenya( u_long idPlayer )	{	return m_adwPower[GetMember(idPlayer)->m_nMemberLv] & PF_PENYA; }

	BOOL	IsGetItem( u_long idPlayer )	{	return m_adwPower[GetMember(idPlayer)->m_nMemberLv] & PF_ITEM; }


	BOOL	IsCmdCap( int nMemberLv, DWORD dwPower )	{	return( ( m_adwPower[nMemberLv] & dwPower )? TRUE: FALSE );	}

#if __VER >= 15 // __GUILD_HOUSE
	BOOL	IsAuthority( DWORD dwPlayerId, int nAuthority )	{ return GetMember(dwPlayerId) ? ( m_adwPower[GetMember(dwPlayerId)->m_nMemberLv] & nAuthority ) : FALSE; }
#endif // __GUILD_HOUSE
	//	Attributes
	int		GetSize( void )	{	return m_mapPMember.size();	}
	CGuildMember*	GetMember( u_long idPlayer );
	int		GetMaxMemberSize( void );

	int		GetMemberLvSize( int nMemberLv );
	int		GetMaxMemberLvSize( int nMemberLv );

	CGuild &	operator = ( CGuild & source );

	GUILDQUEST	m_aQuest[MAX_GUILD_QUEST];
	BYTE	m_nQuestSize;
	PGUILDQUEST		m_pQuest;
	PGUILDQUEST		FindQuest( int nQuestId );
	PGUILDQUEST		GetQuest( int nQuestId )	{	return FindQuest( nQuestId );	}
	BOOL	RemoveQuest( int nQuestId );
	void	SetQuest( int nQuestId, int nState );
#ifdef __WORLDSERVER
	void	ReplaceLodestar( const CRect & rect );
	void	Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL bMasterAround = FALSE );
#endif	// __WORLDSERVER

#ifndef __CORESERVER
	CItemElem*	GetItem( DWORD dwItemId, SERIALNUMBER iSerialNumber );
	short	RemoveItem( DWORD dwId, short nItemNum );
#endif	// __CORESERVER

	static	int	sm_anMaxMemberSize[MAX_GUILD_LEVEL];
	static	int	sm_anMaxMemberLvSize[MAX_GM_LEVEL];
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CGuild>*	sm_pPool;
	void*	operator new( size_t nSize )	{	return CGuild::sm_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CGuild::sm_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CGuild::sm_pPool->Free( (CGuild*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CGuild::sm_pPool->Free( (CGuild*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class CPlayer;

class CGuildMng
{
public:
	u_long	m_id;								// load
	map<u_long, CGuild*>	m_mapPGuild;
	map<string, CGuild*>	m_mapPGuild2;
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	CRIT_SEC	m_AddRemoveLock;
#endif

public:
//	Constructions
	CGuildMng();
	virtual ~CGuildMng();
	void	Clear( void );
	u_long	AddGuild( CGuild* pGuild );
	BOOL	RemoveGuild( u_long idGuild );
	CGuild*		GetGuild( u_long idGuild );
	CGuild*		GetGuild( const char* szGuild );
	void	Serialize( CAr & ar, BOOL bDesc );
	BOOL	SetName( CGuild* pGuild, const char* szName );
public:
#ifdef __CORESERVER
	void	AddConnection( CPlayer* pPlayer );
	void	RemoveConnection( CPlayer* pPlayer );
#endif	// __CORESERVER
//	Attributes
	int		GetSize( void )	{	return m_mapPGuild.size();	}
};



enum {	GUILD_PENYA_MINE_UPDATE, GUILD_ITEM_MINE_UPDATE, GUILD_PENYA_ALL_UPDATE, GUILD_ITEM_ALL_UPDATE };
enum { GUILD_PUT_ITEM, GUILD_GET_ITEM, GUILD_PUT_PENYA, GUILD_GET_PENYA, GUILD_CLOAK, GUILD_QUERY_REMOVE_GUILD_BANK };


#define MAX_RANK_LIST	20

#ifdef __DBSERVER
extern	APP_INFO	g_appInfo;
#endif

#endif	// __GUILD_H__