#include "stdafx.h"



#include "guild.h"
#ifdef __CORESERVER
#include "player.h"
extern	CPlayerMng	g_PlayerMng;
#include "dpcachesrvr.h"
extern	CDPCacheSrvr	g_DPCacheSrvr;
#include "DPCoreSrvr.h"
extern  CDPCoreSrvr		g_dpCoreSrvr;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#include "DPCoreClient.h"
extern  CDPCoreClient	g_DPCoreClient;
#include "user.h"
extern	CUserMng	g_UserMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "defineQuest.h"
#endif // __WORLDSERVER


#include "guildwar.h"

//________________________________________________________________________________
CAr&  operator<<(CAr& ar, CONTRIBUTION_CHANGED_INFO& info)
{
	ar << info.idGuild << info.idPlayer << info.dwPxpCount << info.dwPenya;
	ar << info.dwGuildPxpCount << info.dwGuildPenya << info.nGuildLevel;
	return ar;
}

CAr&  operator>>(CAr& ar, CONTRIBUTION_CHANGED_INFO& info)
{
	ar >> info.idGuild >> info.idPlayer >> info.dwPxpCount >> info.dwPenya;
	ar >> info.dwGuildPxpCount >> info.dwGuildPenya >> info.nGuildLevel;
	return ar;
}

//________________________________________________________________________________

//________________________________________________________________________________
CGuildTable& CGuildTable::GetInstance()
{
	static CGuildTable table;
	return table;
}

CGuildTable::CGuildTable()
{
	m_nCount = 0;
}

CGuildTable::~CGuildTable()
{
}


void CGuildTable::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_nCount;
		for( int i=0; i<m_nCount; i++ )
		{
			ar << m_table[ i ].dwPxpCount;
			ar << m_table[ i ].dwPenya;
			ar << m_table[ i ].nMaxMember;
		}
	}
	else
	{
		ar >> m_nCount;
		for( int i=0; i<m_nCount; i++ )
		{
			ar >> m_table[ i ].dwPxpCount;
			ar >> m_table[ i ].dwPenya;
			ar >> m_table[ i ].nMaxMember;
		}
	}
}


BOOL CGuildTable::ReadBlock( CScript & script )
{
	script.GetToken(); // {
	DWORD dwVal = script.GetNumber();

	int i = NULL;
	for( ; *script.token != '}' ; ++i )
	{
		m_table[ i ].dwPxpCount = dwVal;
		m_table[ i ].dwPenya    = script.GetNumber();
		m_table[ i ].nMaxMember = script.GetNumber();

		dwVal = script.GetNumber();
	}

	ASSERT( i <= MAX_GUILD_LEVEL );
	m_nCount = i;
	return TRUE;
}


DWORD CGuildTable::GetPxpCount(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].dwPxpCount;
}


DWORD CGuildTable::GetPenya(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].dwPenya;
}


WORD CGuildTable::GetMaxMemeber(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].nMaxMember;
}


//________________________________________________________________________________
#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CGuildMember>*	CGuildMember::sm_pPool	= new MemPooler<CGuildMember>( 512, "CGuildMember" );
	#else	// __VM_0819
	MemPooler<CGuildMember>*	CGuildMember::sm_pPool	= new MemPooler<CGuildMember>( 512 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820
CGuildMember::CGuildMember()
{
	m_idPlayer		= 0;
	m_nPay			= 0;
	m_nGiveGold		= 0;
	m_dwGivePxpCount= 0;
	*m_szAlias		= '\0';
	m_nMemberLv		= 0;
	m_nClass = 0;
}

CGuildMember::~CGuildMember()
{

}

void CGuildMember::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idPlayer << m_nPay << m_nGiveGold << m_dwGivePxpCount << m_nMemberLv;
		ar << m_nClass;
		ar.WriteString( m_szAlias );
	}
	else
	{
		ar >> m_idPlayer >> m_nPay >> m_nGiveGold >> m_dwGivePxpCount >> m_nMemberLv;
		ar >> m_nClass;
		ar.ReadString( m_szAlias, MAX_GM_ALIAS );
	}
}

