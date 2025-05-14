// GuildHouse.h: interface for the CGuildHouse class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_GUILDHOUSE_H__7A401B90_A88E_4AB9_A5A3_CBFE71AB03F6__INCLUDED_)
#define AFX_GUILDHOUSE_H__7A401B90_A88E_4AB9_A5A3_CBFE71AB03F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if __VER >= 15 // __GUILD_HOUSE
enum
{
	GUILDHOUSE_PCKTTYPE_LISTUP = 0,
	GUILDHOUSE_PCKTTYPE_LISTDROP,
	GUILDHOUSE_PCKTTYPE_SETUP,
	GUILDHOUSE_PCKTTYPE_REMOVE,
	GUILDHOUSE_PCKTTYPE_RESET,
	GUILDHOUSE_PCKTTYPE_REFRESH,
	GUILDHOUSE_PCKTTYPE_UPKEEP,
	GUILDHOUSE_PCKTTYPE_EXPIRED,
};

static const int	BUY_MIN_MEMBERSIZE = 2;

static const int	BUY_MIN_LEVEL			= 3;
static const int	BUY_PENYA				= 100000000;
static const int	MAX_LISTUP_NUM			= 200;

static const int	UPKEEP_DAY_TIME			= 60 * 60 * 24;
static const int	MAX_UPKEEP_DAY			= 35;
static const int	UPKEEP_DAY_PENYA_SMALL	= 2400000;
static const int	UPKEEP_DAY_PENYA_MIDDLE	= 24000000;


////////////////////////////////////////////////////////////////
// CGuildHouseBase
////////////////////////////////////////////////////////////////

#ifdef __CLIENT
#define GuildHouse CGuildHouseBase::GetInstance()
#endif // __CLIENT
#define GH_Fntr_Info CGuildHouseBase::GuildHouse_Furniture_Info

struct HOUSING_ITEM;

class CGuildHouseBase
{
public:
	struct GuildHouse_Furniture_Info
	{
		DWORD		dwItemId;
		BOOL		bSetup;
		D3DXVECTOR3	vPos;
		float		fAngle;
		OBJID		objId;
		time_t		tKeepTime;
#ifdef __DBSERVER
		int			nSeqNum;
#endif // __DBSERVER

		GuildHouse_Furniture_Info( DWORD dwII = NULL_ID, BOOL bS = FALSE, D3DXVECTOR3 v = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), float fA = 0.0f, time_t tK = 0 );
		void		Serialize( CAr & ar );
	};
	typedef vector<GuildHouse_Furniture_Info>	VECFurnituretInfo;
	typedef VECFurnituretInfo::iterator			VECFurnitureIter;

protected:
	DWORD	m_dwGuildId;
	DWORD	m_dwWorldId;
	VECFurnituretInfo	m_vecFntInfo;

private:
	time_t	m_tUpkeepTime;

public:
	CGuildHouseBase( DWORD dwGuildId, DWORD dwWorldId );
	virtual ~CGuildHouseBase();

	void	Clear();
	DWORD	GetType()	{ return m_dwWorldId; }
	BOOL	OnGuildHousePacket( int nPacketType, GuildHouse_Furniture_Info & gfi, int nIndex );
	void	SerializeAllInfo( CAr & ar );
	GuildHouse_Furniture_Info GetFurnitureInfo( int nIndex ) { return nIndex < (int)( m_vecFntInfo.size() ) ? m_vecFntInfo[nIndex] : GuildHouse_Furniture_Info(); }
	time_t	GetUpkeepTime()	{ return m_tUpkeepTime; }
	void	SetUpkeeptime( time_t tUpkeepTime ) { m_tUpkeepTime = tUpkeepTime; }
	int		GetFurnitureListSize()	{ return m_vecFntInfo.size(); }
	BOOL	ExpiredGuildHouse();

#ifdef __FL_GUILD_FINDER
	GuildHouse_Furniture_Info* GetFurnitureInfoPtr(int nIndex) { return nIndex < (int)(m_vecFntInfo.size()) ? &m_vecFntInfo[nIndex] : NULL; }
#endif

#ifdef __CLIENT
	static CGuildHouseBase* GetInstance();
	void	SendClientToWorld( int nPacketType, GuildHouse_Furniture_Info & gfi, int nIndex );