CGuildMember & CGuildMember::operator = ( CGuildMember & source )
{
	m_idPlayer	= source.m_idPlayer;
	m_nMemberLv	= source.m_nMemberLv;
	return *this;
}

//________________________________________________________________________________
#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CGuild>*	CGuild::sm_pPool		= new MemPooler<CGuild>( 128, "CGuild" );
	#else	// __VM_0819
	MemPooler<CGuild>*	CGuild::sm_pPool		= new MemPooler<CGuild>( 128 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

int	CGuild::sm_anMaxMemberSize[MAX_GUILD_LEVEL]	=
	{	30, 30, 32, 32, 34, 34, 36, 36, 38, 38, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 80 };

int	CGuild::sm_anMaxMemberLvSize[MAX_GM_LEVEL]	=
	{	GM_MASTER, GM_KINGPIN, GM_CAPTAIN, GM_SUPPORTER, GM_ROOKIE	};

CGuild::CGuild()
{
	m_idGuild				= 0;
	*m_szGuild				= '\0';
	m_idMaster				= 0;
	m_nLevel				= 1;
	m_bActive				= FALSE;
	m_dwContributionPxp		= 0;
	*m_szNotice				= '\0';
#if !defined( __CORESERVER)
	m_GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );
#endif

	ZeroMemory( m_adwPower, sizeof(m_adwPower) );
	m_nGoldGuild = 0;

	ZeroMemory( m_aQuest, sizeof(m_aQuest) );
	m_nQuestSize	= 0;
	m_pQuest	= NULL;
#ifdef __FL_GUILD_FINDER
	m_bFinder = TRUE;
#endif
}

CGuild::~CGuild()
{
	Clear();
}

void CGuild::Clear( void )
{
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i	= m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		SAFE_DELETE( pMember );
	}
	m_mapPMember.clear();
}

BOOL CGuild::AddMember( CGuildMember* pMember )
{
	if( GetMember( pMember->m_idPlayer ) )
		return FALSE;	// already exists
	m_mapPMember.insert( map<u_long, CGuildMember*>::value_type( pMember->m_idPlayer, pMember ) );
	return TRUE;
}

BOOL CGuild::RemoveMember( u_long idPlayer )
{
	CGuildMember* pMember	= GetMember( idPlayer );
	if( pMember )
	{
		SAFE_DELETE( pMember );
		m_mapPMember.erase( idPlayer );
		return TRUE;
	}
	return FALSE;
}

void CGuild::Serialize( CAr & ar, BOOL bDesc )
{
	if( ar.IsStoring() )
	{
		ar << m_idGuild << m_idMaster << m_nLevel;
		ar.WriteString( m_szGuild );
		ar << m_nGoldGuild;
#ifdef __FL_GUILD_FINDER
		ar << m_bFinder;
#endif

		if( !bDesc )
		{
			ar.Write( m_adwPower, sizeof(m_adwPower) );
			ar.WriteString( m_szNotice );
			ar << m_dwContributionPxp;
			ar << m_nLevel;
			ar << (short)GetSize();
			for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
				( i->second )->Serialize( ar );

			ar << m_nQuestSize;
			ar.Write( (const void*)m_aQuest, sizeof(GUILDQUEST) * m_nQuestSize );
		}
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_idGuild >> m_idMaster >> m_nLevel;
		ar.ReadString( m_szGuild, MAX_G_NAME );
		ar >> m_nGoldGuild;
#ifdef __FL_GUILD_FINDER
		ar >> m_bFinder;
#endif

		if( !bDesc )
		{
			ar.Read( m_adwPower, sizeof(m_adwPower) );
			ar.ReadString( m_szNotice, MAX_BYTE_NOTICE );
			ar >> m_dwContributionPxp;
			ar >> m_nLevel;

			short i, nSize	= 0;
			ar >> nSize;
			for( i = 0; i < nSize; i++ )
			{
				CGuildMember* pMember	= new CGuildMember;
				pMember->Serialize( ar );
				bool bResult = m_mapPMember.insert(map<u_long, CGuildMember*>::value_type(pMember->m_idPlayer, pMember)).second;
				if (bResult == false)
				{
					SAFE_DELETE(pMember);
				}
			}
			ar >> m_nQuestSize;
			ar.Read( (void*)m_aQuest, sizeof(GUILDQUEST) * m_nQuestSize );
		}
	}
}

CGuildMember* CGuild::GetMember( u_long idPlayer )
{
	map<u_long, CGuildMember*>::iterator i	= m_mapPMember.find( idPlayer );
	if( i != m_mapPMember.end() )
		return i->second;
	return NULL;
}

int CGuild::GetMemberLvSize( int nMemberLv )
{
	int cb	= 0;
	for( map<u_long, CGuildMember*>::iterator i	= m_mapPMember.begin();
		i != m_mapPMember.end(); ++i )
	{
		if( i->second->m_nMemberLv == nMemberLv )
			cb++;
	}
	return cb;
}

int CGuild::GetMaxMemberLvSize( int nMemberLv )
{
	if( nMemberLv < 0 || nMemberLv >= MAX_GM_LEVEL )
		return 0;
	if( nMemberLv == GUD_ROOKIE )
		return GetMaxMemberSize();
	return( CGuild::sm_anMaxMemberLvSize[nMemberLv] );
}

int	CGuild::GetMaxMemberSize( void )
{
	return (int)CGuildTable::GetInstance().GetMaxMemeber( m_nLevel );
}

void CGuild::SetContribution( CONTRIBUTION_CHANGED_INFO& info )
{
	m_nLevel			= info.nGuildLevel;
	m_nGoldGuild		= info.dwGuildPenya;
	m_dwContributionPxp	= info.dwGuildPxpCount;

	CGuildMember* pMember = GetMember( info.idPlayer );
	if( pMember == NULL )
	{
		TRACE("CGuild::SetContribution - Guild[%d] idPlayer[%d] not found \n", m_idGuild, info.idPlayer );
		return;
	}
	pMember->m_dwGivePxpCount += info.dwPxpCount;
	pMember->m_nGiveGold += (int)info.dwPenya;
}

void CGuild::DecrementMemberContribution( u_long idPlayer, DWORD dwPenya, DWORD dwPxpCount )
{
	CGuildMember* pMember = GetMember( idPlayer );
	if( pMember == NULL )
	{
		TRACE("CGuild::SetContribution - Guild[%d] idPlayer[%d] not found \n", m_idGuild, idPlayer );
		return;
	}
	pMember->m_dwGivePxpCount -= dwPxpCount;
	pMember->m_nGiveGold -= (int)dwPenya;
}


CONTRIBUTION_RESULT CGuild::CanContribute( DWORD dwPxp, DWORD dwPenya, u_long idPlayer )
{
	if( dwPxp > 0 && m_nLevel >= CGuildTable::GetInstance().GetMaxLevel() )
		return CONTRIBUTION_FAIL_MAXLEVEL;

	if( m_dwContributionPxp + dwPxp < m_dwContributionPxp )
		return CONTRIBUTION_FAIL_GUILD_OVERFLOW_PXP;

	if( m_nGoldGuild + dwPenya < m_nGoldGuild )
		return CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA;

	CGuildMember* pMember = GetMember( idPlayer );
	if( pMember == NULL )
	{
		TRACE("CanContribute - Guild[%d] idPlayer[%d] not found \n", m_idGuild, idPlayer );
		return CONTRIBUTION_FAIL_INVALID_CONDITION;
	}

	if( pMember->m_dwGivePxpCount + dwPxp < pMember->m_dwGivePxpCount )
		return CONTRIBUTION_FAIL_OVERFLOW_PXP;

	if( pMember->m_nGiveGold + (int)dwPenya < pMember->m_nGiveGold )
		return CONTRIBUTION_FAIL_OVERFLOW_PENYA;

	return CONTRIBUTION_OK;
}