#ifndef __FL_GUILD_FINDER
	GuildHouse_Furniture_Info* GetFurnitureInfoPtr(int nIndex) { return nIndex < (int)(m_vecFntInfo.size()) ? &m_vecFntInfo[nIndex] : NULL; }
#endif
	BOOL	IsMyGuildHouse( DWORD dwWorldId ) { return m_dwWorldId == dwWorldId; }
	GuildHouse_Furniture_Info* Find( OBJID objID );
	int		FindIndex( OBJID objID_ );
	void	ApplyEFTexture( );


	void	Setup( const HOUSING_ITEM& kInfo );
	void	Reset( const HOUSING_ITEM& kInfo );
	void	Remove( );
	void	Reset( );
	int		GetMode( )	{ return m_iMode; }

	DWORD	m_dwSelectedObjID;
	int		m_iMode;
	int		m_nWndDeployingIndex;

public :
	void	SetFurnitureChannel( BOOL bSetFurnitureChannel )	{ m_bSetFurnitureChannel = bSetFurnitureChannel; }
	BOOL	IsSetFurnitureChannel()								{ return m_bSetFurnitureChannel; }
	void	ResetValues( );

private:
	BOOL	m_bSetFurnitureChannel;
#endif // __CLIENT

#ifdef __WORLDSERVER
public:
	void	AfterFailedGuildHousePacket( int nPacketType, GH_Fntr_Info& gfi, int nIndex );
	void	SetWaitDBAckPlayerId( DWORD dwWaitDBAckPlayerId )	{ m_dwWaitDBAckPlayerId = dwWaitDBAckPlayerId; }
	DWORD	GetWaitDBAckPlayerId()	{ return m_dwWaitDBAckPlayerId; }
	BOOL	IsWaitDBAck()	{ return m_dwWaitDBAckPlayerId != NULL_ID; }
	BOOL	SendWorldToDatabase( CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex );
	void	SendWorldToClient( int nPacketType, GH_Fntr_Info& gfi, int nIndex );

	BOOL	IsEnteranceAble( CUser* pUser );
	void	SetApplyDST( CUser* pUser );
	void	ResetApplyDST( CUser* pUser );
	BOOL	EnteranceGuildHouseRoom( CUser* pUser );
	void	GoOutGuildHouseRoom( CUser* pUser );
	void	CheckDestroyRoom( CUser* pUser = NULL );
#endif // __WORLDSERVER

private:
	BOOL	SetupFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi );
	BOOL	RemoveFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi );
	BOOL	ResetFurnitureCtrl( int nIndex, GuildHouse_Furniture_Info & gfi );
	BOOL	ListUpFurniture( GuildHouse_Furniture_Info & gfi );
	BOOL	ListDropFurniture( int nIndex );
#ifdef __WORLDSERVER
	BOOL	IsAuthority( CUser* pUser, int nPacketType );
	BOOL	PreCheckPacket( CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex );
	void	SetDSTFunriture( ItemProp* pItemProp );
	void	ResetDSTFunriture( ItemProp* pItemProp );

	BOOL	CreateGuildHouseRoom();
	BOOL	DestroyGuildHouseRoom();
	BOOL	IsListUpAble( DWORD dwItemId );
	virtual int			GetMaxListUpNum( ItemProp* pItemProp ) = 0;
	virtual int			GetUpkeepPenya() = 0;

	virtual D3DXVECTOR3	GetEnterPos() = 0;
	virtual BOOL		IsSetupAble( CUser* pUser, int nIndex ) = 0;

	DWORD	m_dwWaitDBAckPlayerId;
#endif // __WORLDSERVER
#ifndef __CLIENT
public:
	virtual D3DXVECTOR3 GetTeleporterPos() = 0;
#endif // n__CLIENT

#ifdef __DBSERVER
public:
	int		GetFirstExpiredFurnitureIndex();
private:
#endif // __DBSERVER
};

#ifndef __CLIENT
////////////////////////////////////////////////////////////////
// CGuildHouseSmall : CGuildBase
////////////////////////////////////////////////////////////////
class CGuildHouseSmall : public CGuildHouseBase
{
public:
	CGuildHouseSmall( DWORD dwGuildId );
	virtual ~CGuildHouseSmall();

public:
#ifndef __CLIENT
	virtual D3DXVECTOR3 GetTeleporterPos()	{ return D3DXVECTOR3( 80.0f, 100.0f, 75.0f ); }
#endif // n__CLIENT

#ifdef __WORLDSERVER
private:
	virtual D3DXVECTOR3 GetEnterPos() { return D3DXVECTOR3( (float)( xRandom( 78, 83 ) ), 100.0f, (float)( xRandom( 61, 66 ) ) ); }
	virtual int			GetMaxListUpNum( ItemProp* pItemProp );
	virtual int			GetUpkeepPenya() { return UPKEEP_DAY_PENYA_SMALL; }
	virtual BOOL		IsSetupAble( CUser* pUser, int nIndex );
#endif // __WORLDSERVER


};


////////////////////////////////////////////////////////////////
// CGuildHouseMiddle : CGuildBase
////////////////////////////////////////////////////////////////
class CGuildHouseMiddle : public CGuildHouseBase
{
public:
	CGuildHouseMiddle( DWORD dwGuildId );
	virtual ~CGuildHouseMiddle();

public:
#ifndef __CLIENT
	virtual D3DXVECTOR3 GetTeleporterPos()	{ return D3DXVECTOR3( 250.0f, 100.0f, 250.0f ); }
#endif // n__CLIENT

#ifdef __WORLDSERVER
private:
	virtual D3DXVECTOR3 GetEnterPos() { return D3DXVECTOR3( (float)( xRandom( 248, 252 ) ), 100.0f, (float)( xRandom( 248, 252 ) ) ); }
	virtual int			GetMaxListUpNum( ItemProp* pItemProp )	{ return MAX_LISTUP_NUM; }
	virtual int			GetUpkeepPenya() { return UPKEEP_DAY_PENYA_MIDDLE; }
	virtual BOOL		IsSetupAble( CUser* pUser, int nIndex )	{ return TRUE; }
#endif // __WORLDSERVER
};




////////////////////////////////////////////////////////////////
// CGuildHouseMng : CGuildBase
////////////////////////////////////////////////////////////////
#define	GuildHouseMng	CGuildHouseMng::GetInstance()
class CGuildHouseMng
{
public:
	CGuildHouseMng();
	~CGuildHouseMng();
	static CGuildHouseMng* GetInstance();

	CGuildHouseBase*	MakeGuildHouse( DWORD dwGuildId, DWORD dwWorldId );
	BOOL				AddGuildHouse( DWORD dwGuildId, CGuildHouseBase* pGuildHouse )	{ return m_mapGuildHouse.insert( make_pair( dwGuildId, pGuildHouse ) ).second; }
	BOOL				RemoveGuildHouse( DWORD dwGuildId );
	CGuildHouseBase* GetGuildHouse( DWORD dwGuildId );
	BOOL	IsEmpty()	{ return m_mapGuildHouse.empty(); }
	void	Serialize( CAr & ar );

#ifdef __WORLDSERVER
public:
	BOOL	IsGuildHouse( DWORD dwWorldId );
	BOOL	IsLoginAble( CUser* pUser, DWORD dwWorldId, int nLayer );
	BOOL	EnteranceGuildHouse( CUser* pUser, DWORD dwComebackItemId = NULL_ID );
	void	GoOutGuildHouse( CUser* pUser );
	void	CheckDestroyGuildHouse( CUser* pUser );

	void	ReqBuyGuildHouse( CUser* pUser );
	void	OnBuyGuildHouse( CAr & ar );
	void	OnRemoveGuildHouse( CAr & ar );
	BOOL	SendWorldToDatabase( CUser* pUser, int nPacketType, GH_Fntr_Info& gfi, int nIndex = NULL_ID );

	void	SetApplyDST( CUser* pUser );
	void	ResetApplyDST( CUser* pUser );

private:
	BOOL	IsBuyAble( CUser* pUser );
#endif // __WORLDSERVER

#ifdef __DBSERVER
public:
	void	ProcessExpired();
	BOOL	DBLoadGuildHouse( CQuery* pQuery );
	int		AddSeqNum()	{ return ++m_nSeqNum; }
	int		GetSeqNum() { return m_nSeqNum; }

private:
	int		m_nSeqNum;
#endif // __DBSERVER

private:
	typedef map<DWORD, CGuildHouseBase*> MapGuildHouse;
	MapGuildHouse m_mapGuildHouse;
};
#endif // n__CLIENT

#endif // __GUILD_HOUSE

#endif // !defined(AFX_GUILDHOUSE_H__7A401B90_A88E_4AB9_A5A3_CBFE71AB03F6__INCLUDED_)