BOOL CGuild::AddContribution( DWORD dwPxp, DWORD dwPenya, u_long idPlayer )
{
	if( CanContribute( dwPxp, dwPenya, idPlayer) != CONTRIBUTION_OK )
		return FALSE;

	CGuildMember* pMember = GetMember( idPlayer );
	pMember->m_dwGivePxpCount += dwPxp;
	pMember->m_nGiveGold += (int)dwPenya;

	m_dwContributionPxp += dwPxp;
	m_nGoldGuild += dwPenya;

	if( m_nLevel >= CGuildTable::GetInstance().GetMaxLevel() )
		return TRUE;

	DWORD dwMaxPxp = CGuildTable::GetInstance().GetPxpCount( m_nLevel + 1 );
	DWORD dwMaxPenya = CGuildTable::GetInstance().GetPenya( m_nLevel + 1);
	while ( m_dwContributionPxp >= dwMaxPxp && m_nGoldGuild >= dwMaxPenya )
	{
		m_dwContributionPxp -= dwMaxPxp;
		m_nGoldGuild -= dwMaxPenya;
		++m_nLevel;  // LevelUP

		if (m_nLevel >= CGuildTable::GetInstance().GetMaxLevel())
			return TRUE;

		dwMaxPxp = CGuildTable::GetInstance().GetPxpCount(m_nLevel + 1);
		dwMaxPenya = CGuildTable::GetInstance().GetPenya(m_nLevel + 1);
	}

	return TRUE;
}

void CGuild::SetNotice( const char* szNotice )
{
	ASSERT( szNotice );
	strncpy(m_szNotice, szNotice, MAX_BYTE_NOTICE);
	m_szNotice[MAX_BYTE_NOTICE-1] = '\0';
}

CGuild & CGuild::operator = ( CGuild & source )
{
	m_idGuild	= source.m_idGuild;
	m_idMaster	= source.m_idMaster;
	lstrcpy( m_szGuild, source.m_szGuild );
	for( map<u_long, CGuildMember*>::iterator i = source.m_mapPMember.begin(); i != source.m_mapPMember.end(); ++ i )
	{
		CGuildMember* pMember	= new CGuildMember;
		*pMember	= *( i->second );
		if( !AddMember( pMember ) )
			{	SAFE_DELETE( pMember );	}
	}
	return *this;
}

//________________________________________________________________________________
CGuildMng::CGuildMng()
{
	m_id	= 0;
}

CGuildMng::~CGuildMng()
{
	Clear();
}

void CGuildMng::Clear( void )
{
#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	m_AddRemoveLock.Enter( theLineFile );	// lock1
#endif

	for( map<u_long, CGuild*>::iterator i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
		SAFE_DELETE( i->second );
	m_mapPGuild.clear();
	m_mapPGuild2.clear();

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif
}

BOOL CGuildMng::SetName( CGuild* pGuild, const char* szName )
{
	CGuild* ptmp	= GetGuild( szName );
	if( ptmp )
		return FALSE;
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.erase( pGuild->m_szGuild );	// erase
	lstrcpy( pGuild->m_szGuild, szName );	// set
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.insert( map<string, CGuild*>::value_type( pGuild->m_szGuild, pGuild ) );	// insert
	return TRUE;
}

u_long CGuildMng::AddGuild( CGuild* pGuild )
{
//	locked
	m_id = (pGuild->m_idGuild != 0 ? pGuild->m_idGuild : m_id + 1);
	if (GetGuild(m_id))
	{
		Error("[ Exist Guild ID(%d) ]", m_id);
		return 0;
	}
	pGuild->m_idGuild = m_id;
	bool bResult = m_mapPGuild.insert(map<u_long, CGuild*>::value_type(m_id, pGuild)).second;
	if (bResult == false)
	{
		Error("[ Insert Failed. Guild ID(%d) ]", m_id);
		return 0;
	}
	if (lstrlen(pGuild->m_szGuild) > 0)
	{
		m_mapPGuild2.insert(map<string, CGuild*>::value_type(pGuild->m_szGuild, pGuild));
	}
	return m_id;
}

BOOL CGuildMng::RemoveGuild( u_long idGuild )
{
//	locked
	CGuild* pGuild	= GetGuild( idGuild );
	if( pGuild )
	{
		m_mapPGuild.erase( pGuild->m_idGuild );
		if( lstrlen( pGuild->m_szGuild ) > 0 )
			m_mapPGuild2.erase( pGuild->m_szGuild );
		SAFE_DELETE( pGuild );
		return TRUE;
	}
	return FALSE;
}

CGuild* CGuildMng::GetGuild( u_long idGuild )
{
	map<u_long, CGuild*>::iterator i	= m_mapPGuild.find( idGuild );
	if( i != m_mapPGuild.end() )
		return i->second;
	return NULL;
}

CGuild* CGuildMng::GetGuild( const char* szGuild )
{
	map<string, CGuild*>::iterator i	= m_mapPGuild2.find( szGuild );
	if( i != m_mapPGuild2.end() )
		return i->second;
	return NULL;
}

void CGuildMng::Serialize( CAr & ar, BOOL bDesc )
{
	if( ar.IsStoring() )
	{
		ar << m_id;
		ar << GetSize();
		for( map<u_long, CGuild*>::iterator i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
			( i->second )->Serialize( ar, bDesc );

	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_id;
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CGuild* pGuild	= new CGuild;
			pGuild->Serialize( ar, bDesc );
			bool bResult = m_mapPGuild.insert(map<u_long, CGuild*>::value_type(pGuild->m_idGuild, pGuild)).second;
			if (bResult == false)
			{
				SAFE_DELETE(pGuild);
			}
			else
			{
				if (lstrlen(pGuild->m_szGuild) > 0)
				{
					m_mapPGuild2.insert(map<string, CGuild*>::value_type(pGuild->m_szGuild, pGuild));
				}
			}
		}

	}
}

#ifdef __CORESERVER
void CGuildMng::AddConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CGuild* pGuild	= GetGuild( pPlayer->m_idGuild );
	if( !pGuild || !pGuild->IsMember( pPlayer->uKey ) )
	{

		pPlayer->m_idGuild	= 0;
	}
	else
	{
		if( pGuild->GetSize() > MAX_GM_SIZE )
			return;



		int nMaxLogin = 0;
		u_long uLoginPlayerId[MAX_GM_SIZE];
		u_long uLoginGuildMulti[MAX_GM_SIZE];
		CGuildMember* pMember;
		CPlayer* pSendPlayer;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pSendPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
			if( pSendPlayer )
			{
				uLoginPlayerId[nMaxLogin]	= pSendPlayer->uKey;
				uLoginGuildMulti[nMaxLogin]	= pSendPlayer->m_uIdofMulti;
				++nMaxLogin;
				if( pMember->m_idPlayer != pPlayer->uKey )
					g_DPCacheSrvr.SendGuildMemberLogin( pSendPlayer, 1, pPlayer->uKey, pPlayer->m_uIdofMulti );
			}
		}
		if( nMaxLogin > 0 )
			g_DPCacheSrvr.SendGuildMemberGameJoin( pPlayer, nMaxLogin, uLoginPlayerId, uLoginGuildMulti );
	}
}

void CGuildMng::RemoveConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CGuild* pGuild	= GetGuild( pPlayer->m_idGuild );
	if( pGuild )
	{

		CGuildMember* pMember;
		CPlayer* pSendPlayer;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			if( pMember->m_idPlayer != pPlayer->uKey )
			{
				pSendPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );

				if( pSendPlayer )
					g_DPCacheSrvr.SendGuildMemberLogin( pSendPlayer, 0, pPlayer->uKey, 100 );
			}
		}
		g_dpCoreSrvr.SendGuildMemberLogOut( pGuild->m_idGuild, pPlayer->uKey );
	}
}
#endif	// __CORESERVER

void CGuild::SetQuest( int nQuestId, int nState )
{
	PGUILDQUEST pQuest	= FindQuest( nQuestId );
	if( !pQuest )
	{
		if( m_nQuestSize >= MAX_GUILD_QUEST )
		{
			Error( "OVER MAX_GUILD_QUEST" );
			return;
		}
		for( int i = 0; i < m_nQuestSize; i++ )
		{
			if( m_aQuest[i].nId == -1 )
			{
				pQuest	= m_pQuest	= &m_aQuest[i];
				break;
			}
		}
		if( !pQuest )
			pQuest	= m_pQuest	= &m_aQuest[m_nQuestSize++];
	}

	pQuest->nState	= nState;
	pQuest->nId		= nQuestId;
#ifdef __WORLDSERVER
	CUser* pUser;
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddSetGuildQuest( nQuestId, nState );
	}
#endif	// __WORLDSERVER
}

BOOL CGuild::RemoveQuest( int nQuestId )
{
	PGUILDQUEST pQuest	= FindQuest( nQuestId );
	if( !pQuest )
		return FALSE;
	pQuest->nId	= -1;
	return TRUE;
#ifdef __WORLDSERVER
	CUser* pUser;
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddRemoveGuildQuest( nQuestId );
	}
#endif	// __WORLDSERVER
}

PGUILDQUEST CGuild::FindQuest( int nQuestId )
{
	if( m_pQuest && m_pQuest->nId == nQuestId )
		return m_pQuest;
	for( int i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[i].nId == nQuestId )
		{
			m_pQuest	= &m_aQuest[i];
			return m_pQuest;
		}
	}
	return NULL;
}

#ifdef __WORLDSERVER
void CGuild::ReplaceLodestar( const CRect &rect )
{
// locked
	CUser* pUser;
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				POINT point	= { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
				if( rect.PtInRect( point ) )
				{
					PRegionElem pRgnElem	= NULL;
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
					if( pRgnElem )
						pUser->REPLACE( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer );
				}
			}
		}
	}
}

void CGuild::Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL bMasterAround )
{
//	locked
	CUser* pUser;
	CGuildMember* pMember;
	CUser* pMaster	= NULL;
	if( bMasterAround )
	{
		pMaster	= (CUser*)prj.GetUserByID( m_idMaster );
		if( IsInvalidObj( (CObj*)pMaster ) )
			return;
	}

	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;

		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			if( pMaster && !pMaster->IsNearPC( pUser->GetId() ) )
				continue;

			if( GetQuest( QUEST_WARMON_LV1 ) != NULL )
				pUser->DoEquipItem(pUser->GetEquipItem(PARTS_RIDE));
			pUser->REPLACE( g_uIdofMulti, dwWorldId, vPos, REPLACE_NORMAL, nTempLayer );
		}
	}
}
#endif	// __WORLDSERVER

#ifndef __CORESERVER
CItemElem* CGuild::GetItem( DWORD dwItemId, SERIALNUMBER iSerialNumber )
{
	int cbMax	= m_GuildBank.m_dwItemMax;
	for( int i = 0; i < cbMax ; i++ )
	{
		CItemElem* pItemElem	= m_GuildBank.GetAtId( i );
		if( pItemElem && pItemElem->m_dwItemId == dwItemId && pItemElem->GetSerialNumber() == iSerialNumber )
			return pItemElem;
	}
	return NULL;
}

short CGuild::RemoveItem( DWORD dwId, short nItemNum )
{
	if( nItemNum < 0 )
		nItemNum	= 0;

	short nRemoved	= 0;
	CItemElem* pItemElem	= m_GuildBank.GetAtId( dwId );
	if( pItemElem )
	{
		if( pItemElem->m_nItemNum > nItemNum )
		{
			nRemoved	= nItemNum;
			pItemElem->m_nItemNum	-= nItemNum;
		}
		else
		{
			nRemoved	= pItemElem->m_nItemNum;
			m_GuildBank.RemoveAtId( dwId );
		}
	}
	return nRemoved;
}
#endif	// __CORESERVER

CGuildMng	g_GuildMng;

